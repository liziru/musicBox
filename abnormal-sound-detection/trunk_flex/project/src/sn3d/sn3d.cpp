#include "sn3d.h"
#include <math.h>
#include <string.h>

//float low_pass_filter[ORDER + 1] = {
//	0.0320f, 0.0373f, 0.0515f, 0.0750f, 0.1081f, 0.1507f, 0.2024f, 0.2628f, 0.3312f, 0.4069f, 0.4907f,
//	0.5808f, 0.6820f, 0.8098f, 0.9449f, 1.5071f, 0.9449f, 0.8098f, 0.6820f, 0.5808f, 0.4907f, 0.4069f,
//	0.3312f, 0.2628f, 0.2024f, 0.1507f, 0.1081f, 0.0750f, 0.0515f, 0.0373f, 0.0320f
//};
float low_pass_filter_X[ORDER + 1] = {
	0.0034, 0.0044, 0.0074, 0.0119, 0.0178, 0.0243, 0.0308, 0.0368, 0.0415, 0.0446, 0.5457, 0.0446, 0.0415, 0.0368, 0.0308, 0.0243, 0.0178, 0.0119, 0.0074, 0.0044, 0.0034
};

float low_pass_filter_Y[ORDER + 1] = {
	0.0063, 0.0082, 0.0138, 0.0227, 0.0341, 0.0471, 0.0603, 0.0724, 0.0821, 0.0884, 0.5906, 0.0884, 0.0821, 0.0724, 0.0603, 0.0471, 0.0341, 0.0227, 0.0138, 0.0082, 0.0063
};

float low_pass_filter_2mic[ORDER + 1] = {
	0.0218, 0.0290, 0.0495, 0.0831, 0.1292, 0.1868, 0.2534, 0.3237, 0.3882, 0.4344, 0.9513, 0.4344, 0.3882, 0.3237, 0.2534, 0.1868, 0.1292, 0.0831, 0.0495, 0.0290, 0.0218
};


CSn3d::CSn3d(int profileid, int frame_length) : m_mem(NULL)
														 
{
	m_profileid = profileid;
	m_frame_len = frame_length;
	
	if (m_profileid == BROADSIDE) {
		m_mem = new float[ORDER + DELAY];
		memset(m_mem, 0x00, sizeof(float)*(ORDER + DELAY));
	}
	else if (m_profileid == CIRCULAR) {
		m_mem = new float[N * ORDER];
		memset(m_mem, 0x00, sizeof(float)* N * ORDER);
	}
}

CSn3d::~CSn3d()
{
	//if (NULL != m_filter) { delete[] m_filter; m_filter = NULL; }
	if (NULL != m_mem) { delete[] m_mem; m_mem = NULL; }
}

void CSn3d::set(int format) 
{
	m_format = format;
}


void CSn3d::process(const float *in, float *out)   /* both the input and output are interleaving item */
{
	for (int c = 0; c < N; c++)
	{
		to_sn3d(in, out, c);
	}

	if (m_profileid == CIRCULAR && m_format == CIRCULAR_3MIC) {
		const float sqrt3 = sqrt(3.0f);
		float data[FRAMELEN], *data_iter = data;
		float *out_iter, *out_iter1;

		out_iter = out + 3;
		out_iter1 = out + 1;

		for (int i = 0; i < m_frame_len; i++) {
			*data_iter = 2.0f / sqrt3 * *out_iter + 1.0f / sqrt3 * *out_iter1;
			data_iter++; out_iter += 4; out_iter1 += 4;
		}

		out_iter = out + 3;
		data_iter = data;
		for (int i = 0; i < m_frame_len; i++) {
			*out_iter = *data_iter;
			out_iter += 4; data_iter++;
		}
		out_iter = out + 3;
		out_iter1 = out + 1;
	}
}

void CSn3d::to_sn3d(const float *in, float *out, int c)
{
	if (m_profileid == AMBISONIC) {
		if (m_format == A_FORMAT) {
			static float sqrt3 = sqrtf(3.0f);
			const float *ch1_iter = in, *ch2_iter = in + 1, *ch3_iter = in + 2, *ch4_iter = in + 3;
			float *out_iter = out + c;
			float m_factor[3];
			m_factor[0] = 0.5f;
			m_factor[1] = 1.2247f;
			m_factor[2] = 0.5f*sqrt3;   /* A format is changed to B format, and then rotated by 135 */

			switch (c) {
			case 0: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = m_factor[0] * (*ch1_iter + *ch2_iter + *ch3_iter + *ch4_iter);
					out_iter += 4; ch1_iter += 4; ch2_iter += 4; ch3_iter += 4; ch4_iter += 4;
				}
				break;
			}
			case 1: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = m_factor[1] * (-*ch1_iter + *ch4_iter);
					out_iter += 4; ch1_iter += 4; ch4_iter += 4;
				}
				break;
			}
			case 2: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = m_factor[2] * (*ch1_iter - *ch2_iter - *ch3_iter + *ch4_iter);
					out_iter += 4; ch1_iter += 4; ch2_iter += 4; ch3_iter += 4; ch4_iter += 4;
				}
				break;
			}
			case 3: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = m_factor[1] * (-*ch2_iter + *ch3_iter);
					out_iter += 4; ch2_iter += 4; ch3_iter += 4;
				}
				break;

			}
			default: {
				throw;
			}
			}
		}
		else if (m_format == FRBL) {
			const float *ch1_iter = in, *ch2_iter = in + 1, *ch3_iter = in + 2, *ch4_iter = in + 3;
			float *out_iter = out + c;
			float m_factor[2];
			m_factor[0] = 0.5f;
			m_factor[1] = 1.0f;;

			switch (c) {
			case 0: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = m_factor[0] * (*ch1_iter + *ch2_iter + *ch3_iter + *ch4_iter);
					out_iter += 4; ch1_iter += 4; ch2_iter += 4; ch3_iter += 4; ch4_iter += 4;
				}
				break;
			}
			case 1: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = m_factor[1] * (-*ch2_iter + *ch4_iter);
					out_iter += 4; ch2_iter += 4; ch4_iter += 4;
				}
				break;
			}
			case 2: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = 0;
					out_iter += 4;
				}
				break;
			}
			case 3: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = m_factor[1] * (*ch1_iter - *ch3_iter);
					out_iter += 4; ch1_iter += 4; ch3_iter += 4;
				}
				break;
			}
			default: {
				throw;
			}
			}
		}
		else if (m_format == AMBISONIC_3MIC) {
			static float sqrt3 = sqrtf(3.f);
			const float *ch1_iter = in, *ch2_iter = in + 1, *ch3_iter = in + 2;
			float *out_iter = out + c;
			float m_factor[3];
			m_factor[0] = 2.0f / 3.0f;
			m_factor[1] = 2.0f / sqrt3;
			m_factor[2] = 4.0f / 3.0f;

			switch (c) {
			case 0: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = m_factor[0] * (*ch1_iter + *ch2_iter + *ch3_iter);
					out_iter += 4; ch1_iter += 3; ch2_iter += 3; ch3_iter += 3;
				}
				break;
			}
			case 1: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = m_factor[1] * (*ch2_iter - *ch3_iter);
					out_iter += 4; ch2_iter += 3; ch3_iter += 3;
				}
				break;
			}
			case 2: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = 0;
					out_iter += 4;
				}
				break;
			}
			case 3: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = m_factor[2] * *ch1_iter - m_factor[0] * (*ch2_iter + *ch3_iter);
					out_iter += 4; ch1_iter += 3; ch2_iter += 3; ch3_iter += 3;
				}
				break;
			}
			default: {
				throw;
			}
			}
		}
		else {
			throw;
		}
	}
	else if (m_profileid == BROADSIDE) {
		if (m_format == BROADSIDE_2MIC) {
			const float *ch1_iter = in, *ch2_iter = in + 1;
			float *out_iter = out + c;
			float data[FRAMELEN + ORDER + DELAY], *data_iter = data;
			float *mem_iter = m_mem;
			float *filter_iter;
			float m_factor[2];
			m_factor[0] = 1.0f / 2.0f;
			m_factor[1] = 1.0f;

			switch (c) {
			case 0: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = m_factor[0] * (*ch1_iter + *ch2_iter);
					out_iter += 4; ch1_iter += 2; ch2_iter += 2;;
				}
				break;
			}
			case 1: {
				for (int i = 0; i < ORDER + DELAY; i++) {
					*data_iter = *mem_iter;
					data_iter++; mem_iter++;
				}

				for (int i = 0; i < m_frame_len; i++) {
					*data_iter = m_factor[1] * (*ch1_iter - *ch2_iter);
					data_iter++; ch1_iter += 2; ch2_iter += 2;
				}

				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = 0.0f;
					data_iter = &data[i + ORDER];
					filter_iter = low_pass_filter_2mic;
					for (int j = 0; j <= ORDER; j++) {
						*out_iter += *filter_iter * *data_iter;
						filter_iter++; data_iter--;
					}
					out_iter += 4;
				}

				out_iter = out + c;
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter *= 0.8f;
					out_iter += 4;
				}

				mem_iter = m_mem;
				data_iter = &data[FRAMELEN];
				for (int i = 0; i < ORDER + DELAY; i++) {
					*mem_iter = *data_iter;
					mem_iter++; data_iter++;
				}
				break;
			}
			case 2: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = 0;
					out_iter += 4;
				}
				break;
			}
			case 3: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = 0;
					out_iter += 4;
				}
				break;
			}
			default: {
				break;
			}
			}
		}
		else {
			throw;
		}
	}
	else if (m_profileid == CIRCULAR) {
		if (m_format == CIRCULAR_3MIC) {
			const float *ch1_iter = in, *ch2_iter = in + 1, *ch3_iter = in + 2;
			float *out_iter = out + c;
			float data[FRAMELEN + ORDER], *data_iter = data;
			float *mem_iter = m_mem + c * ORDER;
			float *filter_iter;
			float m_factor[2];
			m_factor[0] = 1.0f / 3.0f;
			m_factor[1] = 1.0f;

			switch (c) {
			case 0: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = m_factor[0] * (*ch1_iter + *ch2_iter + *ch3_iter);
					out_iter += 4; ch1_iter += 3; ch2_iter += 3; ch3_iter += 3;
				}
				break;
			}
			case 1: {
				for (int i = 0; i < ORDER; i++) {
					*data_iter = *mem_iter;
					data_iter++; mem_iter++;
				}

				for (int i = 0; i < m_frame_len; i++) {
					*data_iter = m_factor[1] * (*ch2_iter - *ch3_iter);
					data_iter++; ch2_iter += 3; ch3_iter += 3;
				}

				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = 0.0f;
					data_iter = &data[i + ORDER];
					filter_iter = low_pass_filter_Y;
					for (int j = 0; j <= ORDER; j++) {
						*out_iter += *filter_iter * *data_iter;
						filter_iter++; data_iter--;
					}
					out_iter += 4;
				}

				out_iter = out + c;
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter *= 0.25f;
					out_iter += 4;
				}

				mem_iter = m_mem + c * ORDER;
				data_iter = &data[FRAMELEN];
				for (int i = 0; i < ORDER; i++) {
					*mem_iter = *data_iter;
					mem_iter++; data_iter++;
				}
				break;
			}
			case 2: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = 0;
					out_iter += 4;
				}
				break;
			}
			case 3: {
				for (int i = 0; i < ORDER; i++) {
					*data_iter = *mem_iter;
					data_iter++; mem_iter++;
				}

				for (int i = 0; i < m_frame_len; i++) {
					*data_iter = m_factor[1] * (*ch1_iter - *ch2_iter);
					data_iter++; ch1_iter += 3; ch2_iter += 3;
				}

				for (int i = 0; i < m_frame_len; i++) {
					data_iter = &data[i + ORDER];
					filter_iter = low_pass_filter_X;
					for (int j = 0; j <= ORDER; j++) {
						*out_iter += *filter_iter * *data_iter;
						filter_iter++; data_iter--;
					}
					out_iter += 4;
				}


				mem_iter = m_mem + c * ORDER;
				data_iter = &data[FRAMELEN];
				for (int i = 0; i < ORDER; i++) {
					*mem_iter = *data_iter;
					mem_iter++; data_iter++;
				}
				break;
			}
			default: {
				throw;
			}
			}
		}
		else if (m_format == CIRCULAR_4MIC) {
			const float *ch1_iter = in, *ch2_iter = in + 1, *ch3_iter = in + 2, *ch4_iter = in + 3;
			float data[FRAMELEN + ORDER], *data_iter = data;;
			float *out_iter = out + c;
			float *mem_iter = m_mem + c * ORDER;
			float *filter_iter;
			float m_factor[2];
			m_factor[0] = 0.25f;
			m_factor[1] = 0.5f;

			switch (c) {
			case 0: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = m_factor[0] * (*ch1_iter + *ch2_iter + *ch3_iter + *ch4_iter);
					out_iter += 4; ch1_iter += 4; ch2_iter += 4; ch3_iter += 4; ch4_iter += 4;
				}
				break;
			}
			case 1: {
				for (int i = 0; i < ORDER; i++) {
					*data_iter = *mem_iter;
					data_iter++; mem_iter++;
				}

				for (int i = 0; i < m_frame_len; i++) {
					*data_iter = m_factor[1] * (-*ch1_iter + *ch2_iter + *ch3_iter - *ch4_iter);
					data_iter++; ch1_iter += 4; ch2_iter += 4; ch3_iter += 4; ch4_iter += 4;
				}

				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = 0.0f;
					data_iter = &data[i + ORDER];
					filter_iter = low_pass_filter_Y;
					for (int j = 0; j <= ORDER; j++) {
						*out_iter += *filter_iter * *data_iter;
						filter_iter++; data_iter--;
					}
					out_iter += 4;
				}

				mem_iter = m_mem + c * ORDER;
				data_iter = &data[FRAMELEN];
				for (int i = 0; i < ORDER; i++) {
					*mem_iter = *data_iter;
					mem_iter++; data_iter++;
				}

				break;
			}
			case 2: {
				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = 0;
					out_iter += 4;
				}
				break;
			}
			case 3: {
				for (int i = 0; i < ORDER; i++) {
					*data_iter = *mem_iter;
					data_iter++; mem_iter++;
				}

				for (int i = 0; i < m_frame_len; i++) {
					*data_iter = m_factor[1] * (*ch1_iter + *ch2_iter - *ch3_iter - *ch4_iter);
					data_iter++; ch1_iter += 4; ch2_iter += 4; ch3_iter += 4; ch4_iter += 4;
				}

				for (int i = 0; i < m_frame_len; i++) {
					*out_iter = 0.0f;
					data_iter = &data[i + ORDER];
					filter_iter = low_pass_filter_X;
					for (int j = 0; j <= ORDER; j++) {
						*out_iter += *filter_iter * *data_iter;
						filter_iter++; data_iter--;
					}
					out_iter += 4;
				}

				mem_iter = m_mem + c * ORDER;
				data_iter = &data[FRAMELEN];
				for (int i = 0; i < ORDER; i++) {
					*mem_iter = *data_iter;
					mem_iter++; data_iter++;
				}

				break;
			}
			default: {
				throw;
			}
			}
		}
		else {
			throw;
		}
	}
}