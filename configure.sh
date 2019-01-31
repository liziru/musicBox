#! /bin/bash
'''
@File    :   configure.sh
@Time    :   2018/12/22 18:06:15
@Author  :   LiPingping 
@Version :   1.0
@Contact :   youmakemewhole@163.com
@License :   (C)Copyright 2017-2018, Twirling in time
@Desc    :   configure.
'''
# here is main shell code.
tar xvf *.tar.gz
[ -f core ] && rm core

cd ./dataToPackMB

INSTALL_CMD1="cp ./libTwirlingCapture.so ./libTwirlingWakeupNN.so ./libwiringPi.so ./liblog4cpp.so  /usr/lib/"
INSTALL_CMD2="cp ./run.sh /etc/init.d/"
SET_START_CMD1="update-rc.d run.sh defaults"

echo "${INSTALL_CMD1}"
${INSTALL_CMD1} && sudo ldconfig
echo "${INSTALL_CMD2}"
${INSTALL_CMD2}
echo "cd /etc/init.d/ && ${SET_START_CMD1}"
cd /etc/init.d/ && ${SET_START_CMD1}

cd ~/dataToPackMB
cp ./mozart  /root
cp ./mozart ~
cd ..

#rm -rf *

echo "Configuration is complete. Restart now ? input y or n."
read isReboot
if [ isReboot == "y" ];then
    sudo reboot
else
    echo "reboot later ."
fi


