#if !defined(__NETEASEMUSIC_LANUCHER_)
#define __NETEASEMUSIC_LANUCHER_

#include <string>
#include <pthread.h>
#include <list>

#include "PlayBackAudio.h"
#include "log.h"
#include "WakeupEvent.h"
#include "WakeupListenner.h"
#include "DownloadService.h"
#include "AudioService/TtsService.h"

using namespace std;

class NeteaseMusicLanucher : public WakeupListenner
{
private:
  TtsService *ttsService;
  list<string> playbackList; //播放列表
  WakeupEvent *wakeupEvent;
  // int status;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  bool isRun;
  PlayBackAudio *playbackAudio;
  DownloadService *downloadService;
  int workType; //0:do nothing; 1:playback what I like.
  string taskJson;
  pthread_t th_netease;
  //ways
  static void *neteaseLanucherProcess(void *p);
  bool parseJson(string msg);

public:
  NeteaseMusicLanucher(PlayBackAudio *pb, DownloadService *download, TtsService *tts);
  ~NeteaseMusicLanucher();
  void onWakeup(int type, string msg);
  void onWakeup(WakeupEvent *wuEvent);
  void run();

  int getWorkType();
  void setWorkType(int type);
  list<string> getPlaybackList();
  void setPlaybackList(list<string> list);
};

#endif // __NETEASEMUSIC_LANUCHER_
