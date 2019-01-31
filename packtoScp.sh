#! /bin/bash
'''
@File    :   pack.sh
@Time    :   2018/12/22 18:09:16
@Author  :   LiPingping 
@Version :   1.0
@Contact :   youmakemewhole@163.com
@License :   (C)Copyright 2017-2018, Twirling in time
@Desc    :   None
'''
# here is main shell code.
echo -e "Current dir: $HOME/Desktop/musicBox"
cd $HOME/Desktop/musicBox
make clean; make

[ ! -d dataToPackMB ] && mkdir -p dataToPackMB
echo "data needed: setup.sh run.sh libTwirlingCapture.so sounder"
cp out/lib/libTwirlingCapture.so out/lib/libTwirlingWakeupNN.so out/lib/libwiringPi.so out/lib/liblog4cpp.so run.sh mozart dataToPackMB

tar czvf tcpMBdatapackage.tar.gz dataToPackMB/

iphead="192.168"
echo -e "input the tail of destIP: 192.168."
read iptail

ip=`echo $iphead.$iptail`
echo "send package to $ip"

scp tcpMBdatapackage.tar.gz ./configure.sh pi@$ip:~/
#echo $?
[ $? -eq 0 ] &&echo -e " ## pack and sending is finished." &&exit 0
echo -e "  failed to scp ."