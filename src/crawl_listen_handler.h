/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:55:40 2015
 * Description: 
 ************************************************************************/

#ifndef __CRAWL_LISTEN_HANDLER_H__
#define __CRAWL_LISTEN_HANDLER_H__

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include "CrawlService.h"
#include "cmd_ctrler.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using boost::shared_ptr;

using namespace  ::lspider;

class HttpProcessor;
class RequestRecv;

/**
 * 抓取器抓取服务的网络请求处理逻辑
 * 基于thrift RPC
 */
class CrawlListenHandler : virtual public CrawlServiceIf {
public:
    CrawlListenHandler(HttpProcessor *httpProcessor,
                       RequestRecv *requestRecv,
                       CmdCtrler *cmdCtrler);

    /**
     * RPC接口
     */
    void request(const IUrl& u);
    void exec_cmd(string& response, const string& cmd);

private:
    HttpProcessor *_httpProcessor; // 传递指针
    RequestRecv *_requestRecv;
    CmdCtrler *_cmdCtrler;
};

#endif
