#ifndef __WAKEUP_EVENT_H__
#define __WAKEUP_EVENT_H__
#include <string>
using namespace std;

class WakeupEvent
{
public:
  WakeupEvent();
  float getAngle();
  void setAngle(float angle);
  void setWord(string word);
  string getWord();
  void setAsrFileName(string fileName);
  string getAsrFileName();

private:
  float angle;
  string word;
  string asrFileName;
};
#endif //__WAKEUP_EVENT_H__