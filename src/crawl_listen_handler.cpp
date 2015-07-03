/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:55:32 2015
 * Description: 
 ************************************************************************/


#include "logger_container.h"
#include "http_processor.h"
#include "url_context.h"
#include "crawl_listen_handler.h"

CrawlListenHandler::CrawlListenHandler(HttpProcessor *httpProcessor, CmdCtrler *cmdCtroler)
    :_httpProcessor(httpProcessor), _cmdCtrler(cmdCtroler)

{
}

void CrawlListenHandler::request(const IUrl& u)
{
    UrlContext *urlContext = new UrlContext;
    char normalurl[MAX_URL_LEN];
    if (1 == normalize_url(u.str.c_str(), normalurl, MAX_URL_LEN)) {
        urlContext->url = string("http://") + string(normalurl);
    } else {
        urlContext->url = u.str;
    }
    urlContext->linkDepth = 0;
    urlContext->hub = u.hub;

    uint64_t sign = get_url_sign64(u.str.c_str());
    snprintf(urlContext->sign, MAX_SIGN_LEN, "%lu", sign);

    _httpProcessor->pushConnectQueue(urlContext);
}

void CrawlListenHandler::exec_cmd(string& response, const string& cmd)
{
    LOG_F(DEBUG, "cmd=%s", cmd.c_str());
    assert(NULL != _cmdCtrler);
    _cmdCtrler->control(response, cmd);
}
