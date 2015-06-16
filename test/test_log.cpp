/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Wed 27 May 2015 12:53:16 PM CST
 * Description: 
 ************************************************************************/

#include <string>
#include "logger_container.h"

int main(int argc, char *argv[])
{
    LoggerContainer::init();
    std::string str = "hello";
    LOG_F(DEBUG, "%lu", str.length());
    return 0;
}
