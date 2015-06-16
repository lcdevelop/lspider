/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:58:37 2015
 * Description: 
 ************************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iconv.h>
#include <string.h>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <zlib.h>

using std::string;
using std::vector;

// 求时间差，单位毫秒
#define TIME_DIFF(start, end) \
    ((end.tv_sec-start.tv_sec)*1000+(end.tv_usec-start.tv_usec)/1000)

int ip_aton (const char *ipstring, unsigned int *ip);

int ip_ntoa (const unsigned int ip, char * ipstring, int len);

vector<unsigned long> getSockAddr(const char *host);

vector<string> getip(const char *host);

int setnonblocking (int sockfd);

string& rtrim(string &s);

string& ltrim(string &s);

int gzcompress(Bytef *data, uLong ndata,
               Bytef *zdata, uLong *nzdata);

int gzdecompress(Byte *zdata, uLong nzdata,
                 Byte *data, uLong *ndata);

void hex2dec(const char *hexStr, char *decStr);

bool isNonAsciiUTF8(const char* str, int &len);

bool is_utf8(const char* str);

bool to_utf8(const char* source_str, const size_t len, string & target_str);

bool to_utf8(string &str);

#endif
