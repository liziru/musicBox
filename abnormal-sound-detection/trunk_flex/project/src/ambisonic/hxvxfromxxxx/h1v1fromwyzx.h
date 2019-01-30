#ifndef __H1V1_FROM_WYZX_H__
#define __H1V1_FROM_WYZX_H__

#include "hxvxfromxxxx.h"

class CH1V1FromWYZX : public CHXVXFromXXXX
{
public:
	CH1V1FromWYZX(int frame_length, int object_num);
	~CH1V1FromWYZX();
	void prev(const HeadRotation *rot, const float *in, float *out, size_t op, const MetaData *meta);
	void post();
	void run(int index);
};


#endif /*__H1V1_FROM_WYZX_H__*/