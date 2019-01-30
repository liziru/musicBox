#ifndef __TWIRLING_WAKEUP_API_H__
#define __TWIRLING_WAKEUP_API_H__

#ifdef __cplusplus
extern "C"{
#endif

void * WakeupInit(float param1 = 1e-10, float param2 = 1e-1, float param3 = 300, float param4 = 2000);

void WakeupRun(void *obj, const short* datain, int nlen);

int GetWakeupStatus(void *obj);

void* WakeupRelease(void *obj);

#ifdef __cplusplus				
};
#endif

#endif // __TWIRLING_WAKEUP_API_H__