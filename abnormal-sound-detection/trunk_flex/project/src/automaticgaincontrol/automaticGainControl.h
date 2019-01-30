#ifndef AUTOMATIC_GAIN_CONTROL_H
#define AUTOMATIC_GAIN_CONTROL_H
#include "agcDefines.h"

class CVad;

class CAutomaticGainControl
{
public:
	//frame_length can be 512 for 32kbps, and 256 for 16kbps, ......
	CAutomaticGainControl(int frame_length, int channelN, int sampleRate, float threshold=1.f);
	~CAutomaticGainControl();

    void process(const float *in, float *out, float *params);
	void set(int formatid); 

private:	
	//it support mono or stereo and do not support more channels so far
	void timeDomainAGC(const float *in, float *out, float *params);

private:

	int m_frame_len;               // sample number of each audio frame
	int m_channelN;	
	int m_sampleRate;
	float m_threshold;
	AGC_PARAMETER *m_pAgcParameter;
	AGC_BUFFER *m_pAgcBuffer;
	CVad *m_vad_inst;

	enum AudioFormat_e
	{
		UNKNOWN = 0x00, 
		MONO = 0x01,
		STEREO = 0x02
	};
	typedef AudioFormat_e AudioFormat;

	AudioFormat m_format;
	
	struct SoundParameter_s
	{		
		float agc_gain;   //AGC output
		float vad_flag;   //AGC output      
		float reserved1;       		
	};
	typedef SoundParameter_s SoundParameter;
	SoundParameter m_snd_param;

};

#endif