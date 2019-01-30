@echo off
javac -d . -source 1.7 -target 1.7 Wakeup.java
javah -classpath . -jni com.twirling.audio.Wakeup
jar cvf TwirlingWakeUp.jar com
pause