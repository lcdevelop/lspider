/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Thu May 14 15:58:37 2015
 * Description: 
 ************************************************************************/

#ifndef __URL_TOOLS_H__
#define __URL_TOOLS_H__

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_URL_LEN 1024
#define MAX_SITE_LEN 256
#define MAX_PATH_LEN 1024
#define MAX_PORT_LEN 7

int delete_inter(char * str);

int parse_url(const char *input, char *site,size_t site_size,
                     char *port, size_t port_size,char *path, size_t max_path_size);
uint64_t MurmurHash64A(const void * key, int len, unsigned int seed);

uint64_t get_url_sign64(const char *url);

int normalize_url(const char* url, char* buf,size_t buf_size);
int isnormalized_url(const char *url);

const char* fetch_maindomain(const char* site, char* trunk, int trunk_size,
                                   bool recoveryMode = true);

bool fetch_url_maindomain(const char *url, char *maindomain, int len);

#endif
