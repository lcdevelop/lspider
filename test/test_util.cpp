/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Sat 13 Jun 2015 12:55:53 PM CST
 * Description: 
 ************************************************************************/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iconv.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <zlib.h>
#include <string>
#include <stdio.h>
#include <vector>
//#include "util.h"

using std::string;
using std::vector;

int ip_ntoaTest (const unsigned int ip, char * ipstring, int len)
{
	if ( inet_ntop (AF_INET, &ip, ipstring, len) == NULL)
		return 1;
	else 
		return 0;
}

vector<unsigned long> getSockAddrTest(const char *host)
{
    vector<unsigned long> addrs;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (0 == getaddrinfo(host, NULL, &hints, &res)) {
        if (NULL != res) {
            for (struct addrinfo *rp = res; rp != NULL; rp = rp->ai_next) {
                unsigned long result = ((sockaddr_in*)rp->ai_addr)->sin_addr.s_addr;
                addrs.push_back(result);
            }
            freeaddrinfo(res);
        }
    }
    return addrs;
}

vector<string> getipTest(const char *host)
{
    vector<string> result;
    vector<unsigned long> addrs = getSockAddrTest(host);
    if (0 == addrs.size()) {
        fprintf (stderr, "warn: %s:%d getSockAddrTest %s\n", __FILE__, __LINE__, host);
    } else {
        for (int i = 0; i < addrs.size(); i++) {
            char ipstring[16] = {'\0'};
            if (0 == ip_ntoaTest(addrs[i], ipstring, 16)) {
                result.push_back(ipstring);
            }
        }
    }
    return result;
}

int main(int argc, char *argv[])
{
    vector<string> ips = getipTest("facebook.cn");
    for (int i = 0; i < ips.size(); ++i) {
        printf("ip=%s\n", ips[i].c_str());
    }
    return 0;
}
