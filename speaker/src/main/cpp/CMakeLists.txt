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

# Add FindAndroidThings.cmake dir to the module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_DIR}/libandroidthings)
# Resolve Android Things headers and libraries
find_package(AndroidThings REQUIRED)

add_library(speaker SHARED
            speaker.cpp)

target_include_directories(speaker PRIVATE
                           ${PROJECT_DIR}/android-system-properties
                           ${ANDROID_NDK}/sources/android/native_app_glue
                           ${ANDROIDTHINGS_INCLUDE_DIRS})

add_library(android-system-properties STATIC
            ${PROJECT_DIR}/android-system-properties/AndroidSystemProperties.cpp)
target_include_directories(android-system-properties PRIVATE
                           ${ANDROID_NDK}/sources/android/native_app_glue
                           ${ANDROIDTHINGS_INCLUDE_DIRS})

add_library(native-app-glue STATIC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

add_library(androidthings SHARED
            IMPORTED)
set_target_properties(androidthings
                      PROPERTIES IMPORTED_LOCATION
                      ${ANDROIDTHINGS_LIBRARIES})

target_link_libraries(speaker
                      android
                      log
                      native-app-glue
                      androidthings
                      android-system-properties)
