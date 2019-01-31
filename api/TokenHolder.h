#ifndef __TOKEN_HOLDER_H__
#define __TOKEN_HOLDER_H__
#include <string>
#include <iostream>
#include <stdio.h>
#include "common.h"
using namespace std;
/**
 * token的获取类
 * 将apiKey和secretKey换取token，注意有效期保存在expiresAt
 */
class TokenHolder
{

  public:
    static const string ASR_SCOPE;
    static const string TTS_SCOPE;
    
    //  填写网页上申请的appkey 如 $apiKey="g8eBUMSokVB1BHGmgxxxxxx"
    static const string TWIRLING_APIKEY;

    // 填写网页上申请的APP SECRET 如 $secretKey="94dc99566550d87f8fa8ece112xxxxx"
    static const string TWIRLING_SECRETKEY;

    /**
     * @param apiKey    网页上申请语音识别应用获取的apiKey
     * @param secretKey 网页上申请语音识别应用获取的secretKey
     */
    TokenHolder(string apiKey, string secretKey, string scope);

    /**
     * 获取token，refresh 方法后调用有效
     *
     * @return
     */
    string getToken();

    /**
     * 获取过期时间，refresh 方法后调用有效
     *
     * @return
     */
    long getExpiresAt();

    void resfresh();

  private:
    /**
     * url , Token的url，http可以改为https
     */
    static const string url;

    /**
     * asr的权限 scope 是  "audio_voice_assistant_get"
     * tts 的权限 scope 是 "audio_tts_post"
     */
    string scope;

    /**
     * 网页上申请语音识别应用获取的apiKey
     */
    string apiKey;

    /**
     * 网页上申请语音识别应用获取的secretKey
     */
    string secretKey;

    /**
     * 保存访问接口获取的token
     */
    string token;

    /**
     * 当前的时间戳，毫秒
     */
    long expiresAt;

    // libcurl 返回回调
    static size_t writefunc(void *ptr, size_t size, size_t nmemb, char **result);

    RETURN_CODE parseToken(const char *response);
};
#endif //__TOKEN_HOLDER_H__