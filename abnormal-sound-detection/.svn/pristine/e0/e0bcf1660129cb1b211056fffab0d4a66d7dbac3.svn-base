#include "beamforming.h"
#include "inverse.h"
#include "beamforming_table.h"
#include "beamforming_table_Respeaker.h"
#include "../utils/cond.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#define EPS_32F (1e-8f)

#define AS (0.5f)
#define ZC (0.5f)
#define VARMIN (1e-7f)
#define SMALLV (1e-9f)
#define SMALLV1 (1e-4f)
#define SMOOTHA (1.0f)
#define SMOOTHB (0.99f)
#define SMOOTHC (0.5f)
#define SNRMAX (10.f)
#define SNRMIN (0.1f)
#define MAXSUP1 (-6.f)
#define MAXSUP2 (-50.f)
#define MAXNLPSUP1 (2.0f)
#define MAXNLPSUP2 (0.0f)
#define FFT_IDX_LOW (10)       // remove DC impact
#define FFT_IDX_HIGH (FFT_LEN >> 2)     // lower half frequency. High frequency is not always stable.
#define DEFAULT_FRAME_COUNT_THRESHOLD (5)


#if 0
#define NUM_CHAN 60
#define LO_CHAN 0
#define HI_CHAN (NUM_CHAN-1)

static int ch_tbl[NUM_CHAN][2] = {
	{ 0, 0 },    //31
	{ 1, 1 },    //35
	{ 2, 2 },    //40
	{ 3, 3 },    //44
	{ 4, 4 },    //50
	{ 5, 5 },            //56
	{ 6, 6 },    //63
	{ 7, 7 },           //70
	{ 8, 8 },    //80
	{ 9, 9 },            //88
	{ 10, 10 },    //100
	{ 11, 11 },            //111
	{ 12, 12 },    //125
	{ 13, 13 },             //140
	{ 14, 14 },    //160
	{ 15, 15 },            //178
	{ 16, 16 },    //200
	{ 17, 17 },            //220
	{ 18, 18 },    //250
	{ 19, 19 },           //280
	{ 20, 20 },  //315
	{ 21, 21 },           //350
	{ 22, 22 },  //400
	{ 23, 23 },            //440
	{ 24, 24 },  //500
	{ 25, 25 },          //560
	{ 26, 26 },  //630
	{ 27, 27 },           //700
	{ 28, 28 },  //800
	{ 29, 29 },             //880
	{ 30, 30 },    //1k
	{ 31, 31 },               //1.1k
	{ 32, 32 },    //1.25k
	{ 33, 33 },              //1.4k
	{ 34, 35 },    //1.6k
	{ 36, 37 },              //1.7k
	{ 38, 39 },              //1.7k
	{ 40, 42 },    //2k
	{ 43, 47 },               //2.2k
	{ 48, 53 },    //2.5k
	{ 54, 60 },              //2.8k
	{ 61, 68 },    //3.2k
	{ 69, 76 },            //3.5k
	{ 77, 85 },    //4k
	{ 86, 94 },           //4.4k
	{ 95, 106 },    //5k
	{ 107, 118 },            //5.7k
	{ 119, 134 },    //6.3k
	{ 135, 150 },             //7k
	{ 151, 170 },    //8k
	{ 171, 190 },               //8.8k
	{ 191, 213 },  //10k
	{ 214, 238 },            //11k
	{ 239, 267 },  //12.5k
	{ 268, 300 },          //14k
	{ 301, 341 },  //16k
	{ 342, 382 },            //17.5k
	{ 383, 427 },  //20k
	{ 428, 468 },            //22k
	{ 469, 512 }  //24k
};
#endif

#define NUM_CHAN (FFT_LEN/2+1)
#define NUM_CHAN_16k (FFT_LEN_16k/2+1)
#define LO_CHAN 0
#define HI_CHAN (NUM_CHAN-1)
int ch_tbl[NUM_CHAN][2];

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

CBeamforming::CBeamforming(int profileID, int frame_length, int channelN, int sampleRate, float micDist, bool multithread)
	: m_in_buf(NULL)
	, m_buf_real(NULL)
	, m_buf_spec(NULL)
	, m_in_buf_ch(NULL)
	, m_buf_real_ch(NULL)
	, m_buf_spec_ch(NULL)
	, m_buf_pow(NULL)
	, m_buf_pow_ch(NULL)
	, m_buf_pow_prev(NULL)
	, m_buf_pow_local(NULL)
	, m_noise_var(NULL)
	, m_speech_var(NULL)
	, m_re_average(NULL)
	, m_speech_absence_possibility(NULL)
	, m_real_fft(NULL)
	, m_real_fft_delay(NULL)
	, m_multithread(multithread)
	, m_sin_win(NULL)
	, m_cos_win(NULL)
	, m_hanning_win(NULL)
	, m_bin_gain(NULL)
	, m_bin_gain_smooth(NULL)
	, m_bin_gain_ch(NULL)
	, m_bin_gain_smooth_ch(NULL)
	, m_ready_conds(NULL)
	, m_done_conds(NULL)
	, m_prep_conds(NULL)
	, m_threads(NULL)
	, m_thread_break(false)
	, m_obj_in(NULL)
	, m_obj_out(NULL)
	, m_targetSig(NULL)
	, m_targetSig_delay(NULL)
	, m_nullSig(NULL)
	, m_detectEng(NULL)
	, m_delayBuf_ch(NULL)
	, m_anaBuf_ch(NULL)
	, m_delayBuf_out_ch(NULL)
	, m_delayBuf_spec_ch(NULL)
	, m_delaySumBuf_spec_ch(NULL)
	, m_postFilter_nom(NULL)
	, m_postFilter_den(NULL)
	, m_postFilter_nom_avg(NULL)
	, m_postFilter_den_avg(NULL)
	, m_postFilter_gain(NULL)
	, m_targetDirection(0)
	, m_targetEstimation(0)
	, m_nullDirection(180)
	, m_in(NULL)
	, m_sigma_c(NULL)
	, m_sigma_sm_c(NULL)
	, m_first(true)
	, m_matric_c(NULL)
	, m_nom_c(NULL)
	, m_steerVectors(NULL)
	, m_superDirective(NULL)
	, m_delayVectors_c(NULL)
	, m_superVectors_c(NULL)
	, m_mvdrVectors_c(NULL)
	, m_StableMatrix(NULL)
	, m_tmpv(NULL)
	, m_mem(NULL)
	, m_out_ch(NULL)
	//, m_matric_real(NULL)
	//, m_matric_image(NULL)
	, m_targetFlag(0)
	, m_targetFlag_prev(0)
#if ANS_DEBUG
	, m_out_buf(NULL)
	, m_bin_gain_bgn(NULL)
#endif /*ANS_DEBUG*/
{
	m_profileID = profileID;
	m_frame_len = frame_length;
	m_sampleRate = sampleRate; // todo
	m_mode = 1;
	if (m_sampleRate == 16000) {
		m_fft_len = FFT_LEN_16k;
		m_num_band = NUM_CHAN_16k;
	}
	else{
		m_fft_len = FFT_LEN;
		m_num_band = NUM_CHAN;
	}
	m_fft_len_div_2 = m_fft_len >> 1;
	m_overlap_len = m_frame_len;
	m_channelN_in = channelN;
	m_channelN = 2;
	m_frame_cnt = 0;
	m_max_sup_db = -10;
	if (m_max_sup_db > MAXSUP1) {
		m_max_sup_db = MAXSUP1;
	}
	if (m_max_sup_db < MAXSUP2) {
		m_max_sup_db = MAXSUP2;
	}
	//m_max_sup_db = powf(10.f, m_max_sup_db / 20.f);
	m_nlp_sup = 1.0;
	m_nlp_factor = 0.5;
	m_nlp_gain = 1;
	m_target_delay_len = 10;

	m_in = new float[m_channelN_in*m_frame_len];
	m_detectEng = new float[m_channelN];
	m_targetSig = new float[m_frame_len];
	m_targetSig_delay = new float[m_frame_len + m_target_delay_len];
	memset(m_targetSig_delay, 0x00, sizeof(float) * (m_frame_len + m_target_delay_len));
	m_nullSig = new float[m_channelN_in*m_frame_len];
	m_in_buf = new float[m_fft_len];
	memset(m_in_buf, 0x00, sizeof(float) * m_fft_len);
	m_buf_real = new R[m_fft_len];
	memset(m_buf_real, 0x00, sizeof(R) * m_fft_len);
	m_buf_spec = new C[m_fft_len_div_2 + 1];
	m_buf_pow = new float[m_num_band];
	m_buf_pow_prev = new float[m_num_band];
	m_buf_pow_local = new float[m_num_band];
	m_noise_var = new float[m_num_band];
	m_speech_var = new float[m_num_band];
	m_re_average = new float[m_num_band];
	m_speech_absence_possibility = new float[m_num_band];
	m_bin_gain = new float[m_num_band];
	m_bin_gain_smooth = new float[m_num_band];

	m_postFilter_nom = new R[m_fft_len_div_2 + 1];
	m_postFilter_den = new R[m_fft_len_div_2 + 1];
	memset(m_postFilter_nom, 0x00, sizeof(R) * (m_fft_len_div_2 + 1));
	memset(m_postFilter_den, 0x00, sizeof(R) * (m_fft_len_div_2 + 1));
	m_postFilter_nom_avg = new R[m_fft_len_div_2 + 1];
	m_postFilter_den_avg = new R[m_fft_len_div_2 + 1];
	memset(m_postFilter_nom_avg, 0x00, sizeof(R) * (m_fft_len_div_2 + 1));
	memset(m_postFilter_den_avg, 0x00, sizeof(R) * (m_fft_len_div_2 + 1));
	m_postFilter_gain = new R[m_fft_len_div_2 + 1];

	for (int i = 0; i < m_num_band; i++) {
		m_buf_pow_local[i] = 1.f;  /* why? */
		m_noise_var[i] = 1.f;
		m_speech_var[i] = 1.f;
		m_re_average[i] = 1.f;
		m_bin_gain[i] = 0.f;
		m_bin_gain_smooth[i] = 1.f;
	}
	m_in_buf_ch = new float*[m_channelN];
	m_buf_real_ch = new R*[m_channelN];
	m_buf_spec_ch = new C*[m_channelN];
	m_buf_pow_ch = new float*[m_channelN];
	m_bin_gain_ch = new float*[m_channelN];
	m_bin_gain_smooth_ch = new float*[m_channelN];
	m_delayBuf_ch = new R*[m_channelN_in];
	m_anaBuf_ch = new R*[m_channelN_in];
	m_delayBuf_out_ch = new R*[m_channelN_in];
	m_delayBuf_spec_ch = new C*[m_channelN_in];
	m_delaySumBuf_spec_ch = new C*[m_channelN_in];
	for (int i = 0; i < m_channelN; i++) {
		m_in_buf_ch[i] = new float[m_fft_len];
		memset(m_in_buf_ch[i], 0x00, sizeof(float) * m_fft_len);
		m_buf_real_ch[i] = new R[m_fft_len];
		memset(m_buf_real_ch[i], 0x00, sizeof(R) * m_fft_len);
		m_buf_spec_ch[i] = new C[m_fft_len_div_2 + 1];
		m_buf_pow_ch[i] = new float[m_num_band];
		m_bin_gain_ch[i] = new float[m_num_band];
		m_bin_gain_smooth_ch[i] = new float[m_num_band];
		for (int j = 0; j < m_num_band; j++) {
			m_bin_gain_ch[i][j] = 0.f;
			m_bin_gain_smooth_ch[i][j] = 1.f;
		}
	}
	for (int i = 0; i < m_channelN_in; i++) {
		m_delayBuf_ch[i] = new R[m_fft_len];
		m_anaBuf_ch[i] = new R[m_fft_len];
		m_delayBuf_out_ch[i] = new R[m_fft_len];
		m_delayBuf_spec_ch[i] = new C[m_fft_len_div_2 + 1];
		m_delaySumBuf_spec_ch[i] = new C[m_fft_len_div_2 + 1];
		for (int j = 0; j < m_fft_len; j++) {
			m_delayBuf_ch[i][j] = 0.f;
			m_anaBuf_ch[i][j] = 0.f;
			m_delayBuf_out_ch[i][j] = 0.f;
		}
	}

	uint logN = (uint)(log((float)m_fft_len) / log(2.0f) + 0.5);
	int fft_num = m_multithread ? m_channelN_in : 1;
	m_real_fft = new CRealFFT*[fft_num];
	for (int i = 0; i < fft_num; i++) {
		m_real_fft[i] = new CRealFFT(logN);  //andy: real fft
	}
	m_fft_scale = 1.f / (float)m_fft_len;
	m_real_fft_delay = new CRealFFT(logN);

	m_sin_win = new float[m_frame_len * 2];
	m_cos_win = new float[m_frame_len * 2];
	m_hanning_win = new float[m_frame_len * 2];
	for (int i = 0; i < m_frame_len * 2; i++) {
		float ang = M_PI * (float)i / (float)(m_frame_len * 2);
		m_sin_win[i] = sin(ang);
		m_cos_win[i] = cos(ang);
		m_hanning_win[i] = m_sin_win[i];// (float)sqrt(0.5 - 0.5*cos(2 * M_PI * i / (float)m_fft_len));
	}

	if (m_multithread) {
		m_ready_conds = new CCond[m_channelN_in];
		m_done_conds = new CCond[m_channelN_in];
		m_prep_conds = new CCond[m_channelN_in];
		m_threads = new CThread[m_channelN_in];
		m_thread_break = false;
		for (int i = 0; i < m_channelN_in; i++) {
			m_prep_conds[i].lock();
			ThreadArgs args;
			args.index = i;
			args.user = this;
			m_threads[i].create(CBeamforming::thread_process, &args);
			m_prep_conds[i].wait();
			m_prep_conds[i].unlock();
		}
	}

	m_ans_param.avgsnrglobal = 0.f;

	//beamforming
	m_micDist = micDist;
	m_steerVectors = new float*[M];
	for (int i = 0; i < M; i++) {
		m_steerVectors[i] = new float[m_channelN_in];
	}

	m_superDirective = new float*[m_num_band];
	for (int i = 0; i < m_num_band; i++) {
		m_superDirective[i] = new float[m_channelN_in*m_channelN_in];
	}
	
	m_delayVectors_c = new C*[m_channelN_in];
	m_superVectors_c = new C*[m_channelN_in];
	m_mvdrVectors_c = new C*[m_channelN_in];
	for (int i = 0; i < m_channelN_in; i++) {
		m_delayVectors_c[i] = new C[m_fft_len_div_2 + 1];
		m_superVectors_c[i] = new C[m_fft_len_div_2 + 1];
		m_mvdrVectors_c[i] = new C[m_fft_len_div_2 + 1];
	}

	m_sigma_c = new C[m_channelN_in*m_channelN_in];
	m_sigma_sm_c = new C*[m_num_band];
	for (int i = 0; i < m_num_band; i++) {
		m_sigma_sm_c[i] = new C[m_channelN_in*m_channelN_in];
	}

	m_StableMatrix = new float[m_channelN_in*m_channelN_in];
	for (int i = 0; i < m_channelN_in; i++) {
		for (int j = 0; j < m_channelN_in; j++) {
			if (i == j) {
				m_StableMatrix[i*m_channelN_in + j] = 0.0001f;
			}
			else {
				m_StableMatrix[i*m_channelN_in + j] = 0.00f;
			}		
		}
	}

	m_matric_c = new C[m_channelN_in*m_channelN_in];
	m_nom_c = new C[m_channelN_in];

	m_tmpv = new C[m_channelN_in];

	m_mem = new float*[m_channelN_in];
	m_out_ch = new float*[m_channelN_in];
	for (int i = 0; i < m_channelN_in; i++) {
		m_mem[i] = new float[m_overlap_len];
		memset(m_mem[i], 0x00, sizeof(float)*m_overlap_len);

		m_out_ch[i] = new float[m_frame_len];
		memset(m_out_ch[i], 0x00, sizeof(float)*m_frame_len);
	}
	for (int i = 0; i < NUM_CHAN; i++)
	{
		ch_tbl[i][0] = i;
		ch_tbl[i][1] = i;
	}
	//m_matric_real = new float[m_channelN_in * m_channelN_in];
	//m_matric_image = new float[m_channelN_in * m_channelN_in];

	prebakeSteerVector();

#if ANS_DEBUG
	m_out_buf = new float[m_frame_len];
	m_bin_gain_bgn = new float[m_fft_len_div_2 + 1];
	for (int i = 0; i < m_fft_len_div_2 + 1; i++) {
		m_bin_gain_bgn[i] = 0.f;
	}
#endif /*ANS_DEBUG*/
}

CBeamforming::~CBeamforming()
{
	if (m_multithread) {
		m_thread_break = true;
		for (int i = 0; i < m_channelN_in; i++) {
			m_ready_conds[i].signal_s();
			m_threads[i].join();
		}
	}
	RELEASE_ARRAY(m_threads);
	RELEASE_ARRAY(m_ready_conds);
	RELEASE_ARRAY(m_done_conds);
	RELEASE_ARRAY(m_prep_conds);
	int fft_num = m_multithread ? m_channelN_in : 1;
	for (int i = 0; i < fft_num; i++) {
		RELEASE_POINTER(m_real_fft[i]);
	}
	RELEASE_ARRAY(m_real_fft);
	RELEASE_POINTER(m_real_fft_delay);

	RELEASE_ARRAY(m_buf_real);
	RELEASE_ARRAY(m_buf_spec);
	RELEASE_ARRAY(m_buf_pow);
	RELEASE_ARRAY(m_buf_pow_prev);
	RELEASE_ARRAY(m_buf_pow_local);
	RELEASE_ARRAY(m_noise_var);
	RELEASE_ARRAY(m_speech_var);
	RELEASE_ARRAY(m_re_average);
	RELEASE_ARRAY(m_sin_win);
	RELEASE_ARRAY(m_cos_win);
	RELEASE_ARRAY(m_hanning_win);
	RELEASE_ARRAY(m_bin_gain);
	RELEASE_ARRAY(m_bin_gain_smooth);
	RELEASE_ARRAY(m_speech_absence_possibility);
	RELEASE_ARRAY(m_in_buf);
	for (int i = 0; i < m_channelN; i++) {
		RELEASE_ARRAY(m_in_buf_ch[i]);
		RELEASE_ARRAY(m_buf_real_ch[i]);
		RELEASE_ARRAY(m_buf_spec_ch[i]);
		RELEASE_ARRAY(m_buf_pow_ch[i]);
		RELEASE_ARRAY(m_bin_gain_ch[i]);
		RELEASE_ARRAY(m_bin_gain_smooth_ch[i]);
	}
	RELEASE_ARRAY(m_in_buf_ch);
	RELEASE_ARRAY(m_buf_real_ch);
	RELEASE_ARRAY(m_buf_spec_ch);
	RELEASE_ARRAY(m_buf_pow_ch);
	RELEASE_ARRAY(m_bin_gain_ch);
	RELEASE_ARRAY(m_bin_gain_smooth_ch);
#if ANS_DEBUG
	RELEASE_ARRAY(m_out_buf);
	RELEASE_ARRAY(m_bin_gain_bgn);
#endif /*ANS_DEBUG*/
	for (int i = 0; i < m_channelN_in; i++) {
		RELEASE_ARRAY(m_delayBuf_ch[i]);
		RELEASE_ARRAY(m_anaBuf_ch[i]);
		RELEASE_ARRAY(m_delayBuf_out_ch[i]);
		RELEASE_ARRAY(m_delayBuf_spec_ch[i]);
		RELEASE_ARRAY(m_delaySumBuf_spec_ch[i]);
		RELEASE_ARRAY(m_delayVectors_c[i]);
		RELEASE_ARRAY(m_superVectors_c[i]);
		RELEASE_ARRAY(m_mvdrVectors_c[i]);
		RELEASE_ARRAY(m_mem[i]);
		RELEASE_ARRAY(m_out_ch[i]);
	}
	RELEASE_ARRAY(m_delayBuf_ch);
	RELEASE_ARRAY(m_anaBuf_ch);
	RELEASE_ARRAY(m_delayBuf_out_ch);
	RELEASE_ARRAY(m_delayBuf_spec_ch);
	RELEASE_ARRAY(m_delaySumBuf_spec_ch);
	RELEASE_ARRAY(m_delayVectors_c);
	RELEASE_ARRAY(m_superVectors_c);
	RELEASE_ARRAY(m_mvdrVectors_c);
	RELEASE_ARRAY(m_mem);
	RELEASE_ARRAY(m_out_ch);
	RELEASE_ARRAY(m_postFilter_nom);
	RELEASE_ARRAY(m_postFilter_den);
	RELEASE_ARRAY(m_postFilter_nom_avg);
	RELEASE_ARRAY(m_postFilter_den_avg);
	RELEASE_ARRAY(m_postFilter_gain);

	for (int i = 0; i < M; i++) {
		RELEASE_ARRAY(m_steerVectors[i]);
	}
	RELEASE_ARRAY(m_steerVectors);

	for (int i = 0; i < m_num_band; i++) {
		RELEASE_ARRAY(m_superDirective[i]);
		RELEASE_ARRAY(m_sigma_sm_c[i]);
	}
	RELEASE_ARRAY(m_superDirective);
	RELEASE_ARRAY(m_sigma_sm_c);

	RELEASE_ARRAY(m_sigma_c);
	RELEASE_ARRAY(m_nom_c);

	RELEASE_ARRAY(m_matric_c);
	RELEASE_ARRAY(m_StableMatrix);

	RELEASE_ARRAY(m_targetSig);
	RELEASE_ARRAY(m_targetSig_delay);
	RELEASE_ARRAY(m_nullSig);
	RELEASE_ARRAY(m_detectEng);
	RELEASE_ARRAY(m_in);
	RELEASE_ARRAY(m_tmpv);
	//RELEASE_ARRAY(m_matric_real);
	//RELEASE_ARRAY(m_matric_image);
}

void CBeamforming::noise_spectrum_estimator()
{
	const float *buf_pow_iter = m_buf_pow;
	float *buf_pow_local_iter = m_buf_pow_local;
	*buf_pow_local_iter = AS * *buf_pow_local_iter + (1.f - AS) * *buf_pow_iter;
	buf_pow_local_iter++; buf_pow_iter++;

	for (int i = 1; i < m_num_band - 1; i++) {
		*buf_pow_local_iter = AS * *buf_pow_local_iter + (1.f - AS) * (*(buf_pow_iter - 1) + *(buf_pow_iter + 1) + 2.f * *buf_pow_iter) * 0.25f;
		buf_pow_local_iter++; buf_pow_iter++;
	}
	*buf_pow_local_iter = AS * *buf_pow_local_iter + (1.f - AS) * *buf_pow_iter;

	if (m_frame_cnt < DEFAULT_FRAME_COUNT_THRESHOLD) {
		memcpy(m_noise_var, m_buf_pow_local, sizeof(float) * (m_num_band));
		float *noise_var_iter = m_noise_var;
		for (int i = 0; i < m_num_band; i++) {
			if (*noise_var_iter < VARMIN) {
				*noise_var_iter = VARMIN;
			}
			noise_var_iter++;
		}
	}
	else {
		const float *buf_pow_iter = m_buf_pow;
		const float *buft_pow_local_iter = m_buf_pow_local;
		const float *speech_absence_possibility_iter = m_speech_absence_possibility;
		float *noise_var_iter = m_noise_var;
		for (int i = 0; i < m_num_band; i++) {
			float w;
			if (*buft_pow_local_iter > *noise_var_iter) {
				if (m_ans_param.speechAbsencePossibilityAvg < 0.1f)	{
					w = *noise_var_iter / *buft_pow_local_iter * 0.001f;
				}
				else {
					float tmpf = *speech_absence_possibility_iter;
					if (tmpf < 0.1) tmpf = 0.1;
					w = *noise_var_iter / *buft_pow_local_iter * 0.01f * tmpf;
				}
			}
			else {
				w = 0.1f;
			}

			*noise_var_iter = (1.f - w) * *noise_var_iter + w * *buft_pow_local_iter;
			if (*noise_var_iter < VARMIN) {
				*noise_var_iter = VARMIN;
			}
			buft_pow_local_iter++; buf_pow_iter++; noise_var_iter++; speech_absence_possibility_iter++;
		}
	}
}

void CBeamforming::vocal_spectrum_estimator()
{
	const float *buf_pow_iter = m_buf_pow;
	const float *buf_pow_prev_iter = m_buf_pow_prev;
	const float *noise_var_iter = m_noise_var;
	float *speech_var_iter = m_speech_var;
	for (int i = 0; i < m_num_band; i++) {
		*speech_var_iter = *buf_pow_prev_iter * *speech_var_iter / (*speech_var_iter + *noise_var_iter) * SMOOTHA;
		if (*buf_pow_iter > *noise_var_iter) {
			*speech_var_iter += (1.f - SMOOTHA) * (*buf_pow_iter - *noise_var_iter);
		}
		buf_pow_iter++; buf_pow_prev_iter++; noise_var_iter++; speech_var_iter++;
	}
}

void CBeamforming::snr_estimator()
{
	float *re_average_iter = m_re_average;
	const float *speech_var_iter = m_speech_var;
	const float *noise_var_iter = m_noise_var;
	for (int i = 0; i < m_num_band; i++) {
		*re_average_iter = *re_average_iter * ZC + *speech_var_iter / *noise_var_iter * (1.f - ZC);
		if (*re_average_iter > SNRMAX) {
			*re_average_iter = SNRMAX;
		}
		else if (*re_average_iter < SNRMIN) {
			*re_average_iter = SNRMIN;
		}
		re_average_iter++; speech_var_iter++; noise_var_iter++;
	}

	//global snr 
	m_ans_param.avgsnrglobal = 0.f;
	re_average_iter = m_re_average + FFT_IDX_LOW;
	for (int i = FFT_IDX_LOW; i < FFT_IDX_HIGH; i++) {
		m_ans_param.avgsnrglobal += *re_average_iter;
		re_average_iter++;
	}
	m_ans_param.avgsnrglobal /= (float)(FFT_IDX_HIGH - FFT_IDX_LOW);

	if (m_ans_param.avgsnrglobal > SNRMAX) {
		m_ans_param.avgsnrglobal = SNRMAX;
	}
	else if (m_ans_param.avgsnrglobal < SNRMIN) {
		m_ans_param.avgsnrglobal = SNRMIN;
	}  


	m_ans_param.speechAbsencePossibilityAvg = 1.f - (m_ans_param.avgsnrglobal / SNRMAX);
	float *speech_absence_possibility_iter = m_speech_absence_possibility;
	re_average_iter = m_re_average;

	for (int i = 0; i < m_num_band; i++) {
		*speech_absence_possibility_iter = 1.f - (*re_average_iter / SNRMAX);
		re_average_iter++; speech_absence_possibility_iter++;
	}

	//printf("snr = %.3f", pResGlobal->avgsnrglobal);
}

void CBeamforming::gain_estimator()
{
	memcpy(m_buf_pow_prev, m_buf_pow, sizeof(float) * (m_num_band));

	noise_spectrum_estimator();
	vocal_spectrum_estimator();
	snr_estimator();

	const float *noise_var_iter = m_noise_var;
	const float *buf_pow_iter = m_buf_pow;
	const float *speech_absence_possibility_iter = m_speech_absence_possibility;
	float *bin_gain_iter = m_bin_gain;
	float *bin_gain_smooth_iter = m_bin_gain_smooth;
	float max_sup = powf(10.f, m_max_sup_db / 20.f);
#if ANS_DEBUG
	float *bin_gain_bgn_iter = m_bin_gain_bgn;
#endif /*ANS_DEBUG*/
	for (int i = 0; i < m_num_band; i++) {
		float tmp = *noise_var_iter / *buf_pow_iter;
		if (tmp > 1.f) {
			tmp = 1.f;
		}
#if ANS_DEBUG
		if (tmp > *bin_gain_bgn_iter) {
			*bin_gain_bgn_iter = *bin_gain_bgn_iter * 0.5f + tmp * 0.5f;
		}
		else {
			*bin_gain_bgn_iter = tmp;
		}
		bin_gain_bgn_iter++;
#endif /*ANS_DEBUG*/
#if (1)
		float tmp_gain = 1.f - tmp;
		// tmp_gain = sqrtf(tmp_gain);   //wiena filter or spectrum substraction (sqrt).

		tmp_gain = tmp_gain * (1.f - *speech_absence_possibility_iter) + max_sup * *speech_absence_possibility_iter;

		if (tmp_gain < max_sup) {
			tmp_gain = max_sup;
		}
		tmp_gain = *bin_gain_smooth_iter * 0.5f + tmp_gain * 0.5f;
		*bin_gain_smooth_iter = tmp_gain;
		*bin_gain_iter = tmp_gain;
#else
		*bin_gain_iter = sqrt(pResGlobal->binBgNoiseGain[i]);   //test for bg noise estimation.
#endif
		noise_var_iter++; buf_pow_iter++; speech_absence_possibility_iter++; bin_gain_iter++; bin_gain_smooth_iter++; 
	}
}

void CBeamforming::gain_estimator_ch(int c)
{
	const float *noise_var_iter = m_noise_var;
	const float *speech_absence_possibility_iter = m_speech_absence_possibility;
	const float *buf_pow_ch_iter_s = m_buf_pow_ch[0];
	const float *buf_pow_ch_iter_n = m_buf_pow_ch[1];

	float *bin_gain_ch_iter = m_bin_gain_ch[c];
	float *bin_gain_smooth_ch_iter = m_bin_gain_smooth_ch[c];
	float sup = m_nlp_sup;
	float max_sup = powf(10.f, m_max_sup_db / 20.f);

	m_detectEng[0] = SMALLV;
	m_detectEng[1] = SMALLV;
	int lowIndex = m_num_band * 800 / (m_sampleRate/2); // 800hz  for fs 48000
	int highIndex = m_num_band * 4800 / (m_sampleRate/2); //4.8k
	for (int i = 0; i < m_num_band; i++) {
		float tmp = *noise_var_iter / *buf_pow_ch_iter_s;
		if (tmp > 1.f) {
			tmp = 1.f;
		}
		float tmp_gain_mode2 = 1.f - tmp;
		
		float tmp2 = *buf_pow_ch_iter_n * sup / *buf_pow_ch_iter_s;
		if (tmp2 > 1.f) {
			tmp2 = 1.f;
		}

		tmp2 = 1.f - tmp2;
		if (tmp2 < tmp)
			tmp2 = tmp;

		tmp = *noise_var_iter / (*buf_pow_ch_iter_s*tmp2);
		if (tmp > 1.f) {
			tmp = 1.f;
		}		
		float tmp_gain_mode3 = 1.f - tmp;

		float tmp_gain = 1.0;
		if (m_mode == 5 || m_mode == 3)
			tmp_gain = tmp_gain_mode2;
		else if (m_mode == 7)
			tmp_gain = tmp_gain_mode3;
		// tmp_gain = sqrtf(tmp_gain);   //wiena filter or spectrum substraction (sqrt).
		tmp_gain = tmp_gain * (1.f - *speech_absence_possibility_iter*m_ans_param.speechAbsencePossibilityAvg) + max_sup * *speech_absence_possibility_iter*m_ans_param.speechAbsencePossibilityAvg;

		if (tmp_gain < max_sup) {
			tmp_gain = max_sup;
		}

		if(m_mode == 1 )
		    tmp_gain = 1.0; //beamforming;
		else if(m_mode == 5 || m_mode == 3)
		    tmp_gain = tmp_gain; // beamforming + stationary noise suppress;
		else if(m_mode == 7)
		    tmp_gain = tmp_gain*tmp2;  //beamforming + stationary noise suppress + nlp.
		else
			tmp_gain = 1.0;

		tmp_gain = *bin_gain_smooth_ch_iter * 0.5f + tmp_gain * 0.5f;
		*bin_gain_smooth_ch_iter = tmp_gain;
		*bin_gain_ch_iter = tmp_gain;

		float eng;
		if (i >= lowIndex && i <= highIndex)
		{
			eng = (*buf_pow_ch_iter_s) - (*noise_var_iter);
			if (eng < 0) eng = 0;
			m_detectEng[0] += eng;
			eng = (*buf_pow_ch_iter_n) - (*noise_var_iter);
			if (eng < 0) eng = 0;
			m_detectEng[1] += eng;
		}
		noise_var_iter++; speech_absence_possibility_iter++; buf_pow_ch_iter_s++; buf_pow_ch_iter_n++; bin_gain_ch_iter++; bin_gain_smooth_ch_iter++;
	}
}



void CBeamforming::gain_estimator_ch_opt(int c)
{
	const float *noise_var_iter = m_noise_var;
	const float *speech_absence_possibility_iter = m_speech_absence_possibility;
	const float *buf_pow_ch_iter_s = m_buf_pow_ch[0];
	const float *buf_pow_ch_iter_n = m_buf_pow_ch[1];

	float *bin_gain_ch_iter = m_bin_gain_ch[c];
	float *bin_gain_smooth_ch_iter = m_bin_gain_smooth_ch[c];
	float sup = m_nlp_sup;
	float max_sup = powf(10.f, m_max_sup_db / 20.f);

	m_detectEng[0] = SMALLV;
	m_detectEng[1] = SMALLV;

	for (int i = 0; i < m_num_band; i++) {
		float tmp = *noise_var_iter / *buf_pow_ch_iter_s;
		if (tmp > 1.f) {
			tmp = 1.f;
		}
		float tmp_gain_mode2 = 1.f - tmp;

		/*
		float tmp2 = *buf_pow_ch_iter_n * sup / *buf_pow_ch_iter_s;
		if (tmp2 > 1.f) {
			tmp2 = 1.f;
		}

		tmp2 = 1.f - tmp2;
		if (tmp2 < tmp)
			tmp2 = tmp;
			*/
		//float tmp2 = m_nlp_sup;
		//float postGain = m_postFilter_gain[i];

		if (m_postFilter_gain[i] > 1.0f) {
			m_postFilter_gain[i] = 1.0f;
		}
		if (m_postFilter_gain[i] < 0.0f) {
			m_postFilter_gain[i] = 0.0f;
		}
		
		float postGain = (1.0f - m_nlp_factor)*m_postFilter_gain[i] + m_nlp_factor*m_nlp_gain;

		tmp = *noise_var_iter / (*buf_pow_ch_iter_s*postGain);
		if (tmp > 1.f) {
			tmp = 1.f;
			postGain = *noise_var_iter / (*buf_pow_ch_iter_s + SMALLV);
			if (postGain > 1) postGain = 1;
		}
		float tmp_gain_mode3 = 1.f - tmp;

		float tmp_gain = 1.0;
		if (m_mode == 5 || m_mode == 3)
			tmp_gain = tmp_gain_mode2;
		else if (m_mode == 7)
			tmp_gain = tmp_gain_mode3;
		// tmp_gain = sqrtf(tmp_gain);   //wiena filter or spectrum substraction (sqrt).
		tmp_gain = tmp_gain * (1.f - *speech_absence_possibility_iter*m_ans_param.speechAbsencePossibilityAvg) + max_sup * *speech_absence_possibility_iter*m_ans_param.speechAbsencePossibilityAvg;

		if (tmp_gain < max_sup) {
			tmp_gain = max_sup;
		}

		if (m_mode == 1 )
			tmp_gain = 1.0; //beamforming;
		else if (m_mode == 5 || m_mode == 3)
			tmp_gain = tmp_gain; // beamforming + stationary noise suppress;
		else if (m_mode == 7 )
			tmp_gain = tmp_gain*postGain;  //beamforming + stationary noise suppress + nlp.
		else
			tmp_gain = 1.0;

		tmp_gain = *bin_gain_smooth_ch_iter * 0.5f + tmp_gain * 0.5f;
		*bin_gain_smooth_ch_iter = tmp_gain;
		*bin_gain_ch_iter = tmp_gain;

		noise_var_iter++; speech_absence_possibility_iter++; buf_pow_ch_iter_s++; buf_pow_ch_iter_n++; bin_gain_ch_iter++; bin_gain_smooth_ch_iter++;
	}
}



void CBeamforming::overlap_plus_pre(int c)
{
	C *buf_spec;
	R *buf_real;
	const float *in_buf_iter;
	float *buf_pow_iter;
	CRealFFT *fft;
	if (c < 0) {
		buf_spec = m_buf_spec;
		buf_real = m_buf_real;
		in_buf_iter = m_in_buf;
		buf_pow_iter = m_buf_pow;
		fft = m_real_fft[0];
	}
	else {
		buf_spec = m_buf_spec_ch[c];
		buf_real = m_buf_real_ch[c];
		in_buf_iter = m_in_buf_ch[c];
		buf_pow_iter = m_buf_pow_ch[c];
		fft = m_real_fft[m_multithread ? c : 0];
	}
	const float *sin_win_iter = m_sin_win;
	R *buf_real_iter = buf_real;
	for (int i = 0; i < m_frame_len * 2; i++) {
		*buf_real_iter = *sin_win_iter * *in_buf_iter;
		buf_real_iter++; sin_win_iter++; in_buf_iter++;
	}
	for (int i = m_frame_len * 2; i < m_fft_len; i++)
	{
		*buf_real_iter = 0;
		buf_real_iter++;
	}
	fft->run_fwd_fft(buf_real, buf_spec);
	const C *buf_spec_iter = buf_spec;
	for (int i = 0; i < m_num_band; i++) {
		*buf_pow_iter = 0;
		int j1 = ch_tbl[i][0];
		int j2 = ch_tbl[i][1];
		for (int j = j1; j <= j2; j++)		{
			*buf_pow_iter += buf_spec_iter->re * buf_spec_iter->re + buf_spec_iter->im * buf_spec_iter->im;
			buf_spec_iter++;
		}
		*buf_pow_iter /= (j2 - j1 + 1);
		if (*buf_pow_iter < SMALLV) {
			*buf_pow_iter = SMALLV;
		}
		buf_pow_iter++;
	}
}

void CBeamforming::overlap_plus_post(float *out, int c)
{
	C *buf_spec;
	R *buf_real;
	float *out_iter = NULL;
	int out_step;
	const float *bin_gain_iter;
	CRealFFT *fft;
	if (c < 0) {
		buf_spec = m_buf_spec;
		buf_real = m_buf_real;
#if ANS_DEBUG
		out_iter = m_out_buf;
		out_step = 1;
#endif /*ANS_DEBUG*/
		bin_gain_iter = m_bin_gain;
		fft = m_real_fft[0];
	}
	else {
		buf_spec = m_buf_spec_ch[c];
		buf_real = m_buf_real_ch[c];
		out_iter = out + c;
		out_step = m_channelN;
		bin_gain_iter = m_bin_gain_ch[c];
		fft = m_real_fft[m_multithread ? c : 0];
	}	
	
	C *buf_spec_iter = buf_spec;
	for (int i = 0; i < m_num_band; i++) {
		int j1 = ch_tbl[i][0];
		int j2 = ch_tbl[i][1];
		for (int j = j1; j <= j2; j++) {
			buf_spec_iter->re *= *bin_gain_iter;
			buf_spec_iter->im *= *bin_gain_iter;
			buf_spec_iter++;
		}
		bin_gain_iter++;
	}
	fft->run_inv_fft(buf_spec, buf_real);

	const float *sin_win_iter = m_sin_win;
	R *buf_real_iter = buf_real;
	for (int i = 0; i < m_frame_len * 2; i++) {
		*buf_real_iter *= (*sin_win_iter * m_fft_scale);
		buf_real_iter++; sin_win_iter++;
	}

	if (NULL != out_iter) {
		for (int i = 0; i < m_frame_len; i++) {
			//duplicate mono to stereo;
			*out_iter += *buf_real;
			out_iter += 1; 
			*out_iter += *buf_real;
			out_iter += 1;
			//*out_iter += *buf_real;
			//out_iter += m_channelN;
			buf_real++;
		}
	}
}

void CBeamforming::prepare(const float *in, float *out, int c)
{
	if (c < 0) {
		/* Input. */
		memmove(m_in_buf, m_in_buf + m_frame_len, sizeof(float) * m_overlap_len);
		const float *in_iter = in;
		float *in_buf_iter = m_in_buf + m_overlap_len;
		static float scale = 1.f / sqrtf((float)m_channelN_in);
		for (int i = 0; i < m_frame_len; i++) {
			*in_buf_iter = 0.f;
			for (int k = 0; k < m_channelN_in; k++) {
				*in_buf_iter += *in_iter;
				in_iter++;
			}
			*in_buf_iter *= scale;
			in_buf_iter++;
		}
		/* Output. */
#if ANS_DEBUG
		const float *buf_real_iter = m_buf_real + m_overlap_len;
		float *out_buf_iter = m_out_buf;
		for (int i = 0; i < m_frame_len; i++) {
			*out_buf_iter = *buf_real_iter;
			out_buf_iter++; buf_real_iter++;
		}
#else
		return;
#endif /*ANS_DEBUG*/
	}
	else if (c == 0) {
		/* Input. */
		memmove(m_in_buf_ch[c], m_in_buf_ch[c] + m_frame_len, sizeof(float) * m_overlap_len);
		const float *in_iter = in;
		float *in_buf_ch_inter = m_in_buf_ch[c] + m_overlap_len;
		for (int i = 0; i < m_frame_len; i++) {
			*in_buf_ch_inter = 0.f;

			if (m_profileID == 0)
			{
				if (m_channelN_in == 4)
				{
					*in_buf_ch_inter += *in_iter*m_steerVectors[m_targetDirectionIndex][1];
					in_iter++;
					*in_buf_ch_inter += *in_iter*m_steerVectors[m_targetDirectionIndex][2];
					in_iter++;
					*in_buf_ch_inter += *in_iter*m_steerVectors[m_targetDirectionIndex][0];
					in_iter++;
					*in_buf_ch_inter += *in_iter*m_steerVectors[m_targetDirectionIndex][3];
					in_iter++;
				}
				else if (m_channelN_in == 3)
				{
					*in_buf_ch_inter += *in_iter*m_steerVectors[m_targetDirectionIndex][0];
					in_iter++;
					*in_buf_ch_inter += *in_iter*m_steerVectors[m_targetDirectionIndex][1];
					in_iter++;
					*in_buf_ch_inter += *in_iter*m_steerVectors[m_targetDirectionIndex][2];
					in_iter++;
				}
				else
				{
					throw;
				}
			}
			else if (m_profileID == 1)
			{
				if (m_channelN_in == 2)
				{
					*in_buf_ch_inter += *in_iter * 1;
					in_iter++;
					*in_buf_ch_inter += *in_iter * 0;
					in_iter++;
				}
				else if (m_channelN_in == 3) {
					*in_buf_ch_inter += *in_iter * 1;
					in_iter++;
					*in_buf_ch_inter += *in_iter * 0;
					in_iter++;
				}
				else if(m_channelN_in == 4)
				{
					*in_buf_ch_inter += *in_iter * 1;
					in_iter++;
					*in_buf_ch_inter += *in_iter * 0;
					in_iter++;
				}
				else if (m_channelN_in == 6)
				{
					*in_buf_ch_inter += *in_iter * 1;
					in_iter++;
					*in_buf_ch_inter += *in_iter * 0;
					in_iter++;
				}
				else
				{
					throw;
				}
			}
			else if (m_profileID == 2) {
				if (m_channelN_in == 7) {
					*in_buf_ch_inter += *in_iter * 1;
					in_iter++;
					*in_buf_ch_inter += *in_iter * 0;
					in_iter++;
				}
				else if (m_channelN_in == 4) {
					*in_buf_ch_inter += *in_iter * 1;
					in_iter++;
					*in_buf_ch_inter += *in_iter * 0;
					in_iter++;
				}
				else if (m_channelN_in == 6) {
					*in_buf_ch_inter += *in_iter * 1;
					in_iter++;
					*in_buf_ch_inter += *in_iter * 0;
					in_iter++;
				}
				else {
					throw;
				}
			}
			else
			{
				throw;
			}
			in_buf_ch_inter++;
		}
		/* Output. */
		const float *buf_real_ch_iter = m_buf_real_ch[0] + m_overlap_len;
		float *out_iter = out + c;
		for (int i = 0; i < m_frame_len; i++) {
			*out_iter = *buf_real_ch_iter;
			out_iter += m_channelN; buf_real_ch_iter++;
		}
	}
	else if (c == 1)
	{
		/* Input. */
		memmove(m_in_buf_ch[c], m_in_buf_ch[c] + m_frame_len, sizeof(float) * m_overlap_len);
		const float *in_iter = in;
		float *in_buf_ch_inter = m_in_buf_ch[c] + m_overlap_len;
		for (int i = 0; i < m_frame_len; i++) {
			*in_buf_ch_inter = 0.f;
			if (m_profileID == 0)
			{
				if (m_channelN_in == 4)
				{
					*in_buf_ch_inter += *in_iter*m_steerVectors[m_nullDirectionIndex][1];
					in_iter++;
					*in_buf_ch_inter += *in_iter*m_steerVectors[m_nullDirectionIndex][2];
					in_iter++;
					*in_buf_ch_inter += *in_iter*m_steerVectors[m_nullDirectionIndex][0];
					in_iter++;
					*in_buf_ch_inter += *in_iter*m_steerVectors[m_nullDirectionIndex][3];
					in_iter++;
				}
				else if (m_channelN_in == 3)
				{
					*in_buf_ch_inter += *in_iter*m_steerVectors[m_nullDirectionIndex][0];
					in_iter++;
					*in_buf_ch_inter += *in_iter*m_steerVectors[m_nullDirectionIndex][1];
					in_iter++;
					*in_buf_ch_inter += *in_iter*m_steerVectors[m_nullDirectionIndex][2];
					in_iter++;
				}
				else
				{
					throw;
				}
			}
			else if (m_profileID == 1)
			{
				if (m_channelN_in == 2)
				{
					*in_buf_ch_inter += *in_iter * 0;
					in_iter++;
					*in_buf_ch_inter += *in_iter * 1;
					in_iter++;
				}
				else if (m_channelN_in == 3)
				{
					*in_buf_ch_inter += *in_iter * 0;
					in_iter++;
					*in_buf_ch_inter += *in_iter * 1;
					in_iter++;
				}
				else if (m_channelN_in == 4)
				{
					*in_buf_ch_inter += *in_iter * 0;
					in_iter++;
					*in_buf_ch_inter += *in_iter * 1;
					in_iter++;
				}
				else if (m_channelN_in == 6)
				{
					*in_buf_ch_inter += *in_iter * 1;
					in_iter++;
					*in_buf_ch_inter += *in_iter * 0;
					in_iter++;
				}
				else
				{
					throw;
				}
			}
			else if (m_profileID == 2) {
				if (m_channelN_in == 7) {
					*in_buf_ch_inter += *in_iter * 0;
					in_iter++;
					*in_buf_ch_inter += *in_iter * 1;
					in_iter++;
				}
				else if (m_channelN_in == 4) {
					*in_buf_ch_inter += *in_iter * 0;
					in_iter++;
					*in_buf_ch_inter += *in_iter * 1;
					in_iter++;
				}
				else if (m_channelN_in == 6) {
					*in_buf_ch_inter += *in_iter * 0;
					in_iter++;
					*in_buf_ch_inter += *in_iter * 1;
					in_iter++;
				}
				else {
					throw;
				}
			}
			else
			{
				throw;
			}
			in_buf_ch_inter++;
		}
		/* Output. */
		const float *buf_real_ch_iter = m_buf_real_ch[0] + m_overlap_len;
		float *out_iter = out + c;
		for (int i = 0; i < m_frame_len; i++) {
			*out_iter = *buf_real_ch_iter;
			out_iter += m_channelN; buf_real_ch_iter++;
		}
	}
}


void CBeamforming::delay_sum(const float *in)
{
	for (int i = 0; i < m_channelN_in; i++)
	{
		memcpy(&m_delayBuf_ch[i][0], &m_delayBuf_ch[i][m_frame_len], m_overlap_len * sizeof(float));
	}
	for (int i = 0; i < m_channelN_in; i++)
	{
		for (int j = 0; j < m_frame_len; j++)
		{
			m_delayBuf_ch[i][j + m_overlap_len] = in[j*m_channelN_in + i];
		}
	}
	for (int i = 0; i < m_channelN_in; i++) {
		for (int j = 0; j < m_frame_len * 2; j++) {
			m_anaBuf_ch[i][j] = m_delayBuf_ch[i][j] * m_hanning_win[j];
		}
		for (int j = m_frame_len * 2; j < m_fft_len; j++)
		{
			m_anaBuf_ch[i][j] = 0;
		}
	}
	for (int i = 0; i < m_channelN_in; i++)
	{
		m_real_fft_delay->run_fwd_fft(&m_anaBuf_ch[i][0], &m_delayBuf_spec_ch[i][0]);
	}

	if (m_mode == 3) {
		mvdr_c();
	}
	

	for (int i = 0; i < m_channelN_in; i++)
	{
		const C *hrir_spec_iter = m_delayVectors_c[i];
		C *fft_spec_iter = m_delayBuf_spec_ch[i];
		C *fft_spec_out_iter = m_delaySumBuf_spec_ch[i];
		for (int j = 0; j < m_fft_len_div_2 + 1; j++) {
			R re = fft_spec_iter->re * hrir_spec_iter->re + fft_spec_iter->im * hrir_spec_iter->im;
			R im = -fft_spec_iter->re * hrir_spec_iter->im + fft_spec_iter->im * hrir_spec_iter->re;
			fft_spec_out_iter->re = re;
			fft_spec_out_iter->im = im;
			hrir_spec_iter++; fft_spec_iter++; fft_spec_out_iter++;
		}
	}


	for (int i = 0; i < m_channelN_in; i++)
	{
		const C *hrir_spec_iter;

		if (m_mode == 3) {
			hrir_spec_iter = m_mvdrVectors_c[i];
			//hrir_spec_iter = m_superVectors_c[i];
		}
		else{
			//hrir_spec_iter = m_mvdrVectors_c[i];
			hrir_spec_iter = m_superVectors_c[i];
		}
		C *fft_spec_iter = m_delayBuf_spec_ch[i];
		for (int j = 0; j < m_fft_len_div_2 + 1; j++) {
			R re = fft_spec_iter->re * hrir_spec_iter->re + fft_spec_iter->im * hrir_spec_iter->im;
			R im = -fft_spec_iter->re * hrir_spec_iter->im + fft_spec_iter->im * hrir_spec_iter->re;
			fft_spec_iter->re = re;
			fft_spec_iter->im = im;
			hrir_spec_iter++; fft_spec_iter++;
		}
	}

	for (int j = 0; j < m_fft_len_div_2 + 1; j++) {
		C sum; sum.re = 0; sum.im = 0;
		for (int i = 0; i < m_channelN_in; i++) {
			C spec = m_delayBuf_spec_ch[i][j];
			sum.re += spec.re;
			sum.im += spec.im;
		}
		m_delayBuf_spec_ch[0][j] = sum;
	}

	post_filter();

	m_real_fft_delay->run_inv_fft(&m_delayBuf_spec_ch[0][0], &m_delayBuf_out_ch[0][0]);
	for (int j = 0; j < m_frame_len * 2; j++) {
		m_delayBuf_out_ch[0][j] *= m_hanning_win[j] * m_fft_scale;
	}
	for (int j = 0; j < m_frame_len; j++) {
		m_targetSig[j] = m_delayBuf_out_ch[0][j] + m_mem[0][j];
	}
	for (int j = 0; j < m_frame_len; j++) {
		m_mem[0][j] = m_delayBuf_out_ch[0][j + m_overlap_len];
	}	

	float *pt = m_in;
	for (int i = 0; i < m_frame_len; i++)
	{
		*pt++ = m_targetSig[i];
		*pt++ = m_targetSig[i];
	}
}

void CBeamforming::mvdr_c()
{
	C *sigma;
	const C *data_spec_iter;

	float smooth_factor;

	if (m_targetFlag) {
		smooth_factor = SMOOTHA;
	}
	else {
		if (m_targetFlag_prev) {
			smooth_factor = SMOOTHC;
		}
		else {
			smooth_factor = SMOOTHB;
		}	
	}
	m_targetFlag_prev = m_targetFlag;

	for (int b = 0; b < m_num_band; b++) {
		if (m_targetFlag == 0) {
			for (int c = 0; c < m_channelN_in; c++) {
				sigma = m_sigma_c + c * (m_channelN_in + 1);
				data_spec_iter = &m_delayBuf_spec_ch[c][b];
				(*sigma).re = data_spec_iter->re * data_spec_iter->re + data_spec_iter->im * data_spec_iter->im;
				(*sigma).im = 0.0f;
			}

			for (int i = 0; i < m_channelN_in; i++) {
				for (int j = i + 1; j < m_channelN_in; j++) {
					sigma = m_sigma_c + i * m_channelN_in + j;
					const C *data_spec_iter_1 = &m_delayBuf_spec_ch[i][b];
					const C *data_spec_iter_2 = &m_delayBuf_spec_ch[j][b];
					(*sigma).re = data_spec_iter_1->re * data_spec_iter_2->re + data_spec_iter_1->im * data_spec_iter_2->im;
					(*sigma).im = -data_spec_iter_1->re * data_spec_iter_2->im + data_spec_iter_1->im * data_spec_iter_2->re;

					/* Shane: It may be unnecessary. */
					(*(m_sigma_c + j * m_channelN_in + i)).re = (*sigma).re;
					(*(m_sigma_c + j * m_channelN_in + i)).im = -(*sigma).im;
				}
			}
		}
		else {
			for (int i = 0; i < m_channelN_in * m_channelN_in; i++) {
				m_sigma_c[i].re = m_superDirective[b][i];
				m_sigma_c[i].im = 0.0f;
			}
		}

		if (!m_first) {
			for (int i = 0; i < m_channelN_in * m_channelN_in; i++) {
				m_sigma_sm_c[b][i].re = smooth_factor * m_sigma_sm_c[b][i].re + (1.f - smooth_factor) * m_sigma_c[i].re;
				m_sigma_sm_c[b][i].im = smooth_factor * m_sigma_sm_c[b][i].im + (1.f - smooth_factor) * m_sigma_c[i].im;
			}
		}
		else {
			//memcpy(&m_sigma_sm_c[b][0], m_sigma_c, sizeof(C) * m_channelN_in * m_channelN_in);
			for (int i = 0; i < m_channelN_in * m_channelN_in; i++) {
				m_sigma_sm_c[b][i].re = m_sigma_c[i].re;
				m_sigma_sm_c[b][i].im = m_sigma_c[i].im;
			}
		}

		for (int i = 0; i < m_channelN_in * m_channelN_in; i++) {
			m_matric_c[i].re = m_sigma_sm_c[b][i].re + m_StableMatrix[i];
			m_matric_c[i].im = m_sigma_sm_c[b][i].im;
		}

		inverse_bin_c();

		for (int i = 0; i < m_channelN_in; i++) {  
			m_nom_c[i].re = 0;
			m_nom_c[i].im = 0;
			for (int j = 0; j < m_channelN_in; j++) {
				m_nom_c[i].re += m_matric_c[i + j * m_channelN_in].re * m_delayVectors_c[j][b].re - m_matric_c[i + j * m_channelN_in].im * m_delayVectors_c[j][b].im;
				m_nom_c[i].im += m_matric_c[i + j * m_channelN_in].im * m_delayVectors_c[j][b].re + m_matric_c[i + j * m_channelN_in].re * m_delayVectors_c[j][b].im;

			}
		}

		float den = 0;
		for (int k = 0; k < m_channelN_in; k++) {
			C sum;
			sum.re = 0; sum.im = 0;
			for (int j = 0; j < m_channelN_in; j++) {
				sum.re += m_matric_c[k*m_channelN_in + j].re * m_delayVectors_c[j][b].re + m_matric_c[k*m_channelN_in + j].im * m_delayVectors_c[j][b].im;
				sum.im += -m_matric_c[k*m_channelN_in + j].im * m_delayVectors_c[j][b].re + m_matric_c[k*m_channelN_in + j].re * m_delayVectors_c[j][b].im;
			}
			m_tmpv[k].re = sum.re;
			m_tmpv[k].im = sum.im;
		}
		for (int j = 0; j < m_channelN_in; j++) {
			den += m_delayVectors_c[j][b].re*m_tmpv[j].re + m_delayVectors_c[j][b].im*m_tmpv[j].im;
		}

		//den = fabs(den);
		den = max(EPS_32F, den);

		for (int i = 0; i < m_channelN_in; i++) {
			m_mvdrVectors_c[i][b].re = m_nom_c[i].re / den;
			m_mvdrVectors_c[i][b].im = m_nom_c[i].im / den;
		}
	}

	if (m_first) {
		m_first = false;
	}
}

void CBeamforming::inverse_bin_c()
{
	integer n = (integer)m_channelN_in;
	integer lda = (integer)m_channelN_in;
	integer lwork = (integer)LWORK;  // LWORK >= max(1,3*N-1)
	integer info;
	integer ipiv[LWORK];

	for (int i = 0; i < m_channelN_in; i++) {
		for (int j = 0; j < m_channelN_in; j++) {
			a_c[i*m_channelN_in + j].r = m_matric_c[i*m_channelN_in + j].re;
			a_c[i*m_channelN_in + j].i = -m_matric_c[i*m_channelN_in + j].im;
		}
	}
	char U_buf[2] = { 'U' };
	chetrf_(U_buf, &n, a_c, &lda, ipiv, m_work_c, &lwork, &info);
	if ((integer)0 != info) {
		throw;
	}
	else {
		chetri_(U_buf, &n, a_c, &lda, ipiv, m_work_c, &info);
		if ((integer)0 != info) {
			throw;
		}
	}

	for (int i = 0; i < m_channelN_in - 1; i++) {
		int k = 1;
		for (int j = i*m_channelN_in + i + 1; j < (i + 1)*m_channelN_in; j++) {
			a_c[j].r = a_c[j + k *(m_channelN_in - 1)].r;
			a_c[j].i = -a_c[j + k *(m_channelN_in - 1)].i;
			k++;
		}
	}

	for (int i = 0; i < m_channelN_in * m_channelN_in; i++) {
		m_matric_c[i].re = a_c[i].r;
		m_matric_c[i].im = a_c[i].i;
	}
}
void CBeamforming::post_filter()
{
	memset(m_postFilter_nom, 0x00, sizeof(R) * (m_fft_len_div_2 + 1));
	memset(m_postFilter_den, 0x00, sizeof(R) * (m_fft_len_div_2 + 1));

	for (int i = 0; i < m_channelN_in-1; i++)
	{
		for (int j = i+1; j < m_channelN_in; j++)
		{
			C *Yi = m_delaySumBuf_spec_ch[i];
			C *Yj = m_delaySumBuf_spec_ch[j];
			R *nom = m_postFilter_nom;
			for (int b = 0; b < m_fft_len_div_2 + 1; b++) {
				*nom += Yi->re * Yj->re + Yi->im * Yj->im;
				Yi++; Yj++; nom++;
			}
		}				
	}


	{
		C *Y = m_delayBuf_spec_ch[0];
		R *den = m_postFilter_den;
		for (int b = 0; b < m_fft_len_div_2 + 1; b++) {
			*den = Y->re * Y->re + Y->im * Y->im;
			Y++; den++;
		}
	}

	{
		R *gain = m_postFilter_gain;
		R *nom = m_postFilter_nom;
		R *den = m_postFilter_den;
		R *nom_avg = m_postFilter_nom_avg;
		R *den_avg = m_postFilter_den_avg;
		float smoothPostFilter = 0.9;
		for (int b = 0; b < m_fft_len_div_2 + 1; b++) {
			*nom_avg = (*nom_avg)*smoothPostFilter + (*nom)*(1 - smoothPostFilter);
			*den_avg = (*den_avg)*smoothPostFilter + (*den)*(1 - smoothPostFilter);
			float weight = *nom_avg * 2 / (m_channelN_in*m_channelN_in - m_channelN_in) / (*den_avg + 0.000001f);
			if (weight > 1) weight = 1;
			if (weight < 0) weight = 0;
			*gain= weight;
			gain++; nom++; den++; nom_avg++; den_avg++;
		}
	}
}



// Update is called once every frame
void CBeamforming::process(const float *in, float *out)
{
	prepare(in, out, -1);
	overlap_plus_pre(-1);
	gain_estimator();
	overlap_plus_post(out, -1);

	if (m_multithread) {
		for (int i = 0; i < m_channelN_in; i++) {
			m_done_conds[i].lock();
		}
		m_obj_in = in;
		m_obj_out = out;
		for (int i = 0; i < m_channelN_in; i++) {
			m_ready_conds[i].signal_s();
		}
		for (int i = 0; i < m_channelN_in; i++) {
			m_done_conds[i].wait();
			m_done_conds[i].unlock();
		}
	}
	else {
			if (m_profileID == 0)
			{
				prepare(in, out, 0);
				prepare(in, out, 1);
				overlap_plus_pre(0);
				overlap_plus_pre(1);
				gain_estimator_ch(0);
				//gain_estimator_ch(1);
				overlap_plus_post(out, 0);
				//overlap_plus_post(out, 1);
			}
			else if (m_profileID == 1)
			{				
				delay_sum(in);	
				
				prepare(m_in, out, 0);
				prepare(m_in, out, 1);
				overlap_plus_pre(0);
				//overlap_plus_pre(1);
				gain_estimator_ch_opt(0);
				//gain_estimator_ch(1);
				overlap_plus_post(out, 0);
				//overlap_plus_post(out, 1);
		}
		else if (m_profileID == 2) {
			delay_sum(in);
			prepare(m_in, out, 0);
			prepare(m_in, out, 1);
			overlap_plus_pre(0);
			//overlap_plus_pre(1);
			gain_estimator_ch_opt(0);
			//gain_estimator_ch(1);
			overlap_plus_post(out, 0);
			//overlap_plus_post(out, 1);
		}
			else
			{
				throw;
			}
			
	}
	m_frame_cnt++;
}

void CBeamforming::set(int mode, float max_sup_db, float targetDirection, float targetEstimation, float nlp_sup, float tolerance, int targetFlag)
{

	if( mode == 3 || mode == 5 || mode == 7){
		m_mode = mode;
	}
	else {
		m_mode = 1;
	}

	m_targetDirection = targetDirection;
	m_targetEstimation = targetEstimation;
	if (m_profileID == 0) {
		if (m_targetDirection < 0)
			m_targetDirection += 360;
		m_nullDirection = m_targetDirection - 180;
		if (m_nullDirection < 0)
			m_nullDirection += 360;

		m_targetDirectionIndex = (int)(m_targetDirection / 360 * M);
		if (m_targetDirectionIndex >= M || m_targetDirectionIndex<0)
			m_targetDirectionIndex = 0;
		m_nullDirectionIndex = (int)(m_nullDirection / 360 * M);
		if (m_nullDirectionIndex >= M || m_nullDirectionIndex<0)
			m_nullDirectionIndex = 0;

		m_nlp_sup = nlp_sup;
	}
	else if(m_profileID == 1){
		m_targetDirection = 90 - m_targetDirection;   //0~180
		m_targetEstimation = 90 - m_targetEstimation;
		m_targetDirectionIndex = (int)(m_targetDirection / 180 * M);
		if (m_targetDirectionIndex >= M || m_targetDirectionIndex<0)
			m_targetDirectionIndex = 0;

		float err = fabs(m_targetDirection - m_targetEstimation);
		float TLow = tolerance / 2.f;
		float THigh = TLow + 5.f;
		
		if (err < TLow){
			m_nlp_gain = 1.0;
		}
		else if (err > THigh){
			m_nlp_gain = 0.0;
		}
		else{
			m_nlp_gain = (THigh - err) / (THigh - TLow);
		}
		m_nlp_factor = nlp_sup;
	}
	else if (m_profileID == 2) {
		m_targetDirection += 180;
		m_targetEstimation += 180;
		if (m_targetDirection < 0)
			m_targetDirection += 360;
		if (m_targetEstimation < 0)
			m_targetEstimation += 360;

		m_targetDirectionIndex = (int)(m_targetDirection / 360 * M);
		if (m_targetDirectionIndex >= M || m_targetDirectionIndex<0)
			m_targetDirectionIndex = 0;

		float err = fabs(m_targetDirection - m_targetEstimation);
		float TLow = tolerance / 2.f;
		float THigh = TLow + 5.f;
		if (err >(360 - TLow) || err < TLow) {
			m_nlp_gain = 1.0;
		}
		else if (err < (360 - THigh) && err > THigh) {
			m_nlp_gain = 0.0;
		}
		else if (err >= TLow && err <= THigh) {
			m_nlp_gain = (THigh - err) / (THigh - TLow);
		}
		else {
			m_nlp_gain = ( err -(360 - THigh) ) / (THigh - TLow);
		}
	
		m_nlp_factor = nlp_sup;
	}
	else {
		throw;
	}

	m_max_sup_db = max_sup_db;
	if (m_max_sup_db > MAXSUP1) {
		m_max_sup_db = MAXSUP1;
	}
	if (m_max_sup_db < MAXSUP2) {
		m_max_sup_db = MAXSUP2;
	}
	//m_max_sup_db = powf(10.f, m_max_sup_db / 20.f);

	
	if (m_nlp_sup > MAXNLPSUP1) {
		m_nlp_sup = MAXNLPSUP1;
	}
	if (m_nlp_sup < MAXNLPSUP2) {
		m_nlp_sup = MAXNLPSUP2;
	}
	m_targetFlag = targetFlag;
	prebakeSteerVector();
}

void * CBeamforming::thread_process(void *args)
{
	CBeamforming::ThreadArgs* user = (CBeamforming::ThreadArgs*)args;
	user->user->thread_process_do(user->index);
	return NULL;
}

void CBeamforming::thread_process_do(int index)
{
	m_ready_conds[index].lock();
	m_prep_conds[index].signal_s();
	for (;;) {
		m_ready_conds[index].wait();
		/* This thread cancel point. */
		if (m_thread_break) {
			break;
		}
		prepare(m_obj_in, m_obj_out, index);
		overlap_plus_pre(index);
		gain_estimator_ch(index);
		overlap_plus_post(m_obj_out, index);
		m_done_conds[index].signal_s();
	}
	m_ready_conds[index].unlock();
}

/*
function steerV = prebackSteerVector(M)

alpha1 = pi / 2;
alpha2 = pi;
alpha3 = -pi / 2;
alpha4 = 0;

for k = 1:M
alpha = 2 * pi*k / M;
v1 = 0.5 + 0.5*cos(alpha1 - alpha);
v2 = 0.5 + 0.5*cos(alpha2 - alpha);
v3 = 0.5 + 0.5*cos(alpha3 - alpha);
v4 = 0.5 + 0.5*cos(alpha4 - alpha);
steerV(k, :) = [v1 v2 v3 v4];   %steer vectors
end

return;
*/


void CBeamforming::prebakeSteerVector()
{
	if (m_profileID == 0)
	{
		if (m_channelN_in == 4) //for 4 channel a format
		{
			for (int i = 0; i < M; i++)
			{
				for (int j = 0; j < m_channelN_in; j++)
					m_steerVectors[i][j] = 0.5 + 0.5*cos(2 * M_PI*i / M - 2 * M_PI*j / m_channelN_in);
			}
		}
		else if (m_channelN_in == 3) //for 3 channel LRS
		{
			for (int i = 0; i < M; i++)
			{
				for (int j = 0; j < m_channelN_in; j++)
					m_steerVectors[i][j] = 0.5 + 0.5*cos(2 * M_PI*i / M - 2 * M_PI*j / m_channelN_in /*+ M_PI / m_channelN_in*/);
			}
		}
	}
	else if (m_profileID == 1) //
	{
		float c0 = 346.287;                    // Speed of sound in air

		for (int b = 0; b < m_fft_len_div_2 + 1; b++) {
			float f0 = (float)m_sampleRate*(b + 1) / m_fft_len;
			float omega = 2 * M_PI*f0;
			float k0 = omega / c0;
			float k = m_targetDirection;   //-90~90  -> 180~0
			float alpha = M_PI * (float)k / 180;
			float phi = -k0*m_micDist*cos(alpha);
			float phi0 = -k0*m_micDist*(m_channelN_in - 1);  //avoid negative delay
			phi0 = -phi0;  //by zxt
			for (int i = 0; i < m_channelN_in; i++) {
				m_delayVectors_c[i][b].re = cos(i*phi+phi0);
				m_delayVectors_c[i][b].im = sin(i*phi+phi0);
			}		


			f0 = (float)m_sampleRate*(b+1) / m_fft_len;
			omega = 2 * M_PI*f0;
			k0 = omega / c0;
			alpha = k0*m_micDist;
			for (int i = 0; i < m_channelN_in; i++) {
				for (int j = 0; j < m_channelN_in; j++) {
					float gap = fabs((float)i - j);
					//if (m_mode == 3)   //hack for delay sum;
					//{
					//	if (gap == 0) {
					//		m_superDirective[b][j*m_channelN_in + i] = 1.00f;
					//	}
					//	else {
					//		m_superDirective[b][j*m_channelN_in + i] = 0;
					//	}
					//}
					//else
					{
						if (gap == 0) {
							m_superDirective[b][j*m_channelN_in + i] = 1.01f;
						}
						else {
							m_superDirective[b][j*m_channelN_in + i] = sin(alpha * gap) / (alpha * gap);
						}
					}
				}
			}


			if (m_channelN_in == 4) {     
				for (int i = 0; i < m_channelN_in; i++) {
					long index = m_targetDirectionIndex*m_channelN_in*m_num_band + i*m_num_band + b;
					if (m_micDist < 0.05) {  //the distance is 0.04m
						m_superVectors_c[i][b].re = superdirective_real4[index];
						m_superVectors_c[i][b].im = superdirective_imag4[index];
					}
					else  //the distance is 0.06m
					{
						m_superVectors_c[i][b].re = superdirective_real[index];
						m_superVectors_c[i][b].im = superdirective_imag[index];
					}
			
				}
			}
			else if (m_channelN_in == 2) {    //the distance is 0.06m
				for (int i = 0; i < m_channelN_in; i++) {
					long index = m_targetDirectionIndex*m_channelN_in*m_num_band + i*m_num_band + b;
					m_superVectors_c[i][b].re = superdirective_omni2_real[index];
					m_superVectors_c[i][b].im = superdirective_omni2_imag[index];
				}
			}
			else if (m_channelN_in == 6) {    //the distance is 0.1m
				for (int i = 0; i < m_channelN_in; i++) {
					long index = m_targetDirectionIndex*m_channelN_in*m_num_band + i*m_num_band + b;
					m_superVectors_c[i][b].re = superdirective_omni6_real[index];
					m_superVectors_c[i][b].im = superdirective_omni6_imag[index];
				}
			}
			else {
				throw;
			}
		}
	}
	else if (m_profileID == 2) {
		if (m_channelN_in == 7) // for Respeaker
		{
			float c0 = 346.287;                    // Speed of sound in air

			for (int b = 0; b < m_fft_len_div_2 + 1; b++) {
				float f0 = (float)m_sampleRate*(b + 1) / m_fft_len;
				float omega = 2 * M_PI*f0;
				float k0 = omega / c0;
				float k = m_targetDirection;   //-180~180  -> 0~360
				float alpha = M_PI * (float)k / 180;
				float phi0 = k0*m_micDist * 2;  //avoid negative delay
				for (int i = 0; i < m_channelN_in; i++) {
					float phi;
					if (i == 0) {
						phi = 0;
					}
					else {
						phi = -k0 * m_micDist * cos(alpha - 2 * (i - 1) * M_PI / (m_channelN_in - 1));
					}
					m_delayVectors_c[i][b].re = cos(phi + phi0);
					m_delayVectors_c[i][b].im = sin(phi + phi0);
				}

				for (int i = 0; i < m_channelN_in; i++) {
					long index = m_targetDirectionIndex*m_channelN_in*m_num_band + i*m_num_band + b;
					m_superVectors_c[i][b].re = superdirective_Respeaker_real[index];
					m_superVectors_c[i][b].im = superdirective_Respeaker_imag[index];
				}
			} 
		}
		else if (m_channelN_in == 4)
		{
			float c0 = 346.287;                    // Speed of sound in air

			for (int b = 0; b < m_fft_len_div_2 + 1; b++) {
				float f0 = (float)m_sampleRate*(b + 1) / m_fft_len;
				float omega = 2 * M_PI*f0;
				float k0 = omega / c0;
				float k = m_targetDirection;   //-180~180  -> 0~360
				float alpha = M_PI * (float)k / 180;
				float phi0 = k0*m_micDist * 2;  //avoid negative delay
				for (int i = 0; i < m_channelN_in; i++) {
					float phi;
					phi = -k0 * m_micDist * cos(alpha - 2 * i * M_PI / m_channelN_in);
					m_delayVectors_c[i][b].re = cos(phi + phi0);
					m_delayVectors_c[i][b].im = sin(phi + phi0);
				}

				for (int i = 0; i < m_channelN_in; i++) {
					long index = m_targetDirectionIndex*m_channelN_in*m_num_band + i*m_num_band + b;
					m_superVectors_c[i][b].re = superdirective_circular4_real[index];
					m_superVectors_c[i][b].im = superdirective_circular4_imag[index];
				}
			}
		}
		else if (m_channelN_in == 6)
		{
			float c0 = 346.287;                    // Speed of sound in air

			for (int b = 0; b < m_fft_len_div_2 + 1; b++) {
				float f0 = (float)m_sampleRate*(b + 1) / m_fft_len;
				float omega = 2 * M_PI*f0;
				float k0 = omega / c0;
				float k = m_targetDirection;   //-180~180  -> 0~360
				float alpha = M_PI * (float)k / 180;
				float phi0 = k0*m_micDist * 2;  //avoid negative delay
				for (int i = 0; i < m_channelN_in; i++) {
					float phi;
					phi = -k0 * m_micDist * cos(alpha - 2 * i * M_PI / m_channelN_in);
					m_delayVectors_c[i][b].re = cos(phi + phi0);
					m_delayVectors_c[i][b].im = sin(phi + phi0);
				}

				for (int i = 0; i < m_channelN_in; i++) {
					long index = m_targetDirectionIndex*m_channelN_in*m_num_band + i*m_num_band + b;
					m_superVectors_c[i][b].re = superdirective_circular6_real[index];
					m_superVectors_c[i][b].im = superdirective_circular6_imag[index];
				}
			}
		}
		else {
			throw;
		}
	}
	else
	{
		throw;
	}
}

#if 0
float CBeamforming::getA(float **arcs, int n)
{
	if (n == 1)
	{
		return arcs[0][0];
	}

	int i, j, k;
	float ans = 0;
	float **temp = (float **)malloc(sizeof(float *) *n);
	for (i = 0; i < n; i++) {
		temp[i] = (float *)malloc(sizeof(float *) *n);
	}
	
	for (i = 0; i<n; i++)
	{
		for (j = 0; j<n - 1; j++)
		{
			for (k = 0; k<n - 1; k++)
			{
				temp[j][k] = arcs[j + 1][(k >= i) ? k + 1:k];
			}
		}

		float t = getA(temp, n - 1);
		if (i % 2 == 0)
		{
			ans += arcs[0][i] * t;
		}
		else
		{
			ans -= arcs[0][i] * t;
		}
	}
	return ans;
}

//计算每一行每一列的每个元素所对应的余子式，组成A*
void  CBeamforming::getAStart(float **arcs, int n, float **ans)
{
	if (n == 1)
	{
		ans[0][0] = 1;
		return;
	}
	int i, j, k, t;
	float num = getA(arcs, n);
	float **tmp = (float **)malloc(sizeof(float *) *n);
	for (int i = 0; i < n; i++) {
		tmp[i] = (float *)malloc(sizeof(float *) *n);
	}
	
	for (i = 0; i<n; i++)
	{
		for (j = 0; j<n; j++)
		{
			for (k = 0; k<n - 1; k++)
			{
				for (t = 0; t < n - 1; t++)
				{
					tmp[k][t] = arcs[k >= i ? k + 1 : k][t >= j ? t + 1 : t];
				}
			}
			ans[j][i] = getA(tmp, n - 1) / (num + SMALLV1);
			if ((i + j) % 2 == 1)
			{
				ans[j][i] = -ans[j][i];
			}
		}
	}
}

void CBeamforming::matricAdd(float **a, float **b, float **c, int n) 
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			c[i][j] = a[i][j] + b[i][j];
		}
	}
}

void CBeamforming::matricMul(float **a, float **b, float **c, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			c[i][j] = 0;
			for (int k = 0; k < n; k++) {
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}
}

void CBeamforming::getMatrixInverse(float *real, float *image, int n)  //a:the real part, b:the image part, n:the rank
{
	float **a = (float **)malloc(sizeof(float *) *n);
	for (int i = 0; i < n; i++) {
		a[i] = (float *)malloc(sizeof(float *) *n);
	}

	float **b = (float **)malloc(sizeof(float *) *n);
	for (int i = 0; i < n; i++) {
		b[i] = (float *)malloc(sizeof(float *) *n);
	}

	float **c = (float **)malloc(sizeof(float *) *n);
	for (int i = 0; i < n; i++) {
		c[i] = (float *)malloc(sizeof(float *) *n);
	}

	float **d = (float **)malloc(sizeof(float *) *n);
	for (int i = 0; i < n; i++) {
		d[i] = (float *)malloc(sizeof(float *) *n);
	}

	int k = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			a[i][j] = real[k];
			b[i][j] = image[k] + SMALLV1;
			k++;
		}
	}

	//the real part
	getAStart(a, n, c);
	matricMul(c, b, d, n);
	matricMul(b, d, c, n);
	matricAdd(a, c, c, n);
	getAStart(c, n, a);

	//the image part
	matricMul(d, a, b, n);

	k = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			real[k] = a[i][j];
			image[k] = b[i][j];
			k++;
		}
	}
}
#endif
