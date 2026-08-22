# SDL2 跨平台头文件管理方案

## 背景

SDL2 从 2.0.22+ 版本开始，`SDL_config.h` 由 CMake 构建时生成，内容为目标平台的硬编码配置（如 `#define SDL_VIDEO_DRIVER_WINDOWS 1`）。这导致不同平台编译产出的 `SDL_config.h` 不能互用。

早期版本（2.0.7 等）的 `SDL_config.h` 使用编译器预定义宏（`_WIN32`、`__ANDROID__`）自动分流，一套头文件可跨平台共用。新版本不再如此。

## 解决方案：桥接 SDL_config.h

### 核心思路

SDL2 所有头文件中，**只有 `SDL_config.h` 是平台相关的**，其余头文件（`SDL.h`、`SDL_video.h`、`SDL_syswm.h` 等）本身是平台无关的——它们根据 `SDL_config.h` 中的宏走条件编译分支。

因此：
1. 将各平台编译产出的 `SDL_config.h` 重命名为 `SDL_config_<platform>.h`
2. 自己写一个桥接 `SDL_config.h`，用编译器预定义宏自动选择正确的平台配置
3. 其余所有头文件保持通用，不做任何修改

### 目录结构

```
dependencies/sdl2/include/SDL2/
├── SDL.h                      # 通用（来自 SDL 源码 include/）
├── SDL_video.h                # 通用
├── SDL_syswm.h                # 通用
├── SDL_config.h               # 自己写的桥接文件（见下方）
├── SDL_config_windows.h       # Windows 编译产出，重命名
├── SDL_config_android.h       # Android 编译产出，重命名
├── SDL_config_macosx.h        # macOS 编译产出，重命名（如需要）
├── SDL_config_linux.h         # Linux 编译产出，重命名（如需要）
├── SDL_config_ios.h           # iOS 编译产出，重命名（如需要）
└── ...                        # 其余 SDL 头文件原封不动
```

### 桥接 SDL_config.h 内容

```c
/*
 * SDL_config.h - Platform bridge
 * 
 * This file replaces the CMake-generated SDL_config.h with a platform
 * dispatcher that selects the correct platform-specific config at compile time.
 * 
 * When upgrading SDL, do NOT overwrite this file. Instead, rename the
 * generated SDL_config.h to SDL_config_<platform>.h.
 */
#ifndef SDL_config_h_
#define SDL_config_h_

#ifdef __ANDROID__
#include "SDL_config_android.h"
#elif defined(_WIN32)
#include "SDL_config_windows.h"
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
    #include "SDL_config_ios.h"
    #else
    #include "SDL_config_macosx.h"
    #endif
#elif defined(__linux__)
#include "SDL_config_linux.h"
#else
#error "Unsupported platform for SDL2"
#endif

#endif /* SDL_config_h_ */
```

### CMakeLists.txt

无需任何平台判断，统一一个 include 路径：

```cmake
target_include_directories(T3DPlatform PRIVATE
    ${CMAKE_SOURCE_DIR}/dependencies/sdl2/include/SDL2
)
```

## SDL2 版本升级流程

当需要升级 SDL2 版本时，按以下步骤操作：

### 1. 获取 SDL2 源码

```bash
# 下载新版本源码
wget https://github.com/libsdl-org/SDL/releases/download/release-X.Y.Z/SDL2-X.Y.Z.tar.gz
tar xzf SDL2-X.Y.Z.tar.gz
```

### 2. 为各平台编译

**Windows (MSVC)：**
```bash
cmake -S SDL2-X.Y.Z -B build_win -G "Visual Studio 17 2022" -A x64
cmake --build build_win --config Release
# 产出在: build_win/include/SDL2/SDL_config.h
```

**Android (NDK)：**
```bash
cmake -S SDL2-X.Y.Z -B build_android \
    -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=26
cmake --build build_android
# 产出在: build_android/include/SDL2/SDL_config.h
```

**macOS（如需要）：**
```bash
cmake -S SDL2-X.Y.Z -B build_macos
cmake --build build_macos
```

**iOS（如需要）：**
```bash
cmake -S SDL2-X.Y.Z -B build_ios \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build_ios
```

### 3. 更新头文件

```bash
# 备份桥接文件
cp dependencies/sdl2/include/SDL2/SDL_config.h /tmp/SDL_config_bridge.h

# 用源码头文件覆盖（不含 SDL_config.h）
cp SDL2-X.Y.Z/include/*.h dependencies/sdl2/include/SDL2/
# 注意：这会覆盖桥接文件，需要恢复

# 恢复桥接文件
cp /tmp/SDL_config_bridge.h dependencies/sdl2/include/SDL2/SDL_config.h

# 拷贝各平台配置（重命名）
cp build_win/include/SDL2/SDL_config.h     dependencies/sdl2/include/SDL2/SDL_config_windows.h
cp build_android/include/SDL2/SDL_config.h dependencies/sdl2/include/SDL2/SDL_config_android.h
cp build_macos/include/SDL2/SDL_config.h   dependencies/sdl2/include/SDL2/SDL_config_macosx.h   # 如需要
cp build_ios/include/SDL2/SDL_config.h     dependencies/sdl2/include/SDL2/SDL_config_ios.h      # 如需要
```

### 4. 更新库文件

```bash
cp build_win/Release/SDL2.lib              dependencies/sdl2/lib/windows/SDL2.lib
cp build_win/Release/SDL2.dll              dependencies/sdl2/lib/windows/SDL2.dll
cp build_android/libSDL2.so               dependencies/sdl2/lib/android/arm64-v8a/libSDL2.so
```

### 5. 验证

- Windows 编译通过
- Android 编译通过（确认 `SDL_syswm.h` 不再尝试 include `<windows.h>`）

## 注意事项

1. **桥接文件永远不要被覆盖** — 升级时注意跳过或恢复
2. **SDL_config_*.h 文件名是自定义的** — SDL 内部不会 include 这些名字，只有桥接文件引用它们
3. **SDL 源码的 `include/SDL_config_android.h` 是预设文件**，不等于编译产出。建议使用 CMake 编译产出的版本，因为它经过了真实的平台能力探测
4. **如果只需要 Windows + Android**，可以只维护这两个平台的 config 文件
