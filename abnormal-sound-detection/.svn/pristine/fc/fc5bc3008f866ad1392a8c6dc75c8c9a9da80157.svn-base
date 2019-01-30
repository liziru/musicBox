#ifndef __TWIRLING_WAKEUP_API_H__
#define __TWIRLING_WAKEUP_API_H__

#ifdef __cplusplus
extern "C"{
#endif

void * WakeupInit(float kws_threshold=1e-20, float kws_plp=20);

void WakeupRun(void *obj, const short* datain, int nlen);

bool GetWakeupStatus(void *obj);

void* WakeupRelease(void *obj);

#ifdef __cplusplus				
};
#endif

#endif // __TWIRLING_WAKEUP_API_H__
