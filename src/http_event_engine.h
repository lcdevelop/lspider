/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:58:37 2015
 * Description: 
 ************************************************************************/

#ifndef __HTTP_EVENT_ENGINE_H__
#define __HTTP_EVENT_ENGINE_H__

#include <evhttp.h>
#include <event2/thread.h>
#include "lthread.h"

class HttpProcessor;

/**
 * 时间引擎，维护http请求的event_base
 */
class HttpEventEngine : public lthread::LThread
{
public:
    HttpEventEngine(HttpProcessor *httpProcessor);
    ~HttpEventEngine();

    /**
     * 线程执行函数
     */
    virtual void run();

    void stop();

    static void on_print_state(evutil_socket_t sock, short event, void *arg);

    struct event_base* base; // event_base

private:
    static void event_log_callback(int severity, const char *msg);
    static void event_fatal_callback(int err);

    bool _isStop;
    struct event *_printStateEvent;
    HttpProcessor *_httpProcessor;
};


#endif //__HTTP_EVENT_ENGINE_H__
