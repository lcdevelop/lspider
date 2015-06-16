/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Tue 19 May 2015 03:32:22 PM CST
 * Description: 
 ************************************************************************/

#include <time.h>
#include "logger_container.h"
#include "mysql_dumper.h"
#include "mongo_dumper.h"

using namespace mongo;

const std::string MongoDumper::_db = "lspider.pages";

MongoDumper::MongoDumper(MySqlDumper *mysqlDumper, std::string hostAndPort)
    :_mysqlDumper(mysqlDumper), _hostAndPort(hostAndPort), _isStop(false)
{
}

MongoDumper::~MongoDumper()
{
    stop();
}

void MongoDumper::run()
{
    LOG(INFO, "start");
    while (!_isStop) {
        // 取队列
        UrlContext *urlContext = waitDumpQueue.pop_front();
        if (dump(urlContext)) {
            _mysqlDumper->waitDumpQueue.push_back(urlContext);
        } else {
            delete urlContext;
        }
    }
}

void MongoDumper::stop()
{
    _isStop = true;
    this->terminate();
}

bool MongoDumper::initMongo()
{
    std::string errmsg;
    return _conn.connect(_hostAndPort.c_str(), errmsg);
}

bool MongoDumper::dump(UrlContext *urlContext)
{
    // 在mongodb里查的key
    mongo::BSONObjBuilder query;
    query.append("url", urlContext->url);

    mongo::BSONObjBuilder data;
    makeData(urlContext, data);

    _conn.update(_db,
                 query.obj(),
                 data.obj(),
                 true);
    if ("" == _conn.getLastError(_db)) {
        LOG_F(INFO, "%d [%s] dump mongo success [sign=%s]",
              urlContext->uuid,
              urlContext->url.c_str(),
              urlContext->sign);
        return true;
    } else {
        LOG_F(WARN, "%d [%s] dump mongo fail",
              urlContext->uuid,
              urlContext->url.c_str());
        return false;
    }
}

void MongoDumper::makeData(UrlContext *urlContext, mongo::BSONObjBuilder & data)
{
    // 在mongodb里存的数据
    data.append("url", urlContext->url);
    char linkDepthStr[8];
    snprintf(linkDepthStr, 8, "%d", urlContext->linkDepth);
    data.append("linkDepth", linkDepthStr);
    data.append("hub", urlContext->hub);

    data.append("host", urlContext->host);
    data.append("strport", urlContext->strport);
    data.append("file", urlContext->file);
    data.append("ip", urlContext->ip);
    data.append("site", urlContext->site);
    data.append("sign", urlContext->sign);

    data.append("redirectUrl", urlContext->redirectUrl);
    data.append("redirectDepth", urlContext->redirectDepth);
    char crawlElapseStr[16] = {'\0'};

    snprintf(crawlElapseStr, 16, "%ld", urlContext->crawlElapse);
    data.append("crawlElapse", crawlElapseStr);
    data.append("crawlTime", ::ctime((const time_t *)&(urlContext->finishTime.tv_sec)));
    if (urlContext->headerLen > 0) {
        data.append("header", urlContext->header);
    } else {
        data.append("header", "");
    }
    if (urlContext->bodyPos > 0) {
        data.append("body", urlContext->body);
    } else {
        data.append("body", "");
    }
    data.append("title", urlContext->title);

    BSONObjBuilder link_array;
    makeLinkArray(urlContext, link_array);
    data.appendArray("links", link_array.obj());
}

void MongoDumper::makeLinkArray(UrlContext *urlContext, mongo::BSONObjBuilder & link_array)
{
    LINK_TYPE & links = urlContext->links;
    LINK_TYPE::iterator iter = links.begin();
    int num = 0;
    for (; links.end() != iter; ++iter) {
        char numStr[16];
        sprintf(numStr, "%d", num++);
        mongo::BSONObjBuilder link;
        link.append("href", iter->first);
        link.append("anchor", iter->second);
        link_array.append(numStr, link.obj());
    }
}

MySqlDumper * MongoDumper::mysqlDumper()
{
    return _mysqlDumper;
}
