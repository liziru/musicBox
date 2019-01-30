#ifndef __SERIAL_CONTROL_H_  
#define __SERIAL_CONTROL_H_
#include <string.h>
#include <string>
using namespace std;

#define GATE_LOCK_SERIAL_PORT "/dev/ttyS1"
#define GATE_LOCK_SERIAL_RATE 115200
bool serialWrite(const char *serialPort, int serialRate, string data);

#endif