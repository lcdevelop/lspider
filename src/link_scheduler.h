/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Fri 29 May 2015 11:44:41 AM CST
 * Description: 
 ************************************************************************/

#ifndef __LINK_SCHEDULER_H__
#define __LINK_SCHEDULER_H__

#include <QtCore/QThread>
#include <evhttp.h>
#include <event2/thread.h>
#include "link_table.h"

class LinkScheduler : public QThread
{
    Q_OBJECT
public:
    LinkScheduler(HttpProcessor *httpProcessor);
    virtual ~LinkScheduler();

    virtual void run();
    void stop();

    LinkTable::RetType addUrl(UrlContext *urlContext);
    bool addIpSchedule(IpContext *ipContext);

private:
    static void event_log_callback(int severity, const char *msg);
    static void event_fatal_callback(int err);

    static void on_ip_schedule(evutil_socket_t sock, short event, void *arg);
    static void on_print_state(evutil_socket_t sock, short event, void *arg);

    LinkTable _linkTable;
    HttpProcessor *_httpProcessor; // 传递指针
    struct event_base* _base; // event_base
    struct event *_printStateEvent;
    bool _isStop;
};

#endif //__LINK_SCHEDULER_H__
