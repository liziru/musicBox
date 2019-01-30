#include "automaticGainControl.h"
#include <string.h>
#include <math.h>
#include "../vad/vad.h"


//frame length 2046 for 44.1k because  L_T = 0.0464; L = round(fs*L_T)
CAutomaticGainControl::CAutomaticGainControl(int frame_length, int channelN, int sampleRate, float threshold) : m_frame_len(frame_length)
										,m_channelN(channelN)
										,m_sampleRate(sampleRate)
										,m_vad_inst(NULL)										
										,m_format(UNKNOWN)
										,m_threshold(1.f)
{
	if (m_channelN == 1) {
		m_format = MONO;
	}
	else if (m_channelN == 2) {
		m_format = STEREO;
	} 	
	else
	{ // so far AGC only support mon or stereo, do not support more channels.
		throw;
	}

	if (threshold > 0.f && threshold < 1.f) {
		m_threshold = threshold;
	}
	else
	{
		m_threshold = 1.f;
	}
	

	m_pAgcBuffer = new AGC_BUFFER;	
	m_pAgcBuffer->AGC_input_level = 0.1f;
	m_pAgcBuffer->AGC_gain = 1.f;
	m_pAgcBuffer->AGC_real_gain = 1.f;
	m_pAgcBuffer->AGC_real_gain_prev = 1.f;

	m_pAgcParameter = new AGC_PARAMETER;
	m_pAgcParameter->AGC_alfa_a_ppeak = 1.f - expf(-1.f / ((1.f - AGC_percent_peak) * AGC_T_SP * ((float)m_sampleRate / (float)m_frame_len)));
	m_pAgcParameter->AGC_alfa_d_ppeak = 1.f - expf(-1.f / (AGC_percent_peak * AGC_T_SP * ((float)m_sampleRate / (float)m_frame_len)));	
	m_pAgcParameter->AGC_target = powf(10.f, (AGC_target_level / 20.f));
	m_pAgcParameter->AGC_target_max = powf(10.f, (AGC_target_max / 20.f));
	m_pAgcParameter->AGC_max_gain = powf(10.f, (AGC_max_gain_dB / 20.f));
	m_pAgcParameter->AGC_min_gain = powf(10.f, (AGC_min_gain_dB / 20.f));
	m_pAgcParameter->AGC_pre_gain = powf(10.f, (AGC_pre_gain_dB / 20.f));
	m_pAgcParameter->AGC_attack = 1.f - exp(-1.f / (AGC_attack_time * ((float)m_sampleRate / (float)m_frame_len)));
	m_pAgcParameter->AGC_decay = 1.f - exp(-1.f / (AGC_decay_time * ((float)m_sampleRate / (float)m_frame_len)));
	m_pAgcParameter->AGC_input_Th = powf(10.f, (AGC_input_Th_dB / 20.f));
	m_pAgcParameter->AGC_adap_Th_linear = powf(10.f, (AGC_adap_Th / 20.f)) - 1.f;

	m_vad_inst = new CVad(m_frame_len, m_channelN, m_sampleRate);

	
}

CAutomaticGainControl::~CAutomaticGainControl()
{
	if (NULL != m_vad_inst) { delete m_vad_inst; m_vad_inst = NULL; }
	if (NULL != m_pAgcBuffer) { delete m_pAgcBuffer; m_pAgcBuffer = NULL; }
	if (NULL != m_pAgcParameter) { delete m_pAgcParameter; m_pAgcParameter = NULL; }
}

void CAutomaticGainControl::set(int formatid)
{
	if (formatid == 1) {
		m_format = MONO;
	}
	else if (formatid == 2) {
		m_format = STEREO;
	}
	else
	{ // so far AGC only support mon or stereo, do not support more channels.
		throw;
	}

}



// Update is called once every frame
void CAutomaticGainControl::process(const float *in, float *out, float *params)
{
	//firstly, get the vad result based on the audio in.
	m_snd_param.vad_flag = m_vad_inst->process(in);
	//secondly, do the AGC based on vad result 
	timeDomainAGC(in, out, params);

}

//AGC Algorithm
//it support mono or stereo and do not support more channels so far
void CAutomaticGainControl::timeDomainAGC(const float *in, float *out, float *params)
{
	//calculate the max value of the frame for mono or stereo
	const float *in_iter = in;
	float maxv = fabs(*in_iter);
	for (int i = 0; i < m_frame_len * m_channelN; i++) {
		float tmp = fabs(*in_iter);
		if (tmp > maxv) {
			maxv = tmp;
		}
		in_iter++;
	}

	float AGC_input_aver = m_pAgcParameter->AGC_pre_gain * maxv;
	float AGC_attack_mod = 1.f;
	if (AGC_input_aver > m_pAgcParameter->AGC_input_Th)
	{
		m_snd_param.vad_flag = 1.f;
		AGC_attack_mod = AGC_speed_attack;
	}

	if (m_snd_param.vad_flag > 0.5f) //VAD is active
	{
		if (AGC_input_aver > m_pAgcBuffer->AGC_input_level)
		{
			m_pAgcBuffer->AGC_input_level = (1.f - m_pAgcParameter->AGC_alfa_a_ppeak) * m_pAgcBuffer->AGC_input_level + m_pAgcParameter->AGC_alfa_a_ppeak * AGC_input_aver;
		}			
		else 
		{
			m_pAgcBuffer->AGC_input_level = (1.f - m_pAgcParameter->AGC_alfa_d_ppeak) * m_pAgcBuffer->AGC_input_level + m_pAgcParameter->AGC_alfa_d_ppeak * AGC_input_aver;
		}
		
		float AGC_new_gain = m_pAgcParameter->AGC_target / fmaxf(m_pAgcBuffer->AGC_input_level, m_pAgcParameter->AGC_target / m_pAgcParameter->AGC_max_gain);

		if (AGC_new_gain > m_pAgcBuffer->AGC_gain)
		{
			m_pAgcBuffer->AGC_gain = (1.f - m_pAgcParameter->AGC_decay) * m_pAgcBuffer->AGC_gain + m_pAgcParameter->AGC_decay * AGC_new_gain;
		}			
		else
		{
			m_pAgcBuffer->AGC_gain = (1.f - AGC_attack_mod * m_pAgcParameter->AGC_attack) * m_pAgcBuffer->AGC_gain + AGC_attack_mod * m_pAgcParameter->AGC_attack * AGC_new_gain;
		}
			
		m_pAgcBuffer->AGC_gain = fmaxf(fminf(m_pAgcBuffer->AGC_gain, m_pAgcParameter->AGC_max_gain), m_pAgcParameter->AGC_min_gain);

		if (((m_pAgcBuffer->AGC_gain - m_pAgcBuffer->AGC_real_gain) > m_pAgcParameter->AGC_adap_Th_linear*m_pAgcBuffer->AGC_real_gain) ||
			((m_pAgcBuffer->AGC_gain - m_pAgcBuffer->AGC_real_gain) < -(m_pAgcParameter->AGC_adap_Th_linear / (1 + m_pAgcParameter->AGC_adap_Th_linear))*m_pAgcBuffer->AGC_real_gain))
		{
			m_pAgcBuffer->AGC_real_gain = m_pAgcBuffer->AGC_gain;
		}
			
	}

	// find compression threshold for new gain value
	//     Comp_thresh = AGC_target / AGC_real_gain;
	if (m_pAgcBuffer->AGC_real_gain * maxv > m_pAgcParameter->AGC_target_max)
	{
		m_pAgcBuffer->AGC_real_gain = m_pAgcParameter->AGC_target_max / maxv;
	}
		
	// Apply Gain and Compressor(if needed)	
	in_iter = in;
	float *out_iter = out;
	for (int i = 0; i < m_frame_len; i++) {
		float AGC_curr_gain = (m_pAgcBuffer->AGC_real_gain_prev * (m_frame_len - (i + 1)) + m_pAgcBuffer->AGC_real_gain * (i + 1)) / (float)m_frame_len;
		for (int j = 0; j < m_channelN; j++)
		{
			*out_iter = AGC_curr_gain * (*in_iter);
			*out_iter = fmaxf(*out_iter, -m_threshold);
			*out_iter = fminf(*out_iter, m_threshold);
			in_iter++;
			out_iter++;
		}

	}

	m_pAgcBuffer->AGC_real_gain_prev = m_pAgcBuffer->AGC_real_gain;
	
	//update sound parameters
	m_snd_param.agc_gain = m_pAgcBuffer->AGC_real_gain_prev;
	params[0] = m_snd_param.agc_gain;
	params[1] = m_snd_param.vad_flag;
	params[2] = 0.f; //for reserved
	
}


