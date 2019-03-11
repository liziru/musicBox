#include "TtsService.h"
#include "log.h"
#include "../api/BaiduVoice.h"
#include "wiringPi.h"
#include "gpio.h"
#include "AudioPreprocessDispatcher.h"

TtsService::TtsService(PlayBackAudio *playBack) : playBack(playBack)
{
    this->wakeupEvent = new WakeupEvent();
    this->wakeupEvent->setAngle(0);
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}
TtsService::~TtsService()
{
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    if (wakeupEvent)
    {
        delete wakeupEvent;
        wakeupEvent = NULL;
    }
}
void TtsService::run()
{
    pthread_create(&this->launchThread, NULL, launchProcess, (void *)this);
}
void *TtsService::launchProcess(void *p)
{
    pthread_detach(pthread_self());
    // digitalWrite(SPEAK_LED, HIGH);

    TtsService *ttsLauncher = (TtsService *)p;
    int tmpAngle = 0;
    (int)ttsLauncher->wakeupEvent->getAngle();
#ifdef VERBOSE
    macroFuncVargs("wakeup, angle:%f", tmpAngle);
#endif //VERBOSE

    BaiduVoice *bdVoice = new BaiduVoice();
    TtsResult *ttsResult = NULL;
    string wordContent;

    while (1)
    {
        pthread_mutex_lock(&ttsLauncher->mutex);
        pthread_cond_wait(&ttsLauncher->cond, &ttsLauncher->mutex);
        pthread_mutex_unlock(&ttsLauncher->mutex);

        if (ttsLauncher->wakeupEvent->getWord().length() == 0)
        {
            tmpAngle = (int)ttsLauncher->wakeupEvent->getAngle();
            wordContent = string("你好，我在。声源方向：") + to_string(tmpAngle) + string("度");
        }
        else
            wordContent = ttsLauncher->wakeupEvent->getWord();

        ttsResult = bdVoice->voiceTts(wordContent);
        if (ttsResult->getIsError())
        {
            macroFuncVargs("tts error:%s", ttsResult->getErrorMsg().c_str());
        }
        else
        {
            // macroFuncVargs("TtsService::laun chProcess: mpg123 filename(%s)", ttsResult->getTtsVoiceFilename().c_str());
            // string cmd = string("mpg123 " + ttsResult->getTtsVoiceFilename());
            // system(cmd.c_str());
            // sleep(1);
            // macroFuncVargs("ATTENTION: tts is finished,file name:%s", ttsLauncher->wakeupEvent->getPbFile().c_str());

            ttsLauncher->playBack->start(); //开启音乐播放
            ttsLauncher->wakeupEvent->setPbFile(ttsResult->getTtsVoiceFilename());
            macroFuncVargs("ATTENTION: tts is finished,file name:%s", ttsLauncher->wakeupEvent->getPbFile().c_str());

            ttsLauncher->playBack->onWakeup(ttsLauncher->wakeupEvent);

            // macroFunc(" ## Start to record.");
        }
    }

    //start to record voice and asr-ali
    // AudioPreprocessDispatcher::APDLEVEL = 1; //asr

    // digitalWrite(SPEAK_LED, LOW);
    delete ttsResult;
    delete bdVoice;
    return NULL;
}

void TtsService::onWakeup(WakeupEvent *wakeupEvent)
{
    this->wakeupEvent->setAngle(wakeupEvent->getAngle());
    this->wakeupEvent->setWord(wakeupEvent->getWord());

    pthread_cond_signal(&cond);
}
