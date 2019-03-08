/*
 * @Description:  
 * @version:  V1.0
 * @Company: Twirling in time
 * @Author: Lipingping
 * @Date: 2019-02-18 20:06:38
 * @LastEditors: Lipingping
 * @LastEditTime: 2019-03-06 10:25:33
 */
#if !defined(__SemanticsAnalysis__H)
#define __SemanticsAnalysis__H

#include "WakeupEvent.h"
#include "WakeupListenner.h"
#include <string>
#include <pthread.h>
#include "log.h"
#include <list>

using namespace std;

class SemanticsAnalysis : public WakeupListenner
{
private:
  pthread_t th_sa;
  static void *semanticsAnalysis_process(void *p);
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  WakeupEvent *wakeupEvent;
  bool isRun;
  list<WakeupListenner *> neteaseListeners;

public:
  SemanticsAnalysis();
  ~SemanticsAnalysis();
  void onWakeup(WakeupEvent *wuEvent);
  void run();

  void addNeteaseListeners(WakeupListenner *listenner);
  void removeNeteaseListeners(WakeupListenner *listenner);
};

#endif // __SemanticsAnalysis__H
