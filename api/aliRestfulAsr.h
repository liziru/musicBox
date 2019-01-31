#if !defined(__ALI_RESTFUL_ASR)
#define __ALI_RESTFUL_ASR

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
    static string URL = "http://nls-gateway.cn-shanghai.aliyuncs.com/stream/v1/asr";
    static const string APPKEY = "mrGmd1DlpfGQ9aG3"; //<Appkey Id>

  public:
    Ali_RestfulASR(/* args */);
    ~Ali_RestfulASR();

    int process(const char *fileName);
    int sendAsrRequest(const char *request, const char *token, const char *fileName, string *srResult);

    /**
 * 一句话识别RESTful API HTTP请求的响应回调函数
 * 识别结果为JSON格式的字符串
 */
    size_t responseCallback(void *ptr, size_t size, size_t nmemb, void *userData);

#ifdef _WIN32
    string UTF8ToGBK(const string &strUTF8);
#endif
}
#endif // __ALI_RESTFUL_ASR
