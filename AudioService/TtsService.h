#ifndef __ASR_LAUNCHER_H__
#define __ASR_LAUNCHER_H__
#include "WakeupEvent.h"
#include "WakeupListenner.h"
#include <pthread.h>
#include <string>
#include "PlayBackAudio.h"

using namespace std;

class TtsService : public WakeupListenner
{
public:
  TtsService(PlayBackAudio *playBack);
  ~TtsService();
  void run();
  void onWakeup(WakeupEvent *wakeupEvent);

private:
  pthread_t launchThread;
  WakeupEvent *wakeupEvent;
  static void *launchProcess(void *p);
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  PlayBackAudio *playBack;
};

#endif //__ASR_LAUNCHER_H__