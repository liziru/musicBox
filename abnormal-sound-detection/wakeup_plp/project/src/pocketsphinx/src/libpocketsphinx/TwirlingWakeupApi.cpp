#include "wakeupCore.h"

//#include "android/log.h"
static const char *TAG="byqtm";

#define LOGI(fmt, args...) //__android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) //__android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) //__android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)



#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C" 
#endif



void * WakeupInit(float param1, float param2, float param3, float param4)
{
	LOGD("lib-WakeupInit");
	CTwWakeup *obj = new CTwWakeup();
	if (obj)
	{
	LOGD("param1 =%.30lf",param1);
	LOGD("param2 =%f",param2);
		obj->init(param1, param2, param3, param4);
		LOGD("WakeupInit res=%d",res);
	}
	LOGD("WakeupInit obj=%lu",obj);
	return obj;
}


#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void WakeupRun(void *obj, const short* datain, int nlen)
{
//LOGD("WakeupRun obj=%d",obj);
	if (obj)
	{		
	//LOGD("WakeupRun obj1");
		((CTwWakeup *)obj)->run(datain,nlen);
	}	
}


#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
int GetWakeupStatus(void *obj)
{
	if (obj)
	{
		return ((CTwWakeup *)obj)->get_wakeup_status();
	}
	else
	{
		return false;
	}

}

// return a NULL pointer if success
#ifdef _WINDLL
extern "C" __declspec(dllexport)
#else
extern "C"
#endif
void* WakeupRelease(void *obj)
{
	if (obj)
	{
		delete (CTwWakeup*)obj;
		obj = NULL;
	}
	return obj;	
}

