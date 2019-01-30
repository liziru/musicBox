#include "hxvxfromxxxx.h"
#include <stdio.h>
#include <math.h>

struct anglecs_s
{
	float c;
	float s;
};
typedef anglecs_s anglecs;

CHXVXFromXXXX::CHXVXFromXXXX(int frame_length, int object_num) : m_param(NULL)
															   , m_fac(NULL)
															   , m_first(true)
{
	m_frame_length = frame_length;
	m_object_num = object_num;
	m_fac = new float[frame_length];
	float scale = 1.f / (float)frame_length;
	for (int i = 0; i < frame_length; i++) {
		m_fac[i] = (float)i * scale;
	}
}

CHXVXFromXXXX::~CHXVXFromXXXX()
{
	if (NULL != m_fac) {
		delete [] m_fac;
		m_fac = NULL;
	}
}

void CHXVXFromXXXX::calc_rot_mat(const HeadRotation *rot)
{
	anglecs x, y, z;
	x.c = cos(rot->bank); y.c = cos(rot->pitch); z.c = cos(rot->heading);
	x.s = sin(rot->bank); y.s = sin(rot->pitch); z.s = sin(rot->heading);

	float zm[3][3] = { z.c, z.s,  0.f, -z.s, z.c, 0.f, 0.f,  0.f, 1.f };
	float ym[3][3] = { y.c, 0.f, -y.s,  0.f, 1.f, 0.f, y.s,  0.f, y.c };
	float xm[3][3] = { 1.f, 0.f,  0.f,  0.f, x.c, x.s, 0.f, -x.s, x.c }; 

	float temp[3][3];
	matmul3x3(ym, zm, temp);
	matmul3x3(xm, temp, m_rot_mat);
}

void CHXVXFromXXXX::matmul3x3(const float (*l)[3], const float (*r)[3], float (*res)[3])
{
	res[0][0] = l[0][0] * r[0][0] + l[0][1] * r[1][0] + l[0][2] * r[2][0];
	res[0][1] = l[0][0] * r[0][1] + l[0][1] * r[1][1] + l[0][2] * r[2][1];
	res[0][2] = l[0][0] * r[0][2] + l[0][1] * r[1][2] + l[0][2] * r[2][2];
	res[1][0] = l[1][0] * r[0][0] + l[1][1] * r[1][0] + l[1][2] * r[2][0];
	res[1][1] = l[1][0] * r[0][1] + l[1][1] * r[1][1] + l[1][2] * r[2][1];
	res[1][2] = l[1][0] * r[0][2] + l[1][1] * r[1][2] + l[1][2] * r[2][2];
	res[2][0] = l[2][0] * r[0][0] + l[2][1] * r[1][0] + l[2][2] * r[2][0];
	res[2][1] = l[2][0] * r[0][1] + l[2][1] * r[1][1] + l[2][2] * r[2][1];
	res[2][2] = l[2][0] * r[0][2] + l[2][1] * r[1][2] + l[2][2] * r[2][2];
}
