#if !defined(__THREAD_FUNC)
#define __THREAD_FUNC

#include <pthread.h>
#include "fifo.h"

typedef struct dispatcher_funcPointer
{
    void *iwps;
    void *mds;
    void *asr;
} FUNC_POINTER;

typedef struct audioDispatcher
{
    int status; //0:awakeup process; 1:asr.
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    bool ansIsOn;
    bool drcIsOn;
} DISPATCHER;

typedef struct
{
    short *sArray;
    FIFO_Type fifo;
    FIFO_Type *pfifo;
} Fifo_t;

//宏定义
// #define MAX16S 32767;
// #define MIN16S -32768;
// #define MAXABS16S 32768;

//函数声明
/**
 * @description: 
 * @param {type} 
 * @return: 
 */
void initThreadArgu();
/**
 * @description: 
 * @param {type} 
 * @return: 
 */
void destroyThreadArgu();

void priFifoInit(Fifo_t fifo, unsigned int framelen, unsigned int chan, unsigned int fifo_buffer_count);

void priFifoDestroy(Fifo_t fifo_t);

void *audioPreprocessDispatcher_thread(void *p);

void *sm_thread(void *p);

void *neteaseSerive_thread(void *p);

void *neteaseLanucher_thread(void *p);
#endif // __THREAD_FUNC
