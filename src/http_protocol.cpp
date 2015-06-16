/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Tue 19 May 2015 10:53:29 PM CST
 * Description: 
 ************************************************************************/

#include <sstream>
#include <zlib.h>
#include "logger_container.h"
#include "http_protocol.h"
#include "url_context.h"
#include "http_processor.h"
#include "conf.h"
#include "util.h"

const std::string HttpProtocol::DOUBLECRLF = "\r\n\r\n";
const std::string HttpProtocol::CRLF = "\r\n";

bool HttpProtocol::buildHttpRequestHeader(UrlContext *urlContext)
{
    if ('\0' == urlContext->host[0] || '\0' == urlContext->strport[0]) {
        LOG(WARN, "have not parse url");
        return false;
    }

    urlContext->requestLen += snprintf(urlContext->requestData, MAX_HEAD_CMD_LEN, 
                                       "GET %s HTTP/1.1\r\n", urlContext->file);
    urlContext->requestLen += snprintf(urlContext->requestData+urlContext->requestLen, MAX_HEAD_CMD_LEN-urlContext->requestLen, 
                                       "Host: %s\r\n", urlContext->host);
    urlContext->requestLen += snprintf(urlContext->requestData+urlContext->requestLen, MAX_HEAD_CMD_LEN-urlContext->requestLen, 
                                       "Connection: close\r\n");
    urlContext->requestLen += snprintf(urlContext->requestData+urlContext->requestLen, MAX_HEAD_CMD_LEN-urlContext->requestLen, 
                                       "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n");
    urlContext->requestLen += snprintf(urlContext->requestData+urlContext->requestLen, MAX_HEAD_CMD_LEN-urlContext->requestLen, 
                                       "Accept-Encoding: gzip, deflate, sdch\r\n");
    urlContext->requestLen += snprintf(urlContext->requestData+urlContext->requestLen, MAX_HEAD_CMD_LEN-urlContext->requestLen, 
                                       "Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\n");
    urlContext->requestLen += snprintf(urlContext->requestData+urlContext->requestLen, MAX_HEAD_CMD_LEN-urlContext->requestLen, 
                                       "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.135 Safari/537.36\r\n");
    if (urlContext->cookie.length() > 0) {
        urlContext->requestLen += snprintf(urlContext->requestData+urlContext->requestLen, MAX_HEAD_CMD_LEN-urlContext->requestLen, 
                                           "Cookie: %s\r\n", urlContext->cookie.c_str());
    }
    urlContext->requestLen += snprintf(urlContext->requestData+urlContext->requestLen, MAX_HEAD_CMD_LEN-urlContext->requestLen, "\r\n");

    LOG_F(TRACE, "%d [%s] requestData:[%s]",
          urlContext->uuid,
          urlContext->url.c_str(),
          urlContext->requestData);

    return true;
}

HttpProtocol::HARetType HttpProtocol::analyse(UrlContext *urlContext)
{
    if (false == urlContext->finishAnalyseFirstLine) {
        if (HA_FAIL == analyseFirstLine(urlContext)) {
            return HA_FAIL;
        }
    }

    if (false == urlContext->finishAnalyseHeader) {
        if (HA_FAIL == analyseHeader(urlContext)) {
            return HA_FAIL;
        }
    }

    if (true == urlContext->finishAnalyseFirstLine
        && true == urlContext->finishAnalyseHeader) {
        if (HA_FAIL == analyseBody(urlContext)) {
            return HA_FAIL;
        }
    }

    if (true == urlContext->finishAnalyseFirstLine
        && true == urlContext->finishAnalyseHeader
        && true == urlContext->finishAnalyseBody) {
        return HA_OK;
    } else {
        return HA_FAIL;
    }
}

HttpProtocol::HARetType HttpProtocol::analyseFirstLine(UrlContext *urlContext)
{
    std::string &recvData = urlContext->recvData;
    size_t firstLineTailPos = recvData.find(CRLF);
    if (std::string::npos != firstLineTailPos) {
        std::stringstream ss;
        std::string httpStatusStr;
        ss << recvData.substr(0, firstLineTailPos);
        ss  >> urlContext->httpVersion
            >> httpStatusStr;
        urlContext->httpStatus = atoi(httpStatusStr.c_str());
        urlContext->httpReason = "";
        std::string reason;
        while (ss >> reason) {
            urlContext->httpReason += reason;
            urlContext->httpReason += " ";
        }
        rtrim(urlContext->httpReason);

        LOG_F(DEBUG, "%d [%s] firstLineLen=%lu version:[%s] status:[%d] reason:[%s]",
              urlContext->uuid,
              urlContext->url.c_str(),
              firstLineTailPos,
              urlContext->httpVersion.c_str(),
              urlContext->httpStatus,
              urlContext->httpReason.c_str());
        urlContext->firstLineLen = firstLineTailPos + CRLF.length();
        urlContext->finishAnalyseFirstLine = true;
        return HA_OK;
    }
    return HA_FAIL;
}

HttpProtocol::HARetType HttpProtocol::analyseHeader(UrlContext *urlContext)
{
    std::string &recvData = urlContext->recvData;
    size_t headTailPos = recvData.find(DOUBLECRLF, urlContext->firstLineLen);
    if (std::string::npos != headTailPos) {
        std::string headStr = recvData.substr(urlContext->firstLineLen,
                                              headTailPos - urlContext->firstLineLen + DOUBLECRLF.length());
        std::stringstream ss;
        ss << headStr;
        std::string line;
        while (std::getline(ss, line)) {
            size_t colonPos = line.find(':');
            if (std::string::npos != colonPos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos+1);
                rtrim(ltrim(key));
                rtrim(ltrim(value));
                transform(key.begin(), key.end(), key.begin(), ::tolower);
                urlContext->fields[key] = value;
                LOG_F(DEBUG, "%d [%s] key:[%s] value:[%s]",
                      urlContext->uuid,
                      urlContext->url.c_str(),
                      key.c_str(),
                      value.c_str());
            }
        }
        urlContext->headerLen = headTailPos;
        urlContext->header = urlContext->recvData.substr(0, urlContext->headerLen);
        analyseCookie(urlContext);
        urlContext->finishAnalyseHeader = true;
        return HA_OK;
    }
    return HA_FAIL;
}

HttpProtocol::HARetType HttpProtocol::analyseCookie(UrlContext *urlContext)
{
    if (urlContext->fields.find("set-cookie") != urlContext->fields.end()) {
        urlContext->cookie = urlContext->fields["set-cookie"];
    }
    return HA_OK;
}

HttpProtocol::HARetType HttpProtocol::analyseBody(UrlContext *urlContext)
{
    urlContext->bodyPos = urlContext->headerLen + DOUBLECRLF.length();
    int bodyLen = urlContext->recvData.length() - urlContext->bodyPos;

    if (200 != urlContext->httpStatus) {
        urlContext->body = "";
        urlContext->finishAnalyseBody = true;
        return HA_OK;
    }

    urlContext->body = urlContext->recvData.substr(urlContext->bodyPos, bodyLen);

    if (HA_FAIL == analyseChunk(urlContext)) {
        return HA_FAIL;
    }

    if ("gzip" == urlContext->fields["content-encoding"]) {
        uLong bufferLen = Conf::instance()->httpMaxPageSize;
        char *buf = (char*)malloc(bufferLen);
        if (NULL == buf) {
            LOG_F(WARN, "%d [%s] alloc buffer size=%lu fail",
                  urlContext->uuid,
                  urlContext->url.c_str(),
                  bufferLen);
            return HA_FAIL;
        } else {
            int ret = gzdecompress((Byte*)urlContext->body.c_str(),
                                   (uLong)urlContext->body.length(),
                                   (Byte*)buf, &bufferLen);

            if (0 != ret) {
                LOG_F(WARN, "%d [%s] uncompress fail ret=%d bufferLen=%lu bodyLen=%lu body=[%s]",
                      urlContext->uuid,
                      urlContext->url.c_str(),
                      ret,
                      bufferLen,
                      urlContext->body.length(),
                      urlContext->body.c_str());
                free(buf);
                return HA_FAIL;
            } else {
                LOG_F(DEBUG, "%d [%s] uncompress success sourcelen=%lu uncompressedlen=%lu",
                      urlContext->uuid,
                      urlContext->url.c_str(),
                      urlContext->body.length(),
                      bufferLen);
                urlContext->body = std::string(buf, bufferLen);
                free(buf);
            }
        }
    }

    urlContext->finishAnalyseBody = true;
    return HA_OK;
}

HttpProtocol::HARetType HttpProtocol::analyseChunk(UrlContext *urlContext)
{
    if ("chunked" == urlContext->fields["transfer-encoding"]) {
        std::string joinedBody;
        size_t curPos = 0;
        // 依次处理各个chunk
        while (true) {
            // 找chunksize行
            size_t lineTailPos = urlContext->body.find(CRLF, curPos);
            if (std::string::npos != lineTailPos) {
                std::string chunkSizeStr = urlContext->body.substr(curPos, lineTailPos - curPos);

                // 16进制转10进制
                char decStr[32] = {'\0'};
                hex2dec(chunkSizeStr.c_str(), decStr);

                size_t chunkSize = atoi(decStr);
                // 最后一个chunksize是0，结束
                if (0 == chunkSize) {
                    LOG_F(DEBUG, "%d [%s] bodysize=%lu joinedBodysize=%lu",
                          urlContext->uuid,
                          urlContext->url.c_str(),
                          urlContext->body.length(),
                          joinedBody.length());
                    urlContext->body = joinedBody;
                    return HA_OK;
                } else {
                    joinedBody += urlContext->body.substr(lineTailPos + CRLF.length(), chunkSize);
                    curPos = lineTailPos + CRLF.length() + chunkSize + 1;
                }
            } else {
                // 不以回车结尾格式不对
                LOG_F(WARN, "%d [%s] format error",
                      urlContext->uuid,
                      urlContext->url.c_str());
                return HA_FAIL;
            }
        }
    } else {
        return HA_OK;
    }
}
