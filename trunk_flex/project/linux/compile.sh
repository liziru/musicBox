#!/bin/bash
#export PATH=/home/twirling/program/cross_compile/junzheng/gcc/mipsel-gcc472-glibc216-mips32/bin:$PATH
#export PATH=/root/CodeSourcery/Sourcery_CodeBench_Lite_for_Xilinx_GNU_Linux/bin:$PATH
echo "create lib.so" `date`
#arm-xilinx-linux-gnueabi-g++ \
aarch64-linux-gnu-c++ \
	../src/aec/aec.cpp \
	../src/aec/res.cpp \
	../src/ambisonic/hxvxfromxxxx/h1v1fromobject.cpp \
	../src/ambisonic/hxvxfromxxxx/h1v1fromwyzx.cpp \
	../src/ambisonic/hxvxfromxxxx/hxvxfromxxxx.cpp \
	../src/ambisonic/ambisonic.cpp \
	../src/ans/ans.cpp \
	../src/ans/beamforming.cpp \
	../src/binaural/binaural.cpp \
	../src/core/core.cpp \
	../src/core/micarray_core.cpp \
	../src/doa/localizer.cpp \
	../src/dynamicrangecontrol/dynamicRangeControl.cpp \
	../src/equalizer/equalizer.cpp \
	../src/fft/realFFT.cpp \
	../src/resample/resample.cpp \
	../src/sn3d/sn3d.cpp \
	../src/upmix51/upmix51.cpp \
	../src/vad/vad.cpp \
	../src/api/TwirlingCapture.cpp \
	../src/automaticgaincontrol/automaticGainControl.cpp \
	../src/stereo/stereo.cpp \
	../src/bss/bss.cpp \
	../src/ans/inverse.cpp \
	-fPIC \
	-lpthread \
 	-shared \
	-o ./libTwirlingCapture.so
cp ./libTwirlingCapture.so ../../../out/lib/

#sleep 2
#libTwirlingCaptureW.so wakeup           libTwirlingCaptureNW.so nowakeup  详见note
#echo 'create .o' `date`
#/root/CodeSourcery/Sourcery_CodeBench_Lite_for_Xilinx_GNU_Linux/bin/arm-xilinx-linux-gnueabi-g++ -o TwirlingCaptureW main_linux_micarray_twirling.cpp -L. -lTwirlingCaptureW -lpthread





#if use g++, can test
#sleep 2
#echo '+++++++++++test++++++++++++++'
#export LD_LIBRARY_PATH=./


#./TwirlingCaptureW outW.pcm test_6mic.pcm spk.pcm
