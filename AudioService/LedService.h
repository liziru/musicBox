#ifndef __LEDSERVICE_H__
#define __LEDSERVICE_H__
#include "WakeupEvent.h"
#include "wiringPi.h"
#include "gpio.h"
#include "WakeupListenner.h"
#include "time.h"
#include <sys/time.h>
#include <pthread.h>
class LedService : public WakeupListenner
{
  public:
  	LedService(int pin, int default_mode = 0, int sleep_ms = 1500);
    void onWakeup(WakeupEvent *wakeupEvent);
    ~LedService();
	void lightUp();
	void blackOut();

	bool getIsRun();

    void run();
    void stop();
  private:
  	void digitalPull(int pin, int mode);
  	int m_pin;
	int m_isrun;
	int m_sleep_ms;
	pthread_cond_t m_Cond;
    pthread_mutex_t m_Mutex;
	pthread_t m_th;
	int m_light_status;

	static void *m_Process(void *p);
	struct timeval m_start_time;

};
#endif  //__LEDSERVICE_H__