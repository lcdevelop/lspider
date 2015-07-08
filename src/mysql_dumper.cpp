/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Tue 19 May 2015 03:32:22 PM CST
 * Description: 
 ************************************************************************/

#include <unistd.h>
#include <time.h>
#include <QtSql/QSqlQuery>
#include <QtCore/QVariant>
#include <QtSql/qsqldatabase.h>
#include "logger_container.h"
#include "util.h"
#include "url_tools.h"
#include "mysql_dumper.h"
#include "url_context.h"

MySqlDumper::MySqlDumper(string connectionName)
    :MySqlBase(connectionName), _dumpCount(0)
{
    _isStop = false;
    resetInsertLinksCmd();
    resetUpdateLinksCmd();
    _insertValueCount = 0;
    _updateValueCount = 0;
}


MySqlDumper::~MySqlDumper()
{
    stop();
}

void MySqlDumper::run()
{
    LOG(INFO, "start");
    if (!initMySql()) {
        LOG(FATAL, "mysqlDumper initMySql fail");
        _exit(-1);
        return;
    }

    while (!_isStop) {
        // 如果不刷一下，总是剩余一些刷不进去
        if (0 == waitDumpQueue.size()) {
            flushUpdate();
            flushInsert();
        }
        // 取队列
        UrlContext *urlContext = waitDumpQueue.pop_front();
        dump(urlContext);
        delete urlContext;
    }
}

void MySqlDumper::stop()
{
    _isStop = true;
    this->terminate();
    MySqlBase::stop();
}

bool MySqlDumper::dump(UrlContext *urlContext)
{
    if (urlContext->hub) {
        dumpHub(urlContext);
    }
    updateCrawlState(urlContext);

    // 新增后链
    LINK_TYPE & links = urlContext->links;
    LINK_TYPE::iterator iter = links.begin();
    for (; links.end() != iter; ++iter) {
        string href = iter->first;
        to_utf8(href);
        string anchor = iter->second;
        to_utf8(anchor);

        dumpOneLink(href, anchor, urlContext);
    }

    LOG_F(DEBUG, "%d [%s] dump mysql success",
          urlContext->uuid,
          urlContext->url.c_str());

    return true;
}

bool MySqlDumper::updateCrawlState(UrlContext *urlContext)
{
    // 更新这条url的抓取状态
    // 计算签名
    uint64_t sign = get_url_sign64(urlContext->url.c_str());
    char signstr[MAX_SIGN_LEN];
    snprintf(signstr, MAX_SIGN_LEN, "%lu", sign);
    signstr[23] = '\0';

    _updateLinksCmd += QString(signstr);
    _updateLinksCmd += QString(",");

    _updateValueCount++;

    if (_updateValueCount >= Conf::instance()->mysqlDumperUpdateBatch) {
        flushUpdate();
    }

    return true;
}

bool MySqlDumper::dumpOneLink(const string href, const string anchor, UrlContext *urlContext)
{
    // 计算签名
    uint64_t sign = get_url_sign64(href.c_str());
    char signstr[MAX_SIGN_LEN];
    snprintf(signstr, MAX_SIGN_LEN, "%lu", sign);
    signstr[23] = '\0';

    // 计算maindomain
    char maindomain[MAX_SITE_LEN] = {'\0'};
    if (false == fetch_url_maindomain(href.c_str(), maindomain, MAX_SITE_LEN)) {
        return false;
    }

    _insertLinksCmd += QString("(");
    _insertLinksCmd += QString(signstr) + ",\'";
    _insertLinksCmd += QString(href.c_str()) + "\',\'";
    _insertLinksCmd += QString(maindomain) + "\',\'";
    _insertLinksCmd += QString(urlContext->url.c_str()) + "\',\'";
    _insertLinksCmd += QString(anchor.c_str()).replace('\'', "\\'") + "\',";
    _insertLinksCmd += QString::number(urlContext->linkDepth + 1) + ",";
    _insertLinksCmd += QString::number(MySqlBase::NEW) + ",";
    _insertLinksCmd += QString("current_timestamp()") + "),";

    _insertValueCount++;

    if (_insertValueCount >= Conf::instance()->mysqlDumperInsertBatch) {
        flushInsert();
    }
    return true;
}

bool MySqlDumper::dumpHub(UrlContext *urlContext)
{
    uint64_t sign = get_url_sign64(urlContext->url.c_str());
    char signstr[MAX_SIGN_LEN];
    snprintf(signstr, MAX_SIGN_LEN, "%lu", sign);
    signstr[23] = '\0';

    char maindomain[MAX_SITE_LEN] = {'\0'};
    if (false == fetch_url_maindomain(urlContext->url.c_str(), maindomain, MAX_SITE_LEN)) {
        return false;
    }

    QString cmd = QString("UPDATE IGNORE ") + QString(TABLE_NAME.c_str())
        + " SET sign=" + QString(signstr) + ", url=\'" + QString(urlContext->url.c_str())
        +"\', maindomain=\'" + QString(maindomain)
        + "\', linkdepth=0, crawlstate=" + QString::number(MySqlBase::NEW)
        + ", hub=true, crawledtime=current_timestamp()";

    QSqlQuery updateQuery(_db);
    updateQuery.prepare(cmd);
    if (false == updateQuery.exec()) {
        QSqlError err = updateQuery.lastError();
        LOG_F(WARN, "QSqlQuery fail errtype=%d cmd=%s text=%s",
              err.type(), updateQuery.lastQuery().toUtf8().constData(), err.text().toStdString().c_str());
        resetUpdateLinksCmd();
        return false;
    } else {
        LOG_F(INFO, "QSqlQuery success [cmd=%s]",
              updateQuery.lastQuery().toUtf8().constData());
        resetUpdateLinksCmd();
        return true;
    }
}

bool MySqlDumper::flushInsert()
{
    if (0 == _insertValueCount) {
        return false;
    }

    QSqlQuery query(_db);
    _insertLinksCmd.remove(_insertLinksCmd.length()-1, 1);
    query.prepare(_insertLinksCmd);
    atomic_add(&_dumpCount, _insertValueCount);
    if (false == query.exec()) {
        QSqlError err = query.lastError();
        // 重复key不打log
        if (!err.text().contains("Duplicate entry")) {
            LOG_F(WARN, "QSqlQuery fail errtype=%d cmd=%s text=%s",
                  err.type(), query.lastQuery().toUtf8().constData(), err.text().toStdString().c_str());
        }
        resetInsertLinksCmd();
        return false;
    } else {
        LOG_F(INFO, "QSqlQuery success [cmd=%s]",
              query.lastQuery().toUtf8().constData());
        resetInsertLinksCmd();
        return true;
    }
}

bool MySqlDumper::flushUpdate()
{
    if (0 == _updateValueCount) {
        return false;
    }

    QSqlQuery updateQuery(_db);
    _updateLinksCmd.remove(_updateLinksCmd.length()-1, 1);
    _updateLinksCmd += QString(")");
    updateQuery.prepare(_updateLinksCmd);
    if (false == updateQuery.exec()) {
        QSqlError err = updateQuery.lastError();
        LOG_F(WARN, "QSqlQuery fail errtype=%d cmd=%s text=%s",
              err.type(), updateQuery.lastQuery().toUtf8().constData(), err.text().toStdString().c_str());
        resetUpdateLinksCmd();
        return false;
    } else {
        LOG_F(INFO, "QSqlQuery success [cmd=%s]",
              updateQuery.lastQuery().toUtf8().constData());
        resetUpdateLinksCmd();
        return true;
    }
}

void MySqlDumper::resetInsertLinksCmd()
{
    _insertLinksCmd = QString("INSERT IGNORE INTO ") + QString(TABLE_NAME.c_str()) +
        "(sign, url, maindomain, prelink, preanchor, linkdepth, crawlstate, foundtime)"
        " VALUES";
    _insertValueCount = 0;
}

void MySqlDumper::resetUpdateLinksCmd()
{
    _updateLinksCmd = QString("UPDATE ") + QString(TABLE_NAME.c_str())
        + " SET crawlstate="
        + QString::number(MySqlBase::CRAWLED)
        + ",crawledtime=current_timestamp()"
        + " WHERE sign IN (";
    _updateValueCount = 0;
}

void MySqlDumper::control(string& response, const string& cmd)
{
    char msg[1024] = {'\0'};
    snprintf(msg, 1024, "MySqlDumper dumpCount:%d",
             _dumpCount);
    response = msg;
}
