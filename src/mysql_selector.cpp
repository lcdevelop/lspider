/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Fri 29 May 2015 09:55:01 AM CST
 * Description: 
 ************************************************************************/

#include <unistd.h>
#include <string>
#include <QtSql/QSqlQuery>
#include <QtCore/QVariant>
#include <QtSql/QSqlRecord>
#include <QtSql/qsqldatabase.h>
#include "logger_container.h"
#include "url_context.h"
#include "link_scheduler.h"
#include "conf.h"
#include "mysql_selector.h"

using std::string;

MySqlSelector::MySqlSelector(string connectionName)
    :MySqlBase(connectionName)
{
    _isStop = false;
    _linkScheduler = NULL;
}

MySqlSelector::~MySqlSelector()
{
    stop();
}

void MySqlSelector::setLinkScheduler(LinkScheduler *linkScheduler)
{
    _linkScheduler = linkScheduler;
}

void MySqlSelector::stop()
{
    _isStop = true;
    this->terminate();
    MySqlBase::stop();
}

void MySqlSelector::run()
{
    if ("off" == Conf::instance()->mysqlSelectorSwitch) {
        return;
    }

    LOG(INFO, "start");
    if (!initMySql()) {
        LOG(FATAL, "mysqlSelector initMySql fail");
        _exit(-1);
        return;
    }

    recoverCrawlState();

    while (!_isStop) {
        LOG(DEBUG, "select once");
        select();
        sleep(Conf::instance()->mysqlSelectInterval);
    }
}

void MySqlSelector::recoverCrawlState()
{
    // 构造要抓取的网站主域列表
    vector<string> & maindomainList = Conf::instance()->maindomainList;
    assert(maindomainList.size() > 0);
    string inPhraseList = string("'") + maindomainList[0] + string("'");
    for (size_t i = 1; i < maindomainList.size(); i++) {
        inPhraseList += ",'" + maindomainList[i] + string("'");
    }

    QSqlQuery updateQuery(_db);
    updateQuery.prepare(QString("UPDATE ") + QString(TABLE_NAME.c_str())
                        + " SET crawlstate="
                        + QString::number(MySqlBase::NEW)
                        + " WHERE crawlstate=" + QString::number(MySqlBase::SELECTED)
                        + " AND crawlretry < "
                        + QString::number(Conf::instance()->mysqlMaxCrawlRetry)
                        + " AND maindomain IN (" + inPhraseList.c_str() + ")");
    if (false == updateQuery.exec()) {
        QSqlError err = updateQuery.lastError();
        LOG_F(WARN, "QSqlQuery fail errtype=%d cmd=%s text=%s",
              err.type(), updateQuery.lastQuery().toStdString().c_str(), err.text().toStdString().c_str());
    } else {
        LOG_F(DEBUG, "updateCmd: [%s]", updateQuery.lastQuery().toStdString().c_str());
    }
}


bool MySqlSelector::select()
{
    // 构造要抓取的网站主域列表
    // 动态加载
    Conf::instance()->loadMaindomainList(Conf::instance()->maindomainListFile);
    vector<string> & maindomainList = Conf::instance()->maindomainList;
    if (maindomainList.size() == 0) {
        LOG(FATAL, "maindomainList.size == 0");
    }
    string inPhraseList = string("'") + maindomainList[0] + string("'");
    for (size_t i = 1; i < maindomainList.size(); i++) {
        inPhraseList += ",'" + maindomainList[i] + string("'");
    }

    // 选取新增链接和种子链接
    QString selectCmdStr = "SELECT sign, url, maindomain, ip, prelink, preanchor, weight, linkdepth, crawlstate, crawlretry, hub, fresh, foundtime, crawledtime FROM "
        + QString(TABLE_NAME.c_str())
        + " WHERE ((crawlstate="
        + QString::number(MySqlBase::NEW)
        + " AND crawlretry < "
        + QString::number(Conf::instance()->mysqlMaxCrawlRetry)
        + " AND linkdepth <= " + QString::number(Conf::instance()->mysqlSelectMaxLinkDepth)
        + ") OR hub=true)"
        + " AND maindomain IN (" + inPhraseList.c_str() + ")"
        " ORDER BY linkdepth";
    LOG_F(DEBUG, "selectCmdStr: [%s]", selectCmdStr.toStdString().c_str());
    QSqlQuery query(selectCmdStr, _db);
    LOG_F(INFO, "selectResultNum=%d", query.size());

    if (query.isActive()) {

        // 更新crawlstate信息
        QSqlQuery updateQuery(_db);
        updateQuery.prepare("UPDATE " + QString(TABLE_NAME.c_str())
                            + " SET crawlstate="
                            + QString::number(MySqlBase::SELECTED)
                            + " WHERE (crawlstate=" + QString::number(MySqlBase::NEW)
                            + " OR hub=true) AND crawlretry < "
                            + QString::number(Conf::instance()->mysqlMaxCrawlRetry)
                            + " AND maindomain IN (" + inPhraseList.c_str() + ")");
        if (false == updateQuery.exec()) {
            QSqlError err = updateQuery.lastError();
            LOG_F(WARN, "QSqlQuery fail errtype=%d cmd=%s text=%s",
                  err.type(), updateQuery.lastQuery().toStdString().c_str(), err.text().toStdString().c_str());
        }

        while (query.next()) {
            QSqlRecord rec = query.record();
            UrlContext *urlContext = new UrlContext;
            strncpy(urlContext->sign, rec.value("sign").toString().toStdString().c_str(), MAX_SIGN_LEN);
            urlContext->url = rec.value("url").toString().toUtf8().constData();
            urlContext->linkDepth = rec.value("linkdepth").toInt();
            urlContext->hub = rec.value("hub").toBool();
            LinkTable::RetType ret = _linkScheduler->addUrl(urlContext);
            switch (ret) {
            case LinkTable::RT_PARSE_ERROR:
                // crawlretry++, crawlstate归零
                updateQuery.prepare("UPDATE " + QString(TABLE_NAME.c_str())
                                    + " SET crawlretry=crawlretry+1,crawlstate=" + QString::number(MySqlBase::NEW)
                                    + " WHERE sign="
                                    + QString(urlContext->sign));
                if (false == updateQuery.exec()) {
                    QSqlError err = updateQuery.lastError();
                    LOG_F(WARN, "QSqlQuery fail errtype=%d cmd=%s text=%s",
                          err.type(), updateQuery.lastQuery().toStdString().c_str(), err.text().toStdString().c_str());
                }
                delete urlContext;
                break;

            case LinkTable::RT_TABLE_FULL:
                // crawlstate归零
                updateQuery.prepare("UPDATE " + QString(TABLE_NAME.c_str())
                                    + " SET crawlstate=" + QString::number(MySqlBase::NEW)
                                    + " WHERE sign="
                                    + QString(urlContext->sign));
                if (false == updateQuery.exec()) {
                    QSqlError err = updateQuery.lastError();
                    LOG_F(WARN, "QSqlQuery fail errtype=%d cmd=%s text=%s",
                          err.type(), updateQuery.lastQuery().toStdString().c_str(), err.text().toStdString().c_str());
                }
                delete urlContext;
                break;

            case LinkTable::RT_OK:
                break;
            default:
                break;
            }
        }

        return true;
    } else {
        return false;
    }
}
