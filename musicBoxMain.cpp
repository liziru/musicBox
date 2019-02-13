
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

    // #ifndef USER_DEBUG
    //     int canpass = getPass();
    //     if (canpass != 1)
    //     {
    //         macroFunc("failed start.");
    //         return 0;
    //     }
    // #endif
    macroFunc("\n===============================\n main start.\n");

    // 变量定义
    pthread_t th_capture, th_playback, th_awakeup, th_asr;
    // int capture_retval = 0;
    // int playback_retval = 0;
    // sounder_param *snd_param = (sounder_param *)malloc(sizeof(sounder_param));
    // snd_param_bak = snd_param;

    short *sArray = (short *)malloc(sizeof(short) * FRAMELEN * FIFO_BUFF_COUNT);
    snd_param->pfifo = &snd_param->fifo;
    FIFO_Init(snd_param->pfifo, sArray, sizeof(short) * FRAMELEN, FIFO_BUFF_COUNT);

    //     if (argc < 19)
    //     {
    //         printUsage();
    //     }
    //     //add 2018-4-10
    //     struct sched_param param_capture, param_playback;
    //     pthread_attr_t attr_capture, attr_playback;
    //     pthread_attr_init(&attr_capture);
    //     pthread_attr_init(&attr_playback);
    // #ifndef USER_DEBUG
    //     param_capture.sched_priority = 90;
    //     param_playback.sched_priority = 80;
    //     pthread_attr_setschedpolicy(&attr_capture, SCHED_RR);
    //     pthread_attr_setschedparam(&attr_capture, &param_capture);
    //     pthread_attr_setinheritsched(&attr_capture, PTHREAD_EXPLICIT_SCHED);

    //     pthread_attr_setschedpolicy(&attr_playback, SCHED_RR);
    //     pthread_attr_setschedparam(&attr_playback, &param_playback);
    //     pthread_attr_setinheritsched(&attr_playback, PTHREAD_EXPLICIT_SCHED);
    // #endif

    //     setTimer();
    //     initAudioParam(argv, snd_param);
    //     snd_param->is_first_wait = true;
    //     pthread_cond_init(&snd_param->cond, NULL);
    //     pthread_mutex_init(&snd_param->mutex, NULL);

    // sigignore(SIGSEGV);

    while (1)
    {

        FIFO_Clear(snd_param->pfifo);
        /* 创建线程*/
        // pthread_create(&th_capture, &attr_capture, capture_process, (void *)snd_param);
        pthread_create(&this->th_capture, NULL, capture_process, NULL);

        pthread_mutex_lock(&snd_param->mutex);
        pthread_cond_wait(&snd_param->cond, &snd_param->mutex);
        pthread_mutex_unlock(&snd_param->mutex);
        pthread_create(&th_playback, &attr_playback, playback_process, (void *)snd_param);
        /* 等待两个线程结束*/
        pthread_join(th_playback, (void **)&playback_retval);
        macroFuncVargs("main: playback_retval:%d", (int)playback_retval);
        if ((int)playback_retval < 0)
        {
            pthread_cancel(th_capture);
        }

        sleep(3);
        thread_log_tag = 1;
        while (thread_exit(&th_capture))
        {
            usleep(100);
        }
    }

    pthread_attr_destroy(&attr_capture);
    pthread_attr_destroy(&attr_playback);

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

void *capture_process(void *p)
{
    pthread_detach(pthread_self());

    MicDataSource *micDataSource = (MicDataSource *)p;
    micDataSource->isRun = true;
    int err = 0;
    short *inputMicData16s = NULL;
    inputMicData16s = (short *)malloc(sizeof(short) * FRAMELEN * micDataSource->channel);

#ifdef DUMP_PCM_DATA
    FILE *dumpReadOutput = NULL;
    if ((dumpReadOutput = fopen("./dump_raw.pcm", "wb")) == NULL)
    {
        macroFunc("wakeup: cannot open dump_raw file for wakeup");
        exit(1);
    }
#endif
    macroFunc("capture start. ");

    while (micDataSource->isRun)
    {
        if ((err = snd_pcm_readi(micDataSource->capture_handle, inputMicData16s, FRAMELEN)) != FRAMELEN)
        {
            // read from audio interface failed -77, (File descriptor in bad state)
            macroFuncVargs("capture: read from audio interface failed %d, (%s)",
                           err, snd_strerror(err));
            if (-EPIPE == err || err == -77)
            {
                if ((err = snd_pcm_prepare(micDataSource->capture_handle)) < 0)
                {
                    macroFuncVargs("capture: cannot prepare audio interface for use (%s)",
                                   snd_strerror(err));
                    break;
                }
                macroFunc("capture: audio interface prepared");
                continue;
            }
            else if (-19 == err)
            {
                macroFunc("capture: audio readi error, Exit(-12)");
                break;
            }
            else
            {
                macroFunc("capture: audio readi error, Exit(-13)");
                break;
            }
        }

        if (0 == FIFO_AddOne(micDataSource->pfifo, inputMicData16s))
        {
            macroFunc("capture: buf_write fail, buffer is full.");
        }

#ifdef DUMP_PCM_DATA
        fwrite(inputMicData16s, sizeof(short), MicDataSource::FRAMELEN * micDataSource->getChannel(), dumpReadOutput);
        fflush(dumpReadOutput);
#endif
    }
    if (NULL != inputMicData16s)
    {
        free(inputMicData16s);
        inputMicData16s = NULL;
    }

    if (NULL != micDataSource->capture_hw_params)
    {
        snd_pcm_hw_params_free(micDataSource->capture_hw_params);
        micDataSource->capture_hw_params = NULL;
    }
    if (NULL != micDataSource->capture_handle)
    {
        snd_pcm_drain(micDataSource->capture_handle);
        snd_pcm_close(micDataSource->capture_handle);
    }
    if (NULL != micDataSource->sArray)
    {
        free(micDataSource->sArray);
        micDataSource->sArray = NULL;
    }
    return NULL;
}
