// #include "SemanticsAnalysis.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
// #include <vector>
#include "curl/curl.h"
#include "Ali_RestfulASR.h"
#include "util.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "AudioPreprocessDispatcher.h"
#include "NeteaseMusicService.h"

using namespace std;
using namespace rapidjson;

#define NETEASE_LANUCHER_DEBUG

const string NeteaseMusicService::URL = "http://localhost:3000";
const string NeteaseMusicService::COOKIE_FILE = "/home/pi/cookies.txt";

NeteaseMusicService::NeteaseMusicService()
{
    wakeupEvent = new WakeupEvent();
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    fpr = NULL;
    uid = 0;
//start netease server.
#if 0
    if (!startNeteaseClient())
    {
        macroFuncVargs("FATAL ERROR:failed to start Netease Client.");
        exit(1);
    }
    else
    {
        macroFunc("netease clent started.");
    }
#endif
    // 全局只初始化一次
    curl_global_init(CURL_GLOBAL_ALL);

    //login
    if (login())
    {
        macroFuncVargs("\nATTENTION:login successfully,srRes:%s", loginRes.c_str());
    }
    else
    {
        macroFuncVargs("\nFATAL ERROR: login failed.");
        // exit(1);
    }

    //login/statu, and process json to get userId
    // showLoginStatus();
}

NeteaseMusicService::~NeteaseMusicService()
{
    delete wakeupEvent;
    wakeupEvent = NULL;
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    isRun = false;
    curl_global_cleanup(); //
    if (fpr)
    {
        pclose(fpr);
        fpr = NULL;
    }
}

bool NeteaseMusicService::startNeteaseClient()
{
    vector<int> res = getProcessPid("node");
    if (res.empty())
    {
        // string cmd = "node /home/pi/NeteaseCloudMusicApi-master/app.js 2>&1 "; // node /home/pi/NeteaseCloudMusicApi/app.js
        string cmd = "node /home/pi/NeteaseCloudMusicApi/app.js 2>&1 "; // node /home/pi/NeteaseCloudMusicApi/app.js
        char readbuf[1024] = {};
        bool ret = true;
        fpr = popen(cmd.c_str(), "r");
        if (!fpr)
        {
            fpr = NULL;
            macroFuncVargs("ERROR:failed to popen %s,reason:%s", cmd.c_str(), strerror(errno));
            return false;
        }

        res = getProcessPid("node");
        if (!res.empty())
        {
#if defined(NETEASE_LANUCHER_DEBUG)
            macroFuncVargs("Netease Client starts successfully,freadbuf %s", readbuf);
#endif // NETEASE_LANUCHER_DEBUG
        }
        else
        {
#if defined(NETEASE_LANUCHER_DEBUG)
            macroFuncVargs("ERROR: failed to start Netease Client,freadbuf %s", readbuf);
#endif // NETEASE_LANUCHER_DEBUG
            ret = false;
        }
        return ret;
    }
    else
    {
        return true;
    }
}

bool NeteaseMusicService::login()
{
    // string requ = URL + "/login?email=youmakemewhole@163.com&password=17610021721lp";
    string requ = URL + "/login/cellphone?phone=17610021721&password=17610021721lp" + "&timestamp=" + to_string(getCurrUnixtime());
    if (!loginRequestProcess(requ, &loginRes))
    {
        macroFuncVargs("ERROR: requestProcess failed.");
        return false;
    }
    else
    {
        //process json to get userId
        Document dm;
        dm.Parse(loginRes.c_str());
        if (dm.HasParseError())
        {
            printf("FATAL ERROR: GetParseError( %d ), json:%s\n", dm.GetParseError(), loginRes.c_str());
            return false;
        }

        if (dm.HasMember("loginType"))
        {
            Value &isLogin = dm["loginType"];
            if (isLogin.IsInt())
            {
                if (0 == isLogin.GetInt())
                {
                    return false;
                }
            }
        }

        if (dm.HasMember("account"))
        {
            Value &accInfo = dm["account"];
            if (accInfo.IsObject())
            {
                Value &userId = accInfo["id"];
                uid = userId.GetInt();
                macroFuncVargs("userId:%d", uid);
            }
        }
    }

    return uid != 0;
}

string NeteaseMusicService::showLoginStatus()
{
    string requ = URL + "/login/status";
    string srResult;
    if (requestProcess(requ, &srResult) < 0)
    {
        // exit(1);
        return "";
    }
    else
    {
        return srResult;
    }

    return 0;
}

string NeteaseMusicService::getMusicUrlById(long id)
{
    ///song/url?id=33894312
    string requ = URL + "/song/url?id=" + to_string(id);
    string res = "";
    if (!requestProcess(requ, &res))
    {
        macroFuncVargs("FATAL ERROR: get netease-music-url failed.");
        return "";
    }

    return res;
}

string NeteaseMusicService::getNeteaseRecommandList()
{
    string requ = URL + "/recommend/songs";
    string recommandRes = "";
    if (!requestProcessWithCookie(requ, &recommandRes))
    {
        macroFuncVargs("FATAL ERROR: get netease-recommanded list failed.");
        return "";
    }
    else
    {
        // printf("getNeteaseRecommandList is returned ,con:%s\n\n", recommandRes.c_str());
        Document dm;
        dm.Parse(recommandRes.c_str());
        if (dm.HasParseError())
        {
            printf("FATAL ERROR: GetParseError( %d ), recommandRes-json:%s\n", dm.GetParseError(), recommandRes.c_str());
            return "";
        }
        Document::AllocatorType &allocator = dm.GetAllocator();
        StringBuffer buff;
        Writer<StringBuffer> writer(buff);

        if (dm.HasMember("recommend"))
        {
            Value &recommend = dm["recommend"];
            // Value key(kArrayType);
            // key = bytes.GetArray();
            Value tmp;
            if (recommend.IsArray())
            {
                for (SizeType i = 0; i < recommend.Size(); i++) // 使用 SizeType 而不是 size_t
                {
                    Value &mId = recommend[i]["id"];
                    string mUrl = getMusicUrlById(mId.GetInt());
                    if (mUrl.length() <= 0)
                    {
                        macroFuncVargs("ERROR:failed to getMusicUrlById");
                        return "";
                    }

                    tmp.SetString(mUrl.c_str(), mUrl.length(), allocator);
                    recommend[i].AddMember("musicUrl", tmp, allocator);
                }
            }
        }
        dm.Accept(writer);
        return buff.GetString();
    }
    return "";
}
void *NeteaseMusicService::neteaseProcess_thread(void *p)
{
    pthread_detach(pthread_self());
    NeteaseMusicService *neteaseService = (NeteaseMusicService *)p;
    neteaseService->isRun = true;

    while (neteaseService->isRun)
    {
        pthread_mutex_lock(&neteaseService->mutex);
        pthread_cond_wait(&neteaseService->cond, &neteaseService->mutex);
        pthread_mutex_unlock(&neteaseService->mutex);
        macroFuncVargs("asr res(%s)", neteaseService->wakeupEvent->getAsrRes().c_str());
        //process
        string recommandRes = neteaseService->getNeteaseRecommandList();
        if (recommandRes.length() <= 0)
        {
            macroFuncVargs("ERROR: failed to getNeteaseRecommandList(%s)", recommandRes.c_str());
            exit(1);
        }
        // printf("recommandRes is returned ,con:%s\n\n", recommandRes.c_str());

        // string loginStatus = neteaseService->showLoginStatus();
        // macroFuncVargs("showLoginStatus:%s", loginStatus.c_str());

        neteaseService->wakeupEvent->setLanJsonTask(recommandRes);
        neteaseService->wakeupEvent->setLanType(1); //play recommander-list.
        list<WakeupListenner *>::iterator it = neteaseService->neteaseLanucherListeners.begin();
        for (; it != neteaseService->neteaseLanucherListeners.end(); ++it)
        {
            // (*it)->onWakeup(1, recommandRes);
            (*it)->onWakeup(neteaseService->wakeupEvent);
        }
    }
}

void NeteaseMusicService::onWakeup(WakeupEvent *wuEvent)
{
    wakeupEvent->setAsrRes(wuEvent->getAsrRes());
    pthread_cond_signal(&cond);
}
void NeteaseMusicService::run()
{
    if (!this->isRun)
    {
        pthread_create(&th_netease, NULL, neteaseProcess_thread, (void *)this);
    }
}

void NeteaseMusicService::addNeteaseLanucherListeners(WakeupListenner *listenner)
{
    this->neteaseLanucherListeners.push_back(listenner);
}

void NeteaseMusicService::removeNeteaseLanucherListeners(WakeupListenner *listenner)
{
    this->neteaseLanucherListeners.remove(listenner);
}

bool NeteaseMusicService::requestProcess(string request, string *srResult)
{
    CURL *curl = NULL;
    CURLcode res;

    curl = curl_easy_init();
    if (curl == NULL)
    {
        macroFuncVargs("ERROR:curl_easy_init failed,reason:%s", strerror(errno));
        return false;
    }
#ifndef NETEASE__LANUCHER_DEBUG
    macroFuncVargs("login url:%s", request.c_str());
#endif //NETEASE__LANUCHER_DEBUG
    /**
    * 设置HTTP请求行
    */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_URL, request.c_str());
    // curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "/home/pi/cookies.txt");
    // /**
    // * 设置HTTP请求头部
    // */
    // struct curl_slist *headers = NULL;
    // // token
    // string X_NLS_Token = "X-NLS-Token:";
    // // X_NLS_Token += token;
    // headers = curl_slist_append(headers, X_NLS_Token.c_str());
    // // Content-Type
    // headers = curl_slist_append(headers, "Content-Type:application/octet-stream");
    // // Content-Length
    // string content_Length = "Content-Length:";
    // ostringstream oss;
    // oss << content_Length << audioData.length();
    // content_Length = oss.str();
    // headers = curl_slist_append(headers, content_Length.c_str());

    // curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // /**
    // * 设置HTTP请求数据
    // */
    // curl_easy_setopt(curl, CURLOPT_POSTFIELDS, audioData.c_str());
    // curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, audioData.length());

    /**
    * 设置HTTP请求的响应回调函数
    */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Ali_RestfulASR::responseCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, srResult);

    /**
    * 发送HTTP请求
    */
    res = curl_easy_perform(curl);

    // 释放资源
    // curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << endl;
        return false;
    }
    return true;
}

bool NeteaseMusicService::loginRequestProcess(string request, string *srResult)
{
    CURL *curl = NULL;
    CURLcode res;

    curl = curl_easy_init();
    if (curl == NULL)
    {
        macroFuncVargs("ERROR:curl_easy_init failed,reason:%s", strerror(errno));
        return false;
    }
#ifndef NETEASE__LANUCHER_DEBUG
    macroFuncVargs("loginRequestProcess url:%s", request.c_str());
#endif //NETEASE__LANUCHER_DEBUG
    /**
    * 设置HTTP请求行
    */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_URL, request.c_str());
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, COOKIE_FILE.c_str());
    // /**
    // * 设置HTTP请求头部
    // */
    // struct curl_slist *headers = NULL;
    // // token
    // string X_NLS_Token = "X-NLS-Token:";
    // // X_NLS_Token += token;
    // headers = curl_slist_append(headers, X_NLS_Token.c_str());
    // // Content-Type
    // headers = curl_slist_append(headers, "Content-Type:application/octet-stream");
    // // Content-Length
    // string content_Length = "Content-Length:";
    // ostringstream oss;
    // oss << content_Length << audioData.length();
    // content_Length = oss.str();
    // headers = curl_slist_append(headers, content_Length.c_str());

    // curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // /**
    // * 设置HTTP请求数据
    // */
    // curl_easy_setopt(curl, CURLOPT_POSTFIELDS, audioData.c_str());
    // curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, audioData.length());

    /**
    * 设置HTTP请求的响应回调函数
    */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Ali_RestfulASR::responseCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, srResult);

    /**
    * 发送HTTP请求
    */
    res = curl_easy_perform(curl);

    // 释放资源
    // curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << endl;
        return false;
    }
    return true;
}

bool NeteaseMusicService::requestProcessWithCookie(string request, string *srResult)
{
    CURL *curl = NULL;
    CURLcode res;

    curl = curl_easy_init();
    if (curl == NULL)
    {
        macroFuncVargs("ERROR:curl_easy_init failed,reason:%s", strerror(errno));
        return false;
    }
#ifndef NETEASE__LANUCHER_DEBUG
    macroFuncVargs("login url:%s", request.c_str());
#endif //NETEASE__LANUCHER_DEBUG
    /**
    * 设置HTTP请求行
    */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_URL, request.c_str());
    // curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "/home/pi/cookies.txt");
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, COOKIE_FILE.c_str());
    // /**
    // * 设置HTTP请求头部
    // */
    // struct curl_slist *headers = NULL;
    // // token
    // string X_NLS_Token = "X-NLS-Token:";
    // // X_NLS_Token += token;
    // headers = curl_slist_append(headers, X_NLS_Token.c_str());
    // // Content-Type
    // headers = curl_slist_append(headers, "Content-Type:application/octet-stream");
    // // Content-Length
    // string content_Length = "Content-Length:";
    // ostringstream oss;
    // oss << content_Length << audioData.length();
    // content_Length = oss.str();
    // headers = curl_slist_append(headers, content_Length.c_str());

    // curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // /**
    // * 设置HTTP请求数据
    // */
    // curl_easy_setopt(curl, CURLOPT_POSTFIELDS, audioData.c_str());
    // curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, audioData.length());

    /**
    * 设置HTTP请求的响应回调函数
    */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Ali_RestfulASR::responseCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, srResult);

    /**
    * 发送HTTP请求
    */
    res = curl_easy_perform(curl);

    // 释放资源
    // curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << endl;
        return false;
    }
    return true;
}