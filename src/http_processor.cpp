/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:55:54 2015
 * Description: 
 ************************************************************************/

#include <netinet/in.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "extractor.h"
#include "http_event_engine.h"
#include "mongo_dumper.h"
#include "mysql_dumper.h"
#include "url_tools.h"
#include "util.h"
#include "atomic.h"
#include "url_context.h"
#include "logger_container.h"
#include "http_protocol.h"
#include "conf.h"
#include "http_processor.h"


HttpProcessor::HttpProcessor(MongoDumper* mongoDumper, Extractor *extractor, string connectionName)
    :MySqlBase(connectionName), _tryProcCount(0), _procFinishCount(0)
{
    _concCount = 0;
    _isStop = false;
    _engine = new HttpEventEngine(this);
    _engine->start();
    _mongoDumper = mongoDumper;
    _extractor = extractor;
}

HttpProcessor::~HttpProcessor()
{
    // 这里不应该这样清理
    stop();
    delete _engine;
}

void HttpProcessor::pushConnectQueue(UrlContext *urlContext)
{
    while (waitConnectQueue.size() > Conf::instance()->httpConnectQueueSize) {
        usleep(1000);
    }
    LOG_F(DEBUG, "%d [%s]", urlContext->uuid, urlContext->url.c_str());
    waitConnectQueue.push_back(urlContext);
}

void HttpProcessor::run()
{
    LOG(INFO, "start");
    if (!initMySql()) {
        LOG(FATAL, "HttpProcessor initMySql fail");
        _exit(-1);
        return;
    }

    while (!_isStop) {
        // 取队列，发起连接
        UrlContext *urlContext = waitConnectQueue.pop_front();
        atomic_add(&_tryProcCount, 1);
        while (_concCount > Conf::instance()->httpMaxConcurrence) {
            usleep(5000);
        }
        if (connect(urlContext) < 0) {
            LOG_F(WARN, "%d [%s] connect fail", urlContext->uuid, urlContext->url.c_str());
            delete urlContext;
        }
    }
}

void HttpProcessor::stop()
{
    _isStop = true;
    this->terminate();
    _engine->stop();
}

int HttpProcessor::connect(UrlContext *urlContext)
{
    LOG_F(DEBUG, "%d [%s] begin connect", urlContext->uuid, urlContext->url.c_str());
    gettimeofday(&urlContext->beginConnectTime, NULL);
    struct sockaddr_in serv_addr;
    if (false == getServAddr(urlContext, serv_addr)) {
        return -1;
    }

    urlContext->sock = createNonblockingSocket();
    if (urlContext->sock < 0) {
        return -1;
    }

    if( ! HttpProtocol::buildHttpRequestHeader(urlContext)) {
        return -1;
    }

    LOG_F(DEBUG, "%d [%s] connect to %s %s sock=%d",
          urlContext->uuid, urlContext->url.c_str(), urlContext->ip.c_str(), urlContext->host,
          urlContext->sock);

    // 创建事件
    urlContext->httpProcessor = this;
    urlContext->base = _engine->base;
    urlContext->event = event_new(_engine->base, urlContext->sock, EV_WRITE | EV_ET | EV_TIMEOUT, on_connected, (void*)urlContext);

    // 非阻塞连接
    if (::connect(urlContext->sock, (const struct sockaddr *) & serv_addr, (socklen_t)sizeof (sockaddr)) < 0) {
        if(errno != EINPROGRESS) {
            LOG_F(WARN, "%d [%s] connect error %s sock=%d",
                  urlContext->uuid, urlContext->url.c_str(), strerror(errno), urlContext->sock);
            return -1;
        }
    }

    // 事件注册到IO事件模型
    struct timeval t = {Conf::instance()->httpConnectTimeout, 0 };
    event_add(urlContext->event, &t);

    return 0;
}

void HttpProcessor::on_connected(int sock, short event, void* arg)
{
    UrlContext *urlContext = (UrlContext *)arg;
    LOG_F(DEBUG, "%d [%s]", urlContext->uuid, urlContext->url.c_str());

    if (EV_TIMEOUT & event) {
        urlContext->httpProcessor->doWithSockTimeout(urlContext);
    } else if (EV_WRITE & event) {
        gettimeofday(&urlContext->beginSendTime, NULL);
        assert(sock == urlContext->sock);

        // 更新状态
        if (urlContext->status != UrlContext::CONNECTING) {
            LOG_F(FATAL, "%d [%s] status error sock=%d", urlContext->uuid, urlContext->url.c_str(), urlContext->sock);
            return;
        }
        urlContext->status = UrlContext::SENDING;

        // 连接成功马上就尝试发请求
        urlContext->httpProcessor->doWithSend(urlContext);
    } else {
        abort();
    }
}

void HttpProcessor::on_send(int sock, short event, void* arg)
{
    UrlContext *urlContext = (UrlContext *)arg;
    if (EV_TIMEOUT & event) {
        urlContext->httpProcessor->doWithSockTimeout(urlContext);
    } else if (EV_WRITE & event) {
        assert(sock == urlContext->sock);
        urlContext->httpProcessor->doWithSend(urlContext);
    } else {
        abort();
    }
}

void HttpProcessor::on_recv(int sock, short event, void* arg)
{
    UrlContext *urlContext = (UrlContext *)arg;
    if (EV_TIMEOUT & event) {
        urlContext->httpProcessor->doWithSockTimeout(urlContext);
    } else if (EV_READ & event) {
        assert(sock == urlContext->sock);
        urlContext->httpProcessor->doWithRecv(urlContext);
    } else {
        abort();
    }
}

void HttpProcessor::doWithSend(UrlContext *urlContext)
{
    SndRcvRetType ret = urlContext->httpProcessor->trySend(urlContext);
    if (E_AGAIN == ret) {
        event_del(urlContext->event);
        event_free(urlContext->event);
        urlContext->event = event_new(urlContext->base, urlContext->sock, EV_WRITE | EV_ET | EV_TIMEOUT, on_send, (void*)urlContext);
        struct timeval t = {Conf::instance()->httpSendTimeout, 0 };
        event_add(urlContext->event, &t);
    } else if (E_FINISH == ret) {
        gettimeofday(&urlContext->beginRecvTime, NULL);
        LOG_F(DEBUG, "%d [%s] send finish wlen=%d", urlContext->uuid, urlContext->url.c_str(), urlContext->writeLen);
        // 发送完成马上尝试接收数据
        this->doWithRecv(urlContext);
    } else if (E_ERR == ret) {
        LOG_F(WARN, "%d [%s] send error sock=%d", urlContext->uuid, urlContext->url.c_str(), urlContext->sock);
        event_del(urlContext->event);
        event_free(urlContext->event);
        close(urlContext->sock);
        gettimeofday(&urlContext->finishTime, NULL);
        urlContext->httpProcessor->onBroken(urlContext);
    } else {
        abort();
    }
}

void HttpProcessor::doWithRecv(UrlContext *urlContext)
{
    SndRcvRetType ret = urlContext->httpProcessor->tryRecv(urlContext);
    if (E_AGAIN == ret) {
        struct timeval t = {Conf::instance()->httpRecvTimeout, 0 };
        event_del(urlContext->event);
        event_free(urlContext->event);
        urlContext->event = event_new(urlContext->base, urlContext->sock, EV_READ | EV_ET | EV_TIMEOUT, on_recv, (void*)urlContext);
        event_add(urlContext->event, &t);
    } else if (E_FINISH == ret) {
        LOG_F(DEBUG, "%d [%s] recv finish rlen=%lu", urlContext->uuid, urlContext->url.c_str(), urlContext->recvData.length());
        event_del(urlContext->event);
        event_free(urlContext->event);
        close(urlContext->sock);
        gettimeofday(&urlContext->finishTime, NULL);
        urlContext->httpProcessor->onRecvOnePage(urlContext);
    } else if (E_ERR == ret) {
        LOG_F(WARN, "%d [%s] recv error", urlContext->uuid, urlContext->url.c_str());
        event_del(urlContext->event);
        event_free(urlContext->event);
        close(urlContext->sock);
        gettimeofday(&urlContext->finishTime, NULL);
        urlContext->httpProcessor->onBroken(urlContext);
    } else {
        abort();
    }
}

void HttpProcessor::doWithSockTimeout(UrlContext *urlContext)
{
    LOG_F(DEBUG, "%d [%s]", urlContext->uuid, urlContext->url.c_str());
    event_del(urlContext->event);
    event_free(urlContext->event);
    close(urlContext->sock);
    gettimeofday(&urlContext->finishTime, NULL);
    atomic_add(&_concCount, -1);
    urlContext->httpProcessor->onTimeout(urlContext);
}

HttpProcessor::SndRcvRetType HttpProcessor::trySend(UrlContext *urlContext)
{
    int totalWriteLen = 0;
    while (true) {
        int writeLen = send(urlContext->sock,
                            urlContext->requestData + urlContext->writeLen,
                            urlContext->requestLen - urlContext->writeLen,
                            0);
        if (writeLen >= 0) {
            urlContext->writeLen += writeLen;
            totalWriteLen += writeLen;
            if (urlContext->writeLen == urlContext->requestLen) {
                return E_FINISH;
            }
            assert(urlContext->writeLen < urlContext->requestLen);
        } else {
            if (EAGAIN == errno) {
                return E_AGAIN;
            } else {
                return E_ERR;
            }
        }
    }
}

HttpProcessor::SndRcvRetType HttpProcessor::tryRecv(UrlContext *urlContext)
{
    const int MAX_RECV_LEN = 1024;
    char buf[MAX_RECV_LEN];
    int totalReadLen = 0;
    while (true) {
        int rlen = recv(urlContext->sock, buf, MAX_RECV_LEN, 0);
        if (rlen > 0) {
            totalReadLen += rlen;
            urlContext->recvData.append(buf, rlen);
            if (rlen > Conf::instance()->httpMaxPageSize) {
                LOG_F(WARN, "%d [%s] page size chunked to %lu", urlContext->uuid, urlContext->url.c_str(), urlContext->recvData.length());
                return E_FINISH;
            }
        } else if (rlen == 0) {
            return E_FINISH;
        } else {
            if (EAGAIN == errno) {
                return E_AGAIN;
            } else {
                LOG_F(WARN, "%d [%s] recv error %s", urlContext->uuid, urlContext->url.c_str(), strerror(errno));
                return E_ERR;
            }
        }
    }
}

void HttpProcessor::doWithCrawlDone(UrlContext *urlContext)
{
    LOG_F(DEBUG, "%d [%s] call doWithCrawlDone", urlContext->uuid, urlContext->url.c_str());
    bool canDump = false;
    urlContext->crawlElapse = TIME_DIFF(urlContext->beginConnectTime, urlContext->finishTime);
    std::string statusStr = "";
    switch (urlContext->status) {
    case UrlContext::DONE:
        statusStr = "DONE";
        canDump = true;
        break;
    case UrlContext::TIMEOUT:
        statusStr = "TIMEOUT";
        break;
    case UrlContext::PROTERR:
        statusStr = "PROTERR";
        break;
    default:
        statusStr = "UNKNOWN";
        break;
    }

    LOG_F(INFO, "%d CRAWLLOG [sign=%s] [url=%s] [status=%s] [linkdepth=%d] [hub=%d] [httpretry=%d] [len=%lu] [elapse=%ld]",
          urlContext->uuid,
          urlContext->sign,
          urlContext->url.c_str(),
          statusStr.c_str(),
          urlContext->linkDepth,
          urlContext->hub,
          urlContext->retry,
          urlContext->recvData.length(),
          urlContext->crawlElapse);
    if (canDump) {
        _extractor->push(urlContext);
    } else {
        // crawlretry++
        QSqlQuery updateQuery(_db);
        updateQuery.prepare("UPDATE " + QString(TABLE_NAME.c_str())
                            + " SET crawlretry=crawlretry+1,crawlstate=" + QString::number(MySqlBase::NEW)
                            + " WHERE sign="
                            + QString(urlContext->sign));
        if (false == updateQuery.exec()) {
            QSqlError err = updateQuery.lastError();
            LOG_F(WARN, "QSqlQuery fail errtype=%d cmd=%s text=%s",
                  err.type(), updateQuery.lastQuery().toStdString().c_str(), err.text().toStdString().c_str());
        }
        delete urlContext;
    }
    atomic_add(&_concCount, -1);
    atomic_add(&_procFinishCount, 1);
}

bool HttpProcessor::doWithRedirect(UrlContext *urlContext)
{
    // 如果协议跳转，则发起新一轮抓取
    std::string redirectUrl = urlContext->fields["location"];
    // location有时候只有path部分
    //if (redirectUrl.length() > 0 && redirectUrl[0] == '/') {
    if (redirectUrl.length() > 0 && std::string::npos == redirectUrl.find("://")) {
        redirectUrl = "http://" + string(urlContext->host) + "/" + redirectUrl;
    }
    urlContext->redirectUrl = redirectUrl;
    LOG_F(INFO, "%d [%s] redirect [%s]",
          urlContext->uuid,
          urlContext->url.c_str(),
          urlContext->redirectUrl.c_str());
    // 如果超过跳转次数则认为异常
    if (urlContext->redirectUrl.length() > 0 && urlContext->redirectDepth < Conf::instance()->httpMaxRedirectDepth) {
        urlContext->redirectDepth++;
        urlContext->init();
        urlContext->httpProcessor->waitConnectQueue.push_back(urlContext);
        atomic_add(&_concCount, -1);
        return true;
    } else {
        return false;
    }
}

void HttpProcessor::onRecvOnePage(UrlContext *urlContext)
{
    HttpProtocol::HARetType ret = HttpProtocol::analyse(urlContext);
    if (HttpProtocol::HA_OK == ret) {
        switch (urlContext->httpStatus) {
        case 200:
            urlContext->status = UrlContext::DONE;
            doWithCrawlDone(urlContext);
            break;

        case 301:
        case 302:
            if (false == doWithRedirect(urlContext)) {
                urlContext->status = UrlContext::PROTERR;
                doWithCrawlDone(urlContext);
            }
            break;

        case 404:
            urlContext->status = UrlContext::PROTERR;
            doWithCrawlDone(urlContext);
            break;

        default:
            urlContext->status = UrlContext::DONE;
            doWithCrawlDone(urlContext);
            break;
        }
    } else {
        // 协议错误
        urlContext->status = UrlContext::PROTERR;
        doWithCrawlDone(urlContext);
    }
}

void HttpProcessor::onTimeout(UrlContext *urlContext)
{
    if (++urlContext->retry < Conf::instance()->httpMaxRetry) {
        LOG_F(DEBUG, "%d [%s] ready to retry", urlContext->uuid, urlContext->url.c_str());
        urlContext->init();
        urlContext->httpProcessor->waitConnectQueue.push_back(urlContext);
    } else {
        urlContext->status = UrlContext::TIMEOUT;
        urlContext->recvData = "";
        doWithCrawlDone(urlContext);
    }
}

void HttpProcessor::onBroken(UrlContext *urlContext)
{
    LOG_F(WARN, "%d [%s]", urlContext->uuid, urlContext->url.c_str());
    atomic_add(&_concCount, -1);
    onTimeout(urlContext);
}

int HttpProcessor::createNonblockingSocket()
{
    // 创建socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        LOG(WARN, "socket fail");
        return -1;
    }
    if (0 != setnonblocking(sockfd)) {
        LOG(WARN, "setnonblocking fail");
        close(sockfd);
        return -1;
    }

    atomic_add(&_concCount, 1);

    return sockfd;
}

bool HttpProcessor::getServAddr(UrlContext *urlContext, struct sockaddr_in &serv_addr)
{
    if (0 == urlContext->url.find("https") || 0 == urlContext->redirectUrl.find("https")) {
        LOG_F(WARN, "%d [%s] https not support", urlContext->uuid, urlContext->url.c_str());
        return false;
    }

    // 解析url
    if (false == urlContext->parseUrl()) {
        return false;
    }

    // 赋值端口
    if (urlContext->strport[0] == '\0') {
        serv_addr.sin_port = htons (80);
        strncpy(urlContext->strport, "80", MAX_PORT_LEN);
    } else {
        serv_addr.sin_port = htons(atoi(urlContext->strport));
    }

    // 赋值ip
    unsigned int ip;
    if (ip_aton (urlContext->ip.c_str(), &ip) < 0) {
        LOG_F(WARN, "%d [%s] ip_aton fail ip=%s", urlContext->uuid, urlContext->url.c_str(), urlContext->ip.c_str());
        return false;
    }
    serv_addr.sin_addr.s_addr = ip;

    bzero (&(serv_addr.sin_zero), sizeof (serv_addr.sin_zero));
    serv_addr.sin_family = AF_INET;

    return true;
}

void HttpProcessor::printState()
{
    LOG_F(INFO, "concurrence count=%d waitConnectQueueSize=%d "
          "waitExtractQueueSize=%d mongoWaitDumpQueueSize=%d "
          "mysqlWaitDumpQueueSize=%d",
          _concCount, waitConnectQueue.size(), _extractor->queueSize(),
          _mongoDumper->waitDumpQueue.size(), _mongoDumper->mysqlDumper()->waitDumpQueue.size());
}

void HttpProcessor::control(string& response, const string& cmd)
{
    char msg[1024];
    snprintf(msg, 1024, "concurrence:%d waitConnectQueueSize:%d tryProcCount:%d procFinishCount:%d",
             _concCount,
             waitConnectQueue.size(),
             _tryProcCount,
             _procFinishCount);
    response = string(msg);
}
