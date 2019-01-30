#ifndef BEAMFORMING_H
#define BEAMFORMING_H

#define BEAMFORMING_DEBUG (0)

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include "../fft/realFFT.h"
#include "inverse.h"


#define M (72)
#define FFT_LEN_16k   (512)
#define FFT_LEN   (1024)
#define LWORK (100)

class CThread;
class CCond;

class CBeamforming
{
public:
	// profileID:
	// 0: a format
	// 1: line mic array.
	CBeamforming(int profileID, int frame_length, int channelN, int sampleRate, float micDist, bool multithread);
	~CBeamforming();

	void process(const float *in, float *out);
	void set(int mode, float max_sup_db, float targetDirection, float targetEstimation, float nlp_sup, float tolerance, int targetFlag);

private:
	void gain_estimator();
	void snr_estimator();
	void vocal_spectrum_estimator();
	void noise_spectrum_estimator();
	void prepare(const float *in, float *out, int c);
	void overlap_plus_pre(int c);
	void gain_estimator_ch(int c);
	void gain_estimator_ch_opt(int c);
	void overlap_plus_post(float *out, int c);
	void prebakeSteerVector();
	void adaptiveFilter(float *targetSig, float *nullSig);
	void delay_sum(const float*in);
	void post_filter();
    void mvdr_c();
#if 0
	float getA(float **arcs, int n);
	void getAStart(float **arcs, int n, float **ans);
	void matricAdd(float **a, float **b, float **c, int n);
	void matricMul(float **a, float **b, float **c, int n);
	void getMatrixInverse(float *a, float *b, int n);
#endif
    void inverse_bin_c();

private:
	typedef struct {
		float  speechAbsencePossibilityAvg;
		float  avgsnrglobal;
		float  noiseTrackStep;
	} ResGlobalData;

	struct ThreadArgs_s
	{
		int index;
		CBeamforming *user;
	};
	typedef ThreadArgs_s ThreadArgs;

	int m_profileID;
	int m_frame_len;               // sample number of each audio frame
	int m_sampleRate;
	int m_fft_len;
	int m_fft_len_div_2;
	int m_overlap_len;
	int m_channelN_in;
	int m_channelN;
	int m_frame_cnt;
	float m_max_sup_db;
	int m_mode;
	float m_nlp_sup;
	float m_nlp_factor;
	float m_nlp_gain;
	int m_num_band;
	int m_target_delay_len;
	
	float m_targetDirection;
	float m_nullDirection;
	int m_targetDirectionIndex;
	int m_nullDirectionIndex;
	float m_targetEstimation;
	int m_targetFlag;
	int m_targetFlag_prev;

	ResGlobalData m_ans_param;

	float *m_sin_win;
	float *m_cos_win;
	float *m_hanning_win;
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
	float *m_re_average;
	float *m_speech_absence_possibility;
	float *m_bin_gain;
	float *m_bin_gain_smooth;
	float **m_bin_gain_ch;
	float **m_bin_gain_smooth_ch;
	CRealFFT **m_real_fft;
	CRealFFT *m_real_fft_delay;
	float m_fft_scale;
	bool m_multithread;
	CCond *m_ready_conds;
	CCond *m_done_conds;
	CCond *m_prep_conds;
	CThread *m_threads;
	bool m_thread_break;
	const float *m_obj_in;
	float *m_obj_out;
	float *m_targetSig;
	float *m_targetSig_delay;
	float *m_nullSig;
	float *m_detectEng;
	R **m_anaBuf_ch;
	R **m_delayBuf_ch;
	R **m_delayBuf_out_ch;
	C **m_delayBuf_spec_ch;
	C **m_delaySumBuf_spec_ch;
	R *m_postFilter_nom;
	R *m_postFilter_den;
	R *m_postFilter_nom_avg;
	R *m_postFilter_den_avg;
	R *m_postFilter_gain;
	float *m_in;
	float **m_mem;
	float **m_out_ch;
	C *m_sigma_c;
	C **m_sigma_sm_c;
	C *m_matric_c;
	C *m_tmpv;


	static void *thread_process(void *args);
	void thread_process_do(int index);

	//beamforming
	float **m_steerVectors;
	float **m_superDirective;
	C **m_delayVectors_c;
	C **m_superVectors_c;
	C **m_mvdrVectors_c;
	float m_steerIndex;
	float *m_StableMatrix;
	float m_micDist;
	bool m_first;
	complex a_c[LWORK];
	complex m_work_c[LWORK];
	C *m_nom_c;
#if 0
	float *m_matric_real;
	float *m_matric_image;
#endif
#if ANS_DEBUG
	float *m_out_buf;
	float *m_bin_gain_bgn;
#endif /*ANS_DEBUG*/
};

#endif