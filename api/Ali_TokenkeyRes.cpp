#include <iostream>
#include "Token.h"
#include <sys/time.h>
#include "log.h"
#include "Ali_TokenkeyRes.h"

using namespace std;
using namespace AlibabaNlsCommon;

const string Ali_TokenkeyRes::keySecret = "tbRlbTYXpTqqLMkKGAuSeieYTBIAFv";
const string keyId = "LTAIGEBTcWrw9aKm"; //<Appkey Id>

bool Ali_TokenkeyRes::isTimeExpire()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    macroFuncVargs(" Ali_TokenkeyRes::isTimeExpire: current ali time,%d", tv.tv_sec);
    if (tv.tv_sec >= this->expireTime + this->maxExpireSec)
    {
        return false;
    }
    return true;
}

Ali_TokenkeyRes::Ali_TokenkeyRes()
{

    if (!this->isTimeExpire())
    {
        this->RefreshTokenId();
    }
}

Ali_TokenkeyRes::~Ali_TokenkeyRes()
{
}

//获取访问令牌TokenId
int Ali_TokenkeyRes::RefreshTokenId()
{
    NlsToken nlsTokenRequest;
    /*设置阿里云账号KeySecret*/
    nlsTokenRequest.setKeySecret(this->keySecret);
    /*设置阿里云账号KeyId*/
    nlsTokenRequest.setAccessKeyId(this->keyId);

    /*获取token. 成功返回0, 失败返回-1*/
    if (-1 == nlsTokenRequest.applyNlsToken())
    {
        cout << "Ali_TokenkeyRes::getTokenId failed: " << nlsTokenRequest.getErrorMsg() << endl; /*获取失败原因*/
        return -1;
    }
    else
    {
        this->tokenId = nlsTokenRequest.getToken();
        this->expireTime = nlsTokenRequest.getExpireTime();
        cout << "TokenId: " << this->tokenId << endl;               /*获取TokenId*/
        cout << "TokenId expireTime: " << this->expireTime << endl; /*获取Token有效期时间戳(秒)*/
        return 0;
    }
}

string Ali_TokenkeyRes::getTokenId()
{
    return this->tokenId;
}