/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Sun 31 May 2015 10:02:16 AM CST
 * Description: 
 ************************************************************************/

#include <fstream>
#include <stdio.h>
#include "url_tools.h"

int main(int argc, char *argv[])
{
    std::fstream fin(argv[1]);
    std::string ReadLine;
    while(getline(fin,ReadLine)) {
        const char *url = ReadLine.c_str();
        //printf("%s\n", url);
        //printf("%d\n", isnormalized_url(url));
        //if (0 == isnormalized_url(url)) {
            char buf[4096];
            normalize_url(url, buf, 4096);
            if (strcmp(url, buf) != 0) {
                printf("%s to %s           %d to %d\n", url, buf, isnormalized_url(url), isnormalized_url(buf));
            }
       // }
    }
    /*
    char host[64];
    char strport[64];
    char file[64];
    if (1 == parse_url("https://www.baidu.com:333/index.html",
                       host,
                       MAX_SITE_LEN,
                       strport,
                       MAX_PORT_LEN,
                       file,
                       MAX_PATH_LEN)) {
    }
    printf("host=%s strport=%s file=%s\n", host, strport, file);
    */
    return 0;
}
