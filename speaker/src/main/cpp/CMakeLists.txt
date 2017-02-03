#
# Copyright (C) 2017 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.4.1)

add_library(speaker SHARED
            speaker.cpp)

target_include_directories(speaker PRIVATE
                           ${ANDROID_NDK}/sources/android/native_app_glue
                           ${LIBANDROIDTHINGS_DIR}/${ANDROID_ABI}/include)

add_library(android-system-properties STATIC
            AndroidSystemProperties.cpp)
target_include_directories(android-system-properties PRIVATE
                           ${ANDROID_NDK}/sources/android/native_app_glue
                           ${LIBANDROIDTHINGS_DIR}/${ANDROID_ABI}/include)

add_library(native-app-glue STATIC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

add_library(androidthings SHARED
            IMPORTED)
set_target_properties(androidthings
                      PROPERTIES IMPORTED_LOCATION
                      ${LIBANDROIDTHINGS_DIR}/${ANDROID_ABI}/lib/libandroidthings.so)

target_link_libraries(speaker
                      android
                      log
                      native-app-glue
                      androidthings
                      android-system-properties)
