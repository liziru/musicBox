#ifndef __ASR_SERVICE_H__
#define __ASR_SERVICE_H__
#include "WakeupEvent.h"
#include "WakeupListenner.h"
#include <pthread.h>
#include "AudioPreprocessAsrListenner.h"
class AsrService : public AudioPreprocessAsrListenner
{
public:
  AsrService();
  virtual ~AsrService();
  bool getIsRun();
  void run();
  void stop();
  // virtual void onWakeup(WakeupEvent *wakeupEvent);
  virtual void onDataArrival(WakeupEvent *wakeupEvent);

private:
  bool isRun;
  pthread_t th_asr;
  WakeupEvent *wakeupEvent;
  pthread_cond_t wakeupArrivalCond;
  pthread_mutex_t wakeupArrivalMutex;
  static void *asrProcess(void *p);
};
#endif //__ASR_SERVICE_H__
