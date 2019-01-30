#ifndef BINAURAL_INTERPOLATION_H
#define BINAURAL_INTERPOLATION_H



class CBinaural
{
public:
    CBinaural(int frame_length);
    ~CBinaural();

    void process(float azi, float elv, const float *binauralInput, float *binauralOutput);

private:
	int m_frame_len;               // sample number of each audio frame

};

#endif