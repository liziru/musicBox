#include "realFFT.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

CFFT::CFFT(uint logN) : m_temp(NULL)
                      , m_rev(NULL)
{
    m_logN = logN;
	m_N = (uint)(1 << (int)m_logN);
	m_rev = new uint[m_N];
	m_temp = new C[m_N];

	for (uint k = 0; k < m_N; k++) {
		m_rev[k] = bit_reverse(k, logN);
	}
}

CFFT::~CFFT()
{
	if (NULL != m_rev) {
		delete [] m_rev;
		m_rev = NULL;
	}

	if (NULL != m_temp) {
		delete [] m_temp;
		m_temp = NULL;
	}
}
	
uint CFFT::bit_reverse(uint x, uint num)
{
	uint y = 0;
	for (uint i = 0; i < num; i++) {
		y <<= 1;
		y |= x & 0x0001;
		x >>= 1;
	}
	return y;
}

void CFFT::run_fwd_fft(const R *in, C *out)
{
	fft((const C*)in, out, false);
}

void CFFT::run_inv_fft(const C *in, R *out)
{
	fft(in, (C*)out, true);
}

void CFFT::fft(const C *in, C *out, bool inv)
{
	memcpy(m_temp, in, sizeof(C) * m_N);

	for (int stage = 0; stage < m_logN; stage++) {
		R ang = (R)((1 << stage) * 2.0 * M_PI / m_N) * (inv ? 1 : -1);
		R mul_re = (R)cos(ang);
		R mul_im = (R)sin(ang);
		for (int start = 0; start < m_N; start += (m_N >> stage)) {
			C *t = m_temp + start, *b = m_temp + start + (m_N >> (stage + 1));
			R re = 1.f;
			R im = 0.f;
				
			for (uint fly = 0; fly < (m_N >> (stage + 1)); ++fly) {
				C top = *t, bot = *b;
				t->re = top.re + bot.re;
				t->im = top.im + bot.im;
				t++;
				bot.re = top.re - bot.re;
				bot.im = top.im - bot.im;
				b->re = bot.re * re - bot.im * im;
				b->im = bot.re * im + bot.im * re;
				b++;
				R re_t = re;
				re = re   * mul_re - im * mul_im;
				im = re_t * mul_im + im * mul_re;
			}
		}
	}

	const uint *r = m_rev;
	C *o = out;
	for (int k = 0; k < m_N; k++) {
		*o = *(m_temp + *r);
		o++; r++;
	}
}

CRealFFT::CRealFFT(uint logN) : m_temp(NULL)
							  , m_fft(NULL)
							  , m_tab_sin(NULL)
							  , m_tab_cos(NULL)
{
	m_N = (uint)(1 << ((int)logN - 1));
    m_fft = new CFFT(logN - 1);
    fill_table();

	m_temp = new C[m_N];
}

CRealFFT::~CRealFFT()
{
	if (NULL != m_fft) {
		delete m_fft;
		m_fft = NULL;
	}

	if (NULL != m_temp) {
		delete [] m_temp;
		m_temp = NULL;
	}

	if (NULL != m_tab_sin) {
		delete [] m_tab_sin;
		m_tab_sin = NULL;
	}

	if (NULL != m_tab_cos) {
		delete [] m_tab_cos;
		m_tab_cos = NULL;
	}
}

void CRealFFT::fill_table()
{
	R fac = (R) -M_PI / m_N;
	m_tab_sin = new R[m_N];
	m_tab_cos = new R[m_N];
	for (int i = 0; i < m_N; i++) {
		R theta = fac * i;
		m_tab_sin[i] = (R)sin(theta);
		m_tab_cos[i] = (R)cos(theta);
	}
}

void CRealFFT::run_fwd_fft(const R *in, C *out)
{
	m_fft->run_fwd_fft(in, m_temp);

	out->re = m_temp->re + m_temp->im;
	out->im = 0.f;
	(out + m_N)->re = m_temp->re - m_temp->im;
	(out + m_N)->im = 0.f;

	fft(m_temp, out, false);
}

void CRealFFT::run_inv_fft(const C *in, R *out)
{
	m_temp->re = in->re + (in + m_N)->re;
	m_temp->im = in->re - (in + m_N)->re;

	fft(in, m_temp, true);

	m_fft->run_inv_fft(m_temp, out);
}

void CRealFFT::fft(const C *in, C *out, bool inv)
{
	const R *s_f = m_tab_sin + 1, *s_b = m_tab_sin + m_N - 1;
	const R *c_f = m_tab_cos + 1, *c_b = m_tab_cos + m_N - 1;
	const C *i_f = in + 1, *i_b = in + m_N - 1;
	C *o_f = out + 1, *o_b = out + m_N - 1;
	if (inv) {
		for (int i = 0; i < (m_N >> 1); i++) {
			C f, b;
			f.re =  i_f->re + i_b->re;
			f.im =  i_f->im - i_b->im;
			b.re = -i_f->im - i_b->im;
			b.im = -i_b->re + i_f->re;
			o_f->re = ( f.re + *c_f * b.re + *s_f * b.im);
			o_f->im = ( f.im + *c_f * b.im - *s_f * b.re);
			o_b->re = ( f.re + *c_b * b.re - *s_b * b.im);
			o_b->im = (-f.im - *c_b * b.im - *s_b * b.re);
			s_f++; c_f++; i_f++; o_f++;
			s_b--; c_b--; i_b--; o_b--;
		}
	}
	else {	
		for (int i = 0; i < (m_N >> 1); i++) {
			C f, b;
			f.re = i_f->re + i_b->re;
			f.im = i_f->im - i_b->im;
			b.re = i_f->im + i_b->im;
			b.im = i_b->re - i_f->re;
			o_f->re = ( f.re + *c_f * b.re - *s_f * b.im) * 0.5f;
			o_f->im = ( f.im + *c_f * b.im + *s_f * b.re) * 0.5f;
			o_b->re = ( f.re + *c_b * b.re + *s_b * b.im) * 0.5f;
			o_b->im = (-f.im - *c_b * b.im + *s_b * b.re) * 0.5f;
			s_f++; c_f++; i_f++; o_f++;
			s_b--; c_b--; i_b--; o_b--;
		}
	}
}
