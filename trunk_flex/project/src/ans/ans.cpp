#include "ans.h"
#include "../utils/cond.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

//#define DEBUG_VAD
#ifdef DEBUG_VAD
FILE *debugFile;
short vadflag[512];
#endif

#define AS (0.5f)
#define ZC (0.5f)
#define SMOOTH_SNR (0.98)
#define VARMIN (1e-7f)
#define SMALLV (1e-9f)
#define SMOOTHA (0.9f)
#define SNRMAX (40.f)
#define SNRMIN (0.001f)
#define MAXSUP1 (-3.f)
#define MAXSUP2 (-90.f)
#define DEV_DB_THD (6)
#define DEV_NOISE_DB_THD (3)
#define NOISE_UPDATE_CNT (60)  //1000ms
#define FLATNESS_SMOOTH (0.5)

#define INIT_TIME (200)   //200ms
#define STABLE_TIME (1000)


#define MMSE_LSA_TABLE_LEN (1000)
static float mmse_lsa_table[MMSE_LSA_TABLE_LEN] = {
	7.530525, 5.351377, 4.391010, 3.821457, 3.434768, 3.150791, 2.931221, 2.755146, 2.610058, 2.487957, 2.383460, 2.292793, 2.213223, 2.142713, 2.079715, 2.023024, 1.971689, 1.924946, 1.882177, 1.842872, 1.806608, 1.773030, 1.741839, 1.712780, 1.685633, 1.660210, 1.636347, 1.613901, 1.592745, 1.572770, 1.553877, 1.535978, 1.518998, 1.502864, 1.487516, 1.472897, 1.458955, 1.445646, 1.432926, 1.420757, 1.409106, 1.397939, 1.387227, 1.376944, 1.367064, 1.357566, 1.348427, 1.339628, 1.331152, 1.322980, 1.315098, 1.307491, 1.300145, 1.293048, 1.286187, 1.279552, 1.273132, 1.266917, 1.260899, 1.255068, 1.249416, 1.243936, 1.238621, 1.233463, 1.228456, 1.223595, 1.218873, 1.214284, 1.209824, 1.205488, 1.201271, 1.197168, 1.193176, 1.189290, 1.185507, 1.181822, 1.178233, 1.174736, 1.171328, 1.168006, 1.164767, 1.161608, 1.158527, 1.155521, 1.152588, 1.149725, 1.146930, 1.144201, 1.141536, 1.138933, 1.136390, 1.133906, 1.131478, 1.129105, 1.126785, 1.124517, 1.122300, 1.120131, 1.118010, 1.115934, 1.113904, 1.111917, 1.109973, 1.108069, 1.106206, 1.104382, 1.102596, 1.100847, 1.099135, 1.097457, 1.095813, 1.094203, 1.092625, 1.091079, 1.089564, 1.088079, 1.086623, 1.085196, 1.083797, 1.082425, 1.081080, 1.079761, 1.078467, 1.077198, 1.075953, 1.074732, 1.073533, 1.072357, 1.071204, 1.070072, 1.068960, 1.067870, 1.066799, 1.065748, 1.064716, 1.063703, 1.062709, 1.061732, 1.060773, 1.059831, 1.058906, 1.057998, 1.057105, 1.056228, 1.055367, 1.054521, 1.053690, 1.052873, 1.052070, 1.051281, 1.050506, 1.049744, 1.048996, 1.048260, 1.047536, 1.046825, 1.046126, 1.045439, 1.044763, 1.044099, 1.043446, 1.042803, 1.042172, 1.041551, 1.040940, 1.040339, 1.039748, 1.039167, 1.038595, 1.038033, 1.037480, 1.036935, 1.036400, 1.035873, 1.035355, 1.034845, 1.034343, 1.033850, 1.033364, 1.032886, 1.032415, 1.031952, 1.031496, 1.031048, 1.030607, 1.030172, 1.029744, 1.029323, 1.028909, 1.028501, 1.028099, 1.027704, 1.027314, 1.026931, 1.026554, 1.026182, 1.025816, 1.025456, 1.025101, 1.024752, 1.024408, 1.024069, 1.023735, 1.023406, 1.023083, 1.022764, 1.022450, 1.022141, 1.021836, 1.021536, 1.021240, 1.020949, 1.020662, 1.020379, 1.020101, 1.019827, 1.019557, 1.019290, 1.019028, 1.018770, 1.018515, 1.018264, 1.018017, 1.017773, 1.017533, 1.017296, 1.017063, 1.016833, 1.016607, 1.016384, 1.016164, 1.015947, 1.015734, 1.015523, 1.015316, 1.015111, 1.014909, 1.014711, 1.014515, 1.014322, 1.014131, 1.013944, 1.013759, 1.013576, 1.013397, 1.013220, 1.013045, 1.012873, 1.012703, 1.012535, 1.012370, 1.012208, 1.012047, 1.011889, 1.011733, 1.011579, 1.011427, 1.011278, 1.011130, 1.010985, 1.010842, 1.010700, 1.010561, 1.010423, 1.010288, 1.010154, 1.010022, 1.009892, 1.009764, 1.009637, 1.009512, 1.009389, 1.009268, 1.009148, 1.009030, 1.008914, 1.008799, 1.008685, 1.008574, 1.008463, 1.008354, 1.008247, 1.008141, 1.008037, 1.007934, 1.007832, 1.007732, 1.007633, 1.007536, 1.007440, 1.007345, 1.007251, 1.007159, 1.007067, 1.006977, 1.006889, 1.006801, 1.006715, 1.006630, 1.006546, 1.006463, 1.006381, 1.006300, 1.006220, 1.006142, 1.006064, 1.005987, 1.005912, 1.005837, 1.005764, 1.005691, 1.005620, 1.005549, 1.005479, 1.005410, 1.005342, 1.005275, 1.005209, 1.005144, 1.005079, 1.005016, 1.004953, 1.004891, 1.004830, 1.004770, 1.004710, 1.004651, 1.004593, 1.004536, 1.004480, 1.004424, 1.004369, 1.004314, 1.004261, 1.004208, 1.004156, 1.004104, 1.004053, 1.004003, 1.003953, 1.003904, 1.003856, 1.003808, 1.003761, 1.003715, 1.003669, 1.003624, 1.003579, 1.003535, 1.003491, 1.003448, 1.003406, 1.003364, 1.003322, 1.003282, 1.003241, 1.003201, 1.003162, 1.003123, 1.003085, 1.003047, 1.003010, 1.002973, 1.002937, 1.002901, 1.002865, 1.002830, 1.002795, 1.002761, 1.002728, 1.002694, 1.002661, 1.002629, 1.002597, 1.002565, 1.002534, 1.002503, 1.002473, 1.002443, 1.002413, 1.002384, 1.002355, 1.002326, 1.002298, 1.002270, 1.002243, 1.002215, 1.002189, 1.002162, 1.002136, 1.002110, 1.002084, 1.002059, 1.002034, 1.002010, 1.001986, 1.001962, 1.001938, 1.001915, 1.001891, 1.001869, 1.001846, 1.001824, 1.001802, 1.001780, 1.001759, 1.001738, 1.001717, 1.001696, 1.001676, 1.001656, 1.001636, 1.001616, 1.001597, 1.001578, 1.001559, 1.001540, 1.001522, 1.001503, 1.001485, 1.001468, 1.001450, 1.001433, 1.001416, 1.001399, 1.001382, 1.001366, 1.001349, 1.001333, 1.001317, 1.001302, 1.001286, 1.001271, 1.001256, 1.001241, 1.001226, 1.001211, 1.001197, 1.001183, 1.001169, 1.001155, 1.001141, 1.001128, 1.001114, 1.001101, 1.001088, 1.001075, 1.001062, 1.001050, 1.001037, 1.001025, 1.001013, 1.001001, 1.000989, 1.000977, 1.000966, 1.000954, 1.000943, 1.000932, 1.000921, 1.000910, 1.000899, 1.000889, 1.000878, 1.000868, 1.000858, 1.000848, 1.000838, 1.000828, 1.000818, 1.000808, 1.000799, 1.000789, 1.000780, 1.000771, 1.000762, 1.000753, 1.000744, 1.000735, 1.000727, 1.000718, 1.000710, 1.000701, 1.000693, 1.000685, 1.000677, 1.000669, 1.000661, 1.000654, 1.000646, 1.000638, 1.000631, 1.000624, 1.000616, 1.000609, 1.000602, 1.000595, 1.000588, 1.000581, 1.000574, 1.000568, 1.000561, 1.000554, 1.000548, 1.000542, 1.000535, 1.000529, 1.000523, 1.000517, 1.000511, 1.000505, 1.000499, 1.000493, 1.000487, 1.000482, 1.000476, 1.000471, 1.000465, 1.000460, 1.000454, 1.000449, 1.000444, 1.000439, 1.000434, 1.000429, 1.000424, 1.000419, 1.000414, 1.000409, 1.000404, 1.000400, 1.000395, 1.000391, 1.000386, 1.000382, 1.000377, 1.000373, 1.000368, 1.000364, 1.000360, 1.000356, 1.000352, 1.000348, 1.000344, 1.000340, 1.000336, 1.000332, 1.000328, 1.000324, 1.000321, 1.000317, 1.000313, 1.000310, 1.000306, 1.000302, 1.000299, 1.000296, 1.000292, 1.000289, 1.000285, 1.000282, 1.000279, 1.000276, 1.000273, 1.000269, 1.000266, 1.000263, 1.000260, 1.000257, 1.000254, 1.000251, 1.000249, 1.000246, 1.000243, 1.000240, 1.000237, 1.000235, 1.000232, 1.000229, 1.000227, 1.000224, 1.000221, 1.000219, 1.000216, 1.000214, 1.000211, 1.000209, 1.000207, 1.000204, 1.000202, 1.000200, 1.000197, 1.000195, 1.000193, 1.000191, 1.000189, 1.000186, 1.000184, 1.000182, 1.000180, 1.000178, 1.000176, 1.000174, 1.000172, 1.000170, 1.000168, 1.000166, 1.000164, 1.000162, 1.000161, 1.000159, 1.000157, 1.000155, 1.000153, 1.000152, 1.000150, 1.000148, 1.000147, 1.000145, 1.000143, 1.000142, 1.000140, 1.000138, 1.000137, 1.000135, 1.000134, 1.000132, 1.000131, 1.000129, 1.000128, 1.000126, 1.000125, 1.000123, 1.000122, 1.000121, 1.000119, 1.000118, 1.000117, 1.000115, 1.000114, 1.000113, 1.000111, 1.000110, 1.000109, 1.000108, 1.000106, 1.000105, 1.000104, 1.000103, 1.000102, 1.000101, 1.000099, 1.000098, 1.000097, 1.000096, 1.000095, 1.000094, 1.000093, 1.000092, 1.000091, 1.000090, 1.000089, 1.000088, 1.000087, 1.000086, 1.000085, 1.000084, 1.000083, 1.000082, 1.000081, 1.000080, 1.000079, 1.000078, 1.000077, 1.000077, 1.000076, 1.000075, 1.000074, 1.000073, 1.000072, 1.000072, 1.000071, 1.000070, 1.000069, 1.000068, 1.000068, 1.000067, 1.000066, 1.000065, 1.000065, 1.000064, 1.000063, 1.000062, 1.000062, 1.000061, 1.000060, 1.000060, 1.000059, 1.000058, 1.000058, 1.000057, 1.000056, 1.000056, 1.000055, 1.000055, 1.000054, 1.000053, 1.000053, 1.000052, 1.000052, 1.000051, 1.000050, 1.000050, 1.000049, 1.000049, 1.000048, 1.000048, 1.000047, 1.000047, 1.000046, 1.000046, 1.000045, 1.000045, 1.000044, 1.000044, 1.000043, 1.000043, 1.000042, 1.000042, 1.000041, 1.000041, 1.000040, 1.000040, 1.000039, 1.000039, 1.000039, 1.000038, 1.000038, 1.000037, 1.000037, 1.000036, 1.000036, 1.000036, 1.000035, 1.000035, 1.000034, 1.000034, 1.000034, 1.000033, 1.000033, 1.000033, 1.000032, 1.000032, 1.000031, 1.000031, 1.000031, 1.000030, 1.000030, 1.000030, 1.000029, 1.000029, 1.000029, 1.000028, 1.000028, 1.000028, 1.000028, 1.000027, 1.000027, 1.000027, 1.000026, 1.000026, 1.000026, 1.000025, 1.000025, 1.000025, 1.000025, 1.000024, 1.000024, 1.000024, 1.000024, 1.000023, 1.000023, 1.000023, 1.000023, 1.000022, 1.000022, 1.000022, 1.000022, 1.000021, 1.000021, 1.000021, 1.000021, 1.000020, 1.000020, 1.000020, 1.000020, 1.000019, 1.000019, 1.000019, 1.000019, 1.000019, 1.000018, 1.000018, 1.000018, 1.000018, 1.000018, 1.000017, 1.000017, 1.000017, 1.000017, 1.000017, 1.000016, 1.000016, 1.000016, 1.000016, 1.000016, 1.000016, 1.000015, 1.000015, 1.000015, 1.000015, 1.000015, 1.000015, 1.000014, 1.000014, 1.000014, 1.000014, 1.000014, 1.000014, 1.000013, 1.000013, 1.000013, 1.000013, 1.000013, 1.000013, 1.000013, 1.000012, 1.000012, 1.000012, 1.000012, 1.000012, 1.000012, 1.000012, 1.000012, 1.000011, 1.000011, 1.000011, 1.000011, 1.000011, 1.000011, 1.000011, 1.000011, 1.000010, 1.000010, 1.000010, 1.000010, 1.000010, 1.000010, 1.000010, 1.000010, 1.000010, 1.000009, 1.000009, 1.000009, 1.000009, 1.000009, 1.000009, 1.000009, 1.000009, 1.000009, 1.000009, 1.000008, 1.000008, 1.000008, 1.000008, 1.000008, 1.000008, 1.000008, 1.000008, 1.000008, 1.000008, 1.000008, 1.000008, 1.000007, 1.000007, 1.000007, 1.000007, 1.000007, 1.000007, 1.000007, 1.000007, 1.000007, 1.000007, 1.000007, 1.000007, 1.000007, 1.000006, 1.000006, 1.000006, 1.000006, 1.000006, 1.000006, 1.000006, 1.000006, 1.000006, 1.000006, 1.000006, 1.000006, 1.000006, 1.000006, 1.000006, 1.000005, 1.000005, 1.000005, 1.000005, 1.000005, 1.000005, 1.000005, 1.000005, 1.000005, 1.000005, 1.000005, 1.000005, 1.000005, 1.000005, 1.000005, 1.000005, 1.000005, 1.000005, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000004, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000003, 1.000002, 1.000002, 1.000002, 1.000002, 1.000002, 1.000002, 1.000002, 1.000002, 1.000002, 1.000002, 1.000002, 1.000002, 1.000002, 1.000002, 1.000002, 1.000002, 1.000002
};

#define FFT_LEN_48K   (1024)
#define NUM_CHAN_48K 60

static int ch_tbl_48k[NUM_CHAN_48K*2] = {
	 0, 0 ,    //31
	 1, 1 ,    //35
	 2, 2 ,    //40
	 3, 3 ,    //44
	 4, 4 ,    //50
	 5, 5 ,            //56
	 6, 6 ,    //63
	 7, 7 ,           //70
	 8, 8 ,    //80
	 9, 9 ,            //88
	 10, 10 ,    //100
	 11, 11 ,            //111
	 12, 12 ,    //125
	 13, 13 ,             //140
	 14, 14 ,    //160
	 15, 15 ,            //178
	 16, 16 ,    //200
	 17, 17 ,            //220
	 18, 18 ,    //250
	 19, 19 ,           //280
	 20, 20 ,  //315
	 21, 21 ,           //350
	 22, 22 ,  //400
	 23, 23 ,            //440
	 24, 24 ,  //500
	 25, 25 ,          //560
	 26, 26 ,  //630
	 27, 27 ,           //700
	 28, 28 ,  //800
	 29, 29 ,             //880
	 30, 30 ,    //1k
	 31, 31 ,               //1.1k
	 32, 32 ,    //1.25k
	 33, 33 ,              //1.4k
	 34, 35 ,    //1.6k
	 36, 37 ,              //1.7k
	 38, 39 ,              //1.7k
	 40, 42 ,    //2k
	 43, 47 ,               //2.2k
	 48, 53 ,    //2.5k
	 54, 60 ,              //2.8k
	 61, 68 ,    //3.2k
	 69, 76 ,            //3.5k
	 77, 85 ,    //4k
	 86, 94 ,           //4.4k
	 95, 106 ,    //5k
	 107, 118 ,            //5.7k
	 119, 134 ,    //6.3k
	 135, 150 ,             //7k
	 151, 170 ,    //8k
	 171, 190 ,               //8.8k
	 191, 213 ,  //10k
	 214, 238 ,            //11k
	 239, 267 ,  //12.5k
	 268, 300 ,          //14k
	 301, 341 ,  //16k
	 342, 382 ,            //17.5k
	 383, 427 ,  //20k
	 428, 468 ,            //22k
	 469, 512   //24k
};

#define FFT_LEN_16K   (512)
#define NUM_CHAN_16K 54
static int ch_tbl_16k[NUM_CHAN_16K*2] = {
	// 0, 0 ,    //31
	// 1, 1 ,    //40
	// 2, 2 ,    //50
	// 3, 3 ,    //63
	// 4, 4 ,    //80
	// 5, 5 ,    //100
	// 6, 6 ,    //125
	// 7, 7 ,    //160
	// 8, 8 ,    //200
	// 9, 9 ,    //250
	// 10, 10,  //315
	// 11, 12,  //400
	// 13, 16,  //500
	// 17, 20,  //630
	// 21, 25,  //800
	// 26, 32,    //1k
	// 33, 40,    //1.25k
	// 41, 51,    //1.6k
	// 52, 64,    //2k
	// 65, 80,    //2.5k
	// 81, 102 ,    //3.2k
	// 103, 128,    //4k
	// 129, 160,    //5k
	// 161, 201,    //6.3k
	// 202, 256     //8k
	0, 0,    
	1, 1,    
	2, 2,    
	3, 3,    
	4, 4,    
	5, 5,    
	6, 6,    
	7, 7,    
	8, 8,    
	9, 9,    
	10, 10,  
	11, 11,  
	12, 12,  
	13, 13,  
	14, 14,  
	15, 15,  
	16, 16,  
	17, 17,  
	18, 18,  
	19, 19,  
	20, 20,  
	21, 21,  
	22, 22,  
	23, 23,  
	24, 24,  
	25, 25,  
	26, 26,  
	27, 27,  
	28, 28,  
	29, 29,  
	30, 30,  
	31, 31,  
	32, 32,  
	33, 33,  
	34, 35,  
	36, 37,  
	38, 39,  
	40, 42,  
	43, 47,  
	48, 53,  
	54, 60,  
	61, 68,  
	69, 76,  
	77, 85,  
	86, 94,  
	95, 106, 
	107, 118,
	119, 134,
	135, 150,
	151, 170,
	171, 190,
	191, 213,
	214, 238,
	239, 256 
};

#define RELEASE_POINTER(p) \
	if (NULL != p) { \
		delete p; \
		p = NULL; \
	}

#define RELEASE_ARRAY(p) \
	if (NULL != p) { \
		delete [] p; \
		p = NULL; \
	}

CAns::CAns(int frame_length, int channelN, int sampleRate, float max_sup_db, bool multithread)
	: m_in_buf(NULL)
	, m_buf_real(NULL)
	, m_buf_spec(NULL)
	, m_in_buf_ch(NULL)
	, m_buf_real_ch(NULL)
	, m_buf_spec_ch(NULL)
	, m_buf_pow(NULL)
	, m_ch_enrg_long_db(NULL)
	, m_ch_enrg_db(NULL)
	, m_ch_en_dev(NULL)
	, m_ch_noise_db(NULL)
	, m_ch_noise_dev(NULL)
	, m_buf_pow_ch(NULL)
	, m_buf_pow_prev(NULL)
	, m_buf_pow_local(NULL)
	, m_noise_var(NULL)
	, m_speech_var(NULL)
	, m_priori_average(NULL)
	, m_post_snr(NULL)
	, m_speech_absence_possibility(NULL)
	, m_real_fft(NULL)
	, m_multithread(multithread)
	, m_sin_win(NULL)
	, m_cos_win(NULL)
	, m_bin_gain(NULL)
	, m_bin_gain_smooth(NULL)
	, m_bin_gain_ch(NULL)
	, m_bin_gain_smooth_ch(NULL)
	, m_ready_conds(NULL)
	, m_done_conds(NULL)
	, m_prep_conds(NULL)
	, m_threads(NULL)
	, m_thread_break(false)
	, m_obj_in(NULL)
	, m_obj_out(NULL)
	, m_in(NULL)
	, m_out(NULL)
#if ANS_DEBUG
	, m_out_buf(NULL)
	, m_bin_gain_bgn(NULL)
#endif /*ANS_DEBUG*/
{
#ifdef DEBUG_VAD
	debugFile = fopen("vadflag.pcm", "wb");
#endif

	m_init_stable = false;
	m_noise_update_flag = 0;
	m_enrg_dev_weight = 0.1;
	m_spectralFlatness = 0;
	m_frame_len = frame_length;
	m_sample_rate = sampleRate;
	if (m_sample_rate <= 16000)
	{
		m_fft_len = FFT_LEN_16K;
		m_num_band = NUM_CHAN_16K;
		ch_tbl = ch_tbl_16k;
		m_fft_idx_low = 10;   //300hz
		m_fft_idx_high = 51;  //7000hz
	}
	else
	{
		m_fft_len = FFT_LEN_48K;
		m_num_band = NUM_CHAN_48K;
		ch_tbl = ch_tbl_48k;
		m_fft_idx_low = 10;   //500hz
		m_fft_idx_high = 50;  //8000hz
	}
	m_fft_len_div_2 = m_fft_len >> 1;
	m_overlap_len = m_frame_len;
	m_channelN = channelN;
	m_frame_cnt = 0;
	m_init_frame_num = INIT_TIME*m_sample_rate / m_frame_len/1000;
	m_max_sup_db = max_sup_db;
	if (m_max_sup_db > MAXSUP1) { 
		m_max_sup_db = MAXSUP1;
	}
	if (m_max_sup_db < MAXSUP2) {
		m_max_sup_db = MAXSUP2;
	}
	m_max_sup_db = powf(10.f, m_max_sup_db / 20.f);

	m_overdrive = new float[m_num_band];

    m_ch_enrg_long_db = new float[m_num_band];
	memset(m_ch_enrg_long_db, 0x00, sizeof(float) * m_num_band);
	m_ch_enrg_db = new float[m_num_band];
	memset(m_ch_enrg_db, 0x00, sizeof(float) * m_num_band);
	m_ch_en_dev = new float[m_num_band];
	memset(m_ch_en_dev, 0x00, sizeof(float) * m_num_band);

	m_ch_noise_db = new float[m_num_band];
	memset(m_ch_noise_db, 0x00, sizeof(float) * m_num_band);
	m_ch_noise_dev = new float[m_num_band];
	memset(m_ch_noise_dev, 0x00, sizeof(float) * m_num_band);

	m_in = new float[m_frame_len*m_channelN];
	memset(m_in, 0x00, sizeof(float) * m_frame_len*m_channelN);
	m_out = new float[m_frame_len*m_channelN];
	memset(m_out, 0x00, sizeof(float) * m_frame_len*m_channelN);

	m_in_buf = new float[m_fft_len];
	memset(m_in_buf, 0x00, sizeof(float) * m_fft_len);
	m_buf_real = new R[m_fft_len];
	memset(m_buf_real, 0x00, sizeof(R) * m_fft_len);
	m_buf_spec = new C[m_fft_len_div_2 + 1];
	m_buf_pow = new float[m_num_band];
	m_buf_pow_prev = new float[m_num_band];
	m_buf_pow_local = new float[m_num_band];
	m_noise_var = new float[m_num_band];
	m_speech_var = new float[m_num_band];
	m_priori_average = new float[m_num_band];
	m_post_snr = new float[m_num_band];
	m_speech_absence_possibility = new float[m_num_band];
	m_bin_gain = new float[m_num_band];
	m_bin_gain_smooth = new float[m_num_band];
	for (int i = 0; i < m_num_band; i++) {
		m_buf_pow_prev[i] = 0.f;
		m_buf_pow_local[i] = 1.f;  /* why? */
		m_noise_var[i] = 1.f;
		m_speech_var[i] = 1.f;
		m_priori_average[i] = 1.f;
		m_post_snr[i] = 1.f;
		m_bin_gain[i] = 0.f;
		m_bin_gain_smooth[i] = 1.f;
	}
	m_in_buf_ch = new float*[channelN];
	m_buf_real_ch = new R*[channelN];
	m_buf_spec_ch = new C*[channelN];
	m_buf_pow_ch = new float*[channelN];
	m_bin_gain_ch = new float*[channelN];
	m_bin_gain_smooth_ch = new float*[channelN];
	for (int i = 0; i < channelN; i++) {
		m_in_buf_ch[i] = new float[m_fft_len];
		memset(m_in_buf_ch[i], 0x00, sizeof(float) * m_fft_len);
		m_buf_real_ch[i] = new R[m_fft_len];
		memset(m_buf_real_ch[i], 0x00, sizeof(R) * m_fft_len);
		m_buf_spec_ch[i] = new C[m_fft_len_div_2 + 1];
		m_buf_pow_ch[i] = new float[m_num_band];
		m_bin_gain_ch[i] = new float[m_num_band];
		m_bin_gain_smooth_ch[i] = new float[m_num_band];
		for (int j = 0; j < m_num_band; j++) {
			m_bin_gain_ch[i][j] = 0.f;
			m_bin_gain_smooth_ch[i][j] = 1.f;
		}
	}

	uint logN = (uint)(log((float)m_fft_len) / log(2.0f) + 0.5);
	int fft_num = m_multithread ? channelN : 1;
	m_real_fft = new CRealFFT*[fft_num];
	for (int i = 0; i < fft_num; i++) {
		m_real_fft[i] = new CRealFFT(logN);  //andy: real fft
	}
	m_fft_scale = 1.f / (float)m_fft_len;
	
	m_sin_win = new float[m_frame_len*2];
	m_cos_win = new float[m_frame_len*2];
	for (int i = 0; i < m_frame_len * 2; i++) {
		float ang = M_PI * (float)i / (float)(m_frame_len*2);
		m_sin_win[i] = sin(ang);
		m_cos_win[i] = cos(ang);
	}

	if (m_multithread) {
		m_ready_conds = new CCond[channelN];
		m_done_conds = new CCond[channelN];
		m_prep_conds = new CCond[channelN];
		m_threads = new CThread[channelN];
		m_thread_break = false;
		for (int i = 0; i < channelN; i++) {
			m_prep_conds[i].lock();
			ThreadArgs args;
			args.index = i;
			args.user = this;
			m_threads[i].create(CAns::thread_process, &args);
			m_prep_conds[i].wait();
			m_prep_conds[i].unlock();
		}
	}

	m_ans_param.avgsnrglobal = 0.f;
	m_ans_param.speechAbsencePossibilityAvg = 1.0f;

	float overdrive_bend = 700;
	float overdrive_aggress;
	if (max_sup_db >= -10)
	{
		overdrive_aggress = 0.21;
	}
	else if (max_sup_db >= -20)
	{
		overdrive_aggress = 0.44;
	}
	else if (max_sup_db >= -30)
	{
		overdrive_aggress = 0.69;
	}
	else
	{
		overdrive_aggress = 1.0;
	}

	for (int i = 0; i < m_num_band; i++) {
		float ii = (float)i/(m_num_band-1);
		m_overdrive[i] = (pow(overdrive_bend, ii)-1)/(pow(overdrive_bend, 1)-1)*overdrive_aggress+1;
		if(m_overdrive[i]<1.0)  m_overdrive[i]=1.0;
	}

#if ANS_DEBUG
	m_out_buf = new float[m_frame_len];
	m_bin_gain_bgn = new float[m_fft_len_div_2 + 1];
	for (int i = 0; i < m_fft_len_div_2 + 1; i++) {
		m_bin_gain_bgn[i] = 0.f;
	}
#endif /*ANS_DEBUG*/
}

CAns::~CAns()
{
	if (m_multithread) {
		m_thread_break = true;
		for (int i = 0; i < m_channelN; i++) {
			m_ready_conds[i].signal_s();
			m_threads[i].join();
		}
		RELEASE_ARRAY(m_threads);
		RELEASE_ARRAY(m_ready_conds);
		RELEASE_ARRAY(m_done_conds);
		RELEASE_ARRAY(m_prep_conds);
	}
	int fft_num = m_multithread ? m_channelN : 1;
	for (int i = 0; i < fft_num; i++) {
		RELEASE_POINTER(m_real_fft[i]);
	}
	RELEASE_ARRAY(m_ch_enrg_long_db);
	RELEASE_ARRAY(m_ch_enrg_db);
	RELEASE_ARRAY(m_ch_en_dev);
	RELEASE_ARRAY(m_ch_noise_db);
	RELEASE_ARRAY(m_ch_noise_dev);
	RELEASE_ARRAY(m_in);
	RELEASE_ARRAY(m_out);
	RELEASE_ARRAY(m_real_fft);
	RELEASE_ARRAY(m_in_buf);
	RELEASE_ARRAY(m_buf_real);
	RELEASE_ARRAY(m_buf_spec);
	RELEASE_ARRAY(m_buf_pow);
	RELEASE_ARRAY(m_buf_pow_prev);
	RELEASE_ARRAY(m_buf_pow_local);
	RELEASE_ARRAY(m_noise_var);
	RELEASE_ARRAY(m_speech_var);
	RELEASE_ARRAY(m_priori_average);
	RELEASE_ARRAY(m_post_snr);
	RELEASE_ARRAY(m_sin_win);
	RELEASE_ARRAY(m_cos_win);
	RELEASE_ARRAY(m_bin_gain);
	RELEASE_ARRAY(m_bin_gain_smooth);
	RELEASE_ARRAY(m_speech_absence_possibility);
	for (int i = 0; i < m_channelN; i++) {
		RELEASE_ARRAY(m_in_buf_ch[i]);
		RELEASE_ARRAY(m_buf_real_ch[i]);
		RELEASE_ARRAY(m_buf_spec_ch[i]);
		RELEASE_ARRAY(m_buf_pow_ch[i]);
		RELEASE_ARRAY(m_bin_gain_ch[i]);
		RELEASE_ARRAY(m_bin_gain_smooth_ch[i]);
	}
	RELEASE_ARRAY(m_in_buf_ch);
	RELEASE_ARRAY(m_buf_real_ch);
	RELEASE_ARRAY(m_buf_spec_ch);
	RELEASE_ARRAY(m_buf_pow_ch);
	RELEASE_ARRAY(m_bin_gain_ch);
	RELEASE_ARRAY(m_bin_gain_smooth_ch);
#if ANS_DEBUG
	RELEASE_ARRAY(m_out_buf);
	RELEASE_ARRAY(m_bin_gain_bgn);
#endif /*ANS_DEBUG*/

#ifdef DEBUG_VAD
	fclose(debugFile);
#endif
}

void CAns::noise_spectrum_estimator()
{
	const float *buf_pow_iter = m_buf_pow;
	float *buf_pow_local_iter = m_buf_pow_local;
	*buf_pow_local_iter = AS * *buf_pow_local_iter + (1.f - AS) * *buf_pow_iter;
	buf_pow_local_iter++; buf_pow_iter++;

	for (int i = 1; i < m_num_band - 1; i++) {
		*buf_pow_local_iter = AS * *buf_pow_local_iter + (1.f - AS) * (*(buf_pow_iter - 1) + *(buf_pow_iter + 1) + 2.f * *buf_pow_iter) * 0.25f;
		buf_pow_local_iter++; buf_pow_iter++;
	}
	*buf_pow_local_iter = AS * *buf_pow_local_iter + (1.f - AS) * *buf_pow_iter;

	if (m_frame_cnt < m_init_frame_num) {
		memcpy(m_noise_var, m_buf_pow_local, sizeof(float) * (m_num_band));
		float *noise_var_iter = m_noise_var;
		for (int i = 0; i < m_num_band; i++) {
			if (*noise_var_iter < VARMIN) {
				*noise_var_iter = VARMIN;
			}
			noise_var_iter++;
		}
	}
	else {
		const float *buf_pow_iter = m_buf_pow;
		const float *buft_pow_local_iter = m_buf_pow_local;
		const float *speech_absence_possibility_iter = m_speech_absence_possibility;
		float *noise_var_iter = m_noise_var;
		for (int i = 0; i < m_num_band; i++) {
			float w;
			
			/*
			if (m_noise_update_flag)
			{
				w = 0.5;
			}
			else
			{
				if (*buft_pow_local_iter > *noise_var_iter) {
					float tmpf = *speech_absence_possibility_iter;
					if (tmpf < 0.1) tmpf = 0.1;
					if (m_ans_param.speechAbsencePossibilityAvg < 0.35f)	//global snr greater than 3db
					{
						w = tmpf * 0.001f;
					}
					else {	
						w = tmpf * 0.002f;
					}
				}
				else {
					w = 0.1f;
				}
			}
			*noise_var_iter = (1.f - w) * *noise_var_iter + w * *buft_pow_local_iter;
			*/

			if (m_noise_update_flag)
			{
				float flatnessTmp = (m_spectralFlatness - 0.1);
				if (flatnessTmp<0) flatnessTmp = 0;
				if (*buft_pow_local_iter > *noise_var_iter)
				{
					w = 0.1f * flatnessTmp;  //reset noise, can be very dangerous.
				}
				else
				{
					w = 0.1f;
				}				
			}
			else if (*buft_pow_local_iter > *noise_var_iter)
			{
				float probSpeechAbsence = (*speech_absence_possibility_iter);
				float probSpeechAbsence_avg = m_ans_param.speechAbsencePossibilityAvg;
				w = 0.005*probSpeechAbsence_avg*probSpeechAbsence;
				if (w < 0.00005) w = 0.00005;  //give some chance to climb. 				
			}
			else
			{
				w = 0.1f;				
			}
			*noise_var_iter = (1.f - w) * *noise_var_iter + w * *buft_pow_local_iter;			

			if (*noise_var_iter < VARMIN) {
				*noise_var_iter = VARMIN;
			}
			buft_pow_local_iter++; buf_pow_iter++; noise_var_iter++; speech_absence_possibility_iter++;
		}
	}
}


void CAns::snr_estimator()
{
	float maxsnr = 40.0;
	float *re_average_iter = m_priori_average;
	const float *buf_pow_iter = m_buf_pow;
	const float *buf_pow_prev_iter = m_buf_pow_prev;
	const float *noise_var_iter = m_noise_var;
	float *bin_gain_smooth_iter = m_bin_gain_smooth;
	float *post_snr = m_post_snr;
	for (int i = 0; i < m_num_band; i++) {

		float postsnr = (*buf_pow_iter) / (*noise_var_iter);
		if (postsnr < 1.0) postsnr = 1.0;
		if (postsnr > maxsnr) postsnr = maxsnr;
		*post_snr = postsnr;

		float priorisnr = (*buf_pow_prev_iter) * (*bin_gain_smooth_iter) / (*noise_var_iter);
		if (priorisnr > maxsnr) priorisnr = maxsnr;

		float priorisnr_curr = SMOOTH_SNR*priorisnr + (1 - SMOOTH_SNR)*(postsnr - 1);

		//*re_average_iter = *re_average_iter * ZC + priorisnr_curr * (1.f - ZC);
		*re_average_iter = priorisnr_curr;
		if (*re_average_iter > SNRMAX) {
			*re_average_iter = SNRMAX;
		}
		else if (*re_average_iter < SNRMIN) {
			*re_average_iter = SNRMIN;
		}
		
		re_average_iter++; buf_pow_iter++; buf_pow_prev_iter++; noise_var_iter++; bin_gain_smooth_iter++; post_snr++;
	}	

}

void CAns::spectralFlatness() 
{
	int i;
	float avgSpectralFlatnessNum, avgSpectralFlatnessDen, spectralTmp;

	// Compute spectral measures.
	// For flatness.
	avgSpectralFlatnessNum = 0.0;
	avgSpectralFlatnessDen = 0.0;

	// Compute log of ratio of the geometric to arithmetic mean.
	for (i = m_fft_idx_low; i < m_fft_idx_high; i++) {
		avgSpectralFlatnessNum += (float)log(m_buf_pow[i]);
		avgSpectralFlatnessDen += m_buf_pow[i];
	}
	// Normalize.
	avgSpectralFlatnessDen = avgSpectralFlatnessDen / (m_fft_idx_high - m_fft_idx_low);
	avgSpectralFlatnessNum = avgSpectralFlatnessNum / (m_fft_idx_high - m_fft_idx_low);

	// Ratio and inverse log: check for case of log(0).
	spectralTmp = (float)exp(avgSpectralFlatnessNum) / avgSpectralFlatnessDen;

	// Time-avg update of spectral flatness feature.
	if (m_frame_cnt == 0)
	{
		m_spectralFlatness = spectralTmp;
	}
	else
	{
		m_spectralFlatness += FLATNESS_SMOOTH * (spectralTmp - m_spectralFlatness);
	}
	// Done with flatness feature.
}


void CAns::feature_estimator()
{
	spectralFlatness();
	//global snr 
	float *re_average_iter = m_priori_average;
	m_ans_param.avgsnrglobal = 0.f;
	re_average_iter = m_priori_average + m_fft_idx_low;
	for (int i = m_fft_idx_low; i < m_fft_idx_high; i++) {
		m_ans_param.avgsnrglobal += *re_average_iter;
		re_average_iter++;
	}
	m_ans_param.avgsnrglobal /= (float)(m_fft_idx_high - m_fft_idx_low);

	float lsnr_min = 10 * log10(0.1);
	float lsnr_max = 10 * log10(10);
	float lsnr = 10 * log10(m_ans_param.avgsnrglobal);
	if (lsnr > lsnr_max) lsnr = lsnr_max;
	if (lsnr < lsnr_min) lsnr = lsnr_min;
	m_ans_param.speechAbsencePossibilityAvg = 1.f - (lsnr - lsnr_min) / (lsnr_max - lsnr_min);



	float *speech_absence_possibility_iter = m_speech_absence_possibility;
	re_average_iter = m_priori_average;

	for (int i = 0; i < m_num_band; i++) {
		lsnr_min = 10 * log10(0.5);
		lsnr_max = 10 * log10(40);
		lsnr = 10 * log10(*re_average_iter);
		if (lsnr > lsnr_max) lsnr = lsnr_max;
		if (lsnr < lsnr_min) lsnr = lsnr_min;
		*speech_absence_possibility_iter = 1.f - (lsnr - lsnr_min) / (lsnr_max - lsnr_min);
		re_average_iter++; speech_absence_possibility_iter++;
	}

	for (int i = 0; i < m_num_band; i++)
	{
		//m_ch_enrg_db[i] = log10(m_buf_pow[i] * m_bin_gain_smooth[i]) * 10;
		m_ch_enrg_db[i] = log10(m_buf_pow[i]) * 10;
		m_ch_noise_db[i] = log10(m_noise_var[i]) * 10;
	}
	if (m_frame_cnt == 0)
	{
		m_noise_update_cnt = 0;
		for (int i = 0; i < m_num_band; i++)
		{
			m_ch_enrg_long_db[i] = m_ch_enrg_db[i];
		}
	}
	for (int i = m_fft_idx_low; i < m_fft_idx_high; i++)
	{
		m_ch_en_dev[i] = fabs((m_ch_enrg_long_db[i] - m_ch_enrg_db[i]));
		m_ch_noise_dev[i] = fabs((m_ch_enrg_long_db[i] - m_ch_noise_db[i]));
	}
	float enrg_dev = 0;
	float noise_dev = 0;
	for (int i = m_fft_idx_low; i < m_fft_idx_high; i++)
	{
		enrg_dev = enrg_dev + m_ch_en_dev[i];
		noise_dev = noise_dev + m_ch_noise_dev[i];
	}
	enrg_dev = enrg_dev / (m_fft_idx_high - m_fft_idx_low);
	noise_dev = noise_dev / (m_fft_idx_high - m_fft_idx_low);

	float beta;
	for (int i = 0; i < m_num_band; i++)
	{
		if (m_ch_enrg_db[i] > m_ch_enrg_long_db[i])
		{
			beta = 0.1;
		}
		else
		{
			beta = 0.1;
		}
		m_ch_enrg_long_db[i] = m_ch_enrg_long_db[i] * (1-beta) + m_ch_enrg_db[i] * beta;
	}


	if (m_init_stable && enrg_dev < DEV_DB_THD && m_ans_param.speechAbsencePossibilityAvg<0.5)
	{
		m_noise_update_cnt++;
		if (m_noise_update_cnt > NOISE_UPDATE_CNT)
		{
			m_noise_update_flag = 1;
			m_noise_update_cnt = 0;
		}
	}
	else
	{
		m_noise_update_flag = 0;
		m_noise_update_cnt = 0;
	}


#ifdef DEBUG_VAD
	int outch = 2;
	int nn = 0;
	for (int i = 0; i < 256; i++)
	{
		vadflag[nn++] = (1 - m_ans_param.speechAbsencePossibilityAvg) * 10000;
		//vadflag[nn++] = enrg_dev * 100;
		//vadflag[nn++] = noise_dev * 10;
		//vadflag[i] = m_spectralFlatness * 10000;
		vadflag[nn++] = m_noise_update_flag * 10000;
	}
	fwrite(vadflag, sizeof(short), 256 * outch, debugFile);
	//printf("snr = %.3f", pResGlobal->avgsnrglobal);
#endif
}

void CAns::gain_estimator()
{	
	snr_estimator();
	feature_estimator();
	noise_spectrum_estimator();
	memcpy(m_buf_pow_prev, m_buf_pow, sizeof(float) * (m_num_band));

	const float *noise_var_iter = m_noise_var;
	const float *buf_pow_iter = m_buf_pow;
	const float *speech_absence_possibility_iter = m_speech_absence_possibility;
	float *bin_gain_iter = m_bin_gain;
	float *bin_gain_smooth_iter = m_bin_gain_smooth;

	for (int i = 0; i < m_num_band; i++) {
		float tmp = m_priori_average[i] / (m_overdrive[i] + m_priori_average[i]);

		//apply mmse-lsa gain process
		/*
		float vk = tmp*m_post_snr[i];
		if(vk<1) vk = 1;
		if(vk>10) vk = 10;
		int idx = (int)(vk*100)-1;
		float tmpf = mmse_lsa_table[idx];
		tmp = tmp*tmpf;
		*/

		if (tmp > 1.f) {
			tmp = 1.f;
		}


		float tmp_gain = tmp;
		// tmp_gain = sqrtf(tmp_gain);   //wiena filter or spectrum substraction (sqrt).

		tmp_gain = tmp_gain * (1.f - *speech_absence_possibility_iter*m_ans_param.speechAbsencePossibilityAvg) + m_max_sup_db * *speech_absence_possibility_iter*m_ans_param.speechAbsencePossibilityAvg;
		//tmp_gain = tmp_gain * (1.f - *speech_absence_possibility_iter) + m_max_sup_db * *speech_absence_possibility_iter;


		if(tmp_gain < m_max_sup_db) {
			tmp_gain = m_max_sup_db;
		}
		//tmp_gain = *bin_gain_smooth_iter * 0.5f + tmp_gain * 0.5f;
		*bin_gain_smooth_iter = tmp_gain;
		*bin_gain_iter = tmp_gain;

		noise_var_iter++; buf_pow_iter++; speech_absence_possibility_iter++; bin_gain_iter++; bin_gain_smooth_iter++; 
	}
}

void CAns::gain_estimator_ch(int c)
{
	float *bin_gain_ch_iter = m_bin_gain_ch[c];
	float *bin_gain_smooth_ch_iter = m_bin_gain_smooth;
	for (int i = 0; i < m_num_band; i++) {
		
		*bin_gain_ch_iter = *bin_gain_smooth_ch_iter;
		bin_gain_ch_iter++; bin_gain_smooth_ch_iter++;
	}
}

void CAns::overlap_plus_pre(int c)
{
	C *buf_spec;
	R *buf_real;
	const float *in_buf_iter;
	float *buf_pow_iter;
	CRealFFT *fft;
	if (c < 0) {
		buf_spec = m_buf_spec;
		buf_real = m_buf_real;
		in_buf_iter = m_in_buf;
		buf_pow_iter = m_buf_pow;
		fft = m_real_fft[0];
	}
	else {
		buf_spec = m_buf_spec_ch[c];
		buf_real = m_buf_real_ch[c];
		in_buf_iter = m_in_buf_ch[c];
		buf_pow_iter = m_buf_pow_ch[c];
		fft = m_real_fft[m_multithread ? c : 0];
	}
	const float *sin_win_iter = m_sin_win;
	R *buf_real_iter = buf_real;
	for (int i = 0; i < m_frame_len*2; i++) {
		*buf_real_iter = *sin_win_iter * *in_buf_iter;
		buf_real_iter++; sin_win_iter++; in_buf_iter++;
	}
	for (int i = m_frame_len * 2; i < m_fft_len; i++)
	{
		*buf_real_iter = 0;
		buf_real_iter++;
	}
	fft->run_fwd_fft(buf_real, buf_spec);
	const C *buf_spec_iter = buf_spec;

	for (int i = 0; i < m_num_band; i++) {
		*buf_pow_iter = 0;
		int j1 = ch_tbl[i*2+0];
		int j2 = ch_tbl[i*2+1];
		for (int j = j1; j <= j2; j++)		{
			*buf_pow_iter += buf_spec_iter->re * buf_spec_iter->re + buf_spec_iter->im * buf_spec_iter->im;
			buf_spec_iter++;
		}
		*buf_pow_iter /= (j2 - j1 + 1);
		if (*buf_pow_iter < SMALLV) {
			*buf_pow_iter = SMALLV;
		}
		buf_pow_iter++;
	}
}

void CAns::overlap_plus_post(float *out, int c)
{
	C *buf_spec;
	R *buf_real;
	float *out_iter = NULL;
	int out_step;
	float gaintmp;
	const float *bin_gain_iter;
	CRealFFT *fft;
	if (c < 0) {
		buf_spec = m_buf_spec;
		buf_real = m_buf_real;
#if ANS_DEBUG
		out_iter = m_out_buf;
		out_step = 1;
#endif /*ANS_DEBUG*/
		bin_gain_iter = m_bin_gain;
		fft = m_real_fft[0];
	}
	else {
		buf_spec = m_buf_spec_ch[c];
		buf_real = m_buf_real_ch[c];
		out_iter = out + c;
		out_step = m_channelN;
		bin_gain_iter = m_bin_gain_ch[c];
		fft = m_real_fft[m_multithread ? c : 0];
	}	
	
	C *buf_spec_iter = buf_spec;
	for (int i = 0; i < m_num_band; i++) {
		int j1 = ch_tbl[i*2+0];
		int j2 = ch_tbl[i*2+1];
		int jmid = (int)((j1 + j2) / 2);
		for (int j = j1; j <= j2; j++) {
			if (i == m_num_band - 1 || j1==j2 || j<jmid)
			{
				gaintmp = bin_gain_iter[i];
			}
			else
			{				
				float a1 = ((float)j - jmid) / (j2 - jmid+1);
				float a0 = 1 - a1;
				gaintmp = bin_gain_iter[i] * a0 + bin_gain_iter[i+1]*a1;
			}
			buf_spec_iter->re *= gaintmp;
			buf_spec_iter->im *= gaintmp;
			buf_spec_iter++;
		}
	}
	fft->run_inv_fft(buf_spec, buf_real);

	const float *sin_win_iter = m_sin_win;
	R *buf_real_iter = buf_real;
	for (int i = 0; i < m_frame_len*2; i++) {
		*buf_real_iter *= (*sin_win_iter * m_fft_scale);
		buf_real_iter++; sin_win_iter++;
	}

	if (NULL != out_iter) {
		for (int i = 0; i < m_frame_len; i++) {
			*out_iter += *buf_real;
			out_iter += out_step; buf_real++;
		}
	}
}

void CAns::prepare(const float *in, float *out, int c)
{
	if (c < 0) {
		/* Input. */
		memmove(m_in_buf, m_in_buf + m_frame_len, sizeof(float) * m_overlap_len);
		const float *in_iter = in;
		float *in_buf_iter = m_in_buf + m_overlap_len;
		static float scale = 1.f / sqrtf((float)m_channelN);
		for (int i = 0; i < m_frame_len; i++) {
			*in_buf_iter = 0.f;
			for (int k = 0; k < m_channelN; k++) {
				*in_buf_iter += *in_iter;
				in_iter++;
			}
			*in_buf_iter *= scale;
			in_buf_iter++;
		}
		/* Output. */
#if ANS_DEBUG
		const float *buf_real_iter = m_buf_real + m_overlap_len;
		float *out_buf_iter = m_out_buf;
		for (int i = 0; i < m_frame_len; i++) {
			*out_buf_iter = *buf_real_iter;
			out_buf_iter++; buf_real_iter++;
		}
#else
		return;
#endif /*ANS_DEBUG*/
	}
	else {
		/* Input. */
		memmove(m_in_buf_ch[c], m_in_buf_ch[c] + m_frame_len, sizeof(float) * m_overlap_len);
		const float *in_iter = in + c;
		float *in_buf_ch_inter = m_in_buf_ch[c] + m_overlap_len;
		for (int i = 0; i < m_frame_len; i++) {
			*in_buf_ch_inter = *in_iter;
			in_buf_ch_inter++; in_iter += m_channelN;
		}
		/* Output. */
		const float *buf_real_ch_iter = m_buf_real_ch[c] + m_overlap_len;
		float *out_iter = out + c;
		for (int i = 0; i < m_frame_len; i++) {
			*out_iter = *buf_real_ch_iter;
			out_iter += m_channelN; buf_real_ch_iter++;
		}
	}
}

// Update is called once every frame
void CAns::process(const float *in, float *out)
{
	memcpy(m_in, in, sizeof(float)*m_frame_len*m_channelN);

	prepare(m_in, m_out, -1);
	overlap_plus_pre(-1);
	gain_estimator();
	overlap_plus_post(m_out, -1);

	if (m_multithread) {
		for (int i = 0; i < m_channelN; i++) {
			m_done_conds[i].lock();
		}
		m_obj_in = m_in;
		m_obj_out = m_out;
		for (int i = 0; i < m_channelN; i++) {
			m_ready_conds[i].signal_s();
		}
		for (int i = 0; i < m_channelN; i++) {
			m_done_conds[i].wait();
			m_done_conds[i].unlock();
		}
	}
	else {
		for (int c = 0; c < m_channelN; c++) {
			prepare(m_in, m_out, c);
			overlap_plus_pre(c);
			gain_estimator_ch(c);
			overlap_plus_post(m_out, c);
		}
	}

	memcpy(out, m_out, sizeof(float)*m_frame_len*m_channelN);

	if (m_frame_cnt <= (float)m_sample_rate*STABLE_TIME / 1000 / m_frame_len)
	{
		m_frame_cnt++;
	}	
	else
	{
		m_init_stable = true;
	}

}

void CAns::set(float max_sup_db)
{
	m_max_sup_db = max_sup_db;
	if (m_max_sup_db > MAXSUP1) {
		m_max_sup_db = MAXSUP1;
	}
	if (m_max_sup_db < MAXSUP2) {
		m_max_sup_db = MAXSUP2;
	}
	m_max_sup_db = powf(10.f, m_max_sup_db / 20.f);
}

void * CAns::thread_process(void *args)
{
	CAns::ThreadArgs* user = (CAns::ThreadArgs*)args;
	user->user->thread_process_do(user->index);
	return NULL;
}

void CAns::thread_process_do(int index)
{
	m_ready_conds[index].lock();
	m_prep_conds[index].signal_s();
	for (;;) {
		m_ready_conds[index].wait();
		/* This thread cancel point. */
		if (m_thread_break) {
			break;
		}
		prepare(m_obj_in, m_obj_out, index);
		overlap_plus_pre(index);
		gain_estimator_ch(index);
		overlap_plus_post(m_obj_out, index);
		m_done_conds[index].signal_s();
	}
	m_ready_conds[index].unlock();
}
