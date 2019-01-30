#ifndef __HXVX_FROM_XXXX_H__
#define __HXVX_FROM_XXXX_H__

#include "../../core/def.h"
#include <stdio.h>
#include <math.h>

class CHXVXFromXXXX
{
public:
	CHXVXFromXXXX(int frame_length, int object_num);
	virtual ~CHXVXFromXXXX();
	virtual void prev(const HeadRotation *rot, const float *in, float *out, size_t op, const MetaData *meta) = 0;
	virtual void post() = 0;
	virtual void run(int index) = 0;

protected:
	int m_frame_length;
	int m_object_num;
	float *m_fac;
	bool m_first;
	void *m_param;
	float m_rot_mat[3][3];

	void calc_rot_mat(const HeadRotation *rot);
private:
	void matmul3x3(const float (*l)[3], const float (*r)[3], float (*res)[3]);
};

#endif /*__HXVX_FROM_XXXX_H__*/
