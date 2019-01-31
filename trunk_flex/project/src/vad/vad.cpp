// #include "../core/config.h"
#include "vad.h"
#ifndef VAD


#include <stdio.h>
#include <math.h>
#include <string.h>
#include "vad.h"


CVad::CVad(int frame_length, int channelN, int sample_rate) : m_frame_len(frame_length)
, m_N(channelN)
, m_sampleRate(sample_rate)
, m_vadFlag(0)	//KHT m_VadFlag: 0 -- silence, 1 -- active
, m_state(STATUS_SILENT_BEFORE_START)
, m_pVadBuffer(NULL)
, m_pVadParameter(NULL)
{
#ifdef EVALUATION
	m_evaluation = false;
	m_evaluation_timer = 0;
	m_evaluation_period = 30 * 60 * (sample_rate / frame_length);
#endif
	//KHT for timeDomain VAD 
	m_pVadBuffer = new VAD_BUFFER;
	memset(m_pVadBuffer, 0, sizeof(VAD_BUFFER));
	m_pVadBuffer->VAD_E_Filter = -10.f;

	m_pVadParameter = new VAD_PARAMETER;
	m_pVadParameter->VAD_attack = 1.0 - exp(-1 / (VAD_T_a * (m_sampleRate / m_frame_len)));
	m_pVadParameter->VAD_decay = 1.0 - exp(-1 / (VAD_T_d * (m_sampleRate / m_frame_len)));
	m_pVadParameter->VAD_envelope = 1.0 - exp(-1 / (VAD_T_envelope*(m_sampleRate / m_frame_len)));
	m_pVadParameter->VAD_init_frame = round(VAD_init * m_sampleRate / m_frame_len);
	m_pVadParameter->VAD_count_Th_frame = round(VAD_count_Th_time * m_sampleRate / m_frame_len);

	m_active_cnt = 0;
	m_silent_cnt = 0;
}

CVad::~CVad()
{
	if (NULL != m_pVadBuffer) { 
		delete m_pVadBuffer; 
		m_pVadBuffer = NULL;
	}
	if (NULL != m_pVadParameter) {
		delete m_pVadParameter; 
		m_pVadParameter = NULL; 
	}	
}

void CVad::set()
{
}


float CVad::process(const float *in)
{	
#ifdef EVALUATION
	if (m_evaluation)
	{
		m_evaluation_timer++;
		if (m_evaluation_timer > m_evaluation_period)
			return -1;
	}
#endif
	m_vadFlag = timeDomainVAD(in, 0);	
	return (float)m_vadFlag;	
}
//for some customer, they only care for the start and end points
//so provide the interface below:
//\return: 1 means voice start, 2 means voice in progress, 3 means end of the voice
//         for the first time, return 1, means the start of the voice(one sentence)
//         for the first time, return 3, means the end of the voice(one sentence)
int CVad::getState(const float *in)
{
#ifdef EVALUATION
	if (m_evaluation)
	{
		m_evaluation_timer++;
		if (m_evaluation_timer > m_evaluation_period)
			return -1;
	}
#endif
	int vad = timeDomainVAD(in, 0);
	if (vad == 1)
	{
		m_active_cnt++;
		m_silent_cnt = 0;
	}
	else
	{
		m_silent_cnt++;
		m_active_cnt = 0;
	}
	if (m_active_cnt > 10) //KHT try to change from 5 to 10, try to begin late
	{
		m_state = STATUS_QUESTION_START;
	}
	if (m_state == STATUS_QUESTION_START && m_silent_cnt > 15)  //try to change from 30 to 15, try to end early
	{
		m_state = STATUS_QUESTION_END;
	}


	return (int)m_state;

}




int CVad::timeDomainVAD(const float* in, int c)
{
	// only pick up one channel to do VAD detection
	const float *in_iter = in + c;

	//  VAD Algorithm
	float max_p = deltaA;
	float temp = 0.f;
	for (int i = 0; i < m_frame_len; i++)
	{
		temp = *in_iter * *in_iter;
		if (temp > max_p)
		{
			max_p = temp;
		}
		in_iter += m_N;
	}
	temp = VAD_log10tolog2 * (log(max_p) / log(2.0f));

	// Computing VAD_E_Filter, VAD_E_Filter_max, VAD_E_Filter_min.      
	m_pVadBuffer->VAD_E_Filter = (1 - m_pVadParameter->VAD_envelope) * m_pVadBuffer->VAD_E_Filter + m_pVadParameter->VAD_envelope * temp;

	if (m_pVadBuffer->VAD_counter > m_pVadParameter->VAD_init_frame)
	{
		if (m_pVadBuffer->VAD_E_Filter > m_pVadBuffer->VAD_E_Filter_max)
		{
			m_pVadBuffer->VAD_E_Filter_max = m_pVadBuffer->VAD_E_Filter;
		}
		else
		{
			m_pVadBuffer->VAD_E_Filter_max = (1 - m_pVadParameter->VAD_decay) * m_pVadBuffer->VAD_E_Filter_max + m_pVadParameter->VAD_decay * m_pVadBuffer->VAD_E_Filter;
		}

		if (m_pVadBuffer->VAD_E_Filter > m_pVadBuffer->VAD_E_Filter_min)
		{
			m_pVadBuffer->VAD_E_Filter_min = (1 - m_pVadParameter->VAD_attack) * m_pVadBuffer->VAD_E_Filter_min + m_pVadParameter->VAD_attack * m_pVadBuffer->VAD_E_Filter;;
		}
		else
		{
			m_pVadBuffer->VAD_E_Filter_min = m_pVadBuffer->VAD_E_Filter;
		}

	}
	else
	{
		m_pVadBuffer->VAD_E_Filter_max = m_pVadBuffer->VAD_E_Filter;
		m_pVadBuffer->VAD_E_Filter_min = m_pVadBuffer->VAD_E_Filter;
		m_pVadBuffer->VAD_counter++;
	}

	float VAD_delta_Filter = m_pVadBuffer->VAD_E_Filter_max - m_pVadBuffer->VAD_E_Filter_min;
	float VAD_delta_Dynamic = m_pVadBuffer->VAD_E_Filter - m_pVadBuffer->VAD_E_Filter_min;

	//VAD detection
	int	vad = 0;
	if (VAD_delta_Dynamic > VAD_ratio * VAD_delta_Filter)
	{
		vad = 1;
	}
	if (m_pVadBuffer->VAD_E_Filter_max < VAD_Min_Max_Env)
	{
		vad = 0;
	}
	if (VAD_delta_Filter < VAD_Min_Signal)
	{
		vad = 0;
	}

	float VAD_delta_diff = VAD_delta_Filter - VAD_delta_Dynamic;
	if (VAD_delta_diff < VAD_delta_Th)
	{
		if (m_pVadBuffer->VAD_counter_delta < m_pVadParameter->VAD_count_Th_frame)
		{
			m_pVadBuffer->VAD_counter_delta++;
		}
		else
		{
			vad = 0;
		}
	}
	else
	{
		m_pVadBuffer->VAD_counter_delta = 0;
	}

	return vad;
}

#endif