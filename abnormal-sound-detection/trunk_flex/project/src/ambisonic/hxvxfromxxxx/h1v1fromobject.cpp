#include "h1v1fromobject.h"
#include <string.h>

struct ambweight4_s
{
	float w;
	float x;
	float y;
	float z;
};
typedef ambweight4_s ambweight4;

struct h1v1fromobjectparam_s
{
	const float *in;
	float *out;
	size_t op;
	const float *fac;
	int length;
	int num;
	const ambweight4 *curr;
	const ambweight4 *prev;
};
typedef h1v1fromobjectparam_s h1v1fromobjectparam;

static inline void h1v1_ambisonic_from_object(const h1v1fromobjectparam *param, int index)
{
	const float *in_iter = param->in;
	float *out_iter = (float*)((char*)param->out + param->op * index);
	const float *fac_iter = param->fac;
	for (int k = 0; k < param->length; k++) {
		*out_iter = 0.f;
		const ambweight4 *curr_iter = param->curr;
		const ambweight4 *prev_iter = param->prev;
		for (int c = 0; c < param->num; c++) {
			*out_iter += (*((const float *)curr_iter + index) * *fac_iter + *((const float *)prev_iter + index) * (1.f - *fac_iter)) * *(in_iter + c);
			curr_iter++; prev_iter++;
		}
		in_iter += param->num; out_iter++; fac_iter++;
	}
}

CH1V1FromObject::CH1V1FromObject(int frame_length, int object_num) : CHXVXFromXXXX(frame_length, object_num)
																   , m_curr(NULL)
																   , m_prev(NULL)
{
	m_param = new h1v1fromobjectparam;
	((h1v1fromobjectparam*)m_param)->length = frame_length;
	((h1v1fromobjectparam*)m_param)->num = object_num;
	((h1v1fromobjectparam*)m_param)->fac = m_fac;
	m_curr = new ambweight4[object_num];
	m_prev = new ambweight4[object_num];
	((h1v1fromobjectparam*)m_param)->curr = (const ambweight4*)m_curr;
	((h1v1fromobjectparam*)m_param)->prev = (const ambweight4*)m_prev;
}

CH1V1FromObject::~CH1V1FromObject()
{
	if (NULL != m_curr) {
		delete [] (ambweight4*)m_curr;
		m_curr = NULL;
	}

	if (NULL != m_prev) {
		delete [] (ambweight4*)m_prev;
		m_prev = NULL;
	}

	if (NULL != m_param) {
		delete (h1v1fromobjectparam*)m_param;
		m_param = NULL;
	}
}

void CH1V1FromObject::run(int index)
{
	h1v1_ambisonic_from_object((const h1v1fromobjectparam*)m_param, index);
}

void CH1V1FromObject::prev(const HeadRotation *rot, const float *in, float *out, size_t op, const MetaData *meta)
{
	calc_rot_mat(rot);
	static const float _1_div_sqrt2 = 1.f;

	const MetaData *meta_iter = meta;
	ambweight4 *curr_iter = (ambweight4*)m_curr;
	for (int i = 0; i < m_object_num; i++) {
		float x = cos(meta_iter->azi) * cos(meta_iter->elv);
		float y = sin(meta_iter->azi) * cos(meta_iter->elv);
		float z = sin(meta_iter->elv);
		float v[3] =
		{
			m_rot_mat[0][0] * x + m_rot_mat[0][1] * y + m_rot_mat[0][2] * z,
			m_rot_mat[1][0] * x + m_rot_mat[1][1] * y + m_rot_mat[1][2] * z,
			m_rot_mat[2][0] * x + m_rot_mat[2][1] * y + m_rot_mat[2][2] * z
		};
		MetaData meta;
		meta.elv = asinf(v[2]);
		meta.azi = atan2f(v[1], v[0]);
		if (meta_iter->dist) {
			meta.dist = 1.f / meta_iter->dist;
		}
		else {
			meta.dist = 0.f;
		}
		meta_iter++;

		curr_iter->w = _1_div_sqrt2 * meta.dist;
		curr_iter->x = cos(meta.azi) * cos(meta.elv) * meta.dist;
		curr_iter->y = sin(meta.azi) * cos(meta.elv) * meta.dist;
		curr_iter->z = sin(meta.elv) * meta.dist;
		curr_iter++;
	}
	((h1v1fromobjectparam*)m_param)->in = in;
	((h1v1fromobjectparam*)m_param)->out = out;
	((h1v1fromobjectparam*)m_param)->op = op;
	if (m_first) {
		post();
		m_first = false;
	}
}

void CH1V1FromObject::post()
{
	memcpy(m_prev, m_curr, sizeof(ambweight4) * m_object_num);
}
