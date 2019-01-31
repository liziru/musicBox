#include "upmix51.h"
#include <string.h>
#include <math.h>
#include "stdio.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#define SPLITREAR 1
#define CHANNEL51 6
#define FILTER_LEN 31

static float steering_matrix[14] = {
//	LL	      RL	LR	   RR	    LS	    RS	     LLs	    RLs	        LRs	        RRs	   LC	    RC	     LLFE   RLFE
    0.707f,   0.0f,	0.0f,  0.707f,  0.5f,	-0.5f, 0.5773f,  -0.4081f,	0.4081f,	-0.5773f,  0.5f,	0.5f,  0.0f,   0.0f
};

static float filterCoeff[FILTER_LEN] =
{
	 0.0001387 , 
	 0.0008528 , 
	 0.0008020 , 
	-0.0021899 , 
	-0.0064332 , 
	-0.0051270 , 
	 0.0061806 , 
	 0.0197553 , 
	 0.0174794 , 
	-0.0108190 , 
	-0.0477017 , 
	-0.0509674 , 
	 0.0144925 , 
	 0.1396577 , 
	 0.2650255 , 
	 0.3177071 , 
	 0.2650255 , 
	 0.1396577 , 
	 0.0144925 , 
	-0.0509674 , 
	-0.0477017 , 
	-0.0108190 , 
	 0.0174794 , 
	 0.0197553 , 
	 0.0061806 , 
	-0.0051270 , 
	-0.0064332 , 
	-0.0021899 , 
	 0.0008020 , 
	 0.0008528 , 
	 0.0001387 
};

CUpmix51::CUpmix51(int FrameLength,  int sRate)
{
    init( FrameLength, sRate);
}

CUpmix51::~CUpmix51()
{
	delete pl_surround.Ls_delaybuf;
	delete pl_surround.Rs_delaybuf;
	delete pl_surround.c_delaybuf;
	pl_surround.delaybuf_len=0;
	pl_surround.delaybuf_pos=0;
	pl_surround.c_len=0;
	pl_surround.c_pos=0;
    
}

	
void CUpmix51::init (int FrameLength,  int sRate)
{
	int i,j, n;
    
	// get input audio sampling rate and sample length
	audioFrameLength = FrameLength;		

	pl_surround.passthrough = 0;

	/* Store info on input format to expect */
	pl_surround.rate=sRate;
	pl_surround.format=0;
	pl_surround.input_channels=2;
	pl_surround.output_channels=CHANNEL51;
	pl_surround.msecs = 20;
	pl_surround.c_msecs = 5;

	// Figure out buffer space  needed for the 20msec delay
	pl_surround.delaybuf_len = (pl_surround.rate * pl_surround.msecs / 1000);
	pl_surround.c_len = (pl_surround.rate * pl_surround.c_msecs / 1000);
	// Allocate delay buffers
	pl_surround.Ls_delaybuf = new float[pl_surround.delaybuf_len];
	memset(pl_surround.Ls_delaybuf, 0x00, sizeof(float) * pl_surround.delaybuf_len);
	pl_surround.Rs_delaybuf=new float[pl_surround.delaybuf_len];
	memset(pl_surround.Rs_delaybuf, 0x00, sizeof(float) * pl_surround.delaybuf_len);
	pl_surround.c_delaybuf=new float[pl_surround.c_len];
	memset(pl_surround.c_delaybuf, 0x00, sizeof(float) * pl_surround.c_len);
	//printf("pl_surround: %dmsec surround delay, rate %d - buffers are %d float each\n", pl_surround.msecs,pl_surround.rate,  pl_surround.delaybuf_len);
	pl_surround.delaybuf_pos = 0;
	pl_surround.c_pos = 0;
	// Surround filer coefficients
	pl_surround.filter_coefs_surround = filterCoeff; //calc_coefficients_7kHz_lowpass(pl_surround.rate);
	//dump_filter_coefficients(pl_surround.filter_coefs_surround);
	//testfilter(pl_surround.filter_coefs_surround, 32, pl_surround.rate);

}

// Update is called once every frame
void CUpmix51::process(const float *stereoInput, float *upmix51Output)
{		
	int i;
	float *matrix = steering_matrix; // later we'll index based on detected dominance
	float *out = upmix51Output;
	const float *in = stereoInput;

	for (i=0; i<audioFrameLength; i++) {

		// output front left and right
		out[0] = matrix[0]*in[0] + matrix[1]*in[1];
		out[1] = matrix[2]*in[0] + matrix[3]*in[1];
		out[2] = firfilter(pl_surround.c_delaybuf, pl_surround.c_pos,
				   pl_surround.c_len, FILTER_LEN, pl_surround.filter_coefs_surround);
		out[3] = matrix[12]*in[0] + matrix[13]*in[1];
		// output Ls and Rs - from 20msec ago, lowpass filtered @ 7kHz
		out[4] = firfilter(pl_surround.Ls_delaybuf, pl_surround.delaybuf_pos,
				   pl_surround.delaybuf_len, FILTER_LEN, pl_surround.filter_coefs_surround);
	#ifdef SPLITREAR
		out[5] = firfilter(pl_surround.Rs_delaybuf, pl_surround.delaybuf_pos,
				   pl_surround.delaybuf_len, FILTER_LEN, pl_surround.filter_coefs_surround);
	#else
		out[5] = -out[4];
	#endif
		// calculate and save surround for 20msecs time
	#ifdef SPLITREAR
		pl_surround.Ls_delaybuf[pl_surround.delaybuf_pos] =
		  matrix[6]*in[0] + matrix[7]*in[1];
		pl_surround.Rs_delaybuf[pl_surround.delaybuf_pos++] =
		  matrix[8]*in[0] + matrix[9]*in[1];
	#else
		pl_surround.Ls_delaybuf[pl_surround.delaybuf_pos++] =
		  matrix[4]*in[0] + matrix[5]*in[1];
	#endif
		pl_surround.delaybuf_pos %= pl_surround.delaybuf_len;

		pl_surround.c_delaybuf[pl_surround.c_pos++] =
		  matrix[10]*in[0] + matrix[11]*in[1];
		pl_surround.c_pos %= pl_surround.c_len;		

		// next samples...
		in = &in[pl_surround.input_channels];  
		out = &out[6];
	}

}


float CUpmix51::firfilter(float *buf, int pos, int len, int count, float *coefficients)
{
  float result = 0.0;
  int count1, count2;
  float *ptr;

  if (pos >= count) {
    pos -= count;
    count1 = count; count2 = 0;
  }
  else {
    count2 = pos;
    count1 = count - pos;
    pos = len - count1;
  }
  //fprintf(stderr, "pos=%d, count1=%d, count2=%d\n", pos, count1, count2);

  // high part of window
  ptr = &buf[pos];
  while (count1--)  result += (*ptr++) * (*coefficients++);
  // wrapped part of window
  while (count2--)  result += (*buf++) * (*coefficients++);
  return result;
}