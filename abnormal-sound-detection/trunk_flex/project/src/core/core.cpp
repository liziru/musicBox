#include "core.h"
#include "../upmix51/upmix51.h"
#include "../ambisonic/ambisonic.h"
#include "../binaural/binaural.h"
#include "../dynamicrangecontrol/dynamicRangeControl.h"
#include "../resample/resample.h"
#include "../sn3d/sn3d.h"  //by zxt
#include <stdio.h>
#include <string.h>

#define DEFAULT_META_DATA_DIST (2.f)
#define DEG_TO_RAD(ang) ((float)(ang) * 0.01745329251994329576923690768489f)
#define DEFAULT_DRC_MAX (1.f)
#define DEFAULT_DIRECT_GAIN (1.f)
#define DEFAULT_ENABLE_DRC (false)

CAudioEngineCore::CAudioEngineCore(const AudioEngineCreateParameter *param) : m_meta(NULL)
                                                                            , m_channel_51_data(NULL)
																			, m_upmix_51(NULL)
																			, m_ambisonic(NULL)
																			, m_binaural(NULL)
																			, m_resample(NULL)
																			, m_drc(NULL)
	                                                                        , m_sn3d(NULL)
	                                                                        , m_data_sn3d(NULL)
{
	if (NULL == param) {
		throw;
	}

	if (param->object_number <= 0) {
		throw;
	}
	else {
		m_object_num = param->object_number;
	}

	if (param->frame_length <= 0 || param->frame_length > 512) {
		throw;
	}
	else {
		m_frame_length = param->frame_length;
	}

	switch (param->sample_rate) {
	case SAMPLE_RATE_16000: {
		m_sample_rate = 16000;
		break;
	}
	case SAMPLE_RATE_32000: {
		m_sample_rate = 32000;
		break;
	}
	case SAMPLE_RATE_44100: {
		m_sample_rate = 44100;
		break;
	}
	case SAMPLE_RATE_48000: {
		m_sample_rate = 48000;
		break;
	}
	default: {
		throw;
	}
	}

	m_profile_id = param->profile_id;

	m_resample = new CResample(param->object_number, param->frame_length, m_sample_rate);  

	if (m_profile_id == 0) {
		//ambisonic array.  
		m_sn3d = new CSn3d(m_profile_id, param->frame_length);
		m_data_sn3d = new float[param->frame_length * 4];
		m_ambisonic = new CAmbisonic(1, param->frame_length, param->object_number, m_sample_rate, param->multithread);
	}
	else if (m_profile_id == 1) {
		//broadside array.
		m_sn3d = new CSn3d(m_profile_id, param->frame_length);
		m_data_sn3d = new float[param->frame_length * 4];
		m_ambisonic = new CAmbisonic(1, param->frame_length, param->object_number, m_sample_rate, param->multithread);
	}
	else if (m_profile_id == 2) {
		//circular array.
		m_sn3d = new CSn3d(m_profile_id, param->frame_length);
		m_data_sn3d = new float[param->frame_length * 4];
		m_ambisonic = new CAmbisonic(1, param->frame_length, param->object_number, m_sample_rate, param->multithread);
	}
	else if (m_profile_id == 3) {
		m_ambisonic = new CAmbisonic(20, param->frame_length, param->object_number, m_sample_rate, param->multithread);
	}
	else if (m_profile_id == 4) {
		if (param->object_number != 2) {
			/* upmix requires stereo input */
			throw;
		}
		m_channel_51_data = new float[param->frame_length * 6];
		m_meta = new MetaData[6];
		m_upmix_51 = new CUpmix51(param->frame_length, m_sample_rate);
		m_ambisonic = new CAmbisonic(20, param->frame_length, 6, m_sample_rate, param->multithread);
		/* Set default meta data. */
		m_meta[0].dist = DEFAULT_META_DATA_DIST; m_meta[0].azi = DEG_TO_RAD(  45); m_meta[0].elv = 0.f;
		m_meta[1].dist = DEFAULT_META_DATA_DIST; m_meta[1].azi = DEG_TO_RAD(- 45); m_meta[1].elv = 0.f;
		m_meta[2].dist = DEFAULT_META_DATA_DIST; m_meta[2].azi = DEG_TO_RAD(   0); m_meta[2].elv = 0.f;
		m_meta[3].dist = DEFAULT_META_DATA_DIST; m_meta[3].azi = DEG_TO_RAD(   0); m_meta[3].elv = 0.f;
		m_meta[4].dist = DEFAULT_META_DATA_DIST; m_meta[4].azi = DEG_TO_RAD( 110); m_meta[4].elv = 0.f;
		m_meta[5].dist = DEFAULT_META_DATA_DIST; m_meta[5].azi = DEG_TO_RAD(-110); m_meta[5].elv = 0.f;
	}
	
	else if (m_profile_id == 5) {
		m_meta = new MetaData[6];
		m_ambisonic = new CAmbisonic(20, param->frame_length, 6, m_sample_rate, param->multithread);
		/* Set default meta data. */
		m_meta[0].dist = DEFAULT_META_DATA_DIST; m_meta[0].azi = DEG_TO_RAD(  45); m_meta[0].elv = 0.f;
		m_meta[1].dist = DEFAULT_META_DATA_DIST; m_meta[1].azi = DEG_TO_RAD(- 45); m_meta[1].elv = 0.f;
		m_meta[2].dist = DEFAULT_META_DATA_DIST; m_meta[2].azi = DEG_TO_RAD(   0); m_meta[2].elv = 0.f;
		m_meta[3].dist = DEFAULT_META_DATA_DIST; m_meta[3].azi = DEG_TO_RAD(   0); m_meta[3].elv = 0.f;
		m_meta[4].dist = DEFAULT_META_DATA_DIST; m_meta[4].azi = DEG_TO_RAD( 110); m_meta[4].elv = 0.f;
		m_meta[5].dist = DEFAULT_META_DATA_DIST; m_meta[5].azi = DEG_TO_RAD(-110); m_meta[5].elv = 0.f;
	}
	else if (m_profile_id == 8) {
		//QB solution.
	}
	else {
		throw;
	}

	float ch = 4;
 	m_drc_max = DEFAULT_DRC_MAX;  /* 16bit */
 	m_drc = new CDynamicRangeControl(param->frame_length, ch, m_drc_max);
	m_enable_drc = DEFAULT_ENABLE_DRC;
}

CAudioEngineCore::~CAudioEngineCore()
{
	if (NULL != m_meta) { delete [] m_meta; m_meta = NULL; }
	if (NULL != m_channel_51_data) { delete [] m_channel_51_data; m_channel_51_data = NULL; }
	if (NULL != m_upmix_51) { delete m_upmix_51; m_upmix_51 = NULL; }
	if (NULL != m_ambisonic) { delete m_ambisonic; m_ambisonic = NULL; }
	if (NULL != m_binaural) { delete m_binaural; m_binaural = NULL; }
	if (NULL != m_resample) { delete m_resample; m_resample = NULL; }
	if (NULL != m_drc) { delete m_drc; m_drc = NULL; }
	if (NULL != m_sn3d) { delete m_sn3d; m_sn3d = NULL; }  
	if (NULL != m_data_sn3d) { delete[] m_data_sn3d; m_data_sn3d = NULL; }
}

void CAudioEngineCore::process(const AudioEngineProcessParameter *param)
{
	if (NULL == param) {
		throw;
	}

	switch (m_profile_id) {
	case 0: 
	case 1: 
	case 2: {
		m_sn3d->process(param->input, m_data_sn3d);
		m_ambisonic->process(&param->head, m_data_sn3d, param->output, param->meta);
		break;
	}
	case 3: {
		m_ambisonic->process(&param->head, param->input, param->output, param->meta);
		break;
	}
	case 4: {
		m_upmix_51->process(param->input, m_channel_51_data);
		m_ambisonic->process(&param->head, m_channel_51_data, param->output, m_meta);  //use internal metadata.
		break;
	}
	case 5: {
		m_ambisonic->process(&param->head, param->input, param->output, m_meta);  //use internal metadata.
		break;
	}
	case 8: {
		//QB solution.
		break;
	}
	default:
		break;
	}

	if (m_enable_drc) {
		m_drc->process(param->output, param->output);
	}
}

void CAudioEngineCore::set_param(const AudioEngineSettings *settings)
{
	m_enable_drc = settings->dyc_on;
	m_drc_max = settings->dyc_max_val;
	m_format = settings->formatid;
	if (m_enable_drc) {
		m_drc->set(m_drc_max);
	}
	m_sn3d->set(m_format);
}

int CAudioEngineCore::get_object_num() const
{
	return m_object_num;
}
