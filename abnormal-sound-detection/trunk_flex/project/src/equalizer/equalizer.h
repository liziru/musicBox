#ifndef __EQUALIZER_H__
#define __EQUALIZER_H__

#include "../fft/realFFT.h"

class CThread;
class CCond;

class CEqualizer
{
public:
	CEqualizer(int frame_length, int channelN, int sample_rate, bool multithread);
	~CEqualizer();

	void process(const float *in, float *out, const float *param);

private:
	struct ThreadArgs_s
	{
		int index;
		CEqualizer *user;
	};
	typedef ThreadArgs_s ThreadArgs;

	int m_frame_len;
	int m_fft_len;
	int m_overlap_len;
	int m_fft_len_div_2;
	int m_eq_num;
	int m_channelN;
	float **m_buf;
	C **m_spec;
	R **m_real;
	float* m_eq_linear;
	float* m_eq_xis;
	float *m_sin_win;
	CRealFFT **m_real_fft;
	float m_fft_scale;
	bool m_multithread;
	CCond *m_ready_conds;
	CCond *m_done_conds;
	CCond *m_prep_conds;
	CThread *m_threads;
	bool m_thread_break;
	const float *m_obj_in;
	float *m_obj_out;

	static void *thread_process(void *args);
	void thread_process_do(int index);
	void prepare(const float *in, float *out, int c);
	void eq_pre(int c);
	void eq_post(float *out, int c);
};

#endif /*__EQUALIZER_H__*/