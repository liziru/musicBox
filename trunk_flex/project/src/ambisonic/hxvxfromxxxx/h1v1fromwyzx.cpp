#include "h1v1fromwyzx.h"
#include <string.h>

struct h1v1fromwyzxparam_s
{
	const float *in;
	float *out;
	size_t op;
	const float *fac;
	int length;          // frame length
	float curr[3][3];    // current 3x3 rotation matrix
	float prev[3][3];    // previous 3x3 rotation matrix
};
typedef h1v1fromwyzxparam_s h1v1fromwyzxparam;

typedef void(*h1v1_ambisonic_from_wyzx)(const h1v1fromwyzxparam*);

static inline void h1v1_ambisonic_from_wyzx_0(const h1v1fromwyzxparam *param)
{
	const float *in_iter = param->in;
	float *out_iter = param->out;
	for (int k = 0; k < param->length; k++) {
		*out_iter =  *in_iter;
		in_iter += 4; out_iter++;
	}
}

static inline void h1v1_ambisonic_from_wyzx_1(const h1v1fromwyzxparam *param)
{
	const float *in_iter_1 = param->in + 1; /* y. */
	const float *in_iter_2 = param->in + 2; /* z. */
	const float *in_iter_3 = param->in + 3; /* x. */
	float *out_iter = (float*)((char*)param->out + param->op);
	const float *fac_iter = param->fac;
	for (int k = 0; k < param->length; k++) {
		*out_iter = *in_iter_3 * (param->curr[0][0] * *fac_iter + param->prev[0][0] * (1.f - *fac_iter))
		          + *in_iter_1 * (param->curr[0][1] * *fac_iter + param->prev[0][1] * (1.f - *fac_iter))
		          + *in_iter_2 * (param->curr[0][2] * *fac_iter + param->prev[0][2] * (1.f - *fac_iter));
		in_iter_1 += 4; in_iter_2 += 4; in_iter_3 += 4; out_iter++; fac_iter++;
	}
}

static inline void h1v1_ambisonic_from_wyzx_2(const h1v1fromwyzxparam *param)
{
	const float *in_iter_1 = param->in + 1;
	const float *in_iter_2 = param->in + 2;
	const float *in_iter_3 = param->in + 3;
	float *out_iter = (float*)((char*)param->out + param->op * 2);
	const float *fac_iter = param->fac;
	for (int k = 0; k < param->length; k++) {
		*out_iter = *in_iter_3 * (param->curr[1][0] * *fac_iter + param->prev[1][0] * (1.f - *fac_iter))
		          + *in_iter_1 * (param->curr[1][1] * *fac_iter + param->prev[1][1] * (1.f - *fac_iter))
		          + *in_iter_2 * (param->curr[1][2] * *fac_iter + param->prev[1][2] * (1.f - *fac_iter));
		in_iter_1 += 4; in_iter_2 += 4; in_iter_3 += 4; out_iter++; fac_iter++;
	}
}

static inline void h1v1_ambisonic_from_wyzx_3(const h1v1fromwyzxparam *param)
{
	const float *in_iter_1 = param->in + 1;
	const float *in_iter_2 = param->in + 2;
	const float *in_iter_3 = param->in + 3;
	float *out_iter = (float*)((char*)param->out + param->op * 3);
	const float *fac_iter = param->fac;
	for (int k = 0; k < param->length; k++) {
		*out_iter = *in_iter_3 * (param->curr[2][0] * *fac_iter + param->prev[2][0] * (1.f - *fac_iter))
		          + *in_iter_1 * (param->curr[2][1] * *fac_iter + param->prev[2][1] * (1.f - *fac_iter))
		          + *in_iter_2 * (param->curr[2][2] * *fac_iter + param->prev[2][2] * (1.f - *fac_iter));
		in_iter_1 += 4; in_iter_2 += 4; in_iter_3 += 4; out_iter++; fac_iter++;
	}
}

static const h1v1_ambisonic_from_wyzx h1v1_ambisonic_from_wyzx_list[4] = 
{
	h1v1_ambisonic_from_wyzx_0,  //w
	h1v1_ambisonic_from_wyzx_1,  //y
	h1v1_ambisonic_from_wyzx_2,  //z
	h1v1_ambisonic_from_wyzx_3   //x
};

CH1V1FromWYZX::CH1V1FromWYZX(int frame_length, int object_num) : CHXVXFromXXXX(frame_length, object_num)
{
	m_param = new h1v1fromwyzxparam;
	((h1v1fromwyzxparam*)m_param)->length = frame_length;
	((h1v1fromwyzxparam*)m_param)->fac = m_fac;
}

CH1V1FromWYZX::~CH1V1FromWYZX()
{
	if (NULL != m_param) {
		delete (h1v1fromwyzxparam*)m_param;
		m_param = NULL;
	}
}

void CH1V1FromWYZX::run(int index)
{
	h1v1_ambisonic_from_wyzx_list[index]((const h1v1fromwyzxparam*)m_param);
}

void CH1V1FromWYZX::prev(const HeadRotation *rot, const float *in, float *out, size_t op, const MetaData *meta)
{
	calc_rot_mat(rot);
	((h1v1fromwyzxparam*)m_param)->curr[0][0] = m_rot_mat[0][0];
	((h1v1fromwyzxparam*)m_param)->curr[0][1] = m_rot_mat[0][1];
	((h1v1fromwyzxparam*)m_param)->curr[0][2] = m_rot_mat[0][2];
	((h1v1fromwyzxparam*)m_param)->curr[1][0] = m_rot_mat[1][0];
	((h1v1fromwyzxparam*)m_param)->curr[1][1] = m_rot_mat[1][1];
	((h1v1fromwyzxparam*)m_param)->curr[1][2] = m_rot_mat[1][2];
	((h1v1fromwyzxparam*)m_param)->curr[2][0] = m_rot_mat[2][0];
	((h1v1fromwyzxparam*)m_param)->curr[2][1] = m_rot_mat[2][1];
	((h1v1fromwyzxparam*)m_param)->curr[2][2] = m_rot_mat[2][2];
	((h1v1fromwyzxparam*)m_param)->in = in;
	((h1v1fromwyzxparam*)m_param)->out = out;
	((h1v1fromwyzxparam*)m_param)->op = op;
	if (m_first) {
		post();
		m_first = false;
	}
}

void CH1V1FromWYZX::post()
{
	((h1v1fromwyzxparam*)m_param)->prev[0][0] = ((h1v1fromwyzxparam*)m_param)->curr[0][0];
	((h1v1fromwyzxparam*)m_param)->prev[0][1] = ((h1v1fromwyzxparam*)m_param)->curr[0][1];
	((h1v1fromwyzxparam*)m_param)->prev[0][2] = ((h1v1fromwyzxparam*)m_param)->curr[0][2];
	((h1v1fromwyzxparam*)m_param)->prev[1][0] = ((h1v1fromwyzxparam*)m_param)->curr[1][0];
	((h1v1fromwyzxparam*)m_param)->prev[1][1] = ((h1v1fromwyzxparam*)m_param)->curr[1][1];
	((h1v1fromwyzxparam*)m_param)->prev[1][2] = ((h1v1fromwyzxparam*)m_param)->curr[1][2];
	((h1v1fromwyzxparam*)m_param)->prev[2][0] = ((h1v1fromwyzxparam*)m_param)->curr[2][0];
	((h1v1fromwyzxparam*)m_param)->prev[2][1] = ((h1v1fromwyzxparam*)m_param)->curr[2][1];
	((h1v1fromwyzxparam*)m_param)->prev[2][2] = ((h1v1fromwyzxparam*)m_param)->curr[2][2];
}
