#include "IWakeupService.h"
#include "AudioPreprocessDispatcher.h"
#include "log.h"
//#include "TwirlingWakeupApi.h"
#include "wakeupApi.h"
#include "sqlite3.h"
#include "DeviceInfo.h"
#include <stdlib.h>
#include <iostream>
using namespace std;
#define MAXABS16S (32768)
#define BUFF_SIZE 1024
#define DATABASE_FILENAME "/home/pi/workspace/boa/www/cgi-bin/dataStorage"

static bool argsToDeviceInfo(int argc, char **values, char **name, DeviceInfo *deviceInfo)
{
    int index;
    for (index = 0; index < argc; index++)
    {
        if (0 == strcmp(name[index], "DEVICE_ID"))
        {
            if (NULL == values[index])
            {
                deviceInfo->setDeviceId("");
            }
            else
            {
                deviceInfo->setDeviceId(string(values[index]));
            }
        }
        else if (0 == strcmp(name[index], "DEVICE_DESC"))
        {
            if (NULL == values[index])
            {
                deviceInfo->setDeviceDesc("");
            }
            else
            {
                deviceInfo->setDeviceDesc(string(values[index]));
            }
        }
        else if (0 == strcmp(name[index], "VERSION"))
        {
            if (NULL == values[index])
            {
                deviceInfo->setVersion("");
            }
            else
            {
                deviceInfo->setVersion(string(values[index]));
            }
        }
    }
    return argc > 0;
}

static int getDeviceInfoByIdCallback(void *arg, int argc, char **values, char **name)
{
    int index;
    DeviceInfo *deviceInfo = (DeviceInfo *)arg;
    argsToDeviceInfo(argc, values, name, deviceInfo);
    return 0;
}

DeviceInfo selectDeviceInfo()
{
    DeviceInfo device;
    int count = 0;
    sqlite3 *db = NULL;
    string dbFileName = DATABASE_FILENAME;
    char *errmsg = NULL;
    char sql_buff[BUFF_SIZE] = {0};
    /* open or create database */
    if (SQLITE_OK != sqlite3_open(dbFileName.c_str(), &db))
    {
        macroFunc("sqlite3: sqlite3_open error exit.");
        return device;
    }
    // 查询用户
    memset(sql_buff, 0, sizeof(sql_buff));
    strcat(sql_buff, "select * from DEVICE_INFO;");

    sqlite3_busy_timeout(db, 8 * 1000); //最长等待8m ,也相当于延时
    if (SQLITE_OK != sqlite3_exec(db, sql_buff, getDeviceInfoByIdCallback, &device, &errmsg))
    {
        macroFuncVargs("sqlite3: sqlite3_exec error: %s", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return device;
    }
    sqlite3_close(db);
    return device;
}

const int IWakeupService::ANGLES_MIX_SIZE = 20;

IWakeupService::IWakeupService(float wakeupThreshold, float wakeupPlp, float wakeupDmin, float wakeupDmax, DISPATCHER *dp, TtsService *ttsService)
    : wakeupThreshold(wakeupThreshold), wakeupPlp(wakeupPlp), wakeupDmin(wakeupDmin), wakeupDmax(wakeupDmax),
      th_wakeup(0), isRun(false), audioData(NULL), m_wakeup_obj(NULL)
{
    pthread_cond_init(&this->dataArrivalCond, NULL);
    pthread_mutex_init(&this->dataArrivalMutex, NULL);

    /** init wakeup object */
    int stride_len = MicDataSource::FRAMELEN;
    int chs = 1;
    int sRate = 16000;
    float detect_th = 0.7;

    this->ttsService = ttsService;
    g_dispatcher = dp;

    m_wakeup_obj = wakeupInit(stride_len, chs, sRate, NULL, "", NULL, 2, detect_th, 1500, false);
    if (m_wakeup_obj != NULL)
    {
        wakeupSet(m_wakeup_obj, detect_th);
    }
    // macroFuncVargs("twirlingwakeupNN version: %s \n", wakeupGetVersion());
#ifdef DUMP_WAKEUP_DATA
    dumpWakeupDataOutput = NULL;
    if ((dumpWakeupDataOutput = fopen("./dump_wakeup_data.pcm", "wb+")) == NULL)
    {
        macroFunc("wakeup: cannot open dump_wakeup_data file for wakeup");
        dumpWakeupDataOutput = NULL;
        exit(1);
    }
#endif
}

IWakeupService::~IWakeupService()
{
    isRun = false;
#ifdef DUMP_WAKEUP_DATA
    if (NULL != dumpWakeupDataOutput)
    {
        fclose(dumpWakeupDataOutput);
        dumpWakeupDataOutput = NULL;
    }
#endif
}

void IWakeupService::addWakeupListenner(WakeupListenner *listenner)
{
    this->wakeupListenners.push_back(listenner);
}
void IWakeupService::removeWakeupListenner(WakeupListenner *listenner)
{
    this->wakeupListenners.remove(listenner);
}
bool IWakeupService::getIsRun()
{
    return this->isRun;
}
void IWakeupService::stop()
{
    this->isRun = false;
}

void IWakeupService::run()
{
    if (!this->isRun)
    {
        pthread_create(&this->th_wakeup, NULL, m_wakeupProcess, (void *)this);
    }
}
short *IWakeupService::getAudioDataAddr()
{
    if (NULL == audioData)
    {
        audioData = (short *)malloc(MicDataSource::FRAMELEN * sizeof(short));
    }
    return audioData;
}
float IWakeupService::calcWakeupAngle()
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
        if (i >= size)
        {
            break;
        }
        // cout << *it << " ";
    }
    // cout << endl <<"------------------------------" << endl;
    if (size == 0)
    {
        return 0;
    }

    return sum / angles.size();
}

#ifdef DUMP_WAKEUP_DATA
void IWakeupService::pushBackWakeupData(short *data, const int count)
{
    for (int i = 0; i < count; i++)
    {
        wakeupData.push_back(data[i]);
    }
}
void IWakeupService::popFrontWakeupData(const int count)
{
    if (wakeupData.size() >= count)
    {
        wakeupData.erase(wakeupData.begin(), wakeupData.begin() + count);
    }
}
#endif

void IWakeupService::onDataArrival(short *inputAudioData, float angle)
{
    memcpy(getAudioDataAddr(), inputAudioData, MicDataSource::FRAMELEN * sizeof(short));
    if (angles.size() >= ANGLES_MIX_SIZE)
    {
        angles.pop_front();
    }
    angles.push_back(angle);
#ifdef DUMP_WAKEUP_DATA
    // if (wakeupData.size() >= MicDataSource::FRAMELEN * ANGLES_MIX_SIZE)
    // {
    //     popFrontWakeupData(MicDataSource::FRAMELEN);
    // }
    // pushBackWakeupData(inputAudioData, MicDataSource::FRAMELEN);

    // vector<short>::iterator iter = wakeupData.begin();
    // for (; iter != wakeupData.end(); iter++)
    // {
    //     fwrite(&(*iter), sizeof(short), 1, dumpWakeupDataOutput);
    // }
    // wakeupData.clear();
    fwrite(inputAudioData, sizeof(short), MicDataSource::FRAMELEN, dumpWakeupDataOutput);
    macroFuncVargs("IWakeupService::onDataArrival: fwrite data to file.");
    fflush(dumpWakeupDataOutput);
#endif
    pthread_cond_signal(&dataArrivalCond);
}

void *IWakeupService::m_wakeupProcess(void *p)
{
    pthread_detach(pthread_self());
    IWakeupService *wakeupService = (IWakeupService *)p;
    wakeupService->isRun = true;

    int isWakeup = 0;
    WakeupEvent *wakeupEvent = new WakeupEvent();
    int stride_len = MicDataSource::FRAMELEN;
    float *audio_buf_f = new float[stride_len];

    while (wakeupService->isRun)
    {
        pthread_mutex_lock(&wakeupService->dataArrivalMutex);
        pthread_cond_wait(&wakeupService->dataArrivalCond, &wakeupService->dataArrivalMutex);
        pthread_mutex_unlock(&wakeupService->dataArrivalMutex);
        /** wakeup process */

        static const float scale = 1.f / (float)(MAXABS16S);
        short *inputData16s = wakeupService->getAudioDataAddr();
        /** Convert input data from 16s to 32f. */
        for (int i = 0; i < stride_len; i++)
        {
            audio_buf_f[i] = (float)inputData16s[i] * scale;
        }

        // printf("Wakeup processing!\n");

        // wakeup work
        int wakeupStatus = wakeupProcess(wakeupService->m_wakeup_obj, audio_buf_f);
        // int wakeupStatus = 0;
        if (wakeupStatus == -1)
        {
            macroFunc("Wakeup processing failed!\n");
        }
        else if (wakeupStatus == 1)
        {
            macroFunc("\n=============Wakeup!   Change AudioPreprocessDispatcher::status to 2 (waiting status.).\n");

            wakeupService->g_dispatcher->status = -1; //do nothing
            wakeupEvent->setAngle(wakeupService->calcWakeupAngle());
            wakeupEvent->setWord("");
            wakeupService->ttsService->onWakeup(wakeupEvent);
            sleep(2);
            wakeupService->g_dispatcher->status = 1; //to asr
        }
    }

    if (NULL != wakeupService->audioData)
    {
        free(wakeupService->audioData);
        wakeupService->audioData = NULL;
    }

    if (NULL != wakeupService->m_wakeup_obj)
    {
        wakeupRelease(wakeupService->m_wakeup_obj);
        wakeupService->m_wakeup_obj = NULL;
    }
    delete wakeupEvent;
    delete audio_buf_f;
    return NULL;
}
