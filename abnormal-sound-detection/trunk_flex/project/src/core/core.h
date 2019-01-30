#ifndef __AUDIO_ENGINE_CORE_H__
#define __AUDIO_ENGINE_CORE_H__

#include "def.h"

class CUpmix51;
class CAmbisonic;
class CBinaural;
class CDynamicRangeControl;
class CResample;
class CDoa;
class CSn3d; //by zxt

#define PROFILE_ID_COUNT   (22)

class CAudioEngineCore
{
public:
	CAudioEngineCore(const AudioEngineCreateParameter *param);
	virtual ~CAudioEngineCore();

	void process(const AudioEngineProcessParameter *param);
	void set_param(const AudioEngineSettings *settings);
	int get_object_num() const;

protected:

private:
	int m_profile_id;
	MetaData *m_meta;
	int m_object_num;
	int m_frame_length;
	int m_sample_rate;
	float *m_channel_51_data;
	CUpmix51 *m_upmix_51;
	CAmbisonic *m_ambisonic;
	CBinaural *m_binaural;
	CDynamicRangeControl *m_drc;
	float m_drc_max;
	bool m_enable_drc;
	CResample *m_resample;
	CSn3d *m_sn3d; 
	int m_format;
	float *m_data_sn3d;
};

#endif /*__AUDIO_ENGINE_CORE_H__*/