#ifndef _MIC_ARRAY_CORE_H__
#define _MIC_ARRAY_CORE_H__


class CLocalizer;
class CBeamforming;
class CAec;


class CMicArrayCore
{
public:
	CMicArrayCore(int profile_id, int frame_length, int sample_rate, int mic_channel_num, float mic_dist, int ref_channel_num, int aec_sys_latency);
	virtual ~CMicArrayCore();

	void process(float *audio_input, float *audio_output, float *audio_reference);
	void set_status(int status);
	void set_aec(bool enableRes, float resLevel, bool enableNS, float nsDB, bool enableSpkClip, float spkClipThd, float maxCoupling, float adaptStep);
	void set_beamforming(int mode, float max_sup_db, float targetDirection, float targetEstimation, float nlp_sup, float tolerance, int targetFlag);
	void set_doa(int formatid);
	float get_current_doa();
	void set_target_doa(float target_direction);

protected:

private:
	typedef struct{
		bool enableRes = false;
		float resLevel = 1.0;
		bool enableNS = false;
		float nsDB = -10;
		bool enableSpkClip = false;
		float spkClipThd = 1.0;
		float maxCoupling = 10.0;
		float adaptStep = 0.5;
	}aec_param;

	typedef struct{
		int mode = 7;
		float max_sup_db = -10.0;
		float targetDirection = 0;
		float targetEstimation = 0;
		float nlp_sup = 0.0;
		float tolerance = 30.0;
		int targetFlag = 0;
	}beam_param;

private:
	int m_profile_id;
	int m_frame_length;
	int m_sample_rate;
	int m_mic_channel_num;
	float m_mic_dist;
	int m_ref_channel_num;
	int m_out_channel_num;
	int m_aec_filter_len;
	int m_aec_sys_latency;
	bool m_multithread;
	int m_doa_info_len;
	float m_doa_current;
	int m_status;  //0: not waked up;  1: waked up

	float *m_audio_in;
	float *m_audio_out;
	float *m_audio_ref;
	float *m_doa_info;
	float *m_audio_tmp_buf;

	int m_num_read;
	int m_num_write;
	int m_length_in;
	int m_length_out;
	int m_process_length;
	float *m_buffer_in;
	float *m_buffer_ref;
	float *m_buffer_out;
	float *m_data_out;

	float *m_in_iter;
	float *m_ref_iter;
	float *m_out_iter;
	float *m_buffer_in_iter;
	float *m_buffer_out_iter;
	float *m_buffer_ref_iter;

	CAec **m_aec_array;  //temporary solution
	//CAec *m_aec;
	CBeamforming *m_beamforming;
	CLocalizer *m_localizer;

	aec_param m_aec_param;
	beam_param m_beam_param;

	//evaluation version
	bool m_evaluation;
	int  m_evaluation_timer;
	int  m_evaluation_period;

	
};

#endif /*__AUDIO_ENGINE_CORE_H__*/