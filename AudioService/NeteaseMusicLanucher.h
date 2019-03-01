#if !defined(__NETEASEMUSIC_LANUCHER_)
#define __NETEASEMUSIC_LANUCHER_

#include "WakeupEvent.h"
#include "WakeupListenner.h"
#include <string>
#include <pthread.h>
#include "log.h"
#include <list>
using namespace std;

class NeteaseMusicLanucher : public WakeupListenner
{
  private:
    list<string> playbackList; //播放列表
    WakeupEvent *wakeupEvent;
    int status;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool isRun;
    //ways
    static void *neteaseLanucherProcess(void *p);

  public:
    NeteaseMusicLanucher(/* args */);
    ~NeteaseMusicLanucher();
    void onWakeup(WakeupEvent *wakeupEvent);
    void run();
};

NeteaseMusicLanucher::NeteaseMusicLanucher()
{
    wakeupEvent = new WakeupEvent();
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    isRun = false;
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
void NeteaseMusicLanucher::onWakeup(WakeupEvent *wakeupEvent)
{
    // wakeupEvent
}

static void *NeteaseMusicLanucher::neteaseLanucherProcess(void *p)
{
    NeteaseMusicLanucher *lancher = (NeteaseMusicLanucher *)p;
    lancher->isRun = true;

    while (lancher->isRun)
    {
        pthread_mutex_lock(&neteaseService->mutex);
        pthread_cond_wait(&neteaseService->cond, &neteaseService->mutex);
        pthread_mutex_unlock(&neteaseService->mutex);
        while(1){
            
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

#endif // __NETEASEMUSIC_LANUCHER_
