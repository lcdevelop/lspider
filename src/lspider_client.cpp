/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:56:17 2015
 * Description: 
 ************************************************************************/

#include <string>
#include <fstream>
#include <sstream>

#include <readline/readline.h>
#include <readline/history.h>
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
#include "util.h"

using std::string;
using std::istringstream;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace lspider;

int crawl(string file)
{
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
        std::fstream fin(file.c_str());
        std::string ReadLine;
        while(getline(fin,ReadLine)) {
            printf("%s\n", ReadLine.c_str());
            u.__set_str(ReadLine);
            u.__set_hub(true);
            client.request(u);
        }
    }


    transport->close();
}

bool getline(const char* prompt, string& line)
{
	char* line_buffer = readline(prompt);
	if (line_buffer == NULL)
	{
		return false;
	}
	add_history(line_buffer);
	line = line_buffer;
	free(line_buffer);
	return true;
}

int processCmd()
{
    boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    try {
        transport->open();
    } catch (apache::thrift::transport::TTransportException e) {
        return -1;
    }


    CrawlServiceClient client(protocol);
    string response;
    string cmd_line;
    string cmd;
    const char* prompt = ">";
    while (getline(prompt, cmd_line))
    {
        istringstream istr(cmd_line);
        istr >> cmd;
        try {
            client.exec_cmd(response, rtrim(ltrim(cmd_line)));
        } catch (apache::thrift::transport::TTransportException e) {
            fprintf(stderr, "TTransportException : %s\n", e.what());
            break;
        }
        fprintf(stdout, "%s\n", response.c_str());
    }

    transport->close();
}

void printUsage(char *program)
{
    fprintf(stderr, "Usage: %s -tfh\n");
    fprintf(stderr, "    -t type(crawl|cmd)\n");
    fprintf(stderr, "    -f urllist            use when type is crawl\n");
    fprintf(stderr, "    -h                    help message\n");
}

int main(int argc, char *argv[])
{
    char c;
    string type;
    string file;
    while ((c = getopt(argc, argv, "t:f:h")) != -1)
    {
        switch (c)
        {
        case 't':
            type = optarg;
            break;
        case 'f':
            file = optarg;
            break;
        case 'h':
        default:
            printUsage(argv[0]);
            break;
        }
    }

    if ("crawl" == type) {
        if (file != "") {
            crawl(file);
        } else {
            return -1;
        }
    } else if ("cmd" == type) {
        processCmd();
    }
    return 0;
}
