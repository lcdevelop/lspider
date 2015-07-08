/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Fri 29 May 2015 09:54:47 AM CST
 * Description: 
 ************************************************************************/

#ifndef __MYSQL_SELECTOR_H__
#define __MYSQL_SELECTOR_H__

#include <string>
#include <QtCore/QThread>
#include "controllable.h"
#include "mysql_base.h"

using std::string;

class LinkScheduler;

class MySqlSelector : public QThread, public MySqlBase, public Controllable
{
    Q_OBJECT
public:
    MySqlSelector(string connectionName);
    virtual ~MySqlSelector();

    void setLinkScheduler(LinkScheduler *linkScheduler);

    void recoverCrawlState();

    virtual void stop();

    virtual void control(string& response, const string& cmd);

    /**
     * 线程执行函数
     */
    virtual void run();

private:
    bool select();

    bool _isStop;
    LinkScheduler *_linkScheduler;
};

#endif //__MYSQL_SELECTOR_H__
