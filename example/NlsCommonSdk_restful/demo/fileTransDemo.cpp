#include <iostream>
#include <string>
#include "FileTrans.h"

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

using std::string;
using std::cout;
using std::endl;
using namespace AlibabaNlsCommon;

#ifdef _WIN32
string UTF8ToGBK(const string &strUTF8) {

	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	unsigned short * wszGBK = new unsigned short[len + 1];
	memset(wszGBK, 0, len * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, (char*)strUTF8.c_str(), -1, (wchar_t*)wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, (wchar_t*)wszGBK, -1, NULL, 0, NULL, NULL);

	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, (wchar_t*)wszGBK, -1, szGBK, len, NULL, NULL);

	string strTemp(szGBK);
	delete[] szGBK;
	delete[] wszGBK;

	return strTemp;
}
#endif

//文件转写
int fileTrans(const char* accessKeyId, const char* accessKeySecret, const char* appKey, const char* fileLink) {
    FileTrans request;

    /*设置阿里云账号AccessKey Id*/
    request.setAccessKeyId(accessKeyId);
    /*设置阿里云账号AccessKey Secret*/
    request.setKeySecret(accessKeySecret);
    /*设置阿里云AppKey*/
    request.setAppKey(appKey);
    /*设置音频文件url地址*/
    request.setFileLinkUrl(fileLink);

    /*开始文件转写, 成功返回0, 失败返回-1*/
    int ret = request.applyFileTrans();
    if (-1 == ret) {
        cout << "FileTrans failed: " << request.getErrorMsg() << endl; /*获取失败原因*/
        return -1;
    } else {
        string result = request.getResult();

#ifdef _WIN32
        result = UTF8ToGBK(result);
#endif

		cout << "FileTrans successed: " << result << endl;

		return 0;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "FileTransDemo need params : <AccessKey Id> <AccessKey Secret> <app - key>" << endl;
        return -1;
    }

    const char* accessKeyId = argv[1];
    const char* accessKeySecret = argv[2];
    const char* appKey = argv[3];
    const char* fileLink = "http://aliyun-nls.oss.aliyuncs.com/asr/fileASR/examples/nls-sample.wav";

    fileTrans(accessKeyId, accessKeySecret, appKey, fileLink);

    return 0;
}

