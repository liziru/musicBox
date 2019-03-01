#include "NeteaseMusicService.h"
// #include "SemanticsAnalysis.h"
#include <fstream>
#include <sstream>
#include "curl/curl.h"
#include "Ali_RestfulASR.h"
#include <iostream>
#include <string>
using namespace std;

const string NeteaseMusicService::URL = "http://localhost:3000";

int NeteaseMusicService::startNeteaseClient()
{
    string cmd = "node /home/pi/NeteaseCloudMusicApi/app.js";
    char readbuf[1024] = {};
    int ret = 0;
    FILE *fpr = popen(cmd.c_str(), "r");
    if (!fpr)
    {
        fpr = NULL;
        macroFuncVargs("ERROR:failed to popen %s", cmd.c_str());
        return -1;
    }
    ret = fread(readbuf, 1, sizeof(readbuf), fpr);
    if (strstr(readbuf, "server running") != NULL)
    {
        macroFuncVargs("Netease Client starts successfully,freadbuf %s,len = %d", readbuf, ret);
        ret = 0;
    }
    else
    {
        // return -1;
        // exit(1);
        ret = -1;
    }

    if (fpr != NULL)
    {
        pclose(fpr);
        fpr = NULL;
    }
    return ret;
}
void *NeteaseMusicService::neteaseProcess(void *p)
{
    pthread_detach(pthread_self());
    NeteaseMusicService *neteaseService = (NeteaseMusicService *)p;
    neteaseService->isRun = true;

    while (neteaseService->isRun)
    {
        pthread_mutex_lock(&neteaseService->mutex);
        pthread_cond_wait(&neteaseService->cond, &neteaseService->mutex);
        pthread_mutex_unlock(&neteaseService->mutex);
        macroFuncVargs("NeteaseMusicService: asr res(%s)", neteaseService->wakeupEvent->getAsrRes().c_str());
        //process

        list<WakeupListenner *>::iterator it = neteaseService->downloadListeners.begin();
        for (; it != neteaseService->downloadListeners.end(); ++it)
        {
            (*it)->onWakeup(neteaseService->wakeupEvent);
        }
    }
}

void NeteaseMusicService::onWakeup(WakeupEvent *wakeupEvent)
{
    wakeupEvent->setAsrRes(wakeupEvent->getAsrRes());
    pthread_cond_signal(&cond);
}
void NeteaseMusicService::run()
{
    if (!this->isRun)
    {
        pthread_create(&th_netease, NULL, neteaseProcess, (void *)this);
    }
}

void NeteaseMusicService::addDownloadListeners(WakeupListenner *listenner)
{
    this->downloadListeners.push_back(listenner);
}

void NeteaseMusicService::removeDownloadListeners(WakeupListenner *listenner)
{
    this->downloadListeners.remove(listenner);
}

int NeteaseMusicService::login(string *srResult)
{
    string requ = "/login?email=youmakemewhole@163.com&password=17610021721lp";
    // strint res;

    if (requestProcess(requ, srResult) < 0)
    {
        macroFuncVargs("Error: login failed.");
        exit(1);
    }
    else
    {
        //process json to get userId
    }

    return 0;
}

int NeteaseMusicService::showLoginStatus()
{
    // string requ = "/login/statu";
    // string srResult;
    // if (requestProcess(requ, &srResult) < 0)
    // {
    //     macroFuncVargs("Error: showLoginStatus failed.");
    //     exit(1);
    // }
    // else
    // {
    //     //process json to get userId

    //     // uid=;
    // }

    return 0;
}

NeteaseMusicService::NeteaseMusicService()
{
    wakeupEvent = new WakeupEvent();
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    //start netease server.
    if (startNeteaseClient() < 0)
    {
        macroFuncVargs("FATAL ERROR:failed to start Netease Client.");
        exit(1);
    }
    //login
    // 全局只初始化一次
    curl_global_init(CURL_GLOBAL_ALL);
    string srRes;
    login(&srRes);
    //login/statu, and process json to get userId
    showLoginStatus();
}

NeteaseMusicService::~NeteaseMusicService()
{
    delete wakeupEvent;
    wakeupEvent = NULL;
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    isRun = false;
    curl_global_cleanup(); //
}

int NeteaseMusicService::requestProcess(string request, string *srResult)
{
    CURL *curl = NULL;
    CURLcode res;

    curl = curl_easy_init();
    if (curl == NULL)
    {
        return -1;
    }

    request = URL + request;
    macroFuncVargs("login url:%s", request.c_str());
    /**
    * 设置HTTP请求行
    */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_URL, request.c_str());

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
        return -1;
    }
    return 0;
}