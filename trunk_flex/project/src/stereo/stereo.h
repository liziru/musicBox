#ifndef __STEREO_H__
#define __STEREO_H__

class CStereo
{
public:
	CStereo(int frameLength, int channelN, int sRate);
    virtual ~CStereo();
	void process(const float *in, float *out);
	void set(int placementMode, int cameraMode);


private:
	enum PlacementMode_e
	{
		VERTICAL_UP      = 0,
		HORIZONTAL_LEFT  = 1,
		VERTICAL_DOWN    = 2,
		HORIZONTAL_RIGHT = 3
	};
	typedef PlacementMode_e PlacementMode;


	enum CameraMode_e
	{
		FRONT  = 0,
		BACK   = 1
	};
	typedef CameraMode_e CameraMode;

private:
	int m_placementMode;
	int m_cameraMode;
	int m_frame_len;
	int m_channels;
};
#endif /*__STEREO_H__*/