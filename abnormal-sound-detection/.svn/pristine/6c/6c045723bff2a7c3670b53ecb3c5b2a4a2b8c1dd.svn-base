@echo off
javac -d . -source 1.7 -target 1.7 AudioCapture.java
javah -classpath . -jni com.twirling.audio.AudioCapture
jar cvf TwirlingAudioCapture.jar com
pause