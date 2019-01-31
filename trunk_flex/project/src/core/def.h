#ifndef __AUDIO_ENGINE_DEF_H__
#define __AUDIO_ENGINE_DEF_H__

enum SampleRate_e 
{
	SAMPLE_RATE_16000    = 16000,
	SAMPLE_RATE_32000    = 32000,
	SAMPLE_RATE_44100    = 44100,
	SAMPLE_RATE_48000    = 48000
};
typedef SampleRate_e SampleRate;

struct AudioEngineCreateParameter_s 
{
	int profile_id;
	int frame_length;
	int object_number;
	SampleRate sample_rate;
	bool multithread;
};
typedef AudioEngineCreateParameter_s AudioEngineCreateParameter;

struct MetaData_s
{
	float dist;
	float azi;
	float elv;
};
typedef MetaData_s MetaData;

struct HeadRotation_s
{
	float heading;
	float pitch;
	float bank;
};
typedef HeadRotation_s HeadRotation;

struct AudioEngineProcessParameter_s
{
	HeadRotation head;
	const float *input;
	float *output; 
	const MetaData *meta;
};
typedef AudioEngineProcessParameter_s AudioEngineProcessParameter;

struct AudioEngineSettings_s
{
	bool dyc_on;
	float dyc_max_val;
	int formatid;
};
typedef AudioEngineSettings_s AudioEngineSettings;

struct AudioEngineDoaParameter_s
{
	const float *input;
	float *param;
	int channel;
};
typedef AudioEngineDoaParameter_s AudioEngineDoaParameter;

#endif /*__AUDIO_ENGINE_DEF_H__*/