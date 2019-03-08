#include "util.h"

long str2time(string datetime)
{
    struct tm tm_time;
    long unixtime;
	strptime(datetime.c_str(), "%Y-%m-%d %H:%M:%S", &tm_time);

    unixtime = mktime(&tm_time);
    return unixtime;
}

long getCurrUnixtime(void)
{
    time_t now;
    long unixtime = time(&now);
    return unixtime;
}

string getTimeStr(){
    struct tm *t;
    time_t tt;
    time(&tt);
    t = localtime(&tt);
	char tmp[50] = {0};
	sprintf(tmp, "%4d_%02d_%02d-%02d_%02d_%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	return string(tmp);
}

string getDateTimeSqlStr(){
    struct tm *t;
    time_t tt;
    time(&tt);
    t = localtime(&tt);
	char tmp[50] = {0};
	sprintf(tmp, "%4d-%02d-%02d %02d:%02d:%02d.000", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	return string(tmp);
}
bool fileIsExist(string fileName){
	if(access(fileName.c_str(), R_OK | W_OK) < 0){
		return false;
	}
	return true;
}

bool deleteFile(string fileName){
	if(fileIsExist(fileName)){
		if(0 != remove(fileName.c_str())){
			return false;
		}else{
			return true;
		}
	}
	return true;
}

//C++方法：将 string 转换为数值
double convertFromString(const string &s)
{
	istringstream i(s);
	double x;
	if (i >> x)
		return x;
	return 0.0;//if error
}

int randomNumber(int start, int end)
{
	srand((unsigned)time(NULL));
	int dis = end - start;
	return rand() % dis + start;
}

string getContentFilePath()
{
	return "/home/pi/workspace/alize/";
}

string getGmmFilePath()
{
    return getContentFilePath() + "gmm/";
}

static string getGmmFileName(string userName)
{
    long r = getCurrentTime();
    string rs = userName + string("_") + to_string(r) + ".gmm";
    return rs;
}
string getGmmUniqueFileName(string userName)
{
    string tmp = getGmmFilePath() + getGmmFileName(userName);
    while (true)
    {
        if (fileIsExist(tmp))
        {
            tmp = getGmmFilePath() + getGmmFileName(userName);
        }
        else
        {
            return tmp;
        }
    }
}

long getCurrentTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
int broadcastData(string ipStr, string dataStr)
{

    int sd, rc, i;
    struct sockaddr_in cliAddr, remoteServAddr;
    struct hostent *h;
    int broadcast = 1;

    /* check empty */
    if (ipStr.empty() || dataStr.empty())
    {
        return -1;
    }

    /* get server IP address (no check if input is IP address or DNS name */
    h = gethostbyname(ipStr.c_str());
    if (h == NULL)
    {
        // //printf("%s: unknown host '%s' \n", ipStr.c_str(), argv[1]);
        return -2;
    }

    //printf("%s: sending data to '%s' (IP : %s) \n", ipStr.c_str(), h->h_name,
    //       inet_ntoa(*(struct in_addr *)h->h_addr_list[0]));

    remoteServAddr.sin_family = h->h_addrtype;
    memcpy((char *)&remoteServAddr.sin_addr.s_addr,
           h->h_addr_list[0], h->h_length);
    remoteServAddr.sin_port = htons(REMOTE_SERVER_PORT);

    /* socket creation */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        //printf("%s: cannot open socket \n", ipStr.c_str());
        return -3;
    }

    if (setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1)
    {
        // perror("setsockopt (SO_BROADCAST)");
        return -4;
    }

    /* bind any port */
    cliAddr.sin_family = AF_INET;
    cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    cliAddr.sin_port = htons(0);

    rc = bind(sd, (struct sockaddr *)&cliAddr, sizeof(cliAddr));
    if (rc < 0)
    {
        //printf("%s: cannot bind port\n", ipStr.c_str());
        return -6;
    }

    /* send data */
    rc = sendto(sd, dataStr.c_str(), dataStr.size() + 1, 0,
                (struct sockaddr *)&remoteServAddr,
                sizeof(remoteServAddr));

    if (rc < 0)
    {
        //printf("%s: cannot send data %d \n", ipStr.c_str(), i - 1);
        close(sd);
        sd = 0;
        return -7;
    }
    if (sd > 0)
    {
        close(sd);
        sd = 0;
    }

    return 0;
}

uint32_t getIpAddress()
{
	struct ifaddrs *ifaddr, *ifa;
	int family, s, n;
	uint32_t result;

	if (getifaddrs(&ifaddr) == -1)
	{
		return 0;
	}

	for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++)
	{
		if (ifa->ifa_addr == NULL)
			continue;

		if (ifa->ifa_addr->sa_family != AF_INET)
			continue;

		if (strcmp(ifa->ifa_name, "lo") == 0)
			continue;

		/* AF_INET */
		result = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr;
		freeifaddrs(ifaddr);
		return result;
	}

	freeifaddrs(ifaddr);
	return 0;
}

string ipAddressToString(uint32_t ip)
{
	char result[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, (void *)&ip, result, sizeof(result));

	return string(result);
}

//返回MAC地址字符串  
string getMacAddressToString()  
{  
    struct ifreq tmp;  
    int sock_mac;  
    char mac_addr[30];  
    sock_mac = socket(AF_INET, SOCK_STREAM, 0);  
    if( sock_mac == -1)  
    {  
        // perror("create socket fail\n");  
        return "";  
    }  
    memset(&tmp,0,sizeof(tmp));  
    strncpy(tmp.ifr_name,"eth0",sizeof(tmp.ifr_name)-1 );  
    if( (ioctl( sock_mac, SIOCGIFHWADDR, &tmp)) < 0 )  
    {  
        // printf("mac ioctl error\n");  
        return "";  
    }  
    sprintf(mac_addr, "%02x%02x%02x%02x%02x%02x",  
        (unsigned char)tmp.ifr_hwaddr.sa_data[0],  
        (unsigned char)tmp.ifr_hwaddr.sa_data[1],  
        (unsigned char)tmp.ifr_hwaddr.sa_data[2],  
        (unsigned char)tmp.ifr_hwaddr.sa_data[3],  
        (unsigned char)tmp.ifr_hwaddr.sa_data[4],  
        (unsigned char)tmp.ifr_hwaddr.sa_data[5]  
    );  
    close(sock_mac);  
    return string(mac_addr);
}

bool saveTextToFile(string fileName, string text)
{
    ofstream outf; 
    outf.open(fileName);
    outf<<text<<endl;
    outf.close();
    return fileIsExist(fileName);
}

string readFileToText(string fileName)
{
    ifstream inf; 
    inf.open(fileName);
    string rs;
    string tmp;
    while(getline(inf, tmp)){
        rs += tmp;
    }
    inf.close();
    return rs;
}
vector<int> getProcessPid(string processName)
{
    char buff[80];
    int i = 0;
    char *ret;
    vector<int> rs;

    sprintf(buff, "pidof %s", processName.c_str());
    FILE *pidreader = popen(buff, "r");
    memset(buff, 0, 80);

    ret = fgets(buff, 79, pidreader);
    pclose(pidreader);

    ret = strtok(buff, " ");
    while (ret != NULL)
    {
        rs.push_back(atoi(ret));
        ret = strtok(NULL, " ");
    }
    return rs;
}
