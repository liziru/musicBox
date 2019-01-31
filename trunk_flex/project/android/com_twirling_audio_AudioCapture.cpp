#include "com_twirling_audio_AudioCapture.h"
#include "../src/api/TwirlingCapture.h"
#include <stdio.h>

#define CAPTURE_OBJ(obj)   (*(void **)&obj)


JNIEXPORT jlong JNICALL Java_com_twirling_audio_AudioCapture_micArrayInit(
	JNIEnv *env, jobject obj, jint profileId, jint frameLength, jint sampleRate, jint micChannelNum, jfloat micDist, jint refChannelNum, jint aecSysLatency)
{
	jlong micMrray_obj = 0;
	CAPTURE_OBJ(micMrray_obj) = micArrayInit(profileId, frameLength, sampleRate, micChannelNum, micDist, refChannelNum, aecSysLatency);
	return micMrray_obj;	
}
  
  
 JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_micArrayProcess(
	JNIEnv *env, jobject obj, jlong micMrray_obj, jfloatArray audioInput, jfloatArray audioOutput, jfloatArray audioReference)
  {
	jfloat *audioInput_J = env->GetFloatArrayElements(audioInput, NULL);
	jfloat *audioOutput_J = env->GetFloatArrayElements(audioOutput, NULL);
	jfloat *audioReference_J = env->GetFloatArrayElements(audioReference, NULL);
	
	
	micArrayProcess(CAPTURE_OBJ(micMrray_obj), audioInput_J, audioOutput_J, audioReference_J);
		
	env->ReleaseFloatArrayElements(audioInput, audioInput_J, 0);
	env->ReleaseFloatArrayElements(audioOutput, audioOutput_J, 0);
	env->ReleaseFloatArrayElements(audioReference, audioReference_J, 0);
  }
  
JNIEXPORT void JNICALL JNICALL Java_com_twirling_audio_AudioCapture_micArrayRelease(
	JNIEnv *env, jobject obj, jlong micMrray_obj)
 {
	micArrayRelease(CAPTURE_OBJ(micMrray_obj));  
 }
  
  
 JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_micArraySetStatus(
	JNIEnv *env, jobject obj, jlong micMrray_obj, jint status)
 {
	micArraySetStatus(CAPTURE_OBJ(micMrray_obj), status);
 }
 
 JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_micArraySetAec(
	JNIEnv *env, jobject obj, jlong micMrray_obj, jboolean, jboolean enableRes, jfloat resLevel, jboolean enableNS, jfloat nsDB, jboolean enableSpkClip, jfloat spkClipThd, jfloat maxCoupling, jfloat adaptStep)
{
	micArraySetAec(CAPTURE_OBJ(micMrray_obj), enableRes, resLevel, enableNS, nsDB, enableSpkClip, spkClipThd, maxCoupling, adaptStep);	
}


JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_micArraySetBeamforming(
	JNIEnv *env, jobject obj, jlong micMrray_obj, jint mode, jfloat maxSupDb, jfloat targetDirection, jfloat targetEstimation, jfloat nlpSup, jfloat tolerance, jint targetFlag)
{
	micArraySetBeamforming(CAPTURE_OBJ(micMrray_obj), mode, maxSupDb, targetDirection, targetEstimation, nlpSup, tolerance, targetFlag);	
}

JNIEXPORT jfloat JNICALL Java_com_twirling_audio_AudioCapture_micArrayGetCurrentDoa(
	JNIEnv *env, jobject obj, jlong micMrray_obj)
{
	jfloat currentDoa = 0;	
	currentDoa = micArrayGetCurrentDoa(CAPTURE_OBJ(micMrray_obj));
	return currentDoa;
}

JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_micArraySetTargetDoa(
	JNIEnv *env, jobject obj, jlong micMrray_obj, jfloat targetDirection)
{
	micArraySetTargetDoa(CAPTURE_OBJ(micMrray_obj), targetDirection);  
}
 
JNIEXPORT jlong JNICALL Java_com_twirling_audio_AudioCapture_captureInit(
	JNIEnv *env, jobject obj, jint profileID, jint frameLength, jint channelN, jint sRate, jboolean multithread)
{
	jlong capture_obj = 0;
	CAPTURE_OBJ(capture_obj) = captureInit(profileID, frameLength, channelN, sRate, multithread);
	return capture_obj;
}

JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_captureSet(
	JNIEnv *env, jobject obj, jlong capture_obj, jboolean dycOn, jfloat dycMaxValue, jint formatid)
{
	captureSet(CAPTURE_OBJ(capture_obj), dycOn, dycMaxValue, formatid);
}

JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_captureProcess(
	JNIEnv *env, jobject obj, jlong capture_obj, jfloat heading, jfloat pitch, jfloat bank, jfloatArray objectInput, jfloatArray binauralOutput, jfloatArray metaDataFrame)
{
	jfloat *objectInput_J = env->GetFloatArrayElements(objectInput, NULL);
	jfloat *binauralOutput_J = env->GetFloatArrayElements(binauralOutput, NULL);
	jfloat *metaDataFrame_J = env->GetFloatArrayElements(metaDataFrame, NULL);

	captureProcess(CAPTURE_OBJ(capture_obj), heading, pitch, bank, objectInput_J, binauralOutput_J, metaDataFrame_J);
		
	env->ReleaseFloatArrayElements(objectInput, objectInput_J, 0);
	env->ReleaseFloatArrayElements(binauralOutput, binauralOutput_J, 0);
	env->ReleaseFloatArrayElements(metaDataFrame, metaDataFrame_J, 0);
}


JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_captureRelease(
	JNIEnv *env, jobject obj, jlong capture_obj)
{
	captureRelease(CAPTURE_OBJ(capture_obj));
}

JNIEXPORT jlong JNICALL Java_com_twirling_audio_AudioCapture_captureStereoInit
  (JNIEnv *env, jobject obj, jint frameLength, jint channelN, jint sRate)
{
	jlong capture_stereo_obj = 0;
	CAPTURE_OBJ(capture_stereo_obj) = captureStereoInit(frameLength, channelN, sRate);
	return capture_stereo_obj; 
 }
 
 JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_captureStereoSet
  (JNIEnv *env, jobject obj, jlong capture_stereo_obj, jint placementMode, jint cameraMode)
 {
	captureStereoSet(CAPTURE_OBJ(capture_stereo_obj), placementMode, cameraMode); 
 }
 
 JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_captureStereoProcess
  (JNIEnv *env, jobject obj, jlong capture_stereo_obj, jfloatArray objectInput, jfloatArray stereoOutput)
 {
	jfloat *objectInput_J = env->GetFloatArrayElements(objectInput, NULL);
	jfloat *stereoOutput_J = env->GetFloatArrayElements(stereoOutput, NULL);

	captureStereoProcess(CAPTURE_OBJ(capture_stereo_obj), objectInput_J, stereoOutput_J);
		
	env->ReleaseFloatArrayElements(objectInput, objectInput_J, 0);
	env->ReleaseFloatArrayElements(stereoOutput, stereoOutput_J, 0);
 }
 
 JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_captureStereoRelease(
	JNIEnv *env, jobject obj, jlong capture_stereo_obj)
{
	captureStereoRelease(CAPTURE_OBJ(capture_stereo_obj));
}
  
JNIEXPORT jlong JNICALL Java_com_twirling_audio_AudioCapture_vadInit
  (JNIEnv *env, jobject obj, jint frameLength, jint channelN, jint sRate)
{
	jlong vad_obj = 0;
	CAPTURE_OBJ(vad_obj) = vadInit(frameLength, channelN, sRate);	  
	return vad_obj;
 }
JNIEXPORT jfloat JNICALL Java_com_twirling_audio_AudioCapture_vadProcess
  (JNIEnv *env, jobject obj, jlong vad_obj, jfloatArray objectInput)
{
	jfloat vadFlag = 0;
	jfloat *objectInput_J = env->GetFloatArrayElements(objectInput, NULL);
	vadFlag = vadProcess(CAPTURE_OBJ(vad_obj), objectInput_J);
	env->ReleaseFloatArrayElements(objectInput, objectInput_J, 0); 	
	return vadFlag;
}

JNIEXPORT jint JNICALL Java_com_twirling_audio_AudioCapture_vadGetState
  (JNIEnv *env, jobject obj, jlong vad_obj, jfloatArray objectInput)
{
	jint vadState = 0;
	jfloat *objectInput_J = env->GetFloatArrayElements(objectInput, NULL);
	vadState = vadGetState(CAPTURE_OBJ(vad_obj), objectInput_J);
	env->ReleaseFloatArrayElements(objectInput, objectInput_J, 0);  
	return vadState;
 }

JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_vadRelease
  (JNIEnv *env, jobject obj, jlong vad_obj)
 {
	vadRelease(CAPTURE_OBJ(vad_obj));  
 }
  
JNIEXPORT jlong JNICALL Java_com_twirling_audio_AudioCapture_doaInit(
	JNIEnv *env, jobject obj, jint profileID, jint frameLength, jint channelN, jint sample_rate, jfloat micDist, jboolean multithread)
{
	jlong doa_obj = 0;
	CAPTURE_OBJ(doa_obj) = doaInit(profileID,frameLength, channelN, sample_rate, micDist, multithread);
	return doa_obj;
}

JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_doaSet(
	JNIEnv *env, jobject obj, jlong doa_obj, jint formatid)
{
	doaSet(CAPTURE_OBJ(doa_obj), formatid);
}

JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_doaProcess(
	JNIEnv *env, jobject obj, jlong doa_obj, jfloatArray objectInput, jfloatArray param)
{
	jfloat *objectInput_J = env->GetFloatArrayElements(objectInput, NULL);
	jfloat *param_J = env->GetFloatArrayElements(param, NULL);

	doaProcess(CAPTURE_OBJ(doa_obj), objectInput_J, param_J);

	env->ReleaseFloatArrayElements(objectInput, objectInput_J, 0);
	env->ReleaseFloatArrayElements(param, param_J, 0);
}


JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_doaRelease(
	JNIEnv *env, jobject obj, jlong doa_obj)
{
	doaRelease(CAPTURE_OBJ(doa_obj));
}


JNIEXPORT jlong JNICALL Java_com_twirling_audio_AudioCapture_ansInit(
	JNIEnv *env, jobject obj, jint frameLength, jint channelN, jint sampleRate, jfloat maxSupDB, jboolean multithread)
{
	jlong ans_obj = 0;
	CAPTURE_OBJ(ans_obj) = ansInit(frameLength, channelN, sampleRate, maxSupDB, multithread);
	return ans_obj;
}


JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_ansSet(
	JNIEnv *env, jobject obj, jlong ans_obj, jfloat maxSupDB)
{
	ansSet(CAPTURE_OBJ(ans_obj), maxSupDB);
}

JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_ansProcess(
	JNIEnv *env, jobject obj, jlong ans_obj, jfloatArray audioInput, jfloatArray audioOutput)
{
	jfloat *audioInput_J = env->GetFloatArrayElements(audioInput, NULL);
	jfloat *audioOutput_J = env->GetFloatArrayElements(audioOutput, NULL);

	ansProcess(CAPTURE_OBJ(ans_obj), audioInput_J, audioOutput_J);

	env->ReleaseFloatArrayElements(audioInput, audioInput_J, 0);
	env->ReleaseFloatArrayElements(audioOutput, audioOutput_J, 0);
}


JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_ansRelease(
	JNIEnv *env, jobject obj, jlong ans_obj)
{
	ansRelease(CAPTURE_OBJ(ans_obj));
}

JNIEXPORT jlong JNICALL Java_com_twirling_audio_AudioCapture_beamformingInit(
	JNIEnv *env, jobject obj, jint profileID, jint frameLength, jint channelN, jint sampleRate, jfloat micDist, jboolean multithread)
{
	jlong beamforming_obj = 0;
	CAPTURE_OBJ(beamforming_obj) = beamformingInit(profileID,frameLength, channelN, sampleRate, micDist, multithread);
	return beamforming_obj;
}
  
JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_beamformingSet(
	JNIEnv *env, jobject obj, jlong beamforming_obj, jint mode, jfloat max_sup_db, jfloat targetDirection, jfloat targetEstimation, jfloat nlp_sup, jfloat tolerance, jint targetFlag)
{
	beamformingSet(CAPTURE_OBJ(beamforming_obj), mode, max_sup_db, targetDirection, targetEstimation, nlp_sup, tolerance, targetFlag);
}

JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_beamformingProcess(
	JNIEnv *env, jobject obj, jlong beamforming_obj, jfloatArray audioInput, jfloatArray audioOutput)
{
	jfloat *audioInput_J = env->GetFloatArrayElements(audioInput, NULL);
	jfloat *audioOutput_J = env->GetFloatArrayElements(audioOutput, NULL);

	beamformingProcess(CAPTURE_OBJ(beamforming_obj), audioInput_J, audioOutput_J);

	env->ReleaseFloatArrayElements(audioInput, audioInput_J, 0);
	env->ReleaseFloatArrayElements(audioOutput, audioOutput_J, 0);
}
JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_beamformingRelease(
	JNIEnv *env, jobject obj, jlong beamforming_obj)
{
	beamformingRelease(CAPTURE_OBJ(beamforming_obj));
}  

JNIEXPORT jlong JNICALL Java_com_twirling_audio_AudioCapture_eqInit(
	JNIEnv *env, jobject obj, jint frameLength, jint channelN, jint sRate, jboolean multithread)
{
	jlong eq_obj = 0;
	CAPTURE_OBJ(eq_obj) = eqInit(frameLength, channelN, sRate, multithread);
	return eq_obj;
}


JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_eqProcess(
	JNIEnv *env, jobject obj, jlong eq_obj, jfloatArray audioInput, jfloatArray audioOutput, jfloatArray eqParam)
{
	jfloat *audioInput_J = env->GetFloatArrayElements(audioInput, NULL);
	jfloat *audioOutput_J = env->GetFloatArrayElements(audioOutput, NULL);
	jfloat *eqParam_J = env->GetFloatArrayElements(eqParam, NULL);

	eqProcess(CAPTURE_OBJ(eq_obj), audioInput_J, audioOutput_J, eqParam_J);

	env->ReleaseFloatArrayElements(audioInput, audioInput_J, 0);
	env->ReleaseFloatArrayElements(audioOutput, audioOutput_J, 0);
	env->ReleaseFloatArrayElements(eqParam, eqParam_J, 0);
}


JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_eqRelease(
	JNIEnv *env, jobject obj, jlong eq_obj)
{
	eqRelease(CAPTURE_OBJ(eq_obj));
}


JNIEXPORT jlong JNICALL Java_com_twirling_audio_AudioCapture_aecInit
(JNIEnv *env, jobject obj, jint FrameLength, jint filterLength, jint sRate, jint sysDelay, jint micN, jint spkN, jboolean multithread)
{
	jlong aec_obj = 0;
	CAPTURE_OBJ(aec_obj) = aecInit(FrameLength, filterLength, sRate, sysDelay, micN, spkN, multithread);
	return aec_obj;
}


JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_aecSet
(JNIEnv *env, jobject obj, jlong aec_obj, jboolean enableRes, jfloat resLevel, jboolean enableNS, jfloat nsDB, jboolean enableSpkClip, jfloat spkClipThd, jfloat maxCoupling, jfloat adaptStep)
{
	aecSet(CAPTURE_OBJ(aec_obj), enableRes, resLevel, enableNS, nsDB, enableSpkClip, spkClipThd, maxCoupling, adaptStep);
}


JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_aecProcess(
	JNIEnv *env, jobject obj, jlong aec_obj, jfloatArray  audioInput, jfloatArray audioOutput)
{
	jfloat *audioInput_J = env->GetFloatArrayElements(audioInput, NULL);
	jfloat *audioOutput_J = env->GetFloatArrayElements(audioOutput, NULL);

	aecProcess(CAPTURE_OBJ(aec_obj), audioInput_J, audioOutput_J);

	env->ReleaseFloatArrayElements(audioInput, audioInput_J, 0);
	env->ReleaseFloatArrayElements(audioOutput, audioOutput_J, 0);
}

JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_aecRelease(
	JNIEnv *env, jobject obj, jlong aec_obj)
{
	aecRelease(CAPTURE_OBJ(aec_obj));
}

JNIEXPORT jlong JNICALL Java_com_twirling_audio_AudioCapture_resamplerInit
  (JNIEnv *env, jobject obj, jint channelN, jint frameLength, jint sRate)
  {
	  jlong resampler_obj = 0;
	  CAPTURE_OBJ(resampler_obj) = resamplerInit(channelN, frameLength, sRate);
	  return resampler_obj;
  }
  
  JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_resamplerSet
  (JNIEnv *env, jobject obj, jlong resampler_obj)
  {
	  resamplerSet(CAPTURE_OBJ(resampler_obj));
  }
  
  JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_resamplerProcess
  (JNIEnv *env, jobject obj, jlong resampler_obj, jfloatArray input, jfloatArray output, jint channelN, jint framelen, jint sRate)
  {
	  jfloat *input_J = env->GetFloatArrayElements(input, NULL);
	  jfloat *output_J = env->GetFloatArrayElements(output, NULL);

	  resamplerProcess(CAPTURE_OBJ(resampler_obj), input_J, output_J, channelN, framelen, sRate);

	  env->ReleaseFloatArrayElements(input, input_J, 0);
	  env->ReleaseFloatArrayElements(output, output_J, 0);
  }
  
  JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_resamplerRelease
  (JNIEnv *env, jobject obj, jlong resampler_obj)
  {
	  resamplerRelease(CAPTURE_OBJ(resampler_obj));
  }
  
  
  void * agcInit(int frameLength, int channelN, int sampleRate, float threshold);
  JNIEXPORT jlong JNICALL Java_com_twirling_audio_AudioCapture_agcInit
  (JNIEnv *env, jobject obj, jint frameLength, jint channelN, jint sampleRate, jfloat threshold)
  {
	  jlong agc_obj = 0;
	  CAPTURE_OBJ(agc_obj) = agcInit(frameLength, channelN, sampleRate, threshold);
	  return agc_obj;
  }

JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_agcSet
  (JNIEnv *env, jobject obj, jlong agc_obj, jint formatid)
  {
	  agcSet(CAPTURE_OBJ(agc_obj), formatid);
  }

JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_agcProcess
  (JNIEnv *env, jobject obj, jlong agc_obj, jfloatArray input, jfloatArray output, jfloatArray para)
  {
	  jfloat *input_J = env->GetFloatArrayElements(input, NULL);
	  jfloat *output_J = env->GetFloatArrayElements(output, NULL);
	  jfloat *para_J = env->GetFloatArrayElements(para, NULL);

	  agcProcess(CAPTURE_OBJ(agc_obj), input_J, output_J, para_J);

	  env->ReleaseFloatArrayElements(input, input_J, 0);
	  env->ReleaseFloatArrayElements(output, output_J, 0);
	  env->ReleaseFloatArrayElements(para, para_J, 0);
  }

JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_agcRelease
  (JNIEnv *env, jobject obj, jlong agc_obj)
  {
	  agcRelease(CAPTURE_OBJ(agc_obj));
  }
  
  JNIEXPORT jlong JNICALL Java_com_twirling_audio_AudioCapture_bssInit
  (JNIEnv *env, jobject obj, jint frameLength, jint channelN, jint sampleRate, jboolean multithread)
  {
	  jlong bss_obj = 0;
	  CAPTURE_OBJ(bss_obj) = bssInit(frameLength, channelN, sampleRate, multithread);
	  return bss_obj;
  }
  
  JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_bssSet
  (JNIEnv *env, jobject obj, jlong bss_obj, jint formatid)
  {
	  bssSet(CAPTURE_OBJ(bss_obj), formatid);
  }
  
  
  JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_bssProcess
  (JNIEnv *env, jobject obj, jlong bss_obj, jfloatArray input, jfloatArray output)
  {
	  jfloat *input_J = env->GetFloatArrayElements(input, NULL);
	  jfloat *output_J = env->GetFloatArrayElements(output, NULL);

	  bssProcess(CAPTURE_OBJ(bss_obj), input_J, output_J);

	  env->ReleaseFloatArrayElements(input, input_J, 0);
	  env->ReleaseFloatArrayElements(output, output_J, 0);
  }
  
  
  JNIEXPORT void JNICALL Java_com_twirling_audio_AudioCapture_bssRelease
  (JNIEnv *env, jobject obj, jlong bss_obj)
  {
	  bssRelease(CAPTURE_OBJ(bss_obj));
  }
  
  
  