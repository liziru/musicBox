#include "TtsService.h"
#include "log.h"
#include "../api/BaiduVoice.h"
#include "wiringPi.h"
#include "gpio.h"
#include "AudioPreprocessDispatcher.h"

TtsService::TtsService() : launchThread(0), wakeupEvent(NULL)
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
    delete wakeupEvent;
}
void TtsService::run()
{
    pthread_create(&this->launchThread, NULL, launchProcess, (void *)this);
}
void *TtsService::launchProcess(void *p)
{
    pthread_detach(pthread_self());
    digitalWrite(SPEAK_LED, HIGH);
    TtsService *ttsLauncher = (TtsService *)p;
    int tmpAngle = (int)ttsLauncher->wakeupEvent->getAngle();
#ifdef VERBOSE
    macroFuncVargs("wakeup, angle:%f", tmpAngle);
#endif //VERBOSE

    BaiduVoice *bdVoice = new BaiduVoice();
    TtsResult *ttsResult = NULL;

    while (1)
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        Pthread_mutex_unlock(&mutex);

        ttsResult = bdVoice->voiceTts(string("你好，我在。声源方向：") + to_string(tmpAngle) + string("度"));
        if (ttsResult->getIsError())
        {
            macroFuncVargs("tts error:%s", ttsResult->getErrorMsg().c_str());
        }
        else
        {
            string cmd = string("mpg123 " + ttsResult->getTtsVoiceFilename());
            system(cmd.c_str());
        }
    }

    //start to record voice and asr-ali
    AudioPreprocessDispatcher::APDLEVEL = 1; //asr

    digitalWrite(SPEAK_LED, LOW);
    delete ttsResult;
    delete bdVoice;
    return NULL;
}

void TtsService::onDataArrival(string word, float angle)
{
    this->wakeupEvent->setAngle(angle);
    this->wakeupEvent->setWord(word);

    pthread_cond_signal(&cond);
}