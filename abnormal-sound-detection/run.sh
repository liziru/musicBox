#!/bin/bash
binPath=/home/pi/workspace/mozart
sounderBin=$binPath/mozart

# beamforming set on
# sounder device[hw:1] beamformingFlag[on/off] playbackVolume[0-100]

#$sounderBin hw:1 on 90 &
#$sounderBin hw:1 on 90 1>$binPath/stdio.log 2>$binPath/errio.log &

# beamforming set off
#$sounderBin hw:1 off 80 &

ps -fe|grep sounder |grep -v grep
if [ $? -ne 0 ]
then
	echo "start process....."

	# twirling mic
	# alsa param twirling mic
	#amixer cset numid=2 31
	#amixer cset numid=1 63
	#$sounderBin hw:1 4 16000 hw:0 1 16000 on on on -20 3 1>$binPath/stdio.log 2>$binPath/errio.log &

	# mic on board
	#echo "copy lib"
	#sudo cp libTwirlingCapture.so /usr/lib/
	#sudo cp libWakeup.so /usr/lib/

	amixer cset numid=17 off
	amixer cset numid=12 off
	amixer cset numid=4 7
	amixer cset numid=3 on
	amixer cset numid=2 31
	amixer cset numid=20 1
	amixer cset numid=7 7
	amixer cset numid=18 on
	amixer cset numid=13 off
	amixer cset numid=5 7
	amixer cset numid=8 7
	amixer cset numid=19 off
	amixer cset numid=14 on
	amixer cset numid=6 7
	amixer cset numid=9 4
	amixer cset numid=10 on
	amixer cset numid=1 63
	amixer cset numid=11 off
	amixer cset numid=15 off
	amixer cset numid=16 off
	$sounderBin 1>/tmp/allLog 

else
	echo "runing....."
fi
