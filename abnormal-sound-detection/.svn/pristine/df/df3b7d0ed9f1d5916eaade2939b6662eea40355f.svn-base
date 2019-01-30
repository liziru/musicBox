#include "TtsResult.h"

TtsResult::TtsResult()
    : isError(false), errorMsg(string()), errorCode(RETURN_ERROR), ttsVoiceFilename(string()), ttsVoiceFile(NULL)
{
    // random filename
    ttsVoiceFilename = string("/tmp/ttsVoiceFile.mp3");
}

TtsResult::~TtsResult()
{
    if(this->ttsVoiceFile != NULL)
    {
        fclose(this->ttsVoiceFile);
        this->ttsVoiceFile = NULL;
    }
    // delete file
}

void TtsResult::setIsError(bool isError)
{
    this->isError = isError;
}
void TtsResult::setErrorMsg(string errorMsg)
{
    this->errorMsg = errorMsg;
}
void TtsResult::setErrorCode(RETURN_CODE errorCode)
{
    this->errorCode = errorCode;
}
void TtsResult::setTtsVoiceFilename(string ttsVoiceFilename)
{
    this->ttsVoiceFilename = ttsVoiceFilename;
}
void TtsResult::setTtsVoiceFile(FILE *file)
{
    this->ttsVoiceFile = file;
}

bool TtsResult::getIsError()
{
    return isError;
}
string TtsResult::getErrorMsg()
{
    return errorMsg;
}
RETURN_CODE TtsResult::getErrorCode()
{
    return errorCode;
}
string TtsResult::getTtsVoiceFilename()
{
    return ttsVoiceFilename;
}
FILE *TtsResult::getTtsVoiceFile()
{
    return ttsVoiceFile;
}