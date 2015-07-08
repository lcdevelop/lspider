/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:56:17 2015
 * Description: 
 ************************************************************************/

#include <QtWidgets/QApplication>
#include <QtCore/QTextCodec>
#include "logger_container.h"
#include "request_recv.h"
#include "http_processor.h"
#include "mongo_dumper.h"
#include "extractor.h"
#include "extractor_worker_view.h"
#include "mysql_dumper.h"
#include "mysql_selector.h"
#include "link_scheduler.h"
#include "conf.h"
#include "cmd_ctrler.h"

int main(int argc, char ** argv)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    // 用户webkit解析事件循环
    QApplication app(argc, argv);

    // 日志初始化
    LoggerContainer::init();

    if (false == Conf::instance()->load("./conf/conf.xml")) {
        return -1;
    }

    event_enable_debug_mode();

    CmdCtrler *cmdCtrler = new CmdCtrler(&app);

    // 写mysql
    MySqlDumper *mySqlDumper = new MySqlDumper("linkdump");
    MySqlSelector *mySqlSelector = new MySqlSelector("linkselect");

    // 初始化mongoclient，连接mongodb
    MongoDumper *mongoDumper = new MongoDumper(mySqlDumper, Conf::instance()->mongoHostAndPort);
    if (!mongoDumper->initMongo()) {
        LOG(FATAL, "mongoDumper initMongo fail");
        return -1;
    }

    // webkit模拟浏览器
    ExtractorWorkerView *workerView = new ExtractorWorkerView;
    // webkit解析控制器
    Extractor *extractor = new Extractor(workerView);

    workerView->setMongoDumper(mongoDumper);
    workerView->setExtractor(extractor);

    // http网络处理
    HttpProcessor *httpProcessor = new HttpProcessor(mongoDumper, extractor, "httpprocessor");
    // 请求接收监听
    RequestRecv *recv = new RequestRecv(httpProcessor, cmdCtrler);

    // 链接调度
    LinkScheduler *linkScheduler = new LinkScheduler(httpProcessor);
    mySqlSelector->setLinkScheduler(linkScheduler);

    cmdCtrler->addHandler("RequestRecv", recv);
    cmdCtrler->addHandler("MySqlSelector", mySqlSelector);
    cmdCtrler->addHandler("HttpProcessor", httpProcessor);
    cmdCtrler->addHandler("LinkScheduler", linkScheduler);
    cmdCtrler->addHandler("MongoDumper", mongoDumper);
    cmdCtrler->addHandler("MySqlDumper", mySqlDumper);
    cmdCtrler->addHandler("Extractor", extractor);

    mongoDumper->start();
    httpProcessor->start();
    recv->start();
    mySqlDumper->start();
    usleep(100000); // 停顿一下，避免两个mysql连接同时起不来
    mySqlSelector->start();
    linkScheduler->start();
    extractor->start(); // 阻塞

    app.exec();
    return 0;
}
