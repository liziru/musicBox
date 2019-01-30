#ifndef __SN3D_H__
#define __SN3D_H__

#define ORDER      20
#define DELAY      8   // it is needed for omni2 mode
#define N          4
#define FRAMELEN   512
#define PI         3.1415926

class CSn3d
{
public:
	CSn3d(int profileid, int frame_length);   
    virtual ~CSn3d();	
	void process(const float *in, float *out);
	void set(int format);

private:
	void to_sn3d(const float *in, float *out,int c);

private:
	enum MicArrayType_e
	{
		AMBISONIC,
		BROADSIDE,
		CIRCULAR,
		STEREO,
		FIVE,
		QB
	};
	typedef MicArrayType_e MicArrayType;

	enum AmbisonicFormat_e
	{
		AMBISONIC_3MIC,
		WXY,
		A_FORMAT,
		WXYZ_FUMA,
		WYZX_SN3D,
		FRBL
	};
	typedef AmbisonicFormat_e AmbisonicFormat;

	enum BroadsideFormat_e
	{
		BROADSIDE_2MIC = 0x02,
		BROADSIDE_3MIC = 0x03,
		BROADSIDE_4MIC = 0x04
	};
	typedef BroadsideFormat_e BroadsideFormat;
	
	enum CircularFormat_e
	{
		CIRCULAR_3MIC = 0x03,
		CIRCULAR_4MIC = 0x04
	};
	typedef CircularFormat_e CircularFormat;

private:
	int m_profileid;
	int m_format;
	int m_frame_len;
	int m_channels;
	float *m_mem;
};
#endif /*__SN3D_H__*/