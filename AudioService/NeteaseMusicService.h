#if !defined(__NeteaseMusicProcess__H)
#define __NeteaseMusicProcess__H
#include "WakeupEvent.h"
#include "WakeupListenner.h"
#include <string>
#include <pthread.h>
#include "log.h"
#include <list>
using namespace std;

class NeteaseMusicService : public WakeupListenner
{
private:
  pthread_t th_netease;
  static void *neteaseProcess(void *p);
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  WakeupEvent *wakeupEvent;
  bool isRun;
  const static string URL;
  list<WakeupListenner *> downloadListeners;
  int startNeteaseClient();
  // int requestProcess(string request, string *srResult);

  int login(string *srResult);
  int showLoginStatus();
  int requestProcess(string request, string *srResult);

public:
  NeteaseMusicService();
  ~NeteaseMusicService();
  void onWakeup(WakeupEvent *wakeupEvent);
  void run();

  void addDownloadListeners(WakeupListenner *listenner);
  void removeDownloadListeners(WakeupListenner *listenner);
};
#endif // __NeteaseMusicProcess__H
