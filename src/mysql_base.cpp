/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Tue 19 May 2015 03:32:22 PM CST
 * Description: 
 ************************************************************************/

#include <unistd.h>
#include <QSqlQuery>
#include <QSqlError>
#include "logger_container.h"
#include "mysql_dumper.h"

const string MySqlBase::TABLE_NAME = "link";

MySqlBase::MySqlBase(string connectionName,
                     string hostName,
                     int port,
                     string userName,
                     string passWord,
                     string databaseName)
    :_connectionName(QString(connectionName.c_str())),
    _hostName(QString(hostName.c_str())),
    _port(port),
    _userName(QString(userName.c_str())),
    _passWord(QString(passWord.c_str())),
    _databaseName(QString(databaseName.c_str()))
{
}


MySqlBase::~MySqlBase()
{
    stop();
}

void MySqlBase::stop()
{
    _db.close();
    QSqlDatabase::removeDatabase(_databaseName);
}

bool MySqlBase::initMySql()
{
    _db = QSqlDatabase::addDatabase("QMYSQL", _connectionName);
    _db.setDatabaseName(_databaseName);
    _db.setHostName(_hostName);
    _db.setPort(_port);
    _db.setUserName(_userName);
    _db.setPassword(_passWord);
    if (!_db.open()) {
        QSqlError err = _db.lastError();
        LOG_F(FATAL, "QSqlDatabase open fail errtype=%d text=%s [%s][%s][%s:%d][%s][%s]", 
              err.type(), err.databaseText().toStdString().c_str(),
              _connectionName.toStdString().c_str(), _databaseName.toStdString().c_str(),
              _hostName.toStdString().c_str(), _port,
              _userName.toStdString().c_str(), _passWord.toStdString().c_str());
        return false;
    }
    return true;
}
