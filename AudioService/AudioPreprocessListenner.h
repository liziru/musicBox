#ifndef __AUDIO_PREPROCESS_LISTENNER_H__
#define __AUDIO_PREPROCESS_LISTENNER_H__
class AudioPreprocessListenner
{
  public:
    virtual void onDataArrival(short *audioData, float angle) = 0;//纯虚函数
    virtual inline ~AudioPreprocessListenner() {}
};
#endif
