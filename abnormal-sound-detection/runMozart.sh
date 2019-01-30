#! /bin/bash

echo -e "current time: `date`"
path=$HOME/workspace
targetBin=$path/mozart

currTime=$(date +%H)

starTime=21
endTime=6

#echo $targetBin $starTime $endTime $currTime 

ps ajx | grep mozart | grep -v grep 
if [ $? -ne 0 ]
then
    isMozartRun=0
else
    isMozartRun=1
fi

if [[ $currTime -ge $endTime && $currTime -le $starTime ]];then
    [ $isMozartRun -eq 1 ]&&sudo killall mozart
    echo "mozart should not run."
else
    echo "mozart starts to run."
    if [ $isMozartRun -eq 0 ];then
        sudo $targetBin &
    fi
fi
