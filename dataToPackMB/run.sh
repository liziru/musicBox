#! /bin/bash
'''
@File    :   run.sh
@Time    :   2019/01/31 11:29:23
@Author  :   LiPingping 
@Version :   1.0
@Contact :   youmakemewhole@163.com
@License :   (C)Copyright 2017-2018, Twirling in time
@Desc    :   None
'''
# here is main shell code.
binPath=/root
sounderBin=$binPath/mozart

ps -fe|grep sounder |grep -v grep
if [ $? -ne 0 ]
then
	echo "start process....."
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
	$sounderBin

else
	echo "running....."
fi
