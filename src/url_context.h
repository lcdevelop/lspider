/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:56:35 2015
 * Description: 
 ************************************************************************/

#ifndef __URL_CONTEXT_H__
#define __URL_CONTEXT_H__

#include <string>
#include <map>
#include <vector>
#include <string.h>
#include <evhttp.h>
#include "link.h"
#include "atomic.h"
#include "http_protocol.h"
#include "url_tools.h"

// 最大http请求头长度
#define MAX_HEAD_CMD_LEN 8192
#define MAX_SIGN_LEN 24

using std::string;
using std::map;
using std::vector;
using std::pair;

typedef string Url;
typedef string Site;
typedef string Ip;

//typedef vector<pair<string, string> > LINK_TYPE;
typedef map<string, string> LINK_TYPE;

static volatile int g_uuid = 0;

class HttpProcessor;

/**
 * url请求的上下文信息，包含基本信息和中间状态,生命周期从请求过来到抓取结束
 */
class UrlContext
{
public:
    // 网络通信状态类型
    typedef enum {
        CONNECTING, // 正在连接
        SENDING, // 正在发送请求
        RECVING, // 正在接受数据
        DONE, // 抓取完成
        TIMEOUT, // 抓取超时
        PROTERR, // 协议错误
        DELETED // 已delete
    } StatusType;

    UrlContext();
    ~UrlContext();

    void init();
    bool parseUrl();

public:
    // 直接赋值的变量
    Url url; // url地址
    int uuid; // 全局唯一id
    int linkDepth; // 链接深度,以hub页为准
    bool hub; // 是否是索引页

    // url解析生成的变量
    char host[MAX_SITE_LEN]; // url解析出的域名
    char strport[MAX_PORT_LEN]; // url解析出的端口号
    char file[MAX_PATH_LEN]; // url解析出的路径部分
    Ip ip; // 点分十进制ip地址
    Site site; // 站点名，可能和host相同，也可能不同
    bool hasParsed; // 是否已经解析过了
    char sign[MAX_SIGN_LEN]; // url签名

    // 动态变化的变量
    Url redirectUrl; // 重定向url
    int redirectDepth; // 重定向次数
    int retry; // 抓取重试次数
    StatusType status;

    // 网络通信的变量
    char requestData[MAX_HEAD_CMD_LEN]; // http请求头
    int requestLen; // http请求头长度
    int writeLen; // 已经发送的请求头长度
    int sock; // 和网站的连接socket
    string recvData; // 接收的http返回结果数据

    // protocol处理结果
    int firstLineLen; // response第一行长度
    int headerLen; // response的header部分长度
    int uncompressedBodyLen; // response的body部分长度
    int bodyPos; // response的body首位置
    string httpVersion; // response里提取的HTTP版本信息
    int httpStatus; // response里提取的返回状态信息
    string httpReason; // response里提取的返回码描述信息
    map<string, string> fields; // response的header部分域kv数据
    string header; // header
    string body; // body部分数据
    string cookie; // cookie
    bool finishAnalyseFirstLine; // 是否完成第一行解析
    bool finishAnalyseHeader; // 是否完成header解析
    bool finishAnalyseBody; // 是否完成body解析

    // 传递指针
    HttpProcessor *httpProcessor; // 上下文传递指针
    struct event *event; // 上下文传递指针
    event_base *base; // 上下文传递指针

    // 统计信息
    struct timeval beginConnectTime; // 开始连接时间
    struct timeval beginSendTime; // 开始发送请求时间
    struct timeval beginRecvTime; // 开始接收结果时间
    struct timeval finishTime; // 完成接收时间
    long crawlElapse; // 抓取历经多久，单位毫秒

    // extractor解析生成的变量
    string title;
    LINK_TYPE links; // 后链，包含链接和anchor，有重复

    // 特殊数据结构需要的变量
    DLINK link; // 队列数据结构要用到的变量
    Url key; // keyedQueue数据结构要用的变量
};

struct UrlContextComp {
    bool operator() (UrlContext *& u1, UrlContext *& u2)
    {
        return u1->linkDepth > u2->linkDepth;
    }
};


#endif
