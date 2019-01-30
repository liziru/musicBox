@echo off
rmdir libs obj /s /q
echo Building static arm64-v8a
CALL ndk-build NDK_PROJECT_PATH=. NDK_APPLICATION_MK=Application_a_arm64_v8a.mk
echo Building static armeabi-v7a
CALL ndk-build NDK_PROJECT_PATH=. NDK_APPLICATION_MK=Application_a_armeabi_v7a.mk
echo Building dynamic arm64-v8a
CALL ndk-build NDK_PROJECT_PATH=. NDK_APPLICATION_MK=Application_so_arm64_v8a.mk
echo Builing dynamic armeabi-v7a
CALL ndk-build NDK_PROJECT_PATH=. NDK_APPLICATION_MK=Application_so_armeabi_v7a.mk

rmdir ..\..\bin\android\Release /s /q
rmdir ..\..\lib\android\Release /s /q

md ..\..\bin\android\Release 
md ..\..\lib\android\Release
md ..\..\bin\android\Release\arm64-v8a
md ..\..\bin\android\Release\armeabi-v7a
md ..\..\lib\android\Release\arm64-v8a
md ..\..\lib\android\Release\armeabi-v7a

copy obj\local\arm64-v8a\libTwirlingWakeUp.so  ..\..\bin\android\Release\arm64-v8a
copy obj\local\arm64-v8a\libTwirlingWakeUp.a   ..\..\lib\android\Release\arm64-v8a

copy obj\local\armeabi-v7a\libTwirlingWakeUp.so  ..\..\bin\android\Release\armeabi-v7a
copy obj\local\armeabi-v7a\libTwirlingWakeUp.a   ..\..\lib\android\Release\armeabi-v7a

CALL jar cvf TwirlingWakeUp.jar com
copy TwirlingWakeUp.jar ..\..\include\android
pause