#ifndef BLIND_SOURCE_SEPERATION_H
#define BLIND_SOURCE_SEPERATION_H
#include "bssdefines.h"

class CBss
{
public:
	//our example is 2 channels, 2 sources, but I think we may extend to 4 channels, 2 sources...may have better results.
	CBss(int frame_length, int channelN, int sampleRate, bool multithread);
	virtual ~CBss();    
	void process(const float *in, float *out);

	void set(int formatid); 
	

private:		
	void init();
	void fft(int c);	
	void o_to_a(const float *in, int c);	
	void analysis_filter_bank(const float *in);
	void do_bss();
	void pre_sfb();
	void synthesis_filter_bank(float *out);
	
	//calculate the phase angles, in radians, for a complex number
	//the phase is mapped into the range of [-pi,pi)	
	// zero's angle is defined as 0.
	float calcuAngleRadians(C c); 
	//calculate the angle of a complex and scale it to the range of [-1,1)
	float fixpt_angle(C c);
	//get sign of x, 1: x>0,-1: x<0,0: x=0.
	int sign(float x);
	//@parameter - radius: [-1,1) will be scaled to [-pi,pi)
	//@return - cos value: [-1,1]
	float fixpt_cos(float x);

	//scale x with pow2(NumBits)
	float bits_shift(float x, int NumBits);

	//wrap around a real number to [-1,1)
	float wrap_around(float x);

private:
	int m_channelN;
	int m_N;
	int m_sampleRate;
	int m_frame_len;
		
	BSS_PARAMETER *m_pBssParameter;
	BSS_BUFFER *m_pBssBuffer;

	int m_fft_len;
	int m_fft_len_div_2;
	int m_overlap_len;
	int m_win_len;

	float *m_win;
	CRealFFT **m_real_fft;
	float m_fft_scale;
	R *m_data_real;
	R *m_data_real_win;
	C *m_data_spec;

	C *m_Z0;  
	C *m_Z1;  
	R *m_buff_out_real;	
	R *m_mem;

	bool m_multithread;
};

#endif