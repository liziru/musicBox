#include "bss.h"
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
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

#define FFT_LEN_16k (512)    //add window for fft
#define FFT_LEN (1024)
#ifndef min
#define min(a,b) ((a) <= (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) >= (b) ? (a) : (b))
#endif

CBss::CBss(int frame_length, int channelN, int sampleRate, bool multithread) : m_N(channelN)
										, m_frame_len(frame_length)
										, m_channelN(channelN)
										, m_sampleRate(sampleRate)
										, m_real_fft(NULL)
										, m_multithread(multithread)
										, m_data_real(NULL)
										, m_data_spec(NULL)
										, m_Z0(NULL)
										, m_Z1(NULL)
										, m_win(NULL)
										, m_data_real_win(NULL)
										, m_buff_out_real(NULL)
									
{	
	if (m_sampleRate == 16000) {
		m_fft_len = FFT_LEN_16k;
	}
	else {
		m_fft_len = FFT_LEN;
	}

	if (m_frame_len > m_fft_len)
		throw;
	m_fft_len_div_2 = m_fft_len >> 1;
	m_overlap_len = m_frame_len;   //KHT frame len = 256, fft_len = 512, overlap_len = 256	

	int fft_num = m_multithread ? channelN : 1;
	m_real_fft = new CRealFFT*[fft_num];

	uint logN = (uint)(log((float)m_fft_len) / log(2.f) + 0.5f);
	for (int i = 0; i < fft_num; i++) {
		m_real_fft[i] = new CRealFFT(logN);  //andy: real fft
	}
	m_fft_scale = 1.f / (float)m_fft_len;

	m_Z0 = new C[m_N * (m_fft_len_div_2 + 1)];
	m_Z1 = new C[m_N * (m_fft_len_div_2 + 1)];
	m_data_spec = new C[m_N * (m_fft_len_div_2 + 1)];	
	m_data_real = new R[m_N * m_fft_len];
	memset(m_data_real, 0x00, sizeof(R) * m_N * m_fft_len);
	m_data_real_win = new R[m_N * m_fft_len];
	memset(m_data_real_win, 0x00, sizeof(R) * m_N * m_fft_len);
	m_buff_out_real = new R[m_N * m_fft_len];
	memset(m_buff_out_real, 0x00, sizeof(R) * m_N * m_fft_len);

	m_mem = new R[m_N * m_overlap_len];
	memset(m_mem, 0x00, sizeof(R) * m_N * m_overlap_len);

	m_win = new float[m_frame_len * 2];  	
	for (int i = 0; i < m_frame_len * 2; i++) {
		float ang = M_PI * (float)(i + 1.f) / (float)m_fft_len;
		m_win[i] = sin(ang);
	}
	
	m_pBssParameter = new BSS_PARAMETER;
	m_pBssBuffer = new BSS_BUFFER;
	//first focus on the full-band comparing to Matlab code, later may change to sub-band
	init();	
	
}

CBss::~CBss()
{
	RELEASE_ARRAY(m_win);
	RELEASE_ARRAY(m_data_real);
	RELEASE_ARRAY(m_data_real_win);
	RELEASE_ARRAY(m_buff_out_real);	
	RELEASE_ARRAY(m_mem);
	RELEASE_ARRAY(m_data_spec);
	RELEASE_ARRAY(m_Z0);
	RELEASE_ARRAY(m_Z1);
	
	int fft_num = m_multithread ? m_N : 1;
	for (int i = 0; i < fft_num; i++) {		
		RELEASE_ARRAY(m_real_fft[i]);
	}
	RELEASE_ARRAY(m_real_fft);
	
	RELEASE_POINTER(m_pBssParameter);	
	if (NULL != m_pBssBuffer) {		
		RELEASE_ARRAY(m_pBssBuffer->ANGpXV0);
		RELEASE_ARRAY(m_pBssBuffer->ANGpXV1);
		RELEASE_ARRAY(m_pBssBuffer->Ga01);
		RELEASE_ARRAY(m_pBssBuffer->Ga10);
		RELEASE_ARRAY(m_pBssBuffer->LpGa01);
		RELEASE_ARRAY(m_pBssBuffer->LpGa10);
		RELEASE_ARRAY(m_pBssBuffer->LpX0);
		RELEASE_ARRAY(m_pBssBuffer->LpX1);
		RELEASE_ARRAY(m_pBssBuffer->LpXV0);
		RELEASE_ARRAY(m_pBssBuffer->LpXV1);
	}
	RELEASE_POINTER(m_pBssBuffer);
}

void CBss::set(int formatid)
{	
}

void CBss::init()
{	
	//%% Adaptive Decorrelation Filtering
	float T_A = 0.040;	//% base average time for power estimation(in sec)
	m_pBssParameter->gammaA = 1.0; //% adaptation rate for main acoustic filter
	
	int Rpt = 1;
	int L = m_fft_len_div_2;
	m_pBssParameter->Rpt = Rpt;	//% number of repititions for data-reusing
	
	//note: full band power monitor is not supported yet. May optimize it later.
	//but just hard coded
	m_pBssParameter->fL_FBpwr = 300;	//% lower - bound frequency for full - band power(Hz)
	m_pBssParameter->fH_FBpwr = 3450;	//% upper - bound frequency for full - band power(Hz)
	m_pBssParameter->T_dL2Px = 0.064;	//% defer time for dL2PxFB adaptation(in sec)
	m_pBssParameter->T_adapt = 0.032;	//% defer time for AEC filter adaptation(in sec)
	m_pBssParameter->L2Px_LB = -24;		//% lower - bound for full - band log power
	m_pBssParameter->L2Px_HB = -8;		//% upper - bound for full - band log power

	m_pBssParameter->alfaA = 1 - exp(-L / (T_A*m_sampleRate));
	m_pBssParameter->alfaAr = 1 - exp(-L / (T_A*m_sampleRate*Rpt));
	m_pBssParameter->L_alfaA = log(m_pBssParameter->alfaA) / log(2.0f);  //used in do_bss()
	m_pBssParameter->L_alfaAr = log(m_pBssParameter->alfaAr) / log(2.0f); //used in do_bss()

	m_pBssParameter->muA_on = m_pBssParameter->gammaA;
	m_pBssParameter->muA_off = 0;
	m_pBssParameter->muA = m_pBssParameter->muA_off;

	m_pBssParameter->muA = 1.f; //since we do not support the power monitor, just set it to 1.
	
	//buffer init - only consider 2 channels, may extend it later.
	m_pBssBuffer->Ga01 = new C[m_fft_len_div_2 + 1];
	m_pBssBuffer->Ga10 = new C[m_fft_len_div_2 + 1];
	m_pBssBuffer->LpGa01 = new float[m_fft_len_div_2 + 1];
	m_pBssBuffer->LpGa10 = new float[m_fft_len_div_2 + 1];
	m_pBssBuffer->LpX0 = new float[m_fft_len_div_2 + 1];
	m_pBssBuffer->LpX1 = new float[m_fft_len_div_2 + 1];
	m_pBssBuffer->LpXV0 = new float[m_fft_len_div_2 + 1];
	m_pBssBuffer->LpXV1 = new float[m_fft_len_div_2 + 1];
	m_pBssBuffer->ANGpXV0 = new float[m_fft_len_div_2 + 1];
	m_pBssBuffer->ANGpXV1 = new float[m_fft_len_div_2 + 1];

	for (int i = 0; i < m_fft_len_div_2 + 1; i++) {
		m_pBssBuffer->Ga01[i].re = 0.5f;
		m_pBssBuffer->Ga10[i].re = 0.8f;
		m_pBssBuffer->Ga01[i].im = 0.f;
		m_pBssBuffer->Ga10[i].im = 0.f;

		m_pBssBuffer->LpGa01[i] = log(m_pBssBuffer->Ga01[i].re * m_pBssBuffer->Ga01[i].re + m_pBssBuffer->Ga01[i].im * m_pBssBuffer->Ga01[i].im)/ log(2.0f);
		m_pBssBuffer->LpGa10[i] = log(m_pBssBuffer->Ga10[i].re * m_pBssBuffer->Ga10[i].re + m_pBssBuffer->Ga10[i].im * m_pBssBuffer->Ga10[i].im) / log(2.0f);

		m_pBssBuffer->LpX0[i] = LdeltaP;
		m_pBssBuffer->LpX1[i] = LdeltaP;
		m_pBssBuffer->LpXV0[i] = LdeltaP;
		m_pBssBuffer->LpXV1[i] = LdeltaP;

		m_pBssBuffer->ANGpXV0[i] = 0.f;
		m_pBssBuffer->ANGpXV1[i] = 0.f;
	}
}

// Update is called once every frame
void CBss::process(const float *in, float *out)
{
	analysis_filter_bank(in);
	do_bss();
	pre_sfb();
	synthesis_filter_bank(out);
}

//do real fft with window
void CBss::fft(int c)
{
	m_real_fft[m_multithread ? c : 0]->run_fwd_fft(m_data_real_win + m_fft_len * c, m_data_spec + (m_fft_len_div_2 + 1) * c);	
}

//prepare the data and add window before fft
void CBss::o_to_a(const float *in, int c)
{	
	memmove(m_data_real + m_fft_len * c, m_data_real + m_fft_len * c + m_frame_len, sizeof(float) * m_overlap_len);
	R *data_real_iter = m_data_real + m_fft_len * c + m_overlap_len;
	//BSS will focus on the number of mics not the format/profile of array
	const float *in_iter = in + c;
	for (int i = 0; i < m_frame_len; i++) {
		*data_real_iter = *in_iter;
		in_iter += m_N; data_real_iter++;
	}		

	data_real_iter = m_data_real + m_fft_len * c;
	R *data_real_iter_win = m_data_real_win + m_fft_len * c;
	const float *win_iter = m_win;
	for (int i = 0; i < m_frame_len * 2; i++) {
		*data_real_iter_win = *data_real_iter * *win_iter;
		data_real_iter_win++; data_real_iter++; win_iter++;
	}
	if (m_frame_len * 2 < m_fft_len) {
		data_real_iter_win = m_data_real_win + m_frame_len * 2 + m_fft_len * c;
		for (int i = m_frame_len * 2; i < m_fft_len; i++) {
			*data_real_iter_win = 0;
			data_real_iter_win++;
		}
	}
	
}

void CBss::analysis_filter_bank(const float *in)
{
	for (int c = 0; c < m_N; c++) {
		o_to_a(in, c); //add window before fft
		fft(c);
	}	
}

//return - update the m_Z0, m_Z1
void CBss::do_bss()
{			
	float muA = m_pBssParameter->muA;  //here, muA may be optimized later.
	float muAt = muA;              //1.0 as per definition

	float *LpX0 = m_pBssBuffer->LpX0;
	float *LpX1 = m_pBssBuffer->LpX1;
	C *Ga01 = m_pBssBuffer->Ga01;
	C *Ga10 = m_pBssBuffer->Ga10;
	float L_alfaA = m_pBssParameter->L_alfaA;
	float L_alfaAr = m_pBssParameter->L_alfaAr;	
	float *LpXV0 = m_pBssBuffer->LpXV0;
	float *LpXV1 = m_pBssBuffer->LpXV1;
	float *ANGpXV0 = m_pBssBuffer->ANGpXV0;
	float *ANGpXV1 = m_pBssBuffer->ANGpXV1;
	float *LpGa01 = m_pBssBuffer->LpGa01;
	float *LpGa10 = m_pBssBuffer->LpGa10;

	const C *X0 = m_data_spec; //first mic - X0
	const C *X1 = m_data_spec + (m_fft_len_div_2 + 1); //second mic - X1
	
	float temp = 0.f;

	for (int i = 0; i < m_fft_len_div_2 + 1; i++) {
		//%% Input power estimation		
		float pX0t = X0[i].re * X0[i].re + X0[i].im * X0[i].im;
		float pX1t = X1[i].re * X1[i].re + X1[i].im * X1[i].im;
		float LpX0t = log(max(pX0t, deltaP)) / log(2.0f);
		float LpX1t = log(max(pX1t, deltaP)) / log(2.0f);

		float dLpX0 = LpX0t - LpX0[i];
		float dLpX1 = LpX1t - LpX1[i];
		
		float alfaAX0 = powf(2.f, min(L_alfaA + 2.f * dLpX0, 0));
		float alfaAX1 = powf(2.f, min(L_alfaA + 2.f * dLpX1, 0));

		LpX0[i] += alfaAX0 * dLpX0;
		LpX1[i] += alfaAX1 * dLpX1;

		float LPwrX0 = max(LpX0[i], LpX0t);
		float LPwrX1 = max(LpX1[i], LpX1t);
		
		//%% Full-band power monitoring  -- skipped
		//

		//%% decorrelation filtering			
		muA = 1.0;  //As per Matlab code, since skipped power monitoring 
		muAt = muA;

		// Rpt=1
		//
		//%% Acoustic echo filtering		
		C Y01, Y10;		
		Y01.re = Ga01[i].re * X1[i].re - Ga01[i].im * X1[i].im;
		Y01.im = Ga01[i].re * X1[i].im + Ga01[i].im * X1[i].re;		
		Y10.re = Ga10[i].re * X0[i].re - Ga10[i].im * X0[i].im;
		Y10.im = Ga10[i].re * X0[i].im + Ga10[i].im * X0[i].re;
		C V0, V1;		
		V0.re = X0[i].re - Y01.re;
		V0.im = X0[i].im - Y01.im;		
		V1.re = X1[i].re - Y10.re;
		V1.im = X1[i].im - Y10.im;
		
		//%% Normalization factor
		C NormFactor;	//tempory complex for Normalization factor			
		temp = Ga01[i].re * Ga10[i].re - Ga01[i].im * Ga10[i].im;
		NormFactor.re = 1.f - temp;
		temp = Ga01[i].re * Ga10[i].im + Ga01[i].im * Ga10[i].re;
		NormFactor.im = 0.f - temp;			
		//tempory real for power of Normalization factor
		float pNormFactor = NormFactor.re * NormFactor.re + NormFactor.im * NormFactor.im;		
		//tempory real for log2 of power of Normalization factor
		float LpNormFactor = log(max(pNormFactor, deltaP))/ log(2.0f);
		
		//%% Cross - correlation estimation		
		float pV0t = V0.re * V0.re + V0.im * V0.im;
		float pV1t = V1.re * V1.re + V1.im * V1.im;
		float LpV0t = log(max(pV0t, deltaP)) / log(2.0f);
		float LpV1t = log(max(pV1t, deltaP)) / log(2.0f);
		float LpXV0t = (LpX0t + LpV0t) / 2.f;
		float LpXV1t = (LpX1t + LpV1t) / 2.f;
		float LpV01t = (LpV0t + LpV1t) / 2.f;		
		C pXV0t, pXV1t;
		pXV0t.re = X0[i].re * V0.re + X0[i].im * V0.im;
		pXV0t.im = X0[i].im * V0.re - X0[i].re * V0.im;
		pXV1t.re = X1[i].re * V1.re + X1[i].im * V1.im;
		pXV1t.im = X1[i].im * V1.re - X1[i].re * V1.im;		
		
		float ANGpXV0t = fixpt_angle(pXV0t);
		float ANGpXV1t = fixpt_angle(pXV1t); //[-1,1)

		float dLpXV0 = LpXV0t - LpXV0[i];
		float dLpXV1 = LpXV1t - LpXV1[i];

		float dANGpXV0 = ANGpXV0t - ANGpXV0[i];
		float dANGpXV1 = ANGpXV1t - ANGpXV1[i]; //[-2,2)
		dANGpXV0 = wrap_around(dANGpXV0);    //[-1,1)
		dANGpXV1 = wrap_around(dANGpXV1);    //[-1,1)

		float alfaAXV0 = powf(2.f,min(L_alfaAr + 2.0 * dLpXV0, 0));
		float alfaAXV1 = powf(2.f,min(L_alfaAr + 2.0 * dLpXV1, 0));

		LpXV0[i] += alfaAXV0 * dLpXV0;
		LpXV1[i] += alfaAXV1 * dLpXV1;

		ANGpXV0[i] += alfaAXV0 * dANGpXV0;
		ANGpXV1[i] += alfaAXV1 * dANGpXV1;
		ANGpXV0[i] = wrap_around(ANGpXV0[i]);
		ANGpXV1[i] = wrap_around(ANGpXV1[i]);

		//%%  Target-to-Interfering Ratios(TIRs) estimation			
		C Int0, Int1;
		Int0.re = NormFactor.re * X0[i].re - NormFactor.im * X0[i].im - V0.re;
		Int0.im = NormFactor.re * X0[i].im + NormFactor.im * X0[i].re - V0.im;
		Int1.re = NormFactor.re * X1[i].re - NormFactor.im * X1[i].im - V1.re;
		Int1.im = NormFactor.re * X1[i].im + NormFactor.im * X1[i].re - V1.im;		
		float pInt0 = Int0.re * Int0.re + Int0.im * Int0.im;
		float pInt1 = Int1.re * Int1.re + Int1.im * Int1.im;
		float IntRatio0o = pInt0 / max(max(pNormFactor * pX0t, pInt0), deltaP);
		float IntRatio1o = pInt1 / max(max(pNormFactor * pX1t, pInt1), deltaP);
		float CompIntRatio = max(1.f - (IntRatio0o + IntRatio1o), 0);
		float IntRatio0 = max(min(IntRatio0o + CompIntRatio, 1.f), 0);
		float IntRatio1 = max(min(IntRatio1o + CompIntRatio, 1.f), 0);

		//%% Filter adaptation			
		float LPwrXV0 = max(LpXV0[i], LpXV0t);
		float LPwrXV1 = max(LpXV1[i], LpXV1t);
		float LPwr0 = max(LPwrX0, LPwrXV0);
		float LPwr1 = max(LPwrX1, LPwrXV1);

		float maxLPwr = max(LPwr0, LPwr1);
		float diffLPwr = -fabs(LPwr0 - LPwr1);
		float LPwr = maxLPwr + powf(2.f,diffLPwr);
		int Exp_InvPwr = floor(-LPwr) + 1;
		float Mts_InvPwr = powf(2.f,-LPwr - Exp_InvPwr);

		//%% kalimba function : dGaCalc()
		float COSpXV0 = fixpt_cos(ANGpXV0[i]);
		float COSpXV1 = fixpt_cos(ANGpXV1[i]);				
		float PwrNorm01 = COSpXV1 * Mts_InvPwr; //consistent with matlab. But suspect here.
		float PwrNorm10 = COSpXV0 * Mts_InvPwr; //same as above.
		int Exp_pV01t = floor(LpV01t) + 1;
		C pV01t;
		pV01t.re = V0.re * V1.re + V0.im * V1.im;
		pV01t.im = V1.re * V0.im - V0.re * V1.im;		
		pV01t.re = bits_shift(pV01t.re, -Exp_pV01t);
		pV01t.im = bits_shift(pV01t.im, -Exp_pV01t);
		C dGa01, dGa10;
		dGa01.re = PwrNorm01 * pV01t.re;
		dGa01.im = PwrNorm01 * pV01t.im;

		dGa10.re = PwrNorm10 * pV01t.re;
		dGa10.im = -PwrNorm10 * pV01t.im;
		int Exp_dGa = -Exp_InvPwr - Exp_pV01t;

		//%% reliability
		float xdGa01 = dGa01.re * Ga01[i].re + dGa01.im * Ga01[i].im;
		float xdGa10 = dGa10.re * Ga10[i].re + dGa10.im * Ga10[i].im;
		float LxdGa01 = log(max(fabs(xdGa01), deltaP)) / log(2.0f);
		float LxdGa10 = log(max(fabs(xdGa10), deltaP)) / log(2.0f);
		int sgn_xdGa01 = sign(xdGa01);
		int sgn_xdGa10 = sign(xdGa10);

		float reliability01 = LpGa01[i] - LxdGa01;
		float reliability10 = LpGa10[i] - LxdGa10;

		//%% adapt Factor Calculation
		float diffR01 = sgn_xdGa01 * reliability01 - sgn_xdGa10 * reliability10;
		int neg_diffR01 = (diffR01<0);
		float adaptFactor = 1.f / (1.f + powf(2.f,-fabs(diffR01)));
		float adaptFactor01 = neg_diffR01 * adaptFactor + (1 - neg_diffR01) * (1.f - adaptFactor);
		float adaptFactor10 = (1 - neg_diffR01) * adaptFactor + neg_diffR01 * (1.f - adaptFactor);

		//%% update Decorr Filter
		dGa01.re = muAt * IntRatio0 * adaptFactor01 * dGa01.re;
		dGa01.im = muAt * IntRatio0 * adaptFactor01 * dGa01.im;
		dGa10.re = muAt * IntRatio1 * adaptFactor10 * dGa10.re;
		dGa10.im = muAt * IntRatio1 * adaptFactor10 * dGa10.im;
		
		dGa01.re = bits_shift(dGa01.re, -Exp_dGa);
		dGa01.im = bits_shift(dGa01.im, -Exp_dGa);
		dGa10.re = bits_shift(dGa10.re, -Exp_dGa);
		dGa10.im = bits_shift(dGa10.im, -Exp_dGa);

		Ga01[i].re += dGa01.re;
		Ga01[i].im += dGa01.im;
		Ga10[i].re += dGa10.re;
		Ga10[i].im += dGa10.im;

		float pGa01 = Ga01[i].re * Ga01[i].re + Ga01[i].im * Ga01[i].im;
		float pGa10 = Ga10[i].re * Ga10[i].re + Ga10[i].im * Ga10[i].im;

		LpGa01[i] = log(max(pGa01, deltaP)) / log(2.0f);
		LpGa10[i] = log(max(pGa10, deltaP)) / log(2.0f);

		muAt = muAt / 2; //this is not impacted, since later it was reset to 1.0

		//%% norm Outputs
		int Exp_InvPNormFactor = floor(-LpNormFactor) + 1;
		float Mts_InvPNormFactor = powf(2.f,-LpNormFactor - Exp_InvPNormFactor);
		C invNormFactor;
		invNormFactor.re = NormFactor.re * Mts_InvPNormFactor;
		invNormFactor.im = -NormFactor.im * Mts_InvPNormFactor;
			
		m_Z0[i].re = V0.re * invNormFactor.re - V0.im * invNormFactor.im;
		m_Z0[i].im = V0.re * invNormFactor.im + V0.im * invNormFactor.re;
		m_Z1[i].re = V1.re * invNormFactor.re - V1.im * invNormFactor.im;
		m_Z1[i].im = V1.re * invNormFactor.im + V1.im * invNormFactor.re;
		m_Z0[i].re = bits_shift(m_Z0[i].re, Exp_InvPNormFactor);
		m_Z0[i].im = bits_shift(m_Z0[i].im, Exp_InvPNormFactor);
		m_Z1[i].re = bits_shift(m_Z1[i].re, Exp_InvPNormFactor);
		m_Z1[i].im = bits_shift(m_Z1[i].im, Exp_InvPNormFactor);		
	}
	
	m_pBssParameter->muA = muA;	
}

//just transfer the bss result(Z0) to data_spec for sfb processing
void CBss::pre_sfb() 
{	
	C *Z0 = m_data_spec; //first mic 
	C *Z1 = m_data_spec + (m_fft_len_div_2 + 1); //second mic
	for (int i = 1; i < m_fft_len_div_2; i++) {
		Z0[i].re = m_Z0[i].re; 		
		Z0[i].im = m_Z0[i].im;
		Z1[i].re = m_Z1[i].re;
		Z1[i].im = m_Z1[i].im;
	}
	Z0[0].re = 0; Z0[0].im = 0;
	Z0[m_fft_len_div_2].re = 0; Z0[m_fft_len_div_2].im = 0;
	Z1[0].re = 0; Z1[0].im = 0;
	Z1[m_fft_len_div_2].re = 0; Z1[m_fft_len_div_2].im = 0;
}

int CBss::sign(float x)
{
	return (x > 0) - (x < 0);
}
float CBss::bits_shift(float x, int NumBits) 
{
	return x * powf(2.f, NumBits);
}
float CBss::fixpt_cos(float x)
{
	return cos(x*M_PI);
}
float CBss::fixpt_angle(C c)
{
	float y = calcuAngleRadians(c);   //[-pi,pi)	
	return y / M_PI;      //[-1,1)
}

//@return - a value is between [-pi,pi)
float CBss::calcuAngleRadians(C c)
{
	float angle;
	if (c.re > 0) {
		angle = atanf(c.im / c.re); //in the 1 or 4 sub-area
	} else if(c.re < 0) {
		angle = atanf(c.im / c.re);
		if (c.im > 0) {             //in the 2 sub-area
			angle = angle + M_PI;
		} else if(c.im < 0) {      //in the 3 sub-area
			angle = angle - M_PI;
		}
		else {
			angle = -M_PI;
		}
	} else {
		if (c.im > 0) {
			angle = M_PI/2.f;
		}
		else if (c.im < 0) {
			angle = M_PI /(-2.f);
		}
		else {
			angle = 0;
		}
	}		
	return angle;
}

//wrap around a real number to range:[-1,1). i.e 1 ==> -1
float CBss::wrap_around(float x)
{		
	int y = floor(x + 1);
	float z = x - floor(y / 2.0) * 2;
	return z;
}

void CBss::synthesis_filter_bank(float *out)
{
	for (int c = 0; c < m_N; c++) {		
		m_real_fft[m_multithread ? c : 0]->run_inv_fft(m_data_spec + (m_fft_len_div_2 + 1) * c, m_buff_out_real + m_fft_len * c);
		const float *sin_win_iter = m_win;
		R *buf_out_iter = m_buff_out_real + m_fft_len * c;
		for (int i = 0; i < m_frame_len * 2; i++) {
			*buf_out_iter *= (*sin_win_iter * m_fft_scale);
			buf_out_iter++; sin_win_iter++;
		}
		float *out_iter = out + c;
		buf_out_iter = m_buff_out_real + m_fft_len * c;
		float *mem_iter = m_mem + m_overlap_len * c;
		float *overlap_iter = m_buff_out_real + m_overlap_len + m_fft_len * c;

		if (NULL != out_iter) {
			for (int i = 0; i < m_frame_len; i++) {
				*out_iter = *buf_out_iter + *mem_iter;		//do we need to add*?
				*mem_iter = *overlap_iter;

				out_iter += m_channelN; buf_out_iter++; overlap_iter++; mem_iter++;
			}
		}
	}

}
