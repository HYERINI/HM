#include <iostream>

#pragma comment(lib, "crypt32.lib" )
#pragma comment(lib, "ws2_32.lib") //curl 사용시 추가 해야 링크 오류가 안남
#pragma comment(lib, "wldap32.lib") //curl 사용시 추가 해야 링크 오류가 안남

#include <curl/curl.h>
#include <json/json.h>

struct memory {
    char* response;
    size_t size;
};

static size_t cb(void* data, size_t size, size_t nmemb, void* clientp)
{
    size_t realsize = size * nmemb;
    struct memory* mem = (struct memory*)clientp;

    char* ptr = (char*)realloc(mem->response, mem->size + realsize + 1);
    if (ptr == NULL)
        return 0;  /* out of memory! */

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}



int example() {
    struct memory chunk = { 0 };
    CURL* curl;
    CURLcode res;

    std::string strTargetURL;
    std::string strResourceJSON;

    struct curl_slist* headerlist = nullptr;
    headerlist = curl_slist_append(headerlist, "Content-Type: application/json");

    strTargetURL = "http://127.0.0.1:8080/server/json";
    strResourceJSON = "{\"data\": {\"speed\": \"why this run?\", \"path\": \"\"}}";

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, strTargetURL.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strResourceJSON.c_str());

        // 결과 기록
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        curl_slist_free_all(headerlist);

        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return false;
        }

        std::cout << "------------Result" << std::endl;
        std::cout << chunk.response << std::endl;

        return true;
    }
    return false;
}