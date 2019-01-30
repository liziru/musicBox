#include "dynamicRangeControl.h"
#include <string.h>
#include <math.h>

#define minGain (0.1f)
#define maxGain (1.0f)
#define stepSize (0.002f)

CDynamicRangeControl::CDynamicRangeControl(int frame_length, int channelN, float maxV)
{
    m_frame_len = frame_length;		
	m_channelN = channelN;
	m_th = maxV;
	if (m_th < 1.f) {
		m_th = 1.f;
	}
	m_sg = 1.f;
}

CDynamicRangeControl::~CDynamicRangeControl()
{
}

void CDynamicRangeControl::set(float maxV)
{
	if (m_th < 1.f) {
		m_th = 1.f;
	}
}

// Update is called once every frame
void CDynamicRangeControl::process(const float *in, float *out)
{
	// generate next frame of audio
	const float *in_iter = in;
	float maxip = fabs(*in_iter);
	for (int i = 0; i < m_frame_len * m_channelN; i++) {
		float tmp = fabs(*in_iter);
		if(tmp > maxip) {
			maxip = tmp;
		}
		in_iter++;
	}
	
	float gain = m_th / maxip;  
	if (gain < minGain) { 
		gain = minGain;	
	}

	if (gain > m_sg) {   //here imply that else if (gain<smooth_gain), gain is determined only by current gain. So fast suppress.
        gain = (m_sg + stepSize);  //slow climb and extremely slow when snr is very low.          
		if (gain > maxGain) {         
			gain = maxGain;
		}
	}	
	
	float scale = 1.f / (float)m_frame_len;
	in_iter = in;
	float *out_iter = out;
    for (int i = 0; i < m_frame_len; i++) {
        float gain_t = m_sg + (gain - m_sg) * (float)i * scale;
		for (int j = 0; j < m_channelN; j++) {
			out_iter[j] = in_iter[j] * gain_t;
		}
		in_iter += m_channelN; out_iter += m_channelN;
	}

	m_sg = gain;
}
