#ifndef __AUDIO_PREPROCESS_DISPATCHER_H__
#define __AUDIO_PREPROCESS_DISPATCHER_H__
#include <string>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <list>
#include "fifo.h"
#include "MicDataSource/MicDataSource.h"
#include "AudioPreprocessListenner.h"

using namespace std;
/**
 * 音频前处理分发类
 */
class AudioPreprocessDispatcher
{
  public:
    AudioPreprocessDispatcher(bool ansIsOn, bool drcIsOn, float ansDB, float digitalGain);
    ~AudioPreprocessDispatcher();
    /**
     * 注册mic数据源
     */
    void setMicDataSource(MicDataSource *micDataSource);
    /**
     * 开始处理数据，并进行分发
     */
    void dispatcherAudioData();
    /**
     * 注册数据监听者
     */
    void addAudioPreprocessListenner(AudioPreprocessListenner *listenner);
    /**
     * 移除数据监听者
     */
    void removeAudioPreprocessListenner(AudioPreprocessListenner *listenner);
    /**
     * 获取状态
     */
    bool getIsRun();
    /**
     * 停止分发
     */
    void stop();
    static const short MAX16S = 32767;
    static const short MIN16S = -32768;
    static const int MAXABS16S = 32768;

  private:
    // mic 数据源
    MicDataSource *micDataSource;
    // 监听者列表
    list<AudioPreprocessListenner *> audioPreprocessListenners;
    // 运行状态
    bool isRun;
    // drc开启开关
    bool drcIsOn;
    // ans开启开关
    bool ansIsOn;
    // ans抑制值
    float ansDB;
    // 数字增益倍数
    float digitalGain;
    // ans处理对象
	void *m_ans_obj;
    // drc处理对象
	void *m_drc_obj;
    // doa处理对象
    void *m_doa_obj;
    // 分发线程
    pthread_t th_dispatcher;
    // 转换精度
    static const float SCALE;
    static void *dispatcherThread(void *p);
};
#endif
