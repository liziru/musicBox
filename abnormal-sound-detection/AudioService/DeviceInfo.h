#ifndef __DATABASE_DEVICE_INFO_H__
#define __DATABASE_DEVICE_INFO_H__
#include <string>

using namespace std;

class DeviceInfo //类声明
{
public:
  string toString();
  void setDeviceId(string deviceId);
  void setDeviceDesc(string deviceDesc);
  void setVersion(string version);

  string getDeviceId();
  string getDeviceDesc();
  string getVersion();

private:
  // 设备编号
  string deviceId; //MAC地址
  string deviceDesc;
  string version;
};
#endif