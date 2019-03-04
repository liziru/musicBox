#include "mainFunc.h"

int main(int argc, char *argv[])
{

#ifndef USER_DEBUG
    int canpass = getPass();
    if (canpass != 1)
    {
        macroFunc("failed start.");
        return 0;
    }
#endif
    printf("\n===============================\n main start.\n");

    // 变量定义
    pthread_t th_capture, th_playback;
    int capture_retval = 0;
    int playback_retval = 0;
    sounder_param *snd_param = (sounder_param *)malloc(sizeof(sounder_param));
    snd_param_bak = snd_param;

    float *sArray = (float *)malloc(sizeof(float) * FRAMELEN * FIFO_BUFF_COUNT);
    snd_param->pfifo = &snd_param->fifo;
    FIFO_Init(snd_param->pfifo, sArray, sizeof(float) * FRAMELEN, FIFO_BUFF_COUNT);

    if (argc < 19)
    {
        printUsage();
    }
    //add 2018-4-10
    struct sched_param param_capture, param_playback;
    pthread_attr_t attr_capture, attr_playback;
    pthread_attr_init(&attr_capture);
    pthread_attr_init(&attr_playback);
#ifndef USER_DEBUG
    param_capture.sched_priority = 90;
    param_playback.sched_priority = 80;
    pthread_attr_setschedpolicy(&attr_capture, SCHED_RR);
    pthread_attr_setschedparam(&attr_capture, &param_capture);
    pthread_attr_setinheritsched(&attr_capture, PTHREAD_EXPLICIT_SCHED);

    pthread_attr_setschedpolicy(&attr_playback, SCHED_RR);
    pthread_attr_setschedparam(&attr_playback, &param_playback);
    pthread_attr_setinheritsched(&attr_playback, PTHREAD_EXPLICIT_SCHED);
#endif

    setTimer();
    initAudioParam(argv, snd_param);
    snd_param->is_first_wait = true;
    pthread_cond_init(&snd_param->cond, NULL);
    pthread_mutex_init(&snd_param->mutex, NULL);

    sigignore(SIGSEGV);

    while (1)
    {

        FIFO_Clear(snd_param->pfifo);
        /* 创建线程*/
        pthread_create(&th_capture, &attr_capture, capture_process, (void *)snd_param);
        pthread_mutex_lock(&snd_param->mutex);
        pthread_cond_wait(&snd_param->cond, &snd_param->mutex);
        pthread_mutex_unlock(&snd_param->mutex);
        pthread_create(&th_playback, &attr_playback, playback_process, (void *)snd_param);
        /* 等待两个线程结束*/
        pthread_join(th_playback, (void **)&playback_retval);
        macroFuncVargs("main: playback_retval:%d", (int)playback_retval);
        if ((int)playback_retval < 0)
        {
            pthread_cancel(th_capture);
        }

        sleep(3);
        thread_log_tag = 1;
        while (thread_exit(&th_capture))
        {
            usleep(100);
        }
    }

    pthread_attr_destroy(&attr_capture);
    pthread_attr_destroy(&attr_playback);

    if (NULL != snd_param)
    {
        free(snd_param);
        snd_param = NULL;
    }
    if (NULL != sArray)
    {
        free(sArray);
        sArray = NULL;
    }

    return 0;
}
