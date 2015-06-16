/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu 04 Jun 2015 11:06:18 AM CST
 * Description: 
 ************************************************************************/

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include "url_tools.h"

int main(int argc, char *argv[])
{
    std::string line;
    while (getline(std::cin, line)) {
        const char *url = line.c_str();
        char host[1024];
        char strport[16];
        char file[1024];
        parse_url(url, host, 1024, strport, 16, file, 1024);
        if (strport[0] == '\0') {
            sprintf(strport, "80");
        }

        char trunk[64];
        const char* res = fetch_maindomain(host, trunk, 64);
        printf("%s\t%s\t%s\t%s\t%s\n", host, strport, file, res, trunk);
    }
    return 0;
}
