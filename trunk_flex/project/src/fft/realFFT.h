#ifndef REALFFT_H
#define REALFFT_H

typedef unsigned int uint;
typedef float R;
struct C_s
{
	R re;
	R im;
};
typedef C_s C;

class CFFT
{
public:
    CFFT(uint logN);
    virtual ~CFFT();

	void run_fwd_fft(const R *in, C *out);
	void run_inv_fft(const C *in, R *out);

private:
	C *m_temp;
	uint *m_rev;
	
	uint m_logN;        // log2 of FFT size
	uint m_N;           // FFT size	

	uint bit_reverse(uint x, uint num);
	void fft(const C *in, C *out, bool inv);
};

class CRealFFT 
{
public:
    CRealFFT(uint logN);
    virtual ~CRealFFT();

	void run_fwd_fft(const R *in, C *out);
	void run_inv_fft(const C *in, R *out);

private:
    CFFT *m_fft;
	uint m_N;           // FFT size
	C *m_temp;

	R *m_tab_sin;
	R *m_tab_cos;

	void fft(const C *in, C *out, bool inv);
	void fill_table();
};

#endif