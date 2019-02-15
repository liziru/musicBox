#ifndef __ASR_LAUNCHER_H__
#define __ASR_LAUNCHER_H__
#include "WakeupEvent.h"
#include "WakeupListenner.h"
#include <pthread.h>
#include <string>
using namespace std;

class TtsService : public WakeupListenner
{
public:
  TtsService();
  ~TtsService();
  void run();
  void onWakeup(WakeupEvent *wakeupEvent);

private:
  pthread_t launchThread;
  WakeupEvent *wakeupEvent;
  static void *launchProcess(void *p);
  pthread_mutex_t mutex;
  pthread_cond_t cond;
};

#endif //__ASR_LAUNCHER_H__