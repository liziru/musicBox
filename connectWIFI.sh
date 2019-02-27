#! /bin/bash

#author liping
#date 19.02.18
#version v1.0

if [ $# -lt 3 ];then
    echo "argument is short."
    exit 1
fi

ssid=`$1`
pwd=`$2`
echo -e "ssid:$ssid  pwd:$pwd"

echo -e "查看网络设备列表"
nmcli dev
echo -e "turn on wifi"
nmcli r wifi on
echo -e "扫描附近的 WiFi 热点"
mcli dev wifi
echo -e "connect wifi"
nmcli dev wifi connect "$ssid" password "$pwd" ifname wlan0
[ $? -ne 0 ] && echo -e "  ERROR." && exit 1
echo -e "## Connected Successfully."
