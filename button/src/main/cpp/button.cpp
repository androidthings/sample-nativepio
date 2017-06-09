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

#include <cmath>

#include <android/log.h>
#include <android/looper.h>
#include <android_native_app_glue.h>

#include <pio/gpio.h>
#include <pio/peripheral_manager_client.h>

#include "AndroidSystemProperties.h"

const char* TAG = "button";

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define ASSERT(cond, ...) if (!(cond)) { __android_log_assert(#cond, TAG, __VA_ARGS__);}

void android_main(android_app* app) {
    app_dummy(); // prevent native-app-glue to be stripped.

    AndroidSystemProperties systemProperties(app->activity);
    const char* BUTTON_GPIO;
    if (systemProperties.getBuildDevice() == "rpi3") {
        BUTTON_GPIO = "BCM21";
    } else if (systemProperties.getBuildDevice() == "edison") {
        BUTTON_GPIO = "IO12";
    } else if (systemProperties.getBuildDevice() == "imx7d_pico") {
        BUTTON_GPIO = "GPIO_174";
    }  else {
        LOGE("unsupported device: %s", systemProperties.getBuildDevice().c_str());
        return;
    }

    APeripheralManagerClient* client = APeripheralManagerClient_new();
    ASSERT(client, "failed to open peripheral manager client");
    AGpio* gpio;
    APeripheralManagerClient_openGpio(client, BUTTON_GPIO, &gpio);
    ASSERT(gpio, "failed to open GPIO: %s", BUTTON_GPIO);
    int setDirectionResult = AGpio_setDirection(gpio, AGPIO_DIRECTION_IN);
    ASSERT(setDirectionResult == 0, "failed to set direction for GPIO: %s", BUTTON_GPIO);
    int setEdgeTriggerResult = AGpio_setEdgeTriggerType(gpio, AGPIO_EDGE_FALLING);
    ASSERT(setEdgeTriggerResult == 0, "failed to edge trigger for GPIO: %s", BUTTON_GPIO);
    int fd;
    int getPollingFdResult = AGpio_getPollingFd(gpio, &fd);
    ASSERT(getPollingFdResult == 0, "failed to get polling file descriptor for GPIO: %s",
           BUTTON_GPIO);
    ALooper* looper = ALooper_forThread();
    ASSERT(looper, "failed to get looper for the current thread");
    int addFdResult = ALooper_addFd(looper, fd, LOOPER_ID_USER, ALOOPER_EVENT_INPUT, NULL, NULL);
    ASSERT(addFdResult > 0, "failed to add file description to looper");

    while (!app->destroyRequested) {
        android_poll_source* source;
        // wait indefinitly for an interrupt or a lifecycle event.
        int pollResult = ALooper_pollOnce(-1, NULL, NULL, (void**)&source);
        if (pollResult >= 0) {
            if (source != NULL) {
                // forward event to native-app-glue to handle lifecycle and input event
                // and update `app` state.
                source->process(app, source);
            }
            if (pollResult == LOOPER_ID_USER) {
                int ackInterruptResult = AGpio_ackInterruptEvent(fd);
                ASSERT(ackInterruptResult == 0, "failed to ack interrupt");
                LOGI("GPIO \"%s\" changed: button pressed", BUTTON_GPIO);
            }
        }
    }
    ALooper_removeFd(looper, fd);
    AGpio_delete(gpio);
    APeripheralManagerClient_delete(client);
}
