
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
// #include "PlayBackAudio.h"
#include "AsrService.h"
#include "musicBoxMain.h"

using namespace std;
int main(int argc, char *argv[])
{
    macroFunc("\n===============================\n main start.\n");

    // 变量定义
    pthread_t th_capture, th_playBack, th_awakeUp, th_tts, th_asr, th_sm, th_neteaseService, th_neteaseLanucher, th_download, th_audioPreprocessDispatcher;
    int err_capture = 0, err_audioPreprocessDispatcher = 0, err_awakeUp = 0, err_tts = 0, err_asr = 0, err_sm = 0, err_neteaseService = 0, err_neteaseLanucher = 0, err_download = 0, err_playBack = 0;

    short *sArray = (short *)malloc(sizeof(short) * FRAMELEN * FIFO_BUFF_COUNT);
    snd_param->pfifo = &snd_param->fifo;
    FIFO_Init(snd_param->pfifo, sArray, sizeof(short) * FRAMELEN, FIFO_BUFF_COUNT);

    //     setTimer();
    //     initAudioParam(argv, snd_param);
    //     snd_param->is_first_wait = true;
    //     pthread_cond_init(&snd_param->cond, NULL);
    //     pthread_mutex_init(&snd_param->mutex, NULL);

    // sigignore(SIGSEGV);

    MicDataSource *micDataSource = MicDataSource::getInstance(string("hw:0"), 16000, 1, SND_PCM_FORMAT_S16_LE, 1, 4, 0.06);
    // pthread_create(&this->th_capture, NULL, capture_process, NULL);
    pthread_create(&this->th_audioPreprocessDispatcher, NULL, audioPreprocessDispatcher_thread, (void *)micDataSource));
    pthread_create(&this->th_awakeUp, NULL, aWakeUp_thread, NULL);
    pthread_create(&this->th_tts, NULL, tts_thread, NULL);
    pthread_create(&this->th_asr, NULL, asr_thread, NULL);
    pthread_create(&this->th_sm, NULL, sm_thread, NULL);
    pthread_create(&this->th_neteaseService, NULL, neteaseSerive_thread, NULL);
    pthread_create(&this->th_neteaseLanucher, NULL, neteaseLanucher_thread, NULL);
    pthread_create(&this->th_download, NULL, download_thread, NULL);
    pthread_create(&this->th_playBack, NULL, playback_thread, NULL);

    /* 等待线程结束*/
    while (1)
    {
        // if (err_capture == ESRCH)
        // {
        //     macroFunc("main: capture thread error exit.");
        //     pthread_create(&th_capture, NULL, captureProcess_thread, NULL);
        //     macroFunc("main: restart capture thread.");
        //     err_capture = 0;
        // }
        // else if (err_capture == 0)
        // {
        //     err_capture = pthread_kill(th_capture, 0);
        // }

        if (err_audioPreprocessDispatcher == ESRCH)
        {
            macroFunc("main: audioPreprocessDispatcher thread error exit.");
            pthread_create(&th_audioPreprocessDispatcher, NULL, audioPreprocessDispatcher_thread, (void *)micDataSource);
            macroFunc("main: restart the thread.");
            err_audioPreprocessDispatcher = 0;
        }
        else if (err_audioPreprocessDispatcher == 0)
        {
            err_audioPreprocessDispatcher = pthread_kill(th_audioPreprocessDispatcher, 0);
        }

        if (err_awakeUp == ESRCH) //3
        {
            macroFunc("main: aWakeUp_thread error exit.");
            pthread_create(&th_awakeUp, NULL, aWakeUp_thread, NULL;
            macroFunc("main: restart the thread.");
            err_awakeUp = 0;
        }
        else if (err_awakeUp == 0)
        {
            err_awakeUp = pthread_kill(th_awakeUp, 0);
        }

        if (err_tts == ESRCH)
        {
            macroFunc("main: tts_thread error exit.");
            pthread_create(&th_tts, NULL, tts_thread, NULL);
            macroFunc("main: restart the thread.");
            err_tts = 0;
        }
        else if (err_tts == 0)
        {
            err_tts = pthread_kill(th_tts, 0);
        }

        if (err_asr == ESRCH)
        {
            macroFunc("main: asr_thread error exit.");
            pthread_create(&th_asr, NULL, asr_thread, NULL);
            macroFunc("main: restart the thread.");
            err_asr = 0;
        }
        else if (err_asr == 0)
        {
            err_asr = pthread_kill(th_asr, 0);
        }

        if (err_sm == ESRCH)
        {
            macroFunc("main: sm_thread error exit.");
            pthread_create(&th_sm, NULL, sm_thread, NULL);
            macroFunc("main: restart the thread.");
            err_sm = 0;
        }
        else if (err_sm == 0)
        {
            err_sm = pthread_kill(th_sm, 0);
        }

        if (err_neteaseService == ESRCH)
        {
            macroFunc("main: neteaseSerive_thread error exit.");
            pthread_create(&th_neteaseService, NULL, neteaseSerive_thread, NULL);
            macroFunc("main: restart the thread.");
            err_neteaseService = 0;
        }
        else if (err_neteaseService == 0)
        {
            err_neteaseService = pthread_kill(th_neteaseService, 0);
        }

        if (err_neteaseLanucher == ESRCH)
        {
            macroFunc("main: neteaseLanucher_thread error exit.");
            pthread_create(&th_neteaseLanucher, NULL, neteaseLanucher_thread, NULL);
            macroFunc("main: restart the thread.");
            err_neteaseLanucher = 0;
        }
        else if (err_neteaseLanucher == 0)
        {
            err_neteaseLanucher = pthread_kill(th_neteaseLanucher, 0);
        }

        if (err_download == ESRCH)
        {
            macroFunc("main: download_thread error exit.");
            pthread_create(&th_download, NULL, download_thread, NULL);
            macroFunc("main: restart the thread.");
            err_download = 0;
        }
        else if (err_download == 0)
        {
            err_download = pthread_kill(th_download, 0);
        }

        if (err_playBack == ESRCH)
        {
            macroFunc("main: playback_thread error exit.");
            pthread_create(&th_playBack, NULL, playback_thread, NULL);
            macroFunc("main: restart the thread.");
            err_playBack = 0;
        }
        else if (err_playBack == 0)
        {
            err_playBack = pthread_kill(th_playBack, 0);
        }

        sleep(1);
    }

    if (NULL != snd_param)
    {
        free(snd_param);
        snd_param = NULL;
    }
    if (NULL != sArray)
    {
        free(sArray);
        sArray = NULL;
    }

    return 0;
}
