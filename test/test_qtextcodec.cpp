/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Wed 17 Jun 2015 04:28:25 PM CST
 * Description: 
 ************************************************************************/

#include <stdio.h>
#include <QtCore/QString>

int main(int argc, char *argv[])
{
    QString str = "What's your name";
    QString str2 = str.replace('\'', "\'");
    printf("%s\n", str2.toStdString().c_str());
    return 0;
}
