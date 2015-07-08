/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Mon 29 Jun 2015 11:26:40 AM CST
 * Description: 
 ************************************************************************/

#include <sstream>
#include "logger_container.h"
#include "mysql_selector.h"
#include "mysql_dumper.h"
#include "mongo_dumper.h"
#include "link_scheduler.h"
#include "http_processor.h"
#include "extractor.h"
#include "cmd_ctrler.h"

using std::istringstream;

CmdCtrler::CmdCtrler(QApplication *app)
    :_app(app)
{
}

CmdCtrler::~CmdCtrler()
{
}

void CmdCtrler::control(string& response, const string& cmd)
{
    istringstream sscmd(cmd);
    string level1cmd;
    string level2cmd;
    string level3cmd;
    sscmd >> level1cmd;
    sscmd >> level2cmd;
    sscmd >> level3cmd;

    if ("help" == level1cmd) {
        if ("" == level2cmd) {
            response = "command: help|show";
        }
    } else if ("show" == level1cmd) {
        if ("RequestRecv" == level2cmd) {
            _handlers["RequestRecv"]->control(response, level3cmd);
        } else if ("MySqlSelector" == level2cmd) {
            _handlers["MySqlSelector"]->control(response, level3cmd);
        } else if ("HttpProcessor" == level2cmd) {
            _handlers["HttpProcessor"]->control(response, level3cmd);
        } else if ("LinkScheduler" == level2cmd) {
            _handlers["LinkScheduler"]->control(response, level3cmd);
        } else if ("MongoDumper" == level2cmd) {
            _handlers["MongoDumper"]->control(response, level3cmd);
        } else if ("MySqlDumper" == level2cmd) {
            _handlers["MySqlDumper"]->control(response, level3cmd);
        } else if ("Extractor" == level2cmd) {
            _handlers["Extractor"]->control(response, level3cmd);
        } else {
            response = "must specific modle: RequestRecv|MySqlSelector|HttpProcessor|LinkScheduler|MongoDumper|MySqlDumper|Extractor";
        }
    }
}

void CmdCtrler::addHandler(const string name, Controllable* controllable)
{
    LOG_F(DEBUG, "addHandler %s %p", name.c_str(), controllable);
    _handlers[name] = controllable;
}
