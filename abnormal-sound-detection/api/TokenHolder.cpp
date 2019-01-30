#include "TokenHolder.h"
#include <stdio.h>
#include "curl/curl.h"
#include "../three/rapidjson/document.h"
#include "../three/rapidjson/writer.h"
#include "../three/rapidjson/stringbuffer.h"
#include "log.h"
using namespace rapidjson;

const string TokenHolder::ASR_SCOPE = "audio_voice_assistant_get";
const string TokenHolder::TTS_SCOPE = "audio_tts_post";
const string TokenHolder::TWIRLING_APIKEY = "RpH6m0g1UY9pAPDoG8RE2IUj";
const string TokenHolder::TWIRLING_SECRETKEY = "109729cbd668b71fd31afc296693f0e0";
// const string TokenHolder::TWIRLING_APIKEY = "4E1BG9lTnlSeIf1NQFlrSq6h";
// const string TokenHolder::TWIRLING_SECRETKEY = "544ca4657ba8002e3dea3ac2f5fdd241";
const string TokenHolder::url = "http://openapi.baidu.com/oauth/2.0/token";

TokenHolder::TokenHolder(string apiKey, string secretKey, string scope)
    : apiKey(apiKey), secretKey(secretKey), scope(scope), token(""), expiresAt(0)
{
}

string TokenHolder::getToken()
{
    return token;
}

long TokenHolder::getExpiresAt()
{
    return expiresAt;
}

size_t TokenHolder::writefunc(void *ptr, size_t size, size_t nmemb, char **result)
{
    size_t result_len = size * nmemb;
    if (*result == NULL)
    {
        *result = (char *)malloc(result_len + 1);
    }
    else
    {
        *result = (char *)realloc(*result, result_len + 1);
    }
    if (*result == NULL)
    {
        macroFunc("realloc failure!");
        return 1;
    }
    memcpy(*result, ptr, result_len);
    (*result)[result_len] = '\0';
    // printf("buffer: %s\n", *result);
    return result_len;
}
void TokenHolder::resfresh()
{
    string getTokenURL = url + "?grant_type=client_credentials" + "&client_id=" + apiKey + "&client_secret=" + secretKey;

    // 打印的url出来放到浏览器内可以复现
    macroFuncVargs("token url->:%s", getTokenURL.c_str());

    char *response = NULL;
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_URL, getTokenURL.c_str()); // 注意返回值判读
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60); // 60s超时
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
    CURLcode res_curl = curl_easy_perform(curl);
    RETURN_CODE res = RETURN_OK;
    if (res_curl != CURLE_OK)
    {
        macroFuncVargs("perform curl error:%d, %s.", res, curl_easy_strerror(res_curl));
        res = ERROR_TOKEN_CURL;
    }
    else
    {
        res = parseToken(response); // 解析token，结果保存在token里
    }
    free(response);
    curl_easy_cleanup(curl);
}

// 解析json，获取某个key对应的value
RETURN_CODE TokenHolder::parseToken(const char *json)
{
    Document d;
    d.Parse(json);
    Value &tokenVal = d["access_token"];
    Value &expiresVal = d["expires_in"];
    this->expiresAt = expiresVal.GetInt64();
    this->token = string(tokenVal.GetString());
    return RETURN_OK;
}
