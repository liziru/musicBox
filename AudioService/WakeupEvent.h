#ifndef __WAKEUP_EVENT_H__
#define __WAKEUP_EVENT_H__
#include <string>

class WakeupEvent
{
public:
  WakeupEvent();
  float getAngle();
  void setAngle(float angle);
  // void setFileName
private:
  float angle;
  // string filename;
};
#endif //__WAKEUP_EVENT_H__