#ifndef _TWIRLING_VAD_H_
#define _TWIRLING_VAD_H_
#include "../out/include/TwirlingCapture.h"
#include "stdio.h"
#include "stdlib.h"
#include <string>
#include <math.h>

typedef enum VoiceState
{
    VOICE_START = 1,
    VOICE_ON = 2,
    VOICE_END = 3,
} VoiceState;

class TwirlingVad
{

  public:
    TwirlingVad(int framelen, int captureRate);
    ~TwirlingVad();
    void process(const float *inbuf);
    int getState();

  private:
    int checkState();
    bool checkIsStart(int pos);
    bool checkIsEnd(int pos);
    void *m_vad;
    int m_framelen;
    int m_captureRate;

    int m_preLen;
    int m_preHeadLen;
    int m_preEndLen;
    float *m_preValues;

    int m_min_head;                //ms
    int m_max_gap;                 //ms
    float m_start_voice_threshold; //start阈值
    float m_end_voice_threshold;   //end阈值
    float m_amplitude_dB;//db阈值
    float m_amplitude_sm;

    bool m_isstart;
    int m_curPos;

    VoiceState m_curState;
    VoiceState m_preState;
};

#define FRAME_LEN (256)

#endif //_TWIRLING_VAD_H_