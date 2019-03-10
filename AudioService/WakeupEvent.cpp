#include "WakeupEvent.h"

WakeupEvent::WakeupEvent()
{
    this->angle = 0;
}
float WakeupEvent::getAngle()
{
    return this->angle;
}

void WakeupEvent::setAngle(float angle)
{
    this->angle = angle;
}

void WakeupEvent::setWord(string word)
{
    this->word = word;
}
string WakeupEvent::getWord()
{
    return this->word;
}

void WakeupEvent::setAsrFileName(string fileName)
{
    this->asrFileName = fileName;
}
string WakeupEvent::getAsrFileName()
{
    return this->asrFileName;
}
string WakeupEvent::getAsrRes()
{
    return asrRes;
}
void WakeupEvent::setAsrRes(string res)
{
    this->asrRes = res;
}

string WakeupEvent::getPbFile()
{
    return this->pbFileName;
}

void WakeupEvent::setPbFile(string pbFileName)
{
    this->pbFileName = pbFileName;
}

void WakeupEvent::setLanType(int type)
{
    lanType = type;
}
int WakeupEvent::getLanType()
{
    return lanType;
}

void WakeupEvent::setLanJsonTask(string json)
{
    lanJsonTask = json;
}
string WakeupEvent::getlanJsonTask()
{
    return lanJsonTask;
}

string WakeupEvent::getDlUrl()
{
    return this->dlUrl;
}
void WakeupEvent::setDlUrl(string url)
{
    this->dlUrl = url;
}

string WakeupEvent::getDlMusicFile()
{
    return this->dlMusicFile;
}
void WakeupEvent::setDlMusicFile(string musicFile)
{
    this->dlMusicFile = musicFile;
}