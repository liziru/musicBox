#ifndef VAD_DEFINE_H
#define VAD_DEFINE_H

//KHT VAD Parameters - these are porting from matlab code

const float VAD_log10tolog2 = 3.3219;
const float deltaA = 5.9605e-08;	      //deltaA = pow2(-24);	% Numerical floor, amplitude (single percision)

const float VAD_T_a = 3;                  //% Attack Time to speech estimation of the VAD.
const float VAD_T_d = 2.8; //3;                  //% Decay Time to speech estimation of the VAD.
const float VAD_T_envelope = 0.032; // 0.032;       //% Attack Time to noise estimation of the VAD.
const float VAD_ratio = 0.2; //0.5000;			    //% Ratio betwen speech and input dymanic range for VAD detection.
const float VAD_init = 0.8; //0.2;				//% Initialization time(in sec.) for VAD when the vad detection in 0.
const float VAD_Min_Signal = 2; // 8;			//% Minimum amplitude for VAD detection.
const float VAD_Min_Max_Env = -44;		//% Minimum power envelope for VAD detection.
const float VAD_delta_Th = 5;			    //% Dinamic between maximum power and power to avoid ramping in stationary noise.
const float VAD_count_Th_time = 0.450;	//% Threshold in seconds to have detection of stationary noise.

//VAD_attack = 1 - exp(-1 / (VAD_T_a*(fs / L)))
//VAD_decay = 1 - exp(-1 / (VAD_T_d*(fs / L)))
//VAD_envelope = 1 - exp(-1 / (VAD_T_envelope*(fs / L)))
//VAD_init_frame = round(VAD_init*fs / L) % Number of frames for initialization.
//VAD_count_Th_frame = round(VAD_count_Th_time*fs / L)

typedef struct {
	float VAD_attack;
	float VAD_decay;
	float VAD_envelope;
	int VAD_init_frame;
	int VAD_count_Th_frame;
}VAD_PARAMETER;


//KHT VAD Buffer
typedef struct {
	float VAD_E_Filter;
	float VAD_E_Filter_max;
	float VAD_E_Filter_min;
	int VAD_counter;
	int VAD_counter_delta;
}VAD_BUFFER;

//KHT for webrtc vad
typedef struct _DATA_PACKET
{
	const char	*buffer;
	size_t	     length;
} DATA_PACKET;


#endif