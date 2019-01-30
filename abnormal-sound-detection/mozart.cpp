#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdbool.h>
#include "alsa/asoundlib.h"
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <cstdlib>
#include "wiringPi.h"
#include <iostream>
#include "log.h"
#include "fifo.h"
#include "TwirlingCapture.h"
#include "TwirlingWakeupApi.h"
#include "MicDataSource/MicDataSource.h"
#include "AudioService/AudioPreprocessDispatcher.h"
#include "AudioService/IWakeupService.h"
#include "AudioService/WakeupService.h"
#include "AudioService/LedService.h"
#include "wiringPi.h"
#include "gpio.h"
#include "PlayBackAudio.h"

using namespace std;

int main(int argc, char *argv[])
{
	// 单mic
	// MicDataSource *micDataSource = MicDataSource::getInstance(string("hw:0"), 16000, 1, SND_PCM_FORMAT_S16_LE,
	// 	1, 4, 0.06);

	// 4 mic 线
	MicDataSource *micDataSource = MicDataSource::getInstance(string("hw:0"), 16000, 1, SND_PCM_FORMAT_S16_LE, 1, 4, 0.06);
	PlayBackAudio *playBackAudio = PlayBackAudio::getInstance(string("hw:0"), 16000, 1, SND_PCM_FORMAT_S16_LE, 1, 4, 0.06);
	IWakeupService *wakeupService = new IWakeupService(1e-15, 1e-1, 300, 2000);
	AudioPreprocessDispatcher *audioPreprocessDispatcher = new AudioPreprocessDispatcher(false, false, -15, 1);
	LedService *ledService = new LedService(1, HIGH, 1500);

	audioPreprocessDispatcher->setMicDataSource(micDataSource);
	audioPreprocessDispatcher->addAudioPreprocessListenner((AudioPreprocessListenner *)wakeupService);
	audioPreprocessDispatcher->dispatcherAudioData();

	wakeupService->addWakeupListenner((WakeupListenner *)ledService);
	// wakeupService->addWakeupListenner((WakeupListenner *)playBackAudio);

	wakeupService->run();
	ledService->run();

	while (1)
	{
		sleep(1);
	}

	//delete asrService;
	delete wakeupService;
	delete audioPreprocessDispatcher;
	return 0;
}
