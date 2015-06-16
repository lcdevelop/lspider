/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:59:05 2015
 * Description: 
 ************************************************************************/

#include <unistd.h>
#include "logger_container.h"
#include "http_processor.h"
#include "conf.h"
#include "http_event_engine.h"

HttpEventEngine::HttpEventEngine(HttpProcessor *httpProcessor)
{
    base = NULL;
    _isStop = false;
    _httpProcessor = httpProcessor;
}

HttpEventEngine::~HttpEventEngine()
{
    if (NULL != base) {
        event_base_free(base);
    }
    stop();
}

void HttpEventEngine::event_log_callback(int severity, const char *msg)
{
    LOG_F(DEBUG, "%d %s", severity, msg);
}

void HttpEventEngine::event_fatal_callback(int err)
{
    LOG_F(DEBUG, "%d", err);
}

/**
 * 线程执行函数
 */
void HttpEventEngine::run()
{
    LOG(INFO, "start");
    evthread_use_pthreads();
    event_set_log_callback(HttpEventEngine::event_log_callback);
    event_set_fatal_callback(HttpEventEngine::event_fatal_callback);
    base = event_base_new();

    _printStateEvent = evtimer_new(base, on_print_state, this);
    struct timeval t = {.tv_sec = Conf::instance()->httpEventPrintStateInterval, .tv_usec = 0 };
    evtimer_add(_printStateEvent, &t);

    event_base_dispatch(base);
    abort();
}

void HttpEventEngine::stop()
{
    _isStop = true;
    this->wait();
}

void HttpEventEngine::on_print_state(evutil_socket_t sock, short event, void *arg)
{
    HttpEventEngine *httpEventEngine = (HttpEventEngine*)arg;
    LOG(DEBUG, "on_print_state");
    httpEventEngine->_httpProcessor->printState();
    struct timeval t = {.tv_sec = Conf::instance()->httpEventPrintStateInterval, .tv_usec = 0 };
    evtimer_add(httpEventEngine->_printStateEvent, &t);
}
