#include "binaural.h"
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

CBinaural::CBinaural(int frame_length)
{
	m_frame_len = frame_length;		
}

CBinaural::~CBinaural()
{

}

void CBinaural::process(float azi, float elv, const float *input, float *output)
{
	float pi2 = M_PI * 0.5f;
	azi = -azi;
	float gain[4] = 
	{
		(float)cos(azi), 
		(float)cos(azi - pi2), 
		(float)cos(azi - pi2 * 2.f), 
		(float)cos(azi - pi2 * 3.f)
	};
	
	if (gain[0] < 0) gain[0] = 0;
	if (gain[1] < 0) gain[1] = 0;
	if (gain[2] < 0) gain[2] = 0;
	if (gain[3] < 0) gain[3] = 0;

	int n = 0;
	int m = 0;
	for (int i = 0; i < m_frame_len; i++) {
		output[n++] = input[m++] * gain[0];
		output[n--] = input[m++] * gain[0];
		output[n++] += input[m++] * gain[1];
		output[n--] += input[m++] * gain[1];
		output[n++] += input[m++] * gain[2];
		output[n--] += input[m++] * gain[2];
		output[n++] += input[m++] * gain[3];
		output[n++] += input[m++] * gain[3];
	}
}
