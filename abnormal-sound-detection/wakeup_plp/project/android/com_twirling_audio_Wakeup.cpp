#include "com_twirling_audio_Wakeup.h"
#include "../src/pocketsphinx/include/TwirlingWakeUpApi.h"
#include <stdio.h>

#define AUDIO_OBJ(obj)   (*(void **)&obj)

JNIEXPORT jlong JNICALL Java_com_twirling_audio_Wakeup_WakeupInit(
	JNIEnv *env, jobject obj)
{
	jlong audio_obj = 0;
	AUDIO_OBJ(audio_obj) = WakeupInit();
	return audio_obj;
}

JNIEXPORT jboolean JNICALL Java_com_twirling_audio_Wakeup_GetWakeupStatus(
	JNIEnv *env, jobject obj, jlong audio_obj)
{
	return GetWakeupStatus(AUDIO_OBJ(audio_obj));
}

JNIEXPORT void JNICALL Java_com_twirling_audio_Wakeup_WakeupRun(
	JNIEnv *env, jobject obj, jlong audio_obj, jshortArray objectInput, jint frameLength)
{
	jshort *objectInput_J = env->GetShortArrayElements(objectInput, NULL);
	
	WakeupRun(AUDIO_OBJ(audio_obj), objectInput_J, frameLength);
		
	env->ReleaseShortArrayElements(objectInput, objectInput_J, 0);
}

JNIEXPORT void JNICALL Java_com_twirling_audio_Wakeup_WakeupRelease(
	JNIEnv *env, jobject obj, jlong audio_obj)
{
	WakeupRelease(AUDIO_OBJ(audio_obj));
}