/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:55:32 2015
 * Description: 
 ************************************************************************/

#include <unistd.h>
#include "extractor_worker_view.h"
#include "logger_container.h"
#include "conf.h"
#include "url_context.h"
#include "extractor.h"

Extractor::Extractor(ExtractorWorkerView *extractorWorkerView)
{
    _extractorWorkerView = extractorWorkerView;
}

void Extractor::start()
{
    LOG(INFO, "start");
    _extractorWorkerView->doWithOneUrl();
}

void Extractor::push(UrlContext *urlContext)
{
    while (_waitExtractQueue.size() > Conf::instance()->extractorMaxQueueSize) {
        usleep(10000);
    }
    _waitExtractQueue.push_back(urlContext);
}

UrlContext * Extractor::pop()
{
    return _waitExtractQueue.pop_front();
}

int Extractor::queueSize()
{
    return _waitExtractQueue.size();
}

void Extractor::control(string& response, const string& cmd)
{
    LOG(DEBUG, "");
}
