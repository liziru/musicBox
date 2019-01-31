#include "WakeupService.h"
#include "AudioPreprocessDispatcher.h"
#include "log.h"
#include "TwirlingWakeupApi.h"
#include <iostream>
using namespace std;

const int WakeupService::ANGLES_MIX_SIZE = 20;
WakeupService::WakeupService(float wakeupThreshold, float wakeupPlp, float wakeupDmin, float wakeupDmax)
	:wakeupThreshold(wakeupThreshold), wakeupPlp(wakeupPlp), wakeupDmin(wakeupDmin), wakeupDmax(wakeupDmax),
	 th_wakeup(0), isRun(false), audioData(NULL), m_wakeup_obj(NULL)
{
    pthread_cond_init(&this->dataArrivalCond, NULL);
    pthread_mutex_init(&this->dataArrivalMutex, NULL);
    /** init wakeup object */
    m_wakeup_obj = WakeupInit(wakeupThreshold, wakeupPlp, wakeupDmin, wakeupDmax);
}
WakeupService::~WakeupService()
{
    isRun = false;
}

void WakeupService::addWakeupListenner(WakeupListenner *listenner)
{
    this->wakeupListenners.push_back(listenner);
}
void WakeupService::removeWakeupListenner(WakeupListenner *listenner)
{
    this->wakeupListenners.remove(listenner);
}
bool WakeupService::getIsRun()
{
    return this->isRun;
}
void WakeupService::stop()
{
    this->isRun = false;
}

void WakeupService::run()
{
    if (!this->isRun)
    {
        pthread_create(&this->th_wakeup, NULL, wakeupProcess, (void *)this);
    }
}
short *WakeupService::getAudioDataAddr()
{
    if (NULL == audioData)
    {
        audioData = (short *)malloc(MicDataSource::FRAMELEN * sizeof(short));
    }
    return audioData;
}
float WakeupService::calcWakeupAngle()
{
    float sum = 0;
    int size = angles.size();
    int i = 0;
    list<float>::iterator it = angles.begin();
    // cout <<"------------Angle---------------" << endl;
    for (; it != angles.end(); ++it)
    {
        sum += *it;
        i++;
        if(i >= size)
        {
            break;
        }
        // cout << *it << " ";
    }
    // cout << endl <<"------------------------------" << endl;
    if(size == 0)
    {
        return 0;
    }
#ifdef DUMP_WAKEUP_DATA
    FILE *dumpWakeupDataOutput = NULL;
    if ((dumpWakeupDataOutput = fopen("/tmp/dump_wakeup_data.pcm", "wb")) == NULL)
    {
        macroFunc("wakeup: cannot open dump_wakeup_data file for wakeup");
        exit(1);
    }
    vector<short>::iterator iter = wakeupData.begin();
    for(; iter != wakeupData.end(); iter++)
    {
        fwrite(&(*iter), sizeof(short), 1, dumpWakeupDataOutput);
    }
    wakeupData.clear();
    fflush(dumpWakeupDataOutput);
    fclose(dumpWakeupDataOutput);
#endif
    return sum / angles.size();
}

#ifdef DUMP_WAKEUP_DATA
void WakeupService::pushBackWakeupData(short *data, const int count)
{
    for(int i = 0; i < count; i++)
    {
        wakeupData.push_back(data[i]);
    }
}
void WakeupService::popFrontWakeupData(const int count)
{
    if(wakeupData.size() >= count)
    {
        wakeupData.erase(wakeupData.begin(), wakeupData.begin()+count);
    }
}
#endif    
void WakeupService::onDataArrival(short *audioData, float angle)
{
    memcpy(getAudioDataAddr(), audioData, MicDataSource::FRAMELEN * sizeof(short));
    if(angles.size() >= ANGLES_MIX_SIZE){
        angles.pop_front();
    }
    angles.push_back(angle);
#ifdef DUMP_WAKEUP_DATA
    if(wakeupData.size() >= MicDataSource::FRAMELEN * ANGLES_MIX_SIZE)
    {
        popFrontWakeupData(MicDataSource::FRAMELEN);
    }
    pushBackWakeupData(audioData, MicDataSource::FRAMELEN);
#endif
    pthread_cond_signal(&dataArrivalCond);
}

void *WakeupService::wakeupProcess(void *p)
{
    pthread_detach(pthread_self());
    WakeupService *wakeupService = (WakeupService *)p;
    wakeupService->isRun = true;
    int isWakeup = 0;
    WakeupEvent *wakeupEvent = new WakeupEvent();

    while (wakeupService->isRun)
    {
        pthread_mutex_lock(&wakeupService->dataArrivalMutex);
        pthread_cond_wait(&wakeupService->dataArrivalCond, &wakeupService->dataArrivalMutex);
        pthread_mutex_unlock(&wakeupService->dataArrivalMutex);

        /** wakeup process */
		WakeupRun(wakeupService->m_wakeup_obj, wakeupService->getAudioDataAddr(), MicDataSource::FRAMELEN);
		isWakeup = GetWakeupStatus(wakeupService->m_wakeup_obj);
		if (isWakeup == 1){
			// nofity wakeup

            list<WakeupListenner *>::iterator it = wakeupService->wakeupListenners.begin();
            for (; it != wakeupService->wakeupListenners.end(); ++it)
            {
                wakeupEvent->setAngle(wakeupService->calcWakeupAngle());
                (*it)->onWakeup(wakeupEvent);
            }
		}else if(isWakeup == 0){
			//macroFunc("0\n");
		}else {
			// macroFunc("VAD\n");
		}
    }

    if (NULL != wakeupService->audioData)
    {
        free(wakeupService->audioData);
        wakeupService->audioData = NULL;
    }

    if (NULL != wakeupService->m_wakeup_obj)
    {
        WakeupRelease(wakeupService->m_wakeup_obj);
        wakeupService->m_wakeup_obj = NULL;
    }
    delete wakeupEvent;
    return NULL;
}
