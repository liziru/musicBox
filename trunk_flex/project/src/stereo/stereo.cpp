#include "stereo.h"
#include <math.h>
#include <string.h>

CStereo::CStereo(int frameLength, int channelN, int sRate) 													 
{
	m_frame_len = frameLength;
	m_channels = channelN;
	m_placementMode = 0;
	m_cameraMode = 1;
}

CStereo::~CStereo()
{
	
}

void CStereo::set(int placementMode, int cameraMode)
{
	m_placementMode = placementMode;
	m_cameraMode = cameraMode;
}


void CStereo::process(const float *in, float *out)   /* both the input and output are interleaving item */
{
	const float *ch1_iter = in, *ch2_iter = in + 1, *ch3_iter = in + 2, *ch4_iter = in + 3;
	float *out_iter = out;
	if (m_cameraMode == BACK) {
		if (m_placementMode == VERTICAL_UP) {
			for (int i = 0; i < m_frame_len; i++) {
				*out_iter = *ch4_iter;
				out_iter++;
				*out_iter = *ch1_iter;
				out_iter++;
				ch1_iter += m_channels; ch4_iter += m_channels;
			}		
		}
		else if(m_placementMode == HORIZONTAL_LEFT) {
			for (int i = 0; i < m_frame_len; i++) {
				*out_iter = *ch2_iter + *ch3_iter;
				out_iter++;
				*out_iter = *ch1_iter + *ch4_iter;
				out_iter++;
				ch1_iter += m_channels; ch2_iter += m_channels; ch3_iter += m_channels; ch4_iter += m_channels;
			}
		}
		else if (m_placementMode == VERTICAL_DOWN) {
			for (int i = 0; i < m_frame_len; i++) {
				*out_iter = *ch1_iter;
				out_iter++;
				*out_iter = *ch4_iter;
				out_iter++;
				ch1_iter += m_channels; ch4_iter += m_channels;
			}
		}
		else if (m_placementMode == HORIZONTAL_RIGHT) {
			for (int i = 0; i < m_frame_len; i++) {
				*out_iter = *ch1_iter + *ch4_iter;
				out_iter++;
				*out_iter = *ch2_iter + *ch3_iter;
				out_iter++;
				ch1_iter += m_channels; ch2_iter += m_channels; ch3_iter += m_channels; ch4_iter += m_channels;
			}
		}
		else {
			throw;
		}
	}
	else if (m_cameraMode == FRONT) {
		if (m_placementMode == VERTICAL_UP) {
			for (int i = 0; i < m_frame_len; i++) {
				*out_iter = *ch1_iter;
				out_iter++;
				*out_iter = *ch4_iter;
				out_iter++;
				ch1_iter += m_channels; ch4_iter += m_channels;
			}
		}
		else if (m_placementMode == HORIZONTAL_LEFT) {
			for (int i = 0; i < m_frame_len; i++) {
				*out_iter = *ch1_iter + *ch4_iter;
				out_iter++;
				*out_iter = *ch2_iter + *ch3_iter;
				out_iter++;
				ch1_iter += m_channels; ch2_iter += m_channels; ch3_iter += m_channels; ch4_iter += m_channels;
			}
		}
		else if (m_placementMode == VERTICAL_DOWN) {
			for (int i = 0; i < m_frame_len; i++) {
				*out_iter = *ch4_iter;
				out_iter++;
				*out_iter = *ch1_iter;
				out_iter++;
				ch1_iter += m_channels; ch4_iter += m_channels;
			}
		}
		else if (m_placementMode == HORIZONTAL_RIGHT) {
			for (int i = 0; i < m_frame_len; i++) {
				*out_iter = *ch2_iter + *ch3_iter;
				out_iter++;
				*out_iter = *ch1_iter + *ch4_iter;
				out_iter++;
				ch1_iter += m_channels; ch2_iter += m_channels; ch3_iter += m_channels; ch4_iter += m_channels;
			}
		}
		else {
			throw;
		}
	}
}
