#ifndef __VAD_H__
#define __VAD_H__
#include "../fft/realFFT.h"
#include "vadDefines.h"

#define LWORK (100)



class CVad
{
public:
	CVad(int frame_length, int channelN, int sample_rate);
	virtual ~CVad();
	
	float process(const float *in);
	int getState(const float *in);

	void set();

private:

	int timeDomainVAD(const float* in, int c = 0);


private:
	bool m_bUseWebrtcVad;
	int m_webrtc_frame;

	DATA_PACKET m_webrtc_packet;

	int m_frame_len;
	int m_N;
	float m_sampleRate;
	int m_vadFlag;
	VAD_PARAMETER *m_pVadParameter;
	VAD_BUFFER *m_pVadBuffer;
	
	int m_active_cnt;
	int m_silent_cnt;

	enum StatusType_vad
	{
		STATUS_SILENT_BEFORE_START = 0,
		STATUS_QUESTION_START,
		STATUS_QUESTION_PROGRESS,
		STATUS_QUESTION_END,
		STATUS_SILENT_AFTER_END
	};

	StatusType_vad m_state;

};
#endif /*__DOA_H__*/