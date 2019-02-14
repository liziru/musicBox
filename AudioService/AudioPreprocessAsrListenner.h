#if !defined(__AUDIOPREPROCESS_ASR_LISTENNER)
#define __AUDIOPREPROCESS_ASR_LISTENNER

class AudioPreprocessAsrListenner
{
  public:
    virtual void onDataArrival(short *audioData, float angle) = 0; //纯虚函数
    virtual inline ~AudioPreprocessAsrListenner() {}
};

#endif // __AUDIOPREPROCESS_ASR_LISTENNER
