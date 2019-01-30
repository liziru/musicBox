/*
 * Copyright 2014 Google Inc. All Rights Reserved.

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.twirling.audio;

/**
 * Contains vertex, normal and color data.
 */
public class TwirlingWakeUp {

    public native long WakeupInit();
    public native boolean GetWakeupStatus(long obj);
    public native void WakeupRun(long obj, short datain[], int nlen);
    public native void WakeupRelease(long obj);

    static {
        System.loadLibrary("TwirlingWakeUp");
    }
}
