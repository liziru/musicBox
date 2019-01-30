#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <cstdlib>
#include "TwirlingWakeupApi.h"



int main(int argc,char *argv[])
{
	if (argc != 4 && argc != 2) {
		printf("arguments number is wrong!\n");
		printf("App.exe wav [keyword_threshold] [kws_plp]\n");
		printf("keyword_threshold default:1e-30, kws_plp default:1e-1\n");
		return 0;
	}

	short adbuf[256];
	const char *fname;
	int k;
	void *m_wakeup_obj = NULL;
	FILE *rawfd;
	fname = argv[1];
	if ((rawfd = fopen(fname, "rb")) == NULL) {
		printf("Failed to open file '%s' for reading",	fname);
	}

	if (argc == 4) {
		float threshold=atof(argv[2]);
		float plp=atof(argv[3]);
		m_wakeup_obj = WakeupInit(threshold,plp);
	}
	else {
		m_wakeup_obj = WakeupInit();
	}

	if (m_wakeup_obj != NULL){
		printf("successfully init for wakup!\n");
	}


	int FrameLength = 160;
	while ((k = fread(adbuf, sizeof(short), FrameLength, rawfd)) > 0) {
		
		short *m_recBuffer = (short *)adbuf;
		WakeupRun(m_wakeup_obj, m_recBuffer, k);
		int wakeup_flag = GetWakeupStatus(m_wakeup_obj);
		if (wakeup_flag == 1)
		{
			printf("1\n");
		}
		else if(wakeup_flag == 0)
		{
			printf("0\n");
		}
		else 
		{
			printf("2\n");
		}	
	}
	WakeupRelease(m_wakeup_obj);
	fclose(rawfd);

        return 1;

}
