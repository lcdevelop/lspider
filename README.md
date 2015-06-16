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

### 安装方法(RHEL6)

===================================================================================================================

## 一、安装依赖库（包括libevent、boost、mongo-client、qt、thrift）

### 1、安装libevent

在http://libevent.org/ 中找最新版（如2.0.22）

`wget https://sourceforge.net/projects/levent/files/libevent/libevent-2.0/libevent-2.0.22-stable.tar.gz`

解压后执行

`./configure --prefix=${HOME}/libevent`

`make`

`make install`

### 2、安装boost

`yum install boost-devel`

### 3、安装scons(因为下面编译mongoclient要用到)

`yum install scons`

### 4、安装mongo-client

在https://github.com/mongodb/mongo-cxx-driver/releases/ 中找最新版（如1.0.2）

`wget https://github.com/mongodb/mongo-cxx-driver/archive/legacy-1.0.2.tar.gz`

解压后执行

`cd mongo-cxx-driver-legacy-1.0.2`

`scons --prefix=$HOME/mongo-client install`

### 5、安装log4cplus

`yum install log4cplus-devel`

### 6、安装qt5

`yum install qt5-qtbase-devel`

`yum install qt5-qtwebkit-devel`

### 7、安装bison（后面编译安装thrift会用到）

从http://ftp.gnu.org/gnu/bison/bison-2.7.tar.gz 下载2.5以上版本的bison，解压后执行

`cd bison-2.7`

`./configure`

`make`

`make install`

### 8、安装thrift

从http://thrift.apache.org/download 下载最新版thrift的源代码(如：http://apache.fayea.com/thrift/0.9.0/thrift-0.9.0.tar.gz)，解压后执行

`cd thrift-0.9.0`

`./configure --prefix=${HOME}/thrift --without-csharp --without-java --without-erlang --without-nodejs --without-lua --without-python --without-perl --without-ruby --without-haskell --without-go --without-d`

`make`

`make install`

### 8、安装高版本autoconf

从ftp://ftp.gnu.org/gnu/autoconf/ 下载最新版autoconf的源代码（如：ftp://ftp.gnu.org/gnu/autoconf/autoconf-2.69.tar.gz），解压后执行

`./configure`

`make`

`make install`

### 9、安装高版本automake

从ftp://ftp.gnu.org/gnu/automake/ 下载最新版automake的源代码（如：ftp://ftp.gnu.org/gnu/automake/automake-1.13.tar.gz），解压后执行

`./configure`

`make`

`make install`

## 二、安装lspider

### 1.从github下载并编译

`git clone git@git.coding.net:RichardLi/lspider.git`

### 2.编译安装

`./configure --prefix=${HOME}/lspider --with-libevent-include=${HOME}/libevent/include/ --with-libevent-libdir=${HOME}/libevent/lib/ --with-mongo-client-include=${HOME}/mongo-client/include/ --with-mongo-client-libdir=${HOME}/mongo-client/lib/ --with-thrift-include=${HOME}/thrift/include/ --with-thrift-libdir=${HOME}/thrift/lib/`

`make`

`make install`

### 3.配置

`mkdir -p ${HOME}/lspider/conf ${HOME}/lspider/log`

`cp conf.xml.demo ${HOME}/lspider/conf/conf.xml`

`cp log.properties.demo ${HOME}/lspider/conf/log.properties`

`cp maindomain.list.demo ${HOME}/lspider/conf/maindomain.list`

`export LD_LIBRARY_PATH=/home/lichuang/libevent/lib/:/home/lichuang/thrift/lib/:${LD_LIBRARY_PATH}`

### 4.安装xvfb

`yum install xorg-x11-server-Xvfb`

## 三、运行lspider

### 1.确定mysql和mongo已经启动并执行

`xvfb-run --server-args="-screen 0, 1024x768x24" ./bin/lspider`
