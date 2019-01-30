#ifndef __H1V1_FROM_OBJECT_H__
#define __H1V1_FROM_OBJECT_H__

#include "hxvxfromxxxx.h"

class CH1V1FromObject : public CHXVXFromXXXX
{
public:
	CH1V1FromObject(int frame_length, int object_num);
	~CH1V1FromObject();
	void prev(const HeadRotation *rot, const float *in, float *out, size_t op, const MetaData *meta);
	void post();
	void run(int index);

private:
	void *m_curr;
	void *m_prev;
};


#endif /*__H1V1_FROM_OBJECT_H__*/