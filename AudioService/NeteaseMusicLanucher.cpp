#include "NeteaseMusicLanucher.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

NeteaseMusicLanucher::NeteaseMusicLanucher(PlayBackAudio *pb, DownloadService *download) : playbackAudio(pb), downloadService(download)
{
    wakeupEvent = new WakeupEvent();
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    isRun = false;
    workType = 0;
}

NeteaseMusicLanucher::~NeteaseMusicLanucher()
{
    if (wakeupEvent)
    {
        delete wakeupEvent;
        wakeupEvent = NULL;
    }
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}

bool NeteaseMusicLanucher::parseJson(string msg)
{
    // printf("\n\ntype:%d, Json :%s\n\n", workType, msg.c_str());

    if (workType == 1)
    {
        Document dm;
        dm.Parse(msg.c_str());
        if (dm.HasParseError())
        {
            printf("FATAL ERROR: GetParseError( %d ), msg-json:%s\n", dm.GetParseError(), msg.c_str());
            return "";
        }
        // Document::AllocatorType &allocator = dm.GetAllocator();
        StringBuffer buff;
        Writer<StringBuffer> writer(buff);
        if (dm.HasMember("recommend"))
        {
            Value &songs = dm["recommend"];
            // Value key(kArrayType);
            // key = bytes.GetArray();
            // Value tmp;
            if (songs.IsArray())
            {
                for (SizeType i = 0; i < songs.Size(); i++)
                {
                    Value &song = songs[i];
                    // playbackList.push_back(song.GetString());
                    // printf("\n\n\nSONGS :%s\n\n", song.GetString());
                    if (song.IsObject())
                    {
                        song.Accept(writer);
                        printf("\n\nit is object,song:%s\n\n\n",buff.GetString());
                    }
                }
                exit(1);
            }
        }
    }
}

void NeteaseMusicLanucher::onWakeup(int type, string msg)
{
    // wakeupEvent
    // wakeupEvent->setAsrRes(wuEvent->getAsrRes());
    // string jsonMsg = msg;
    workType = type;

    if (parseJson(msg) && !playbackList.empty())
    {
        pthread_cond_signal(&cond);
    }
}

void NeteaseMusicLanucher::onWakeup(WakeupEvent *wuEvent)
{
    // printf("\n\n\n\nJson to parse:%s\n\n", wakeupEvent->getlanJsonTask().c_str());
    taskJson = (wuEvent->getlanJsonTask());
    workType = wuEvent->getLanType();

    if (parseJson(taskJson) && !playbackList.empty())
    {
        pthread_cond_signal(&cond);
    }
}

void *NeteaseMusicLanucher::neteaseLanucherProcess(void *p)
{
    NeteaseMusicLanucher *lancher = (NeteaseMusicLanucher *)p;
    lancher->isRun = true;

    while (lancher->isRun)
    {
        pthread_mutex_lock(&lancher->mutex);
        pthread_cond_wait(&lancher->cond, &lancher->mutex);
        pthread_mutex_unlock(&lancher->mutex);
        while (1)
        {
            //循环播放列表
            macroFuncVargs("Prepare to download and playback music.");
            exit(1);
        }
    }
}

void NeteaseMusicLanucher::run()
{
    if (!this->isRun)
    {
        pthread_create(&th_netease, NULL, neteaseLanucherProcess, (void *)this);
    }
}
