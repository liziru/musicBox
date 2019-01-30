#ifndef __WAKEUP_API_H__
#define __WAKEUP_API_H__

#ifdef __cplusplus
extern "C"{
#endif

char* wakeupGetVersion();

void* wakeupInit(int stride_len, int chs, int sRate, char *graph, char *labels, char *keyword, 
				 int smooth_N, float detect_T, int clip_len, float detect_N, bool printFlag);


void wakeupSet(void *obj, float detectTh);


int wakeupProcess(void *obj, const float* input_data);


void wakeupRelease(void *obj);

#ifdef __cplusplus				
};
#endif

#endif  // __WAKEUP_API_H__
