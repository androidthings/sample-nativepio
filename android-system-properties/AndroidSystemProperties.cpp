/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "AndroidSystemProperties.h"

#include <jni.h>
#include <android/native_activity.h>

AndroidSystemProperties::AndroidSystemProperties(ANativeActivity* activity) {
    JNIEnv *jni;
    activity->vm->AttachCurrentThread(&jni, NULL);
    jclass buildClass = jni->FindClass("android/os/Build");
    jfieldID buildDeviceFieldID = jni->GetStaticFieldID(buildClass, "DEVICE",
                                                        "Ljava/lang/String;");
    jstring buildDeviceField = (jstring) jni->GetStaticObjectField(buildClass,
                                                                   buildDeviceFieldID);
    const char *buildDevice = jni->GetStringUTFChars(buildDeviceField, NULL);
    buildDevice_ = buildDevice;
    jni->ReleaseStringUTFChars(buildDeviceField, buildDevice);
    activity->vm->DetachCurrentThread();
}