#include "serialControl.h"
#include <wiringPi.h>
#include <wiringSerial.h>

bool serialWrite(const char *serialPort, int serialRate, string data)
{
    int fd;
    if ((fd = serialOpen(serialPort, serialRate)) < 0)
    {
        return false;
    }
    if (wiringPiSetup() == -1)
    {
        return false;
    }
    serialPuts(fd, data.c_str());
    serialFlush(fd);
    delay(3);
    serialClose(fd);
    return true;
}