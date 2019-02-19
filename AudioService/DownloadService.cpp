#include "DownloadService.h"
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

void *DownloadService::downloadProcess(void *p)
{
    pthread_detach(pthread_self());
    DownloadService *downloadService = (DownloadService *)p;
    downloadService->isRun = true;

    while (downloadService->isRun)
    {
        pthread_mutex_lock(&downloadService->mutex);
        pthread_cond_wait(&downloadService->cond, &downloadService->mutex);
        pthread_mutex_unlock(&downloadService->mutex);
        macroFuncVargs("asr res(%s)", downloadService->wakeupEvent->getAsrRes().c_str());
        //process
        list<WakeupListenner *>::iterator it = downloadService->playbackListeners.begin();
        for (; it != downloadService->playbackListeners.end(); ++it)
        {
            (*it)->onWakeup(downloadService->wakeupEvent);
        }
    }
}

void DownloadService::onWakeup(WakeupEvent *wakeupEvent)
{
    wakeupEvent->setAsrRes(wakeupEvent->getAsrRes());
    pthread_cond_signal(&cond);
}
void DownloadService::run()
{
    if (!this->isRun)
    {
        pthread_create(&th_download, NULL, downloadProcess, (void *)this);
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