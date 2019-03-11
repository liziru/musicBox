#include "NeteaseMusicLanucher.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

NeteaseMusicLanucher::NeteaseMusicLanucher(PlayBackAudio *pb, DownloadService *download, TtsService *tts) : playbackAudio(pb), downloadService(download)
{
    wakeupEvent = new WakeupEvent();
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    isRun = false;
    workType = 0;
    playbackList.clear();
    ttsService = tts;
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
    playbackList.clear();
}

bool NeteaseMusicLanucher::parseJson(string msg)
{
#ifdef NETEASE_LANUCHER_DEBUG
    printf("\n\ntype:%d, Json :%s\n\n", workType, msg.c_str());
#endif

    if (workType == 1)
    {
        SizeType i = 0;
        Document dm;
        dm.Parse(msg.c_str());
        if (dm.HasParseError())
        {
            printf("FATAL ERROR: GetParseError( %d ), msg-json:%s\n", dm.GetParseError(), msg.c_str());
            return false;
        }
        // Document::AllocatorType &allocator = dm.GetAllocator();

        if (dm.HasMember("recommend"))
        {
            Value &songs = dm["recommend"];
            // Value key(kArrayType);
            // key = bytes.GetArray();
            // Value tmp;
            if (songs.IsArray())
            {
                playbackList.clear(); //清空播放列表

                for (i = 0; i < songs.Size(); i++)
                {
                    StringBuffer buff;
                    Writer<StringBuffer> writer(buff);
                    Value &song = songs[i];
                    // playbackList.push_back(song.GetString());
                    // printf("\n\n\nSONGS :%s\n\n", song.GetString());
                    if (song.IsObject())
                    {
                        song.Accept(writer);
#ifdef NETEASE_LANUCHER_DEBUG
                        printf("\n\nit is object,song:%s\n\n\n", buff.GetString());
#endif
                        playbackList.push_back(buff.GetString());
                    }
                }

                macroFuncVargs("playlist.size:%d,i=%d", playbackList.size(), i);
                if (i == playbackList.size())
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
    }
    return false;
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

// song:{"name":"孤独的人是可耻的","id":186105,"position":6,
//"alias":[],"status":0,"fee":8,"copyrightId":684010,"disc":"1","no":6,
// "artists":[{"name":"张楚","id":6455,"picId":0,"img1v1Id":0,"briefDesc":"","picUrl":"http://p2.music.126.net/6y-UleORITEDbvrOLV0Q8A==/5639395138885805.jpg","img1v1Url":"http://p2.music.126.net/6y-UleORITEDbvrOLV0Q8A==/5639395138885805.jpg","albumSize":0,"alias":[],"trans":"","musicSize":0}],"album":{"name":"孤独的人是可耻的","id":18913,"type":"专辑","size":10,"picId":722379139460902,"blurPicUrl":"http://p2.music.126.net/korTTSnbk-PQzFZml9S07Q==/722379139460902.jpg","companyId":0,"pic":722379139460902,"picUrl":"http://p2.music.126.net/korTTSnbk-PQzFZml9S07Q==/722379139460902.jpg","publishTime":775670400000,"description":"","tags":"","company":"魔岩唱片","briefDesc":"","artist":{"name":"","id":0,"picId":0,"img1v1Id":0,"briefDesc":"","picUrl":"http://p2.music.126.net/6y-UleORITEDbvrOLV0Q8A==/5639395138885805.jpg","img1v1Url":"http://p2.music.126.net/6y-UleORITEDbvrOLV0Q8A==/5639395138885805.jpg","albumSize":0,"alias":[],"trans":"","musicSize":0},"songs":[],"alias":[],"status":1,"copyrightId":684010,"commentThreadId":"R_AL_3_18913","artists":[{"name":"张楚","id":6455,"picId":0,"img1v1Id":0,"briefDesc":"","picUrl":"http://p2.music.126.net/6y-UleORITEDbvrOLV0Q8A==/5639395138885805.jpg","img1v1Url":"http://p2.music.126.net/6y-UleORITEDbvrOLV0Q8A==/5639395138885805.jpg","albumSize":0,"alias":[],"trans":"","musicSize":0}],"subType":"录音室版","transName":null},
// "starred":false,"popularity":95,"score":95,"starredNum":0,"duration":268042,"playedNum":0,"dayPlays":0,"hearTime":0,"ringtone":"600907000001196435","crbt":"51fb89da0a3b3e1ae74aca9c5e41daba","audition":null,"copyFrom":"","commentThreadId":"R_SO_4_186105","rtUrl":null,"ftype":0,"rtUrls":[],"copyright":0,"transName":null,"sign":null,"mvid":0,"rtype":0,"rurl":null,
// "bMusic":{"name":"孤独的人是可耻的","id":10479057,"size":3233769,"extension":"mp3","sr":44100,"dfsId":0,"bitrate":96000,"playTime":268042,"volumeDelta":-0.000265076},"mp3Url":null,
// "hMusic":{"name":"孤独的人是可耻的","id":10479055,"size":10732791,"extension":"mp3","sr":44100,"dfsId":0,"bitrate":320000,"playTime":268042,"volumeDelta":-0.000265076},
// "mMusic":{"name":"孤独的人是可耻的","id":10479056,"size":5376645,"extension":"mp3","sr":44100,"dfsId":0,"bitrate":160000,"playTime":268042,"volumeDelta":-0.000265076},
// "lMusic":{"name":"孤独的人是可耻的","id":10479057,"size":3233769,"extension":"mp3","sr":44100,"dfsId":0,"bitrate":96000,"playTime":268042,"volumeDelta":-0.000265076},
// "reason":"根据你可能喜欢的单曲 赤裸裸","privilege":{"id":186105,"fee":8,"payed":0,"st":0,"pl":128000,"dl":0,"sp":7,"cp":1,"subp":1,"cs":false,"maxbr":320000,"fl":128000,"toast":false,"flag":128,"preSell":false},"alg":"itembased",
// "musicUrl":"{\"data\":[{\"id\":186105,\"url\":\"http://m10.music.126.net/20190311182035/f71810af10084a8924e97cbf0dec6ece/ymusic/cf31/9b0d/89ef/37fb2d6d59c0938391c3c9c94cab0187.mp3\",\"br\":128000,\"size\":4285858,\"md5\":\"37fb2d6d59c0938391c3c9c94cab0187\",\"code\":200,\"expi\":1200,\"type\":\"mp3\",\"gain\":-0.0002,\"fee\":8,\"uf\":null,\"payed\":0,\"flag\":128,\"canExtend\":false,\"freeTrialInfo\":null,\"level\":\"standard\",\"encodeType\":\"mp3\"}],\"code\":200}"}
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
            // exit(1);
            if (lancher->getWorkType() == 1) //play recommended-list.
            {
                WakeupEvent *wakeupEvent = new WakeupEvent();
                list<string> recommendList = lancher->getPlaybackList();
                list<string>::iterator it = recommendList.begin();
                for (; it != recommendList.end(); it++)
                {
                    // string song = it->c_str();
                    Document dm;
                    Value songName;
                    string dlUrl = "";

                    dm.Parse(it->c_str());
                    if (dm.HasMember("name"))
                    {
                        songName = dm["name"];
                        macroFuncVargs("dlurl:%s\n\n", songName.GetString());
                    }
                    if (dm.HasMember("musicUrl"))
                    {
                        Value &musicUrl = dm["musicUrl"];
                        if (musicUrl.IsObject())
                        {
                            Value &data = musicUrl["data"];
                            if (data.IsObject())
                            {
                                Value &url = data["url"];
                                dlUrl = url.GetString();
                                macroFuncVargs("dlUrl:%s\n\n", dlUrl.c_str());
                            }
                        }
                    }

                    //得到url，进行下载
                    string word = string("下面为您播放") + songName.GetString();
                    wakeupEvent->setAngle(0);
                    wakeupEvent->setWord(word);
                    lancher->ttsService->onWakeup(wakeupEvent);
                    // lancher->downloadService->onWakeup();
                    //下载好，进行播放
                }

                delete wakeupEvent;
                wakeupEvent = NULL;
            }
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

int NeteaseMusicLanucher::getWorkType()
{
    return this->workType;
}
void NeteaseMusicLanucher::setWorkType(int type)
{
    this->workType = type;
}
list<string> NeteaseMusicLanucher::getPlaybackList()
{
    return this->playbackList;
}
void NeteaseMusicLanucher::setPlaybackList(list<string> list)
{
    this->playbackList = list;
}