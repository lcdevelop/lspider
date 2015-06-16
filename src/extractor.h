/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:55:40 2015
 * Description: 
 ************************************************************************/

#ifndef __EXTRACTOR_H__
#define __EXTRACTOR_H__

#include "synced_queue.h"
#include "DoubleList.h"

class ExtractorWorkerView;
class UrlContext;

class Extractor
{
public:
    Extractor(ExtractorWorkerView *workerView);

    void start();

    void push(UrlContext *urlContext);
    UrlContext * pop();
    int queueSize();

private:
    ExtractorWorkerView *_extractorWorkerView;
    TSyncedQueue<UrlContext, TLinkedList<UrlContext> > _waitExtractQueue; // 待连接请求队列
};


#endif
