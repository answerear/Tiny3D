# 把动态库加载与模块查询下沉到 T3DPlatform —— 设计文档

> **目标**：把 `LoadLibrary` / `dlopen`、`GetProcAddress` / `dlsym`、`GetModuleHandleEx` / `dladdr`、`dlerror` 这类平台系统调用收进 `T3DPlatform`，上层（`T3DHLSLCross`、`T3DCore::Dylib`、`T3DSystem::ObjectTracer`、编辑器）只走平台无关接口。
>
> **实现状态（2026-09-29）**：P1–P4 在 **Windows / macOS / Android** 上已落地；**Linux / iOS 仅部分落地**——工厂已给出 `createPlatformSharedLibrary()`（`UnixSharedLibrary`），但两端都没有 `IPlatform` 实现，`getModulePath` / `isAddressMapped` 无落点，且工厂本身缺 `createPlatform` 等纯虚实现、`T3DPlatform` 在这两端编不过，见 P1b。P5（`T3DHLSLCross` / `T3DDxcDriver`）等替换方案阶段 2，本文不再手写 `dlopen`。
>
> **直接驱动方**：
> - `doc/todo/ShaderConductor-Replacement-todo.md` §6.1.2 的 `T3DDxcDriver`（本文完成后，那边不再手写 `#if` 平台分支）
> - `source/Core/Source/Resource/T3DDylib.cpp`（引擎插件加载）
>
> **不在范围**：
> - 候选路径怎么拼、环境变量叫什么、跟谁同目录部署——这些是**调用方策略**，不下沉
> - 渲染后端里创建 HWND 用的 `GetModuleHandle(nullptr)`（Windows 图形 API，不是可移植的"加载动态库"）
> - 第三方源码（imgui / rttr / protobuf）里的 `dlopen`，不动

---

## 1. 为什么要下沉

### 1.1 同一组系统调用写了好几遍

仓库里至少有四处各自用 `#if` 包了一套动态库 / 模块查询：

| 位置 | 系统调用 | 用途 |
|------|----------|------|
| `Core/Source/Resource/T3DDylib.cpp:31-49` | `LoadLibrary` / `dlopen(RTLD_NOW)`、`GetProcAddress` / `dlsym`、`FreeLibrary` / `dlclose`、`dlerror` | 引擎插件 |
| `System/Source/Object/T3DObjectTracer.cpp:68-78` | `GetModuleHandleEx` / `dladdr` | 判断 vptr 所属模块是否还在地址空间 |
| `ShaderConductor-Replacement-todo.md` §6.1.2 | `LoadLibrary` / `dlopen(RTLD_LAZY\|RTLD_LOCAL)`、`GetProcAddress` / `dlsym`、`GetModuleHandleEx`+`GetModuleFileName` / `dladdr`+`realpath`、`dlerror`、`getenv` | 运行期加载 `dxcompiler` |
| `Editor/TinyEditor/CppBuildSystem.cpp:199-209` | 平台宏拼 `lib` 前缀与扩展名 | 与 `Dylib::onLoad` 保持一致，否则影子副本对不上 |

`T3DHLSLCross` 文档 §6.1.1 写过「不复用 `T3DDylib`，因为那会拖进整个 `T3DCore`」。这个判断对——但结论不该是再抄一套 `dlopen`，而该是**把这层下沉到已经存在的 `T3DPlatform`**。`T3DHLSLCross` 本来就要链 Platform（头文件已用 `T3DType.h` / `T3DPlatformLib.h`），多链一个本仓库模块比再写一遍 Win32/POSIX 分支干净。

### 1.2 现有 `T3DPlatform` 缺的就是这一块

`T3DPlatform` 已经按「门面类 + `I*` 适配器 + `IFactory::create*()`」收过目录、进程、线程、区域编码：

| 门面 | 适配器 | 工厂方法 |
|------|--------|----------|
| `Dir` | `IDir` | `createPlatformDir()` |
| `Process` | `IProcess` | `createPlatformProcess()` |
| `Locale` | `ILocale` | `createPlatformLocale()` |
| （无） | （无） | —— |

动态库加载没有对等物，所以 `T3DCore::Dylib` 只好自己 `#include <windows.h>` / `<dlfcn.h>`。

`Locale::UTF8ToUnicode()` 已经覆盖 §6.1.3 的宽字符转换，**不要再做一个**。`Dir::parsePath()` / `Dir::getNativeSeparator()` 已经覆盖路径拆分。本文只补「加载库」和「按地址反查模块」两块系统调用。

### 1.3 什么留下、什么下去

| 留下调用方 | 下沉 Platform |
|------------|---------------|
| 候选路径列表（环境变量 → 自身旁 → 裸名字） | `open(绝对或相对路径, flags)` |
| 环境变量名叫 `T3D_DXCOMPILER_PATH` | `Environment::get()`（可选，见 §5） |
| 跟 `T3DHLSLCross` 同目录部署 | `getModulePath(本函数地址)` + `Dir::parsePath` 取出目录 |
| 插件搜 `Agent::getPluginsPath()` | `makeFileName("FreeImage")` + `Dir` 拼路径 |
| 不主动 `dlclose` 以免静态析构顺序问题 | `open` / `close` 成对，析构默认 `close`，调用方可选择不析构 |

---

## 2. 现状审计（调用点）

### 2.1 `T3DCore::Dylib` —— 第一迁移目标

```31:49:source/Core/Source/Resource/T3DDylib.cpp
#if defined (T3D_OS_WINDOWS)
    #include <windows.h>
    typedef HINSTANCE   DYLIB_HANDLE;
    #define DYLIB_LOAD(name)            LoadLibrary(name)
    #define DYLIB_GETSYM(handle, name)  GetProcAddress((HMODULE)handle, name)
    #define DYLIB_UNLOAD(handle)        FreeLibrary((HMODULE)handle)
    #define DYLIB_ERROR()               "Unknown Error"
#elif defined (T3D_OS_LINUX) || defined (T3D_OS_OSX) || defined (T3D_OS_ANDROID) || defined (T3D_OS_IOS)
    #include <dlfcn.h>
    typedef void*       DYLIB_HANDLE;
    #define DYLIB_LOAD(name)            dlopen(name, RTLD_NOW)
    #define DYLIB_GETSYM(handle, name)  dlsym(handle, name)
    #define DYLIB_UNLOAD(handle)        dlclose(handle)
    #define DYLIB_ERROR()               dlerror()
#endif
```

要点：

- 逻辑名不含扩展名，加载时拼 `name.dll` / `libname.so` / `libname.dylib`
- 非 Android 用 `searchPath` 或 `Agent::getPluginsPath()` 拼绝对路径再加载
- Android 只传裸文件名，走系统搜索路径
- POSIX 固定 `RTLD_NOW`，**没有** `RTLD_LOCAL`
- Windows 失败时 `DYLIB_ERROR()` 写死 `"Unknown Error"`，不如 `dlerror()`——下沉时一并修成 `FormatMessage`

`Dylib` 本身是 `Resource` 子类（引用计数、管理器、UUID），这个身份留在 Core。Platform 只提供「打开 / 查符号 / 关闭」三件套。

### 2.2 `T3DSystem::ObjectTracer`

```68:78:source/System/Source/Object/T3DObjectTracer.cpp
            return GetModuleHandleExA(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                    | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCSTR>(vptr), &module) != FALSE;
            ...
            return dladdr(const_cast<void*>(vptr), &info) != 0
                && info.dli_fname != nullptr;
```

只要「这个地址还属于某个已映射模块」，不要路径。对应本文 `SharedLibrary::isAddressMapped()`。

### 2.3 `T3DHLSLCross`（尚未落地，设计在替换方案 §6.1.2）

需要的系统调用比 `Dylib` 多两样：

1. **按本库某个函数地址反查自身路径**（`GetModuleHandleEx` + `GetModuleFileName` / `dladdr` + `realpath`）。`Dir::getAppPath()` 是**宿主可执行文件**目录，锚点不对。
2. **`RTLD_LAZY | RTLD_LOCAL`**：隔离 dxcompiler 内部 LLVM 符号，避开 `dependencies/llvm`。`Dylib` 的 `RTLD_NOW` 不够用，接口必须带 flags。

宽字符转换走现成的 `T3D_LOCALE.UTF8ToUnicode()`，不要在 driver 里手写 UTF-8 → UTF-16/32。

### 2.4 编辑器 `CppBuildSystem::platformLibFileName`

与 `Dylib::onLoad` 的拼接规则必须一致。下沉成 `SharedLibrary::makeFileName()` 后，两边都调它，规则不会再漂。

### 2.5 可以迁、但优先级低

| 位置 | 说明 |
|------|------|
| `Platform/Source/Adapter/Windows/T3DWin32MemManager.cpp` | `LoadLibrary("dbghelp.dll")`，Platform 内部自用，迁不迁都行 |
| `Platform/Source/Adapter/Android/T3DAndroidMemManager.cpp` | `dladdr` 取符号名打堆栈，可复用 `getModulePath` / 后续再加 `getSymbolName` |
| `Plugins/Renderer/OpenGL4` / `Vulkan` 的 `GetModuleHandle` | 给 Win32 窗口类用，**不要迁** |

### 2.6 明确不迁

- `getenv` 的**名字与优先级**（`T3D_DXCOMPILER_PATH`、`TINY3D_SDK_ROOT`）是业务
- imgui / rttr / protobuf 第三方
- `GetModuleHandle` 取当前 EXE 的 `HINSTANCE` 去 `RegisterClass`

---

## 3. 总体设计

### 3.1 分层

```
调用方（零平台宏）
    T3DHLSLCross::DxcLibrary
    T3DCore::Dylib
    T3DSystem::ObjectTracer
    TinyEditor::CppBuildSystem
        │
        ▼
门面  SharedLibrary / Environment     （T3D_PLATFORM_API，与 Process / Dir 同级）
        │
        ▼
适配器 ISharedLibrary                 （各平台实现，调用方看不到）
        Win32SharedLibrary
        UnixSharedLibrary             （OSX / Linux / Android / iOS 共用）
        │
        ▼
系统    LoadLibrary / dlopen / dladdr / GetModuleHandleEx
```

进程级查询（按地址找模块）挂在 `IPlatform` 上，跟现有 `getCurrentProcessID()` 一类；门面 `SharedLibrary` 用静态方法转过去。这样不必为一次 `dladdr` 再造一个单例。

### 3.2 目录与文件

```
source/Platform/Include/
    Adapter/T3DSharedLibraryInterface.h
    SharedLibrary/T3DSharedLibrary.h
    SharedLibrary/T3DEnvironment.h          # 可选，见 §5

source/Platform/Source/
    SharedLibrary/T3DSharedLibrary.cpp
    SharedLibrary/T3DEnvironment.cpp
    Adapter/Windows/T3DWin32SharedLibrary.h/.cpp
    Adapter/Unix/T3DUnixSharedLibrary.h/.cpp
```

Unix 实现放 `Adapter/Unix/`，OSX / Linux / Android / iOS 工厂都 `create` 它——跟 `T3DPosixThread` / `T3DPosixProcess` 一样。不要在四个平台各抄一份 `dlopen`。

### 3.3 工厂

`IFactory` 增加：

```cpp
virtual ISharedLibrary *createPlatformSharedLibrary() = 0;
```

Windows / OSX / Linux / iOS / Android 五个 Factory 都要实现。Linux / iOS 工厂目前对 Thread / Process 等方法不完整，**本接口必须五端都给**——`T3DHLSLCross` 把 Linux 当一等目标，缺 Linux 实现等于白做。

> **现状（2026-09-29）**：五端工厂都已实现 `createPlatformSharedLibrary()`。但 `IPlatform` 的两个新方法只在 `Win32Platform` / `OSXPlatform` / `AndroidPlatform` 有实现；Linux / iOS 没有 `*Platform` 类，`LinuxFactory` / `iOSFactory` 也缺 `createPlatform`、`createPlatformThread`、各同步对象、`createPlatformProcess`、`createPlatformLocale` 等纯虚实现，是抽象类。这是本文落地前就存在的缺口，但它直接导致 `SharedLibrary::getModulePath` / `getModuleDir` / `isAddressMapped` 在这两端不可用，补齐见 §8 P1b。

---

## 4. 接口设计

### 4.1 加载标志

```cpp
enum SharedLibraryFlags : uint32_t
{
    /// 解析到的未定义符号立即失败（POSIX RTLD_NOW；Windows 无对应，忽略）
    kNow    = 1u << 0,
    /// 符号用到再解析（POSIX RTLD_LAZY；Windows 忽略）
    kLazy   = 1u << 1,
    /// 符号不进入全局符号表（POSIX RTLD_LOCAL；Windows 默认即是）
    kLocal  = 1u << 2,
    /// 符号进入全局符号表（POSIX RTLD_GLOBAL；Windows 无对应，忽略）
    kGlobal = 1u << 3,
};

/// 引擎插件默认：立刻解析，符号可见（对齐今天 Dylib 的 RTLD_NOW）
constexpr uint32_t kSharedLibraryPlugin = kNow;

/// 第三方编译器 / 带私有 LLVM 的库：惰性 + 本地（对齐 HLSLCross 的 dxcompiler）
constexpr uint32_t kSharedLibraryIsolated = kLazy | kLocal;
```

`kNow` 与 `kLazy` 互斥，`kLocal` 与 `kGlobal` 互斥。实现里校验，非法组合返回失败，不要静默改掉。

Windows 忽略 POSIX-only 位是刻意的：行为已经等价（`LoadLibrary` 既不延迟解析导出表给调用方、也不把符号注入进程全局）。调用方仍应传完整 flags，换到 Linux 才生效。

### 4.2 `ISharedLibrary`

```cpp
class ISharedLibrary : public Allocator
{
    T3D_DECLARE_INTERFACE(ISharedLibrary);

public:
    /// 打开动态库。path 原样交给系统：可以是绝对路径、相对路径或裸文件名
    virtual TResult open(const String &path, uint32_t flags) = 0;

    /// 关闭。未打开或已关闭则空操作
    virtual void close() = 0;

    virtual bool isOpen() const = 0;

    /// 按导出符号名取地址。未打开或找不到返回 nullptr
    virtual void *getSymbol(const String &name) const = 0;

    /// 最近一次失败的平台原文（Windows FormatMessage / POSIX dlerror）
    virtual String getLastError() const = 0;

    virtual THandle getNativeHandle() const = 0;
};
```

`open` 的 `path` **不做** `lib` 前缀、**不改** 扩展名、**不**自动加搜索目录。拼路径是调用方的事（§1.3）。Android 上传裸 `libfoo.so`、桌面上传绝对路径，都合法。

### 4.3 门面 `SharedLibrary`

照 `Process`：实例方法转 `ISharedLibrary`，静态方法转 `IPlatform`。

```cpp
class T3D_PLATFORM_API SharedLibrary : public Allocator, public Noncopyable
{
public:
    SharedLibrary();
    ~SharedLibrary() override;   // 若仍 isOpen() 则 close()

    TResult open(const String &path, uint32_t flags = kSharedLibraryPlugin);
    void    close();
    bool    isOpen() const;
    void   *getSymbol(const String &name) const;
    String  getLastError() const;
    THandle getNativeHandle() const;

    /// 逻辑名 → 平台文件名。不含目录。
    /// "dxcompiler" → "dxcompiler.dll" / "libdxcompiler.so" / "libdxcompiler.dylib"
    static String makeFileName(const String &logicalName);

    /// 反查 address 所属模块的绝对路径。失败返回空串。
    /// 传入本模块内任意静态函数地址，即可得到本 .dll / .so / .dylib 的路径。
    static String getModulePath(const void *address);

    /// getModulePath + Dir::parsePath，只返回目录，末尾不带分隔符
    static String getModuleDir(const void *address);

    /// address 是否仍映射在某个已加载模块里（给 ObjectTracer）
    static bool isAddressMapped(const void *address);

protected:
    ISharedLibrary *mLib {nullptr};
};
```

`getModuleDir` 是便利函数，内部只调 `getModulePath` + 已有的 `Dir::parsePath`。**路径拼接规则仍用 `Dir`，不在本类里发明第二套。**

### 4.4 `IPlatform` 新增

```cpp
virtual String getModulePath(const void *address) const = 0;
virtual bool   isAddressMapped(const void *address) const = 0;
```

实现要点：

| 平台 | `getModulePath` | `isAddressMapped` |
|------|-----------------|-------------------|
| Windows | `GetModuleHandleExA(FROM_ADDRESS \| UNCHANGED_REFCOUNT)` + `GetModuleFileNameW` + UTF-8 | `GetModuleHandleExA` 成功即 true |
| POSIX | `dladdr`；`dli_fname` 可能是相对路径，必须再 `realpath` | `dladdr != 0 && dli_fname != nullptr` |

Windows 用宽版 `GetModuleFileNameW` 再经 `Locale` 转 UTF-8，避免非 ASCII 安装路径截断。POSIX 的 `realpath` 失败则退回 `dli_fname` 原文，并在日志里留一句，不要直接返回空（某些嵌入式 / Android 旧 bionic 对 `realpath` 较严）。

`_GNU_SOURCE` / `-ldl` 从 `T3DHLSLCross` 的 CMake **挪到 `T3DPlatform`**。HLSLCross 不再为 `dladdr` 单独加编译定义。

### 4.5 错误码

`T3DPlatformErrorDef.h` 追加：

```cpp
T3D_ERR_SHAREDLIB_OPEN,      // open 失败
T3D_ERR_SHAREDLIB_NOT_OPEN,  // 未打开就 getSymbol
T3D_ERR_SHAREDLIB_SYMBOL,    // 符号不存在（可选；getSymbol 返回 nullptr 也可，看调用方）
T3D_ERR_SHAREDLIB_FLAGS,     // flags 非法组合
```

`getSymbol` 失败用返回值 `nullptr` 即可，不必强制错误码——`Dylib` 和 HLSLCross 都是这样用的。`open` 必须返回 `TResult`，并把平台原文塞进 `getLastError()`。

---

## 5. 环境变量（可选、建议一并做）

`getenv` 在三平台 CRT 都能用，严格说不是必须下沉。但 Windows 的 `getenv` 走 ANSI 代码页，路径含非 ASCII 会坏；而且 HLSLCross、`PlayerApp`、`ReflectionPreprocessor` 已经各写各的 `std::getenv`。

建议加一个很薄的门面，**不**做成 Factory 接口：

```cpp
class T3D_PLATFORM_API Environment
{
public:
    /// UTF-8 名 → UTF-8 值。不存在返回空串，has() 区分「未设置」与「设成空」
    static String get(const String &name);
    static bool   has(const String &name);
};
```

Windows：`GetEnvironmentVariableW` + UTF-8。POSIX：`getenv`（环境本身是字节，按 UTF-8 解释）。

一期不做 `set` / `unset`，没有调用方需要。

HLSLCross 的候选列表仍然自己排：

```cpp
if (Environment::has("T3D_DXCOMPILER_PATH"))
    candidates.push_back(Environment::get("T3D_DXCOMPILER_PATH"));
```

变量名不下沉。

---

## 6. 调用方怎么改（仍不改代码，只定契约）

### 6.1 `T3DHLSLCross::DxcLibrary`（替换方案 §6.1.2）

改造后伪代码。平台头文件、`moduleDir` 手写、`openLib` 的 `#if` 全部消失：

```cpp
SharedLibrary lib;   // 或挂在 DxcLibrary 成员上，故意不析构以免进程退出时 unload

const String fileName = SharedLibrary::makeFileName("dxcompiler");
TArray<String> candidates;

if (Environment::has("T3D_DXCOMPILER_PATH"))
    candidates.push_back(Environment::get("T3D_DXCOMPILER_PATH"));

const String dir = SharedLibrary::getModuleDir((void*)&moduleDirAnchor);
if (!dir.empty())
    candidates.push_back(dir + Dir::getNativeSeparator() + fileName);

candidates.push_back(fileName);   // 退到系统搜索路径

for (const String &c : candidates)
{
    if (lib.open(c, kSharedLibraryIsolated) == T3D_OK)
        break;
}

mCreateInstance = (DxcCreateInstanceProc)lib.getSymbol("DxcCreateInstance");
if (mCreateInstance == nullptr)
    mError = String("Failed to load ") + fileName + ". " + lib.getLastError();
```

**仍留在 HLSLCross 的**：候选顺序、环境变量名、逻辑名 `"dxcompiler"`、`kSharedLibraryIsolated` 这个选择。

**因此替换方案 §6.1.1 / §6.1.2 里那大段 `#if _WIN32` 示例作废**，改为引用本文。`T3DDxcDriver.cpp` 不再 `#include <windows.h>` / `<dlfcn.h>`。

CMake：`T3DHLSLCross` **链接 `T3DPlatform`**，去掉自己的 `${CMAKE_DL_LIBS}`、`_GNU_SOURCE`、`Threads::Threads`（后两者若 DXC 仍需要线程库再另说，跟 dlopen 无关）。

前提：`Platform::init()` 已完成。`scc` 是 Console 应用，启动时就会 init。`isAvailable()` 放在参数解析之后、编译循环之前（替换方案 §7.1），满足这个前提。

### 6.2 `T3DCore::Dylib`

`T3DDylib.cpp` 删掉全部平台宏和 `DYLIB_*`。成员 `THandle mHandle` 改成 `SharedLibrary mLib`（或堆上 `SharedLibrary*`，看要不要保持 `THandle` 对外）。

```cpp
TResult Dylib::onLoad(Archive *archive)
{
    const String fileName = SharedLibrary::makeFileName(mName);
#if defined (T3D_OS_ANDROID)
    const String path = fileName;          // 仍走系统搜索；Android 部署模型没变
#else
    const String root = mSearchPath.empty()
        ? Agent::getInstance().getPluginsPath() : mSearchPath;
    const String path = root + Dir::getNativeSeparator() + fileName;
#endif
    const TResult ret = mLib.open(path, kSharedLibraryPlugin);
    if (ret != T3D_OK)
    {
        T3D_LOG_ERROR(LOG_TAG_PLUGIN, "Load plugin failed ! Desc : %s",
                      mLib.getLastError().c_str());
        return T3D_ERR_PLG_LOAD_FAILED;
    }
    return Resource::onLoad(archive);
}
```

Android 那条 `#if` 是**部署策略**（不拼绝对路径），按 §1.3 可以留。若以后 `Dir::getLibraryPath()` 已经给出 JNI 库目录，可以再改成绝对路径，那是另一件事。

`getSymbol` / `onUnload` 直接转 `mLib`。

对外 `T3DDylib.h` 不必变。`Agent` / `DylibManager` 不必变。

### 6.3 `ObjectTracer`

```cpp
return SharedLibrary::isAddressMapped(vptr);
```

`T3DObjectTracer.cpp` 去掉 `<windows.h>` / `<dlfcn.h>`。

### 6.4 `CppBuildSystem::platformLibFileName`

```cpp
return SharedLibrary::makeFileName(name);
```

`platformSymbolFileName`（`.pdb`）仍是 MSVC 调试信息，不是动态库加载，**不迁**。

### 6.5 宽字符

替换方案 §6.1.3 的 `toWide()` 改为：

```cpp
std::wstring toWide(const String &s)
{
    return T3D_LOCALE.UTF8ToUnicode(s);
}
```

`Locale` 已按平台处理 UTF-16 / UTF-32。HLSLCross 需要 `Platform::init()` 之后才能用 `T3D_LOCALE`（与 §6.1 同一前提）。

---

## 7. 实现要点（给写代码的人）

### 7.1 Windows

- `open`：绝对路径用 `LoadLibraryW`（先 UTF-8 → UTF-16），堵住当前目录劫持；裸文件名也走 `LoadLibraryW`，搜索顺序交给系统
- `getLastError`：`GetLastError` + `FormatMessageW` → UTF-8，不要再返回 `"Unknown Error"`
- `getSymbol`：`GetProcAddress`，符号名仍是 ASCII
- `close`：`FreeLibrary`；注意 `GetModuleHandleEx` 的 `UNCHANGED_REFCOUNT` **不会**增加引用，和 `LoadLibrary` 的计数是两套，不要对 `getModulePath` 拿到的模块调用 `FreeLibrary`

### 7.2 POSIX（一份 Unix 实现）

- `open`：把 flags 映射成 `RTLD_*`，`dlopen`
- `getLastError`：必须在失败后立刻 `dlerror()`，下一次 `dlopen`/`dlsym` 会冲掉
- `getModulePath`：`dladdr` + `realpath`；`_GNU_SOURCE` 放在 **Platform** 的 CMake `target_compile_definitions`，不要依赖包含顺序
- `CMake`：`T3DPlatform` 在 UNIX 上链 `${CMAKE_DL_LIBS}`（若尚未链）

### 7.3 与 `Dir::getAppPath()` 的区别

| API | 锚点 |
|-----|------|
| `Dir::getAppPath()` | 宿主可执行文件 |
| `SharedLibrary::getModulePath(addr)` | `addr` 所属的那一个已加载模块 |

`T3DHLSLCross` 是动态库，跟 `scc` 可以不在同一层。必须用后者。静态链进某个 exe 时，`addr` 落在 exe 镜像里，退化为 exe 路径——两种链接形态都对，跟替换方案 §6.1.2 原来的设计一致。

### 7.4 不要在 `SharedLibrary` 里做搜索

曾经有过冲动：`open("dxcompiler")` 自动搜 exe 旁、`LD_LIBRARY_PATH`、自身旁。**不要。** 各调用方搜索策略不同（插件目录 / 环境变量 / 同级 dxcompiler），合到 Platform 里会变成无法拆开的大杂烩。`open` 只做系统调用。

---

## 8. 实施步骤

| # | 阶段 | 交付 | 验收 | 状态 |
|---|------|------|------|------|
| P1 | 接口 + Unix/Win 实现 | `ISharedLibrary`、`SharedLibrary`、`IPlatform` 两个新方法、五端 Factory | 临时验证程序（未入库）：`open` 系统库、`getSymbol`、`getModulePath(自身地址)` 非空且文件存在 | **部分落地**：接口、`Win32` / `Unix` 适配器、五端 `createPlatformSharedLibrary()` 已完成；`IPlatform` 两个新方法仅 Windows / macOS / Android 有实现 |
| P1b | Linux / iOS 平台层补齐 | `LinuxPlatform` / `iOSPlatform`：`getModulePath` / `isAddressMapped` 转调 `UnixSharedLibrary::queryModulePath` / `queryAddressMapped`（同 `OSXPlatform`）；两端工厂补齐 `createPlatform`、`createPlatformThread`、同步对象、`createPlatformProcess`、`createPlatformLocale` 等，复用 `Adapter/Unix/` 下现成的 `PosixThread` / `T3DPosixSyncObject.h` / `PosixProcess` / `PosixLocale` | `T3DPlatform` 在 Linux 上编得过；Linux 上 `getModuleDir(自身地址)` 非空；`T3D_LOCALE` 可用。iOS 同理，但不阻塞替换方案 | **未开始** |
| P2 | 错误与 flags | `FormatMessage` / `dlerror`、flags 互斥校验、`kSharedLibraryIsolated` | Windows 失败信息不再是 `"Unknown Error"`；已验证互斥 flags 报错与失败原文 | 已落地 |
| P3 | `Environment`（可选） | `Environment::get/has` | Windows 走 `GetEnvironmentVariableW` | 已落地 |
| P4 | 迁 `Dylib` + `ObjectTracer` + 编辑器文件名 | 三处去掉平台宏 | `makeFileName` 共用；Android 部署策略 `#if` 按 §6.2 保留 | 已落地 |
| P5 | `T3DHLSLCross` 按本文改 §6.1.2 | 见替换方案阶段 2 | `isAvailable()` 报错带平台原文 | **未开始**（库尚未入库） |

P1–P3 可先于 ShaderConductor 替换落地。P5 依赖 P1。P4 可与替换方案并行，互不阻塞。

P1b 的 Linux 部分是替换方案**阶段 9（Linux 验证）**的前置条件（见 `ShaderConductor-Replacement-todo.md` §9.1「`T3DPlatform` 的 Linux 缺口」），不阻塞阶段 2-8。iOS 部分与替换方案无关，可按需排期。

**不要**在同一次提交里既下沉 Platform 又换掉 ShaderConductor。对拍出差异时先确认不是加载路径变了。

---

## 9. 验收清单

- [x] `source/` 下业务代码（不含 `External/`、不含渲染后端 Win32 窗口）不再直接 `#include <dlfcn.h>` 或为加载动态库而 `#include <windows.h>`
- [x] `T3DDylib.cpp` 零平台宏（Android 部署策略那一处除外，见 §6.2）
- [x] `T3DObjectTracer.cpp` 零平台宏
- [x] `CppBuildSystem::platformLibFileName` 与 `Dylib::onLoad` 都只调 `makeFileName`
- [ ] `T3DHLSLCross` 的 `T3DDxcDriver.cpp` 不包含 `windows.h` / `dlfcn.h`（P5，库尚未入库）
- [ ] Linux：`dlerror` / `FormatMessage` 原文能出现在 `isAvailable()` 与插件加载失败日志里（P5，依赖 P1b）
- [x] `getModulePath` 返回绝对路径（Windows / macOS / Android；Probe 覆盖；含空格、非 ASCII 安装目录待实机补测）
- [ ] Linux / iOS：`T3DPlatform` 可编译，`getModulePath` 返回绝对路径（P1b）
- [ ] 静态链接与动态链接两种形态下，`getModuleDir(本函数地址)` 都指向该镜像所在目录（P5 对拍时再验）

---

## 10. 和替换方案的关系

`doc/todo/ShaderConductor-Replacement-todo.md` 的 §6.1.1 / §6.1.2 / §6.1.2.1 / §6.1.3 / §8.3 以本文为平台层前提：

- 那边不再设计第二套 `LoadLibrary` / `dlopen` 封装
- 那边的 Linux 清单里，`-ldl`、`_GNU_SOURCE`、`dlerror` 格式、`RTLD_LOCAL`、`dladdr`/`realpath` 的实现细节归本文；HLSLCross 只保留「候选路径策略」和「用 `kSharedLibraryIsolated`」
- `T3DHLSLCross` 链接 `T3DPlatform` + `spirv-cross`，不再以「完全不链仓库内模块」为约束——原先要躲的是 **Core**，不是 Platform

替换方案阶段 2（DXC 驱动）启动前，本文至少完成 P1。否则阶段 2 又会把平台分支写回 `T3DDxcDriver.cpp`，本文失去意义。当前 P1 在 Windows / macOS 上已满足，阶段 2 可以开工；替换方案阶段 9 启动前还需完成 P1b 的 Linux 部分。
