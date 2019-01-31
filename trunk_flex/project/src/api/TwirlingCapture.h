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


/* Capture engine. */
/**
	\brief initialize audio capture to get an object.
	\param [in] profileID   -  0: ambisonic array to wyzx (b format sn3d)
	                           1: broadside array to wyzx (b format sn3d)
							   2: circular array  to wyzx (b format sn3d)
							   3: object          to wyzx (b format sn3d)
							   4: stereo          to wyzx (b format sn3d)
							   5: 5.1             to wyzx (b format sn3d)
							   8: QB              to wyzx (b format sn3d)
	\param [in] frameLength - Length of one frame for processing, must be LESS or EQUAL to 512.
	\param [in] channelN    - Number of channels, 1 -- 128.
	\param [in] sRate       - Rate of sampling, must be 44100 or 48000.
	\param [in] multithread - Flag of turning on multi-threads whiling computing. 
	                          This is a performance parameter and the behavior 
							  depends on specific platform and processor. 
	\return The initialized object of audio capture or null pointer that means failed.
*/
void * captureInit(int profileID, int frameLength, int channelN, int sRate, bool multithread);

/**
	\brief set audio capture object.
	\param [in] obj         - The audio capture object.
	\param [in] dycOn       - Flag of turning on dynamic range control, default false.
	\param [in] dycMaxValue - Max input data value of dynamic range control.
	\param [in] formatId    - the format of mic array.
	
*/
void captureSet(void *obj, bool dycOn, float dycMaxValue, int formatId);

/**
	\brief process by audio capture object.
	NOTICE: We use the right-handed coordinate system, with the head at the origin of the coordinate, 
	facing the positive direction of the X-axis, the left ear facing the positive direction of the Y-axis 
	and the head is the positive direction of Z-axis. The head will be rotated by Euler angles as input 
	in fixed order HEADING - PITCH - BANK.
	\param [in]  obj            - The audio capture object.
	\param [in]  heading        - Heading angle of head in RADIAN measure, -PI -- PI, positive when head to left.
	\param [in]  pitch          - Pitch angle of head in RADIAN measure, -PI -- PI, positive when head down.
	\param [in]  bank           - Bank angle of head in RADIAN measure, -PI -- PI, positive when head to right.
	\param [in]  audioInput     - Interlaced multi-channel input audio data buffer.
	                              Data in this buffer will not be modified during processing.
	\param [out] audioOutput    - Interlaced output audio data buffer.
	\param [in]  metaDataFrame  - Object audio position metadata buffer with length (3 * channelN).
								  Data in this buffer will not be modified during processing.
								  Format: r1, azi1, elv1, r2, azi2, elv2, ... where 
								  ri is distance of object i, must be NON-NEGATIVE,
								  azii is azimuth angle of object i in RADIAN measure, 
								  elvi is elevation angle of object i in RADIAN measure.
								  NOTICE: These values ??must correspond to our given coordinate system.
								  TIP: Suppose an object is at (x, y, z) in our given coordinate system,
								  the corresponding metadata is 
								  dist = sqrt(x * x + y * y + z * z)
								  azi = atan2(y, x) or atan(y / x)
								  elv = asin(z / dist)
*/
void captureProcess(void *obj, float heading, float pitch, float bank, const float *audioInput, float *audiolOutput, const float *metaDataFrame);

/**
	\brief release audio capture object.
	\param [in] obj - The audio capture object to be released.
*/
void captureRelease(void *obj);


/* CaptureStereo engine. */
/**
\brief initialize stereo audio capture to get an object.
\param [in] frameLength   - Length of one frame for processing, must be LESS or EQUAL to 512.
\param [in] channelN      - Number of channels.
\param [in] sRate         - Rate of sampling, must be 44100 or 48000.
\return The initialized object of stereo audio capture or null pointer that means failed.
*/
void * captureStereoInit(int frameLength, int channelN, int sRate);

/**
\brief set stereo audio capture object.
\param [in] obj           - The stereo audio capture object.
\param [in] placementMode - The placement mode.
							0: The equipment is placed vertically and upward.
							1: The equipment is placed horizontally and left.
							2: The equipment is placed vertically and downward.
							3: The equipment is placed horizontally and right.
\param [in] cameraMode    - The camera mode.
							0: front
							1: back
*/
void captureStereoSet(void *obj, int placementMode, int cameraMode);

/**
\brief process by stereo audio capture object.
\param [in]  obj            - The stereo audio capture object.
\param [in]  audioInput     - Interlaced multi-channel input audio data buffer.
							  Data in this buffer will not be modified during processing.
\param [out] audioOutput    - Interlaced output audio data buffer.
*/
void captureStereoProcess(void *obj, const float *audioInput, float *audiolOutput);

/**
\brief release stereo audio capture object.
\param [in] obj - The stereo audio capture object to be released.
*/
void captureStereoRelease(void *obj);


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
	\param [in] maxSupDB - Max suppressed dB, -40 -- -3.
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



/* Beamforming. */

/**
	\brief initialize Beamforming to get an instance.
	\param [in] profileID   - mic array profile:  
	                          0: ambisonic
							  1: broadside;
							  2: circular
	\param [in] frameLength - Length of one frame for processing, must be LESS or EQUAL to 512.
	\param [in] channelN    - Number of channels.
	\param [in] sampleRate  - Sampling Rate.
	\param [in] micDist     - The distance of the broadside mic array.
	\param [in] multithread - Flag of turning on multi-threads whiling computing. 
	                          This is a performance parameter and the behavior
                              depends on specific platform and processor.
	\return The initialized instance of Beamforming or null pointer that means failed.
*/
void * beamformingInit(int profileID, int frameLength, int channelN, int sampleRate, float micDist, bool multithread);

/**
	\brief set Beamforming instance.
	\param [in] obj              - The Beamforming instance.
	\param [in] mode             - 1: beamforming;   2: beamforming+ns;  3: beamforming+ns+nlp
	\param [in] maxSupDB         - Max suppressed dB, -20 -- -6.
	\param [in] targetDirection  - Target signal direction in azimuth angle, 0~180 for broadside array, 0~180 for ambisonic array
	\param [in] targetEstimation - Estimation signal direction in azimuth angle, 0~180 for broadside array, 0~180 for ambisonic array
	\param [in] nlpSup           - The nlp suppress level. the scope is 0.5 -- 2.0 for ambisonic mode and 0.0 -- 1.0 for braodside mode. 
	\param [in] mainlobeWidth    - The width of mainlobe of target direction. 
*/
void beamformingSet(void *obj, int mode, float maxSupDB, float targetDirection, float targetEstimation, float nlpSup, float mainlobeWidth, int targetFlag);

/**
	\brief process by Beamforming instance.
	\param [in]  obj         - The Beamforming instance.
	\param [in]  audioInput  - Interlaced multi-channel input audio data buffer.
						       Data in this buffer will not be modified during processing.
	\param [out] audioOutput - Interlaced multi-channel output audio data buffer.	             
*/
void beamformingProcess(void *obj, const float *audioInput, float *audioOutput);

/**
	\brief release Beamforming instance.
	\param [in] obj - The Beamforming instance to be released.
*/
void beamformingRelease(void *obj);



//suggested parameters value setting
//  int sysDelay = 0;
//	bool enableRes = true;
//	float resLevel = 1.0;
//	bool enableNS = true;
//	float nsDB = -10.0;
//	bool enableSpkClip = false;
//	float spkClipThd = 1.0;   
//	float maxCoupling = 10.0;
//	float adaptStep =0.5;
//  int filterLength = 4096;
//  int frameLength = 256;


#if 1
/* Acoustic Echo Cancellation */

/**
	\brief initialize Aec to get an instance.
    \param [in] frameLength   - Length of one frame for processing, must be LESS or EQUAL to 512.
	\param [in] filterLength  - Length of the filter, must be 2^n, equal or greater than 2048;
	\param [in] channelN      - Number of the audio signal, must be set to 1 for this version..
	\param [in] sRate         - sample Rate. support all.	
	\param [in] sysDelay      - set it to 0 unless you know the amount of system latency that can be compensated.
	\param [in] micN          - number of mic.
	\param [in] spkN          - number of speaker.
	\param [in] multithread   - Flag of turning on multi-threads whiling computing, must be false for this version.
	                           This is a performance parameter and the behavior
                               depends on specific platform and processor.
	\return The initialized object of Aec or null pointer that means failed.
*/
void * aecInit(int frameLength, int filterLength, int sRate, int sysDelay, int micN, int spkN, bool multithread);

/**
	\brief set audio aec object.
	\param [in] obj            - aec instance pointer.
	\param [in] enableRes      - Flag of enable residual echo suppression.
	\param [in] resLevel       - 0~10, suppress more residual echo if you set this value high. The downside is possible near end voice distortion.
	\param [in] enableNS       - Flag of enable noise suppression.
	\param [in] nsDB           - maximum noise suppression in DB. -6dB ~ -20dB.
	\param [in] enableSpkClip  - Flag of speaker clipping control, set it false unless you know the speaker nolinear clip character.
	\param [in] spkClipThd     - 0~1.
	\param [in] maxCoupling    - max coupling factor between spk to mic. 0.5~20
	\param [in] adaptSetp      - adaptive filter step size. 0.1~1.0
*/
void aecSet(void *obj, bool enableRes, float resLevel, bool enableNS, float nsDB, bool enableSpkClip, float spkClipThd, float maxCoupling, float adaptStep);

/**
	\brief process by Aec object.
	\param [in] obj          - aec instance pointer.
	\param [in] audioSpk     - normalized speaker input audio pcm sample sequence. (value: -1~1)
	\param [in/out] audioMic - normalized mic input audio pcm sample sequence. (value: -1~1)
							   also normalized output audio pcm sample sequence.  (value: -1~1)
	                           pcm sample sequence length is 256.
*/
void aecProcess(void *obj, const float *audioSpk, float *audioMic);

/**
	\brief release Aec instance.
	\param [in] obj - The Aec instance to be released.
*/
void aecRelease(void *obj);
#endif

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


/* Automatic gain control - AGC. */

/**
\brief Initialize AGC to get an object.
\param [in] frameLength - Length of one frame for processing, must be LESS or EQUAL to 512.
\param [in] channelN    - Number of channels.
\param [in] sampleRate       - Rate of sampling.
\param [in] threshold - maximum range of the output audio, normally it is set as 1.0f
\return The initialized object of AGC or null pointer that means failed.
*/
void * agcInit(int frameLength, int channelN, int sampleRate, float threshold);

/**
\brief set property for AGC object.
\param [in] obj         - The AGC object.
\param [in] formatid    - 1 means mono, 2 means stereo, do not support other format so far
*/
void agcSet(void *obj, int formatid);

/**
\brief Process by AGC object.
\param [in]  obj         - The AGC object.
\param [in]  audioInput  - Interlaced multi-channel input audio data buffer.
Data in this buffer will not be modified during processing.
\param [out] audioOutput - Interlaced multi-channel output audio data buffer.
NOTICE: It is also ALLOWED audioInput = audioOutput (in-place).
\param [in]  params     - Parameter of processing with buffer length 3*sizeof(float),
params[0] is the agc_gain, params[1] is the vad_flag, params[2] is the reserved(0.f).
Data in this buffer will be modified during processing.
*/
void agcProcess(void *obj, const float *audioInput, float *audioOutput, float *params);

/**
\brief Release AGC object.
\param [in] obj - The AGC object to be released.
*/
void agcRelease(void * obj);


/* Blind source seperation - BSS. */

/**
\brief Initialize BSS to get an object.
\param [in] frameLength - Length of one frame for processing, must be LESS or EQUAL to 512.
\param [in] channelN    - Number of channels. NOW only support 2 channels!
\param [in] sampleRate       - Rate of sampling.
\param [in] multithread - true for multi-thread, false for single-thread
\return The initialized object of BSS or null pointer that means failed.
*/
void * bssInit(int frameLength, int channelN, int sampleRate, bool multithread);

/**
\brief set property for BSS object. Reserved for future, do nothing now.
\param [in] obj         - The BSS object.
\param [in] formatid    - reserved for future, do nothing now.
*/
void bssSet(void *obj, int formatid);

/**
\brief Process by BSS object.
\param [in]  obj         - The BSS object.
\param [in]  audioInput  - Interplaced multi-channel input audio data buffer.
Data in this buffer will not be modified during processing.
\param [out] audioOutput - Interlaced multi-channel output audio data buffer.
NOTICE: It is also ALLOWED audioInput = audioOutput (in-place).
*/
void bssProcess(void *obj, const float *audioInput, float *audioOutput);

/**
\brief Release BSS object.
\param [in] obj - The BSS object to be released.
*/
void bssRelease(void * obj);


void * drcInit(int frameLength, int channelN, float maxV);

void drcSet(void *obj, float maxV);

void drcProcess(void *obj, const float *audioInput, float *audioOutput);

void drcRelease(void *obj);

#ifdef __cplusplus				
};
#endif

#endif /*__AUDIO_CAPTURE_API__*/
