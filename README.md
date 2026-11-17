# Tiny3D 开发环境搭建指南

本文档用于帮助开发者快速完成 **Tiny3D** 引擎、编辑器及样例工程（Samples）的开发环境搭建与首次构建。

---

## 目录

- [1. 获取源码与基础准备](#1-获取源码与基础准备)
- [2. Windows 开发环境（推荐）](#2-windows-开发环境推荐)
- [3. Android 交叉编译环境](#3-android-交叉编译环境)
- [4. macOS / iOS 开发环境](#4-macos--ios-开发环境)
- [5. Linux 支持状态](#5-linux-支持状态)
- [6. 常用 CMake 选项与目录结构](#6-常用-cmake-选项与目录结构)
- [7. 常见问题排查](#7-常见问题排查)

---

## 1. 获取源码与基础准备

### 1.1 Git LFS

仓库中 `dependencies/` 下的预编译动态库/静态库（`*.dll`、`*.lib`、`*.dylib` 及部分 Linux `*.so`）通过 **Git LFS** 管理，克隆前请确保已安装 Git LFS：

```bash
git lfs install
git clone <repository-url>
```

如果此前未使用 LFS 克隆，可在仓库根目录补拉二进制依赖：

```bash
git lfs pull
```

> 仓库已自带第三方源码（`source/External/`）与预编译库（`dependencies/`，含 SDL2、LLVM/libclang、DXC、glslang、FBX SDK、FreeImage、GLAD、zlib 等），**无需手动编译第三方依赖**。

### 1.2 通用工具要求

| 工具 | 要求 | 用途 |
|------|------|------|
| **CMake** | 3.16 或更高版本（需加入 `PATH`） | 工程生成、原生反射配置（`ReflectionSettings.json`） |
| **Python 3** | 3.x（需加入 `PATH`） | 构建编辑器/启动器多语言头文件 `Language.h` |

> **提示**：源码目录与构建目录路径中**不要包含空格**，否则部分工具链和 CMake 脚本可能出错。

---

## 2. Windows 开发环境（推荐）

Windows 是 Tiny3D 的主力开发平台，支持 **D3D11**、**OpenGL 4**、**Vulkan** 与 **Null** 渲染后端，以及 **TinyEditor** 桌面编辑器和全套离线工具链。

### 2.1 前置依赖安装

1. **Visual Studio（二选一或同时安装）**
   - **Visual Studio 2019**（v142 工具集）：对应 `generate-vs2019-x64-debug.bat`
   - **Visual Studio 2022**：对应 `generate-vs2022-x64-debug.bat`
   - 安装时勾选工作负载：**使用 C++ 的桌面开发**，以及 **Windows 10 SDK**（`10.0.17763` / `10.0.19042` 或更高版本）。

2. **Python 依赖（用于生成多语言文件）**
   - 运行 `source\Projects\setup_env.bat`，或手动执行：
     ```cmd
     python -m pip install openpyxl pandas
     ```

3. **Vulkan SDK 1.1+（启用 Vulkan 渲染后端需要）**
   - 从 [LunarG Vulkan SDK](https://vulkan.lunarg.com/sdk/home) 下载并安装 Windows 版本（默认路径通常为 `C:\VulkanSDK\<version>`）。
   - 安装程序会自动设置 `VULKAN_SDK` 环境变量。打开新终端验证：
     ```cmd
     echo %VULKAN_SDK%
     ```
     并确认 `%VULKAN_SDK%\Include\vulkan\vulkan.h` 与 `%VULKAN_SDK%\Lib\vulkan-1.lib` 存在。
   - *注：如果暂未安装 Vulkan SDK，CMake 会自动跳过 `VKRenderer` 目标，D3D11 / OpenGL4 / Null 后端仍可正常构建。*

4. **Windows Graphics Tools（调试 D3D11 必装）**
   - 以**管理员身份**打开 PowerShell，执行：
     ```powershell
     Add-WindowsCapability -Online -Name "Graphics.Tools~~~~0.0.1.0"
     ```
   - *说明：Debug 模式下启动引擎/样例会启用 D3D11 Debug Layer；未安装该组件时启动可能触发异常。*

### 2.2 生成工程与构建

所有工程生成脚本均位于 `source\Projects\` 目录：

#### 方式 A：生成 Visual Studio 工程并在 IDE 中开发（推荐）

- **VS 2019 Debug**：双击或运行 `source\Projects\generate-vs2019-x64-debug.bat`
- **VS 2022 Debug**：双击或运行 `source\Projects\generate-vs2022-x64-debug.bat`
- **VS 2019 Release**：运行 `source\Projects\generate-vs2019-x64.bat`

脚本会自动完成以下工作：
1. 调用 `setup-msvc-env.bat` 初始化并缓存 MSVC 编译环境（供内置 `rpp` 反射解析使用，**无需手动从 Developer Command Prompt 启动**）；
2. 清理旧构建目录并生成 `source\vs2019-x64\Tiny3D.sln`（或 `source\vs2022-x64\Tiny3D.sln`）；
3. 自动打开 Visual Studio 解决方案。

> **关于 MSVC 环境缓存**：首次运行会将 `vcvarsall.bat` 环境变量缓存至 `source\Projects\.msvc-env-*.cmd` 以加速后续生成。如果本机升级或重装了 Visual Studio，可删除该缓存文件，或执行 `setup-msvc-env.bat <版本范围> [toolset] -refresh` 重建；如需手动打开带 VC 环境的命令行，可运行 `source\Projects\start_vsenv.bat`。

#### 方式 B：命令行直接全量构建

```cmd
cd source\Projects
build-vs2019-x64-debug.bat
```

该脚本会完成 configure 并执行 `cmake --build . --config Debug` 编译全部引擎库、编辑器、工具与 Samples，完成后打开工程。

> **为什么构建脚本未开启 MSBuild 多项目并行（`-m`）**：`ResourceApp` 与 `SkyboxApp` 的 `POST_BUILD` 都会调用 `BundleBuilder` 打包并拷贝同一份 `assets/samples/bundle`，多项目并行构建时可能相互冲突报 `MSB3073`。在 VS IDE 内单独编译或运行某个 Sample / `TinyEditor` 无此影响。

#### 方式 C：仅刷新已有工程（不清空构建目录）

当增删源文件或调整 CMake 配置后，可运行 `source\Projects\refresh-vs2019-x64.bat` 保留缓存重新 configure。

### 2.3 运行与调试

- 构建产物默认输出至：`source\bin\Windows\Debug\`（或 `Release\`）。
- 在 Visual Studio 中将任一目标设为**启动项目**并按 `F5` 运行，例如：
  - 编辑器：`TinyEditor` / `TinyLauncher`
  - 示例程序：`HelloApp`、`GeometryApp`、`LightApp`、`ShadowApp`、`ComputeApp`、`RmlUiApp` 等

---

## 3. Android 交叉编译环境

Android 端使用 Android Studio + Gradle + NDK CMake 构建，默认启用 **OpenGL ES 3**、**OpenGL ES 2** 与 **Vulkan** 渲染后端。

### 3.1 前置依赖

以当前工程配置（`source/Samples/*/Android/`、`source/Platform/Android/`）为准：

| 组件 | 要求版本 |
|------|----------|
| **宿主机（Host）** | Windows（Gradle 会调用本机 VS 2022 构建 Host 端反射工具 `rpp.exe`） |
| **Visual Studio（Host 工具）** | Visual Studio 2022（C++ x64 工具链，供 `tiny3d-build-host-rpp.gradle` 编译 `rpp.exe`） |
| **Android Studio** | 支持 AGP `8.5.0` / Gradle `8.9` 的版本（建议使用较新版 Android Studio，Gradle JDK 17+） |
| **Android SDK** | `compileSdk 33`、`targetSdk 33`、`minSdk 26`、`Build-Tools 33.0.2` |
| **Android NDK** | `24.0.8215888`（对应 NDK 自带 CMake 3.18.1） |
| **支持 ABI** | `arm64-v8a`、`x86_64`（SDL2 预编译库已内置于 `dependencies/sdl2/prebuilt/android/`） |

### 3.2 构建与运行

1. 使用 Android Studio 直接打开对应目录（无需手动跑 CMake）：
   - 运行样例：`source/Samples/<AppName>/Android`（如 `source/Samples/HelloApp/Android`、`source/Samples/ComputeApp/Android`）
   - 仅编引擎库：`source/Platform/Android`
2. 首次 Gradle Sync / Build 时，`source/Platform/Android/tiny3d-build-host-rpp.gradle` 会自动先在 Windows 宿主机构建 `source/bin/Windows/<Config>/rpp.exe`，随后触发 NDK CMake 完成交叉编译并打包 APK。
3. 连接真机或启动模拟器直接 Run 即可。

### 3.3 清理 Android 缓存

切换分支或更新了 CMake 反射/工具链配置后，如遇旧 `.cxx` 缓存导致头文件探测异常：
- 仅重置 CMake/NDK 缓存（推荐先试）：运行 `source\Projects\clean-android-cxx.bat`
- 清理全部 Android 构建产物：运行 `source\Projects\clean-android.bat`（会自动保护受 Git 跟踪的文件不被误删）

---

## 4. macOS / iOS 开发环境

### 4.1 macOS

- **前置要求**：macOS（建议 13.0+）、Xcode 13+、CMake、Python 3。
- **架构说明**：仓库内预编译的第三方库（如 FreeImage、FBX SDK）目前仅提供 `x86_64`，因此默认以 `x86_64` 架构构建（Apple Silicon Mac 可通过 Rosetta 2 编译和运行；CMake 已自动忽略 `/opt/homebrew` 以避免架构混链）。默认启用 **Metal** 渲染后端。
- **生成工程**：
  ```bash
  cd source/Projects
  ./generate-xcode-osx.sh
  ```
  工程将生成在 `source/xcode-osx/Tiny3D.xcodeproj` 并自动用 Xcode 打开；后续增量刷新可运行 `./refresh-xcode-osx.sh`。

> **提示**：主 `CMakeLists.txt` 已全面启用 CMake 原生反射配置（`TINY3D_REFLECT_CMAKE_NATIVE=ON`）。如果 `generate-xcode-osx.sh` 中的旧式 `cct`/`nmake` 步骤报错，可直接在 `source/xcode-osx` 目录执行原生 configure（参考 `refresh-xcode-osx.sh`）：
> ```bash
> mkdir -p source/xcode-osx && cd source/xcode-osx
> cmake -G Xcode -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 -DTINY3D_BUILD_RTTR_TOOL=OFF ../
> ```

### 4.2 iOS

- **前置要求**：macOS、Xcode 13+、CMake。
- **生成工程**（位于 `source/Projects/`）：
  - 真机（`arm64`）：`./generate-xcode-ios.sh`（输出至 `source/xcode-ios/`）
  - 模拟器（`x86_64`）：`./generate-xcode-simulator.sh`（输出至 `source/xcode-simulator/`）

---

## 5. Linux 支持状态

当前暂未提供完整支持（`source/Plugins/Renderer/CMakeLists.txt` 中 Linux 渲染后端默认未启用）。

---

## 6. 常用 CMake 选项与目录结构

### 6.1 常用 CMake 选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `TINY3D_BUILD_SAMPLES` | `ON`（桌面端） | 是否构建 `source/Samples/` 下的示例程序 |
| `TINY3D_BUILD_EDITOR` | `ON`（桌面端） | 是否构建 `TinyEditor`、`TinyLauncher` 与 `TinyPlayer` |
| `TINY3D_BUILD_TOOLS` | `ON`（桌面端） | 是否构建 `rpp`、`ShaderCrossCompiler`、`MeshConverter` 等工具 |
| `TINY3D_BUILD_RMLUI` | `ON`（桌面 / Android） | 是否构建 RmlUi 运行时 UI 插件与 `RmlUiApp` |
| `TINY3D_COPY_RESOURCE` | `OFF`（脚本默认传 `ON`） | 是否拷贝编辑器资源并生成多语言头文件 `Language.h` |
| `TINY3D_INCREMENTAL_RTTR` | `ON` | 是否在构建期运行 `rpp` 增量生成反射代码 |

### 6.2 核心目录速览

```text
Tiny3D/
├── assets/             # 引擎与样例资源（builtin、editor 模板、config、samples 等）
├── dependencies/       # 预编译第三方依赖（sdl2、llvm、dxc、glslang、fbxsdk、freeimage 等）
├── doc/                # 设计文档与 API 文档
└── source/
    ├── CMake/          # CMake 模块、Find 包脚本与原生反射/SDK 导出逻辑
    ├── Common/         # 公共头文件（ImGuiApp、SampleCommon 等）
    ├── Core/           # 引擎核心（Kernel、Component、Render、RHI、Resource、Serializer）
    ├── Editor/         # TinyEditor、TinyLauncher、TinyImGui
    ├── External/       # 参与源码编译的第三方库（rttr、protobuf、rapidjson、RmlUi、spirv-cross 等）
    ├── Math/           # 数学库
    ├── Platform/       # 跨平台抽象层（窗口、IO、线程、SharedLibrary、ZipAssetManager）
    ├── Player/         # 独立运行时宿主 TinyPlayer
    ├── Plugins/        # 渲染器（D3D11/GL4/GLES3/Vulkan/Metal/Null）、Archive、ImageCodec、RmlUi
    ├── Projects/       # 各平台一键生成 / 构建 / 清理脚本
    ├── Samples/        # 示例程序
    ├── System/         # 对象模型、智能指针、UUID 等基础设施
    └── Tools/          # 离线工具（rpp、ShaderCrossCompiler、HLSLCross、MeshConverter 等）
```

---

## 7. 常见问题排查

1. **Windows 重新运行 `generate-*.bat` 报目录删除失败**
   - 请先关闭正在打开 `Tiny3D.sln` 的 Visual Studio（以及占用 `bin/` 下程序的进程），再重新运行脚本。
2. **编译 `TinyEditor` / `TinyLauncher` 提示找不到 `Language.h`**
   - 确认已安装 Python 3 及 `openpyxl`、`pandas`，并使用带 `-DTINY3D_COPY_RESOURCE=ON` 的脚本（如 `generate-vs2019-x64-debug.bat` / `generate-vs2022-x64-debug.bat`）重新 configure。
3. **Debug 模式运行样例在启动阶段崩溃 / 抛异常**
   - 请检查是否已安装 Windows 可选功能 **Graphics Tools**（D3D11 Debug Layer），并确认 `dependencies/` 下的 DLL 已通过 `git lfs pull` 完整拉取（文件大小非几百字节的 LFS 指针文本）。
4. **反射生成报错（找不到系统头文件或 `libclang` 失败）**
   - 确认 `source\Projects\.msvc-env-*.cmd` 未指向已卸载的旧版 VS，可删除该文件后重新运行 `generate-*.bat`；Android 若报类似错误，先执行 `clean-android-cxx.bat` 清空 `.cxx` 缓存后再同步构建。
