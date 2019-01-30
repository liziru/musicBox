//***************************************************************************
//
//           Macro and global variables definition for Residual echo suppresstion and comfort noise.
//         
//***************************************************************************
#ifndef RES_DEFINE_H
#define RES_DEFINE_H

#include "commonDefine.h"
 



typedef struct{
float *inBuf;
float *dfBuf;
float *dfBufPow;
float *yfBuf;
float *yfBufPow;
float *outBuf;
float *efBufPow;
float *efBufPowPrev;
float *efBufPowLocal;   
float *speechVar;        
float *noiseVar;         
float *reAverage;     
float *binGain; 
float *binGain_smooth; 
float *binBgNoiseGain;        
float *m_speech_absence_possibility;

float avgefPow;
float avgyfPow;
float avgsnrglobal;
float noiseTrackStep;
float m_max_sup;
float m_max_nlp_sup;
float speechAbsencePossibilityAvg;
float frameCnt;
}ResGlobalData;


#endif