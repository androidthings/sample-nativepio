# Simple example of Android Things Native Peripheral I/O APIs

This Android Things app runs basic code that exercises
the [Native PIO APIs][native-pio] from C++. Each sample is an Android
module that can be run independently.

## Pre-requisites

- Android Things compatible board
- Android Studio 2.2+
- Android NDK bundle

For the Blink sample:
- 1 LED
- 1 resistor
- 2 jumper wires
- 1 breadboard

For the Button sample:
- 1 push button
- 1 resistor
- 2 jumper wires
- 1 breadboard

For the Speaker sample:
- 1 piezo buzzer
- 2 jumper wires
- 1 breadboard

## Build and install

[Download][releases] the latest Android Things native library release
and extract it in the project root directory.

It should contains the following directories:
```
libandroidthings/
  ${ABI}/
    include/
      pio/
        *.h
    lib/
      libandroidthings.so
```

On Android Studio, select the module in the select box by the "Run" button, and
then click on the "Run" button.

If you prefer to run on the command line, type

```bash
./gradlew <module>:installDebug
adb shell am start com.example.androidthings.nativepio/.<ModuleActivity>
```

## Sample Specifics

### Blink

![Schematics for Raspberry Pi 3](blink/rpi3_schematics.png)

```bash
    ./gradlew blink:installDebug
    adb shell am start com.example.androidthings.nativepio/android.app.NativeActivity
```

Blinks an LED connected to a GPIO pin.

### Button

![Schematics for Raspberry Pi 3](button/rpi3_schematics.png)

```bash
    ./gradlew button:installDebug
    adb shell am start com.example.androidthings.nativepio/android.app.NativeActivity
```

Logs to logcat when a button connected to a GPIO pin is pressed. Make sure you
use a pull-down or pull-up resistor to avoid fluctuation.

### Speaker

![Schematics for Raspberry Pi 3](speaker/rpi3_schematics.png)

```bash
    ./gradlew speaker:installDebug
    adb shell am start com.example.androidthings.nativepio/android.app.NativeActivity
```

Plays an annoying alarm sound on the PWM speaker. Stop it by turning off the
Raspberry Pi.

## License

Copyright 2016 The Android Open Source Project, Inc.

Licensed to the Apache Software Foundation (ASF) under one or more contributor
license agreements.  See the NOTICE file distributed with this work for
additional information regarding copyright ownership.  The ASF licenses this
file to you under the Apache License, Version 2.0 (the "License"); you may not
use this file except in compliance with the License.  You may obtain a copy of
the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
License for the specific language governing permissions and limitations under
the License.

[native-pio]: https://developer.android.com/things/sdk/pio/native.html
[releases]: https://github.com/androidthings/native-libandroidthings/releases
