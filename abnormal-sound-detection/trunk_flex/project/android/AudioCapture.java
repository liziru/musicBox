/*
 * Copyright 2014 Google Inc. All Rights Reserved.

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.twirling.audio;

/**
 * Contains vertex, normal and color data.
 */
public class AudioCapture {
    public native long micArrayInit(int profileId, int frameLength, int sampleRate, int micChannelNum, float micDist, int refChannelNum, int aecSysLatency);
	public native void micArrayProcess(long obj, float audio_input[], float audio_output[], float audio_reference[]);
	public native void micArrayRelease(long obj);
	public native void micArraySetStatus(long obj, int status);
	public native void micArraySetAec(long obj, boolean enableRes, float resLevel, boolean enableNS, float nsDB, boolean enableSpkClip, float spkClipThd, float maxCoupling, float adaptStep);
	public native void micArraySetBeamforming(long obj, int mode, float maxSupDB, float targetDirection, float targetEstimation, float nlpSup, float tolerance, int targetFlag);
	public native float micArrayGetCurrentDoa(long obj);
	public native void micArraySetTargetDoa(long obj, float targetDirection);
	
    public native long captureInit(int profileID, int frameLength, int channelN, int sRate, boolean multithread);
    public native void captureSet(long obj, boolean dycOn, float dycMaxValue, int formatid);
    public native void captureProcess(long obj, float heading, float pitch, float bank, float objectInput[], float binauralOutput[], float metaDataFrame[]);
    public native void captureRelease(long obj);
	
	public native long captureStereoInit(int frameLength, int channelN, int sRate);
    public native void captureStereoSet(long obj, int placementMode, int cameraMode);
    public native void captureStereoProcess(long obj, float objectInput[], float binauralOutput[]);
    public native void captureStereoRelease(long obj);
	
	public native long vadInit(int framelength, int channelN, int sRate);
	public native float vadProcess(long obj, float objectInput[]);
	public native int vadGetState(long obj, float objectInput[]);
	public native void vadRelease(long obj);
	public native long doaInit(int profileID, int frameLength, int channelN, int sample_rate, float micDist, boolean multithread);
	public native void doaSet(long obj, int formatid);
	public native void doaProcess(long obj, float objectInput[], float param[]);
	public native void doaRelease(long obj);
	public native long beamformingInit(int profileID, int frameLength, int channelN, int sampleRate, float micDist, boolean multithread);
	public native void beamformingSet(long obj, int mode, float max_sup_db, float targetDirection, float targetEstimation, float nlp_sup, float tolerance, int targetFlag);
	public native void beamformingProcess(long obj, float audioInput[], float audioOutput[]);
	public native void beamformingRelease(long obj);
	public native long ansInit(int frameLength, int channelN, int sampleRate, float maxSupDB, boolean multithread);
	public native void ansSet(long obj, float maxSupDB);
	public native void ansProcess(long obj, float audioInput[], float audioOutput[]);
	public native void ansRelease(long obj);
	public native long eqInit(int frameLength, int channelN, int sRate, boolean multithread);
	public native void eqProcess(long obj, float audioInput[], float audioOutput[], float eqParam[]);
	public native void eqRelease(long obj);
	public native long aecInit(int FrameLength, int filterLength, int sRate, int sysDelay, int micN, int spkN, boolean multithread);
	public native void aecSet(long obj, boolean enableRes, float resLevel, boolean enableNS, float nsDB, boolean enableSpkClip, float spkClipThd, float maxCoupling, float adaptStep);
	public native void aecProcess(long obj, float audioInput[], float audioOutput[]);
    public native void aecRelease(long obj);
	public native long resamplerInit(int channelN, int frameLength, int sRate);
	public native void resamplerSet(long obj);
	public native void resamplerProcess(long obj, float input[], float output[], int channelN, int framelen, int sRate);
	public native void resamplerRelease(long obj);
	
	public native long agcInit(int frameLength, int channelN, int sampleRate, float threshold);
    public native void agcSet(long obj, int formatid);
    public native void agcProcess(long obj, float audioInput[], float audioOutput[], float params[]);
    public native void agcRelease(long obj);
	
	public native long bssInit(int frameLength, int channelN, int sampleRate, boolean multithread);
	public native void bssSet(long obj, int formatid);
	public native void bssProcess(long obj, float audioInput[], float audioOutput[]);
	public native void bssRelease(long obj);
	
    static {
        System.loadLibrary("TwirlingCapture");
    }
}
