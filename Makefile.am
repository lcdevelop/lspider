AUTOMAKE_OPTIONS=foreign

bin_PROGRAMS=lspider lspider_client parse_url

lspider_SOURCES=src/main.cpp src/conf.cpp src/extractor_worker_view.cpp src/lcrawler_constants.cpp src/logger_container.cpp src/moc_link_scheduler.cpp src/mysql_base.cpp src/url_context.cpp \
				src/crawl_listen_handler.cpp src/http_event_engine.cpp src/lcrawler_types.cpp src/lthread.cpp src/moc_mysql_dumper.cpp src/mysql_dumper.cpp src/url_tools.cpp \
				src/CrawlService.cpp src/http_processor.cpp src/link_scheduler.cpp src/moc_mysql_selector.cpp src/mysql_selector.cpp src/util.cpp \
				src/extractor.cpp src/http_protocol.cpp src/link_table.cpp src/moc_extractor_worker_view.cpp src/mongo_dumper.cpp src/request_recv.cpp
lspider_CPPFLAGS=$(QT_CPPFLAGS)/QtGui $(QT_CPPFLAGS)/QtSql $(QT_CPPFLAGS)/QtCore \
				 $(BOOST_CPPFLAGS) $(LOG4CPLUS_CPPFLAGS) $(MONGOCLIENT_CPPFLAGS) $(LIBEVENT_CPPFLAGS) \
				 $(QT_CPPFLAGS)/QtWebKit $(THRIFT_CPPFLAGS)

lspider_client_SOURCES=src/lspider_client.cpp src/lcrawler_constants.cpp src/CrawlService.cpp src/lcrawler_types.cpp src/url_context.cpp src/logger_container.cpp src/url_tools.cpp src/util.cpp
lspider_client_CPPFLAGS=$(QT_CPPFLAGS)/QtGui $(QT_CPPFLAGS)/QtSql $(QT_CPPFLAGS)/QtCore \
				 $(BOOST_CPPFLAGS) $(LOG4CPLUS_CPPFLAGS) $(MONGOCLIENT_CPPFLAGS) $(LIBEVENT_CPPFLAGS) \
				 $(QT_CPPFLAGS)/QtWebKit $(THRIFT_CPPFLAGS)

parse_url_SOURCES=src/parse_url.cpp src/url_tools.cpp

LIBS=$(QT_LDFLAGS)/ -lQtCore -lQtGui -lQtWebKit -lQtNetwork -lQtSql \
				$(MONGOCLIENT_LDFLAGS) -lmongoclient $(LIBEVENT_LDFLAGS) -levent -levent_pthreads \
				$(BOOST_LDFLAGS) -lboost_system -lboost_thread -lboost_filesystem -lboost_program_options -lboost_regex $(LOG4CPLUS_LDFLAGS) -llog4cplus \
				$(THRIFT_LDFLAGS) -lthrift -lz -lpthread

