#ifndef RESAMPLE_H
#define RESAMPLE_H

#define L_FILT_MAX  90
#define L_FILTER    181
#define L_FRAME     768 //510
#define L_FRAME_16  256 //170
#define L_FRAME_32  512 //340

class CResample
{
public:
     CResample(int iChan,int framelen, int sRate);
    ~CResample();
	 void initial(int iChan,int framelen, int sRate);
	 void resample(float *InputData, float *OutputData, int iChan, int framelen, int sRate);
private:
	void modify_Fs(float *sigIn, float *sigOut, float *mem, int filt_len);       
    float interpolation(float *x, const float *win, short frac, short up_samp, short nb_coef);                      
private:
	short lg;
	short lg_out;
	short fac_num;
	short fac_den;
	short filt_len_up;
	short filt_len_down;
	short frac;
	short mem_len;
    short datastep;
	short fracstep;
	float num_den;
	float *signal_tab;
	float *signal_tab_up;
	float *signal_tab_down;
    float *mem_up;
	float *mem_down;
	float *audioInputData_up;
	float *audioOutputData_up;
	float *audioInputData_down;
	float *audioOutputData_down;
	bool firstFlag;
};
#endif