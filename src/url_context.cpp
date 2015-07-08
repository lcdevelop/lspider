/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Sat 30 May 2015 10:41:48 AM CST
 * Description: 
 ************************************************************************/

#include "util.h"
#include "logger_container.h"
#include "url_context.h"

UrlContext::UrlContext()
{
    uuid = atomic_add(&g_uuid, 1);
    retry = 0;
    redirectDepth = 0;
    hub = false;

    httpProcessor = NULL;
    event = NULL;
    base = NULL;

    sign[0] = '\0';
    linkDepth = 0;

    init();
    LOG_F(DEBUG, "%d [%s] construct", uuid, url.c_str());
}

UrlContext::~UrlContext()
{
    LOG_F(DEBUG, "%d [%s] deconstruct", uuid, url.c_str());
    if (DELETED == status) {
        string backtrace = get_backtrace();
        LOG_F(FATAL, "%d [%s] double free %s", uuid, url.c_str(), backtrace.c_str());
    }
    status = DELETED;
}

void UrlContext::init()
{
    host[0] = '\0';
    strport[0] = '\0';
    file[0] = '\0';
    ip = "";
    hasParsed = false;

    status = CONNECTING;

    requestLen = 0;
    writeLen = 0;
    recvData = "";

    firstLineLen = -1;
    headerLen = -1;
    uncompressedBodyLen = -1;
    bodyPos = -1;
    fields.clear();
    finishAnalyseFirstLine = false;
    finishAnalyseHeader = false;
    finishAnalyseBody = false;

    crawlElapse = -1;

    DLINK_INITIALIZE(&link);
}

bool UrlContext::parseUrl()
{
    // 不重复解析, init()之后会复位
    if (hasParsed) {
        return true;
    }

    // 第一次执行走这个分支，第二次执行重定向走下一个分支
    if (redirectDepth == 0) {
        if (1 == parse_url(url.c_str(),
                           host,
                           MAX_SITE_LEN,
                           strport,
                           MAX_PORT_LEN,
                           file,
                           MAX_PATH_LEN)) {
            // 这里简单起见，site先赋值为host
            site = host;
            // 可能上游已经指定了ip
            if (0 == ip.length()) {
                vector<string> ips = getip(host);
                size_t ipsSize = ips.size();
                if (ipsSize > 0) {
                    ip = ips[rand() % ipsSize];
                }
                if (0 == ip.length()) {
                    LOG_F(WARN, "%d [%s] parse_url getip fail", uuid, url.c_str());
                    return false;
                }
            }
            hasParsed = true;
            return true;
        } else {
            LOG_F(WARN, "%d [%s] parse_url fail", uuid, url.c_str());
            return false;
        }
    } else {
        // 如果是重定向，则可以确保在linktable调用过parseUrl,site一定有值
        assert(site.length() != 0);
        if (1 == parse_url(redirectUrl.c_str(),
                           host,
                           MAX_SITE_LEN,
                           strport,
                           MAX_PORT_LEN,
                           file,
                           MAX_PATH_LEN)) {
            vector<string> ips = getip(host);
            size_t ipsSize = ips.size();
            if (ipsSize > 0) {
                ip = ips[rand() % ipsSize];
            }
            if (0 == ip.length()) {
                LOG_F(WARN, "%d [%s] parse_url redirectUrl:%s getip fail", uuid, url.c_str(), redirectUrl.c_str());
                return false;
            }
            hasParsed = true;
            return true;
        } else {
            LOG_F(WARN, "%d [%s] parse_url redirectUrl:%s fail", uuid, url.c_str(), redirectUrl.c_str());
            return false;
        }
    }
}
