#include "threadFunc.h"
#include <string>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include "fifo.h"
#include "log.h"
#include "TwirlingCapture.h"
#include "TwirlingVad.h"
#include "MicDataSource.h"

void *audioPreprocessDispatcher_thread(void *p)
{
    MicDataSource *micDataSource = (MicDataSource *)p;
    dispatcher->isRun = true;
    dispatcher->micDataSource->run();

    float angle = 0;
    float doaParams[4] = {0};
    float inputMicData32f[MicDataSource::FRAMELEN];
    float outputData32f[MicDataSource::FRAMELEN];
    short outData16s[MicDataSource::FRAMELEN];
    short tmpData16s[MicDataSource::FRAMELEN];
    short *inputMicData16s = (short *)malloc(sizeof(short) * MicDataSource::FRAMELEN * micDataSource->getChannel());
    float *rawMicData32f = (float *)malloc(sizeof(float) * MicDataSource::FRAMELEN * micDataSource->getChannel());
    // dispatcher->isRun = true;

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
    TwirlingVad *m_vad = new TwirlingVad(MicDataSource::FRAMELEN, micDataSource->getRate());

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

                WakeupEvent *wakeupEvent = new WakeupEvent();
                wakeupEvent->setAsrFileName(asrFileName);
                list<AudioPreprocessAsrListenner *>::iterator it = dispatcher->audioPreprocessAsrListenners.begin();
                for (; it != dispatcher->audioPreprocessAsrListenners.end(); ++it)
                {
                    (*it)->onDataArrival(wakeupEvent);
                }
                macroFunc("AudioPreprocessDispatcher::dispatcherThread: display audio data to asr_process.");
                delete wakeupEvent;
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