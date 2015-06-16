/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Tue 19 May 2015 03:27:05 PM CST
 * Description: 
 ************************************************************************/

#ifndef __MYSQL_BASE_H__
#define __MYSQL_BASE_H__

#include <string>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include "conf.h"

using std::string;

class MySqlBase
{
public:
    typedef enum {
        NEW = 0,
        SELECTED = 1,
        CRAWLED = 2
    } CrawlState;

    MySqlBase(string connectionName,
              string hostName = Conf::instance()->mysqlHost,
              int port = Conf::instance()->mysqlPort,
              string userName = Conf::instance()->mysqlUser,
              string passWord = Conf::instance()->mysqlPassword,
              string databaseName = Conf::instance()->mysqlDatabase);

    virtual ~MySqlBase();

    virtual bool initMySql();
    virtual void stop();

protected:
    QString _connectionName;
    QString _hostName;
    int _port;
    QString _userName;
    QString _passWord;
    QString _databaseName;
    QSqlDatabase _db;

    static const string TABLE_NAME;
};

#endif //__MYSQL_BASE_H__
