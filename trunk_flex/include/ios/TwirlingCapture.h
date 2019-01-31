#ifndef __AUDIO_CAPTURE_API__
#define __AUDIO_CAPTURE_API__

#ifdef __cplusplus
extern "C"{
#endif

/* Audio engine. */

/**
	\brief Initialize audio engine to get an object.
	\param [in] profileID   -  1: object to wyzx (b format sn3d)
	                           2: stereo to wyzx (b format sn3d)
							   3: 3mic   to wyzx (b format sn3d)
							   4: 4mic   to wyzx (b format sn3d)
							   5: 5.1    to wyzx (b format sn3d)
							   8: QB     to wyzx (b format sn3d)
	\param [in] frameLength - Length of one frame for processing, must be LESS or EQUAL to 512.
	\param [in] channelN    - Number of channels, 1 -- 128.
	\param [in] sRate       - Rate of sampling, must be 44100 or 48000.
	\param [in] multithread - Flag of turning on multi-threads whiling computing. 
	                          This is a performance parameter and the behavior 
							  depends on specific platform and processor. 
	\return The initialized object of audio engine or null pointer that means failed.
*/
void * captureInit(int profileID, int frameLength, int channelN, int sRate, bool multithread);

/**
	\brief Set audio engine object.
	\param [in] obj         - The audio engine object.
	\param [in] dycOn       - Flag of turning on dynamic range control, default false.
	\param [in] dycMaxValue - Max input data value of dynamic range control.
*/
void captureSet(void *obj, bool dycOn, float dycMaxValue);

/**
	\brief Process by audio engine object.
	NOTICE: We use the right-handed coordinate system, with the head at the origin of the coordinate, 
	facing the positive direction of the X-axis, the left ear facing the positive direction of the Y-axis 
	and the head is the positive direction of Z-axis. The head will be rotated by Euler angles as input 
	in fixed order HEADING - PITCH - BANK.
	\param [in]  obj            - The audio engine object.
	\param [in]  heading        - Heading angle of head in RADIAN measure, -PI -- PI, positive when head to left.
	\param [in]  pitch          - Pitch angle of head in RADIAN measure, -PI -- PI, positive when head down.
	\param [in]  bank           - Bank angle of head in RADIAN measure, -PI -- PI, positive when head to right.
	\param [in]  objectInput    - Interlaced multi-channel input audio data buffer.
	                              Data in this buffer will not be modified during processing.
	\param [out] binauralOutput - Interlaced binaural output audio data buffer.
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
void captureProcess(void *obj, float heading, float pitch, float bank, const float *objectInput, float *binauralOutput, const float *metaDataFrame);

/**
	\brief Release audio engine object.
	\param [in] obj - The audio engine object to be released.
*/
void captureRelease(void *obj);




/**
\brief Initialize Doa to get an object.
\param [in] frameLength   - Length of one frame for processing, must be LESS or EQUAL to 512
\param [in] multithread   - Flag of turning on multi-threads whiling computing.
This is a performance parameter and the behavior
depends on specific platform and processor.
\return The initialized object of Doa or null pointer that means failed.
*/
void * doaInit(int frameLength, bool multithread);

/**
\brief Process by Doa object.
\param [in]  obj         - The Doa object.
\param [in]  objectInput - Interlaced multi-channel input audio data buffer.
Data in this buffer will not be modified during processing.
\param [out] param       - Sound parameters buffer with length 4 stores
sound location, sound power, sound diffusivity and sound reliability.
\param [in]  format      - Used to check which mode should be used (0: LRS, 1: wxy, 2: wxyz FuMa, 3: wyzx SN3D).
*/
void doaProcess(void *obj, const float *objectInput, float *param, int format);

/**
\brief Release Doa object.
\param [in] obj - The Doa object to be released.
*/
void doaRelease(void *obj);

/* Ans. */

/**
\brief Initialize Ans to get an object.
\param [in] frameLength - Length of one frame for processing, must be LESS or EQUAL to 512.
\param [in] channelN    - Number of channels.
\param [in] maxSupDB    - Max suppressed dB, -20 -- -6.
\param [in] multithread - Flag of turning on multi-threads whiling computing.
This is a performance parameter and the behavior
depends on specific platform and processor.
\return The initialized object of Ans or null pointer that means failed.
*/
void * ansInit(int frameLength, int channelN, float maxSupDB, bool multithread);

/**
\brief Set Ans object.
\param [in] obj      - The Ans object.
\param [in] maxSupDB - Max suppressed dB, -20 -- -6.
*/
void ansSet(void *obj, float maxSupDB);

/**
\brief Process by Ans object.
\param [in]  obj         - The Doa object.
\param [in]  audioInput  - Interlaced multi-channel input audio data buffer.
Data in this buffer will not be modified during processing.
\param [out] audioOutput - Interlaced multi-channel output audio data buffer.
NOTICE: It is also ALLOWED audioInput = audioOutput (in-place).
*/
void ansProcess(void *obj, const float *audioInput, float *audioOutput);

/**
\brief Release Ans object.
\param [in] obj - The Ans object to be released.
*/
void ansRelease(void *obj);

/* Beamforming. */

/**
	\brief Initialize Ans to get an object.
	\param [in] frameLength - Length of one frame for processing, must be LESS or EQUAL to 512.
	\param [in] channelN    - Number of channels.
	\param [in] maxSupDB    - Max suppressed dB, -20 -- -6.
	\param [in] multithread - Flag of turning on multi-threads whiling computing. 
	                          This is a performance parameter and the behavior
							  depends on specific platform and processor.
	\return The initialized object of Ans or null pointer that means failed.
*/
void * beamformingInit(int frameLength, int channelN, float maxSupDB, bool multithread);

/**
	\brief Set Ans object.
	\param [in] obj      - The Ans object.
	\param [in] mode -   0: beamforming;   1: beamforming+ns;  2: beamforming+ns+nlp
	\param [in] maxSupDB - Max suppressed dB, -20 -- -6.
	\param [in] targetDirection - target signal direction in azimuth angle. 0~360
	\param [in] nlp_sup - nlp suppress level. 0.5 -- 2.0.  Default value is 1.0
	*/
void beamformingSet(void *obj, int mode, float maxSupDB, float targetDirection, float nlp_sup);

/**
	\brief Process by Ans object.
	\param [in]  obj         - The Doa object.
	\param [in]  audioInput  - Interlaced multi-channel input audio data buffer.
						       Data in this buffer will not be modified during processing.
	\param [out] audioOutput - Interlaced multi-channel output audio data buffer.
	                           NOTICE: It is also ALLOWED audioInput = audioOutput (in-place).
*/
void beamformingProcess(void *obj, const float *audioInput, float *audioOutput);

/**
	\brief Release Ans object.
	\param [in] obj - The Ans object to be released.
*/
void beamformingRelease(void *obj);

/* Equalizer. */

/**
\brief Initialize equalizer to get an object.
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
\brief Process by equalizer object.
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
\brief Release equalizer object.
\param [in] obj - The equalizer object to be released.
*/
void eqRelease(void * obj);


#ifdef __cplusplus				
};
#endif

#endif /*__AUDIO_CAPTURE_API__*/
