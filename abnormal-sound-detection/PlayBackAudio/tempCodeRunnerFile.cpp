{
			// read from audio interface failed -77, (File descriptor in bad state)
			macroFuncVargs("capture: read from audio interface failed %d, (%s)",
						   err, snd_strerror(err));
			if (-EPIPE == err || err == -77)
			{
				if ((err = snd_pcm_prepare(PlayBackAudio->playback_handle)) < 0)
				{
					macroFuncVargs("capture: cannot prepare audio interface for use (%s)",
								   snd_strerror(err));
					break;
				}