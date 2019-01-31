#include "../core/core.h"
#include "../core/micarray_core.h"
#include "../doa/localizer.h"
#include "../ans/ans.h"
#include "../ans/beamforming.h"
#include "../equalizer/equalizer.h"
#include "../aec/aec.h"
#include "../vad/vad.h"
#include "../resample/resample.h"
#include "../automaticgaincontrol/automaticGainControl.h"
#include "../stereo/stereo.h"
#include "../bss/bss.h"
#include "../dynamicrangecontrol/dynamicRangeControl.h"

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void * micArrayInit(int profileId, int frameLength, int sampleRate, int micChannelNum, float micDist, int refChannelNum, int aecSysLatency)
{
	return new CMicArrayCore(profileId, frameLength, sampleRate, micChannelNum, micDist, refChannelNum, aecSysLatency);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void micArrayProcess(void *obj, float *audioInput, float *audioOutput, float *audioReference)
{
	((CMicArrayCore*)obj)->process(audioInput, audioOutput, audioReference);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void micArrayRelease(void *obj)
{
	delete (CMicArrayCore*)obj;
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void micArraySetStatus(void *obj, int status)
{
	((CMicArrayCore*)obj)->set_status(status);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void micArraySetTargetDoa(void *obj, float target_direction)
{
	((CMicArrayCore*)obj)->set_target_doa(target_direction);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
float micArrayGetCurrentDoa(void *obj)
{
	return ((CMicArrayCore*)obj)->get_current_doa();
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void micArraySetAec(void *obj, bool enableRes, float resLevel, bool enableNS, float nsDB, bool enableSpkClip, float spkClipThd, float maxCoupling, float adaptStep)
{
	((CMicArrayCore*)obj)->set_aec(enableRes, resLevel, enableNS, nsDB, enableSpkClip, spkClipThd, maxCoupling, adaptStep);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void micArraySetBeamforming(void *obj, int mode, float maxSupDB, float targetDirection, float targetEstimation, float nlpSup, float tolerance, int targetFlag)
{
	((CMicArrayCore*)obj)->set_beamforming(mode, maxSupDB, targetDirection, targetEstimation, nlpSup, tolerance, targetFlag);
}



#ifdef _WINDLL
extern "C" __declspec(dllexport) 
#else
extern "C" 
#endif
void * captureInit(int profileID, int FrameLength, int objNum, int sRate, bool multithread)
{
	AudioEngineCreateParameter param;
	param.profile_id = profileID;
	param.frame_length = FrameLength;
	param.object_number = objNum;
	param.sample_rate = (SampleRate)sRate;
	param.multithread = multithread;
	return new CAudioEngineCore(&param);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void captureSet(void *obj, bool dycOn, float dycMaxValue, int formatid)
{
	AudioEngineSettings settings;
	settings.dyc_on = dycOn;
	settings.dyc_max_val = dycMaxValue;
	settings.formatid = formatid;
	((CAudioEngineCore*)obj)->set_param(&settings);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void captureProcess(void *obj, float heading, float pitch, float bank, const float *objectInput, float *binauralOutput, const float *metaDataFrame)
{
	AudioEngineProcessParameter param;
	param.head.heading = heading;
	param.head.pitch = pitch;
	param.head.bank = bank;
	param.input = objectInput;
	param.output = binauralOutput;
	param.meta = (const MetaData *)metaDataFrame;
    ((CAudioEngineCore*)obj)->process(&param);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void captureRelease(void *obj)
{
    delete (CAudioEngineCore*)obj;
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void * captureStereoInit(int FrameLength, int channelN, int sRate)
{
	return new CStereo(FrameLength, channelN, sRate);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void captureStereoSet(void *obj, int placementMode, int cameraMode)
{
	((CStereo*)obj)->set(placementMode, cameraMode);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void captureStereoProcess(void *obj, const float *objectInput, float *binauralOutput)
{
	((CStereo*)obj)->process(objectInput, binauralOutput);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void captureStereoRelease(void *obj)
{
	delete (CStereo*)obj;
}
#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void * vadInit(int frameLength, int channelN, int sample_rate)
{   return new CVad(frameLength, channelN, sample_rate);
}


#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
float vadProcess(void *obj, const float *objectInput)
{	
	return ((CVad*)obj)->process(objectInput);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
int vadGetState(void *obj, const float *objectInput)
{
	return ((CVad*)obj)->getState(objectInput);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void vadRelease(void *obj)
{   delete (CVad*)obj;
}


#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void * doaInit(int profileID, int frameLength, int channelN, int sample_rate, float micDist, bool multithread)
{   //KHT use Localizer to replace doa, i.e. use gcc-phat to replace MUSIC 
	//return new CDoa(profileID, frameLength, channelN, sample_rate, micDist, multithread);
	return new CLocalizer(profileID, frameLength, channelN, sample_rate, micDist, multithread); 
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void doaSet(void *obj, int formatid)
{   //KHT use Localizer to replace doa, i.e. use gcc-phat to replace MUSIC 
	((CLocalizer*)obj)->set(formatid);
}


#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void doaProcess(void *obj, const float *objectInput, float *param)
{
	//KHT use Localizer to replace doa, i.e.use gcc - phat to replace MUSIC
	//((CDoa*)obj)->process(objectInput, param);
	((CLocalizer*)obj)->process(objectInput, param);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void doaRelease(void *obj)
{   //KHT use Localizer to replace doa, i.e.use gcc - phat to replace MUSIC
	//delete (CDoa*)obj;
	delete (CLocalizer*)obj;
}


#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif

void * localizerInit(int profileID, int frameLength, int channelN, int sample_rate, float micDist, bool multithread)
{
	return new CLocalizer(profileID, frameLength, channelN, sample_rate, micDist, multithread);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void localizerProcess(void *obj, const float *objectInput, float *param)
{
	((CLocalizer*)obj)->process(objectInput, param);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void localizerRelease(void *obj)
{
	delete (CLocalizer*)obj;
}



#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void * ansInit(int FrameLength, int channelN, int sampleRate, float maxSupDB, bool multithread)
{
	return new CAns(FrameLength, channelN, sampleRate, maxSupDB, multithread);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void ansSet(void *obj, float maxSupDB)
{
	((CAns*)obj)->set(maxSupDB);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void ansProcess(void *obj, const float *audioInput, float *audioOutput)
{
	((CAns*)obj)->process(audioInput, audioOutput);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void ansRelease(void *obj)
{
	delete (CAns*)obj;
}



#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void * beamformingInit(int profileID, int FrameLength, int channelN, int sampleRate, float micDist, bool multithread)
{
	return new CBeamforming(profileID, FrameLength, channelN, sampleRate, micDist, multithread);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void beamformingSet(void *obj, int mode, float max_sup_db, float targetDirection, float targetEstimation, float nlp_sup, float tolerance,int targetFlag)
{
	((CBeamforming*)obj)->set(mode, max_sup_db, targetDirection, targetEstimation, nlp_sup, tolerance, targetFlag);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void beamformingProcess(void *obj, const float *audioInput, float *audioOutput)
{
	((CBeamforming*)obj)->process(audioInput, audioOutput);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void beamformingRelease(void *obj)
{
	delete (CBeamforming*)obj;
}


#if 1
#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void * aecInit(int FrameLength, int filterLength, int sRate, int sysDelay, int micN, int spkN, bool multithread)
{
	return new CAec(FrameLength, filterLength, sRate, micN, spkN, sysDelay);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void aecSet(void *obj, bool enableRes, float resLevel, bool enableNS, float nsDB, bool enableSpkClip, float spkClipThd, float maxCoupling, float adaptStep)
{
	((CAec*)obj)->set(enableRes, resLevel, enableNS, nsDB, enableSpkClip, spkClipThd, maxCoupling, adaptStep);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void aecProcess(void *obj, const float *audioInput, float *audioOutput)
{
	((CAec*)obj)->process(audioInput, audioOutput);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void aecRelease(void *obj)
{
	delete (CAec*)obj;
}
#endif

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void * eqInit(int FrameLength, int channelN, int sRate, bool multithread)
{
	return new CEqualizer(FrameLength, channelN, sRate, multithread);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void eqProcess(void *obj, const float *audioInput, float *audioOutput, const float *eqParam)
{
	((CEqualizer*)obj)->process(audioInput, audioOutput, eqParam);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif
void eqRelease(void * obj)
{
	delete (CEqualizer*)obj;
}
#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void * resamplerInit(int channelN, int frameLength, int sRate)
{
	
	return new CResample(channelN,frameLength,sRate);
	
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void resamplerSet(void *obj)
{
	return;
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void resamplerProcess(void *obj, float *input, float *output, int channelN, int framelen, int sRate)
{
	((CResample *)obj)->resample(input, output, channelN, framelen, sRate);

}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void resamplerRelease(void *obj)
{
	delete (CResample *)obj;
}



#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void * agcInit(int FrameLength, int channelN, int sRate, float threshold)
{
	return new CAutomaticGainControl(FrameLength, channelN, sRate, threshold);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void agcSet(void *obj, int formatid)
{
	((CAutomaticGainControl*)obj)->set(formatid);
}


#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void agcProcess(void *obj, const float *audioInput, float *audioOutput, float *params)
{
	((CAutomaticGainControl*)obj)->process(audioInput, audioOutput, params);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void agcRelease(void * obj)
{
	delete (CAutomaticGainControl*)obj;
}


#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void * bssInit(int FrameLength, int channelN, int sRate, bool multithread)
{
	if (channelN > 2)	//only support 1 or 2 channels for BSS
		return NULL;
	return new CBss(FrameLength, channelN, sRate, multithread);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void bssSet(void *obj, int formatid)
{
	((CBss*)obj)->set(formatid);
}


#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void bssProcess(void *obj, const float *audioInput, float *audioOutput)
{
	((CBss*)obj)->process(audioInput, audioOutput);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void bssRelease(void * obj)
{
	delete (CBss*)obj;
}


#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void * drcInit(int frameLength, int channelN, float maxV)
{
	return new CDynamicRangeControl(frameLength, channelN, 1.0);
}
#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void drcSet(void *obj, float maxV)
{
	((CDynamicRangeControl*)obj)->set(maxV);
}

#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void drcProcess(void *obj, const float *audioInput, float *audioOutput)
{
	((CDynamicRangeControl*)obj)->process(audioInput, audioOutput);
}
#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void drcRelease(void *obj) 
{
	delete (CDynamicRangeControl*)obj;
}