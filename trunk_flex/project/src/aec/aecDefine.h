//***************************************************************************
//
//           Macro and global variables definition for acoustic echo cancellation.
//       
//***************************************************************************
#ifndef AEC_DEFINE_H
#define AEC_DEFINE_H

#include "commonDefine.h"
#include "resDefine.h"
#include "../fft/realFFT.h"
//------------------------------------------------------------------------------------
// Macro defination
//------------------------------------------------------------------------------------

//DTD
#define DT_CONVERGE_THD 0.5f
#define DT_CONVERGE_PRD 5
#define DT_CONVERGE_PRD_MAX 200

#define DT_RATIO 0.5f
#define DT_RATIO_H 0.25f
#define TURN_ON  1
#define TURN_OFF 0

#define STEP1 1.0f
#define STEP2 0.5f
#define STEP3 0.1f
#define STEP4 0.01f

//FDLMS

//------------------------------------------------------------------------------------
//adaptive filter coefficients refresh parameters
//------------------------------------------------------------------------------------
#define ALPHA 0.5f  // power smooth factor, power up
#define BETA 0.05f  // power smooth factor, power down
#define MIN_POW (1e-8f) //min power to avoid divide zero
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
/* dB Values */
//------------------------------------------------------------------------------------
#define M0dB  1.0f
#define M3dB  0.71f
#define M6dB  0.50f
#define M9dB  0.35f
#define M12dB  0.25f
#define M18dB  0.125f
#define M20dB  0.100f
#define M24dB  0.063f
#define M30dB  0.0315f

/* dB values for 16bit PCM */
/* MxdB_PCM = 32767 * 10 ^(x / 20) */
#define MAXPCM     (32767.0f/32768.0f)
#define M10dB_PCM  (10362.0f/32768.0f)
#define M20dB_PCM  (3277.0f/32768.0f)
#define M25dB_PCM  (1843.0f/32768.0f)
#define M30dB_PCM  (1026.0f/32768.0f)
#define M35dB_PCM  (583.0f/32768.0f)
#define M40dB_PCM  (328.0f/32768.0f)
#define M45dB_PCM  (184.0f/32768.0f)
#define M50dB_PCM  (104.0f/32768.0f)
#define M55dB_PCM  (58.0f/32768.0f)
#define M60dB_PCM  (33.0f/32768.0f)
#define M65dB_PCM  (18.0f/32768.0f)
#define M70dB_PCM  (10.0f/32768.0f)
#define M75dB_PCM  (6.0f/32768.0f)
#define M80dB_PCM  (3.0f/32768.0f)
#define M85dB_PCM  (2.0f/32768.0f)
#define M90dB_PCM  (1.0f/32768.0f)
//-----------------------------------------------------------------------------------

typedef struct{
	float dfPow;
	float yfPow;
	float efPow;
}DtdGlobalData;

typedef struct{
	
	float *dBuf;
	float *xBuf;	
	float *yBuf;
	float *eBuf;
	C     *xSpec;
	float *xfBufPow;		
	float *yfBufRe;
	float *yfBufIm;	
	float *filterRe;  
	float *filterIm;  
	float *spkCircleBuf;
	float *spkBuf;

	int *spkRpt;
	int *spkWpt;

	float frameCnt;
	float stepAdj;

	int framelens;
	int fftlens_res;
	int sysDelay;
	bool enableRes; 
	float resLevel;
	bool enableNS;
	float nsDB;
	bool enableSpkClip; 
	float spkClipThd;
	float maxCoupling;
	float adaptStep;
	int samplerate;

	float *dwfPow;
	float *ywfPow;
	float *ewfPow;
	float *dwf;
	float *ywf;

	int dtdFlag;
	int vadFlag;
	int convergeFlag;
	int convergeCount;
	int convergeTimer;
	int firstFlag;

	ResGlobalData *resGlobal;
	DtdGlobalData dtdGlobal;
	
}AecGlobalData;


#endif