#ifndef __TTS_RESULT_H__
#define __TTS_RESULT_H__
#include <stdio.h>
#include <string>
#include "common.h"
using namespace std;
class TtsResult
{
  private:
    bool isError;
    string errorMsg;
    RETURN_CODE errorCode;
    string ttsVoiceFilename;
    FILE *ttsVoiceFile; // 保存结果的文件
  public:
    TtsResult();
    ~TtsResult();
    void setIsError(bool isError);
    void setErrorMsg(string errorMsg);
    void setErrorCode(RETURN_CODE errorCode);
    void setTtsVoiceFilename(string ttsVoiceFilename);
    void setTtsVoiceFile(FILE *file);

    bool getIsError();
    string getErrorMsg();
    RETURN_CODE getErrorCode();
    string getTtsVoiceFilename();
    FILE *getTtsVoiceFile();
};
#endif //__TTS_RESULT_H__  
