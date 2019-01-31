#!/bin/bash
cd ../../project/linux
./compile.sh
cd ../../bin/linux

cp ./libWakeup.so ../../../out/lib/
cp ../../project/src/pocketsphinx/include/TwirlingWakeupApi.h ../../../out/include/
rm -rf ../../project/linux/*.o


aarch64-linux-gnu-g++ -o wakeup main.cpp -L. -lWakeup

#echo "------------test TwirlingCaptureA--------------------"
#export LD_LIBRARY_PATH=./
#./wakeup andy.raw 1e-20 1e-4  #不需要拷贝libTwirlingCaptureA.a

