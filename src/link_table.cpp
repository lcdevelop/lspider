/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Fri 29 May 2015 05:35:15 PM CST
 * Description: 
 ************************************************************************/

#include "url_tools.h"
#include "util.h"
#include "logger_container.h"
#include "url_context.h"
#include "link_scheduler.h"
#include "conf.h"
#include "link_table.h"

LinkTable::LinkTable(LinkScheduler *linkScheduler)
{
    pthread_mutex_init(&_mutex, NULL);
    _linkScheduler = linkScheduler;
    _count = 0;
}

LinkTable::~LinkTable()
{
    pthread_mutex_destroy(&_mutex);
}

LinkTable::RetType LinkTable::addUrl(UrlContext *urlContext)
{
    if (false == urlContext->parseUrl()) {
        return RT_PARSE_ERROR;
    }
    if (urlContext->site.length() == 0 || urlContext->ip.length() == 0) {
        return RT_PARSE_ERROR;
    }

    pthread_mutex_lock(&_mutex);
    IpContext *ipContext = getIpContext(&_ipTable, urlContext->ip);
    SiteContext *siteContext = getSiteContext(ipContext, urlContext->site);
    urlContext->key = urlContext->url;
    if (siteContext->urlTable.size() < Conf::instance()->schedulerMaxUrlTableSize) {
        siteContext->urlTable.push(urlContext);
        LOG_F(DEBUG, "%s", urlContext->url.c_str());
        atomic_add(&_count, 1);
        pthread_mutex_unlock(&_mutex);
        return RT_OK;
    } else {
        pthread_mutex_unlock(&_mutex);
        return RT_TABLE_FULL;
    }
}

bool LinkTable::select(IpContext *ipContext,
                       Ip &ip,
                       Site &site,
                       UrlContext* &urlContext)
{
    bool isIpContextObsolete = false;
    pthread_mutex_lock(&_mutex);
    ip = ipContext->ip;
    SiteContext *siteContext = ipContext->select();
    site = siteContext->site;
    assert(NULL != siteContext);
    urlContext = siteContext->urlTable.top();
    siteContext->urlTable.pop();
    atomic_add(&_count, -1);
    if (siteContext->urlTable.size() == 0) {
        ipContext->siteTable.erase(siteContext->site);
        delete siteContext;
    }
    if (ipContext->siteTable.size() == 0) {
        ipContext->linkTable->_ipTable.erase(ipContext->ip);
        delete ipContext;
        isIpContextObsolete = true;
    } else {
        isIpContextObsolete = false;
    }

    pthread_mutex_unlock(&_mutex);
    return isIpContextObsolete;
}

IpContext* LinkTable::getIpContext(IpTable *ipTable, Ip ip, bool allocAtFail)
{
    IpTable::iterator iter = ipTable->find(ip);
    if (ipTable->end() == iter) {
        if (allocAtFail) {
            IpContext *ipContext = new IpContext;
            ipContext->ip = ip;
            ipContext->scheduleInterval = Conf::instance()->schedulerIpScheduleInterval;
            ipContext->linkTable = this;
            ipTable->insert(pair<Ip, IpContext*>(ip, ipContext));
            _linkScheduler->addIpSchedule(ipContext);
            return ipContext;
        } else {
            return NULL;
        }
    } else {
        return iter->second;
    }
}

SiteContext* LinkTable::getSiteContext(IpContext *ipContext, Site site, bool allocAtFail)
{
    SiteTable::iterator iter = ipContext->siteTable.find(site);
    if (ipContext->siteTable.end() == iter) {
        if (allocAtFail) {
            SiteContext *siteContext = new SiteContext;
            siteContext->site = site;
            ipContext->siteTable.insert(pair<Site, SiteContext*>(site, siteContext));
            return siteContext;
        } else {
            return NULL;
        }
    } else {
        return iter->second;
    }
}

int LinkTable::count() const
{
    return _count;
}

void LinkTable::printState()
{
    LOG(DEBUG, "---------------- begin state ------------------");
    pthread_mutex_lock(&_mutex);
    IpTable::const_iterator table_iter = _ipTable.begin();
    for (; _ipTable.end() != table_iter; ++table_iter) {
        LOG_F(DEBUG, "ip:%s", table_iter->first.c_str());
        IpContext *ipContext = table_iter->second;
        SiteTable::const_iterator ip_iter = ipContext->siteTable.begin();
        for (; ipContext->siteTable.end() != ip_iter; ++ip_iter) {
            SiteContext *siteContext = ip_iter->second;
            LOG_F(DEBUG, "\tsite:%s urlcount=%lu", ip_iter->first.c_str(), siteContext->urlTable.size());
        }
    }
    pthread_mutex_unlock(&_mutex);
    LOG(DEBUG, "---------------- end state ------------------");
}

LinkScheduler* LinkTable::getLinkScheduler()
{
    return _linkScheduler;
}
