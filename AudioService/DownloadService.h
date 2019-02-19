/*
 * @Description:  
 * @version:  V1.0
 * @Company: Twirling in time
 * @Author: Lipingping
 * @Date: 2019-02-18 20:06:38
 * @LastEditors: Lipingping
 * @LastEditTime: 2019-02-19 09:50:35
 */
#if !defined(__DOWNLOAD_SERVICE)
#define __DOWNLOAD_SERVICE

#include "WakeupEvent.h"
#include "WakeupListenner.h"
#include <string>
#include <pthread.h>
#include "log.h"
#include <list>

using namespace std;

class DownloadService : public WakeupListenner
{
  private:
    pthread_t th_download;
    static void *downloadProcess(void *p);
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    WakeupEvent *wakeupEvent;
    bool isRun;
    list<WakeupListenner *> playbackListeners;

  public:
    DownloadService();
    ~DownloadService();
    void onWakeup(WakeupEvent *wakeupEvent);
    void run();

    void addPlaybackListeners(WakeupListenner *listenner);
    void removePlaybackListeners(WakeupListenner *listenner);
};

#endif // __DOWNLOAD_SERVICE
