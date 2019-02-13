#if !defined(_ALI_TOKENKEY_RES)
#define _ALI_TOKENKEY_RES
#include <string>
#include <iostream>
#include "Token.h"

using namespace std;

class Ali_TokenkeyRes
{
private:
  string tokenkey;
  long expireTime;
  string tokenId;
  static const string keySecret ; //<AccessKey Secret>
  static const string keyId ;                   //<Appkey Id>
  static const long maxExpireSec = 172800;

public:
  Ali_TokenkeyRes();
  ~Ali_TokenkeyRes();

  bool isTimeExpire();
  int RefreshTokenId();

  string getTokenId();
};

#endif // _ALI_TOKENKEY_RES
