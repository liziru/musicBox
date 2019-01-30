#include "wakeupCore.h"
#include <sphinxbase/err.h>
#include <sphinxbase/ad.h>
#include <pocketsphinx.h>
//#include "string.h"
#include <string.h>
#include "kws_search.h"

//#include "android/log.h"
static const char *TAG="byqtm";
#define LOGI(fmt, args...) //__android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) //__android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...)// __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)
//
CTwWakeup::CTwWakeup() 
{		
	
	
}

CTwWakeup::~CTwWakeup()
{ 
	release();
}


int CTwWakeup::init(float kws_threshold, float kws_plp, float duration_min, float duration_max)
{	
	static const arg_t cont_args_def[] = {
		POCKETSPHINX_OPTIONS,
		CMDLN_EMPTY_OPTION
	};

	kws_plp_param = kws_plp;
	kws_threshold_param = kws_threshold;
	duration_min_param = duration_min;
	duration_max_param = duration_max;

	m_config = cmd_ln_parse_r(NULL, cont_args_def, 0, NULL, TRUE);

	ps_default_search_args(m_config);

	m_ps = ps_init(m_config);

	if (m_ps == NULL) {
		cmd_ln_free_r(m_config);
		return 1;
	}
	if (ps_start_utt(m_ps) < 0){
		LOGD("Failed to start utterance\n");
	}
	m_utt_started = FALSE;
	m_wakeup = FALSE;
	m_wakeup_status = 0x0000;
	wakeCount = 0;
	wakeupTimer = 0;
	return 0;
}




void CTwWakeup::release()
{
	ps_end_utt(m_ps);
	ps_free(m_ps);
	cmd_ln_free_r(m_config);

	return;
}

#if 0
//\in buffer is 16bits
void CTwWakeup::run(const short* datain, int nlen)
{
//LOGD("CTwWakeup::run");
	const int16 *recBuf16 = (int16*)datain;

	const char *hyp;

	int32 k; //real samples number
	int32 buffer_len;
	uint8 in_speech;
	m_wakeup_status = 0;


	size_t perSendSize = 2048;
	size_t curSize = 0;

	while (curSize < nlen)
	{
		buffer_len = (nlen - curSize) > perSendSize ? perSendSize : (nlen - curSize);
		k = buffer_len;  // get the number of the samples;
		//LOGD("CTwWakeup::run1");
		ps_process_raw(m_ps, recBuf16, k, FALSE, FALSE);
		//LOGD("CTwWakeup::run2");
		in_speech = ps_get_in_speech(m_ps);
		//LOGD("CTwWakeup::run3");
		if (in_speech && !m_utt_started) {
			//LOGD("CTwWakeup::run4");
			m_utt_started = TRUE;
			E_INFO("Listening...\n");
			LOGD("CTW");
		}
		//LOGD("CTwWakeup::run6");
		if (!in_speech && m_utt_started) {
			LOGD("CTwWakeup::run7");
			/* speech -> silence transition, time to start new utterance  */
			ps_end_utt(m_ps);
			//LOGD("CTwWakeup::run8");
			hyp = ps_get_hyp(m_ps, NULL);
			//LOGD("CTwWakeup::run8");
			if (hyp != NULL) {
				LOGD("CTJ");
				m_wakeup_status = 0x0001;
				printf("%s\n", hyp);
				fflush(stdout);
			}
			else
			{
				LOGD("CTD");
				m_wakeup_status = 0x0002;
			}
			if (ps_start_utt(m_ps) < 0)
				E_FATAL("Failed to start utterance\n");
			m_utt_started = FALSE;
			E_INFO("Ready....\n");
		}
		
		curSize += buffer_len;
	}
}
#endif
void CTwWakeup::run(const short* datain, int nlen)
{
	const int16 *recBuf16 = (int16*)datain;
	const char *hyp;
	int32 k; //real samples number
	int32 buffer_len;
	uint8 in_speech;
	m_wakeup_status = 0;

	size_t perSendSize = 2048;
	size_t curSize = 0;
	wakeCount++;
	while (curSize < nlen)
	{
		buffer_len = (nlen - curSize) > perSendSize ? perSendSize : (nlen - curSize);
		k = buffer_len;  // get the number of the samples;
		ps_process_raw(m_ps, recBuf16, k, FALSE, FALSE);
		in_speech = ps_get_in_speech(m_ps);
		if (in_speech && !m_utt_started) 
		{
			m_utt_started = TRUE;
			E_INFO("Listening...\n");
			LOGD("qtm speech start music");
			wakeupTimer = 0;
		}
		else if (in_speech && m_utt_started)
		{
			int samplerate = 16000;  //samplerate: 16k
			wakeupTimer += (float)buffer_len / samplerate;
			if (wakeupTimer > 0.1)  //300ms
			{
				wakeupTimer = 0;
				hyp = ps_get_hyp(m_ps, NULL);
				if (hyp != NULL)
				{
					LOGD("qtm speeching wake music");
					m_wakeup_status = 0x0001;
					ps_end_utt(m_ps);
					printf("%s\n", hyp);
					fflush(stdout);
					if (ps_start_utt(m_ps) < 0)
					{			
						LOGD("qtm Failed to start");
						E_FATAL("Failed to start utterance\n");
					}
					m_utt_started = FALSE;
					E_INFO("Ready....\n");
				}
			}
			
		}
		else if (!in_speech && m_utt_started) 
		{
			LOGD("qtm speech stop music");
			ps_end_utt(m_ps);			
			hyp = ps_get_hyp(m_ps, NULL);
			if (hyp != NULL) 
			{
				LOGD("qtm speech end wake music");
				m_wakeup_status = 0x0001;
				printf("%s\n", hyp);
				fflush(stdout);
			}
			else
			{
				LOGD("qtm speech end1 music");
				m_wakeup_status = 0x0002;
			}

			if (ps_start_utt(m_ps) < 0)
			{			
				LOGD("qtm Failed to start");
				E_FATAL("Failed to start utterance\n");
			}
			m_utt_started = FALSE;
			m_wakeup = FALSE;
			E_INFO("Ready....\n");
		}
		curSize += buffer_len;
	}

}

int CTwWakeup::get_wakeup_status()
{
	return m_wakeup_status;
}
