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
  const static string URL;
  const static string COOKIE_FILE;
  pthread_t th_netease;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  WakeupEvent *wakeupEvent;
  bool isRun;
  list<WakeupListenner *> neteaseLanucherListeners;
  string loginRes;
  FILE *fpr;
  unsigned long uid;

  //way
  bool startNeteaseClient();
  // int requestProcess(string request, string *srResult);
  bool login();
  string showLoginStatus();
  static bool requestProcess(string request, string *srResult);
  static bool requestProcessWithCookie(string request, string *srResult);
  static bool loginRequestProcess(string request, string *srResult);

  string getNeteaseRecommandList();
  string getMusicUrlById(long id);
  static void *neteaseProcess_thread(void *p);

public:
  NeteaseMusicService();
  ~NeteaseMusicService();
  void onWakeup(WakeupEvent *wuEvent);
  void run();

  void addNeteaseLanucherListeners(WakeupListenner *listenner);
  void removeNeteaseLanucherListeners(WakeupListenner *listenner);
};
#endif // __NeteaseMusicProcess__H
