#ifndef _IPUTIL_H
#define _IPUTIL_H

#include "curl/curl.h"
#include <iostream>
#include <string.h>
#include <regex>

class IpUtil
{
public:
    std::string getIpAddress();

private:
    std::string get_ip_from_endpoint(const std::string &endpoint)
    {
        std::regex pattern("(\\d+.){3}\\d+");
        std::smatch result;
        if (regex_search(endpoint, result, pattern))
        {
            return result.str();
        }
        return "";
    }

    static size_t write_data(void *ptr, size_t size, size_t nmemb, char *stream)
    {
        if (strlen((char *)stream) + strlen((char *)ptr) > 999999)
            return 0;
        strcat(stream, (char *)ptr);
        return size * nmemb;
    }
};

#endif