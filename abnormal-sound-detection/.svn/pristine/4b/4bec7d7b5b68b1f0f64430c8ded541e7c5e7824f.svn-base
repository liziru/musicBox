#include <string.h>
#include <math.h>
#include "aec.h"
#include "stdio.h"


#ifndef PI
#define PI       3.14159265358979323846
#endif

#define maxv(a,b) ((a>b)? (a):(b))
#define minv(a,b) ((a<b)? (a):(b))
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

#ifdef ENABLE_DEBUG
/* clip to [-32768, 32767] or [ -pow(2,15), pow(2,15)-1] */
short saturation16(int x)
{
	x = maxv(x, -32768);
	x = minv(x, 32767);

	return (short)x;
}
#endif

#ifdef ENABLE_DEBUG
#include <stdio.h>
FILE* pfile1;
FILE* pfile2;
FILE* pfile3;
FILE* pfile4;
#endif


CAec::CAec(int frameLength, int filterLength, int sampleRate, int micN, int spkN, int sysDelay)
{

	m_mic_num = micN;
	m_spk_num = spkN;
	m_frame_len = frameLength;
	m_filter_len = filterLength;
	m_fft_len = m_filter_len*2;
	m_spectrum_len = m_filter_len + 1;
	
	if (m_frame_len <= 256)
		m_filter_len2 = 256;
	else
		m_filter_len2 = 512;
	m_fft_len2 = m_filter_len2 * 2;
	m_spectrum_len2 = m_filter_len2 + 1;
	m_dt_fft_idx_low = (int)((float)m_fft_len2 * 200 / sampleRate);
	m_dt_fft_idx_high = (int)((float)m_fft_len2 * 4000 / sampleRate);
	if (m_dt_fft_idx_high >= m_spectrum_len2)
	{
		m_dt_fft_idx_high = m_spectrum_len2 - 1;
	}

	max_filter_delay = m_filter_len - m_frame_len;
	min_filter_delay = 0;
	filter_delay = max_filter_delay;  // Set extra delay caused by filtering.
	system_delay_buf_size = sysDelay + m_frame_len;

	pAecGlobal = new AecGlobalData;

	memset(pAecGlobal, 0, sizeof(AecGlobalData));

	pAecGlobal->framelens = m_frame_len;
	pAecGlobal->fftlens_res = m_fft_len2;
	pAecGlobal->samplerate = sampleRate;
	pAecGlobal->sysDelay = sysDelay;
	aecInit();

}

CAec::~CAec()
{
	RELEASE_POINTER(resInst);
	RELEASE_POINTER(m_real_fft);
	RELEASE_ARRAY(pAecGlobal->dBuf);
	RELEASE_ARRAY(pAecGlobal->xBuf);
	RELEASE_ARRAY(pAecGlobal->yBuf);
	RELEASE_ARRAY(pAecGlobal->eBuf);
	RELEASE_ARRAY(pAecGlobal->xSpec);
	RELEASE_ARRAY(pAecGlobal->xfBufPow);
	RELEASE_ARRAY(pAecGlobal->yfBufRe);
	RELEASE_ARRAY(pAecGlobal->yfBufIm);
	RELEASE_ARRAY(pAecGlobal->filterRe);
	RELEASE_ARRAY(pAecGlobal->filterIm);
	RELEASE_ARRAY(pAecGlobal->spkCircleBuf);
	RELEASE_ARRAY(pAecGlobal->spkBuf);
	RELEASE_ARRAY(pAecGlobal->spkRpt);
	RELEASE_ARRAY(pAecGlobal->spkWpt);
	RELEASE_POINTER(pAecGlobal->resGlobal);

	RELEASE_ARRAY(pAecGlobal->dwfPow);
	RELEASE_ARRAY(pAecGlobal->ywfPow);
	RELEASE_ARRAY(pAecGlobal->ewfPow);
	RELEASE_ARRAY(pAecGlobal->dwf);
	RELEASE_POINTER(pAecGlobal->ywf);

	RELEASE_POINTER(pAecGlobal);	
	RELEASE_ARRAY(m_ebuf_real);
	RELEASE_ARRAY(m_ebuf_spec);
	RELEASE_ARRAY(m_xe_real);
	RELEASE_ARRAY(m_xe_spec);
	RELEASE_ARRAY(m_x_real);
	RELEASE_ARRAY(m_x_spec);
	RELEASE_ARRAY(m_y_real);
	RELEASE_ARRAY(m_y_spec);

	RELEASE_POINTER(m_real_fft_dtd);
	RELEASE_ARRAY(m_dw_real);
	RELEASE_ARRAY(m_dw_spec);
	RELEASE_ARRAY(m_ew_real);
	RELEASE_ARRAY(m_ew_spec);
	RELEASE_ARRAY(m_yw_real);
	RELEASE_ARRAY(m_yw_spec);
	

#ifdef ENABLE_DEBUG
	fclose(pfile1);
	fclose(pfile2);
	fclose(pfile3);
	fclose(pfile4);
#endif
	
}


//-------------------------------------------------------------------
//Initial buffer and pt
//-------------------------------------------------------------------
int  CAec::aecInit()
{
	m_ebuf_real = new R[m_fft_len];
	m_ebuf_spec = new C[m_fft_len / 2 + 1];
	m_xe_real = new R[m_fft_len];
	m_xe_spec = new C[m_fft_len / 2 + 1];
	m_x_real = new R[m_fft_len];
	m_x_spec = new C[m_fft_len / 2 + 1];
	m_y_real = new R[m_fft_len];
	m_y_spec = new C[m_fft_len / 2 + 1];
	m_fft_scale = 1.f / (float)m_fft_len;
	uint logN = (uint)(log((float)m_fft_len) / log(2.0f) + 0.5);
	m_real_fft = new CRealFFT(logN);


	//512
	m_dw_real = new R[m_filter_len2 * 2];
	m_dw_spec = new C[m_filter_len2 + 1];
	m_ew_real = new R[m_filter_len2 * 2];
	m_ew_spec = new C[m_filter_len2 + 1];
	m_yw_real = new R[m_filter_len2 * 2];
	m_yw_spec = new C[m_filter_len2 + 1];
	m_fft_scale_dtd = 1.f / (float)(m_filter_len2 * 2);
	uint logN2 = (uint)(log((float)(m_filter_len2 * 2)) / log(2.0f) + 0.5);
	m_real_fft_dtd = new CRealFFT(logN2);

	pAecGlobal->dBuf         = new float[m_filter_len];
	pAecGlobal->xBuf         = new float[m_fft_len*m_spk_num];
	pAecGlobal->yBuf		 = new float[m_fft_len*m_spk_num];
	pAecGlobal->eBuf         = new float[m_filter_len];
	pAecGlobal->xSpec        = new C[m_spectrum_len*m_spk_num];
	pAecGlobal->xfBufPow     = new float[m_spectrum_len*m_spk_num];
	pAecGlobal->yfBufRe      = new float[m_fft_len];
	pAecGlobal->yfBufIm      = new float[m_fft_len];
	pAecGlobal->filterRe     = new float[m_spectrum_len*m_spk_num];
	pAecGlobal->filterIm     = new float[m_spectrum_len*m_spk_num];
	pAecGlobal->spkCircleBuf = new float[system_delay_buf_size*m_spk_num];
	pAecGlobal->spkBuf       = new float[m_frame_len*m_spk_num];

	memset(pAecGlobal->dBuf, 0, m_filter_len*sizeof(float));
	memset(pAecGlobal->xBuf, 0, m_fft_len*m_spk_num*sizeof(float));
	memset(pAecGlobal->yBuf, 0, m_fft_len*m_spk_num*sizeof(float));
	memset(pAecGlobal->eBuf, 0, m_filter_len*sizeof(float));
	memset(pAecGlobal->xSpec, 0, m_spectrum_len*m_spk_num*sizeof(C));
	memset(pAecGlobal->xfBufPow, 0, m_spectrum_len*m_spk_num*sizeof(float));
	memset(pAecGlobal->yfBufRe, 0, m_fft_len*sizeof(float));
	memset(pAecGlobal->yfBufIm, 0, m_fft_len*sizeof(float));
	memset(pAecGlobal->filterRe, 0, m_spectrum_len*m_spk_num*sizeof(float));
	memset(pAecGlobal->filterIm, 0, m_spectrum_len*m_spk_num*sizeof(float));
	memset(pAecGlobal->spkCircleBuf, 0, system_delay_buf_size*m_spk_num*sizeof(float));

	memset(m_dw_real, 0, m_fft_len2*sizeof(float));
	memset(m_ew_real, 0, m_fft_len2*sizeof(float));
	memset(m_yw_real, 0, m_fft_len2*sizeof(float));

	pAecGlobal->dwfPow = new float[m_filter_len2 + 1];
	pAecGlobal->ywfPow = new float[m_filter_len2 + 1];
	pAecGlobal->ewfPow = new float[m_filter_len2 + 1];
	pAecGlobal->dwf    = new float[m_frame_len];
	pAecGlobal->ywf    = new float[m_frame_len];
	


	pAecGlobal->spkRpt = new int[m_spk_num];
	pAecGlobal->spkWpt = new int[m_spk_num];
	for (int i = 0; i < m_spk_num; i++)
	{
		pAecGlobal->spkRpt[i] = 0;
		pAecGlobal->spkWpt[i] = pAecGlobal->sysDelay;
	}

	pAecGlobal->frameCnt = 0;
	pAecGlobal->stepAdj = 1.0;

	pAecGlobal->vadFlag = 0;
	pAecGlobal->dtdFlag = 0;
	pAecGlobal->convergeFlag = 0;
	pAecGlobal->convergeCount = 0;
	pAecGlobal->convergeTimer = 0;
	pAecGlobal->firstFlag = 1;

	pAecGlobal->enableRes = true;
	pAecGlobal->resLevel = 1.0;
	pAecGlobal->enableNS = true;
	pAecGlobal->nsDB = -10;
	pAecGlobal->enableSpkClip = false;
	pAecGlobal->spkClipThd = 1.0;
	pAecGlobal->maxCoupling = 10.0;
	pAecGlobal->adaptStep = 0.5;

#ifdef ENABLE_DEBUG
	pfile1 = fopen("ref.pcm", "wb");
	pfile2 = fopen("e.pcm", "wb");
	pfile3 = fopen("mic.pcm", "wb");
	pfile4 = fopen("dtd.pcm", "wb");
#endif
	pAecGlobal->resGlobal = new ResGlobalData;
	memset(pAecGlobal->resGlobal, 0, sizeof(ResGlobalData));

	resInst = new CRes(pAecGlobal);
	resInst->resSet(pAecGlobal);
	dtdInit();

	return 1;
}

void CAec::set(bool enableRes, float resLevel, bool enableNS, float nsDB, bool enableSpkClip, float spkClipThd, float maxCoupling, float adaptStep)
{		
	pAecGlobal->enableRes = enableRes;
	pAecGlobal->resLevel = resLevel;
	pAecGlobal->enableNS = enableNS;
	pAecGlobal->nsDB = nsDB;
	pAecGlobal->enableSpkClip = enableSpkClip;
	pAecGlobal->spkClipThd = spkClipThd;
	pAecGlobal->maxCoupling = maxCoupling;
	pAecGlobal->adaptStep = adaptStep;

	resInst->resSet(pAecGlobal);
}

// Update is called once every frame
void CAec::process(const float *spkSig, float *micSig)
{
	// generate next frame of audio
	pAecGlobal->frameCnt++;
	fdnlms(micSig, spkSig);
	if (pAecGlobal->enableRes)  //enable NLP + comfort noise
		resInst->resProcess(micSig, pAecGlobal);

	pAecGlobal->firstFlag = 0;
}

// temporarily use a simple VAD
int CAec::speakerVad(float *buf_spk, int bsize)
{
	int i;
	float sum1 = 0;
	for (i = 0; i < bsize; i++)
	{
		sum1 += fabs((float)buf_spk[i]);
	}
	sum1 /= bsize;
	if (sum1 >= M50dB_PCM)
		return TURN_ON;
	else
		return TURN_OFF;
}

void CAec::dtdInit()
{
	DtdGlobalData *pDtdGlobal = &pAecGlobal->dtdGlobal;
	pDtdGlobal->efPow = 0;
	pDtdGlobal->yfPow = 0;
	pDtdGlobal->dfPow = 0;
}

int CAec::dtdProcess()
{
	DtdGlobalData *pDtdGlobal = &pAecGlobal->dtdGlobal;
	int i;
	float dPow, yPow, ePow;
	dPow = yPow = ePow = 0;

	for (i = 0; i < m_frame_len; i++)
	{
		pAecGlobal->dwf[i] = pAecGlobal->dBuf[i];
		pAecGlobal->ywf[i] = m_y_real[i + m_filter_len];
	}

	for (i = 0; i < m_frame_len; i++)
	{
		m_ew_real[i + (m_fft_len2 - m_frame_len)] = pAecGlobal->eBuf[i];
		m_dw_real[i + (m_fft_len2 - m_frame_len)] = pAecGlobal->dwf[i];
		m_yw_real[i + (m_fft_len2 - m_frame_len)] = pAecGlobal->ywf[i];
	}
	
	m_real_fft_dtd->run_fwd_fft(m_dw_real, m_dw_spec);
	m_real_fft_dtd->run_fwd_fft(m_yw_real, m_yw_spec);
	m_real_fft_dtd->run_fwd_fft(m_ew_real, m_ew_spec);

	for (i = 0; i <= m_filter_len2; i++)
	{
		pAecGlobal->dwfPow[i] = m_dw_spec[i].re * m_dw_spec[i].re + m_dw_spec[i].im *m_dw_spec[i].im;
		pAecGlobal->ywfPow[i] = m_yw_spec[i].re * m_yw_spec[i].re + m_yw_spec[i].im *m_yw_spec[i].im;
		pAecGlobal->ewfPow[i] = m_ew_spec[i].re * m_ew_spec[i].re + m_ew_spec[i].im *m_ew_spec[i].im;
	}

	for (i = 0; i < (m_fft_len2 - m_frame_len); i++)
	{
		m_ew_real[i] = m_ew_real[i + m_frame_len];
		m_dw_real[i] = m_dw_real[i + m_frame_len];
		m_yw_real[i] = m_yw_real[i + m_frame_len];
	}



	for (i = m_dt_fft_idx_low; i < m_dt_fft_idx_high; i++)
	{
		dPow += pAecGlobal->dwfPow[i];
		yPow += pAecGlobal->ywfPow[i];
		ePow += pAecGlobal->ewfPow[i];
	}

	pAecGlobal->stepAdj = STEP1;

	pAecGlobal->vadFlag = speakerVad(pAecGlobal->xBuf, m_fft_len*m_spk_num);
	//check if filter is converged
	if (pAecGlobal->convergeFlag == TURN_OFF  && pAecGlobal->vadFlag == TURN_ON)
	{
		if (dPow * DT_CONVERGE_THD > ePow)
		{
			pAecGlobal->convergeCount++;
		}
		else
		{
			pAecGlobal->convergeCount = 0;
		}

		if (pAecGlobal->convergeCount > DT_CONVERGE_PRD)
		{
			pAecGlobal->convergeFlag = TURN_ON;
		}

		if (pAecGlobal->convergeTimer <= DT_CONVERGE_PRD_MAX)
		{
			pAecGlobal->convergeTimer++;
		}
	}

	if (ePow > pDtdGlobal->efPow)    // fast increase
	{
		pDtdGlobal->efPow = pDtdGlobal->efPow * 0.8f + ePow * 0.2f;
	}
	else                                     // slow decay 
	{
		pDtdGlobal->efPow = pDtdGlobal->efPow * 0.95f + ePow * 0.05f;
	}

	if (yPow > pDtdGlobal->yfPow)
	{
		pDtdGlobal->yfPow = pDtdGlobal->yfPow * 0.8f + yPow * 0.2f;
	}
	else
	{
		pDtdGlobal->yfPow = pDtdGlobal->yfPow * 0.95f + yPow * 0.05f;
	}

	if (dPow > pDtdGlobal->dfPow)
	{
		pDtdGlobal->dfPow = pDtdGlobal->dfPow * 0.8f + dPow * 0.2f;
	}
	else
	{
		pDtdGlobal->dfPow = pDtdGlobal->dfPow * 0.95f + dPow * 0.05f;
	}

	if (pAecGlobal->convergeFlag == TURN_ON)
	{

		if (pDtdGlobal->dfPow * DT_RATIO_H > pDtdGlobal->yfPow)
		{
			pAecGlobal->dtdFlag = TURN_ON;
			pAecGlobal->stepAdj = STEP4;
		}
		else if (pDtdGlobal->dfPow * DT_RATIO > pDtdGlobal->yfPow)
		{
			pAecGlobal->dtdFlag = TURN_ON;
			pAecGlobal->stepAdj = STEP3;
		}
		else
		{
			pAecGlobal->dtdFlag = TURN_OFF;
			pAecGlobal->stepAdj = STEP2;
		}
	}
	else
	{
		if (pAecGlobal->convergeTimer < DT_CONVERGE_PRD_MAX)
			pAecGlobal->stepAdj = STEP1;
		else
			pAecGlobal->stepAdj = STEP2;
	}

	return pAecGlobal->dtdFlag;
}

//---------------------------------------------------------------------------------
// Convert e from time domain to frequency domain
//---------------------------------------------------------------------------------
void CAec::getErrorSpectrum()
{
	int i,c;

	// add all filter outputs;
	memset(m_y_real, 0, m_fft_len*sizeof(float));
	for (i = 0; i < m_fft_len; i++)
	{
		for (c = 0; c < m_spk_num; c++)
		{
			m_y_real[i] += pAecGlobal->yBuf[c*m_fft_len+i];
		}
	}

	for (i = 0; i<m_filter_len; i++)
	{
		pAecGlobal->eBuf[i] = pAecGlobal->dBuf[i] - m_y_real[i + m_filter_len];
	}

	memset(m_ebuf_real, 0, sizeof(R)*m_fft_len);
	for (i = 0; i < m_filter_len; i++)
	{
		m_ebuf_real[i + m_filter_len] = pAecGlobal->eBuf[i];
	}
	m_real_fft->run_fwd_fft(m_ebuf_real, m_ebuf_spec);

#ifdef ENABLE_DEBUG
	short x[m_frame_len];
	for (i = 0; i<m_frame_len; i++)
	{
		x[i] = saturation16((int)(m_y_real[i + m_filter_len]*32767));
	}

	fwrite(x, sizeof(short), m_frame_len, pfile1);

	for (i = 0; i<m_frame_len; i++)
	{
		x[i] = saturation16((int)(pAecGlobal->eBuf[i]*32767));
	}

	fwrite(x, sizeof(short), m_frame_len, pfile2);

	for (i = 0; i<m_frame_len; i++)
	{
		x[i] = saturation16((int)(pAecGlobal->dBuf[i]*32767));
	}

	fwrite(x, sizeof(short), m_frame_len, pfile3);

#endif

}

//refresh adaptive filter.
void CAec::refreshFilter(int c)
{

	int i;
	float mymu = pAecGlobal->adaptStep*pAecGlobal->stepAdj; //stepsize is adjusted by dtd result
	
	int overflowflag = 0;
	float pwtmp;

	float *filterR = &pAecGlobal->filterRe[c*m_spectrum_len];
	float *filterI = &pAecGlobal->filterIm[c*m_spectrum_len];
	float *xPow = &pAecGlobal->xfBufPow[c*m_spectrum_len];
	memcpy(m_x_spec, &pAecGlobal->xSpec[c*m_spectrum_len], m_spectrum_len*sizeof(C));
	//calculate farend signal power for normalize.
	for (i = 0; i <m_spectrum_len; i++)
	{
		pwtmp = m_x_spec[i].re*m_x_spec[i].re + m_x_spec[i].im * m_x_spec[i].im;
		if (pwtmp>xPow[i])
			xPow[i] = xPow[i] * (1 - ALPHA) + pwtmp*ALPHA;
		else
			xPow[i] = xPow[i] * (1 - BETA) + pwtmp*BETA;
	}

	//calculate correlation between farend signal and residual echo in order to get refresh size
	for (i = 0; i <m_spectrum_len; i++)
	{
		m_xe_spec[i].re = (m_x_spec[i].re * m_ebuf_spec[i].re + m_x_spec[i].im * m_ebuf_spec[i].im) / (xPow[i] + MIN_POW);
		m_xe_spec[i].im = (m_x_spec[i].re * m_ebuf_spec[i].im - m_x_spec[i].im * m_ebuf_spec[i].re) / (xPow[i] + MIN_POW);
	}

	// limit refresh size, it's useful in double talk
#if 1
	for (i = 0; i <m_spectrum_len; i++)
	{
		pwtmp = sqrt(m_xe_spec[i].re*m_xe_spec[i].re + m_xe_spec[i].im * m_xe_spec[i].im);
		if (pwtmp>pAecGlobal->maxCoupling)
		{
			m_xe_spec[i].re = m_xe_spec[i].re / pwtmp*pAecGlobal->maxCoupling;
			m_xe_spec[i].im = m_xe_spec[i].im / pwtmp*pAecGlobal->maxCoupling;
		}

	}
#endif

	m_real_fft->run_inv_fft(m_xe_spec, m_xe_real);

	for (i = 0; i<m_filter_len; i++)
	{
		m_xe_real[i] *= m_fft_scale;
		m_xe_real[i + m_filter_len] = 0;
	}
	m_real_fft->run_fwd_fft(m_xe_real, m_xe_spec);

	//refresh adaptive filter in frequency domain.
	for (i = 0; i <m_spectrum_len; i++)
	{
		filterR[i] += m_xe_spec[i].re * mymu;
		filterI[i] += m_xe_spec[i].im * mymu;
	}
}

//-----------------------------------------------------------------------------------
// Frequency domain Adaptive filtering
//-----------------------------------------------------------------------------------
void CAec::adaptiveFilter(int c)
{
	int i;

	memcpy(m_x_real, &pAecGlobal->xBuf[c*m_fft_len], m_fft_len*sizeof(float));
	m_real_fft->run_fwd_fft(m_x_real, m_x_spec);

	memcpy(&pAecGlobal->xSpec[m_spectrum_len*c], m_x_spec, m_spectrum_len*sizeof(C));
	float *filterR = &pAecGlobal->filterRe[c*m_spectrum_len];
	float *filterI = &pAecGlobal->filterIm[c*m_spectrum_len];
	for (i = 0; i <m_spectrum_len; i++)
	{
		m_y_spec[i].re = m_x_spec[i].re * filterR[i] - m_x_spec[i].im * filterI[i];
		m_y_spec[i].im = m_x_spec[i].re * filterI[i] + m_x_spec[i].im * filterR[i];
	}

	m_real_fft->run_inv_fft(m_y_spec, m_y_real);
	for (i = 0; i < m_fft_len; i++)
	{
		m_y_real[i] *= m_fft_scale;
	}
	memcpy(&pAecGlobal->yBuf[c*m_fft_len], m_y_real, m_fft_len*sizeof(float));
}


//-----------------------------------------------------------------------------------
//Clipping compensation
//-----------------------------------------------------------------------------------
void CAec::spkClipCompensation(float * buf_spk, float Thd, int bsize)
{
	int i;
	for (i = 0; i<bsize; i++)
	{
		buf_spk[i] = maxv(buf_spk[i], -Thd);
		buf_spk[i] = minv(buf_spk[i], Thd);
	}
}


//******************************************************************
//Frequency domain NLMS
//******************************************************************
void CAec::fdnlms(float *dBuffer, const float *xBuffer)
{
	int i, j, c;
	float *pTemp1;
	float *pTemp3;
	int aecDelay = minv(max_filter_delay, maxv(filter_delay, min_filter_delay));


	float *dBuf = dBuffer;
	for (c = 0; c < m_spk_num; c++)
	{
		float *xBuf = &pAecGlobal->spkBuf[c*m_frame_len];
		for (i = 0; i < m_frame_len; i++)
		{
			xBuf[i] = xBuffer[m_spk_num*i+c];
		}
	}

	// we compensate clip effect of loudspeaker, so that adaptive filter works on its best
	if (pAecGlobal->enableSpkClip)  //enable clip compensation
	{
		for (c = 0; c < m_spk_num; c++)
		{
			float *xBuf = &pAecGlobal->spkBuf[c*m_frame_len];
			spkClipCompensation(xBuf, pAecGlobal->spkClipThd, m_frame_len);
		}
	}

	// we compensate constant delay (such as system delay), so that adaptive filter works on its best
	for (c = 0; c < m_spk_num; c++)
	{
		float *xBuf = &pAecGlobal->spkBuf[c*m_frame_len];
		float *circleBuf = &pAecGlobal->spkCircleBuf[c*system_delay_buf_size];
		for (i = 0; i < m_frame_len; i++)
		{
			circleBuf[pAecGlobal->spkWpt[c]] = xBuf[i];
			xBuf[i] = circleBuf[pAecGlobal->spkRpt[c]];
			pAecGlobal->spkWpt[c] = (pAecGlobal->spkWpt[c] + 1) % system_delay_buf_size;
			pAecGlobal->spkRpt[c] = (pAecGlobal->spkRpt[c] + 1) % system_delay_buf_size;
		}
	}


	for (i = 0; i<m_frame_len; i++)
	{
		pAecGlobal->dBuf[m_filter_len - m_frame_len + i] = dBuf[i];
		
	}
	for (c = 0; c < m_spk_num; c++)
	{
		float *xBufAcc = &pAecGlobal->xBuf[c*m_fft_len];
		float *xBuf = &pAecGlobal->spkBuf[c*m_frame_len];
		for (i = 0; i < m_frame_len; i++)
		{
			xBufAcc[m_fft_len - m_frame_len + i] = xBuf[i];
		}
	}  

	for (c = 0; c < m_spk_num; c++)
	{
		adaptiveFilter(c); //adaptive filtering
		
	}
	getErrorSpectrum();

	dtdProcess();//double talk detect

#ifdef ENABLE_DEBUG
	short debugDtFlag[FRAME_LEN];
	for (i = 0; i<FRAME_LEN; i++)
	{
		debugDtFlag[i] = pAecGlobal->dtdFlag * 10000;
	}
	fwrite(debugDtFlag, 2, m_frame_len, pfile4);
#endif

	if (pAecGlobal->vadFlag)	 //if far end is silent, we don't do filter refresh.
	{
		for (c = 0; c < m_spk_num; c++)
		{
			refreshFilter(c);
		}
	}

	//pTemp1=pAecGlobal->eBuf+MAX_FILTER_DELAY; //if no extra delay is accepted, aecDelay is set to 0.
	pTemp1 = pAecGlobal->eBuf + max_filter_delay - aecDelay;
	pTemp3 = dBuffer;
	for (i = 0; i<m_frame_len; i++)
	{
		(*pTemp3++) = (*pTemp1++);
	}

	for (c = 0; c < m_spk_num; c++)
	{
		float *xBuf = &pAecGlobal->xBuf[c*m_fft_len];
		memcpy(xBuf, xBuf + m_frame_len, (m_fft_len - m_frame_len)*sizeof(float));
	}
	memcpy(pAecGlobal->dBuf, pAecGlobal->dBuf + m_frame_len, (m_filter_len - m_frame_len)*sizeof(float));

	return;
}




