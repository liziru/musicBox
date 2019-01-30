#include "equalizer.h"
#include "../utils/cond.h"
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
#define FFT_LEN (1024)
#define FFT_LEN_16k  (512)
#define EQ_LOG_NUM (10)
#define EQ_LOG_NUM_16k (8)

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

static const float c_eq_log[EQ_LOG_NUM] =
{
	31, 62, 125, 250, 500, 1000, 2000, 4000, 8000, 16000
};

static const float c_eq_log_16k[EQ_LOG_NUM_16k] =
{
	31, 62, 125, 250, 500, 1000, 2000, 4000
};

CEqualizer::CEqualizer(int frame_length, int channelN, int sample_rate, bool multithread)
	: m_spec(NULL)
	, m_real(NULL)
	, m_buf(NULL)
	, m_sin_win(NULL)
	, m_real_fft(NULL)
	, m_multithread(multithread)
	, m_ready_conds(NULL)
	, m_done_conds(NULL)
	, m_prep_conds(NULL)
	, m_threads(NULL)
	, m_thread_break(false)
	, m_obj_in(NULL)
	, m_obj_out(NULL)
{
	m_frame_len = frame_length;
	if (m_frame_len == 256) {
		m_fft_len = FFT_LEN_16k;
		m_eq_num = EQ_LOG_NUM_16k;
	}
	else {
		m_fft_len = FFT_LEN;
		m_eq_num = EQ_LOG_NUM;
	}

	m_overlap_len = m_fft_len - m_frame_len;
	m_fft_len_div_2 = m_fft_len >> 1;

	m_channelN = channelN;

	m_spec = new C*[channelN];
	m_real = new R*[channelN];
	m_buf = new float*[channelN];
	for (int i = 0; i < channelN; i++) {
		m_spec[i] = new C[m_fft_len_div_2 + 1];
		m_real[i] = new R[m_fft_len];
		memset(m_real[i], 0x00, sizeof(R) * m_fft_len);
		m_buf[i] = new float[m_fft_len];
		memset(m_buf[i], 0x00, sizeof(float) * m_fft_len);
	}

	m_eq_linear = new float[m_fft_len_div_2 + 1];
	m_eq_xis = new float[m_eq_num];
	for (int i = 0; i < m_eq_num; i++) {
		m_eq_xis[i] = c_eq_log[i] / sample_rate * m_fft_len;
	}

	uint logN = (uint)(log((float)m_fft_len) / log(2.0f) + 0.5f);
	int fft_num = m_multithread ? channelN : 1;
	m_real_fft = new CRealFFT*[fft_num];
	for (int i = 0; i < fft_num; i++) {
		m_real_fft[i] = new CRealFFT(logN);  //andy: real fft
	}
	m_fft_scale = 1.f / (float)m_fft_len;

	if (m_multithread) {
		m_ready_conds = new CCond[channelN];
		m_done_conds = new CCond[channelN];
		m_prep_conds = new CCond[channelN];
		m_threads = new CThread[channelN];
		m_thread_break = false;
		for (int i = 0; i < channelN; i++) {
			m_prep_conds[i].lock();
			ThreadArgs args;
			args.index = i;
			args.user = this;
			m_threads[i].create(CEqualizer::thread_process, &args);
			m_prep_conds[i].wait();
			m_prep_conds[i].unlock();
		}
	}

	m_sin_win = new float[m_fft_len];
	for (int i = 0; i < m_fft_len; i++) {
		m_sin_win[i] = sin(M_PI * ((float)i + 0.5f) / (float)m_fft_len);
	}
}

CEqualizer::~CEqualizer()
{
	if (m_multithread) {
		m_thread_break = true;
		for (int i = 0; i < m_channelN; i++) {
			m_ready_conds[i].signal_s();
			m_threads[i].join();
		}
	}
	RELEASE_ARRAY(m_threads);
	RELEASE_ARRAY(m_ready_conds);
	RELEASE_ARRAY(m_done_conds);
	RELEASE_ARRAY(m_prep_conds);
	int fft_num = m_multithread ? m_channelN : 1;
	for (int i = 0; i < fft_num; i++) {
		RELEASE_POINTER(m_real_fft[i]);
	}
	RELEASE_ARRAY(m_real_fft);

	for (int i = 0; i < m_channelN; i++) {
		RELEASE_ARRAY(m_buf[i]);
		RELEASE_ARRAY(m_real[i]);
		RELEASE_ARRAY(m_spec[i]);
	}
	RELEASE_ARRAY(m_buf);
	RELEASE_ARRAY(m_real);
	RELEASE_ARRAY(m_spec);

	RELEASE_ARRAY(m_sin_win);
	RELEASE_ARRAY(m_eq_linear);
	RELEASE_ARRAY(m_eq_xis);
}

// Update is called once every frame
void CEqualizer::process(const float *in, float *out, const float *param)
{
	float eq_gain[EQ_LOG_NUM];
	for (int i = 0; i < m_eq_num; i++) {
		eq_gain[i] = pow(10.f, param[i] / 20.f);
	}

	for (int i = 0; i < m_fft_len_div_2 + 1; i++) {
		if (i <= m_eq_xis[0]) {
			m_eq_linear[i] = eq_gain[0];
		}
		else if (i > m_eq_xis[m_eq_num - 1]) {
			m_eq_linear[i] = eq_gain[m_eq_num - 1];
		}
		else {
			for (int j = 0; j < m_eq_num - 1; j++) {
				if (i > m_eq_xis[j] && i <= m_eq_xis[j + 1]) {
					float inter_factor = ((float)i - m_eq_xis[j]) / ((float)m_eq_xis[j + 1] - m_eq_xis[j]);
					m_eq_linear[i] = eq_gain[j] * (1.f - inter_factor) + eq_gain[j + 1] * inter_factor;
					break;
				}
			}
		}
	}

	if (m_multithread) {
		for (int i = 0; i < m_channelN; i++) {
			m_done_conds[i].lock();
		}
		m_obj_in = in;
		m_obj_out = out;
		for (int i = 0; i < m_channelN; i++) {
			m_ready_conds[i].signal_s();
		}
		for (int i = 0; i < m_channelN; i++) {
			m_done_conds[i].wait();
			m_done_conds[i].unlock();
		}
	}
	else {
		for (int c = 0; c < m_channelN; c++) {
			prepare(in, out, c);
			eq_pre(c);
			eq_post(out, c);
		}
	}
}

void * CEqualizer::thread_process(void *args)
{
	CEqualizer::ThreadArgs* user = (CEqualizer::ThreadArgs*)args;
	user->user->thread_process_do(user->index);
	return NULL;
}

void CEqualizer::thread_process_do(int index)
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
		eq_pre(index);
		eq_post(m_obj_out, index);
		m_done_conds[index].signal_s();
	}
	m_ready_conds[index].unlock();
}

void CEqualizer::prepare(const float *in, float *out, int c)
{
	/* Input. */
	memmove(m_buf[c], m_buf[c] + m_frame_len, sizeof(float) * m_overlap_len);
	const float *in_iter = in + c;
	float *buf_iter = m_buf[c] + m_overlap_len;
	for (int i = 0; i < m_frame_len; i++) {
		*buf_iter = *in_iter;
		buf_iter++; in_iter += m_channelN;
	}
	/* Output. */
	const float *real_iter = m_real[c] + m_overlap_len;
	float *out_iter = out + c;
	for (int i = 0; i < m_frame_len; i++) {
		*out_iter = *real_iter;
		out_iter += m_channelN; real_iter++;
	}
}

void CEqualizer::eq_pre(int c)
{
	const float *buf_iter = m_buf[c];
	const float *sin_win_iter = m_sin_win;
	R *real_iter = m_real[c];
	for (int i = 0; i < m_fft_len; i++) {
		*real_iter = *sin_win_iter * *buf_iter;
		real_iter++; sin_win_iter++; buf_iter++;
	}
	m_real_fft[m_multithread ? c : 0]->run_fwd_fft(m_real[c], m_spec[c]);
	C *spec_iter = m_spec[c];
	const float *eq_linear_iter = m_eq_linear;
	for (int i = 0; i < m_fft_len_div_2 + 1; i++) {
		spec_iter->re *= *eq_linear_iter;
		spec_iter->im *= *eq_linear_iter;
		spec_iter++; eq_linear_iter++;
	}
}

void CEqualizer::eq_post(float *out, int c)
{
	m_real_fft[m_multithread ? c : 0]->run_inv_fft(m_spec[c], m_real[c]);
	const float *sin_win_iter = m_sin_win;
	R *real_iter = m_real[c];
	for (int i = 0; i < m_fft_len; i++) {
		*real_iter *= (*sin_win_iter * m_fft_scale);
		real_iter++; sin_win_iter++;
	}
	float *out_iter = out + c;
	real_iter = m_real[c];
	for (int i = 0; i < m_frame_len; i++) {
		*out_iter += *real_iter;
		out_iter += m_channelN; real_iter++;
	}
}