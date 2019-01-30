#ifndef DYNAMIC_RANGE_CONTROL_H
#define DYNAMIC_RANGE_CONTROL_H

class CDynamicRangeControl
{
public:

    CDynamicRangeControl(int frame_length, int channelN, float maxV);
    ~CDynamicRangeControl();

    void process(const float *input, float *output);
	void set(float maxV);

private:

	int m_frame_len;               // sample number of each audio frame
	int m_channelN;
	float m_th;
	float m_sg;
};

#endif