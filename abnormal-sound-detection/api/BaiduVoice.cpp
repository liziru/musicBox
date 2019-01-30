#include "TokenHolder.h"
#include "BaiduVoice.h"
#include "log.h"
#include <curl/curl.h>
#include <memory.h>

const string BaiduVoice::API_TTS_URL = "http://tsn.baidu.com/text2audio";
const string BaiduVoice::cuid = "1234567JAVA";

// 发音人选择, 0为普通女声，1为普通男生，3为情感合成-度逍遥，4为情感合成-度丫丫，默认为普通女声
const int BaiduVoice::per = 0;
// 语速，取值0-9，默认为5中语速
const int BaiduVoice::spd = 5;
// 音调，取值0-9，默认为5中语调
const int BaiduVoice::pit = 5;
// 音量，取值0-9，默认为5中音量
const int BaiduVoice::vol = 5;
const int BaiduVoice::MAX_HEADER_VALUE_LEN = 100;

BaiduVoice::BaiduVoice()
{
}
BaiduVoice::~BaiduVoice()
{
}
string BaiduVoice::voiceTtsUrl(string text)
{
    TokenHolder *holder = new TokenHolder(TokenHolder::TWIRLING_APIKEY, TokenHolder::TWIRLING_SECRETKEY, TokenHolder::ASR_SCOPE);
    holder->resfresh();
    string token = holder->getToken();
    CURL *curl = curl_easy_init();                                       // 需要释放
    char *cuidTmp = curl_easy_escape(curl, cuid.c_str(), cuid.length()); // 需要释放
    char *textTmp = curl_easy_escape(curl, text.c_str(), text.length()); // 需要释放
    string url2 = API_TTS_URL + "?tex=" + string(textTmp);
    url2 += "&per=" + per;
    url2 += "&spd=" + spd;
    url2 += "&pit=" + pit;
    url2 += "&vol=" + vol;
    url2 += "&cuid=" + string(cuidTmp);
    url2 += "&tok=" + token;
    url2 += "&lan=zh&ctp=1";
    macroFunc(url2.c_str());
    curl_easy_cleanup(curl);
    free(cuidTmp);
    free(textTmp);
    delete holder;
    return url2;
}

TtsResult *BaiduVoice::voiceTts(string text)
{
    string url2 = voiceTtsUrl(text);
    TtsResult *ttsResult = new TtsResult();

    CURL *curl = curl_easy_init(); // 需要释放
    curl_easy_setopt(curl, CURLOPT_URL, url2.c_str());
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);              // 连接5s超时
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);                    // 整体请求60s超时
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback); // 检查头部
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, ttsResult);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefuncData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, ttsResult);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);

    CURLcode res_curl = curl_easy_perform(curl);
    RETURN_CODE res = RETURN_OK;
    if (res_curl != CURLE_OK)
    {
        // curl 失败
        macroFuncVargs("perform curl error:%d, %s.", res, curl_easy_strerror(res_curl));
        res = ERROR_TTS_CURL;
    }
    // free(result.fp);
    curl_easy_cleanup(curl);
    return ttsResult;
}

RETURN_CODE BaiduVoice::search_header(const char *buffer, size_t len, const char *key, char *value)
{
    size_t len_key = strlen(key);
    char header_key[len_key + 1];
    header_key[len_key] = '\0';
    memcpy(header_key, buffer, len_key);
    if (strcasecmp(key, header_key) == 0 && (len - len_key) < MAX_HEADER_VALUE_LEN)
    {
        int len_value = len - len_key;
        value[len_value - 1] = '\0';
        memcpy(value, buffer + len_key + 1, len_value);
        return RETURN_OK;
    }
    return RETURN_FAIL;
}

size_t BaiduVoice::headerCallback(char *buffer, size_t size, size_t nitems, TtsResult *result)
{
    size_t len = size * nitems;
    char key[] = "Content-Type";
    char value[MAX_HEADER_VALUE_LEN];
    if (search_header(buffer, len, key, value) == RETURN_OK)
    {
        if (strstr(value, "mp3") != NULL)
        {
            result->setIsError(false);
        }
        else
        {
            result->setIsError(true);
            result->setErrorMsg(string(key) + ":" + string(value));
            macroFuncVargs("Server return ERROR, %s : %s", key, value);
        }
    }
    return len;
}
size_t BaiduVoice::writefuncData(void *ptr, size_t size, size_t nmemb, TtsResult *result)
{
    if (!result->getIsError())
    {
        if (result->getTtsVoiceFile() == NULL)
        {
            FILE *tmp = fopen(result->getTtsVoiceFilename().c_str(), "w+");
            result->setTtsVoiceFile(tmp);
            if (result->getTtsVoiceFile() == NULL)
            {
                result->setIsError(true);
                result->setErrorMsg(result->getTtsVoiceFilename() + " cannot be opened");
                macroFuncVargs("%s cannot be opened", result->getTtsVoiceFilename().c_str());
                return 0;
            }
        }
    }
    else
    {
        macroFunc("Server return ERROR");
        return 0;
    }
    macroFuncVargs("Data will be write into %s in the current directory", result->getTtsVoiceFilename().c_str());
    result->setIsError(false);
    int count = 0;
    count = fwrite(ptr, size, nmemb, result->getTtsVoiceFile());
    fflush(result->getTtsVoiceFile());
    return count;
}