/*
 * @Description:  
 * @version:  V1.0
 * @Company: Twirling in time
 * @Author: Lipingping
 * @Date: 2019-01-30 18:41:28
 * @LastEditors: Lipingping
 * @LastEditTime: 2019-03-05 20:11:39
 */
#ifndef __PLAYBACK_AUDIO_H__
#define __PLAYBACK_AUDIO_H__
#include <string>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include "fifo.h"
#include "WakeupListenner.h"

using namespace std;
class PlayBackAudio : public WakeupListenner //类声明
{
public:
  /**
     * 一次读取的音频帧数 为一包
     */
  static const int FRAMELEN = 256;
  /**
     * FIFO 队列中保存数据包的个数
     */
  static const int FIFO_BUFF_COUNT = 8;
  /**
     * 获取单例
     */
  static PlayBackAudio *getInstance(string deviceName, int rate,
                                    int channel, snd_pcm_format_t dataFormart, int micProfileId, int micFormatId, float micDist);

  /**
     * 运行playback线程
     */
  void run();

  /**
     * 停止录音
     */
  void stop();

  void start();
  /**
     * pthread_signal,启动playback线程
     */
  void onWakeup(WakeupEvent *wuEvent);
  // void onWakeup(string file);

  ~PlayBackAudio();

  string getDeviceName();
  int getRate();
  int getChannel();
  snd_pcm_format_t getDataFormart();
  int getMicProfileId();
  int getMicFormatId();
  float getMicDist();
  bool getDataPackage(void *data);
  bool getIsRun();
  pthread_t getCaptureThreadId();

private:
  PlayBackAudio(string deviceName, int rate,
                int channel, snd_pcm_format_t dataFormart, int micProfileId, int micFormatId, float micDist);
  PlayBackAudio(const PlayBackAudio &);
  PlayBackAudio &operator=(const PlayBackAudio &);

  static void *playback_process(void *p);

  int prepare();

  /**
     * PlayBackAudio运行状态
     */
  bool isRun;

  pthread_t th_capture;
  /**
     * 单例对象
     */
  static PlayBackAudio *instance;
  /**
     * 录音设备名
     */
  string deviceName;
  /**
     * 设备采样率
     */
  int rate;
  /**
     * 录音通道数
     */
  int channel;
  /**
     * 数据格式
     */
  snd_pcm_format_t dataFormart;
  /**
     * 数据队列
     */
  short *sArray;

  /**
     * profileId
     */
  int micProfileId;
  /**
     * micFormatId
     */
  int micFormatId;
  /**
     * mic距离
     */
  float micDist;

  FIFO_Type fifo;
  FIFO_Type *pfifo;
  /**
     * alsa录音借口
     */
  snd_pcm_t *playback_handle;
  snd_pcm_hw_params_t *playback_hw_params;

  pthread_cond_t m_Cond;
  pthread_mutex_t m_Mutex;

  WakeupEvent *wakeupEvent;
};
#endif