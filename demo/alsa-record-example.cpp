/* 
   A Minimal Capture Program

   This program opens an audio interface for capture, configures it for
   stereo 4 channel, 16 bit, 16kHz, interleaved conventional read/write
   access. Then its reads a chunk of random data from it, and exits. It
   isn't meant to be a real program.

   From on Paul David's tutorial : http://equalarea.com/paul/alsa-audio.html

   Fixes rate and buffer problems

   Added recording to audio file - Kwang Myung Jeon, 2017-09-04

   Usage (Works on Raspbian)
   sudo apt-get install libasound2-dev
   gcc -o alsa-record-example -lasound alsa-record-example.c
   ./alsa-record-example hw:1 test.pcm
   */

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <signal.h>
#include <unistd.h>
#include "TwirlingCapture.h"

#define FRAMElEN (256)
#define MIC_NUM (4)
#define MIC_NUM_BEAMFORMING (3)
#define REF_NUM (1)
#define MAX16S (32767)
#define MIN16S (-32768)
#define MAXABS16S (32768)
#ifndef PI
#define PI (3.1415926)
#endif

//#define PRINT_TIME
//#define DUMP_DATA

int beamformingIsOn = 0;
snd_pcm_t *capture_handle;
snd_pcm_t *playback_handle;
snd_pcm_hw_params_t *hw_params;
snd_pcm_hw_params_t *hw_params_playback;

snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
unsigned int rate = 16000;

void printUsage(){
	printf("Usage:  ./sounder hw:1 hw:2 2 off 90\n");
	printf("        ./sounder hw:1 hw:2 2 on 80\n");
	exit (1);
}

int recordPrepare(const char *devName, unsigned int channel_num){
	int err = 0;
	if ((err = snd_pcm_open (&capture_handle, devName, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		fprintf (stderr, "cannot open capture audio device %s (%s)\n", 
				devName,
				snd_strerror (err));
		exit (1);
	}
	fprintf(stdout, "capture audio interface opened\n");

	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stderr, "capture: cannot allocate hardware parameter structure (%s)\n",
				snd_strerror (err));
		exit (1);
	}
	fprintf(stdout, "capture: hw_params allocated\n");

	if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
		fprintf (stderr, "capture: cannot initialize hardware parameter structure (%s)\n",
				snd_strerror (err));
		exit (1);
	}
	fprintf(stdout, "capture: hw_params initialized\n");

	if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "capture: cannot set access type (%s)\n",
				snd_strerror (err));
		exit (1);
	}
	fprintf(stdout, "capture: hw_params access setted\n");

	if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
		fprintf (stderr, "capture: cannot set sample format (%s)\n",
				snd_strerror (err));
		exit (1);
	}
	fprintf(stdout, "capture: hw_params format setted\n");

	if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
		fprintf (stderr, "capture: cannot set sample rate (%s)\n",
				snd_strerror (err));
		exit (1);
	}
	fprintf(stdout, "capture: hw_params rate setted\n");

	if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, channel_num)) < 0) {
		fprintf (stderr, "capture: cannot set channel count (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "capture: hw_params channels setted\n");

	if ((err = snd_pcm_hw_params_set_buffer_size(capture_handle, hw_params, FRAMElEN*2)) < 0) {
		fprintf (stderr, "capture: cannot set buffer size (%s)\n",
				snd_strerror (err));
		exit (1);
	}
	fprintf(stdout, "capture: hw_params buffer size setted\n");

	int val;
	snd_pcm_hw_params_get_buffer_size(hw_params,
			(snd_pcm_uframes_t *) &val);
	printf("capture: buffer size = %d frames\n", val);

	if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
		fprintf (stderr, "capture: cannot set parameters (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "capture: hw_params setted\n");

	snd_pcm_hw_params_free (hw_params);

	fprintf(stdout, "capture: hw_params freed\n");

	if ((err = snd_pcm_prepare (capture_handle)) < 0) {
		fprintf (stderr, "capture: cannot prepare audio interface for use (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "capture: audio interface prepared\n");
	return err;
}

int playbackPrepare(const char *devName, unsigned int channel_num){
	int err = 0;
	if ((err = snd_pcm_open (&playback_handle, devName, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		fprintf (stderr, "cannot open audio playback device (%s)\n", 
				snd_strerror (err));
		exit (1);
	}
	fprintf(stdout, "playback: audio playback interface opened\n");

	if ((err = snd_pcm_hw_params_malloc (&hw_params_playback)) < 0) {
		fprintf (stderr, "playback: cannot allocate hardware parameter structure (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "playback: hw_params_playback allocated\n");

	if ((err = snd_pcm_hw_params_any (playback_handle, hw_params_playback)) < 0) {
		fprintf (stderr, "playback: cannot initialize hardware parameter structure (%s)\n",
				snd_strerror (err));
		exit (1);
	}
	fprintf(stdout, "playback: hw_params_playback initialized\n");

	if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params_playback, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "playback: cannot set access type (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "playback: hw_params_playback access setted\n");

	if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params_playback, format)) < 0) {
		fprintf (stderr, "cannot set sample format (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "playback: hw_params_playback format setted\n");

	if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params_playback, &rate, 0)) < 0) {
		fprintf (stderr, "playback: cannot set sample rate (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "playback: hw_params_playback rate setted\n");

	if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params_playback, channel_num)) < 0) {
		fprintf (stderr, "playback: cannot set channel count (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "playback: hw_params_playback channels setted\n");

	int val;
	snd_pcm_hw_params_get_buffer_size(hw_params,
			(snd_pcm_uframes_t *) &val);
	printf("playback: buffer size = %d frames\n", val);
	//snd_pcm_hw_params_get_buffer_size_max(hw_params, (snd_pcm_uframes_t *)&val);
	//fprintf(stdout, "playback: hw_params buffer size max is %d\n", val);

	//if ((err = snd_pcm_hw_params_set_buffer_size(playback_handle, hw_params, FRAMElEN)) < 0) {
	//	fprintf (stderr, "playback: cannot set buffer size (%s)\n",
	//			snd_strerror (err));
	//	exit (1);
	//}
	//fprintf(stdout, "playback: hw_params buffer size setted\n");

	if ((err = snd_pcm_hw_params (playback_handle, hw_params_playback)) < 0) {
		fprintf (stderr, "playback: cannot set parameters (%s)\n",
				snd_strerror (err));
		exit (1);
	}
	fprintf(stdout, "playback: hw_params_playback setted\n");

	snd_pcm_hw_params_free (hw_params_playback);
	fprintf(stdout, "playback: hw_params freed\n");

	if ((err = snd_pcm_prepare (playback_handle)) < 0) {
		fprintf (stderr, "playback: cannot prepare audio interface for use (%s)\n",
				snd_strerror (err));
		exit (1);
	}
	fprintf(stdout, "playback: audio interface prepared\n");
	return err;
}

int setPlaybackVolume(const char *devName, const int vol){
	snd_mixer_t *mixer;
	snd_mixer_elem_t *pcm_element;
	long alsa_min_vol, alsa_max_vol;

	snd_mixer_open(&mixer, 0);
	snd_mixer_attach(mixer, devName);
	snd_mixer_selem_register(mixer, NULL, NULL);
	snd_mixer_load(mixer);
	pcm_element = snd_mixer_first_elem(mixer);
	if(NULL == pcm_element){
		fprintf(stdout, "mixer: snd_mixer_first_elem get fail\n");
		return -1;
	}
	snd_mixer_selem_get_playback_volume_range(pcm_element,
			&alsa_min_vol,
			&alsa_max_vol);
	snd_mixer_selem_set_playback_volume_range(pcm_element, 0, 100);
	//snd_mixer_selem_set_playback_volume(pcm_element,
	//		SND_MIXER_SCHN_FRONT_LEFT,
	//		vol);
	//snd_mixer_selem_set_playback_volume(pcm_element,
	//		SND_MIXER_SCHN_FRONT_RIGHT,
	//		vol);
	if(0 != snd_mixer_selem_set_playback_volume_all(pcm_element, vol)){
		fprintf(stdout, "mixer: snd_mixer_selem_set_playback_volume_all fail\n");
		return -2;
	}
	fprintf(stdout, "mixer: set playback volume(%d) success\n", vol);
	snd_mixer_close(mixer);
	return vol;
}


int main (int argc, char *argv[])
{
	int i,j;
	int test = 0;
	int err;
	int volume = 90;
	const int buffer_frames = FRAMElEN;

	void *m_mic_array_obj = NULL;

	int i_ch_spk = REF_NUM;
	int i_ch_out = 1;
	int frameLength = FRAMElEN;
	int profileID = 0;	
	int aec_sys_latency = 0;
	float mic_dist = 0.058;  //distance between 2 mics

	short inputMicData16s[FRAMElEN*MIC_NUM];
	short inputBeamformingData16s[FRAMElEN*MIC_NUM_BEAMFORMING];
	short outData16s[FRAMElEN];
	short *outDataMitChannel16s = NULL;
	float inputMicData32f[FRAMElEN*MIC_NUM_BEAMFORMING];
	float inputSpkData32f[FRAMElEN*REF_NUM];
	float outputData32f[FRAMElEN];
	char *playbackDev= NULL;
	char *captureDev = NULL;
	unsigned int output_channel_num = 1;
#ifdef DUMP_DATA
	FILE *dumpOutput = NULL;
	FILE *dumpOutput3 = NULL;
#endif
#ifdef PRINT_TIME
	struct timeval us1;
	struct timeval us2;
	int total = 0;
	int count = 0;
#endif

	if(argc < 5){
		printUsage();
	}
	captureDev = argv[1];
	playbackDev = argv[2];
	sscanf(argv[3], "%d", &output_channel_num);
	if (strcmp(argv[4], "on") == 0){
		beamformingIsOn = 1;
		printf("beamformingIsOn is turn on\n");
	}else{
		printf("beamformingIsOn is turn off\n");
	}
	sscanf(argv[5], "%d", &volume);

	signal(SIGPIPE,SIG_IGN);
	fprintf(stdout, "SIGPIPE signal ignore\n");

	recordPrepare(captureDev, MIC_NUM);

	playbackPrepare(playbackDev, output_channel_num);

	setPlaybackVolume(playbackDev, volume);

	if(beamformingIsOn == 0){
		if(1 != output_channel_num){
			outDataMitChannel16s = (short *)malloc(FRAMElEN * output_channel_num * sizeof(short));
		}
		while (1) 
		{
			if ((err = snd_pcm_readi (capture_handle, inputMicData16s, buffer_frames)) != buffer_frames) 
			{
				fprintf (stderr, "read from audio interface failed (%s)\n",
						err, snd_strerror (err));
				exit (1);
			}

			if(1 == output_channel_num){
				// change to 1 channel
				for(i = 0; i < buffer_frames; i++){
					outData16s[i] = inputMicData16s[i * MIC_NUM];
				}
				err = snd_pcm_writei (playback_handle, outData16s, buffer_frames);
			}else{
				//changeData
				for(i = 0; i < buffer_frames; i++){
					for(j = 0; j < output_channel_num; j++){
						memcpy(outDataMitChannel16s+i * output_channel_num+j, &inputMicData16s[i * MIC_NUM + j], sizeof(short));
					}
				}
				err = snd_pcm_writei (playback_handle, outDataMitChannel16s, buffer_frames);
			}

			if (err != buffer_frames) {
				if (err < 0){
					err = snd_pcm_recover(playback_handle, err, 0);
				}
				if(-EPIPE == err){
					fprintf(stderr, "playback: doing snd_pcm_writei err=%d, %s.\n", err, snd_strerror(err));
					if ((err = snd_pcm_prepare (playback_handle)) < 0) {
						fprintf (stderr, "playback: cannot prepare audio interface for use (%s)\n",
								snd_strerror (err));
						exit (1);
					}
					fprintf(stdout, "playback: audio interface prepared\n");
				}else if(-EAGAIN == err){
					fprintf(stderr, "playback: doing snd_pcm_writei err=%d, %s.\n", err, snd_strerror(err));
					continue;
				}else if (err >= 0 && err < buffer_frames){
					fprintf(stderr, "playback: Short write (expected %li, wrote %li)\n", buffer_frames, err);
					//snd_pcm_wait(playback_handle, 100);
					//fprintf(stderr, "playback: snd_pcm_wait\n");
					//if ((err = snd_pcm_prepare (playback_handle)) < 0) {
					//	fprintf (stderr, "playback: cannot prepare audio interface for use (%s)\n",
					//			snd_strerror (err));
					//	exit (1);
					//}
					//fprintf(stdout, "playback: audio interface prepared\n");
				}else{
					fprintf(stderr, "playback: something is error, err= %d, %s.\n", err, snd_strerror(err));
					if ((err = snd_pcm_prepare (playback_handle)) < 0) {
						fprintf (stderr, "playback: cannot prepare audio interface for use (%s)\n",
								snd_strerror (err));
						exit (1);
					}
					fprintf(stdout, "playback: audio interface prepared\n");
					//continue;
					//exit(1);
				}
			}
		}
	}else{

		/** Create and initialize mic_array object. */
		m_mic_array_obj = micArrayInit(profileID, frameLength, rate, MIC_NUM_BEAMFORMING, mic_dist, i_ch_spk, aec_sys_latency);
		if (NULL == m_mic_array_obj) {
			printf("Error: failed to initialize mic_array object.\n");
		}
		/** Set mic_array status. */
		micArraySetStatus(m_mic_array_obj, 1);   //1: wakeup    0: nowakeup   1:beamforming

		/** Set Beamforming */
		micArraySetBeamforming(m_mic_array_obj, 7, -10, 0, 0, 1.0, 30, 0);

		/** initialize inputSpkData32f. */
		memset(inputSpkData32f, 0, FRAMElEN * REF_NUM * sizeof(float));

		if(1 != output_channel_num){
			outDataMitChannel16s = (short *)malloc(FRAMElEN * output_channel_num * sizeof(short));
		}
#ifdef DUMP_DATA
		if ((dumpOutput = fopen("./dump_data.pcm", "wb")) == NULL)
		{
			printf("cannot open dump_data file for recording\n");
			exit (1);
		} 
		fprintf(stdout, "capture: dump_data file for recording opened\n");

		if ((dumpOutput3 = fopen("./dump_data3.pcm", "wb")) == NULL)
		{
			printf("cannot open dump_data3 file for recording\n");
			exit (1);
		} 
		fprintf(stdout, "capture: dump_data3 file for recording opened\n");
#endif

		while (1) {
			static const float scale = 1.f / (float)(MAXABS16S);

#ifdef PRINT_TIME
			gettimeofday(&us1,NULL);
#endif
			if ((err = snd_pcm_readi (capture_handle, inputMicData16s, buffer_frames)) != buffer_frames) 
			{
				fprintf (stderr, "read from audio interface failed (%s)\n",
						err, snd_strerror (err));
				exit (1);
			}

			/** Convert 4 channel to 3 channel. */
			for (i = 0; i < frameLength; i++) {
				for (j = 0; j < MIC_NUM_BEAMFORMING; j++) {
					inputBeamformingData16s[i*3+j] = inputMicData16s[4*i+j];
				}
			}
			/** Convert input data from 16s to 32f. */
			for (i = 0; i < frameLength; i++) {
				inputMicData32f[3 * i + 0] = (float)inputBeamformingData16s[3 * i + 0] * scale * 1.29 * 20;
				inputMicData32f[3 * i + 1] = (float)inputBeamformingData16s[3 * i + 1] * scale * 0.78 * 20;
				inputMicData32f[3 * i + 2] = (float)inputBeamformingData16s[3 * i + 2] * scale * 20;
			}

			/**
			 * mic_array process.
			 */
			micArrayProcess(m_mic_array_obj, inputMicData32f, outputData32f, inputSpkData32f);

			//float angle = micArrayGetCurrentDoa(m_mic_array_obj);
			//printf("estimated direction of arrival = %.1f\n ", angle);

			/** Convert input data from 32f to 16s. */
			for (i = 0; i < FRAMElEN; i++) {
				int val = (int)(outputData32f[i] * (float)MAXABS16S);
				if (val >(int)MAX16S) {
					outData16s[i] = MAX16S;
				}
				else if (val < (int)MIN16S) {
					outData16s[i] = MIN16S;
				}
				else {
					outData16s[i] = (short)val;
				}
			}
			/*
			test++;
			if(test > 500){
				printf("delay 10\n");
				struct timeval delay;  
				delay.tv_sec = 0;  
				delay.tv_usec = 10 * 1000; // 20 ms  
				select(0, NULL, NULL, NULL, &delay);
				test = 0;
			}
			*/

#ifdef DUMP_DATA
			fwrite(outData16s, sizeof(short), buffer_frames, dumpOutput);
			fflush(dumpOutput);
			fwrite(inputBeamformingData16s, sizeof(short) * MIC_NUM_BEAMFORMING, buffer_frames, dumpOutput3);
			fflush(dumpOutput3);
#endif

#ifdef PRINT_TIME
			gettimeofday(&us2,NULL);
			printf("TWIR_TIME=%u\n",((us2.tv_sec*1000000 + us2.tv_usec) - (us1.tv_sec*1000000 + us1.tv_usec)));
#endif
			if(1 == output_channel_num){
				err = snd_pcm_writei (playback_handle, outData16s, buffer_frames);
			}else{
				//changeData
				for(i = 0; i < buffer_frames; i++){
					for(j = 0; j < output_channel_num; j++){
						memcpy(outDataMitChannel16s+i * output_channel_num+j, &outData16s[i], sizeof(short));
					}
				}
				err = snd_pcm_writei (playback_handle, outDataMitChannel16s, buffer_frames);
			}
			if (err != buffer_frames) {
				if (err < 0){
					err = snd_pcm_recover(playback_handle, err, 0);
				}
				if(-EPIPE == err){
					fprintf(stderr, "playback: beamforming doing snd_pcm_writei err=%d, %s.\n", err, snd_strerror(err));
					if ((err = snd_pcm_prepare (playback_handle)) < 0) {
						fprintf (stderr, "playback: beamforming cannot prepare audio interface for use (%s)\n",
								snd_strerror (err));
						exit (1);
					}
					fprintf(stdout, "playback: beamforming audio interface prepared\n");
				}else if(-EAGAIN == err){
					fprintf(stderr, "playback: beamforming doing snd_pcm_writei err=%d, %s.\n", err, snd_strerror(err));
					continue;
				}else if (err >= 0 && err < buffer_frames){
					//fprintf(stderr, "playback: beamforming Short write (expected %li, wrote %li)\n", buffer_frames, err);
					//snd_pcm_wait(playback_handle, 100);
					//fprintf(stderr, "playback: beamforming snd_pcm_wait\n");
					if ((err = snd_pcm_prepare (playback_handle)) < 0) {
						fprintf (stderr, "playback: beamforming cannot prepare audio interface for use (%s)\n",
								snd_strerror (err));
						exit (1);
					}
					fprintf(stdout, "playback: beamforming audio interface prepared\n");
				}else{
					fprintf(stderr, "playback: beamforming something is error, err= %d, %s.\n", err, snd_strerror(err));
					if ((err = snd_pcm_prepare (playback_handle)) < 0) {
						fprintf (stderr, "playback: beamforming cannot prepare audio interface for use (%s)\n",
								snd_strerror (err));
						exit (1);
					}
					fprintf(stdout, "playback: beamforming audio interface prepared\n");
					//continue;
					//exit(1);
				}
			}
		}
	}

	snd_pcm_close (capture_handle);
	fprintf(stdout, "capture: audio interface closed\n");
	snd_pcm_close (playback_handle);
	fprintf(stdout, "playback: audio interface closed\n");

	if (NULL != m_mic_array_obj) { micArrayRelease(m_mic_array_obj); m_mic_array_obj = NULL; }
	exit (0);
	return 0;
}
