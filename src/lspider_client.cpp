/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:56:17 2015
 * Description: 
 ************************************************************************/

#include <fstream>

#include <netdb.h>
#include <sys/socket.h>
#include <string>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include "CrawlService.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace lcrawler;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s urllist\n", argv[0]);
        return -1;
    }

    boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    try {
        transport->open();
    } catch (apache::thrift::transport::TTransportException e) {
        return -1;
    }


    CrawlServiceClient client(protocol);

    IUrl u;

    for (int i = 0; i < 1; i++) {
        std::fstream fin(argv[1]);
        std::string ReadLine;
        while(getline(fin,ReadLine)) {
            printf("%s\n", ReadLine.c_str());
            u.__set_str(ReadLine);
            u.__set_hub(true);
            client.request(u);
        }
    }


    //sleep(1);
    transport->close();
    return 0;
}
