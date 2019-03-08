#include "NeteaseMusicLanucher.h"

NeteaseMusicLanucher::NeteaseMusicLanucher(PlayBackAudio *pb, DownloadService *download) : playbackAudio(pb), downloadService(download)
{
    wakeupEvent = new WakeupEvent();
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    isRun = false;
    workType = 0;
}

NeteaseMusicLanucher::~NeteaseMusicLanucher()
{
    if (wakeupEvent)
    {
        delete wakeupEvent;
        wakeupEvent = NULL;
    }
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}

bool NeteaseMusicLanucher::parseJson(string msg)
{
    if (workType == 1)
    {
        printf("\n\nJson to parse:%s\n\n", msg.c_str());
    }
}

void NeteaseMusicLanucher::onWakeup(int type, string msg)
{
    // wakeupEvent
    // wakeupEvent->setAsrRes(wuEvent->getAsrRes());
    // string jsonMsg = msg;
    workType = type;
    if (parseJson(msg) && !playbackList.empty())
    {
        pthread_cond_signal(&cond);
    }
}

void NeteaseMusicLanucher::onWakeup(WakeupEvent *wakeupEvent)
{
    pthread_cond_signal(&cond);
}

void *NeteaseMusicLanucher::neteaseLanucherProcess(void *p)
{
    NeteaseMusicLanucher *lancher = (NeteaseMusicLanucher *)p;
    lancher->isRun = true;

    while (lancher->isRun)
    {
        pthread_mutex_lock(&lancher->mutex);
        pthread_cond_wait(&lancher->cond, &lancher->mutex);
        pthread_mutex_unlock(&lancher->mutex);
        while (1)
        {
            //循环播放列表
            macroFuncVargs("Prepare to download and playback music.");
            exit(1);
        }
    }
}

void NeteaseMusicLanucher::run()
{
    if (!this->isRun)
    {
        pthread_create(&th_netease, NULL, neteaseLanucherProcess, (void *)this);
    }
}
