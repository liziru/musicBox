#include <iostream>
#include "DeviceInfo.h"

string DeviceInfo::toString()
{
    return this->getDeviceId() + ", " + this->getDeviceDesc() \
         + ", " + this->getVersion() ;
}

void DeviceInfo::setDeviceId(string deviceId)
{
    this->deviceId = deviceId;
}

void DeviceInfo::setDeviceDesc(string deviceDesc)
{
    this->deviceDesc = deviceDesc;
}

void DeviceInfo::setVersion(string version)
{
    this->version = version;
}

string DeviceInfo::getDeviceId()
{
    return this->deviceId;
}

string DeviceInfo::getDeviceDesc()
{
    return this->deviceDesc;
}

string DeviceInfo::getVersion()
{
    return this->version;
}