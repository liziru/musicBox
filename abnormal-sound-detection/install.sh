# 改密码
sudo passwd pi   #lxl
sudo passwd		#lxl

# 更新源
wget http://112.124.9.243/aptsouce.sh
chmod 755 aptsouce.sh
sudo -H ./aptsouce.sh
sudo apt-get update

# 启动ap
sudo turn-wifi-into-apmode yes
sudo turn-wifi-into-apmode yes

# 拷贝sounder程序
beyond@beyond:~/workspace/code/sounder$ ssh-copy-id pi@192.168.8.1
beyond@beyond:~/workspace/code/sounder$ make install

# 修改等待时间
sed '$,$s/5min/3sec/' /etc/systemd/system/network-online.target.wants/networking.service

# 安装sounder程序
mkdir -p /home/pi/workspace/alize
cd /home/pi/workspace/alize
tar xmf data.tar.gz
rm data.tar.gz
sudo cp ./lib* /usr/lib/
mv gmm/world.gmm .
rm gmm/*
mv world.gmm gmm/
echo "" > dataMap.txt
sudo cp run.sh /etc/init.d/
sudo update-rc.d run.sh defaults

sudo reboot