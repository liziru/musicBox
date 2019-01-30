#ifndef __ASR_LAUNCHER_H__
#define __ASR_LAUNCHER_H__
#include "WakeupEvent.h"
#include "WakeupListenner.h"
#include <pthread.h>
class AsrLauncher
{
  public:
    AsrLauncher(WakeupEvent *wakeupEvent);
    ~AsrLauncher();
    void run();

  private:
    pthread_t launchThread;
    WakeupEvent *wakeupEvent;
    static void *launchProcess(void *p);
};
#endif  //__ASR_LAUNCHER_H__