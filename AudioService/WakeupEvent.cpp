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