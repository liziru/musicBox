#ifndef __WAKEUP_SERVICE_H__
#define __WAKEUP_SERVICE_H__
// #define DUMP_WAKEUP_DATA
#include "WakeupEvent.h"
#include "WakeupListenner.h"
#include "AudioPreprocessListenner.h"
#include "../MicDataSource/MicDataSource.h"
#include <pthread.h>
#include <list>
#include <vector>
using namespace std;
class WakeupService : public AudioPreprocessListenner
{
  public:
    WakeupService(float wakeupThreshold, float wakeupPlp, float wakeupDmin, float wakeupDmax);
    virtual ~WakeupService();
    /**
     * 注册数据监听者
     */
    void addWakeupListenner(WakeupListenner *listenner);
    /**
     * 移除数据监听者
     */
    void removeWakeupListenner(WakeupListenner *listenner);
    /**
     * 获取状态
     */
    bool getIsRun();

    void run();
    void stop();
    void onDataArrival(short *audioData, float angle);

  private:
    bool isRun;
    short *audioData;
    pthread_t th_wakeup;
    // 唤醒对象
    void *m_wakeup_obj;
    // 唤醒参数
    float wakeupThreshold;
    float wakeupPlp;
    float wakeupDmin;
    float wakeupDmax;
    pthread_cond_t dataArrivalCond;
    pthread_mutex_t dataArrivalMutex;
        // 监听者列表
    list<WakeupListenner *> wakeupListenners;
    list<float> angles;
#ifdef DUMP_WAKEUP_DATA
    vector<short> wakeupData;
    void pushBackWakeupData(short *data, const int count);
    void popFrontWakeupData(const int count);
#endif
    short *getAudioDataAddr();
    float calcWakeupAngle();
    static void *wakeupProcess(void *p);
    static const int ANGLES_MIX_SIZE;
};
#endif
