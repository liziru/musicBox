#ifndef AMBISONIC_H
#define AMBISONIC_H

#include "../fft/realFFT.h"
#include "../core/def.h"

class CThread;
class CCond;
class CHXVXFromXXXX;

class CAmbisonic
{
public:

    CAmbisonic(int profile_id,int frame_length, int object_num, int sample_rate, bool multithread = true);
    ~CAmbisonic();

    void process(const HeadRotation *rot, const float *in, float *out, const MetaData *meta);

private:

	struct ThreadArgs_s
	{
		int index;
		CAmbisonic *user;
	};
	typedef ThreadArgs_s ThreadArgs;

    CRealFFT **m_real_fft;

	int m_filter_len;			// HRIR filter length
    int m_fft_len;				// FFT points, must be power of 2
    int m_fft_len_div_2;
	int m_overlap_len;		    // overlap length for overlap_add type of convolution
	R m_fft_scale;

	int m_sample_rate;
	int m_frame_len;               // sample number of each audio frame
	int m_object_num;
    int m_wxyN;
	int m_spkN;

	R *m_ambisonic_buf;
	R *m_fft_real;
	C *m_fft_spec;
	C *m_hrir_spec; // -- x ((fft lenght) / 2 + 1), spectrum of hrir.
	bool m_multithread;
	bool *m_opposite;
	CCond *m_ready_conds;
	CCond *m_done_conds;
	CCond *m_prep_conds;
	CThread *m_threads;
	bool m_thread_break;
	const float *m_obj_in;
	CHXVXFromXXXX *m_hxvx_from_xxxx;

	bool opposite(int profile_id, int index);
	void a_to_b(const float *in, int index);
	void update_out(float *out, const float *up, int c, bool opposite);
    void binaural_decoder_filter(const float *in, float *out, const float *dec, const float *decog);
    void rirs_fft(const float *in);
	static void *thread_process(void *args);
	void thread_process_do(int index);
};

#endif