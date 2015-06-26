/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Wed 03 Jun 2015 02:58:02 PM CST
 * Description: 
 ************************************************************************/

#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "logger_container.h"
#include "util.h"
#include "conf.h"

IMPLEMENT_SINGLETON(Conf);

bool Conf::load(const char *confFile)
{
    boost::property_tree::ptree pt;
    try {
        read_xml(confFile, pt);
        mysqlHost = pt.get<string>("lspider.mysql.host");
        mysqlPort = pt.get<int>("lspider.mysql.port");
        mysqlUser = pt.get<string>("lspider.mysql.user");
        mysqlPassword = pt.get<string>("lspider.mysql.password");
        mysqlDatabase = pt.get<string>("lspider.mysql.database");
        mysqlTableName = pt.get<string>("lspider.mysql.tableName");
        mysqlSelectInterval = pt.get<int>("lspider.mysql.selector.selectInterval");
        mysqlSelectMaxLinkDepth = pt.get<int>("lspider.mysql.selector.maxLinkDepth");
        mysqlMaxCrawlRetry = pt.get<int>("lspider.mysql.selector.maxCrawlRetry");
        mysqlSelectorSwitch = pt.get<string>("lspider.mysql.selector.switch");
        maindomainListFile = pt.get<string>("lspider.mysql.selector.maindomainListFile");
        mysqlDumperInsertBatch = pt.get<int>("lspider.mysql.dumper.insertBatch");
        mysqlDumperUpdateBatch = pt.get<int>("lspider.mysql.dumper.updateBatch");
        httpEventPrintStateInterval = pt.get<int>("lspider.http.eventPrintStateInterval");
        httpConnectTimeout = pt.get<int>("lspider.http.connectTimeout");
        httpSendTimeout = pt.get<int>("lspider.http.sendTimeout");
        httpRecvTimeout = pt.get<int>("lspider.http.recvTimeout");
        httpMaxRetry = pt.get<int>("lspider.http.maxRetry");
        httpMaxConcurrence = pt.get<int>("lspider.http.maxConcurrence");
        httpMaxRedirectDepth = pt.get<int>("lspider.http.maxRedirectDepth");
        httpMaxPageSize = pt.get<int>("lspider.http.maxPageSize");
        httpConnectQueueSize = pt.get<int>("lspider.http.connectQueueSize");
        schedulerPrintStateInterval = pt.get<int>("lspider.scheduler.printStateInterval");
        schedulerIpScheduleInterval = pt.get<int>("lspider.scheduler.ipScheduleInterval");
        schedulerMaxUrlTableSize = pt.get<size_t>("lspider.scheduler.maxUrlTableSize");
        mongoHostAndPort = pt.get<string>("lspider.mongo.hostAndPort");
        extractorMaxQueueSize = pt.get<int>("lspider.extractor.maxQueueSize");
        extractorTimeout = pt.get<int>("lspider.extractor.timeout");

        if (false == loadMaindomainList(maindomainListFile)) {
            LOG_F(FATAL, "%s must exist and has at least one maindomain", maindomainListFile.c_str());
            return false;
        }
    } catch (boost::property_tree::xml_parser::xml_parser_error &e) {
        LOG_F(FATAL, "read_xml %s fail line:%ld message:%s what:%s",
              confFile, e.line(), e.filename().c_str(), e.what());
        return false;
    } catch (boost::property_tree::ptree_bad_path &e) {
        LOG_F(FATAL, "read_xml %s fail what:%s",
              confFile, e.what());
        return false;
    } catch (std::exception &e) {
        LOG_F(FATAL, "read_xml %s fail what:%s",
              confFile, e.what());
        return false;
    }

    return true;
}

bool Conf::loadMaindomainList(const string maindomainListFile)
{
    maindomainList.clear();
    std::fstream fin(maindomainListFile.c_str());
    std::string line;
    while (getline(fin, line)) {
        ltrim(rtrim(line));
        if (line.length() == 0) {
            continue;
        }
        maindomainList.push_back(line);
    }

    if (maindomainList.size() > 0) {
        return true;
    } else {
        return false;
    }
}
