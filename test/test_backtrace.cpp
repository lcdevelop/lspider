/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Sun 05 Jul 2015 08:20:24 PM CST
 * Description: 
 ************************************************************************/

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
using std::string;

string getLine(int nptrs, void *buffer[100], char *argv[])
{
    string result;
    char cmd[512];
    int len = snprintf(cmd, sizeof(cmd),
                               "addr2line -ifsC -e %s", argv[0]);
    char *p = cmd + len;
    size_t s = sizeof(cmd) - len;
    for(int i = 0; i < nptrs; ++i) {
        if(s > 0) {
            len = snprintf(p, s, " %p", buffer[i]);
            p += len;
            s -= len;
        }
    }
    FILE *fp;
    char buf[128];
    printf("%s\n", cmd);
    if((fp = popen(cmd, "r"))) {
        while(fgets(buf, sizeof(buf), fp)) {
            result += buf;
        }
        pclose(fp);
    }

    return result;
}

string myfunc1(char *argv[])
{
    int nptrs;
    void *buffer[100];
    char **strings;
    string result;

    nptrs = backtrace(buffer, 100);

    strings = backtrace_symbols(buffer, nptrs);
    /*
    if (NULL != strings) {
        for (int i = 0; i < nptrs; i++) {
            result += "{";
            result += strings[i];
            result += "}  ";
        }
        free(strings);
    }
    */

    result += getLine(nptrs, buffer, argv);
    free(strings);

    return result;
}

int main(int argc, char *argv[])
{
    string result = myfunc1(argv);
    printf("%s\n", result.c_str());
    exit(EXIT_SUCCESS);
}

