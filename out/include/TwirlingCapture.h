#ifndef __AUDIO_CAPTURE_API__
#define __AUDIO_CAPTURE_API__

#ifdef __cplusplus
extern "C"{
#endif


/* Mic Array Process */
/** 
	\brief Initialize mic array process to get an object.
	\param [in] profileId       - 0: ambisonic
								  1: broadside
								  2: circular
	\param [in] frameLength     - Length of one frame for processing.
	\param [in] sampleRate      - Rate of sampling, must be 16000, 32000, 44100 or 48000.
	\param [in] micChannelNum   - Number of the input audio signal.
	\param [in] micDist         - the distance or the radius of the mic array.
	\param [in] refChannelNum   - Number of the reference audio signal.
	\param [in] aecSysLatency   - Set it to 0 unless you know the amount of system latency that can be compensated.
	\return the initialized object of mic array process or null pointer that means failed.
*/
void * micArrayInit(int profileId, int frameLength, int sampleRate, int micChannelNum, float micDist, int refChannelNum, int aecSysLatency);

/**
	\brief process of mic array.
	\param [in]  obj             - The object of mic array porcess.
	\param [in]  audioInput      - Interlaced multi-channel input audio data buffer.				
	\param [out] audioOutput     - The output audio data buffer.
	\param [in]  audioReference  - The reference audio data buffer used for AEC.
*/
void micArrayProcess(void *obj, float *audioInput, float *audioOutput, float *audioReference);

/**
	\brief release mic array process object.
	\param [in] obj - The object of mic array porcess to be released.
*/
void micArrayRelease(void *obj);

/**
	\brief set the status.
	\param [in] obj         - The mic array process object.
	\param [in] status      - The status, it is set to 0 without wakeup and 1 with wakeup.
*/
void micArraySetStatus(void *obj, int status);

/**
	\brief set the parameters for the AEC.
	\param [in] obj            - The object of mic array porcess.
	\param [in] enableRes      - Flag of enable residual echo suppression.
	\param [in] resLevel       - 0~10, suppress more residual echo if you set this value high. The downside is possible near end voice distortion.
	\param [in] enableNS       - Flag of enable noise suppression.
	\param [in] nsDB           - Maximum noise suppression in dB. -6dB~-20dB.
	\param [in] enableSpkClip  - Flag of speaker clipping control, set it false unless you know the speaker nolinear clip character.
	\param [in] spkClipThd     - 0~1.
	\param [in] maxCoupling    - Max coupling factor between spk and mic. 0.5~20
	\param [in] adaptSetp      - Adaptive filter step size. 0.1~1.0
*/
void micArraySetAec(void *obj, bool enableRes, float resLevel, bool enableNS, float nsDB, bool enableSpkClip, float spkClipThd, float maxCoupling, float adaptStep);

/**
	\brief set the parameters for the beamforming.
	\param [in] obj              - The object of mic array porcess.
	\param [in] mode             - 1: beamforming;   3:super-beamforming;   5: beamforming+ns;  7: super-beamforming+ns
	\param [in] maxSupDB         - Max suppressed dB, -20~-6 dB.
	\param [in] targetDirection  - Target signal direction in azimuth angle, -90~90 for broadside array, -180~180 for ambisonic array and circular array
	\param [in] targetEstimation - Estimation signal direction in azimuth angle, -90~90 for broadside array, -180~180 for ambisonic array and circular array
	\param [in] nlpSup           - The nlp suppress level. the scope is 0.5~2.0 for ambisonic array and 0.0~1.0 for broadside and circular array.
	\param [in] mainlobeWidth    - The mainlobe width of target direction. 
*/
void micArraySetBeamforming(void *obj, int mode, float maxSupDB, float targetDirection, float targetEstimation, float nlpSup, float mainlobeWidth, int targetFlag);

/**
	\brief get the current doa.
	\param [in] obj              - The mic array process object.
	\return the current doa value.
**/
float micArrayGetCurrentDoa(void *obj);

/**
	\brief set the current direction for beamforming.
	\param [in] obj               - The mic array process object.
	\param [out] targetDirection  - The target direction.
**/
void micArraySetTargetDoa(void *obj, float targetDirection);

/* Ans. */

/**
	\brief initialize Ans to get an object.
	\param [in] frameLength - Length of one frame for processing, must be LESS or EQUAL to 512.
	\param [in] channelN    - Number of channels.
	\param [in] maxSupDB    - Max suppressed dB, -20 -- -6.
	\param [in] multithread - Flag of turning on multi-threads whiling computing.
							  This is a performance parameter and the behavior
							  depends on specific platform and processor.
	\return The initialized object of Ans or null pointer that means failed.
*/
void * ansInit(int frameLength, int channelN, int sampleRate, float maxSupDB, bool multithread);

/**
	\brief set Ans object.
	\param [in] obj      - The Ans object.
	\param [in] maxSupDB - Max suppressed dB, -20 -- -6.
*/
void ansSet(void *obj, float maxSupDB);

/**
	\brief process by Ans object.
	\param [in]  obj         - The Doa object.
	\param [in]  audioInput  - Interlaced multi-channel input audio data buffer.
							   Data in this buffer will not be modified during processing.
	\param [out] audioOutput - Interlaced multi-channel output audio data buffer.
	NOTICE: It is also ALLOWED audioInput = audioOutput (in-place).
*/
void ansProcess(void *obj, const float *audioInput, float *audioOutput);

/**
	\brief release Ans object.
	\param [in] obj - The Ans object to be released.
*/
void ansRelease(void *obj);


/**
	\brief initialize VAD to get an object.
	\param [in] frameLength   - Length of one frame for processing, must be LESS or EQUAL to 512.
	\param [in] channelN      - Number of the audio signal.
	\param [in] sampleRate    - Rate of sampling, must be 16000,32000, 44100 or 48000.
	\return The initialized object of VAD or null pointer that means failed.
*/
void * vadInit(int frameLength, int channelN, int sampleRate);

/**
	\brief process by VAD object.
	\param [in]  obj         - The VAD object.
	\param [in]  objectInput - Interlaced multi-channel input audio data buffer.
                               Data in this buffer will not be modified during processing.
	\The returned value will be 1 or 0, means voice or noise.
*/
float vadProcess(void *obj, const float *objectInput);

/**
	\brief getState by VAD object.
	\param[in]  obj         - The VAD object.
	\param[in]  objectInput - Interlaced multi - channel input audio data buffer.
                          Data in this buffer will not be modified during processing.
	\return: 1 means voice start, 2 means voice in progress, 3 means end of the voice
			 for the first time, 1 means the start of the voice(one sentence)
             for the first time, 3 means the end of the voice(one sentence)
**/
int vadGetState(void *obj, const float *objectInput);


/**
\brief release VAD object.
\param [in] obj - The VAD object to be released.
*/
void vadRelease(void *obj);

/* Equalizer. */

/**
	\brief initialize equalizer to get an object.
	\param [in] frameLength - Length of one frame for processing, must be LESS or EQUAL to 512.
	\param [in] channelN    - Number of channels.
	\param [in] sRate       - Rate of sampling.
	\param [in] multithread - Flag of turning on multi-threads whiling computing.
	                          This is a performance parameter and the behavior
                              depends on specific platform and processor.
	\return The initialized object of equalizer or null pointer that means failed.
*/
void * eqInit(int frameLength, int channelN, int sRate, bool multithread);

/**
	\brief process by equalizer object.
	\param [in]  obj         - The equalizer object.
	\param [in]  audioInput  - Interlaced multi-channel input audio data buffer.
                               Data in this buffer will not be modified during processing.
	\param [out] audioOutput - Interlaced multi-channel output audio data buffer.
						       NOTICE: It is also ALLOWED audioInput = audioOutput (in-place).
	\param [in]  eqParam     - Parameter of processing with buffer length 10,
							   each one is of range -12 -- 12dB, 0 as default.
                               Hz： 31, 62, 125, 250, 500, 1000, 2000, 4000, 8000, 16000
                               Data in this buffer will not be modified during processing.
*/
void eqProcess(void *obj, const float *audioInput, float *audioOutput, const float *eqParam);

/**
	\brief release equalizer object.
	\param [in] obj - The equalizer object to be released.
*/
void eqRelease(void * obj);

/* Resampler. */
/**
	\brief initialize resampler to get an object.
	\param [in] channelN    - Number of channels, 1 -- 128.
	\param [in] frameLength - The expected Length of one frame after the resampler Processing(i.e. 256 for 16k sample rate)
	\param [in] sRate       - The expected Rate of sampling, support 16000, 32000, 44100 or 48000.
	\return The initialized object of resampler or null pointer that means failed.
*/
void * resamplerInit(int channelN, int frameLength, int sRate);

/**
	\brief set resampler object.
	\param [in] obj         - The resampler object.
*/
void resamplerSet(void *obj);

/**
	\brief process by resampler object.
	\param [in]  obj             - The resampler object.
	\param [in]  input           - The input data buffer.
	\param [out] output          - The output data buffer.
	\param [in]  channelN        - Number of channels, 1 -- 128.
	\param [in]  frameLength     - input data frameLength for this frame.
	\param [in]  sRate           - the expected Rate of sampling, support 16000,22050,32000, 44100 or 48000.
*/
void resamplerProcess(void *obj, float *input, float *output, int channelN, int framelen, int sRate);

/**
	\brief release resampler object.
	\param [in] obj - The resampler object to be released.
*/
void resamplerRelease(void *obj);

void * drcInit(int frameLength, int channelN, float maxV);

void drcSet(void *obj, float maxV);

void drcProcess(void *obj, const float *audioInput, float *audioOutput);

void drcRelease(void *obj);

/**
	\brief initialize Doa to get an object.
	\param [in] profileID     - 0: ambisonic
	                            1: broadside
								2: circular
	\param [in] frameLength   - Length of one frame for processing, must be LESS or EQUAL to 512.
	\param [in] channelN      - Number of the audio signal.
	\param [in] sample_rate   - Rate of sampling, must be 16000,32000, 44100 or 48000.
	\param [in] micDist       - The distance of the broadside mic array or the radius of circular mic array.
	\param [in] multithread   - Flag of turning on multi-threads whiling computing.
								This is a performance parameter and the behavior
								depends on specific platform and processor.
	\return The initialized object of Doa or null pointer that means failed.
*/
void * doaInit(int profileID, int frameLength, int channelN, int sample_rate, float micDist, bool multithread);

/**
	\brief set audio doa object.
	\param [in] obj         - The doa object.
	\param [in] formatId    - The format of mic array.
*/
void doaSet(void *obj, int formatId);

/**
	\brief process by Doa object.
	\param [in]  obj         - The Doa object.
	\param [in]  objectInput - Interlaced multi-channel input audio data buffer.
	                           Data in this buffer will not be modified during processing.
	\param [out] param       - Sound parameters buffer with length 4 stores
							   sound location, sound power, sound diffusivity , sound reliability, vad result and so on.
*/
void doaProcess(void *obj, const float *objectInput, float *param);

/**
	\brief release Doa object.
	\param [in] obj - The Doa object to be released.
*/
void doaRelease(void *obj);

#ifdef __cplusplus				
};
#endif

#endif /*__AUDIO_CAPTURE_API__*/
