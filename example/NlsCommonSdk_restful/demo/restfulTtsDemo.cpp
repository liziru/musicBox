#ifdef _WIN32
#include <Windows.h>
#endif

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>

#include "curl/curl.h"
#include "json/json.h"

using namespace std;

#ifdef _WIN32
string GBKToUTF8(const string &strGBK)
{
    string strOutUTF8 = "";
    WCHAR *str1;

    int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);

    str1 = new WCHAR[n];

    MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);

    n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);

    char *str2 = new char[n];
    WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
    strOutUTF8 = str2;

    delete[] str1;
    str1 = NULL;
    delete[] str2;
    str2 = NULL;

    return strOutUTF8;
}
#endif

void stringReplace(string &src, const string &s1, const string &s2)
{
    string::size_type pos = 0;
    while ((pos = src.find(s1, pos)) != string::npos)
    {
        src.replace(pos, s1.length(), s2);
        pos += s2.length();
    }
}

string urlEncode(const string &src)
{
    CURL *curl = curl_easy_init();
    char *output = curl_easy_escape(curl, src.c_str(), src.size());
    string result(output);

    curl_free(output);
    curl_easy_cleanup(curl);

    return result;
}

size_t responseHeadersCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    map<string, string> *headers = (map<string, string> *)userdata;
    string line((char *)ptr);
    string::size_type pos = line.find(':');
    if (pos != line.npos)
    {
        string name = line.substr(0, pos);
        string value = line.substr(pos + 2);
        size_t p = 0;
        if ((p = value.rfind('\r')) != value.npos)
        {
            value = value.substr(0, p);
        }

        headers->insert(make_pair(name, value));
    }
    return size * nmemb;
}

size_t responseBodyCallback(void *ptr, size_t size, size_t nmemb, void *userData)
{

    size_t len = size * nmemb;
    char *pBuf = (char *)ptr;
    string *bodyContent = (string *)userData;
    (*bodyContent).append(string(pBuf, pBuf + len));

    return len;
}

int processGETRequest(string appKey, string token, string text,
                      string audioSaveFile, string format, int sampleRate)
{
    CURL *curl = NULL;
    CURLcode res;

    curl = curl_easy_init();
    if (curl == NULL)
    {
        return -1;
    }

    string url = "https://nls-gateway.cn-shanghai.aliyuncs.com/stream/v1/tts";
    /**
     * 设置HTTPS URL请求参数
     */
    ostringstream oss;
    oss << url;
    oss << "?appkey=" << appKey;
    oss << "&token=" << token;
    oss << "&text=" << text;
    oss << "&format=" << format;
    oss << "&sample_rate=" << sampleRate;

    // voice 发音人，可选，默认是xiaoyun
    // oss << "&voice=" << "xiaoyun";
    // volume 音量，范围是0~100，可选，默认50
    // oss << "&volume=" << 50;
    // speech_rate 语速，范围是-500~500，可选，默认是0
    // oss << "&speech_rate=" << 0;
    // pitch_rate 语调，范围是-500~500，可选，默认是0
    // oss << "&pitch_rate=" << 0;

    string request = oss.str();
    cout << "request str: " << request << endl;

    curl_easy_setopt(curl, CURLOPT_URL, request.c_str());

    /**
     * 设置获取响应的HTTPS Headers回调函数
     */
    map<string, string> responseHeaders;
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, responseHeadersCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeaders);

    /**
     * 设置获取响应的HTTPS Body回调函数
     */
    string bodyContent = "";
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseBodyCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bodyContent);

    /**
     * 发送HTTPS GET请求
     */
    res = curl_easy_perform(curl);

    /**
     * 释放资源
     */
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << endl;
        return -1;
    }

    /**
     * 处理服务端返回的响应
     */
    map<string, string>::iterator it = responseHeaders.find("Content-Type");
    if (it != responseHeaders.end() && it->second.compare("audio/mpeg") == 0)
    {
        ofstream fs;
        fs.open(audioSaveFile.c_str(), ios::out | ios::binary);
        if (!fs.is_open())
        {
            cout << "The audio save file can not open!";
            return -1;
        }
        fs.write(bodyContent.c_str(), bodyContent.size());
        fs.close();

        cout << "The GET request succeed!" << endl;
    }
    else
    {
        cout << "The GET request failed: " + bodyContent << endl;
        return -1;
    }

    return 0;
}

int processPOSTRequest(string appKey, string token, string text,
                       string audioSaveFile, string format, int sampleRate)
{
    CURL *curl = NULL;
    CURLcode res;

    curl = curl_easy_init();
    if (curl == NULL)
    {
        return -1;
    }

    string url = "http://nls-gateway.cn-shanghai.aliyuncs.com/stream/v1/tts";

    /**
     * 设置HTTPS POST URL
     */
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    /**
     * 设置HTTPS POST请求头部
     */
    struct curl_slist *headers = NULL;
    // Content-Type
    curl_slist_append(headers, "Content-Type:application/json");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    /**
     * 设置HTTPS POST请求体
     */
    Json::Value root;
    Json::FastWriter writer;
    root["appkey"] = appKey;
    root["token"] = token;
    root["text"] = text;
    root["format"] = format;
    root["sample_rate"] = sampleRate;

    // voice 发音人，可选，默认是xiaoyun
    // root["voice"] = "xiaoyun";
    // volume 音量，范围是0~100，可选，默认50
    // root["volume"] = 50;
    // speech_rate 语速，范围是-500~500，可选，默认是0
    // root["speech_rate"] = 0;
    // pitch_rate 语调，范围是-500~500，可选，默认是0
    // root["pitch_rate"] = 0;

    string task = writer.write(root);
    cout << "POST request Body: " << task << endl;

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, task.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, task.length());

    /**
     * 设置获取响应的HTTPS Headers回调函数
     */
    map<string, string> responseHeaders;
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, responseHeadersCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeaders);

    /**
     * 设置获取响应的HTTPS Body回调函数
     */
    string bodyContent = "";
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseBodyCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bodyContent);

    /**
     * 发送HTTPS POST请求
     */
    res = curl_easy_perform(curl);

    /**
     * 释放资源
     */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << endl;
        return -1;
    }

    /**
     * 处理服务端返回的响应
     */
    map<string, string>::iterator it = responseHeaders.find("Content-Type");
    if (it != responseHeaders.end() && it->second.compare("audio/mpeg") == 0)
    {
        ofstream fs;
        fs.open(audioSaveFile.c_str(), ios::out | ios::binary);
        if (!fs.is_open())
        {
            cout << "The audio save file can not open!";
            return -1;
        }
        fs.write(bodyContent.c_str(), bodyContent.size());
        fs.close();

        cout << "The POST request succeed!" << endl;
    }
    else
    {
        cout << "The POST request failed: " + bodyContent << endl;
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cerr << "params is not valid. Usage: ./demo your_token your_appkey" << endl;
        return -1;
    }

    string token = argv[1];
    string appKey = argv[2];

    string text = "嗯呢，我在呢";
#ifdef _WIN32
    text = GBKToUTF8(text);
#endif

    string textUrlEncode = urlEncode(text);
    cout << "urlEncode: " << textUrlEncode << endl;
    stringReplace(textUrlEncode, "+", "%20");
    stringReplace(textUrlEncode, "*", "%2A");
    stringReplace(textUrlEncode, "%7E", "~");

    string audioSaveFile = "syAudio.wav";
    string format = "wav";
    int sampleRate = 16000;

    // 全局只初始化一次
    curl_global_init(CURL_GLOBAL_ALL);

    processGETRequest(appKey, token, textUrlEncode, audioSaveFile, format, sampleRate);

    //processPOSTRequest(appKey, token, text, audioSaveFile, format, sampleRate);

    curl_global_cleanup();

    return 0;
}
