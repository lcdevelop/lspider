/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:54:43 2015
 * Description: 
 ************************************************************************/

#ifndef __REQUEST_RECV_H__
#define __REQUEST_RECV_H__

#include "lthread.h"

class HttpProcessor;

class RequestRecv : public lthread::LThread
{
public:
    RequestRecv(HttpProcessor *httpProcessor);
    virtual void run();
    void stop();

private:
    HttpProcessor *_httpProcessor;
};

#endif
