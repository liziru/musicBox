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
  string getAsrRes();
  void setAsrRes(string res);

  string getPbFile();
  void setPbFile(string pbFileName);

  void setLanType(int type);
  int getLanType();

  void setLanJsonTask(string json);
  string getlanJsonTask();

private:
  float angle;
  string word;
  string asrFileName;
  string asrRes;
  string pbFileName; //playback-file-name
  //netease lanucher
  int lanType;
  string lanJsonTask;
};
#endif //__WAKEUP_EVENT_H__