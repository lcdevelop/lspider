/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Mon 29 Jun 2015 11:26:13 AM CST
 * Description: 
 ************************************************************************/

#ifndef __CMD_CTRLER_H__
#define __CMD_CTRLER_H__

#include <string>
#include <map>
#include <QtWidgets/QApplication>
#include "controllable.h"

using std::string;
using std::map;

class CmdCtrler
{
public:
    CmdCtrler(QApplication *app);
    ~CmdCtrler();

    void control(string& response, const string& cmd);
    void addHandler(const string name, Controllable* controllable);

private:
    map<string, Controllable* > _handlers;
    QApplication *_app;
};

#endif //__CMD_CTRLER_H__
