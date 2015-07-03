/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Fri 29 May 2015 11:45:03 AM CST
 * Description: 
 ************************************************************************/

#include "logger_container.h"
#include "http_processor.h"
#include "conf.h"
#include "link_scheduler.h"

LinkScheduler::LinkScheduler(HttpProcessor *httpProcessor)
    :_linkTable(this)
{
    _base = NULL;
    _isStop = false;
    _httpProcessor = httpProcessor;
}

LinkScheduler::~LinkScheduler()
{
    if (NULL != _base) {
        event_base_free(_base);
    }
    stop();
}

void LinkScheduler::event_log_callback(int severity, const char *msg)
{
    LOG_F(DEBUG, "%d %s", severity, msg);
}

void LinkScheduler::event_fatal_callback(int err)
{
    LOG_F(DEBUG, "%d", err);
}

void LinkScheduler::run()
{
    LOG(INFO, "start");
    evthread_use_pthreads();
    event_set_log_callback(LinkScheduler::event_log_callback);
    event_set_fatal_callback(LinkScheduler::event_fatal_callback);
    _base = event_base_new();

    _printStateEvent = evtimer_new(_base, on_print_state, this);
    struct timeval t = {Conf::instance()->schedulerPrintStateInterval, 0 };
    evtimer_add(_printStateEvent, &t);

    event_base_dispatch(_base);
    abort();
}

void LinkScheduler::stop()
{
    _isStop = true;
    this->wait();
}

LinkTable::RetType LinkScheduler::addUrl(UrlContext *urlContext)
{
    return _linkTable.addUrl(urlContext);
}

bool LinkScheduler::addIpSchedule(IpContext *ipContext)
{
    struct event *ipScheduleEvent = evtimer_new(_base, on_ip_schedule, ipContext);
    ipContext->ipScheduleEvent = ipScheduleEvent;
    struct timeval t = {ipContext->scheduleInterval, 0 };
    evtimer_add(ipScheduleEvent, &t);
    return true;
}

void LinkScheduler::on_ip_schedule(evutil_socket_t sock, short event, void *arg)
{
    IpContext *ipContext = (IpContext*)arg;
    HttpProcessor *httpProcessor = ipContext->linkTable->getLinkScheduler()->_httpProcessor;
    Ip ip;
    Site site;
    UrlContext *urlContext = NULL;
    bool isIpContextObsolete = ipContext->linkTable->select(ipContext, ip, site, urlContext);
    LOG_F(DEBUG, "pop ip=%s site=%s url=%s urlContext:%p",
          ip.c_str(), site.c_str(), urlContext->url.c_str(), urlContext);

    httpProcessor->pushConnectQueue(urlContext);

    // select()可能delete掉ipContext
    if (!isIpContextObsolete) {
        struct event *ipScheduleEvent = ipContext->ipScheduleEvent;
        struct timeval t = {ipContext->scheduleInterval, 0 };
        evtimer_add(ipScheduleEvent, &t);
    }
}

void LinkScheduler::on_print_state(evutil_socket_t sock, short event, void *arg)
{
    LinkScheduler *linkScheduler = (LinkScheduler*)arg;
    LOG_F(INFO, "on_print_state table count=%d", linkScheduler->_linkTable.count());
    linkScheduler->_linkTable.printState();
    struct timeval t = {Conf::instance()->schedulerPrintStateInterval, 0 };
    evtimer_add(linkScheduler->_printStateEvent, &t);
}

void LinkScheduler::control(const string& cmd)
{
}
