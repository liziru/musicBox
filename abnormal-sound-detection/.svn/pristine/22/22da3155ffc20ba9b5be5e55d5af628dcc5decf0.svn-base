#include "localizer.h"
#include <string.h>
#include <math.h>
#include "../vad/vad.h"

#ifndef max
#define max(a,b) ((a) >= (b) ? (a) : (b))
#endif

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
#define FFT_LEN_16k (512)    //add window for fft
#define FFT_LEN (1024)
#define M (72)              // control the estimation granularity 72/pi*180 = 2.5   
#define SMALL_VALUE (1e-4f)
#define F_LOW (1000)   //the band can be tuned
#define F_HIGH (4000)  //the band can be tuned

#define SMOOTH_FACTOR (0.9f)   //KHT smooth between frames in time
#define SMOOTH_FACTOR_NOISE (0.95f)   

CLocalizer::CLocalizer(int profileID, int frame_length, int channelN, int sampleRate, float micDist, bool multithread /*= true*/) : m_N(2)
														    , m_first(true)
															, m_real_fft(NULL)
															, m_multithread(multithread)
															, m_data_real(NULL)
															, m_data_spec(NULL)
															, m_win(NULL)
															, m_data_real_win(NULL)
															, m_steerv_broadside(NULL)
	                                                        , m_steerv(NULL)
															, m_sigma_broadside_c(NULL)
															, m_sigma_broadside_sm_c(NULL)
															, m_sigma_smooth_factor(SMOOTH_FACTOR)
															, m_format(0)
	                                                        , m_alpha(NULL)
	                                                        , m_factor(NULL)
	                                                        , m_amp_sum(NULL)
	                                                        , m_amp_sm(NULL)
	                                                        , m_Rmax(NULL)
	                                                        , m_targetDirection(0)
	                                                        , m_targetDirectionIndex(0)
	                                                        , m_width(15)
	                                                        , m_widthIndex(0)
	                                                        , m_targetNum(0)
	                                                        , m_targetFlag_prev(0)
															, m_vad_inst(NULL)
{
	m_profileID = profileID;
	m_frame_len = frame_length;
	m_sampleRate = sampleRate;
	m_N = channelN;
	m_soundSpeed = 346.287f;
	m_micDistance = micDist;
	if (m_sampleRate == 16000) {
		m_fft_len = FFT_LEN_16k;
	}
	else {
		m_fft_len = FFT_LEN;
	}
	m_deltFrequency = (float)m_sampleRate / m_fft_len;

	if (m_profileID == AMBISONIC) {
		m_N = 3;
		m_alpha = new float[m_N];
		
		m_alpha[0] = 0;
		m_alpha[1] = 2 * M_PI / 3;
		m_alpha[2] = -2 * M_PI / 3;

		m_factor = new float[m_N];
		m_factor[0] = 0.5f;
		m_factor[1] = -0.25f;
		m_factor[2] = sqrtf(3.f) * 0.25f;
		
		m_amp_sum = new float[m_N];
		m_amp_sm = new float[m_N];

		m_low_bnd_idx = (short)(m_fft_len / (float)m_sampleRate * 1000);
		m_up_bnd_idx = (short)(m_fft_len / (float)m_sampleRate * 4000);
		m_bnd_num = m_up_bnd_idx - m_low_bnd_idx;
	}
	else {
		//findClosestFrequencey(F_LOW, &m_low_bnd_freq, &m_low_bnd_idx);
		//findClosestFrequencey(F_HIGH, &m_high_bnd_freq, &m_up_bnd_idx);	
		float centerFreq = m_soundSpeed / (m_micDistance * 2);
		if (m_profileID == CIRCULAR) {
			if (m_N != 7) {
				centerFreq = m_soundSpeed / (2 * micDist * (float)sin(M_PI / m_N) * 2);
			}

		}
		m_low_bnd_idx = (short)(m_fft_len / (float)m_sampleRate * (centerFreq*0.2));
		m_up_bnd_idx = (short)(m_fft_len / (float)m_sampleRate * (centerFreq*0.9));
		m_bnd_num = m_up_bnd_idx - m_low_bnd_idx;
	}
	
	m_Rmax = new float[M];
	m_steerv_broadside = new C[m_bnd_num * m_N * M];
	m_steerv = new float[m_N * M];
	steerv();

	m_sigma_broadside_c = new C[m_bnd_num * (m_N - 1)]; //gcc_phat based on mic1 -- interleaves frequency with the other mics	
	memset(m_sigma_broadside_c, 0x00, sizeof(C) * m_bnd_num * (m_N - 1));
	m_sigma_broadside_sm_c = new C[m_bnd_num * (m_N - 1)]; //gcc_phat smoothed
	memset(m_sigma_broadside_sm_c, 0x00, sizeof(C) * m_bnd_num * (m_N - 1));
	m_sigma_broadside_sm_norm_c = new C[m_bnd_num * (m_N - 1)]; //gcc_phat smoothed and normlized 
	memset(m_sigma_broadside_sm_norm_c, 0x00, sizeof(C) * m_bnd_num * (m_N - 1));


	if (m_frame_len > m_fft_len)
		throw;
	m_fft_len_div_2 = m_fft_len >> 1;
	m_overlap_len = m_frame_len;   //KHT frame len = 256, fft_len = 512, overlap_len = 256	
	
	int fft_num = m_multithread ? m_N : 1;
	m_real_fft = new CRealFFT*[fft_num];
	uint logN = (uint)(log((float)m_fft_len) / log(2.f) + 0.5f);
	for (int i = 0; i < fft_num; i++) {
		m_real_fft[i] = new CRealFFT(logN);  //andy: real fft
	}

	m_data_spec = new C[m_N * (m_fft_len_div_2 + 1)];
	m_data_real = new R[m_N * m_fft_len];
	memset(m_data_real, 0x00, sizeof(R) * m_N * m_fft_len);

	m_data_real_win = new R[m_N * m_fft_len];
	memset(m_data_real_win, 0x00, sizeof(R) * m_N * m_fft_len);

	m_win = new float[m_frame_len * 2];  //KHT changed to fft length	
	for (int i = 0; i < m_frame_len * 2; i++) {
		float ang = M_PI * (float)(i+1.f) / (float)(m_frame_len * 2);
		m_win[i] = sin(ang);		
	}

	m_vad_inst = new CVad(m_frame_len, m_N, m_sampleRate);
	
}

void CLocalizer::steerv()
{
	if (m_profileID == AMBISONIC) {
		float *steerv_iter = m_steerv;
		for (int k = 0; k < M; k++) {
			float alpha = 2.f * M_PI * (float)k / (float)M;
			for (int i = 0; i < m_N; i++) {
				*(steerv_iter + i * M) = 0.5f + 0.5f * cos(m_alpha[i] - alpha);  //the location is 0,120, -120
			}
			steerv_iter++;
		}
	}
	else if (m_profileID == BROADSIDE)  //BROADSIDE
	{
		/*
		omega = 2 * pi*f0;                % Angular frequency of the signal
		k0 = omega / c;                      % wave number
		phi = -k0*dist*cos(theta_test);    % Electrical angle
		for i = 1:M
		a(i, :) = exp(j*(i - 1)*phi);    % Steering vector
		end
		*/

		float dist = m_micDistance;
		float c0 = m_soundSpeed;                    // Speed of sound in air

		for (int b = 0; b < m_bnd_num; b++) {
			float f0 = ((m_low_bnd_idx+1) + (b+1)) * m_deltFrequency; //KHT to match matlab for index difference
			float omega = 2 * M_PI*f0;
			float k0 = omega / c0;
			C *steerv_iter = m_steerv_broadside + b * m_N * M;
			for (int k = 0; k < M; k++) {
				float alpha = M_PI * (float)(k) /(float)(M);
				float phi = -k0 * dist * cos(alpha);
				for (int i = 0; i < m_N; i++) {
					(steerv_iter + i * M)->re = cos((float)i * phi);
					(steerv_iter + i * M)->im = sin((float)i * phi);
				}
				steerv_iter++;
			}
		}
	}	
	else if(m_profileID == CIRCULAR)  
	{
		float dist = m_micDistance;
		float c0 = m_soundSpeed;                    // Speed of sound in air

		if (m_N == 7)  //6+1
		{
			for (int b = 0; b < m_bnd_num; b++) {
				float f0 = ((m_low_bnd_idx + 1) + (b + 1)) * m_deltFrequency; //KHT to match matlab for index difference
				float omega = 2 * M_PI*f0;
				float k0 = omega / c0;
				C *steerv_iter = m_steerv_broadside + b * m_N * M;
				for (int k = 0; k < M; k++) {
					float alpha = 2 * M_PI * (float)(k) / (float)(M);
					for (int i = 1; i < m_N; i++) {  //the first mic is in the center
						float phi = -k0 * dist * cos(alpha - 2 * (i - 1) * M_PI / (m_N - 1));
						(steerv_iter + i * M)->re = cos(phi);
						(steerv_iter + i * M)->im = sin(phi);
					}
					steerv_iter++;
				}
			}
		}
		else if (m_N == 3) {
			for (int b = 0; b < m_bnd_num; b++) {
				float f0 = ((m_low_bnd_idx + 1) + (b + 1)) * m_deltFrequency; //KHT to match matlab for index difference
				float omega = 2 * M_PI*f0;
				float k0 = omega / c0;
				C *steerv_iter = m_steerv_broadside + b * m_N * M;
				for (int k = 0; k < M; k++) {
					float alpha = 2 * M_PI * (float)(k) / (float)(M);
					for (int i = 0; i < m_N; i++) {  //the first mic is in the center
						float phi = -k0 * 2 * dist * sin(M_PI * i / m_N) * cos(alpha + (M_PI - 2 * i * M_PI / m_N)/2);
						(steerv_iter + i * M)->re = cos(phi);
						(steerv_iter + i * M)->im = sin(phi);
					}
					steerv_iter++;
				}
			}
		}
		else if (m_N == 4) {
			for (int b = 0; b < m_bnd_num; b++) {
				float f0 = ((m_low_bnd_idx + 1) + (b + 1)) * m_deltFrequency; //KHT to match matlab for index difference
				float omega = 2 * M_PI*f0;
				float k0 = omega / c0;
				C *steerv_iter = m_steerv_broadside + b * m_N * M;
				for (int k = 0; k < M; k++) {
					float alpha = 2 * M_PI * (float)(k) / (float)(M);
					for (int i = 0; i < m_N; i++) { 
						float phi = -k0 * 2 * dist * sin(M_PI * i / m_N) * cos(alpha + (M_PI - 2 * i * M_PI / m_N) / 2);
						(steerv_iter + i * M)->re = cos(phi);
						(steerv_iter + i * M)->im = sin(phi);
					}
					steerv_iter++;
				}
			}
		}
		else if (m_N == 6) {
			for (int b = 0; b < m_bnd_num; b++) {
				float f0 = ((m_low_bnd_idx + 1) + (b + 1)) * m_deltFrequency; //KHT to match matlab for index difference
				float omega = 2 * M_PI*f0;
				float k0 = omega / c0;
				C *steerv_iter = m_steerv_broadside + b * m_N * M;
				for (int k = 0; k < M; k++) {
					float alpha = 2 * M_PI * (float)(k) / (float)(M);
					for (int i = 0; i < m_N; i++) {  
						float phi = -k0 * 2 * dist * sin(M_PI * i / m_N) * cos(alpha + (M_PI - 2 * i * M_PI / m_N) / 2);
						(steerv_iter + i * M)->re = cos(phi);
						(steerv_iter + i * M)->im = sin(phi);
					}
					steerv_iter++;
				}
			}
		}
		else {
			throw;
		}	
	}
}

CLocalizer::~CLocalizer()
{		
	if (NULL != m_win) { delete[] m_win; m_win = NULL; }	
	if (NULL != m_data_real) { delete[] m_data_real; m_data_real = NULL; }	
	if (NULL != m_data_real_win) { delete[] m_data_real_win; m_data_real_win = NULL; }

	if (NULL != m_data_spec) { delete[] m_data_spec; m_data_spec = NULL; }
	if (NULL != m_steerv_broadside) { delete[] m_steerv_broadside; m_steerv_broadside = NULL; }
	if (NULL != m_steerv) { delete[] m_steerv; m_steerv = NULL; }
	if (NULL != m_alpha) { delete[] m_alpha; m_alpha = NULL; }
	if (NULL != m_factor) { delete[] m_factor; m_factor = NULL; }
	if (NULL != m_amp_sum) { delete[] m_amp_sum; m_amp_sum = NULL; }
	if (NULL != m_amp_sm) { delete[] m_amp_sm; m_amp_sm = NULL; }
	if (NULL != m_Rmax) { delete[] m_Rmax; m_Rmax = NULL; }

	
	if (NULL != m_sigma_broadside_sm_c) { delete[] m_sigma_broadside_sm_c; m_sigma_broadside_sm_c = NULL; }
	if (NULL != m_sigma_broadside_c) { delete[] m_sigma_broadside_c; m_sigma_broadside_c = NULL; }
	if (NULL != m_sigma_broadside_sm_norm_c) { delete[] m_sigma_broadside_sm_norm_c; m_sigma_broadside_sm_norm_c = NULL; }

	int fft_num = m_multithread ? m_N : 1;
	for (int i = 0; i < fft_num; i++) {
		if (NULL != m_real_fft[i]) { 
			delete m_real_fft[i];
			m_real_fft[i] = NULL; 
		}
	}
	
	if (NULL != m_vad_inst) { delete m_vad_inst; m_vad_inst = NULL; }
	if (NULL != m_real_fft) { delete[] m_real_fft; m_real_fft = NULL; }
}

//prepare the data and add window before fft
void CLocalizer::o_to_a(const float *in, int c)
{
	static float sqrt2 = sqrtf(2.f);
	memmove(m_data_real + m_fft_len * c, m_data_real + m_fft_len * c + m_frame_len, sizeof(float) * m_overlap_len);

	R *data_real_iter = m_data_real + m_fft_len * c + m_overlap_len;
	if (m_profileID == BROADSIDE)
	{
		switch (m_format) {
		case BROADSIDE_2MIC: {
			const float *in_iter = in + c;
			for (int i = 0; i < m_frame_len; i++) {
				*data_real_iter = *in_iter;
				in_iter += 2; data_real_iter++;
			}
			break;
		}
		case BROADSIDE_3MIC: {
			const float *in_iter = in + c;
			for (int i = 0; i < m_frame_len; i++) {
				*data_real_iter = *in_iter;
				in_iter += 3; data_real_iter++;
			}
			break;
		}
		case BROADSIDE_4MIC: {
			const float *in_iter = in + c;
			for (int i = 0; i < m_frame_len; i++) {
				*data_real_iter = *in_iter;
				in_iter += 4; data_real_iter++;
			}
			break;
		}
		case BROADSIDE_6MIC: {
			const float *in_iter = in + c;
			for (int i = 0; i < m_frame_len; i++) {
				*data_real_iter = *in_iter;
				in_iter += 6; data_real_iter++;
			}
			break;
		}
		default: {
			throw;
		}
		}
	}
	else if (m_profileID == AMBISONIC)
	{
		switch (m_format) {
		case AMBISONIC_3MIC: {
			const float *in_iter = in + c;
			for (int i = 0; i < m_frame_len; i++) {
				*data_real_iter = *in_iter;
				in_iter += 3; data_real_iter++;
			}
			break;
		}
		case WXY: {
			const float *w_iter = in, *x_iter = in + 1, *y_iter = in + 2;
			switch (c) {
			case 0: {
				for (int i = 0; i < m_frame_len; i++) {
					*data_real_iter = m_factor[0] * (*w_iter + *x_iter);
					data_real_iter++; w_iter += 3; x_iter += 3; y_iter += 3;
				}
				break;
			}
			case 1: {
				for (int i = 0; i < m_frame_len; i++) {
					*data_real_iter = m_factor[0] * *w_iter + m_factor[1] * *x_iter + m_factor[2] * *y_iter;
					data_real_iter++; w_iter += 3; x_iter += 3; y_iter += 3;
				}
				break;
			}
			case 2: {
				for (int i = 0; i < m_frame_len; i++) {
					*data_real_iter = m_factor[0] * *w_iter + m_factor[1] * *x_iter - m_factor[2] * *y_iter;
					data_real_iter++; w_iter += 3; x_iter += 3; y_iter += 3;
				}
				break;
			}
			default: {
				throw;
			}
			}
			break;
		}
		case A_FORMAT: {
			const float *lf_iter = in, *rf_iter = in + 1, *lb_iter = in + 2, *rb_iter = in + 3;
			float factor[12]; //A fomat is transformed to B format, then rotated by 135, and finally transformed to 3mic_format(0,120,-120)
			factor[0] = 0.2500;
			factor[1] = -0.3624;
			factor[2] = 0.8624;
			factor[3] = 0.2500;
			factor[4] = -0.2804;
			factor[5] = 0.5561;
			factor[6] = -0.0561;
			factor[7] = 0.7804;
			factor[8] = 0.7803;
			factor[9] = 0.5562;
			factor[10] = -0.0562;
			factor[11] = -0.2803;


			switch (c) {
			case 0: {
				for (int i = 0; i < m_frame_len; i++) {
					*data_real_iter = factor[0] * *lf_iter + factor[1] * *rf_iter + factor[2] * *lb_iter + factor[3] * *rb_iter;
					data_real_iter++; lf_iter += 4; rf_iter += 4; lb_iter += 4; rb_iter += 4;
				}
				break;
			}
			case 1: {
				for (int i = 0; i < m_frame_len; i++) {
					*data_real_iter = factor[4] * *lf_iter + factor[5] * *rf_iter + factor[6] * *lb_iter + factor[7] * *rb_iter;
					data_real_iter++; lf_iter += 4; rf_iter += 4; lb_iter += 4; rb_iter += 4;
				}
				break;
			}
			case 2: {
				for (int i = 0; i < m_frame_len; i++) {
					*data_real_iter = factor[8] * *lf_iter + factor[9] * *rf_iter + factor[10] * *lb_iter + factor[11] * *rb_iter;
					data_real_iter++; lf_iter += 4; rf_iter += 4; lb_iter += 4; rb_iter += 4;
				}
				break;
			}
			default: {
				throw;
			}
			}
			break;
		}
		case WXYZ_FUMA: {
			const float *w_iter = in, *x_iter = in + 1, *y_iter = in + 2;
			switch (c) {
			case 0: {
				for (int i = 0; i < m_frame_len; i++) {
					*data_real_iter = m_factor[0] * (sqrt2 * *w_iter + *x_iter);
					data_real_iter++; w_iter += 4; x_iter += 4; y_iter += 4;
				}
				break;
			}case 1: {
				for (int i = 0; i < m_frame_len; i++) {
					*data_real_iter = m_factor[0] * sqrt2 * *w_iter + m_factor[1] * *x_iter + m_factor[2] * *y_iter;
					data_real_iter++; w_iter += 4; x_iter += 4; y_iter += 4;
				}
				break;
			}
			case 2: {
				for (int i = 0; i < m_frame_len; i++) {
					*data_real_iter = m_factor[0] * sqrt2 * *w_iter + m_factor[1] * *x_iter - m_factor[2] * *y_iter;
					data_real_iter++; w_iter += 4; x_iter += 4; y_iter += 4;
				}
				break;
			}
			default: {
				throw;
			}
			}
			break;
		}
		case WYZX_SN3D: {
			const float *w_iter = in, *x_iter = in + 3, *y_iter = in + 1;
			switch (c) {
			case 0: {
				for (int i = 0; i < m_frame_len; i++) {
					*data_real_iter = m_factor[0] * (*w_iter + *x_iter);
					data_real_iter++; w_iter += 4; x_iter += 4; y_iter += 4;
				}
				break;
			}case 1: {
				for (int i = 0; i < m_frame_len; i++) {
					*data_real_iter = m_factor[0] * *w_iter + m_factor[1] * *x_iter + m_factor[2] * *y_iter;
					data_real_iter++; w_iter += 4; x_iter += 4; y_iter += 4;
				}
				break;
			}
			case 2: {
				for (int i = 0; i < m_frame_len; i++) {
					*data_real_iter = m_factor[0] * *w_iter + m_factor[1] * *x_iter - m_factor[2] * *y_iter;
					data_real_iter++; w_iter += 4; x_iter += 4; y_iter += 4;
				}
				break;
			}
			
			default: {
				throw;
			}
			}
			break;
		}
		default: {
			throw;
		}
		}
	}
	else if(m_profileID == CIRCULAR)
	{
		switch (m_format) {
		case CIRCULAR_3MIC: {
			const float *in_iter = in + c;
			for (int i = 0; i < m_frame_len; i++) {
				*data_real_iter = *in_iter;
				in_iter += 3; data_real_iter++;
			}
			break;
		}
		case CIRCULAR_4MIC: {
			const float *in_iter = in + c;
			for (int i = 0; i < m_frame_len; i++) {
				*data_real_iter = *in_iter;
				in_iter += 4; data_real_iter++;
			}
			break;
		}
		case CIRCULAR_6MIC: {
			const float *in_iter = in + c;
			for (int i = 0; i < m_frame_len; i++) {
				*data_real_iter = *in_iter;
				in_iter += 6; data_real_iter++;
			}
			break;
		}
		case CIRCULAR_6_1MIC: {
			const float *in_iter = in + c;
			for (int i = 0; i < m_frame_len; i++) {
				*data_real_iter = *in_iter;
				in_iter += 7; data_real_iter++;
			}
			break;
		}
		default: {
			throw;
		}
		}
	}
	else
	{
		throw;
	}
	for (int i = m_frame_len * 2; i < m_fft_len; i++)
	{
		*data_real_iter = 0;
		data_real_iter++;
	}
	data_real_iter = m_data_real + m_fft_len * c;
	R *data_real_iter_win = m_data_real_win + m_fft_len * c;
	const float *win_iter = m_win;
	for (int i = 0; i < m_frame_len * 2; i++) {
		*data_real_iter_win = *data_real_iter * *win_iter;
		data_real_iter_win++; data_real_iter++; win_iter++;
	}
}
void CLocalizer::set(int formatid)
{  //for broadside, it should be equal to channel number
	m_format = formatid;
}

//so far, only support single thread for the Localizer
void CLocalizer::process(const float *in, float *param)
{
#ifdef EVALUATION
	if (m_evaluation)
	{
		m_evaluation_timer++;
		if (m_evaluation_timer > m_evaluation_period)
			return;
	}
#endif
	m_snd_param.vad_flag = m_vad_inst->process(in);

	if (m_snd_param.vad_flag)
	{
		m_sigma_smooth_factor = SMOOTH_FACTOR;
	}
	else
	{
		m_sigma_smooth_factor = SMOOTH_FACTOR_NOISE;
	}
	
	for (int c = 0; c < m_N; c++) {
		o_to_a(in, c); //add window before fft
		fft(c);		
	}
	metric_c();	//calculate the sigma
	float directionAngle = get_target_angle(); //calculate Rmax and get the target angle

	m_snd_param.location = directionAngle;
	sound_power();
	m_snd_param.diffusivity = 0.1f;	//KHT for localizer only care for the location/angle now, other parameters will be fixed and not correct.
	m_snd_param.reliability = 1.f;
	m_snd_param.source_num = 1.f; //this is not supported in localizer - gcc-phat method for tdoa

	if (m_first) {
		m_snd_param.location_prev = 0.f;		
	}
	else {
		//if (m_snd_param.power < -60.0f) { // adjustment based on the sound power
		//	m_snd_param.location = m_snd_param.location_prev;
		//}
		m_snd_param.location_prev = m_snd_param.location;
	}

	while (m_snd_param.location > 180.f) {
		m_snd_param.location -= 360.f;
	}

	if (m_first) {
		m_first = false;
	}

	param[0] = m_snd_param.location;
	param[1] = m_snd_param.power;
	param[2] = m_snd_param.diffusivity;
	param[3] = m_snd_param.reliability;
	param[4] = m_snd_param.source_num;
	param[5] = m_snd_param.vad_flag;
}

int CLocalizer::getState(float targetDirection)
{
	m_targetDirection = targetDirection;
	if (m_profileID == 0) {
		if (m_targetDirection < 0)
			m_targetDirection += 360;

		m_targetDirectionIndex = (int)(m_targetDirection / 360 * M);
		if (m_targetDirectionIndex >= M || m_targetDirectionIndex<0)
			m_targetDirectionIndex = 0;

		m_widthIndex = (int)(m_width / 360 * M);
	}
	else if (m_profileID == 1) {
		m_targetDirection = 90 - m_targetDirection;   //0~180
		m_targetDirectionIndex = (int)(m_targetDirection / 180 * M);
		if (m_targetDirectionIndex >= M || m_targetDirectionIndex<0)
			m_targetDirectionIndex = 0;

		m_widthIndex = (int)(m_width / 180 * M);

	}
	else if (m_profileID == 2) {
		m_targetDirection += 180;
		if (m_targetDirection < 0)
			m_targetDirection += 360;

		m_targetDirectionIndex = (int)(m_targetDirection / 360 * M);
		if (m_targetDirectionIndex >= M || m_targetDirectionIndex<0)
			m_targetDirectionIndex = 0;

		m_widthIndex = (int)(m_width / 360 * M);
	}
	else {
		throw;
	}
	
	float minval = minValue();
	for (int i = 0; i < M; i++) {
		m_Rmax[i] -= minval;
	}
	float medianval = medianValue();

	int targetFlag = 0;
	if (m_snd_param.vad_flag) {
		int startPoint, endPoint;
		startPoint = m_targetDirectionIndex - m_widthIndex + 1;
		endPoint = m_targetDirectionIndex + m_widthIndex;
		if (m_profileID == 1) {
			if (startPoint < 1) {
				if ((m_Rmax[0] > m_Rmax[1]) && (m_Rmax[0] > 0.5f*medianval) && (m_Rmax[0] > 20)) {
					targetFlag = 1;
				}
			}
			else if (endPoint > M - 2) {
				if ((m_Rmax[M - 1] > m_Rmax[M - 2]) && (m_Rmax[M - 1] > 0.5f*medianval) && (m_Rmax[M - 1] > 20)) {
					targetFlag = 1;
				}
			}
			else {
				for (int i = startPoint; i < endPoint; i++) {
					if ((m_Rmax[i] > m_Rmax[i - 1]) && (m_Rmax[i] > m_Rmax[i + 1]) && (m_Rmax[i] > 0.5f*medianval) && (m_Rmax[i] > 20)) {
						targetFlag = 1;
					}
				}
			}
		}
		else {
			if (startPoint < 0 && endPoint > 0) {
				float buffer_tmp[M] = { 0.0 };
				int i;
				for (i = 0; i <(int) fabs((float)startPoint); i++)
					buffer_tmp[i] = m_Rmax[startPoint + M + i];

				for (; i <= m_width * 2; i++) {
					buffer_tmp[i] = m_Rmax[i - ((int)fabs((float)startPoint))];
				}

				for (int i = 1; i < m_width * 2; i++) {
					if ((buffer_tmp[i] > buffer_tmp[i - 1]) && (buffer_tmp[i] > buffer_tmp[i + 1]) && (buffer_tmp[i] > 0.5f*medianval) && (buffer_tmp[i] > 20)) {
						targetFlag = 1;
					}
				}
			}
			else if (startPoint >= 0 && startPoint <= M - 1) {
				if (startPoint == 0) {
					if ((m_Rmax[0] > m_Rmax[1]) && (m_Rmax[0] > 0.5f*medianval) && ((m_Rmax[0] > 20))) {
						targetFlag = 1;
					}
				}
				else if (endPoint == M - 1) {
					if ((m_Rmax[M - 1] > m_Rmax[M - 2]) && (m_Rmax[M - 1] > 0.5f*medianval) && ((m_Rmax[M - 1] > 20))) {
						targetFlag = 1;
					}
				}
				else {
					for (int i = startPoint; i < endPoint; i++) {
						if ((m_Rmax[i] > m_Rmax[i - 1]) && (m_Rmax[i] > m_Rmax[i + 1]) && (m_Rmax[i] > 0.5f*medianval) && ((m_Rmax[i] > 20))) {
							targetFlag = 1;
						}
					}
				}
			}
		}
	}

	if (targetFlag == 0) {
		m_targetNum++;
		if (m_targetNum < 30 && m_targetFlag_prev == 1) {
			targetFlag = 1;
		}
	}
	else {
		m_targetNum = 0;
	}

	m_targetFlag_prev = targetFlag;

	return targetFlag;
}

void CLocalizer::sound_power()
{
	m_snd_param.power = 0.f;
	for (int i = 0; i < m_frame_len; i++) {
		float tmp = 0.f;
		const float *data_real_iter = m_data_real + m_overlap_len + i;
		for (int j = 0; j < m_N; j++) {
			tmp += *data_real_iter;
			data_real_iter += m_fft_len;
		}
		m_snd_param.power += tmp * tmp;
	}
	m_snd_param.power = 10.f * log10f(m_snd_param.power / (float)m_frame_len);
}


void CLocalizer::findClosestFrequencey(const float input, float *freq, int *idx)
{
	//Find the closest frequency from the discrete frequency vector resulted from FFT
	float fMin = m_sampleRate;	
	float tfreq = 0.f;
	for (int i = 0; i < FFT_LEN; i++)
	{
		tfreq = m_deltFrequency * i;
		float diff = fabs(tfreq - input);
		if (diff < fMin)
		{
			fMin = diff;
			*idx = i; //index is 0 based	
			*freq = tfreq;
		}		
	}
}

//do real fft with window
void CLocalizer::fft(int c)
{
	m_real_fft[m_multithread ? c : 0]->run_fwd_fft(m_data_real_win + m_fft_len * c, m_data_spec + (m_fft_len_div_2 + 1) * c);
}

//Based on the mic1, calculate the cross-correlation matrix with mic234
void CLocalizer::metric_c()
{		
	if (m_profileID == 0) {
		for (int j = 0; j < m_N; j++) {
			const C *data_spec_iter = m_data_spec + (m_fft_len_div_2 + 1) * j + m_low_bnd_idx;
			m_amp_sum[j] = 0;
			for (int b = 0; b < m_bnd_num; b++) {
				m_amp_sum[j] += sqrt(data_spec_iter->re * data_spec_iter->re + data_spec_iter->im * data_spec_iter->im);
				data_spec_iter++;
			}

			if (m_first) {
				m_amp_sm[j] = m_amp_sum[j];
			}
			else{
				m_amp_sm[j] = m_sigma_smooth_factor * m_amp_sm[j] + ( 1.0f - m_sigma_smooth_factor)*m_amp_sum[j];
			}
		}
	}
	else {
		for (int j = 1; j < m_N; j++)
		{
			const C *data_spec_iter_1 = m_data_spec + m_low_bnd_idx; //first mic
			const C *data_spec_iter_2 = m_data_spec + (m_fft_len_div_2 + 1) * j + m_low_bnd_idx; //other mic

		C *X1j_cor = m_sigma_broadside_c + j - 1;
		int i = 0;
		for (int b = 0; b < m_bnd_num; b++) {

			X1j_cor->re = data_spec_iter_1->re * data_spec_iter_2->re + data_spec_iter_1->im * data_spec_iter_2->im;
			X1j_cor->im = -data_spec_iter_1->re * data_spec_iter_2->im + data_spec_iter_1->im * data_spec_iter_2->re;
			if (m_first) {
				m_sigma_broadside_sm_c[i + j - 1].re = X1j_cor->re;
				m_sigma_broadside_sm_c[i + j - 1].im = X1j_cor->im;
			}
			else
			{
				m_sigma_broadside_sm_c[i + j - 1].re = m_sigma_smooth_factor * m_sigma_broadside_sm_c[i + j - 1].re + (1.f - m_sigma_smooth_factor) * X1j_cor->re;
				m_sigma_broadside_sm_c[i + j - 1].im = m_sigma_smooth_factor * m_sigma_broadside_sm_c[i + j - 1].im + (1.f - m_sigma_smooth_factor) * X1j_cor->im;

			}
			float mod2 = m_sigma_broadside_sm_c[i + j - 1].re * m_sigma_broadside_sm_c[i + j - 1].re + m_sigma_broadside_sm_c[i + j - 1].im * m_sigma_broadside_sm_c[i + j - 1].im;
			m_sigma_broadside_sm_norm_c[i + j - 1].re = m_sigma_broadside_sm_c[i + j - 1].re / (sqrtf(mod2) + SMALL_VALUE);  // to avoid divided by 0
			m_sigma_broadside_sm_norm_c[i + j - 1].im = m_sigma_broadside_sm_c[i + j - 1].im / (sqrtf(mod2) + SMALL_VALUE);  // to avoid divided by 0

				data_spec_iter_1++;
				data_spec_iter_2++;
				X1j_cor += (m_N - 1);
				i += (m_N - 1);
			}
		}
	}	
}

//calculate Rmax matrix and find the max index for the angle
//return the target angle for this frame
float CLocalizer::get_target_angle()
{
	if (m_profileID == AMBISONIC) {
		float Rmax[M] = { 0.f };
		float* steerv_iter = m_steerv;
		float* data_iter = m_amp_sm;
		for (int k = 0; k < M; k++) {
			Rmax[k] = 0.f;
			for (int i = 0; i < m_N; i++) {
				Rmax[k] += m_steerv [i * M + k] * m_amp_sm[i];
			}
		}
		
		for (int i = 0; i < M; i++) {
			m_Rmax[i] = Rmax[i];
		}
	}
	else if(m_profileID == BROADSIDE){
		C Rmax_c[M];
		C*  steerv_iter = m_steerv_broadside;
		C*  gcc_phat_iter = m_sigma_broadside_sm_norm_c;
		for (int k = 0; k < M; k++) {
			Rmax_c[k].re = 0.f;
			Rmax_c[k].im = 0.f;
			for (int b = 0; b < m_bnd_num; b++) {
				for (int i = 1; i < m_N; i++)
				{
					float real1 = (steerv_iter + (k + M * i) + b * (M * m_N))->re;
					float imagin1 = (steerv_iter + (k + M * i) + b * (M * m_N))->im;

					float real2 = (gcc_phat_iter + b * (m_N - 1) + (i - 1))->re;
					float imagin2 = (gcc_phat_iter + b * (m_N - 1) + (i - 1))->im;

					float real = real1 * real2 - imagin1 * imagin2;
					float imag = imagin1 * real2 + real1 * imagin2;
					Rmax_c[k].re += real;
					Rmax_c[k].im += imag;
				}
			}
		}
		for (int i = 0; i < M; i++) {
			m_Rmax[i] = (float)sqrt(Rmax_c[i].re * Rmax_c[i].re + Rmax_c[i].im * Rmax_c[i].im);
		}
	}
	else if (m_profileID == CIRCULAR) {
		C Rmax_c[M] = { 0.0f };
		if (m_N == 7) {
			C*  steerv_iter = m_steerv_broadside;
			C*  gcc_phat_iter = m_sigma_broadside_sm_norm_c;
			for (int k = 0; k < M; k++) {
				Rmax_c[k].re = 0.f;
				Rmax_c[k].im = 0.f;
				for (int b = 0; b < m_bnd_num; b++) {
					for (int i = 1; i < m_N; i++)
					{
						float real1 = (steerv_iter + (k + M * i) + b * (M * m_N))->re;
						float imagin1 = (steerv_iter + (k + M * i) + b * (M * m_N))->im;

						float real2 = (gcc_phat_iter + b * (m_N - 1) + (i - 1))->re;
						float imagin2 = (gcc_phat_iter + b * (m_N - 1) + (i - 1))->im;

						float real = real1 * real2 - imagin1 * imagin2;
						float imag = imagin1 * real2 + real1 * imagin2;
						Rmax_c[k].re += real;
						Rmax_c[k].im += imag;
					}
				}
			}
		}
		else {
			C*  steerv_iter = m_steerv_broadside;
			C*  gcc_phat_iter = m_sigma_broadside_sm_norm_c;

			for (int k = 0; k < M; k++) {
				for (int b = 0; b < m_bnd_num; b++) {
					for (int i = 1; i < m_N; i++)
					{
						float real1 = (steerv_iter + (k + M * i) + b * (M * m_N))->re;
						float imagin1 = (steerv_iter + (k + M * i) + b * (M * m_N))->im;

						float real2 = (gcc_phat_iter + b * (m_N - 1) + (i - 1))->re;
						float imagin2 = (gcc_phat_iter + b * (m_N - 1) + (i - 1))->im;

						float real = real1 * real2 - imagin1 * imagin2;
						float imag = real1 * imagin2 + imagin1 * real2;
						Rmax_c[k].re += real;
						Rmax_c[k].im += imag;
					}
				}
			}
		}

		for (int i = 0; i < M; i++) {
			m_Rmax[i] = (float)sqrt(Rmax_c[i].re * Rmax_c[i].re + Rmax_c[i].im * Rmax_c[i].im);
		}
	}

	

	//find the max value within Rmax
	float tmp = m_Rmax[0];
	int idx = 0;
	for (int k = 1; k < M; k++) {
		if (m_Rmax[k] > tmp)
		{
			tmp = m_Rmax[k];
			idx = k;
		}
	}
	//find the angle and return the target
	float alpha;
	if (m_profileID == AMBISONIC || m_profileID == CIRCULAR) {
		alpha = (float)(idx) * 360.f / (float)(M);
	}
	else if (m_profileID == BROADSIDE) {
		alpha = (float)(idx) * 180.f / (float)(M);
		alpha = 90 - alpha;
	}
	return alpha;
}

float CLocalizer::medianValue() {
	float tmp;
	float Rmax_tmp[M] = { 0.0 };
	memcpy(Rmax_tmp, m_Rmax,sizeof(float)*M);
	for(int i = 0; i<M - 1; i++)
		for (int j = i + 1; j<M; j++) {
			if (Rmax_tmp[i]>Rmax_tmp[j]) {
				tmp = Rmax_tmp[i];
				Rmax_tmp[i] = Rmax_tmp[j];
				Rmax_tmp[j] = tmp;
			}
		}
	if (M % 2 == 1) {
		return Rmax_tmp[M/2];
	}
	else {
		return (float)((Rmax_tmp[M / 2] + Rmax_tmp[M / 2 + 1]) / 2.0);
	};
}

float CLocalizer::minValue() {
	float Rmax_tmp[M] = { 0.0 };
	memcpy(Rmax_tmp, m_Rmax, sizeof(float)*M);
	float min_tmp = Rmax_tmp[0];
	for (int i = 1; i < M; i++) {
		if (Rmax_tmp[i] < min_tmp) {
			min_tmp = Rmax_tmp[i];
		}
	}
	return min_tmp;
}