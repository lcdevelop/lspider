/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:56:04 2015
 * Description: 
 ************************************************************************/

#ifndef __HTTP_PROCESSOR_H__
#define __HTTP_PROCESSOR_H__

#include <string>
#include "lthread.h"
#include "synced_queue.h"
#include "locked_queue.h"
#include "DoubleList.h"
#include "controllable.h"
#include "mysql_base.h"

using std::string;

class HttpEventEngine;
class MongoDumper;
class Extractor;
class UrlContext;

/**
 * http协议处理，网络收发数据
 */
class HttpProcessor : public lthread::LThread, public MySqlBase, public Controllable
{
public:
    typedef enum {
        E_ERR = -1, // socket读写异常
        E_FINISH = 0, // 发送或接收完成
        E_AGAIN = 1, // 读写正常，buffer为空
    } SndRcvRetType;

    HttpProcessor(MongoDumper* mongoDumper, Extractor *extractor, string connectionName);
    ~HttpProcessor();

    TSyncedQueue<UrlContext, TLinkedList<UrlContext> > waitConnectQueue; // 待连接请求队列

    void pushConnectQueue(UrlContext *urlContext);

    virtual void run();
    void stop();
    virtual void control(string& response, const string& cmd);

    /**
     * 收到完整网页包后回调
     */
    virtual void onRecvOnePage(UrlContext *urlContext);
    /**
     * 抓取超时回调
     */
    virtual void onTimeout(UrlContext *urlContext);
    /**
     * 对方关闭连接
     */
    virtual void onBroken(UrlContext *urlContext);

    void printState();


private:
    // libevent 回调事件
    static void on_connected(int sock, short /*event*/, void* arg);
    static void on_send(int sock, short /*event*/, void* arg);
    static void on_recv(int sock, short /*event*/, void* arg);

    int connect(UrlContext *urlContext);
    bool getServAddr(UrlContext *urlContext, struct sockaddr_in &serv_addr);
    int createNonblockingSocket();
    SndRcvRetType tryRecv(UrlContext *urlContext);
    SndRcvRetType trySend(UrlContext *urlContext);
    void doWithSockTimeout(UrlContext *urlContext);
    void doWithSend(UrlContext *urlContext);
    void doWithRecv(UrlContext *urlContext);
    void doWithCrawlDone(UrlContext *urlContext);
    bool doWithRedirect(UrlContext *urlContext);

    HttpEventEngine *_engine;
    MongoDumper *_mongoDumper;
    Extractor *_extractor;
    bool _isStop;

    volatile int _concCount;
    volatile int _tryProcCount;
    volatile int _procFinishCount;
};

#endif
