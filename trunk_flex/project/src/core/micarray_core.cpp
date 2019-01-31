#include "../doa/localizer.h"
#include "../ans/ans.h"
#include "../ans/beamforming.h"
#include "../aec/aec.h"
#include "micarray_core.h"
#include <stdio.h>
#include <string.h>

#define LENGTH_16K 256
#define LENGTH     512
#define BUFFER_LEN_16K   512
#define BUFFER_LEN       1024


enum SampleRate_e
{
	SAMPLE_RATE_16000 = 16000,
	SAMPLE_RATE_32000 = 32000,
	SAMPLE_RATE_44100 = 44100,
	SAMPLE_RATE_48000 = 48000
};
typedef SampleRate_e SampleRate;

#define RELEASE_POINTER(p) \
	if (NULL != p) { \
		delete p; \
		p = NULL; \
			}

#define RELEASE_ARRAY(p) \
	if (NULL != p) { \
		delete [] p; \
		p = NULL; \
			}

CMicArrayCore::CMicArrayCore(int profile_id, int frame_length, int sample_rate, int mic_channel_num, float mic_dist, int ref_channel_num, int aec_sys_latency)
	: m_profile_id(1)
	, m_frame_length(256)
	, m_sample_rate(16000)
	, m_mic_channel_num(2)
	, m_mic_dist(0.06f)
	, m_ref_channel_num(1)
	, m_out_channel_num(2)
	, m_aec_filter_len(1024)
	, m_aec_sys_latency(0)
	, m_doa_info_len(10)
	, m_doa_current(0)
	, m_status(0)
	, m_multithread(false)
	, m_audio_in(NULL)
	, m_audio_out(NULL)
	, m_audio_ref(NULL)
	, m_buffer_in(NULL)
	, m_buffer_ref(NULL)
	, m_buffer_out(NULL)
	, m_data_out(NULL)
	, m_doa_info(NULL)
	, m_audio_tmp_buf(NULL)
	, m_aec_array(NULL)
	//, m_aec(NULL)
	, m_beamforming(NULL)
	, m_localizer(NULL)
{
	m_evaluation = false;
	m_evaluation_timer = 0;
	m_evaluation_period = 30 * 60 * (sample_rate / frame_length);

	m_profile_id = profile_id;
	m_frame_length = frame_length;
	m_mic_channel_num = mic_channel_num;
	m_mic_dist = mic_dist;
	m_ref_channel_num = ref_channel_num;
	m_aec_sys_latency = aec_sys_latency;


	switch (sample_rate) {
	case SAMPLE_RATE_16000: {
		m_sample_rate = 16000;
		break;
	}
	case SAMPLE_RATE_32000: {
		m_sample_rate = 32000;
		break;
	}
	case SAMPLE_RATE_44100: {
		m_sample_rate = 44100;
		break;
	}
	case SAMPLE_RATE_48000: {
		m_sample_rate = 48000;
		break;
	}
	default: {
		throw;
	}
	}

	if (m_sample_rate == SAMPLE_RATE_16000)  {		
		m_length_in = BUFFER_LEN_16K;
		m_length_out = BUFFER_LEN_16K;
		m_process_length = LENGTH_16K;
		if (m_frame_length > 256) {
			throw;
		}
		
	}
	else {
		m_length_in = BUFFER_LEN;
		m_length_out = BUFFER_LEN;
		m_process_length = LENGTH;
		if (m_frame_length > 512) {
			throw;
		}
	}

	m_num_read = 0;
	m_num_write = m_process_length;
	 
	m_buffer_in = new float[m_mic_channel_num * m_length_in];
	memset(m_buffer_in, 0x00, sizeof(float) * m_mic_channel_num * m_length_in);
	m_buffer_ref = new float[m_ref_channel_num * m_length_in];
	memset(m_buffer_ref, 0x00, sizeof(float) * m_ref_channel_num * m_length_in);
	m_buffer_out = new float[m_length_out];
	memset(m_buffer_out, 0x00, sizeof(float) * m_length_out);
	m_data_out = new float[m_process_length];
	memset(m_data_out, 0x00, sizeof(float) * m_process_length);

	m_audio_in = new float[m_process_length*m_mic_channel_num];
	m_audio_out = new float[m_process_length*m_out_channel_num];
	m_audio_ref = new float[m_process_length*m_ref_channel_num];
	m_doa_info = new float[m_doa_info_len];
	m_audio_tmp_buf = new float[m_process_length];

	m_aec_array = new CAec*[m_mic_channel_num];
	for (int i = 0; i < m_mic_channel_num; i++) {
		m_aec_array[i] = new CAec(m_process_length, m_aec_filter_len, m_sample_rate, 1, ref_channel_num, m_aec_sys_latency);
	}
	//m_aec = new CAec(m_frame_length, m_aec_filter_len, m_sample_rate, mic_channel_num, ref_channel_num, m_aec_sys_latency);

	m_beamforming = new CBeamforming(m_profile_id, m_process_length, mic_channel_num, m_sample_rate, m_mic_dist, m_multithread);
	m_localizer = new CLocalizer(m_profile_id, m_process_length, mic_channel_num, m_sample_rate, m_mic_dist, m_multithread);


	m_aec_param.enableRes = false;
	m_aec_param.resLevel = 1.0;
	m_aec_param.enableNS = false;
	m_aec_param.nsDB = -10;
	m_aec_param.enableSpkClip = false;
	m_aec_param.spkClipThd = 1.0;
	m_aec_param.maxCoupling = 4.0;
	m_aec_param.adaptStep = 0.5;

	m_beam_param.mode = 7;
	m_beam_param.max_sup_db = -10.0;
	m_beam_param.targetDirection = 0;
	m_beam_param.targetEstimation = 0;
	m_beam_param.nlp_sup = 0.5;
	m_beam_param.tolerance = 30.0;
	m_beam_param.targetFlag = 0;

	set_aec(m_aec_param.enableRes, m_aec_param.resLevel, m_aec_param.enableNS, m_aec_param.nsDB, m_aec_param.enableSpkClip, m_aec_param.spkClipThd, m_aec_param.maxCoupling, m_aec_param.adaptStep);
	set_beamforming(m_beam_param.mode, m_beam_param.max_sup_db, m_beam_param.targetDirection, m_beam_param.targetEstimation, m_beam_param.nlp_sup, m_beam_param.tolerance, m_beam_param.targetFlag);
	set_doa(m_mic_channel_num);
}

CMicArrayCore::~CMicArrayCore()
{
	for (int i = 0; i < m_mic_channel_num; i++) {
		RELEASE_POINTER(m_aec_array[i]);
	}
	RELEASE_ARRAY(m_aec_array);

	//RELEASE_POINTER(m_aec);
	RELEASE_POINTER(m_beamforming);
	RELEASE_POINTER(m_localizer);
	RELEASE_ARRAY(m_audio_in);
	RELEASE_ARRAY(m_audio_out);
	RELEASE_ARRAY(m_audio_ref);
	RELEASE_ARRAY(m_buffer_in);
	RELEASE_ARRAY(m_buffer_ref);
	RELEASE_ARRAY(m_buffer_out);
	RELEASE_ARRAY(m_doa_info);
	RELEASE_ARRAY(m_audio_tmp_buf);

}

void CMicArrayCore::process(float *audio_input, float *audio_output, float *audio_reference)
{
	if (m_evaluation)
	{
		m_evaluation_timer++;
		if (m_evaluation_timer > m_evaluation_period)
			return;
	}

	if (m_aec_array == NULL) throw;
	//if (m_aec == NULL) throw;
	if (m_localizer == NULL) throw;
	if (m_beamforming == NULL) throw;


	for (int i = 0; i < m_mic_channel_num; i++) {
		m_buffer_in_iter = m_buffer_in + i * m_length_in + m_num_read;
		m_in_iter = audio_input + i;
		for (int j = 0; j < m_frame_length; j++) {
			*m_buffer_in_iter = *m_in_iter;
			m_buffer_in_iter++;  m_in_iter += m_mic_channel_num; 
		}
	}
	for (int i = 0; i < m_ref_channel_num; i++) {
		m_buffer_ref_iter = m_buffer_ref + i * m_length_in + m_num_read;
		m_ref_iter = audio_reference + i;
		for (int j = 0; j < m_frame_length; j++) {
			*m_buffer_ref_iter = *m_ref_iter;
			 m_buffer_ref_iter++;  m_ref_iter += m_ref_channel_num;
		}
	}
	m_num_read += m_frame_length;

	if (m_num_read >= m_process_length) {
		for (int i = 0; i < m_mic_channel_num; i++) {
			m_in_iter = m_audio_in + i;
			m_buffer_in_iter = m_buffer_in + i * m_length_in;
			for (int j = 0; j < m_process_length; j++) {
				*m_in_iter = *m_buffer_in_iter;
				m_in_iter += m_mic_channel_num; m_buffer_in_iter++; 
			}
		}

		for (int i = 0; i < m_ref_channel_num; i++) {
			m_ref_iter = m_audio_ref + i;
			m_buffer_ref_iter = m_buffer_ref + i * m_length_in;
			for (int j = 0; j < m_process_length; j++) {
				*m_ref_iter = *m_buffer_ref_iter;
				m_ref_iter += m_ref_channel_num; m_buffer_ref_iter++;
			}
		}
		m_num_read -= m_process_length;
		for (int i = 0; i < m_mic_channel_num; i++) {
			memcpy(m_buffer_in + i * m_length_in, m_buffer_in + i * m_length_in + m_process_length, sizeof(float)*(m_length_out - m_process_length));
		}

		for (int i = 0; i < m_ref_channel_num; i++) {
			memcpy(m_buffer_ref + i * m_length_in, m_buffer_ref + i * m_length_in + m_process_length, sizeof(float)*(m_length_out - m_process_length));
		}

		if (m_status == 0)  //not waked up, beamforming needn't work.
		{
			//memcpy(m_audio_ref, audio_reference, sizeof(float)*m_ref_channel_num*m_frame_length);
			//memcpy(m_audio_in, audio_input, sizeof(float)*m_mic_channel_num*m_frame_length);
			//m_aec->process(m_audio_ref, m_audio_in);
			for (int i = 0; i < m_mic_channel_num; i++)
			{
				for (int j = 0; j < m_process_length; j++)
				{
					m_audio_tmp_buf[j] = m_audio_in[j*m_mic_channel_num + i];
				}
				m_aec_array[i]->process(m_audio_ref, m_audio_tmp_buf);
				for (int j = 0; j < m_process_length; j++)
				{
					m_audio_in[j*m_mic_channel_num + i] = m_audio_tmp_buf[j];
				}
			}

			m_localizer->process(m_audio_in, m_doa_info);
			m_doa_current = m_doa_info[0];

			for (int i = 0; i < m_process_length; i++)
			{
				m_data_out[i] = m_audio_in[i*m_mic_channel_num];
			}
		}
		else if (m_status == 1)  //waked up, aec needn't work.
		{
			m_localizer->process(m_audio_in, m_doa_info);
			m_doa_current = m_doa_info[0];

			m_beam_param.targetEstimation = m_doa_current;
            m_beam_param.targetFlag = m_localizer->getState(m_beam_param.targetDirection);
		    //printf( "%d\n ", m_beam_param.targetFlag);
		    m_beamforming->set(m_beam_param.mode, m_beam_param.max_sup_db, m_beam_param.targetDirection, m_beam_param.targetEstimation, m_beam_param.nlp_sup, m_beam_param.tolerance, m_beam_param.targetFlag);
			m_beamforming->process(m_audio_in, m_audio_out);

			for (int i = 0; i < m_process_length; i++)
			{
				m_data_out[i] = m_audio_out[i*m_out_channel_num];
			}
		}

		m_buffer_out_iter = m_buffer_out + m_num_write;
		m_out_iter = m_data_out;
		for (int j = 0; j < m_process_length; j++) {
			*m_buffer_out_iter = *m_out_iter;
			m_buffer_out_iter++; m_out_iter ++;
		}

		m_num_write += m_process_length;
	}

	m_out_iter = audio_output;
	m_buffer_out_iter = m_buffer_out;
	for (int j = 0; j < m_frame_length; j++) {
		*m_out_iter = *m_buffer_out_iter;
		m_out_iter ++; m_buffer_out_iter++;
	}
	memcpy(m_buffer_out, m_buffer_out + m_frame_length, sizeof(float) *(m_length_out - m_frame_length));
	m_num_write -= m_frame_length;
}

void CMicArrayCore::set_status(int status)
{
	m_status = status;
}

void CMicArrayCore::set_aec(bool enableRes, float resLevel, bool enableNS, float nsDB, bool enableSpkClip, float spkClipThd, float maxCoupling, float adaptStep)
{
	m_aec_param.enableRes = enableRes;
	m_aec_param.resLevel = resLevel;
	m_aec_param.enableNS = enableNS;
	m_aec_param.nsDB = nsDB;
	m_aec_param.enableSpkClip = enableSpkClip;
	m_aec_param.spkClipThd = spkClipThd;
	m_aec_param.maxCoupling = maxCoupling;
	m_aec_param.adaptStep = adaptStep;

	//if (m_aec == NULL) throw;
	//m_aec->set(enableRes, resLevel, enableNS, nsDB, enableSpkClip, spkClipThd, maxCoupling, adaptStep);
	if (m_aec_array == NULL) throw;
	for (int i = 0; i < m_mic_channel_num; i++)
	{
		m_aec_array[i]->set(m_aec_param.enableRes, m_aec_param.resLevel, m_aec_param.enableNS, m_aec_param.nsDB, m_aec_param.enableSpkClip, m_aec_param.spkClipThd, m_aec_param.maxCoupling, m_aec_param.adaptStep);
	}
}

void CMicArrayCore::set_beamforming(int mode, float max_sup_db, float targetDirection, float targetEstimation, float nlp_sup, float tolerance, int targetFlag)
{
	if (m_beamforming == NULL) throw;
	m_beam_param.mode = mode;
	m_beam_param.max_sup_db = max_sup_db;
	m_beam_param.targetDirection = targetDirection;
	m_beam_param.targetEstimation = targetEstimation;
	m_beam_param.nlp_sup = nlp_sup;
	m_beam_param.tolerance = tolerance;
	m_beam_param.targetFlag = targetFlag;

	m_beamforming->set(m_beam_param.mode, m_beam_param.max_sup_db, m_beam_param.targetDirection, m_beam_param.targetEstimation, m_beam_param.nlp_sup, m_beam_param.tolerance, m_beam_param.targetFlag);
}

void CMicArrayCore::set_doa(int formatid)
{
	if (m_localizer == NULL) throw;
	m_localizer->set(formatid);
}

void CMicArrayCore::set_target_doa(float target_direction)
{
	m_beam_param.targetDirection = target_direction;
}

float CMicArrayCore::get_current_doa()
{
	return m_doa_current;
}
