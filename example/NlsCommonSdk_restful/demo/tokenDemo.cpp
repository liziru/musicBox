#include <iostream>
#include "Token.h"

using std::cout;
using std::endl;
using namespace AlibabaNlsCommon;

//./tokenDemo tbRlbTYXpTqqLMkKGAuSeieYTBIAFv LTAIGEBTcWrw9aKm
//export LD_LIBRARY_PATH=/home/lee/Desktop/musicBox/example/NlsCommonSdk_restful/lib/linux

//获取访问令牌TokenId
int getTokenId(const char *keySecret, const char *keyId)
{
    NlsToken nlsTokenRequest;

    /*设置阿里云账号KeySecret*/
    nlsTokenRequest.setKeySecret(keySecret);
    /*设置阿里云账号KeyId*/
    nlsTokenRequest.setAccessKeyId(keyId);

    /*获取token. 成功返回0, 失败返回-1*/
    if (-1 == nlsTokenRequest.applyNlsToken())
    {
        cout << "Failed: " << nlsTokenRequest.getErrorMsg() << endl; /*获取失败原因*/

        return -1;
    }
    else
    {
        cout << "TokenId: " << nlsTokenRequest.getToken() << endl;                 /*获取TokenId*/
        cout << "TokenId expireTime: " << nlsTokenRequest.getExpireTime() << endl; /*获取Token有效期时间戳(秒)*/

        return 0;
    }
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cout << "FileTransDemo need params : <AccessKey Secret> <AccessKey Id>" << endl;
        return -1;
    }

    getTokenId(argv[1], argv[2]);

    return 0;
}
