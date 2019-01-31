#ifndef UPMIX51_H
#define UPMIX51_H

class CUpmix51
{
public:

    CUpmix51(int FrameLength,  int sRate);
    ~CUpmix51();

    void init (int FrameLength, int sRate);
    void process(const float *stereoInput, float *upmix51Output);
	float firfilter(float *buf, int pos, int len, int count, float *coefficients);


private:

	int audioFrameLength;               // sample number of each audio frame

	typedef struct pl_surround_s
	{
	  int passthrough;      // Just be a "NO-OP"
	  int msecs;            // Rear channel delay in milliseconds
	  int c_msecs;            // center channel delay in milliseconds
	  float* databuf;     // Output audio buffer
	  float* Ls_delaybuf; // circular buffer to be used for delaying Ls audio
	  float* Rs_delaybuf; // circular buffer to be used for delaying Rs audio
	  int delaybuf_len;     // delaybuf buffer length in samples
	  int delaybuf_pos;     // offset in buffer where we are reading/writing
	  float* c_delaybuf; // circular buffer to be used for delaying c audio
	  int c_len;     // delaybuf buffer length in samples
	  int c_pos;     // offset in buffer where we are reading/writing
	  float* filter_coefs_surround; // FIR filter coefficients for surround sound 7kHz lowpass
	  int rate;             // input data rate
	  int format;           // input format
	  int input_channels;   // input channels
	  int output_channels;

	} pl_surround_t;

	pl_surround_t pl_surround;
};


#endif