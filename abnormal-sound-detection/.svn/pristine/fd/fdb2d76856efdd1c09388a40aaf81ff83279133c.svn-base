#ifndef BSS_DEFINE_H
#define BSS_DEFINE_H
#include "../fft/realFFT.h"
//KHT BSS Parameters - these are porting from Matlab code

//%% Delay compensation
//const float DlyIO = 0;		//% leave file I / O delay in Snd_Out 
const int Nbits = 24;			    //% Number of bits for single precision
const int LdeltaA = -(Nbits);		//% Numerical floor, log2(amplitude)
const int LdeltaP = LdeltaA * 2;	//% Numerical floor, log2(power)
//const float deltaA = 5.9605e-08;	//% Numerical floor, amplitude(single percision), pow2(LdeltaA)
const float deltaP = 3.5527e-15;	//% Numerical floor, power(double precision), pow2(LdeltaP)

typedef struct {
	float gammaA;
	int Rpt;	//% number of repititions for data-reusing
	float fL_FBpwr;
	float fH_FBpwr;
	float T_dL2Px;
	float T_adapt;
	float L2Px_LB;
	float L2Px_HB;
	float alfaA;
	float alfaAr;
	float L_alfaA;
	float L_alfaAr;
	float muA_on;
	float muA_off;
	float muA;
}BSS_PARAMETER;

//KHT BSS Buffer
typedef struct {
	C *Ga01;
	C *Ga10;

	float *LpGa01;
	float *LpGa10;

	float *LpX0;
	float *LpX1;

	float *LpXV0;
	float *LpXV1;

	float *ANGpXV0;
	float *ANGpXV1;
}BSS_BUFFER;


#endif