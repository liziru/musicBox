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
    static void *downloadProcess_thread(void *p);
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    WakeupEvent *wakeupEvent;
    bool isRun;
    list<WakeupListenner *> playbackListeners;
    string downloadUrl;
    string destFileName;
    
    const static int BUFFER_SIZE;
    static bool isValidUrl(string url);
    static bool downloadMusicByUrl(string url,string musicFile);
    static string getFileFromUrl(string url);
  public:
    DownloadService();
    ~DownloadService();
    void onWakeup(WakeupEvent *wuEvent);
    void run();

    void addPlaybackListeners(WakeupListenner *listenner);
    void removePlaybackListeners(WakeupListenner *listenner);
};

#endif // __DOWNLOAD_SERVICE
