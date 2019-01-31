#ifndef AGC_DEFINE_H
#define AGC_DEFINE_H

//KHT AGC Parameters - these are porting from matlab code

const float AGC_target_max = -3;
const float AGC_target_level = -3;          // Target level(in dB), lower than full scale.
const float AGC_max_gain_dB = 20; //30; //20;          // Maximum gain in dB.  % in dB
const float AGC_min_gain_dB = -20; // 0;           // Minimum gain in dB.
const float AGC_pre_gain_dB = 0;           // Constant gain to be applied before AGC(in dB).
const float AGC_decay_time = 1.0;         // How slow the AGC gain increases(in sec).
const float AGC_attack_time = 0.5;         // How fast the AGC gain decreases(in sec).
const float AGC_T_SP = 2.0;         // Time constant for speech envelope(in sec).
const float AGC_percent_peak = 0.9;         // Time constant for estimation of speech peak(in percen / 100).
const float AGC_input_Th_dB = -2;          // Value in dB to the one the AGC will adapt always if the input is above it.

const float AGC_speed_attack = 3;           // Speed up the AGC gain decrement if the input is above AGC_input_Th.
const float AGC_compression = 0.7;         // Angular coeficient of the compressor.
const float AGC_adap_Th = 1;           // Threshold in dB for which the AGC_real_gain is updated.


typedef struct {
	float AGC_alfa_a_ppeak;
	float AGC_alfa_d_ppeak;
	float AGC_target;
	float AGC_target_max;
	float AGC_max_gain;
	float AGC_min_gain;
	float AGC_pre_gain;
	float AGC_attack;
	float AGC_decay;
	float AGC_input_Th;
	float AGC_adap_Th_linear;
}AGC_PARAMETER;


//KHT AGC Buffer
typedef struct {
	float AGC_input_level;
	float AGC_gain;
	float AGC_real_gain;
	float AGC_real_gain_prev;
}AGC_BUFFER;


#endif