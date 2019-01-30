#ifndef __LOCALIZER_H__
#define __LOCALIZER_H__

#include "../fft/realFFT.h"
class CVad;


class CLocalizer
{
public:
	CLocalizer(int profileID, int frame_length, int channelN, int sampleRate, float micDist, bool multithread = false);
	virtual ~CLocalizer();
	void set(int formatid); //KHT provide the interface but no impact so far. May enhance in future.
	void process(const float *in, float *param);
	int getState(float targetDirection);
	void fft(int c);
private:
	void sound_power();
	//Find the closest frequency from the discrete frequency vector resulted from FFT
	//@input : the expected frequency value
	//@output: float* freq - the actual frequency value closest to the expected value
	//@output: int* idx - the actual index corresponding to the freq
	void findClosestFrequencey(float input, float *freq, int *idx);
	void metric_c();
	float get_target_angle();
	void steerv();
	void o_to_a(const float *in, int c);
	float medianValue();
	float minValue();

private:
	struct SoundParameter_s
	{
		float power;
		float location;
		float location_prev;
		float diffusivity;
		float reliability;
		float source_num;
		float vad_flag;        //KHT added to return the vad detection result
		float reserved1;       //KHT added for future usage: reserved1,reserved2,reserved3,reserved4
		float reserved2;
		float reserved3;
		float reserved4;
	};
	typedef SoundParameter_s SoundParameter;


	enum MicArrayType_e
	{
		AMBISONIC,
		BROADSIDE,
		CIRCULAR
	};
	typedef MicArrayType_e MicArrayType;

	enum BroadsideFormat_e
	{
		BROADSIDE_2MIC = 0x02,
		BROADSIDE_3MIC = 0x03,
		BROADSIDE_4MIC = 0x04,
		BROADSIDE_6MIC = 0x06
	};
	typedef BroadsideFormat_e BroadsideFormat;

	enum AmbisonicFormat_e
	{
		WXY            = 0x00,
		WXYZ_FUMA      = 0x01,
		WYZX_SN3D      = 0x02,
		AMBISONIC_3MIC = 0x03,
		A_FORMAT       = 0x04
	};
	typedef AmbisonicFormat_e AmbisonicFormat;

	enum CircularFormat_e
	{
		CIRCULAR_3MIC   = 0x03,
		CIRCULAR_4MIC   = 0x04,
		CIRCULAR_6MIC   = 0x06,
		CIRCULAR_6_1MIC = 0x07
	};
	typedef CircularFormat_e CircularFormat;

	
	SoundParameter m_snd_param;
	int m_profileID;
	int m_N;
	int m_sampleRate;
	float m_soundSpeed;
	float m_micDistance;
	float m_deltFrequency;
	float m_low_bnd_freq;
	int m_low_bnd_idx;
	float m_high_bnd_freq;
	int m_up_bnd_idx;
	int m_bnd_num;

	bool m_first;
	int m_frame_len;
	int m_fft_len;
	int m_fft_len_div_2;
	int m_overlap_len;
	int m_win_len;
	
	float m_sigma_smooth_factor;

	float *m_win;
	CRealFFT **m_real_fft;
	R *m_data_real;
	R *m_data_real_win;
	C *m_data_spec;
	C *m_steerv_broadside;
	float *m_steerv;
	float *m_alpha;
	float *m_factor;
	float *m_amp_sum;
	float *m_amp_sm;
	float m_targetDirection;
	int m_targetDirectionIndex;
	float m_width;
	int m_widthIndex;
	int m_targetNum;
	int m_targetFlag_prev;
	float *m_Rmax;
	
	C *m_sigma_c;	
	C *m_sigma_sm_c;
	C *m_sigma_broadside_c;
	C *m_sigma_broadside_sm_c;
	C *m_sigma_broadside_sm_norm_c;

	bool m_multithread;
	CVad *m_vad_inst;
	int m_format;
};
#endif /*__LOCALIZER_H__*/