#include <stdio.h>
#include <iostream>
#define CURL_STATICLIB
#include "curl/curl.h"
#include <ctime>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "lib/libcurl.lib")
#pragma comment(lib, "Normaliz.lib")

using namespace std;

int main()
{
    clock_t start, end;
    curl_global_init(CURL_GLOBAL_ALL);
    start = clock();
    CURL *curl = curl_easy_init();
    if (curl)
    {
        // const char* url = "https://myip.ipip.net";
        const char *url =
            "http://api.ipstack.com/223.72.74.114?access_key=95cff80ad8e20bd78f08891cb7e0bae9";
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);

        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

        CURLcode curlCode = curl_easy_perform(curl);
        if (CURLE_OK != curlCode)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(curlCode));
        curl_easy_cleanup(curl);
        int http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        end = clock();
        cout << "接口调用耗时:" << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
    }
    else
        printf("Something went wrong\n");
    curl_global_cleanup();
    // system("pause");
    return 0;
}