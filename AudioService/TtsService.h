#ifndef __ASR_LAUNCHER_H__
#define __ASR_LAUNCHER_H__
#include "WakeupEvent.h"
#include "WakeupListenner.h"
#include <pthread.h>
#include <string>
#include "AudioPreprocessAsrListenner.h"
using namespace std;

class TtsService : public AudioPreprocessAsrListenner
{
public:
  TtsService(WakeupEvent *wakeupEvent);
  ~TtsService();
  void run();
  void onDataArrival(string word, float angle);

private:
  int angle;
  string word;
  pthread_t launchThread;
  WakeupEvent *wakeupEvent;
  static void *launchProcess(void *p);
  pthread_mutex_t mutex;
  pthread_cond_t cond;
};

#endif //__ASR_LAUNCHER_H__