/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Wed 27 May 2015 10:01:35 AM CST
 * Description: 
 ************************************************************************/

#ifndef __EXTRACTOR_WORKER_VIEW_H__
#define __EXTRACTOR_WORKER_VIEW_H__

#include <string>
#include <QWebView>
#include "DoubleList.h"

using std::string;

class Extractor;
class MongoDumper;
class UrlContext;

class MyWebPage : public QWebPage {
protected:
    virtual void javaScriptAlert (QWebFrame*, const QString &) {}
};

class ExtractorWorkerView : public QWebView
{
    Q_OBJECT
public:
    explicit ExtractorWorkerView(QWidget *parent = 0);


    void setExtractor(Extractor *extractor);
    void setMongoDumper(MongoDumper *mongoDumper);

    /**
     * 从队列取一条url并开始加载
     */
    void doWithOneUrl();

private:
    /**
     * 提取title
     */
    void getTitle();
    /**
     * 提取后链
     */
    void getLinks();
    /**
     * 修正anchor
     */
    void amendAnchor(string &anchor);

protected slots:
    /**
     * url加载完成触发回调
     */
    void onLoadFinish(bool ok);

protected:
    void timerEvent(QTimerEvent *event);

private:
    UrlContext *_urlContext; // 临时存储当前处理中的urlContext
    Extractor *_extractor;
    MongoDumper *_mongoDumper;
    int _timerID;

    const static int MAX_ANCHOR_LEN = 64;
};


#endif //__EXTRACTOR_WORKER_VIEW_H__
