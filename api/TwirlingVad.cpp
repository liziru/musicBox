#include "TwirlingVad.h"
#include "log.h"

//#define DEBUG_LOG
#ifndef DEBUG_LOG
#define macroFuncVargs(...)
#define macroFunc(...)
#endif

int logcount = 0;
int freqlen = 50;

TwirlingVad::TwirlingVad(int framelen, int captureRate) : m_vad((void *)NULL),
                                                          m_captureRate(captureRate),
                                                          m_framelen(framelen),
                                                          m_preValues(NULL),
                                                          m_preLen(0),
                                                          m_preHeadLen(0),
                                                          m_preEndLen(0),
                                                          m_curPos(0),
                                                          m_max_gap(640),                //最大语音间隙
                                                          m_min_head(320),               //开始最小时间
                                                          m_start_voice_threshold(0.97), //开始阈值
                                                          m_end_voice_threshold(0.85),   //结束阈值
                                                          m_isstart(true),
                                                          m_curState(VoiceState::VOICE_END),
                                                          m_preState(VoiceState::VOICE_END),
                                                          m_amplitude_sm(0.0),
                                                          m_amplitude_dB(0.0)

{
    m_vad = vadInit(m_framelen, 1, m_captureRate);
    m_preHeadLen = m_min_head / (m_framelen * 1000 / m_captureRate);
    m_preEndLen = m_max_gap / (m_framelen * 1000 / m_captureRate);
    if (m_preHeadLen > m_preLen)
        m_preLen = m_preHeadLen + 1;
    if (m_preEndLen > m_preLen)
        m_preLen = m_preEndLen + 1;
    m_preValues = new float[m_preLen];
}

TwirlingVad::~TwirlingVad()
{
    if (m_vad)
    {
        vadRelease(m_vad);
        m_vad = NULL;
    }
    if (m_preValues)
    {
        delete m_preValues;
        m_preValues = NULL;
    }
}

bool TwirlingVad::checkIsStart(int pos)
{
    float freq = 0;
    int curpos = pos;
    int count = 0;
    for (int i = 0; i < m_preHeadLen; i++)
    {
        if (m_preValues[curpos] > m_start_voice_threshold)
            count++;
        if (++curpos >= m_preLen)
            curpos = 0;
    }
    freq = (float)(((float)count) / m_preHeadLen);
    return freq > 0.9;
}

bool TwirlingVad::checkIsEnd(int pos)
{
    int curpos = pos;
    bool hasVoice = false;
    for (int i = 0; i < m_preEndLen; i++)
    {
        if (m_preValues[curpos--] > m_start_voice_threshold)
        {
            hasVoice = true; //on;
            break;
        }
        curpos = curpos < 0 ? m_preLen : curpos;
    }
    return !hasVoice;
}

int TwirlingVad::checkState()
{
    int curpos = m_curPos - 1;
    curpos = curpos < 0 ? m_preLen - 1 : curpos;
    if (logcount % freqlen == 0)
        macroFuncVargs("curpos = %d\n", curpos);
    if (logcount % freqlen == 0)
        macroFuncVargs("m_preValues[curpos] =%f m_start_voice_threshold =%f m_end_voice_threshold=%f m_preState = %d\n", m_preValues[curpos], m_start_voice_threshold, m_end_voice_threshold, m_preState);

    if (m_preState == VoiceState::VOICE_END)
    {
        if (m_preValues[curpos] >= m_start_voice_threshold)
        {
            int pos = m_curPos - m_preHeadLen;
            if (pos < 0)
                pos += m_preLen;
            if (checkIsStart(pos) && m_amplitude_dB > -60) //modified. -45
            {
                m_curState = VoiceState::VOICE_START;
                macroFunc("\n myVad:       =========================================== start.\n");
            }
        }
    }
    else if (m_preState == VoiceState::VOICE_ON)
    {
        if (m_preValues[curpos] < m_end_voice_threshold)
        {
            if (checkIsEnd(curpos))
            {
                m_curState = VoiceState::VOICE_END;
                macroFunc("\n myVad:       =========================================== end.\n");
            }
            else
            {
                m_curState = VoiceState::VOICE_ON;
            }
        }
        else
        {
            m_curState = VoiceState::VOICE_ON;
        }
    }
    else if (m_preState == VoiceState::VOICE_START)
    {
        m_curState = VoiceState::VOICE_ON;
    }
    m_preState = m_curState;
    return (int)m_curState;
}

void TwirlingVad::process(const float *inbuf)
{
    if (inbuf == NULL)
        return;
    if (m_vad == NULL)
        return;

    //add new test
    int i;
    float factor = 0.95f;
    float amplitude = 0.0f;

    for (i = 0; i < FRAME_LEN; i++)
    {
        amplitude += fabs(inbuf[i]);
    }

    amplitude /= FRAME_LEN;

    m_amplitude_sm = m_amplitude_sm * factor + amplitude * (1 - factor);//smmoth

    m_amplitude_dB = 20 * logf((float)m_amplitude_sm);
    // if (m_amplitude_dB > -10)
    //     printf("m_amplitude_dB : %f \n", m_amplitude_dB);

    //new test ends.

    float ret = vadProcess(m_vad, inbuf);
    //printf("vadprocess data:%f..........ret = %f\n",inbuf[0], ret);
    m_preValues[m_curPos] = ret;
    if (logcount % freqlen == 0)
        macroFuncVargs("m_preValues[%d] = %f  m_preLen = %d  m_curPos = %d\n", m_curPos, ret, m_preLen, m_curPos);
    m_curPos = (++m_curPos) % m_preLen;
    if (logcount % freqlen == 0)
        macroFuncVargs("m_curPos = %d\n", m_curPos);

    int state = checkState();
    logcount++;
}

int TwirlingVad::getState()
{
    // int ret = 0;
    // if (m_curState == VoiceState::VOICE_END)
    // {
    //     ret = 0;
    // }
    // else
    // {
    //     ret = 1;
    // }
    return (int)m_curState;
}
