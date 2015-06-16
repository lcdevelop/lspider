/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Tue 19 May 2015 03:27:05 PM CST
 * Description: 
 ************************************************************************/

#ifndef __MYSQL_DUMPER_H__
#define __MYSQL_DUMPER_H__

#include <string>
#include <map>
#include <QThread>
#include <QSqlDatabase>
#include "synced_queue.h"
#include "DoubleList.h"
#include "mysql_base.h"

using std::string;
using std::map;

class UrlContext;

class MySqlDumper : public QThread, public MySqlBase
{
    Q_OBJECT
public:
    MySqlDumper(string connectionName);

    virtual ~MySqlDumper();

    virtual void stop();

    virtual void run();

    TSyncedQueue<UrlContext, TLinkedList<UrlContext> > waitDumpQueue; // 待写mongo队列

private:
    bool dump(UrlContext *urlContext);
    bool updateCrawlState(UrlContext *urlContext);
    bool dumpOneLink(const string href, const string anchor, UrlContext *urlContext);
    bool dumpHub(UrlContext *urlContext);
    /**
     * 这里有个缺陷，就是flush是由数据触发的，每n个一触发，如果没数据可能剩一些触发不了
     */
    bool flushInsert();
    bool flushUpdate();
    void resetInsertLinksCmd();
    void resetUpdateLinksCmd();

    bool _isStop;
    QString _insertLinksCmd;
    QString _updateLinksCmd;
    int _insertValueCount;
    int _updateValueCount;
};

#endif //__MYSQL_DUMPER_H__
