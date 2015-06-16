lspider
=====
Linux下的轻量级网页抓取系统

[下载地址](https://github.com/warmheartli/lspider/archive/master.zip)

### 功能特性
- 将整套spider系统功能浓缩到一个二进制中，部署轻便
- 多线程和并发网络通信，性能高，资源利用充分，单机可同时抓取数百站点并能保持cpu低占用
- 简易配置，指定待抓取站点列表和高质量种子链接即可自适应抓取扩散至整站，参考链接深度的广度优先遍历同时保证时新性
- 通过配置不同站点ip的抓取压力，全异步的压力控制和链接调度，优雅抓取，避免封禁
- 基于WebKit的可定制的页面渲染和链接提取，便于性能和效果之间的权衡
- mysql作为链接存储介质，更高效；mongo作为网页存储介质，便于字段扩展和快速查询
- 天然支持分布式部署(基于站点划分)，可横向扩展

### 安装依赖
- libevent (>=2.0.22)
- thrift (>=0.9.2)
- qt (>=4.8.6)
- mongo-cxx-driver-legacy (>=1.0.2)
- log4cplus (>=1.2.0)
- boost (>=1.58.0)

### 安装方法
- ./configure --prefix=${HOME}/lspider --with-qt=${HOME}/qt4/ --with-qt-libdir=${HOME}/qt4/lib --with-boost=${HOME}/boost/ --with-boost-libdir=${HOME}/boost/lib --with-log4cplus=${HOME}/log4cplus/ --with-log4cplus-libdir=${HOME}/log4cplus/lib --with-mongoclient=${HOME}/mongoclient/ --with-mongoclient-libdir=${HOME}/mongoclient/lib --with-libevent=${HOME}/libevent --with-libevent-libdir=${HOME}/libevent/lib --with-thrift=${HOME}/thrift/ --with-thrift-libdir=${HOME}/thrift/lib
    (具体路径要换，但参数需要写全)
- make
- make install

### 运行时依赖
- mysql (>=5.6.24)
    (用于存储链接)
- mysql建表命令:create.sql
- mongodb (>=3.0.3)
    (用于存储网页)
- xorg-x11-server-Xvfb

### 运行方法
- cd ${work_dir}
- cp ${lspider_dir}/conf.xml.demo ./conf.xml
- cp ${lspider_dir}/log.properties.demo ./log.properties
- cp ${lspider_dir}/maindomain.list.demo ./maindomain.list
- mkdir log
- export LD_LIBRARY_PATH=${HOME}/qt4/lib/:${HOME}/boost/lib/:${HOME}/log4cplus/lib/:${HOME}/thrift/lib/:${LD_LIBRARY_PATH}
- xvfb-run --auto-servernum --server-args="-screen 0 1280x760x24" ./lspider
