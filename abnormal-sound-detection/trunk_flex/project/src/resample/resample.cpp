/* 16kHz->48kHz, 32kHz->48kHz, 48kHz->16kHz, 48kHz->32kHz*/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "resample.h"

const float *filter;

static const float filter_LP12_180H[] =   /* Sincfilt.m: N=180*2+1, Fmin=0, Fmax=1/12, hann( N )' */
{
    1.00000000f,  0.98854064f,  0.95463880f,  0.89969939f,  0.82598609f,  0.73650898f,
    0.63487604f,  0.52511631f,  0.41148461f,  0.29825804f,  0.18953518f,  0.08904855f,
    0.00000000f, -0.07507283f, -0.13439242f, -0.17699551f, -0.20274380f, -0.21229157f,
    -0.20701353f, -0.18889782f, -0.16041129f, -0.12434529f, -0.08365114f, -0.04127482f,
    -0.00000000f,  0.03769212f,  0.06973903f,  0.09458356f,  0.11122751f,  0.11925054f,
    0.11879487f,  0.11051837f,  0.09552022f,  0.07524478f,  0.05137065f,  0.02569204f,
    0.00000000f, -0.02402919f, -0.04493222f, -0.06153821f, -0.07302535f, -0.07895262f,
    -0.07926575f, -0.07427839f, -0.06463103f, -0.05123120f, -0.03517999f, -0.01768990f,
    -0.00000000f,  0.01670619f,  0.03137493f,  0.04314423f,  0.05139005f,  0.05575472f,
    0.05615642f,  0.05278011f,  0.04605151f,  0.03659641f,  0.02518906f,  0.01269314f,
    0.00000000f, -0.01203202f, -0.02263294f, -0.03116789f, -0.03717261f, -0.04037571f,
    -0.04070711f, -0.03829248f, -0.03343501f, -0.02658610f, -0.01830764f, -0.00922872f,
    -0.00000000f,  0.00875109f,  0.01646140f,  0.02266671f,  0.02702792f,  0.02934764f,
    0.02957617f,  0.02780739f,  0.02426498f,  0.01928068f,  0.01326622f,  0.00668131f,
    0.00000000f, -0.00632223f, -0.01187840f, -0.01633509f, -0.01945125f, -0.02108964f,
    -0.02122066f, -0.01991853f, -0.01735069f, -0.01376125f, -0.00945018f, -0.00474975f,
    -0.00000000f,  0.00447492f,  0.00838805f,  0.01150721f,  0.01366775f,  0.01478001f,
    0.01483115f,  0.01388152f,  0.01205626f,  0.00953282f,  0.00652562f,  0.00326903f,
    0.00000000f, -0.00305850f, -0.00571204f, -0.00780638f, -0.00923565f, -0.00994662f,
    -0.00993903f, -0.00926212f, -0.00800799f, -0.00630232f, -0.00429337f, -0.00214002f,
    -0.00000000f,  0.00198116f,  0.00367946f,  0.00499962f,  0.00587975f,  0.00629327f,
    0.00624819f,  0.00578397f,  0.00496633f,  0.00388057f,  0.00262394f,  0.00129781f,
    0.00000000f, -0.00118189f, -0.00217597f, -0.00292996f, -0.00341330f, -0.00361747f,
    -0.00355476f, -0.00325545f, -0.00276400f, -0.00213446f, -0.00142560f, -0.00069606f,
    -0.00000000f,  0.00061651f,  0.00111820f,  0.00148212f,  0.00169815f,  0.00176842f,
    0.00170582f,  0.00153178f,  0.00127370f,  0.00096205f,  0.00062756f,  0.00029879f,
    -0.00000000f, -0.00025029f, -0.00044009f, -0.00056414f, -0.00062342f, -0.00062426f,
    -0.00057701f, -0.00049453f, -0.00039069f, -0.00027889f, -0.00017088f, -0.00007586f,
    -0.00000000f,  0.00005374f,  0.00008534f,  0.00009723f,  0.00009358f,  0.00007948f,
    0.00006013f,  0.00004011f,  0.00002289f,  0.00001046f,  0.00000327f,  0.00000042f,
    0.00000000f
};

static const float filter_LP3_90H[] =   /* Sincfilt.m: N=90*2+1, Fmin=0, Fmax=1/3, hann( N )' */
{
    1.00000000f,  0.82674145f,  0.41299304f,  0.00000000f, -0.20574231f, -0.16414228f,
    -0.00000000f,  0.11638725f,  0.10137190f,  0.00000000f, -0.08020564f, -0.07244401f,
    -0.00000000f,  0.06039577f,  0.05561375f,  0.00000000f, -0.04776011f, -0.04448829f,
    -0.00000000f,  0.03891245f,  0.03651267f,  0.00000000f, -0.03231551f, -0.03046677f,
    -0.00000000f,  0.02717149f,  0.02569502f,  0.00000000f, -0.02302575f, -0.02181436f,
    -0.00000000f,  0.01960070f,  0.01858630f,  0.00000000f, -0.01671751f, -0.01585488f,
    -0.00000000f,  0.01425599f,  0.01351396f, -0.00000000f, -0.01213249f, -0.01148889f,
    -0.00000000f,  0.01028699f,  0.00972562f,  0.00000000f, -0.00867534f, -0.00818410f,
    -0.00000000f,  0.00726427f,  0.00683387f, -0.00000000f, -0.00602813f, -0.00565135f,
    -0.00000000f,  0.00494677f,  0.00461782f,  0.00000000f, -0.00400399f, -0.00371817f,
    -0.00000000f,  0.00318651f,  0.00293988f, -0.00000000f, -0.00248317f, -0.00227240f,
    -0.00000000f,  0.00188444f,  0.00170665f,  0.00000000f, -0.00138200f, -0.00123460f,
    -0.00000000f,  0.00096839f,  0.00084908f, -0.00000000f, -0.00063685f, -0.00054348f,
    0.00000000f,  0.00038113f,  0.00031171f,  0.00000000f, -0.00019534f, -0.00014798f,
    -0.00000000f,  0.00007391f,  0.00004679f, -0.00000000f, -0.00001145f, -0.00000283f,
    -0.00000000f
};


CResample::CResample(int iChan,int framelen, int sRate)
{
	initial(iChan,framelen,sRate);
}

CResample::~CResample()
{
	delete []signal_tab_up;
	delete []signal_tab_down;
	delete []audioInputData_up;
	delete []audioOutputData_up;
	delete []audioInputData_down;
	delete []audioOutputData_down;
	delete []mem_up;
	delete []mem_down;
}

void CResample::initial(int iChan,int framelen, int sRate)
{
	firstFlag = true;

	audioInputData_up = new float[iChan*framelen];
	memset(audioInputData_up, 0, (iChan*framelen)*sizeof(float));

	audioOutputData_up = new float[iChan*L_FRAME];
	memset(audioOutputData_up, 0, (iChan*L_FRAME)*sizeof(float));

	audioInputData_down = new float[iChan*L_FRAME];
	memset(audioInputData_down, 0, (iChan*L_FRAME)*sizeof(float));

	audioOutputData_down = new float[iChan*framelen];
	memset(audioOutputData_down, 0, (iChan*framelen)*sizeof(float));

	if(sRate == 16000)
	{
		filt_len_up = 15;
		filter = filter_LP12_180H;
	}
	else
	{
		filt_len_up = 30;
		filter = filter_LP3_90H;
	}
	filt_len_down = 45;

	mem_up = new float[iChan*2*filt_len_up];
	mem_down = new float[iChan*2*filt_len_down];

	signal_tab_up = new float[2*filt_len_up + framelen];
	memset(signal_tab_up, 0, (2*filt_len_up + framelen)*sizeof(float));

	signal_tab_down = new float[2*filt_len_down + L_FRAME];
	memset(signal_tab_down, 0, (2*filt_len_down + L_FRAME)*sizeof(float));

}

void CResample::resample(float *InputData, float *OutputData, int iChan, int framelen, int sRate)
{	
	int i, j, n;
	int filt_len;
	float *mem, *audioInputData, *audioOutputData;

	lg = framelen;

	if(firstFlag)
	{
		memset(mem_up, 0, iChan*2*filt_len_up*sizeof(float));
		memset(mem_down, 0, iChan*2*filt_len_down*sizeof(float));  
		firstFlag = false;
	}

	if(framelen == L_FRAME)  //dowmsample
	{
		filt_len = filt_len_down;
		mem = mem_down;
		audioInputData = audioInputData_down;
		audioOutputData = audioOutputData_down;
		signal_tab = signal_tab_down;

		if(sRate == 16000)
		{
			lg_out = L_FRAME_16;
			fac_num = 4;
            fac_den = 12;		
		}
		else
		{
			lg_out = L_FRAME_32;
			fac_num = 2;
            fac_den = 3;		
		}
	}
	else                //upsample
	{
		filt_len = filt_len_up;
		mem = mem_up;
		audioInputData = audioInputData_up;
		audioOutputData = audioOutputData_up;
		signal_tab = signal_tab_up;

		if(sRate == 16000)
		{		
			fac_num = 12;
            fac_den = 4;
		}
		else
		{
			fac_num = 3;
            fac_den = 2;
		}
		lg_out = L_FRAME;
	}
	
	n = 0;
	for(i=0;i<lg;i++)
	{                           
		for(j=0; j<iChan; j++)
		{
			audioInputData[j*lg+i] = InputData[n];
			n++;
		}
	}  

	for(j=0; j<iChan; j++)
	{
		modify_Fs(&audioInputData[j*lg], &audioOutputData[j*lg_out], &mem[j*2*filt_len], filt_len);
	}	

	n=0;
	for(i=0;i<lg_out;i++)
	{
		for(j=0;j<iChan;j++)
		{
			OutputData[n++] = audioOutputData[j*lg_out+i];
		}
	}
}
/*-------------------------------------------------------------------*
 * modify_Fs()
 *
 * Function for resampling of signals
 *-------------------------------------------------------------------*/

void CResample::modify_Fs(           
    float sigIn[],                   /* i  : signal to decimate  */
    float sigOut[],		             /* o  : decimated signal    */
    float mem[],                      /* i/o: filter memory       */
    int filt_len                     /* i  : filter length       */
)
{
    short i;
	float *signal, *sigIn_ptr;

	signal = signal_tab;

    /* append filter memory */
    for (i=0; i<2*filt_len; i++)
    {
        signal[i] = mem[i];
    }

    for (i=0; i<lg; i++)
    {
        signal[i+(2*filt_len)] = sigIn[i];
    }

    /* interpolation */
    datastep = fac_den / fac_num;
    fracstep = fac_den - datastep * fac_num; /* equivalent to fracstep = fac_den % fac_num */

    frac = 0;
	sigIn_ptr = signal + filt_len;
    for(i=0; i<lg_out; i++)
    {
        sigOut[i] = interpolation(sigIn_ptr, filter, frac, fac_num, filt_len);

        frac = frac + fracstep;
        if ( frac >= fac_num )
        {
            frac = frac - fac_num;
            sigIn_ptr++;
        }

        sigIn_ptr += datastep;
    }

	/* rescaling */
    if ( fac_num > fac_den )
    {
        //num_den = (float)fac_num / fac_den;

        //for( i=0; i<lg_out; i++ )
        //{
        //    sigOut[i] *= (float)1/num_den;
        //}
    }
	else if (fac_num < fac_den) //down-sample
	{
		num_den = (float)fac_den / fac_num;

		for (i = 0; i<lg_out; i++)
		{
			sigOut[i] *= (float)1 / num_den;
		}

	}

    /* update the filter memory */
    for (i=0; i<2*filt_len; i++)
    {
        mem[i] = signal[i+lg];
    }
}

/*-------------------------------------------------------------------*
 * interpolation()
 *
 * Fractional interpolation of signal at position (frac/resol)
 *-------------------------------------------------------------------*/

float CResample::interpolation(      /* o  : interpolated value   */
    float *x,             /* i  : input vector         */
    const float *win,     /* i  : interpolation window */
    short frac,           /* i  : fraction             */
    short up_samp,        /* i  : upsampling factor    */
    short nb_coef         /* i  : nb of filter coef    */
)
{
    short i;
    float s;
    float *x1, *x2;
	const float *c1, *c2;

    x1 = &x[0];
    x2 = &x[1];
    c1 = &win[frac];
    c2 = &win[up_samp-frac];
    s = 0.0f;

    for (i=0; i< nb_coef; i++ )
    {
        s += (*x1--) * (*c1) + (*x2++) * (*c2);
        c1 += up_samp;
        c2 += up_samp;
    }

    return s;
}