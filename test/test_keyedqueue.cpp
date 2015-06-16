/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Fri 29 May 2015 04:51:48 PM CST
 * Description: 
 ************************************************************************/

#include <string.h>
#include <stdio.h>
#include "keyed_queue.h"

struct ElementType {
    ElementType() {
        DLINK_INITIALIZE(&link);
    }
    DLINK link;
    int key;
};

bool func(ElementType *pe, ptr data)
{
    printf("%d\n", pe->key);
    return true;
}

int main(int argc, char *argv[])
{
    ptr p;
    TKeyedQueue<int, ElementType> kq;
    ElementType e;
    ElementType e2;

    e.key = 1;
    e2.key = 2;
    printf("%d\n", __LINE__);
    kq.Enumerate(func, p);
    printf("pushbackret=%d\n", kq.push_back(&e));
    printf("pushbackret=%d\n", kq.push_back(&e2));
    printf("%d\n", __LINE__);
    kq.Enumerate(func, p);
    ElementType *pe = kq.pop_front();
    printf("%d\n", __LINE__);
    kq.Enumerate(func, p);
    ElementType *e3 = kq.find(1);
    kq.remove(2);
    printf("%d\n", __LINE__);
    kq.Enumerate(func, p);
    return 0;
}
