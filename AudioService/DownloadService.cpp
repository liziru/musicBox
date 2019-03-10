#include "DownloadService.h"
#include "util.h"
#include<string>
#include<iostream>
using namespace std;

const int DownloadService::BUFFER_SIZE = 2048;

DownloadService::DownloadService()
{
    wakeupEvent = new WakeupEvent();
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}

DownloadService::~DownloadService()
{
    delete wakeupEvent;
    wakeupEvent = NULL;
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    isRun = false;
}

bool DownloadService::isValidUrl(string url)
{
    // wget --spider -T 5 -q -t 2 www.oldboyedu.com
    // [root@oldboy ~]# echo $? #<==利用返回值确定网站是否正常。
    string cmd = "wget --spider -T 5 -t 2 " + url;
    bool res = false;
    FILE *fpr = popen(cmd.c_str(), "r");
    if (!fpr)
    {
        macroFuncVargs("ERROR: popen failed,reason:%s", strerror(errno));
        exit(1);
    }
    char readBuf[BUFFER_SIZE] = {};
    int ret = fread(readBuf, 1, sizeof(readBuf), fpr);
    if (string(readBuf).find("Remote file exists.") != string::npos)//string::npos
    {
        res = true;
    }
    else
    {
        res = false;
        macroFuncVargs("ERROR: URL:%s is invalid,readbuf:%s", url.c_str(), readBuf);
    }

    if (fpr)
    {
        pclose(fpr);
        fpr = NULL;
    }

    return res;
}

string DownloadService::getFileFromUrl(string url)
{
    size_t pos=0;
    if ((pos=url.find_last_of("/"))!=string::npos) {
        return url.substr(pos);
    }
    
    return "";
}

bool DownloadService::downloadMusicByUrl(string url, string musicFile)
{
    //1.判断url是否有效
    if (!isValidUrl(url))
    {
        macroFuncVargs("ERROR: url is invalid.");
        return false;
    }
    //2.下载
    string mp3File=getFileFromUrl(url);
    string cmd = "wget -t 3 " + url;
    bool res = false;
    FILE *fpr = popen(cmd.c_str(), "r");
    if (!fpr)
    {
        macroFuncVargs("ERROR: popen(%s) failed,reason:%s", cmd.c_str(), strerror(errno));
        exit(1);
    }
    char readBuf[BUFFER_SIZE] = {};
    int ret = fread(readBuf, 1, sizeof(readBuf), fpr);
    if (string(readBuf).find("saved") != string::npos)
    {
        res = true;
        string newName="";
        renameFile(mp3File,newName);//结束后，需要删除文件
    }
    else
    {
        res = false;
        macroFuncVargs("ERROR: download error.URL:%s is invalid,readbuf:%s", url.c_str(), readBuf);
    }

    if (fpr)
    {
        pclose(fpr);
        fpr = NULL;
    }

    return res;
}

void *DownloadService::downloadProcess_thread(void *p)
{
    pthread_detach(pthread_self());
    DownloadService *downloadService = (DownloadService *)p;
    downloadService->isRun = true;

    while (downloadService->isRun)
    {
        pthread_mutex_lock(&downloadService->mutex);
        pthread_cond_wait(&downloadService->cond, &downloadService->mutex);
        pthread_mutex_unlock(&downloadService->mutex);
        // macroFuncVargs("asr res(%s)", downloadService->wakeupEvent->getAsrRes().c_str());
        //process
        // list<WakeupListenner *>::iterator it = downloadService->playbackListeners.begin();
        // for (; it != downloadService->playbackListeners.end(); ++it)
        // {
        //     (*it)->onWakeup(downloadService->wakeupEvent);
        // }

        // downloadMusicByUrl();
    }
}

void DownloadService::onWakeup(WakeupEvent *wuEvent)
{
    wakeupEvent->setAsrRes(wakeupEvent->getAsrRes());
    //
    pthread_cond_signal(&cond);
}
void DownloadService::run()
{
    if (!this->isRun)
    {
        pthread_create(&th_download, NULL, downloadProcess_thread, (void *)this);
    }
}

void DownloadService::addPlaybackListeners(WakeupListenner *listenner)
{
    this->playbackListeners.push_back(listenner);
}

void DownloadService::removePlaybackListeners(WakeupListenner *listenner)
{
    this->playbackListeners.remove(listenner);
}