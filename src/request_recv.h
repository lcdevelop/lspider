/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:54:43 2015
 * Description: 
 ************************************************************************/

#ifndef __REQUEST_RECV_H__
#define __REQUEST_RECV_H__

#include "lthread.h"
#include "controllable.h"

class HttpProcessor;
class CmdCtrler;

class RequestRecv : public lthread::LThread, public Controllable
{
public:
    RequestRecv(HttpProcessor *httpProcessor, CmdCtrler *cmdCtrler);
    virtual void run();
    void stop();
    void addRequestCount();
    virtual void control(string& response, const string& cmd);

private:
    HttpProcessor *_httpProcessor;
    CmdCtrler *_cmdCtrler;
    volatile int _requestCount;
};

#endif
