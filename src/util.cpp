/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Sun 31 May 2015 12:30:57 AM CST
 * Description: 
 ************************************************************************/

#include "util.h"

/**
 * 将一个IP字符串转换为unsigned int
 * @param ip 转换后的数值
 * @retval 非0 转换成功
 * @retval 0 转换失败
 */
int ip_aton (const char *ipstring, unsigned int *ip)
{
	in_addr l_inp;
	int ret;

	if (strcmp (ipstring, "0") == 0)
	{
		*ip = 0;
		return 1;
	}

	ret = inet_pton (AF_INET, ipstring, &l_inp);

	*ip = l_inp.s_addr;

	if (ret <= 0)
	{
		return 0;
	}
	else
	{
		return ret;
	}
}

/**
 * 将整数型IP转换成字符串
 * @param ip 
 * @retval 非0 转换成功
 * @retval 0 转换失败
 */
int ip_ntoa (const unsigned int ip, char * ipstring, int len)
{
	if ( inet_ntop (AF_INET, &ip, ipstring, len) == NULL)
		return 1;
	else 
		return 0;
}

vector<unsigned long> getSockAddr(const char *host)
{
    vector<unsigned long> addrs;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (0 == getaddrinfo(host, NULL, &hints, &res)) {
        if (NULL != res) {
            for (struct addrinfo *rp = res; rp != NULL; rp = rp->ai_next) {
                unsigned long result = ((sockaddr_in*)rp->ai_addr)->sin_addr.s_addr;
                addrs.push_back(result);
            }
            freeaddrinfo(res);
        }
    }
    return addrs;
}

vector<string> getip(const char *host)
{
    vector<string> result;
    vector<unsigned long> addrs = getSockAddr(host);
    if (0 == addrs.size()) {
        fprintf (stderr, "warn: %s:%d getSockAddr %s\n", __FILE__, __LINE__, host);
    } else {
        for (int i = 0; i < addrs.size(); i++) {
            char ipstring[16] = {'\0'};
            if (0 == ip_ntoa(addrs[i], ipstring, 16)) {
                result.push_back(ipstring);
            }
        }
    }
    return result;
}

/**
 * 设置socket为非阻塞
 */
int setnonblocking (int sockfd)
{
	// non-blocking
	int flags = fcntl (sockfd, F_GETFL, 0);

	if (flags == -1)
	{
		fprintf (stderr, "error: %s:%d\n", __FILE__, __LINE__);
		return -2;
	}
	if (fcntl (sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		fprintf (stderr, "error: %s:%d\n", __FILE__, __LINE__);
		return -3;
	}
	return 0;
}

std::string& rtrim(std::string &s)
{
    return s.erase(s.find_last_not_of(" \t\n\r") + 1);
}

std::string& ltrim(std::string &s)
{
    return s.erase(0, s.find_first_not_of(" \t\n\r"));
}

int gzcompress(Bytef *data, uLong ndata,
               Bytef *zdata, uLong *nzdata)
{
    z_stream c_stream;
    int err = 0;

    if(data && ndata > 0) {
        c_stream.zalloc = NULL;
        c_stream.zfree = NULL;
        c_stream.opaque = NULL;
        //只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
        if(deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                        MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
        c_stream.next_in  = data;
        c_stream.avail_in  = ndata;
        c_stream.next_out = zdata;
        c_stream.avail_out  = *nzdata;
        while(c_stream.avail_in != 0 && c_stream.total_out < *nzdata) {
            if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
        }
        if(c_stream.avail_in != 0) return c_stream.avail_in;
        for(;;) {
            if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
            if(err != Z_OK) return -1;
        }
        if(deflateEnd(&c_stream) != Z_OK) return -1;
        *nzdata = c_stream.total_out;
        return 0;
    }
    return -1;
}

int gzdecompress(Byte *zdata, uLong nzdata,
                 Byte *data, uLong *ndata)
{
    int err = 0;
    z_stream d_stream = {0}; /* decompression stream */
    static char dummy_head[2] = {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = NULL;
    d_stream.zfree = NULL;
    d_stream.opaque = NULL;
    d_stream.next_in  = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = data;
    //只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
    if(inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK) {
        return -1;
    }
    while(d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
        if(err != Z_OK) {
            if(err == Z_DATA_ERROR) {
                d_stream.next_in = (Bytef*) dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
                    inflateEnd(&d_stream);
                    return -2;
                }
            } else {
                inflateEnd(&d_stream);
                return -3;
            }
        }
    }
    if(inflateEnd(&d_stream) != Z_OK) return -4;
    *ndata = d_stream.total_out;
    return 0;
}

void hex2dec(const char *hexStr, char *decStr)
{
    int a;
    sscanf(hexStr, "%x", &a);
    sprintf(decStr, "%d", a);
}

bool isNonAsciiUTF8(const char* str, int &len) {
    if (*str == 0) {
        return false;
    }

    unsigned char chr = *str;

    if(chr>=0x80)
    {
        if(chr>=0xFC&&chr<=0xFD)
            len=6;
        else if(chr>=0xF8)
            len=5;
        else if(chr>=0xF0)
            len=4;
        else if(chr>=0xE0)
            len=3;
        else if(chr>=0xC0)
            len=2;
        else
        {
            return false;
        }
    }


    for(int i=1; i<len; i++) {
        chr = *(str+i);
        if (chr == 0) return false;
        if( (chr & 0xC0) != 0x80) {
            return false;
        }
    }

    return true;
}

bool is_utf8(const char* str)
{
    unsigned char chr;

    while(*str) {
        chr = *str;
        if((chr & 0x80) == 0) {
            ++str;
            continue;
        } else {
            int len;
            if (!isNonAsciiUTF8(str, len)) {
                return false;
            } else {
                str += len;
                continue;
            }
        }
    }

    return true;
}

bool to_utf8(const char* source_str, const size_t len, std::string & target_str)
{
    if (is_utf8(source_str)) {
        target_str = source_str;
        return true;
    } else {
        iconv_t cd = iconv_open("UTF-8", "GB18030");
        if ((iconv_t)-1 != cd) {
            size_t inlen = len;
            char *in = const_cast<char*>(source_str);
            size_t outlen = 4 * inlen;
            char *out = (char*)malloc(outlen);
            memset(out, '\0', outlen);
            if (NULL == out) {
                iconv_close(cd);
                return false;
            }
            char *pout = out;
            if ((size_t)-1 == iconv(cd, &in, &inlen, &pout, &outlen)) {
                iconv_close(cd);
                free(out);
                return false;
            } else {
                target_str = std::string(out);
            }
            free(out);
            iconv_close(cd);
            return true;
        } else {
            return false;
        }
    }
}

bool to_utf8(std::string &str)
{
    return to_utf8(str.c_str(), str.length(), str);
}
