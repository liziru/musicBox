#include "NeteaseMusicService.h"
// #include "SemanticsAnalysis.h"

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
}

NeteaseMusicService::~NeteaseMusicService()
{
    delete wakeupEvent;
    wakeupEvent = NULL;
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    isRun = false;
}

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
    }
    else
    {
        return -1;
        // exit(1);
    }

    if (fpr != NULL)
    {
        pclose(fpr);
        fpr = NULL;
    }
    return 0;
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