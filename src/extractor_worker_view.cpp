/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Wed 27 May 2015 10:01:38 AM CST
 * Description: 
 ************************************************************************/

#include <QWebSettings>
#include <QtWebKitWidgets/QWebFrame>
#include <QWebElementCollection>
#include <QWebElement>
#include "logger_container.h"
#include "util.h"
#include "mongo_dumper.h"
#include "url_context.h"
#include "extractor.h"
#include "conf.h"
#include "extractor_worker_view.h"

ExtractorWorkerView::ExtractorWorkerView(QWidget *parent)
    :QWebView(parent)
{
    setPage(new MyWebPage());
    _extractor = NULL;
    _mongoDumper = NULL;
    connect(this, SIGNAL(loadFinished(bool)), SLOT(onLoadFinish(bool)));

    this->page()->networkAccessManager()->setNetworkAccessible(QNetworkAccessManager::NotAccessible);
    this->settings()->setAttribute(QWebSettings::AutoLoadImages, false);
    this->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
    this->settings()->setMaximumPagesInCache(0);
    this->settings()->setObjectCacheCapacities(0, 0, 0);
    this->settings()->setOfflineWebApplicationCacheQuota(0);
}

void ExtractorWorkerView::setExtractor(Extractor *extractor)
{
    _extractor = extractor;
}

void ExtractorWorkerView::setMongoDumper(MongoDumper *mongoDumper)
{
    _mongoDumper = mongoDumper;
}

void ExtractorWorkerView::doWithOneUrl()
{
    assert(NULL != _extractor);
    _urlContext = _extractor->pop();
    QString body = QString(_urlContext->body.c_str());
    QString url = QString(_urlContext->url.c_str());
    // 设置解析超时时间
    _timerID = this->startTimer(Conf::instance()->extractorTimeout);
    this->setHtml(body, url);
}

void ExtractorWorkerView::getTitle()
{
    // 获取标题
    QString title = this->title();
    if (title.length() > 0) {
        _urlContext->title = title.toStdString();
    }
    to_utf8(_urlContext->title);
    LOG_F(DEBUG, "%d [%s] title=[%s]",
          _urlContext->uuid,
          _urlContext->url.c_str(),
          _urlContext->title.c_str());
}

void ExtractorWorkerView::getLinks()
{
    // 找所有超链接
    QWebFrame *frame = this->page()->mainFrame();
    QWebElementCollection collection = frame->documentElement().findAll("a[href]");
    foreach (QWebElement element, collection) {
        std::string anchor = element.toPlainText().toStdString();
        amendAnchor(anchor);
        std::string source_anchor = anchor;

        std::string href = element.attribute(QString("href")).toLocal8Bit().constData();
        to_utf8(href);
        ltrim(rtrim(href));

        if (href[0] != '#' && href != "" && href != "/"
            && (href.find("javascript") == std::string::npos)
            && (href.find("mailto") == std::string::npos)) {
            // 如果链接是相对路径
            if (std::string::npos == href.find("://")) {
                if (href[0] == '/') {
                    href = std::string("http://") + std::string(_urlContext->host) + href;
                } else {
                    href = std::string("http://") + std::string(_urlContext->host) + "/" + href;
                }
            } else if (0 != href.find("http")) {
                // 如果是其他特殊协议则不保存
                LOG_F(DEBUG, "%d [%s] ignore link[%s]",
                      _urlContext->uuid,
                      _urlContext->url.c_str(),
                      href.c_str());
                continue;
            }

            char normalurl[MAX_URL_LEN];
            if (1 == normalize_url(href.c_str(), normalurl, MAX_URL_LEN)) {
                href = string("http://") + string(normalurl);
            } else {
                LOG_F(DEBUG, "%d [%s] normalurl fail [%s]",
                      _urlContext->uuid,
                      _urlContext->url.c_str(),
                      href.c_str());
            }

            if (href.length() >= MAX_URL_LEN-1) {
                continue;
            }
            if (href == _urlContext->url) {
                continue;
            }

            LOG_F(DEBUG, "%d [%s] href=[%s] anchor=[%s]",
                  _urlContext->uuid,
                  _urlContext->url.c_str(),
                  href.c_str(),
                  anchor.c_str());
            _urlContext->links[href] = anchor;
        }
    }
}

void ExtractorWorkerView::amendAnchor(string &anchor)
{
    to_utf8(anchor);
    ltrim(rtrim(anchor));
    if (anchor.length() > MAX_ANCHOR_LEN) {
        anchor = anchor.substr(0, MAX_ANCHOR_LEN);
    }
    size_t pos = string::npos;
    if ((pos = anchor.find('\n')) != string::npos) {
        anchor = anchor.substr(0, pos);
    }
    if ((pos = anchor.find('\r')) != string::npos) {
        anchor = anchor.substr(0, pos);
    }
}

void ExtractorWorkerView::onLoadFinish(bool ok)
{
    assert(NULL != _mongoDumper);
    getTitle();
    getLinks();
    _mongoDumper->waitDumpQueue.push_back(_urlContext);
    killTimer(_timerID);
    doWithOneUrl();
}

void ExtractorWorkerView::timerEvent(QTimerEvent *event)
{
    LOG_F(WARN, "%d [%s] extract timeout", _urlContext->uuid, _urlContext->url.c_str());
    // 这里还不确定超时后重试能否就成功了，所以继续观察
    //doWithOneUrl();
    QString body = QString(_urlContext->body.c_str());
    QString url = QString(_urlContext->url.c_str());
    _timerID = this->startTimer(Conf::instance()->extractorTimeout);
    this->setHtml(body, url);
}
