
#include <stdio.h>
#include <stdlib.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <sys/time.h>
// #include <stdbool.h>
// #include "alsa/asoundlib.h"
#include <signal.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <string.h>
// #include <assert.h>
// #include <cstdlib>
// #include "wiringPi.h"
// #include <iostream>

#include "log.h"
#include "wiringPi.h"
#include "gpio.h"
// #include "fifo.h"
// #include "TwirlingCapture.h"
// #include "TwirlingWakeupApi.h"
#include "MicDataSource/MicDataSource.h"
// #include "AudioService/AudioPreprocessDispatcher.h"
#include "AudioService/IWakeupService.h"
// #include "AudioService/WakeupService.h"
// #include "AudioService/LedService.h"
#include "AudioService/TtsService.h"
#include "SemanticsAnalysis.h"
#include "NeteaseMusicService.h"
#include "NeteaseMusicLanucher.h"
#include "DownloadService.h"
#include "PlayBackAudio.h"
#include "AsrService.h"
#include "musicBoxMain.h"
#include "threadFunc.h"

// using namespace std;

DISPATCHER dispatcher;

int main(int argc, char *argv[])
{
    macroFunc("\n===============main start.================ \n");

    // 变量定义
    pthread_t th_capture, th_playBack, th_awakeUp, th_tts, th_asr, th_sm, th_neteaseService, th_neteaseLanucher, th_download, th_audioPreprocessDispatcher;
    int err_capture = 0, err_audioPreprocessDispatcher = 0, err_awakeUp = 0, err_tts = 0, err_asr = 0, err_sm = 0, err_neteaseService = 0, err_neteaseLanucher = 0, err_download = 0, err_playBack = 0;

    // sigignore(SIGSEGV);

    initThreadArgu();

    PlayBackAudio *playBack = PlayBackAudio::getInstance(string("hw:0"), 16000, 1, SND_PCM_FORMAT_S16_LE, 1, 4, 0.06);
    AsrService *asrService = new AsrService();
    TtsService *ttsService = new TtsService(playBack);

    SemanticsAnalysis *sa = new SemanticsAnalysis();
    NeteaseMusicService *neteaseService = new NeteaseMusicService();
    DownloadService *downloadService = new DownloadService();
    NeteaseMusicLanucher *neteaseLanucher = new NeteaseMusicLanucher(playBack, downloadService, ttsService);

    MicDataSource *micDataSource = MicDataSource::getInstance(string("hw:0"), 16000, 1, SND_PCM_FORMAT_S16_LE, 1, 4, 0.06);
    IWakeupService *iWakeupService = new IWakeupService(1e-15, 1e-1, 300, 2000, &dispatcher, ttsService);
    FUNC_POINTER funcPointer;
    funcPointer.iwps = (void *)iWakeupService;
    funcPointer.mds = (void *)micDataSource;
    funcPointer.asr = (void *)asrService;

    // macroFunc("main:test01.");

    asrService->addSaListeners((WakeupListenner *)sa);
    sa->addNeteaseListeners((WakeupListenner *)neteaseService);
    neteaseService->addNeteaseLanucherListeners((WakeupListenner *)neteaseLanucher);

    neteaseLanucher->run();
    downloadService->run();
    neteaseService->run();
    sa->run();
    asrService->run();
    iWakeupService->run();
    ttsService->run();
    playBack->run();

    // pthread_create(&this->th_capture, NULL, capture_process, NULL);
    pthread_create(&th_audioPreprocessDispatcher, NULL, audioPreprocessDispatcher_thread, (void *)&funcPointer);
    // pthread_create(&th_awakeUp, NULL, aWakeUp_thread, NULL);
    // pthread_create(&th_tts, NULL, tts_thread, NULL);

    // pthread_create(&th_asr, NULL, asr_thread, NULL);
    // pthread_create(&th_sm, NULL, sm_thread, NULL);
    // pthread_create(&th_neteaseService, NULL, neteaseSerive_thread, NULL);
    // pthread_create(&th_neteaseLanucher, NULL, neteaseLanucher_thread, NULL);
    // pthread_create(&th_download, NULL, download_thread, NULL);
    // pthread_create(&th_playBack, NULL, playback_thread, NULL);

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

        // if (err_awakeUp == ESRCH) //3
        // {
        //     macroFunc("main: aWakeUp_thread error exit.");
        //     pthread_create(&th_awakeUp, NULL, aWakeUp_thread, NULL;
        //     macroFunc("main: restart the thread.");
        //     err_awakeUp = 0;
        // }
        // else if (err_awakeUp == 0)
        // {
        //     err_awakeUp = pthread_kill(th_awakeUp, 0);
        // }

        // if (err_tts == ESRCH)
        // {
        //     macroFunc("main: tts_thread error exit.");
        //     pthread_create(&th_tts, NULL, tts_thread, NULL);
        //     macroFunc("main: restart the thread.");
        //     err_tts = 0;
        // }
        // else if (err_tts == 0)
        // {
        //     err_tts = pthread_kill(th_tts, 0);
        // }

        // if (err_asr == ESRCH)
        // {
        //     macroFunc("main: asr_thread error exit.");
        //     pthread_create(&th_asr, NULL, asr_thread, NULL);
        //     macroFunc("main: restart the thread.");
        //     err_asr = 0;
        // }
        // else if (err_asr == 0)
        // {
        //     err_asr = pthread_kill(th_asr, 0);
        // }

        // if (err_sm == ESRCH)
        // {
        //     macroFunc("main: sm_thread error exit.");
        //     pthread_create(&th_sm, NULL, sm_thread, NULL);
        //     macroFunc("main: restart the thread.");
        //     err_sm = 0;
        // }
        // else if (err_sm == 0)
        // {
        //     err_sm = pthread_kill(th_sm, 0);
        // }

        // if (err_neteaseService == ESRCH)
        // {
        //     macroFunc("main: neteaseSerive_thread error exit.");
        //     pthread_create(&th_neteaseService, NULL, neteaseSerive_thread, NULL);
        //     macroFunc("main: restart the thread.");
        //     err_neteaseService = 0;
        // }
        // else if (err_neteaseService == 0)
        // {
        //     err_neteaseService = pthread_kill(th_neteaseService, 0);
        // }

        // if (err_neteaseLanucher == ESRCH)
        // {
        //     macroFunc("main: neteaseLanucher_thread error exit.");
        //     pthread_create(&th_neteaseLanucher, NULL, neteaseLanucher_thread, NULL);
        //     macroFunc("main: restart the thread.");
        //     err_neteaseLanucher = 0;
        // }
        // else if (err_neteaseLanucher == 0)
        // {
        //     err_neteaseLanucher = pthread_kill(th_neteaseLanucher, 0);
        // }

        // if (err_download == ESRCH)
        // {
        //     macroFunc("main: download_thread error exit.");
        //     pthread_create(&th_download, NULL, download_thread, NULL);
        //     macroFunc("main: restart the thread.");
        //     err_download = 0;
        // }
        // else if (err_download == 0)
        // {
        //     err_download = pthread_kill(th_download, 0);
        // }

        // if (err_playBack == ESRCH)
        // {
        //     macroFunc("main: playback_thread error exit.");
        //     pthread_create(&th_playBack, NULL, playback_thread, NULL);
        //     macroFunc("main: restart the thread.");
        //     err_playBack = 0;
        // }
        // else if (err_playBack == 0)
        // {
        //     err_playBack = pthread_kill(th_playBack, 0);
        // }

        sleep(1);
    }

    if (micDataSource)
    {
        delete micDataSource;
        micDataSource = NULL;
    }
    destroyThreadArgu();

    delete downloadService;
    delete neteaseLanucher;
    delete neteaseService;
    delete sa;
    delete asrService;
    delete micDataSource;
    delete ttsService;
    delete iWakeupService;
    delete playBack;

    return 0;
}
