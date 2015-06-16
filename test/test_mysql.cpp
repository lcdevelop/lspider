/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu 28 May 2015 02:03:54 PM CST
 * Description: 
 ************************************************************************/

#include <stdint.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QTextCodec>
#include <QSqlError>
#include <stdio.h>

int main(int argc, char *argv[])
{
    uint64_t v = 18142234485787147589;
    char value[24];
    sprintf(value, "%llu", v);
    printf("%llu %s\n", v, value);
    return 0;
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("lspider");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("");
    if (!db.open()) {
        QSqlError err = db.lastError();
        printf("db.open fail errtype=%d\n", err.type());
        return -1;
    }

    QString selectCmdStr = "SELECT sign, url, maindomain, ip, prelink, preanchor, weight, linkdepth, crawlstate, crawlretry, hub, fresh, foundtime, crawledtime FROM link" 
        " WHERE link.crawlstate=0 OR hub=true"
        " ORDER BY link.linkdepth";
    QSqlQuery query(selectCmdStr, db);
    QSqlRecord rec = query.record();

    if (query.isActive()) {
        while (query.next()) {
            QSqlRecord rec = query.record();
            printf("%llu\t%f\t%s\t%s\n",
                   (uint64_t)rec.value("sign").value<uint64_t>(),
                   rec.value("sign").toDouble(),
                   rec.value("sign").toString().toStdString().c_str(),
                   rec.value("url").toString().toStdString().c_str());
        }
    }

    db.close();
    QSqlDatabase::removeDatabase("QMYSQL");


    return 0;
}
