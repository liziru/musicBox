#ifndef __UTIL_H_  
#define __UTIL_H_
#include <string.h>
#include <string>
#include <stdio.h>
#include <sys/time.h>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdint.h>
#include <strings.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#define REMOTE_SERVER_PORT 1500

using namespace std;

long str2time(string datetime);

long getCurrUnixtime(void);

string getTimeStr();

string getDateTimeSqlStr();

bool fileIsExist(string fileName);

bool deleteFile(string fileName);

double convertFromString(const string &s);

int randomNumber(int start, int end);

string getContentFilePath();

string getGmmFilePath();

string getGmmUniqueFileName(string userName);

long getCurrentTime();

int broadcastData(string ipStr, string dataStr);

uint32_t getIpAddress();

string ipAddressToString(uint32_t ip);

string getMacAddressToString();

bool saveTextToFile(string fileName, string text);

string readFileToText(string fileName);
#endif

