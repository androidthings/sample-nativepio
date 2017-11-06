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

const char* TAG = "speaker";
const double NOTE_A4_FREQUENCY = 440.0;
const double FREQUENCY_INC_PER_MS = 0.1;

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define ASSERT(cond, ...) if (!(cond)) { __android_log_assert(#cond, TAG, __VA_ARGS__);}

int64_t millis() {
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec*1000 + llround(now.tv_nsec / 1e6);
}

void android_main(android_app* app) {
    app_dummy(); // prevent native-app-glue to be stripped.

    AndroidSystemProperties systemProperties(app->activity);
    const char* SPEAKER_PWM;
    if (systemProperties.getBuildDevice() == "rpi3") {
        SPEAKER_PWM = "PWM1";
    } else if (systemProperties.getBuildDevice() == "imx6ul_pico") {
        SPEAKER_PWM = "PWM8";
    } else if (systemProperties.getBuildDevice() == "imx7d_pico") {
        SPEAKER_PWM = "PWM2";
    } else {
        LOGE("unsupported device: %s", systemProperties.getBuildDevice().c_str());
        return;
    }

    APeripheralManagerClient* client = APeripheralManagerClient_new();
    ASSERT(client, "failed to open peripheral manager client");
    APwm* pwm;
    int openResult = APeripheralManagerClient_openPwm(client, SPEAKER_PWM, &pwm);
    ASSERT(openResult == 0, "failed to open PWM: %s", SPEAKER_PWM);
    int setDutyCycleResult = APwm_setDutyCycle(pwm, 50.0);
    ASSERT(setDutyCycleResult == 0, "failed to set PWM duty cycle: %s", SPEAKER_PWM);
    int enablePwmResult = APwm_setEnabled(pwm, 1);
    ASSERT(enablePwmResult == 0, "failed to enable PWM: %s", SPEAKER_PWM);

    double frequency = NOTE_A4_FREQUENCY;
    int64_t lastMs = millis();
    while (!app->destroyRequested) {
        android_poll_source* source;
        int pollResult = ALooper_pollOnce(0, NULL, NULL, (void**)&source);
        if (pollResult >= 0) {
            if (source != NULL) {
                // forward event to native-app-glue to handle lifecycle and input event
                // and update `app` state.
                source->process(app, source);
            }
        }
        if (APwm_setFrequencyHz(pwm, frequency) != 0) {
            continue; // retry
        }
        int64_t now = millis();
        int64_t elapsedMs = now - lastMs;
        frequency += FREQUENCY_INC_PER_MS * elapsedMs;
        if (frequency > NOTE_A4_FREQUENCY * 2) {
            frequency = NOTE_A4_FREQUENCY;
        }
        lastMs = now;
    }
    APwm_setEnabled(pwm, 0);
    APwm_delete(pwm);
    APeripheralManagerClient_delete(client);
}
