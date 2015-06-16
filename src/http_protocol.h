/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Tue 19 May 2015 10:53:24 PM CST
 * Description: 
 ************************************************************************/

#ifndef __HTTP_PROTOCOL_H__
#define __HTTP_PROTOCOL_H__

class UrlContext;

class HttpProtocol
{
public:
    typedef enum {
        HA_OK,
        HA_FAIL
    } HARetType;

    static bool buildHttpRequestHeader(UrlContext *urlContext);
    static HARetType analyse(UrlContext *urlContext);
    static HARetType analyseFirstLine(UrlContext *urlContext);
    static HARetType analyseHeader(UrlContext *urlContext);
    static HARetType analyseCookie(UrlContext *urlContext);
    static HARetType analyseBody(UrlContext *urlContext);
    static HARetType analyseChunk(UrlContext *urlContext);
    const static std::string DOUBLECRLF;
    const static std::string CRLF;
};

#endif //__HTTP_PROTOCOL_H__
