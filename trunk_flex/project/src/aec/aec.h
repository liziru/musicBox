#ifndef AEC_INST_H
#define AEC_INST_H
#include "aecDefine.h"
#include "res.h"

class CAec
{
public:

	CAec(int frameLength, int filterLength, int sampleRate, int micN, int spkN, int sysDelay);
    ~CAec();

    void process(const float *spkSig, float *micSig);
	void set(bool enableRes, float resLevel, bool enableNS, float nsDB, bool enableSpkClip, float spkClipThd, float maxCoupling, float adaptStep);
	
private:
	int  aecInit();
	int speakerVad(float *buf_spk, int bsize);
	void dtdInit();
	int dtdProcess();

	void getErrorSpectrum();
	void refreshFilter(int c);
	void adaptiveFilter(int c);
	void spkClipCompensation(float * buf_spk, float Thd, int bsize);
	void fdnlms(float *dBuf, const float *xBuf);

	int m_mic_num;
	int m_spk_num;
	int m_frame_len;               // sample number of each audio frame
	int m_filter_len;
	int m_spectrum_len;
	int m_fft_len;
	int m_dt_fft_idx_low;
	int m_dt_fft_idx_high;

	int m_filter_len2;
	int m_spectrum_len2;	
	int m_fft_len2;

	int max_filter_delay;
	int min_filter_delay;
	int filter_delay;
	int system_delay_buf_size;

	CRes *resInst;
	AecGlobalData *pAecGlobal;

	//------------fft for aec--------------
	R *m_ebuf_real;
	C *m_ebuf_spec;
	R *m_xe_real;
	C *m_xe_spec;
	R *m_x_real;
	C *m_x_spec;
	R *m_y_real;
	C *m_y_spec;
	CRealFFT *m_real_fft;
	float m_fft_scale;

	//---------fft for dtd and res---------------------
	R *m_dw_real;
	C *m_dw_spec;
	R *m_ew_real;
	C *m_ew_spec;
	R *m_yw_real;
	C *m_yw_spec;
	CRealFFT *m_real_fft_dtd;
	float m_fft_scale_dtd;

};

#endif