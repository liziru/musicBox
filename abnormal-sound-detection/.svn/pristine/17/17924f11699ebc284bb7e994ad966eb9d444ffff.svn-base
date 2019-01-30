

int wakeup_flag,wakeup_flag1;
    	
	mWakeupLibProcess->WakeupRun(handleWakeupLibProcess, wakeupbuf, 256);

	wakeup_flag = mWakeupLibProcess->GetWakeupStatus(handleWakeupLibProcess);

	ALOGD("wakeup_flag = %d",wakeup_flag);
	ALOGD("wakeupstatus = %d",wakeupstatus);
	if ((wakeupstatus == 0)&&(wakeup_flag == 1))//从睡眠到唤醒
	{
		wakeupstatus = 1;
		ALOGD("huanxing");
		short *reStoreBuffer = (short *)buffer;

		for(int i=0;i<shorts_per_MIC/2;i++)
		{
			reStoreBuffer[i] = mic3[256+i];

		}
		return (shorts_per_MIC/2*2);
			
	}
	else if ((wakeupstatus == 1)&&(wakeup_flag == 0))//唤醒之后的命令识别
	{
		//wakeupstatus = 0;
		ALOGD("shibie");
		short *reStoreBuffer = (short *)buffer;
		for(int i=0;i<shorts_per_MIC;i++)
		{
			reStoreBuffer[i] = mic3[i];

		}
		return shorts_per_MIC*2;
	}
	else if ((wakeupstatus == 1)&&(wakeup_flag == 2))//一次唤醒并且命令识别结束
	{
		ALOGD("jieshu");
		wakeupstatus = 0;
		short *reStoreBuffer = (short *)buffer;
		for(int i=0;i<shorts_per_MIC;i++)
		{
			reStoreBuffer[i] = mic3[i];

		}
		return shorts_per_MIC*2;
	}

	
	
	
		mWakeupLibProcess->WakeupRun(handleWakeupLibProcess, wakeupbuf1, 256);

		wakeup_flag1 = mWakeupLibProcess->GetWakeupStatus(handleWakeupLibProcess);

	ALOGD("wakeupstatus1 = %d",wakeupstatus);
		ALOGD("wakeup_flag1 = %d",wakeup_flag1);
		if ((wakeupstatus == 0)&&(wakeup_flag1 == 1))//从睡眠到唤醒
		{
			ALOGD("huanxing1");
			wakeupstatus = 1;
		}
		return 0;

		
