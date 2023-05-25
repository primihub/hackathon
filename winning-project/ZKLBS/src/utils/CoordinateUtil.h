#ifndef _COORDINATEUTIL_H
#define _COORDINATEUTIL_H

#include "curl/curl.h"
#include "json/json.h"
#include <iostream>
#include <string.h>
#include "IpUtil.h"

class CoordinateUtil
{
public:
    std::pair<long int, long int> getCoordinate();

private:
    static size_t write_data(void *ptr, size_t size, size_t nmemb, char *stream)
    {
        if (strlen((char *)stream) + strlen((char *)ptr) > 999999)
            return 0;
        strcat(stream, (char *)ptr);
        return size * nmemb;
    }
    std::pair<long int, long int> ip2Coordinate(std::string ip)
    {
        static char str[10000000];
        strcpy(str, "");
        CURL *curl = curl_easy_init();
        std::cout << "From coordinateUtil ip: " << ip << std::endl;

        std::string url = "http://api.ipstack.com/" + ip +
                          "?access_key=95cff80ad8e20bd78f08891cb7e0bae9";
        std::cout << "From coordinateUtil ur;: " << url << std::endl;
        const char *url_char = url.c_str();

        CURLcode res_ip;
        curl_easy_setopt(curl, CURLOPT_URL, url_char);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, str);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        Json::Reader Reader;
        Json::Value DevJson;
        Reader.parse(str, DevJson);
        int lat = DevJson["latitude"].asInt();
        int lon = DevJson["longitude"].asInt();
        std::pair<long int, long int> coordinate = std::make_pair(lon, lat);
        return coordinate;
    }
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
    std::string getIpAddress()
    {
        static char str[10000000];
        strcpy(str, "");
        CURL *curl = curl_easy_init();
        if (curl)
        {
            CURLcode res_ip;
            curl_easy_setopt(curl, CURLOPT_URL, "https://myip.ipip.net");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, str);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            std::string ip;
            ip = get_ip_from_endpoint(str);
            return ip;
        }
        return "";
    }
};

#endif