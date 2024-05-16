# Environment & Build

## Windows

**Prerequisites**

- Windows 10
- Visual Studio 2019
- Windows SDK 10.0.19042
- Python 3

**Build**

Run the script **<generate-vs2019-x64-debug.bat>** in directory **<$(Root)/source/Projects>** for debug, and run the script **<generate-vs2019-x64.bat>** in the same directory for release.

## Mac OS X

**Prerequisites**

- Mac OS X Monterey (12.x.x)
- xcode 13

**Build**

Run the script **<generate-xcode-osx.sh>** in directory **<$(Root)/source/Projects>**

## iOS

**Prerequisites**

- Mac OS X Monterey (12.x.x)
- xcode 13

**Build**

Runt the script in **<generate-xcode-ios.sh>** in directory **<$(Root)/source/Projects>**

## Android

**Prerequisites**

- Android Studio Bumblebee 2021.1.1 Patch 3 Build #AI-211.7628.21.2111.8309675
- Android SDK API Level : 32
- NDK Version : 24.0.8215888

**Build**

There is no building script for Android. You should build the program with Android Studio directly. You could import the android project in directory **<$(Root)/source/Samples/[Demo]/Android>**.

## Linux

Unsupported right now.

## 