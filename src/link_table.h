/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Fri 29 May 2015 05:33:10 PM CST
 * Description: 
 ************************************************************************/

#ifndef __LINK_TABLE_H__
#define __LINK_TABLE_H__

#include <string>
#include <map>
#include <queue>
#include <pthread.h>
#include "keyed_queue.h"
#include "url_context.h"

// 每秒选一个，10年的选取总量作为最大值
#define MAX_SCALE (86400*365*10*1.0)
#define DEFAULT_SCHEDULE_INTERVAL 1

using std::string;
using std::map;
using std::priority_queue;

class LinkScheduler;
class IpContext;
class LinkTable;

typedef priority_queue<UrlContext*, vector<UrlContext*>, UrlContextComp> UrlTable;
struct SiteContext {
    SiteContext() {
        concurrenceCount = 0;
        quota = 1;
        selectCountDivQuota = 0.0;
    }
    Site site;
    UrlTable urlTable;
    volatile int concurrenceCount;
    int quota;
    float selectCountDivQuota;
};

typedef map<Site, SiteContext*> SiteTable;
struct IpContext {
    IpContext() {
        concurrenceCount = 0;
        scheduleInterval = DEFAULT_SCHEDULE_INTERVAL;
        ipScheduleEvent = NULL;
    }

    SiteContext *select() {
        float minScale = MAX_SCALE;
        SiteContext *selectedContext = NULL;
        SiteTable::iterator iter = siteTable.begin();
        // 找到最小的selectCountDivQuota
        for (; siteTable.end() != iter; ++iter) {
            SiteContext *siteContext = iter->second;
            if (siteContext->selectCountDivQuota < minScale) {
                minScale = siteContext->selectCountDivQuota;
                selectedContext = siteContext;
            }
        }
        selectedContext->selectCountDivQuota += 1.0 / selectedContext->quota;
        return selectedContext;
    }

    Ip ip;
    SiteTable siteTable;
    volatile int concurrenceCount;
    int scheduleInterval;
    struct event *ipScheduleEvent;
    LinkTable *linkTable;
};

typedef map<Ip, IpContext*> IpTable;

class LinkTable
{
public:
    typedef enum {
        RT_PARSE_ERROR = -2,
        RT_TABLE_FULL = -1,
        RT_OK = 0,
    } RetType;

    LinkTable(LinkScheduler *linkScheduler);
    virtual ~LinkTable();

    RetType addUrl(UrlContext *urlContext);
    bool select(IpContext *ipContext,
                           Ip &ip,
                           Site &site,
                           UrlContext* &urlContext);
    int count() const;
    void printState();

    LinkScheduler* getLinkScheduler();

private:
    IpContext* getIpContext(IpTable *ipTable, Ip ip, bool allocAtFail = true);
    SiteContext* getSiteContext(IpContext *ipContext, Site site, bool allocAtFail = true);

    IpTable _ipTable;
    LinkScheduler *_linkScheduler;
    pthread_mutex_t _mutex;
    volatile int _count;
};

#endif //__LINK_TABLE_H__
