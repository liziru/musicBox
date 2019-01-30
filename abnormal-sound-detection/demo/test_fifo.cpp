#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>  
#include "fifo.h"
#define FRAMELEN 256
#define FRAMECOUNT 100
void *write_process(void *p){
    FIFO_Type   *pfifo = (FIFO_Type *)p;  
	FILE *input = NULL;
	int err;
	short inputData16s[FRAMELEN];
	if ((input = fopen("./dump_data.pcm", "rb")) == NULL)
	{
		printf("cannot open dump_data file for recording\n");
	}
	while(1){
		err = fread(inputData16s, sizeof(short), FRAMELEN, input);
		if(err != FRAMELEN){
			return NULL;
		}else{
			if(0 == FIFO_Add(pfifo, inputData16s, FRAMELEN)){
				fprintf(stderr, "buf_write fail, buffer is full.\n");
				//return NULL;
			}
		}
	}

}

void *dump_process(void *p){
    FIFO_Type   *pfifo = (FIFO_Type *)p;  
	short inputData16s[FRAMELEN];
	FILE *dumpReadOutput = NULL;
	if ((dumpReadOutput = fopen("./dump_read_data.pcm", "wb")) == NULL)
	{
		printf("cannot open dump_data file for recording\n");
	} 
	fprintf(stdout, "capture: dump_read_data file for recording opened\n");

	while(1){
		if(0 == FIFO_Get(pfifo, inputData16s, FRAMELEN)){
			fprintf(stderr, "buf_read fail, buffer is empty.\n");
			//return NULL;
		}else{
			fwrite(inputData16s, sizeof(short), FRAMELEN, dumpReadOutput);
			fflush(dumpReadOutput);
		}
	}

}
int main(int argc, char *argv[])
{
    FIFO_Type   fifo;  
    FIFO_Type   *pfifo;  
    pthread_t th_capture, th_playback;
    void *retval;
	short *sArray = (short *)malloc(sizeof(short) * FRAMELEN * FRAMECOUNT);

    pfifo = &fifo;  
    FIFO_Init(pfifo, sArray, sizeof(short), FRAMELEN * FRAMECOUNT);  
    pthread_create(&th_playback, NULL, dump_process, (void *)pfifo);  
    pthread_create(&th_capture, NULL, write_process, (void *)pfifo);  
    pthread_join(th_capture, &retval);  
    pthread_join(th_playback, &retval);  
}
