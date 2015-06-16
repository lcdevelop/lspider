~/thrift/bin/thrift -r -out src/ --gen cpp lcrawler.thrift

moc-qt5 src/extractor_worker_view.h -o src/moc_extractor_worker_view.cpp
moc-qt5 src/extractor_worker_view.h -o src/moc_extractor_worker_view.cpp^C
moc-qt5 src/link_scheduler.h -o src/moc_link_scheduler.cpp
moc-qt5 src/mysql_dumper.h -o src/moc_mysql_dumper.cpp
moc-qt5 src/mysql_selector.h -o src/moc_mysql_selector.cpp
