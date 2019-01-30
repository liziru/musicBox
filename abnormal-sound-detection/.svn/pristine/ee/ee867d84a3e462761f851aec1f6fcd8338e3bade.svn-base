#include <math.h>
#include "res.h"
#include <string.h>

#ifdef ENABLE_DEBUG
#include <stdio.h>
FILE* pfile_snr;
#endif

#ifndef PI
#define PI       3.14159265358979323846
#endif

#define AS  0.3f
#define AD  0.8f    
#define AP  0.3f     
#define AL  20    
#define PP 0.3f  
#define ZC 0.5f
#define SMOOTHA  0.9f
#define VARMIN   (1e-7f)
#define SMALLV (1e-9f)
#define SNRMAX (10.f)
#define SNRMIN (0.1f)

#define MAXSUP1 (-6.f)
#define MAXSUP2 (-20.f)
#define MAXNLPSUP1 (100.0f)
#define MAXNLPSUP2 (0.0f)


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



CRes::CRes(AecGlobalData* pAecGlobal)
{
	resInit(pAecGlobal);
}

CRes::~CRes()
{
	RELEASE_POINTER(m_real_fft);
	//RELEASE_POINTER(pResGlobal);
	RELEASE_ARRAY(m_in_real);
	RELEASE_ARRAY(m_in_spec);
	RELEASE_ARRAY(m_d_real);
	RELEASE_ARRAY(m_d_spec);
	RELEASE_ARRAY(m_y_real);
	RELEASE_ARRAY(m_y_spec);
	RELEASE_ARRAY(m_out_real);
	RELEASE_ARRAY(m_out_spec);

	RELEASE_ARRAY(pResGlobal->inBuf);
	RELEASE_ARRAY(pResGlobal->dfBuf);
	RELEASE_ARRAY(pResGlobal->dfBufPow);
	RELEASE_ARRAY(pResGlobal->yfBuf);
	RELEASE_ARRAY(pResGlobal->yfBufPow);
	RELEASE_ARRAY(pResGlobal->outBuf);
	RELEASE_ARRAY(pResGlobal->efBufPow);
	RELEASE_ARRAY(pResGlobal->efBufPowPrev);
	RELEASE_ARRAY(pResGlobal->efBufPowLocal);
	RELEASE_ARRAY(pResGlobal->speechVar);
	RELEASE_ARRAY(pResGlobal->noiseVar);
	RELEASE_ARRAY(pResGlobal->reAverage);
	RELEASE_ARRAY(pResGlobal->binGain);
	RELEASE_ARRAY(pResGlobal->binGain_smooth);
	RELEASE_ARRAY(pResGlobal->binBgNoiseGain);
	RELEASE_ARRAY(pResGlobal->m_speech_absence_possibility);

#ifdef ENABLE_DEBUG
	fclose(pfile_snr);
#endif
}



void CRes::resInit(AecGlobalData* pAecGlobal)
{
	pResGlobal = pAecGlobal->resGlobal;
	m_frame_len = pAecGlobal->framelens;
	m_filter_len = pAecGlobal->fftlens_res/2;
	m_spectrum_len = m_filter_len + 1;
	m_fft_len = m_filter_len * 2;
	m_res_fft_idx_low = (int)((float)m_fft_len * 200 / pAecGlobal->samplerate);
	m_res_fft_idx_high =  (int)((float)m_fft_len * 4000 / pAecGlobal->samplerate);
	if (m_res_fft_idx_high >= m_spectrum_len)
	{
		m_res_fft_idx_high = m_spectrum_len - 1;
	}

	pResGlobal->inBuf						= new float[m_fft_len];
	pResGlobal->dfBuf						= new float[m_fft_len];
	pResGlobal->dfBufPow					= new float[m_spectrum_len];
	pResGlobal->yfBuf						= new float[m_fft_len];
	pResGlobal->yfBufPow					= new float[m_spectrum_len];
	pResGlobal->outBuf						= new float[m_fft_len];
	pResGlobal->efBufPow					= new float[m_spectrum_len];
	pResGlobal->efBufPowPrev				= new float[m_spectrum_len];
	pResGlobal->efBufPowLocal				= new float[m_spectrum_len];
	pResGlobal->speechVar					= new float[m_spectrum_len];
	pResGlobal->noiseVar					= new float[m_spectrum_len];
	pResGlobal->reAverage					= new float[m_spectrum_len];
	pResGlobal->binGain						= new float[m_spectrum_len];
	pResGlobal->binGain_smooth				= new float[m_spectrum_len];
	pResGlobal->binBgNoiseGain				= new float[m_spectrum_len];
	pResGlobal->m_speech_absence_possibility = new float[m_spectrum_len];

	memset(pResGlobal->inBuf, 0, sizeof(float)*m_fft_len);
	memset(pResGlobal->dfBuf, 0, sizeof(float)*m_fft_len);
	memset(pResGlobal->dfBufPow, 0, sizeof(float)*m_spectrum_len);
	memset(pResGlobal->yfBuf, 0, sizeof(float)*m_fft_len);
	memset(pResGlobal->yfBufPow, 0, sizeof(float)*m_spectrum_len);
	memset(pResGlobal->outBuf, 0, sizeof(float)*m_fft_len);
	memset(pResGlobal->efBufPow, 0, sizeof(float)*m_spectrum_len);
	memset(pResGlobal->efBufPowPrev, 0, sizeof(float)*m_spectrum_len);
	memset(pResGlobal->efBufPowLocal, 0, sizeof(float)*m_spectrum_len);
	memset(pResGlobal->speechVar, 0, sizeof(float)*m_spectrum_len);
	memset(pResGlobal->noiseVar, 0, sizeof(float)*m_spectrum_len);
	memset(pResGlobal->reAverage, 0, sizeof(float)*m_spectrum_len);
	memset(pResGlobal->binGain, 0, sizeof(float)*m_spectrum_len);
	memset(pResGlobal->binGain_smooth, 0, sizeof(float)*m_spectrum_len);
	memset(pResGlobal->binBgNoiseGain, 0, sizeof(float)*m_spectrum_len);
	memset(pResGlobal->m_speech_absence_possibility, 0, sizeof(float)*m_spectrum_len);

	m_in_real = new R[m_fft_len];
	m_in_spec = new C[m_fft_len / 2 + 1];
	m_d_real = new R[m_fft_len];
	m_d_spec = new C[m_fft_len / 2 + 1];
	m_y_real = new R[m_fft_len];
	m_y_spec = new C[m_fft_len / 2 + 1];
	m_out_real = new R[m_fft_len];
	m_out_spec = new C[m_fft_len / 2 + 1];
	m_fft_scale = 1.f / (float)m_fft_len;
	uint logN = (uint)(log((float)m_fft_len) / log(2.0f) + 0.5);
	m_real_fft = new CRealFFT(logN);
	
	memset(m_in_real, 0, sizeof(R)*m_fft_len);
	memset(m_d_real, 0, sizeof(R)*m_fft_len);
	memset(m_y_real, 0, sizeof(R)*m_fft_len);
	memset(m_out_real, 0, sizeof(R)*m_fft_len);

	int i;
	for (i = 0; i<m_spectrum_len; i++)
		pResGlobal->noiseVar[i] = 1;
	for (i = 0; i<m_spectrum_len; i++)
		pResGlobal->reAverage[i] = pResGlobal->speechVar[i] = 1;
	for (i = 0; i<m_spectrum_len; i++)
		pResGlobal->efBufPowLocal[i] = 1;
	for (i = 0; i<m_spectrum_len; i++)
		pResGlobal->binGain_smooth[i] = 1.0;

	pResGlobal->avgsnrglobal = 0;
	pResGlobal->frameCnt = 0;

	pResGlobal->m_max_sup = powf(10.f, MAXSUP1 / 20.f);
	pResGlobal->m_max_nlp_sup = MAXNLPSUP2;
	
#ifdef ENABLE_DEBUG
	pfile_snr=fopen("snr.pcm","wb");
#endif
}

void CRes::resSet(AecGlobalData* pAecGlobal)
{
	pResGlobal->m_max_sup = pAecGlobal->nsDB;
	if (pResGlobal->m_max_sup > MAXSUP1) {
		pResGlobal->m_max_sup = MAXSUP1;
	}
	if (pResGlobal->m_max_sup < MAXSUP2) {
		pResGlobal->m_max_sup = MAXSUP2;
	}
	pResGlobal->m_max_sup = powf(10.f, pResGlobal->m_max_sup / 20.f);

	pResGlobal->m_max_nlp_sup = pAecGlobal->resLevel;
	if (pResGlobal->m_max_nlp_sup > MAXNLPSUP1) {
		pResGlobal->m_max_nlp_sup = MAXNLPSUP1;
	}
	if (pResGlobal->m_max_nlp_sup < MAXNLPSUP2) {
		pResGlobal->m_max_nlp_sup = MAXNLPSUP2;
	}
}

void CRes::noiseSpectrumEstimator()
{
	int i;
	float w;
	pResGlobal->efBufPowLocal[0] = AS*pResGlobal->efBufPowLocal[0]+ (1-AS)*pResGlobal->efBufPow[0];
	for (i = 1; i<m_filter_len; i++)
	{
		w = (pResGlobal->efBufPow[i-1]+pResGlobal->efBufPow[i+1] + 2*pResGlobal->efBufPow[i])/4;
		pResGlobal->efBufPowLocal[i] =   AS*pResGlobal->efBufPowLocal[i]+ (1-AS)*w;
	}
	pResGlobal->efBufPowLocal[m_filter_len] = AS*pResGlobal->efBufPowLocal[m_filter_len] + (1 - AS)*pResGlobal->efBufPow[m_filter_len];

	if (pResGlobal->frameCnt<5)
      {
		  for (i = 0; i<m_spectrum_len; i++)
	      {
				pResGlobal->noiseVar[i] = pResGlobal->efBufPowLocal[i];
				if(pResGlobal->noiseVar[i] <VARMIN) 
				pResGlobal->noiseVar[i] = VARMIN;
          }
      }
      else
      {
		  for (i = 0; i<m_spectrum_len; i++)
	      {
			  if (pResGlobal->efBufPowLocal[i] > pResGlobal->noiseVar[i])
			  {
				  if (pResGlobal->speechAbsencePossibilityAvg < 0.1f)
					  w = pResGlobal->noiseVar[i] / pResGlobal->efBufPowLocal[i] * 0.001f;
				  else
					  w = pResGlobal->noiseVar[i] / pResGlobal->efBufPowLocal[i] * 0.01f * pResGlobal->m_speech_absence_possibility[i];
			  }
			  else
				  w=0.1f;
		
			  pResGlobal->noiseVar[i] = (1-w)*pResGlobal->noiseVar[i] + w*pResGlobal->efBufPow[i];
			  if(pResGlobal->noiseVar[i] <VARMIN) 
				  pResGlobal->noiseVar[i] = VARMIN;
	      }
      }
	
}

void CRes::vocalSpectrumEstimator()
{
	int i;
	for (i = 0; i<m_spectrum_len; i++)
	{
		pResGlobal->speechVar[i] = pResGlobal->efBufPowPrev[i]*pResGlobal->speechVar[i]/(pResGlobal->speechVar[i]+pResGlobal->noiseVar[i])*SMOOTHA;

		if(pResGlobal->efBufPow[i]>pResGlobal->noiseVar[i])
			pResGlobal->speechVar[i] += (1-SMOOTHA)*(pResGlobal->efBufPow[i]-pResGlobal->noiseVar[i]);
	}
}


void CRes::snrEstimator()
{
	int i;
	float averageFrame;
	///////////////////////////////////////////
	for (i = 0; i <m_spectrum_len; i++)
	{
		pResGlobal->reAverage[i] = pResGlobal->reAverage[i] * ZC + pResGlobal->speechVar[i] / pResGlobal->noiseVar[i] * (1 - ZC);
		if (pResGlobal->reAverage[i] > SNRMAX) {
			pResGlobal->reAverage[i] = SNRMAX;
		}
		else if (pResGlobal->reAverage[i] < SNRMIN) {
			pResGlobal->reAverage[i] = SNRMIN;
		}
	}
		
    //global snr 
	averageFrame =0;
	for (i = m_res_fft_idx_low; i < m_res_fft_idx_high; i++)
		averageFrame += pResGlobal->reAverage[i];
	averageFrame = averageFrame / (m_res_fft_idx_high - m_res_fft_idx_low);

    pResGlobal->avgsnrglobal=averageFrame;   

	if (pResGlobal->avgsnrglobal>SNRMAX)
		pResGlobal->avgsnrglobal = SNRMAX;
	else if (pResGlobal->avgsnrglobal < SNRMIN) {
		pResGlobal->avgsnrglobal = SNRMIN;
	}
	pResGlobal->speechAbsencePossibilityAvg = 1.f - (pResGlobal->avgsnrglobal / SNRMAX);
	float *speech_absence_possibility_iter = pResGlobal->m_speech_absence_possibility;
	float *re_average_iter = pResGlobal->reAverage;

	for (int i = 0; i <m_spectrum_len; i++) {
		*speech_absence_possibility_iter = 1.f - (*re_average_iter / SNRMAX);
		re_average_iter++; speech_absence_possibility_iter++;
	}



#ifdef ENABLE_DEBUG
	short x[FRAME_LEN];
	for(i=0;i<FRAME_LEN;i++)
	{
		x[i]=pResGlobal->avgsnrglobal*100;
	}

	fwrite(x,sizeof(short),FRAME_LEN,pfile_snr);
#endif

}

void CRes::gainEstimator(AecGlobalData* pAecGlobal)
{
	int i;
	float tmp;
	for (i = 0; i<m_spectrum_len; i++)
		pResGlobal->efBufPowPrev[i] = pResGlobal->efBufPow[i];

	noiseSpectrumEstimator();
	vocalSpectrumEstimator();
	snrEstimator();

	float avgSuppress = (pResGlobal->avgefPow - pResGlobal->avgyfPow * pResGlobal->m_max_nlp_sup) / (pResGlobal->avgefPow + SMALLV);
	if (avgSuppress < 0) avgSuppress = 0;

	for (i = 0; i<m_spectrum_len; i++)
	{
		tmp = pResGlobal->noiseVar[i] / (pResGlobal->efBufPow[i] + SMALLV);
		if(tmp>1.0) tmp=1.0;
		if(tmp > pResGlobal->binBgNoiseGain[i] )
			pResGlobal->binBgNoiseGain[i] = pResGlobal->binBgNoiseGain[i]*0.5f + tmp * 0.5f;
		else
			pResGlobal->binBgNoiseGain[i] = tmp;

#if 1
	
		float x;
		if (pAecGlobal->dtdFlag == TURN_ON)   //need refine
			x = (pResGlobal->efBufPow[i] - pResGlobal->yfBufPow[i] * pResGlobal->m_max_nlp_sup) / (pResGlobal->efBufPow[i] + SMALLV);
		else
			x = (pResGlobal->efBufPow[i] - pResGlobal->yfBufPow[i] * pResGlobal->m_max_nlp_sup) / (pResGlobal->efBufPow[i] + SMALLV)*avgSuppress;
	
		if(x<pResGlobal->binBgNoiseGain[i])
			x = pResGlobal->binBgNoiseGain[i];     //preserve background noise as comfort noise.		

		if (pAecGlobal->enableNS)
		{
			tmp = pResGlobal->noiseVar[i] / (pResGlobal->efBufPow[i] * x + SMALLV);
			if (tmp > 1.f) {
				tmp = 1.f;
			}
			tmp = 1.f - tmp;
			tmp = tmp * (1.f - pResGlobal->m_speech_absence_possibility[i]) + pResGlobal->m_max_sup * pResGlobal->m_speech_absence_possibility[i];

			if (tmp < pResGlobal->m_max_sup) {
				tmp = pResGlobal->m_max_sup;
			}
			x = x*tmp;
		}
		x = pResGlobal->binGain_smooth[i] * 0.5f + x*0.5f;
		pResGlobal->binGain_smooth[i] = x;

		pResGlobal->binGain[i] = (x);

#else
		pResGlobal->binGain[i] = sqrt(pResGlobal->binBgNoiseGain[i]);   //test for bg noise estimation.
#endif
	}
}


void CRes::overlapPlus(AecGlobalData* pAecGlobal)
{
	int i, pos;
	float avg;

	for (pos = 0; pos<m_frame_len; pos++)
		pResGlobal->outBuf[pos] = m_out_real[pos + m_frame_len];

	for (i = 0; i < m_frame_len*2; i++)
	{
		m_in_real[i] = pResGlobal->inBuf[i] * sin(PI*i / (m_frame_len * 2));
		m_d_real[i] = pResGlobal->dfBuf[i] * sin(PI*i / (m_frame_len * 2));
		m_y_real[i] = pResGlobal->yfBuf[i] * sin(PI*i / (m_frame_len * 2));
	}
	
	for (i = m_frame_len * 2; i<m_fft_len; i++)
	{
		m_in_real[i] = 0;
		m_d_real[i]  = 0;
		m_y_real[i]  = 0;
	}

	m_real_fft->run_fwd_fft(m_in_real, m_in_spec);
	m_real_fft->run_fwd_fft(m_d_real, m_d_spec);
	m_real_fft->run_fwd_fft(m_y_real, m_y_spec);
	
	for (i = 0; i<m_spectrum_len; i++)
	{
		pResGlobal->efBufPow[i] = m_in_spec[i].re*m_in_spec[i].re + m_in_spec[i].im*m_in_spec[i].im;
		if (pResGlobal->efBufPow[i]<SMALLV) pResGlobal->efBufPow[i] = SMALLV;
		pResGlobal->dfBufPow[i] = m_d_spec[i].re*m_d_spec[i].re + m_d_spec[i].im*m_d_spec[i].im;
		if (pResGlobal->dfBufPow[i] < SMALLV) pResGlobal->dfBufPow[i] = SMALLV;
		pResGlobal->yfBufPow[i] = m_y_spec[i].re*m_y_spec[i].re + m_y_spec[i].im*m_y_spec[i].im;
		if (pResGlobal->yfBufPow[i] < SMALLV) pResGlobal->yfBufPow[i] = SMALLV;
	}

	avg = 0;
	for (i = m_res_fft_idx_low; i < m_res_fft_idx_high; i++)
		avg += pResGlobal->yfBufPow[i];
	avg = avg / (m_res_fft_idx_high - m_res_fft_idx_low);
	pResGlobal->avgyfPow = avg;
	avg = 0;
	for (i = m_res_fft_idx_low; i < m_res_fft_idx_high; i++)
		avg += pResGlobal->efBufPow[i];
	avg = avg / (m_res_fft_idx_high - m_res_fft_idx_low);
	pResGlobal->avgefPow = avg;

	gainEstimator(pAecGlobal);

	for (i = 0; i<m_spectrum_len; i++)
	{
		m_out_spec[i].re = m_in_spec[i].re * pResGlobal->binGain[i];
		m_out_spec[i].im = m_in_spec[i].im * pResGlobal->binGain[i];
	}
	m_real_fft->run_inv_fft(m_out_spec, m_out_real);

	for (i = 0; i<(m_frame_len * 2); i++)
	{
		m_out_real[i] *= (m_fft_scale*sin(PI*i / (m_frame_len * 2)));
	}
	
	for (pos = 0; pos<m_frame_len; pos++)
	{
		pResGlobal->outBuf[pos] += m_out_real[pos];
	}

	for (pos = 0; pos < m_frame_len; pos++)
	{
		pResGlobal->inBuf[pos] = pResGlobal->inBuf[pos + m_frame_len];
		pResGlobal->dfBuf[pos] = pResGlobal->dfBuf[pos + m_frame_len];
		pResGlobal->yfBuf[pos] = pResGlobal->yfBuf[pos + m_frame_len];
	}

}


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
int CRes::resProcess(float *eBuf, AecGlobalData* pAecGlobal)
{
	int i;
	pResGlobal->frameCnt++;
	for (i = 0; i<m_frame_len; i++)
	{
		pResGlobal->inBuf[i + m_frame_len] = eBuf[i];
		pResGlobal->dfBuf[i + m_frame_len] = pAecGlobal->dwf[i];
		pResGlobal->yfBuf[i + m_frame_len] = pAecGlobal->ywf[i];
	}
	overlapPlus(pAecGlobal);
	for (i = 0; i<m_frame_len; i++)
	{
		eBuf[i]=pResGlobal->outBuf[i];
	}
	return 1;
}

