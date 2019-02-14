#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "AudioPreprocessDispatcher.h"
#include "TwirlingCapture.h"
#include "log.h"
#include "TwirlingVad.h"

const float AudioPreprocessDispatcher::SCALE = 1.f / (float)(MAXABS16S);
string AudioPreprocessDispatcher::asrFileName = "";
int AudioPreprocessDispatcher::APDLEVEL = 0;

AudioPreprocessDispatcher::AudioPreprocessDispatcher(bool ansIsOn, bool drcIsOn, float ansDB, float digitalGain, MicDataSource *micDataSource)
    : ansIsOn(ansIsOn), drcIsOn(drcIsOn), ansDB(ansDB), digitalGain(digitalGain), micDataSource(micDataSource),
      isRun(isRun), m_ans_obj(NULL), m_drc_obj(NULL), m_doa_obj(NULL), th_dispatcher(0)

{
    this->sArray = (short *)malloc(sizeof(short) * MicDataSource::FRAMELEN * this->micDataSource->getChannel() * 500); //8s
    this->pfifo = &this->fifo;
    FIFO_Init(this->pfifo, sArray, sizeof(short) * MicDataSource::FRAMELEN * this->micDataSource->getChannel(), 500); //FIFO_BUFF_COUNT 750
}

AudioPreprocessDispatcher::~AudioPreprocessDispatcher()
{
    if (NULL != m_ans_obj)
    {
        ansRelease(m_ans_obj);
        m_ans_obj = NULL;
    }
    if (NULL != m_drc_obj)
    {
        drcRelease(m_drc_obj);
        m_drc_obj = NULL;
    }
    if (NULL != m_doa_obj)
    {
        doaRelease(m_doa_obj);
        m_doa_obj = NULL;
    }
}

void AudioPreprocessDispatcher::setMicDataSource(MicDataSource *micDataSource)
{
    this->micDataSource = micDataSource;
}

void AudioPreprocessDispatcher::dispatcherAudioData()
{
    if (!this->isRun)
    {
        pthread_create(&this->th_dispatcher, NULL, dispatcherThread, (void *)this);
    }
}

string AudioPreprocessDispatcher::getTimeStr()
{
    struct tm *t;
    time_t tt;
    time(&tt);
    t = localtime(&tt);
    char tmp[50] = {0};
    sprintf(tmp, "%4d_%02d_%02d-%02d_%02d_%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    return string(tmp);
}

int AudioPreprocessDispatcher::writeBuffToFile(string &pcmFileName, void *p)
{
    short inputData16s[MicDataSource::FRAMELEN];
    FILE *pcmOutput = NULL;
    int i;
    int rd;
    AudioPreprocessDispatcher *dispatcher = (AudioPreprocessDispatcher *)p;

    // string tmp = getTimeStr();
    // sprintf(pcmFileName, "./data/pcm/voice_%s.raw", tmp.c_str());
    pcmFileName = "/home/pi/workspace/data/asrAudio/voice_" + getTimeStr() + ".raw";
#ifdef VERBOSE
    macroFuncVargs(" AudioPreprocessDispatcher::writeBuffToFile : pcmFileName (%s),fifo_counter (%d)", pcmFileName.c_str(), dispatcher->pfifo->Counter);
#endif //VERBOSE

    if ((pcmOutput = fopen(pcmFileName.c_str(), "wb")) == NULL)
    {
        macroFunc("cannot open voice file for recording");
        return -1;
    }

    for (; dispatcher->pfifo->Counter > 0;)
    {
        if (0 == FIFO_GetOne(dispatcher->pfifo, inputData16s))
        {
            macroFunc("FIFO_GetOne is error, buffer is empty.");
            return -2;
        }
        fwrite(inputData16s, sizeof(short), MicDataSource::FRAMELEN, pcmOutput);
    }
    fflush(pcmOutput);
    fsync(fileno(pcmOutput));
    fclose(pcmOutput);

    return 0;
}

void *AudioPreprocessDispatcher::dispatcherThread(void *p)
{
    AudioPreprocessDispatcher *dispatcher = (AudioPreprocessDispatcher *)p;
    dispatcher->isRun = true;
    dispatcher->micDataSource->run();

    float angle = 0;
    float doaParams[4] = {0};
    float inputMicData32f[MicDataSource::FRAMELEN];
    float outputData32f[MicDataSource::FRAMELEN];
    short outData16s[MicDataSource::FRAMELEN];
    short tmpData16s[MicDataSource::FRAMELEN];
    short *inputMicData16s = (short *)malloc(sizeof(short) * MicDataSource::FRAMELEN * dispatcher->micDataSource->getChannel());
    float *rawMicData32f = (float *)malloc(sizeof(float) * MicDataSource::FRAMELEN * dispatcher->micDataSource->getChannel());
    dispatcher->isRun = true;

    void *m_ans_obj = NULL;
    void *m_eq_obj = NULL;
    void *m_drc_obj = NULL;
    void *m_vad_obj = NULL;

    //vad init
    int currVadRs = 1;
    int prevVadRs = 1;
    // int beginCounter = 0;
    // int endCounter = 0;
    bool isStartSpeak = false;
    bool isInSpeak = false;
    bool isEndSpeak = false;
    bool resetFlag = false;
    TwirlingVad *m_vad = new TwirlingVad(MicDataSource::FRAMELEN, dispatcher->micDataSource->getRate());

#ifdef PRINT_TIME
    struct timeval us1;
    struct timeval us2;
    int total = 0;
    int count = 0;
#endif
#ifdef DUMP_PROCESS_PCM_DATA
    FILE *dumpProcessOutput = NULL;
    if ((dumpProcessOutput = fopen("./dump_process.pcm", "wb")) == NULL)
    {
        macroFunc("wakeup: cannot open dump_process file for wakeup");
        exit(1);
    }
#endif
    while (dispatcher->isRun)
    {
#ifdef PRINT_TIME
        gettimeofday(&us1, NULL);
#endif
        if (!dispatcher->micDataSource->getDataPackage(inputMicData16s))
        {
            // macroFunc("wakeup: buf_read fail, buffer is empty.");
            usleep(100);
            continue;
        }

        for (int i = 0; i < MicDataSource::FRAMELEN * dispatcher->micDataSource->getChannel(); i++)
        {
            rawMicData32f[i] = (float)inputMicData16s[i] * SCALE;
        }

        // DOA process
        /* if(NULL == dispatcher->m_doa_obj){
            dispatcher->m_doa_obj = doaInit(dispatcher->micDataSource->getMicProfileId(), 
                    MicDataSource::FRAMELEN, 
                    dispatcher->micDataSource->getChannel(), 
                    dispatcher->micDataSource->getRate(), 
                    dispatcher->micDataSource->getMicDist(), false);
            doaSet(dispatcher->m_doa_obj, dispatcher->micDataSource->getMicFormatId());
        }
        doaProcess(dispatcher->m_doa_obj, rawMicData32f, doaParams);
        */
        // angle = doaParams[0];
        // macroFuncVargs("estimated direction of arrival = %.1f ", angle);

        /** to one channel */
        for (int i = 0; i < MicDataSource::FRAMELEN; i++)
        {
            inputMicData32f[i] = (float)rawMicData32f[i * dispatcher->micDataSource->getChannel()] * dispatcher->digitalGain;
        }

        if (resetFlag)
        {
            currVadRs = 0;
            prevVadRs = 0;
            isInSpeak = false;
            isEndSpeak = false;
            isStartSpeak = false;
            resetFlag = false;
        }

        //my vad
        m_vad->process(outputData32f);
        currVadRs = m_vad->getState();
        // macroFuncVargs("dispatcherThread: vad getstatus, currVadRs=%d,prevVadRs=%d\n", currVadRs, prevVadRs);

        if (dispatcher->ansIsOn)
        {
            if (NULL == dispatcher->m_ans_obj)
            {
                dispatcher->m_ans_obj = ansInit(MicDataSource::FRAMELEN, 1, dispatcher->micDataSource->getRate(), dispatcher->ansDB, false);
            }
            ansProcess(dispatcher->m_ans_obj, outputData32f, outputData32f);
            macroFunc("dispatcherThread: ans is on.");
        }
        if (dispatcher->drcIsOn)
        {
            if (NULL == dispatcher->m_drc_obj)
            {
                dispatcher->m_drc_obj = drcInit(MicDataSource::FRAMELEN, 1, 1.0);
            }
            drcProcess(dispatcher->m_drc_obj, outputData32f, outputData32f);
            macroFunc("dispatcherThread: drc is on.");
        }

        if (!(dispatcher->drcIsOn || dispatcher->ansIsOn))
        {
            // macroFunc("dispatcherThread:  output the raw pcm.");
            memcpy(outputData32f, inputMicData32f, MicDataSource::FRAMELEN * sizeof(float));
        }

        /** Convert input data from 32f to 16s. */
        for (int i = 0; i < MicDataSource::FRAMELEN; i++)
        {
            int val = (int)(outputData32f[i] * (float)AudioPreprocessDispatcher::MAXABS16S);
            if (val > (int)AudioPreprocessDispatcher::MAX16S)
            {
                outData16s[i] = AudioPreprocessDispatcher::MAX16S;
            }
            else if (val < (int)AudioPreprocessDispatcher::MIN16S)
            {
                outData16s[i] = AudioPreprocessDispatcher::MIN16S;
            }
            else
            {
                outData16s[i] = (short)val;
            }
        }

        if (APDLEVEL == 0) //awakeup process
        {
            // dispatcher audio data
            list<AudioPreprocessListenner *>::iterator it = dispatcher->audioPreprocessListenners.begin();
            for (; it != dispatcher->audioPreprocessListenners.end(); ++it)
            {
                (*it)->onDataArrival(outData16s, angle);
            }
            macroFunc("AudioPreprocessDispatcher::dispatcherThread: display audio data to wakeup process.");
        }
        else if (AudioPreprocessDispatcher::APDLEVEL == 1) //vad to save file to asr.
        {
            //vad starts.
            if (currVadRs == 1 || currVadRs == 2)
            {
                currVadRs = 1;
            }
            else if (currVadRs == 3)
            {
                currVadRs = 0;
            }

            if (currVadRs == 0 && prevVadRs == 0)
            {
                if (dispatcher->pfifo->Counter < 15)
                {
                    FIFO_AddOne(dispatcher->pfifo, outData16s);
                }
                else
                {
                    FIFO_GetOne(dispatcher->pfifo, tmpData16s);
                    FIFO_AddOne(dispatcher->pfifo, outData16s);
                }

                isInSpeak = false;
                isEndSpeak = false;
                isStartSpeak = false;
            }
            else if (currVadRs == 1 && prevVadRs == 0) //begin
            {
                isInSpeak = true;
                isEndSpeak = false;
                isStartSpeak = true;
                // digitalPull(SPEAK_LED, HIGH);
            }
            else if (currVadRs == 1 && prevVadRs == 1) //in speak
            {
                isInSpeak = true;
                isEndSpeak = false;
                isStartSpeak = false;
            }
            else if (currVadRs == 0 && prevVadRs == 1) //end
            {
                isInSpeak = false;
                isEndSpeak = true;
                isStartSpeak = false;
                // digitalPull(SPEAK_LED, LOW);
            }

            if (isInSpeak && !isEndSpeak) //有人说话时
            {
                FIFO_AddOne(dispatcher->pfifo, outData16s);
            }

            prevVadRs = currVadRs;

            if (isEndSpeak)
            {
                resetFlag = true;
                writeBuffToFile(asrFileName, dispatcher);
                macroFuncVargs("AudioPreprocessDispatcher::dispatcherThread: ready to asr (%s).", asrFileName.c_str());
            }
        }

#ifdef DUMP_PROCESS_PCM_DATA
        fwrite(outData16s, sizeof(short), MicDataSource::FRAMELEN, dumpProcessOutput);
        fflush(dumpProcessOutput);
#endif
#ifdef PRINT_TIME
        gettimeofday(&us2, NULL);
        macroFuncVargs("TWIR_TIME=%u", ((us2.tv_sec * 1000000 + us2.tv_usec) - (us1.tv_sec * 1000000 + us1.tv_usec)));
#endif
    }

    if (NULL != inputMicData16s)
    {
        free(inputMicData16s);
        inputMicData16s = NULL;
    }
    if (NULL != dispatcher->m_ans_obj)
    {
        ansRelease(dispatcher->m_ans_obj);
        dispatcher->m_ans_obj = NULL;
    }
    if (NULL != dispatcher->m_drc_obj)
    {
        drcRelease(dispatcher->m_drc_obj);
        dispatcher->m_drc_obj = NULL;
    }
    return NULL;
}
void AudioPreprocessDispatcher::addAudioPreprocessListenner(AudioPreprocessListenner *listenner)
{
    this->audioPreprocessListenners.push_back(listenner);
}
void AudioPreprocessDispatcher::removeAudioPreprocessListenner(AudioPreprocessListenner *listenner)
{
    this->audioPreprocessListenners.remove(listenner);
}
bool AudioPreprocessDispatcher::getIsRun()
{
    return this->isRun;
}
void AudioPreprocessDispatcher::stop()
{
    this->isRun = false;
}
