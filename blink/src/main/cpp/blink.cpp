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
#include <android_native_app_glue.h>

#include <pio/gpio.h>
#include <pio/peripheral_manager_client.h>

#include "AndroidSystemProperties.h"

const char* TAG = "blink";
const int BLINK_INTERVAL_MS = 1000;

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define ASSERT(cond, ...) if (!(cond)) { __android_log_assert(#cond, TAG, __VA_ARGS__);}

void android_main(android_app* app) {
    app_dummy(); // prevent native-app-glue to be stripped.

    AndroidSystemProperties systemProperties(app->activity);
    const char* LED_GPIO;
    if (systemProperties.getBuildDevice() == "rpi3") {
        LED_GPIO = "BCM6";
    } else if (systemProperties.getBuildDevice() == "edison") {
        LED_GPIO = "IO13";
    } else if (systemProperties.getBuildDevice() == "imx7d_pico") {
        LED_GPIO = "GPIO2_IO02";
    } else {
        LOGE("unsupported device: %s", systemProperties.getBuildDevice().c_str());
        return;
    }

    APeripheralManagerClient* client = APeripheralManagerClient_new();
    ASSERT(client, "failed to open peripheral manager client");
    AGpio* gpio;
    int openResult = APeripheralManagerClient_openGpio(client, LED_GPIO, &gpio);
    ASSERT(openResult == 0, "failed to open GPIO: %s", LED_GPIO);
    int setDirectionResult = AGpio_setDirection(gpio, AGPIO_DIRECTION_OUT_INITIALLY_LOW);
    ASSERT(setDirectionResult == 0, "failed to set direction for GPIO: %s", LED_GPIO);

    while (!app->destroyRequested) {
        int gpioValue;
        if (AGpio_getValue(gpio, &gpioValue) != 0) {
            LOGE("failed to get value for GPIO: %s", LED_GPIO);
            continue; // retry immediately
        }
        if (AGpio_setValue(gpio, !gpioValue) != 0) {
            LOGE("failed to set value for GPIO: %s", LED_GPIO);
            continue; // retry immediately
        }
        android_poll_source* source;
        // wait for at most BLINK_INTERVAL_MS.
        // proper control loop implementation would also account for the time spent handling
        // lifecycle and input events.
        int pollResult = ALooper_pollOnce(BLINK_INTERVAL_MS, NULL, NULL, (void**)&source);
        if (pollResult >= 0) {
            if (source != NULL) {
                // forward event to native-app-glue to handle lifecycle and input event
                // and update `app` state.
                source->process(app, source);
            }
        }
    }
    AGpio_delete(gpio);
    APeripheralManagerClient_delete(client);
}
