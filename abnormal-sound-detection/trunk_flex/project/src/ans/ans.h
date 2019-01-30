#ifndef ANS_H
#define ANS_H

#define ANS_DEBUG (0)

#include "../fft/realFFT.h"

class CThread;
class CCond;

class CAns
{
public:
	CAns(int frame_length, int channelN, int sampleRate, float max_sup_db, bool multithread);
	~CAns();

	void process(const float *in, float *out);
	void set(float max_sup_db);

private:
	void gain_estimator();
	void snr_estimator();
	void spectralFlatness();
	void feature_estimator();
	void noise_spectrum_estimator();
	void prepare(const float *in, float *out, int c);
	void overlap_plus_pre(int c);
	void gain_estimator_ch(int c);
	void overlap_plus_post(float *out, int c);
	

private:
	typedef struct {
		float  speechAbsencePossibilityAvg;
		float  avgsnrglobal;
		float  noiseTrackStep;
	} ResGlobalData;

	struct ThreadArgs_s
	{
		int index;
		CAns *user;
	};
	typedef ThreadArgs_s ThreadArgs;

	int m_frame_len;               // sample number of each audio frame
	int m_fft_len;
	int m_fft_len_div_2;
	int m_overlap_len;
	int m_channelN;
	int m_frame_cnt;
	float m_max_sup_db;
	int m_num_band;
	int m_sample_rate;
	int m_fft_idx_low;
	int m_fft_idx_high;
	int m_init_frame_num;
	ResGlobalData m_ans_param;

	float *m_sin_win;
	float *m_cos_win;
	R *m_buf_real;
	C *m_buf_spec;
	R **m_buf_real_ch;
	C **m_buf_spec_ch;
	float *m_in_buf;
	float **m_in_buf_ch;
	float *m_buf_pow;
	float **m_buf_pow_ch;
	float *m_buf_pow_prev;
	float *m_buf_pow_local;
	float *m_noise_var;
	float *m_speech_var;
	float *m_priori_average;
	float *m_post_snr;
	float *m_speech_absence_possibility;
	float *m_bin_gain;
	float *m_bin_gain_smooth;
	float **m_bin_gain_ch;
	float **m_bin_gain_smooth_ch;
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
	int *ch_tbl;
	float *m_in;
	float *m_out;
	float *m_ch_enrg_long_db;
	float *m_ch_enrg_db;
	float *m_ch_noise_db;
	float *m_ch_en_dev;
	float *m_ch_noise_dev;
	float m_enrg_dev_weight;
	int m_noise_update_cnt;
	int m_noise_update_flag;
	float m_spectralFlatness;
	float *m_overdrive;
	bool m_init_stable;

	static void *thread_process(void *args);
	void thread_process_do(int index);

#if ANS_DEBUG
	float *m_out_buf;
	float *m_bin_gain_bgn;
#endif /*ANS_DEBUG*/
};

#endif
