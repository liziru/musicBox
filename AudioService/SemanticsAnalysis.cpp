#include "SemanticsAnalysis.h"
SemanticsAnalysis::SemanticsAnalysis()
{
    wakeupEvent = new WakeupEvent();
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}

SemanticsAnalysis::~SemanticsAnalysis()
{
    delete wakeupEvent;
    wakeupEvent = NULL;
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    isRun = false;
}

void *SemanticsAnalysis::semanticsAnalysis_process(void *p)
{
    pthread_detach(pthread_self());
    SemanticsAnalysis *sa = (SemanticsAnalysis *)p;
    sa->isRun = true;

    while (sa->isRun)
    {
        pthread_mutex_lock(&sa->mutex);
        pthread_cond_wait(&sa->cond, &sa->mutex);
        pthread_mutex_unlock(&sa->mutex);
        macroFuncVargs("asr res(%s)", sa->wakeupEvent->getAsrRes().c_str());
        //process

        // sa->wakeupEvent->setAsrRes(asrLauncher->getFinalRes());
        list<WakeupListenner *>::iterator it = sa->neteaseListeners.begin();
        for (; it != sa->neteaseListeners.end(); ++it)
        {
            (*it)->onWakeup(sa->wakeupEvent);
        }
    }
}

void SemanticsAnalysis::onWakeup(WakeupEvent *wuEvent)
{
    wakeupEvent->setAsrRes(wuEvent->getAsrRes());
    pthread_cond_signal(&cond);
}
void SemanticsAnalysis::run()
{
    if (!this->isRun)
    {
        pthread_create(&th_sa, NULL, semanticsAnalysis_process, (void *)this);
    }
}

void SemanticsAnalysis::addNeteaseListeners(WakeupListenner *listenner)
{
    this->neteaseListeners.push_back(listenner);
}

void SemanticsAnalysis::removeNeteaseListeners(WakeupListenner *listenner)
{
    this->neteaseListeners.remove(listenner);
}
//    void addNeteaseListeners(WakeupListenner *listenner);
//     void removeNeteaseListeners(WakeupListenner *listenner);