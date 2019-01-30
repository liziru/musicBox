#ifndef RES_AEC_H
#define RES_AEC_H

#include "aecDefine.h"
#include "resDefine.h"
#include "../fft/realFFT.h"

class CRes
{
public:

	CRes(AecGlobalData* pAecGlobal);
	~CRes();

	void resInit(AecGlobalData* pAecGlobal);
	void resSet(AecGlobalData* pAecGlobal);
	int resProcess(float *eBuf,  AecGlobalData* pAecGlobal);
	void resRelease();
	
private:
	void noiseSpectrumEstimator();
	void vocalSpectrumEstimator();
	void snrEstimator();
	void gainEstimator(AecGlobalData* pAecGlobal);
	void overlapPlus(AecGlobalData* pAecGlobal);

	ResGlobalData *pResGlobal;
	//---------512 fft ---------------------
	int m_frame_len;
	int m_filter_len;
	int m_fft_len;
	int m_spectrum_len;
	int m_res_fft_idx_low;
	int m_res_fft_idx_high;
	R *m_in_real;
	C *m_in_spec;
	R *m_d_real;
	C *m_d_spec;
	R *m_y_real;
	C *m_y_spec;
	R *m_out_real;
	C *m_out_spec;
	CRealFFT *m_real_fft;
	float m_fft_scale;
	//
};

#endif