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