#ifndef __BAIDU_VOICE_H__
#define __BAIDU_VOICE_H__
#include <string>
#include "common.h"
#include "TtsResult.h"
using namespace std;

class BaiduVoice
{
public:
  static const string API_TTS_URL;
  BaiduVoice();
  ~BaiduVoice();
  TtsResult *voiceTts(string tts);
  string voiceTtsUrl(string tts);

private:
  static const int spd;
  static const int pit;
  static const int vol;
  static const int per;
  static const string cuid;
  static const int MAX_HEADER_VALUE_LEN;
  static size_t headerCallback(char *buffer, size_t size, size_t nitems, TtsResult *result);
  static size_t writefuncData(void *ptr, size_t size, size_t nmemb, TtsResult *result);
  static RETURN_CODE search_header(const char *buffer, size_t len, const char *key, char *value);
};
#endif //__BAIDU_VOICE_H__
