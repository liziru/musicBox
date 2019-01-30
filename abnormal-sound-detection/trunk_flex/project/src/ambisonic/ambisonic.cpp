#include "ambisonic.h"
#include "ambisonicData.h"
#include <string.h>
#include <math.h>
#include "../utils/cond.h"
#include "hxvxfromxxxx/h1v1fromobject.h"
#include "hxvxfromxxxx/h1v1fromwyzx.h"

#define outChannels (4)
#define FFT_LEN (1024)

CAmbisonic::CAmbisonic(int profile_id, int frame_length, int object_num, int sample_rate, bool multithread /*= true*/) 
	: m_ready_conds(NULL)
	, m_done_conds(NULL)
	, m_prep_conds(NULL)
	, m_threads(NULL)
	, m_multithread(multithread)
	, m_hxvx_from_xxxx(NULL)
	, m_opposite(NULL)
	, m_thread_break(false)
{
    const float *hrir, *dec, *decog;
	
	switch (profile_id) {
	case 1: {
		m_fft_len = FFT_LEN;
		m_filter_len = HRIR_LEN;
		m_wxyN = wxyN_pf0;
		m_spkN = speakerNum_pf0;
		hrir = HrirCoeff_3D;
		dec = *decH1V1_3D;
		decog = decOrderGainH1V1_3D;
		m_hxvx_from_xxxx = new CH1V1FromWYZX(frame_length, object_num);
		break;
	}
	case 20: {
		m_fft_len = FFT_LEN;
		m_filter_len = HRIR_LEN;
        m_wxyN = wxyN_pf0;
		m_spkN = speakerNum_pf0;
		hrir = HrirCoeff_3D;
		dec = *decH1V1_3D;
		decog = decOrderGainH1V1_3D;
		m_hxvx_from_xxxx = new CH1V1FromObject(frame_length, object_num);
		break;
	}

	default: {
		throw;
	}
	}

	m_fft_len_div_2 = m_fft_len >> 1;
	m_frame_len = frame_length;	
	m_overlap_len = m_fft_len - m_frame_len;
	m_ambisonic_buf = new float[m_wxyN * m_fft_len];
	memset(m_ambisonic_buf, 0x00, sizeof(float) * m_fft_len * m_wxyN);

	m_object_num = object_num;
	m_sample_rate = sample_rate;

	uint logN = (uint)(log((float)m_fft_len) / log(2.0f) + 0.5);
	m_fft_scale = 1.0 / m_fft_len;
	m_hrir_spec = new C[m_wxyN * (m_fft_len_div_2 + 1)];
	

	if (m_multithread) {
		m_ready_conds = new CCond[m_wxyN];
		m_done_conds = new CCond[m_wxyN];
		m_prep_conds = new CCond[m_wxyN];
		m_threads = new CThread[m_wxyN];
		m_thread_break = false;
		for (int i = 0; i < m_wxyN; i++) {
			m_prep_conds[i].lock();
			ThreadArgs args;
			args.index = i;
			args.user = this;
			m_threads[i].create(CAmbisonic::thread_process, &args);
			m_prep_conds[i].wait();
			m_prep_conds[i].unlock();
		}
	}

	int fft_num = m_multithread ? m_wxyN : 1;
	m_real_fft = new CRealFFT*[fft_num];
	for (int i = 0; i < fft_num; i++) {
		m_real_fft[i] = new CRealFFT(logN);  //andy: real fft
	}
	m_fft_real = new R[fft_num * m_fft_len];
	m_fft_spec = new C[fft_num * (m_fft_len_div_2 + 1)];

	float *hrir_real = new R[m_wxyN * m_filter_len];
	binaural_decoder_filter(hrir, hrir_real, dec, decog);
	rirs_fft(hrir_real);
	delete [] hrir_real;
	hrir_real = NULL;
}

CAmbisonic::~CAmbisonic()
{
	if (m_multithread) {
		m_thread_break = true;
		for (int i = 0; i < m_wxyN; i++) {
			m_ready_conds[i].signal_s();
			m_threads[i].join();
		}
	}
	if (NULL != m_threads) { delete [] m_threads; m_threads = NULL; }
	if (NULL != m_ready_conds) { delete [] m_ready_conds; m_ready_conds = NULL; }
	if (NULL != m_done_conds) { delete [] m_done_conds; m_done_conds = NULL; }
	if (NULL != m_prep_conds) { delete [] m_prep_conds; m_prep_conds = NULL; }

	if (NULL != m_ambisonic_buf) { delete [] m_ambisonic_buf; m_ambisonic_buf = NULL; }
	if (NULL != m_fft_spec) { delete [] m_fft_spec; m_fft_spec = NULL; }
	if (NULL != m_fft_real) { delete [] m_fft_real; m_fft_real = NULL; }
	if (NULL != m_hrir_spec) { delete [] m_hrir_spec; m_hrir_spec = NULL; }

	int fft_num = m_multithread ? m_wxyN : 1;
	for (int i = 0; i < fft_num; i++) {
		if (NULL != m_real_fft[i]) { delete m_real_fft[i]; m_real_fft[i] = NULL; }
	}
	if (NULL != m_real_fft) { delete [] m_real_fft; m_real_fft = NULL; }
}

// Update is called once every frame
void CAmbisonic::process(const HeadRotation *rot, const float *in, float *out, const MetaData *meta)
{
	m_hxvx_from_xxxx->prev(rot, in, m_ambisonic_buf, m_frame_len * sizeof(float), meta);

	memset(out, 0x00, (outChannels * m_frame_len * sizeof(float)));

	if (m_multithread) {
		for (int i = 0; i < m_wxyN; i++) {
			m_done_conds[i].lock();
		}
		m_obj_in = in;
		for (int i = 0; i < m_wxyN; i++) {
			m_ready_conds[i].signal_s();
		}
		for (int i = 0; i < m_wxyN; i++) {
			m_done_conds[i].wait();
			m_done_conds[i].unlock();
		}
		for (int j = 0; j < m_wxyN; j++) {
			update_out(out, m_ambisonic_buf, j, false);
		}
	}
	else {
		for (int j = 0; j < m_wxyN; j++) {
			a_to_b(in, j);
			update_out(out, m_ambisonic_buf, j, false);

		}
	}
	m_hxvx_from_xxxx->post();
}

//compute binaural decoder filter
void CAmbisonic::binaural_decoder_filter(const float *in, float *out, const float *dec, const float *decog)
{
	for (int i = 0; i < m_wxyN; i++) {
		float *out_iter = out + i * m_filter_len;
		for (int j = 0; j < m_filter_len; j ++) {
			float sum = 0.f;
			for (int k = 0; k < m_spkN; k++) {
				sum = sum + in[j + m_filter_len * k] * dec[k * m_wxyN + i] * decog[i];
			}
			*out_iter = sum;
			out_iter++;
		}
	}
}

// fft for hrir
void CAmbisonic::rirs_fft(const float *in)
{
	R *fft_r = new R[m_fft_len];
	/* Shane: left padding zeros. */
	memset(fft_r, 0x00, sizeof(R) * m_fft_len);
	for (int c = 0; c < m_wxyN; c++) {
		memcpy(fft_r + (m_fft_len - m_filter_len), in + c * m_filter_len, m_filter_len * sizeof(float));
		m_real_fft[0]->run_fwd_fft(fft_r, m_hrir_spec + c * (m_fft_len_div_2 + 1));
	}
	delete [] fft_r;
	fft_r = NULL;
}

void *CAmbisonic::thread_process(void *args)
{
	CAmbisonic::ThreadArgs* user = (CAmbisonic::ThreadArgs*)args;
	user->user->thread_process_do(user->index);
	return NULL;
}

void CAmbisonic::thread_process_do(int index)
{
	m_ready_conds[index].lock();
	m_prep_conds[index].signal_s();
	for (;;) {
		m_ready_conds[index].wait();
		/* This thread cancel point. */
		if (m_thread_break) {
			break;
		}
		a_to_b(m_obj_in, index);
		m_done_conds[index].signal_s();
	}
	m_ready_conds[index].unlock();
}



void CAmbisonic::a_to_b(const float *in, int index)
{

	m_hxvx_from_xxxx->run(index);

}

void CAmbisonic::update_out(float *out, const float *up, int c, bool opposite)
{
	int co;
	// fuma to sn3d order.
	switch (c)  
	{
	case 0:
		co = 0;
		break;
	case 1:
		co = 3;
		break;
	case 2:
		co = 1;
		break;
	case 3:
		co = 2;
		break;
	default:
		throw;
		break;
	}

	float *out_iter = out + co;
	const float *up_iter = up + c*m_frame_len;


	for (int i = 0; i < m_frame_len; i++) {
		*out_iter += *up_iter;
		out_iter += outChannels; up_iter++;
	}

}