/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Wed 03 Jun 2015 02:57:09 PM CST
 * Description: 
 ************************************************************************/

#ifndef __CONF_H__
#define __CONF_H__

#include <string>
#include <vector>
#include "singleton.h"

using std::string;
using std::vector;

class Conf
{
    DECLARE_SINGLETON(Conf);
public:
    bool load(const char *confFile);
    bool loadMaindomainList(const string maindomainListFile);

    string mysqlHost;
    int mysqlPort;
    string mysqlUser;
    string mysqlPassword;
    string mysqlDatabase;
    string mysqlTableName;
    int mysqlMaxCrawlRetry;
    int mysqlSelectInterval;
    int mysqlSelectMaxLinkDepth;
    string mysqlSelectorSwitch;
    int mysqlDumperInsertBatch;
    int mysqlDumperUpdateBatch;
    int httpEventPrintStateInterval;
    int httpConnectTimeout;
    int httpSendTimeout;
    int httpRecvTimeout;
    int httpMaxRetry;
    int httpMaxConcurrence;
    int httpMaxRedirectDepth;
    int httpMaxPageSize;
    int httpConnectQueueSize;
    int schedulerPrintStateInterval;
    int schedulerIpScheduleInterval;
    size_t schedulerMaxUrlTableSize;
    string mongoHostAndPort;
    int extractorMaxQueueSize;
    // 解析超时时间，单位毫秒
    int extractorTimeout;
    string maindomainListFile;
    vector<string> maindomainList;
};

#endif //__CONF_H__
