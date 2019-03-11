/*
 * @Description:  
 * @version:  V1.0
 * @Company: Twirling in time
 * @Author: Lipingping
 * @Date: 2019-01-30 18:43:04
 * @LastEditors: Lipingping
 * @LastEditTime: 2019-03-11 18:45:44
 */

#include "PlayBackAudio.h"
#include "log.h"
#include "util.h"

// #define DUMP_PCM_DATA
using namespace std;

PlayBackAudio::PlayBackAudio(string deviceName, int rate,
							 int channel, snd_pcm_format_t dataFormart, int micProfileId, int micFormatId, float micDist)
	: deviceName(deviceName), rate(rate), channel(channel), dataFormart(dataFormart),
	  micProfileId(micProfileId), micFormatId(micFormatId), micDist(micDist), th_capture(0),
	  isRun(false), playback_handle(NULL), playback_hw_params(NULL), sArray(NULL), pfifo(NULL)
{
	// this->sArray = (short *)malloc(sizeof(short) * FRAMELEN * this->channel * FIFO_BUFF_COUNT);
	// this->pfifo = &this->fifo;
	// FIFO_Init(this->pfifo, sArray, sizeof(short) * FRAMELEN * this->channel, FIFO_BUFF_COUNT);
	pthread_mutex_init(&this->m_Mutex, NULL);
	pthread_cond_init(&this->m_Cond, NULL);
	wakeupEvent = new WakeupEvent();
	this->prepare();
	this->isRun = false;
}

PlayBackAudio::~PlayBackAudio()
{
	this->isRun = false;
	pthread_mutex_destroy(&this->m_Mutex);
	pthread_cond_destroy(&this->m_Cond);
	if (instance)
	{
		delete instance;
		instance = NULL;
	}
	if (wakeupEvent)
	{
		delete wakeupEvent;
		wakeupEvent = NULL;
	}
}

PlayBackAudio::PlayBackAudio(const PlayBackAudio &)
	: deviceName("hw:0"), rate(16000), channel(1), dataFormart(SND_PCM_FORMAT_S16_LE),
	  micProfileId(1), micFormatId(4), micDist(0.06), th_capture(0),
	  isRun(false), playback_handle(NULL), playback_hw_params(NULL), sArray(NULL), pfifo(NULL)
{
}

PlayBackAudio &PlayBackAudio::operator=(const PlayBackAudio &s)
{
	if (this == &s)
		return *this;
	this->deviceName = s.deviceName;
	this->rate = s.rate;
	this->channel = s.channel;
	this->dataFormart = s.dataFormart;
	this->micProfileId = s.micProfileId;
	this->micFormatId = s.micFormatId;
	this->micDist = s.micDist;
	this->th_capture = 0;
	this->isRun = s.isRun;
	this->playback_handle = s.playback_handle;
	this->playback_hw_params = s.playback_hw_params;
	this->sArray = NULL;
	this->pfifo = NULL;
	return *this;
}

//在此处初始化
PlayBackAudio *PlayBackAudio::instance = NULL;

PlayBackAudio *PlayBackAudio::getInstance(string deviceName, int rate,
										  int channel, snd_pcm_format_t dataFormart, int micProfileId, int micFormatId, float micDist)
{
	if (instance == NULL)
	{
		// lock();
		if (instance == NULL)
		{
			instance = new PlayBackAudio(deviceName, rate, channel, dataFormart, micProfileId, micFormatId, micDist);
		}
		// unlock();
	}
	return instance;
}

string PlayBackAudio::getDeviceName()
{
	return this->deviceName;
}
int PlayBackAudio::getRate()
{
	return this->rate;
}
int PlayBackAudio::getChannel()
{
	return this->channel;
}
snd_pcm_format_t PlayBackAudio::getDataFormart()
{
	return this->dataFormart;
}

int PlayBackAudio::getMicProfileId()
{
	return this->micProfileId;
}
int PlayBackAudio::getMicFormatId()
{
	return this->micFormatId;
}
float PlayBackAudio::getMicDist()
{
	return this->micDist;
}
bool PlayBackAudio::getDataPackage(void *data)
{
	return 1 == FIFO_GetOne(this->pfifo, data);
}
bool PlayBackAudio::getIsRun()
{
	return this->isRun;
}

pthread_t PlayBackAudio::getCaptureThreadId()
{
	return this->th_capture;
}
/**
 * @description: playback initial func
 * @param {type} 
 * @return: 
 */
int PlayBackAudio::prepare()
{
	int err = 0;
	int dir;
	unsigned int tmpRate = this->rate;
	if ((err = snd_pcm_open(&this->playback_handle, this->deviceName.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0)
	{
		macroFuncVargs("playback: cannot open playback audio device %s (err=%d, %s)",
					   this->deviceName.c_str(),
					   err,
					   snd_strerror(err));
		return -1;
	}
	macroFunc("playback: playback audio interface opened");

	if ((err = snd_pcm_nonblock(this->playback_handle, 0)) != 0)
	{
		macroFuncVargs("playback: cannot set block playback audio device %s (%s)",
					   this->deviceName.c_str(),
					   snd_strerror(err));
		return -2;
	}
	macroFunc("playback: playback audio interface set block");

	if ((err = snd_pcm_hw_params_malloc(&this->playback_hw_params)) < 0)
	{
		macroFuncVargs("playback: cannot allocate hardware parameter structure (%s)",
					   snd_strerror(err));
		return -3;
	}
	macroFunc("playback: this->playback_hw_params allocated");

	if ((err = snd_pcm_hw_params_any(this->playback_handle, this->playback_hw_params)) < 0)
	{
		macroFuncVargs("playback: cannot initialize hardware parameter structure (%s)",
					   snd_strerror(err));
		return -4;
	}
	macroFunc("playback: this->playback_hw_params initialized");

	if ((err = snd_pcm_hw_params_set_access(this->playback_handle, this->playback_hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
	{
		macroFuncVargs("playback: cannot set access type (%s)",
					   snd_strerror(err));
		return -5;
	}
	macroFunc("playback: this->playback_hw_params access setted");

	if ((err = snd_pcm_hw_params_set_format(this->playback_handle, this->playback_hw_params, this->dataFormart)) < 0)
	{
		macroFuncVargs("playback: cannot set sample format (%s)",
					   snd_strerror(err));
		return -6;
	}
	macroFunc("playback: this->playback_hw_params format setted");

	dir = 0;
	if ((err = snd_pcm_hw_params_set_rate_near(this->playback_handle, this->playback_hw_params, &tmpRate, &dir)) < 0)
	{
		macroFuncVargs("playback: cannot set sample rate (%s)",
					   snd_strerror(err));
		return -7;
	}
	macroFuncVargs("playback: this->playback_hw_params rate setted, rate is %d", tmpRate);

	if ((err = snd_pcm_hw_params_set_channels(this->playback_handle, this->playback_hw_params, this->channel)) < 0)
	{
		macroFuncVargs("playback: cannot set channel count (%s)",
					   snd_strerror(err));
		return -8;
	}

	macroFunc("playback: this->playback_hw_params channels setted");

	if ((err = snd_pcm_hw_params_set_buffer_size(this->playback_handle, this->playback_hw_params, FRAMELEN * 2)) < 0)
	{
		macroFuncVargs("playback: cannot set buffer size (%s)",
					   snd_strerror(err));
		return -9;
	}
	macroFunc("playback: this->playback_hw_params buffer size setted");

	dir = 0;
	if ((err = snd_pcm_hw_params_set_period_size(this->playback_handle, this->playback_hw_params, FRAMELEN, dir)) < 0)
	{
		macroFuncVargs("playback: cannot set period size (%s)",
					   snd_strerror(err));
		return -10;
	}
	macroFunc("playback: this->playback_hw_params period size setted");

	int val;
	snd_pcm_hw_params_get_buffer_size(this->playback_hw_params,
									  (snd_pcm_uframes_t *)&val);
	macroFuncVargs("playback: buffer size = %d frames", val);

	if ((err = snd_pcm_hw_params(this->playback_handle, this->playback_hw_params)) < 0)
	{
		macroFuncVargs("playback: cannot set parameters (%s)",
					   snd_strerror(err));
		return -11;
	}

	macroFunc("playback: this->playback_hw_params setted");

	snd_pcm_hw_params_free(this->playback_hw_params);
	this->playback_hw_params = NULL;

	macroFunc("playback: this->playback_hw_params freed");

	if ((err = snd_pcm_prepare(this->playback_handle)) < 0)
	{
		macroFuncVargs("playback: cannot prepare audio interface for use (%s)",
					   snd_strerror(err));
		return -12;
	}

	macroFunc("playback: audio interface prepared");
	return 0;
}

void PlayBackAudio::run()
{
	if (!this->isRun)
	{
		pthread_create(&this->th_capture, NULL, playback_process, (void *)this);
	}
}

void PlayBackAudio::stop()
{
	this->isRun = false;
}

void PlayBackAudio::start()
{
	this->isRun = true;
}
/**
 * @description: signal the playback_thread to read audio file and playback.
 * @param {type} 
 * @return: 
 */
void PlayBackAudio::onWakeup(WakeupEvent *wuEvent)
{
	wakeupEvent->setPbFile(wuEvent->getPbFile());
	macroFuncVargs("ATTENTION: file name:%s", wakeupEvent->getPbFile().c_str());

	pthread_cond_signal(&this->m_Cond);
}

void *PlayBackAudio::playback_process(void *p)
{
	// pthread_detach(pthread_self());
	PlayBackAudio *pb = (PlayBackAudio *)p;
	int err = 0, buffer_frames = FRAMELEN, ret = 0;
	int time_delay = 12000, err_count = 0;
	short *inputMicData16s = (short *)malloc(sizeof(short) * FRAMELEN * pb->channel);
	string pcmFile;
	FILE *fpr = NULL;
#ifdef DUMP_PCM_DATA
	FILE *dumpReadOutput = NULL;
	if ((dumpReadOutput = fopen("./dump_raw.pcm", "wb")) == NULL)
	{
		macroFunc("wakeup: cannot open dump_raw file for wakeup");
		exit(1);
	}
#endif
	macroFunc("playback_process start. ");

	while (1)
	{
		pb->stop();
		pthread_mutex_lock(&pb->m_Mutex);
		pthread_cond_wait(&pb->m_Cond, &pb->m_Mutex);
		pthread_mutex_unlock(&pb->m_Mutex);

		string playBackFile = pb->wakeupEvent->getPbFile();
		if (!playBackFile.empty())
		{
			if (playBackFile.rfind(".mp3") != std::string::npos)
			{
				// string cmd = ("mpg123  " + playBackFile);
				pcmFile = "/home/pi/test.pcm";
				string cmd = "sudo ffmpeg  -i " + playBackFile + " -acodec pcm_s16le -f s16le -ac 1 -ar 16000  " + pcmFile + "  >/dev/null"; // ffmpeg -i ttsVoiceFile.mp3  -f s16be -ar 16000 -ac 1 -acodec pcm_s16le pcm16k.pcm
				macroFuncVargs("ATTENTION: cmd:%s", cmd.c_str());
				system(cmd.c_str());
			}
		}
		else
		{
			continue;
		}

		if (!fileIsExist(pcmFile))
		{
			macroFuncVargs("ERROR: file does not exist.");
			continue;
		}

		fpr = fopen(pcmFile.c_str(), "rb");
		if (fpr == NULL)
		{
			printf("ERROR: failed to fopen:%s", pcmFile.c_str());
			fpr = NULL;
			exit(1);
		}

		while (pb->isRun)
		{
			if (fread(inputMicData16s, sizeof(short), buffer_frames, fpr) <= 0)
			{
				macroFuncVargs("WARNING: fread is finished,reason:%s", strerror(errno));
				break;
			}

			err = snd_pcm_writei(pb->playback_handle, inputMicData16s, buffer_frames);
			if (err != buffer_frames)
			{
				if (err < 0)
				{
					macroFuncVargs("playback: snd_pcm_recover err=%d, %s.", err, snd_strerror(err));
					err = snd_pcm_recover(pb->playback_handle, err, 0);
					macroFuncVargs("playback: after snd_pcm_recover err=%d, %s.", err, snd_strerror(err));
				}
				if (-EPIPE == err)
				{
					macroFuncVargs("playback: beamforming doing snd_pcm_writei err=%d, %s.", err, snd_strerror(err));
					if ((err = snd_pcm_prepare(pb->playback_handle)) < 0)
					{
						macroFuncVargs("playback: beamforming cannot prepare audio interface for use (%s)",
									   snd_strerror(err));
						return (void *)-20;
					}
					macroFunc("playback: beamforming audio interface prepared");
				}
				else if (-EAGAIN == err)
				{
					macroFuncVargs("playback: beamforming doing snd_pcm_writei err=%d, %s.", err, snd_strerror(err));
					continue;
				}
				else if (err > 0 && err < buffer_frames)
				{
					macroFuncVargs("PCM state = %s,state num= %d", snd_pcm_state_name(snd_pcm_state(pb->playback_handle)), snd_pcm_state(pb->playback_handle));
					macroFuncVargs("playback: beamforming Short write (expected %d, wrote %d)", buffer_frames, err);

					if (SND_PCM_STATE_PREPARED == snd_pcm_state(pb->playback_handle))
					{
						macroFunc("playback: snd_pcm_start operation.");
						if (snd_pcm_start(pb->playback_handle) < 0)
						{
							macroFuncVargs("playback: failed to snd_pcm_start (%s)", snd_strerror(err));
							macroFuncVargs("PCM state = %s,state num= %d", snd_pcm_state_name(snd_pcm_state(pb->playback_handle)), snd_pcm_state(pb->playback_handle));
							// return (void *)-21;
							if (snd_pcm_wait(pb->playback_handle, time_delay) == 0)
							{
								macroFunc("@@  wait time out ");
							}
							else
							{
								macroFunc("@@  deserve to wait. ");
							}
							macroFuncVargs("playback: beamforming snd_pcm_wait = %d", time_delay);
						}
						// if ((err = snd_pcm_prepare(param->playback_handle)) < 0)
						// {
						// 	macroFuncVargs("playback: beamforming cannot prepare audio interface for use (%s)", snd_strerror(err));
						// 	// return (void *)-21;
						// }
						// macroFunc("playback: beamforming audio interface prepared");
					}

					if (err_count++ > 100)
					{
						macroFunc("playback: return -22 and exit...........");
						err_count = 0;
						return (void *)-22;
					}
				}
				else if (err == 0)
				{
					macroFuncVargs("playback: beamforming something is error, err= %d, %s.", err, snd_strerror(err));
					macroFunc("playback: beamforming audio interface prepared");
				}
				else
				{
					macroFuncVargs("playback: beamforming something is error, err= %d, %s.", err, snd_strerror(err));
					if ((err = snd_pcm_prepare(pb->playback_handle)) < 0)
					{
						macroFuncVargs("playback: beamforming cannot prepare audio interface for use (%s)", snd_strerror(err));
						return (void *)-23;
					}
					macroFunc("playback: beamforming audio interface prepared");
					return (void *)-24;
				}
			}
#ifdef DUMP_PCM_DATA
			fwrite(inputMicData16s, sizeof(short), PlayBackAudio::FRAMELEN * PlayBackAudio->getChannel(), dumpReadOutput);
			fflush(dumpReadOutput);
#endif
		}

		if (fpr)
		{
			free(fpr);
			fpr = NULL;
		}
		deleteFile(pcmFile);
	}
	if (NULL != inputMicData16s)
	{
		free(inputMicData16s);
		inputMicData16s = NULL;
	}

	if (NULL != pb->playback_hw_params)
	{
		snd_pcm_hw_params_free(pb->playback_hw_params);
		pb->playback_hw_params = NULL;
	}
	if (NULL != pb->playback_handle)
	{
		snd_pcm_drain(pb->playback_handle);
		snd_pcm_close(pb->playback_handle);
	}
	if (NULL != pb->sArray)
	{
		free(pb->sArray);
		pb->sArray = NULL;
	}

	return NULL;
}
