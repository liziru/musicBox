LOCAL_PATH := $(call my-dir)  
include $(CLEAR_VARS)  

LOCAL_ARM_MODE  := arm

OPT_CFLAGS := -O3 -ffunction-sections -fdata-sections
OPT_LDFLAGS := -O3 -Wl,--gc-sections
LOCAL_CFLAGS := $(LOCAL_CFLAGS) $(OPT_CFLAGS)  
LOCAL_LDFLAGS := $(LOCAL_LDFLAGS) $(OPT_LDFLAGS)
LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -std=c++11
LOCAL_MODULE    := TwirlingCapture
LOCAL_SRC_FILES :=  \
../src/ambisonic/hxvxfromxxxx/hxvxfromxxxx.cpp \
../src/ambisonic/hxvxfromxxxx/h1v1fromobject.cpp \
../src/ambisonic/hxvxfromxxxx/h1v1fromwyzx.cpp \
../src/ambisonic/ambisonic.cpp \
../src/ans/ans.cpp \
../src/ans/inverse.cpp \
../src/ans/beamforming.cpp \
../src/api/TwirlingCapture.cpp \
../src/binaural/binaural.cpp \
../src/bss/bss.cpp \
../src/core/core.cpp \
../src/core/micarray_core.cpp \
../src/doa/localizer.cpp \
../src/dynamicrangecontrol/dynamicRangeControl.cpp \
../src/automaticgaincontrol/automaticGainControl.cpp \
../src/equalizer/equalizer.cpp \
../src/stereo/stereo.cpp \
../src/fft/realFFT.cpp \
../src/resample/resample.cpp \
../src/sn3d/sn3d.cpp \
../src/upmix51/upmix51.cpp \
../src/aec/aec.cpp \
../src/aec/res.cpp \
../src/vad/vad.cpp \
./com_twirling_audio_AudioCapture.cpp
include $(BUILD_SHARED_LIBRARY)
