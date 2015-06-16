/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Sun 31 May 2015 03:50:09 PM CST
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
        char host[1024];
        char strport[16];
        char file[1024];
        parse_url(url, host, 1024, strport, 16, file, 1024);

        char trunk[64];
        const char* res = fetch_maindomain(host, trunk, 64);
        printf("%s\t\t%s\t\t%s\n", host, res, trunk);
    }
    return 0;
}
