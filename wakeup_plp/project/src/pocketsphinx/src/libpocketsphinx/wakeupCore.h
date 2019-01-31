#ifndef __TWWAKEUP_H__
#define __TWWAKEUP_H__

#include <sphinxbase/err.h>
#include <sphinxbase/ad.h>
#include <pocketsphinx.h>

using namespace std;


class CTwWakeup
{
public:
	CTwWakeup();
	virtual ~CTwWakeup();
	int init(float kws_threshold, float kws_plp, float duration_min, float duration_max);
	void run(const short* datain, int nlen);
	void release();	
	int get_wakeup_status();
	
private:

	ps_decoder_t *m_ps;
	cmd_ln_t *m_config;
	uint8 m_utt_started;
	uint8 m_wakeup;
	int m_wakeup_status;
	int wakeCount;
	float wakeupTimer;
};
#endif /*__TWWAKEUP_H__*/