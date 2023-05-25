#include "IpUtil.h"

std::string IpUtil::getIpAddress()
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