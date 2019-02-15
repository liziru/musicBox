#include "AsrService.h"
#include "log.h"
#include "Ali_RestfulASR.h"

AsrService::AsrService()
    : isRun(false), th_asr(0), wakeupEvent(NULL)
{
    wakeupEvent = new WakeupEvent();
    pthread_cond_init(&this->wakeupArrivalCond, NULL);
    pthread_mutex_init(&this->wakeupArrivalMutex, NULL);
}
AsrService::~AsrService()
{
    delete wakeupEvent;
    wakeupEvent = NULL;
}
bool AsrService::getIsRun()
{
    return this->isRun;
}
void AsrService::run()
{
    if (!this->isRun)
    {
        pthread_create(&this->th_asr, NULL, asrProcess, (void *)this);
    }
}

void AsrService::stop()
{
    this->isRun = false;
}

void AsrService::onDataArrival(WakeupEvent *wakeupEvent)
{
    this->wakeupEvent->setAsrFileName(wakeupEvent->getAsrFileName());
    pthread_cond_signal(&wakeupArrivalCond);
}
void *AsrService::asrProcess(void *p)
{
    pthread_detach(pthread_self());
    AsrService *asrService = (AsrService *)p;
    asrService->isRun = true;
    Ali_RestfulASR *asrLauncher = new Ali_RestfulASR("841dd91f814c46dcb558402e4844e00d"); //tokenkey

    while (asrService->isRun)
    {
        pthread_mutex_lock(&asrService->wakeupArrivalMutex);
        pthread_cond_wait(&asrService->wakeupArrivalCond, &asrService->wakeupArrivalMutex);
        pthread_mutex_unlock(&asrService->wakeupArrivalMutex);

        asrLauncher->process(asrService->wakeupEvent->getAsrFileName().c_str());
    }

    return NULL;
}
