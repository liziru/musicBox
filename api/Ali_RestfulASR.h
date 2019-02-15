#if !defined(__ALI_RESTFUL_ASR)
#define __ALI_RESTFUL_ASR
#include <string>

using namespace std;

class Ali_RestfulASR
{
private:
  // long expireTime;
  // string appkey;
  string tokenkey;
  string fileName;
  string request;
  string response;
  string finalRes;
  static const string URL;
  static const string APPKEY; //<Appkey Id>

public:
  Ali_RestfulASR(string tokenId);
  ~Ali_RestfulASR();

  int sendAsrRequest(const char *request, const char *token, const char *fileName, string *srResult);

  int process(const char *fileName);

  /**
 * 一句话识别RESTful API HTTP请求的响应回调函数
 * 识别结果为JSON格式的字符串
 */
  static size_t responseCallback(void *ptr, size_t size, size_t nmemb, void *userData);

#ifdef _WIN32
  string UTF8ToGBK(const string &strUTF8);
#endif
};
#endif // __ALI_RESTFUL_ASR
