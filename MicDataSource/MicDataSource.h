#ifndef __MIC_DATA_SOURCE_H__
#define __MIC_DATA_SOURCE_H__
#include <string>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include "fifo.h"

using namespace std;
class MicDataSource //类声明
{
 public:
   /**
     * 一次读取的音频帧数 为一包
     */
   static const int FRAMELEN = 512;
   /**
     * FIFO 队列中保存数据包的个数
     */
   static const int FIFO_BUFF_COUNT = 8;
   /**
     * 获取单例
     */
   static MicDataSource *getInstance(string deviceName, int rate,
                                     int channel, snd_pcm_format_t dataFormart, int micProfileId, int micFormatId, float micDist);

   static int status; //1,capture voice to asr; 0, capture to fifo;

#define ASRFILE "./asrfile.pcm"

   /**
     * 运行录音线程
     */
   void run();

   /**
     * 停止录音线程
     */
   void stop();

   ~MicDataSource();

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
   MicDataSource(string deviceName, int rate,
                 int channel, snd_pcm_format_t dataFormart, int micProfileId, int micFormatId, float micDist);
   MicDataSource(const MicDataSource &);
   MicDataSource &operator=(const MicDataSource &);

   static void *capture_process(void *p);

   int prepare();

   /**
     * MicDataSource运行状态
     */
   bool isRun;

   pthread_t th_capture;
   /**
     * 单例对象
     */
   static MicDataSource *instance;
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
   snd_pcm_t *capture_handle;
   snd_pcm_hw_params_t *capture_hw_params;
};
#endif