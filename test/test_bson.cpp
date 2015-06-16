/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Wed 27 May 2015 09:39:35 AM CST
 * Description: 
 ************************************************************************/

#include <stdio.h>
#include "mongo/bson/bson.h"

using namespace mongo;

int main(int argc, char *argv[])
{
    BSONObjBuilder data;
    data.append("title", "this is title");
    BSONObjBuilder arr;
    BSONObjBuilder subdata1;
    subdata1.append("anchor", "this is anchor");
    subdata1.append("href", "this is href");
    BSONObjBuilder subdata2;
    subdata2.append("anchor", "this is anchor");
    subdata2.append("href", "this is href");
    arr.append("link1", subdata1.obj());
    arr.append("link2", subdata2.obj());
    data.appendArray("links", arr.obj());
    printf("%s\n", data.obj().toString().c_str());
    return 0;
}
