/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Tue 19 May 2015 03:27:05 PM CST
 * Description: 
 ************************************************************************/

#ifndef __MONGO_DUMPER_H__
#define __MONGO_DUMPER_H__

#include "mongo/client/dbclient.h"
#include "lthread.h"
#include "synced_queue.h"
#include "DoubleList.h"
#include "controllable.h"
#include "url_context.h"

class MySqlDumper;

class MongoDumper : public lthread::LThread, public Controllable
{
public:
    MongoDumper(MySqlDumper *mysqlDumper, std::string hostAndPort);
    ~MongoDumper();

    bool initMongo();
    void stop();

    virtual void run();
    virtual void control(string& response, const string& cmd);

    MySqlDumper * mysqlDumper();

    TSyncedQueue<UrlContext, TLinkedList<UrlContext> > waitDumpQueue; // 待写mongo队列

private:
    bool dump(UrlContext *urlContext);
    void makeData(UrlContext *urlContext, mongo::BSONObjBuilder & data);
    void makeLinkArray(UrlContext *urlContext, mongo::BSONObjBuilder & link_array);

    MySqlDumper *_mysqlDumper;
    mongo::DBClientConnection _conn;
    std::string _hostAndPort;
    bool _isStop;
    static const std::string _db;
    volatile int _dumpSuccCount;
    volatile int _dumpFailCount;
};

#endif //__MONGO_DUMPER_H__
