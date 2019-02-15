#if !defined(__AUDIOPREPROCESS_ASR_LISTENNER)
#define __AUDIOPREPROCESS_ASR_LISTENNER
#include "WakeupEvent.h"

class AudioPreprocessAsrListenner
{
public:
  virtual void onDataArrival(WakeupEvent *wakeupEvent) = 0; //纯虚函数
  virtual inline ~AudioPreprocessAsrListenner() {}
};

#endif // __AUDIOPREPROCESS_ASR_LISTENNER
