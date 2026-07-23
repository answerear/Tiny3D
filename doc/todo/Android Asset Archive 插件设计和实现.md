# Android Asset Archive 插件设计和实现

## 一、背景与动机

当前 Tiny3D 引擎在 Android 平台使用 `ZipArchive`（基于 minizip）把 APK 当作 zip 文件打开，从中读取 `Tiny3D.cfg` 及其他资源。该方案存在以下问题：

- 依赖第三方库 minizip，且插件代码仍停留在旧的 Archive API，未迁移到新的 callback + DataStream 接口
- 即使资源未压缩，也需要先读到内存再使用，无法利用 mmap 零拷贝
- 不兼容 Android App Bundle (AAB) 拆包机制和 Play Asset Delivery (PAD)
- minizip 的 ZipArchive CMake 构建默认已被注释掉，处于半废弃状态

**目标：** 使用 Android NDK 原生的 `AAssetManager` API 实现新的 `AndroidAssetArchive` 插件，完全替代 ZipArchive 在 Android 平台的职责。

---

## 二、设计目标

1. 完全遵循引擎新的 `Archive` 接口（`T3DArchive.h` 中的 callback + DataStream 模式）
2. 利用 `AAssetManager` 对未压缩资源的 mmap 零拷贝能力
3. 天然兼容 APK、AAB、Play Asset Delivery (install-time)
4. 不引入第三方库依赖，仅使用 NDK `<android/asset_manager.h>`
5. 像 `FileSystemArchive` 一样以插件形式注册到 `ArchiveManager`

---

## 三、目录结构

由于此插件仅在 Android Runtime 使用，不存在 Editor 变体，因此不拆分 Runtime/Editor 子目录，CMakeLists.txt 直接放在插件根目录下：

```
source/Plugins/Archive/AndroidAsset/
├── CMakeLists.txt                              # 构建 .so，仅 Android 平台编译
├── Include/
│   ├── T3DAndroidAssetArchivePrerequisites.h   # 前置声明、导出宏
│   ├── T3DAndroidAssetArchive.h                # Archive 子类
│   ├── T3DAndroidAssetArchivePlugin.h          # Plugin 类
│   └── T3DAssetDataStream.h                    # 基于 AAsset 的 DataStream
└── Source/
    ├── T3DAndroidAssetArchive.cpp
    ├── T3DAndroidAssetArchivePlugin.cpp
    ├── T3DAndroidAssetArchivePluginDLL.cpp      # DLL 入口
    └── T3DAssetDataStream.cpp
```

---

## 四、核心类设计

### 4.1 AssetDataStream — 基于 AAsset 的 DataStream 实现

继承 `DataStream`，包装 NDK 的 `AAsset` 句柄，提供流式读取能力。

```cpp
#ifndef __T3D_ASSET_DATA_STREAM_H__
#define __T3D_ASSET_DATA_STREAM_H__

#include <android/asset_manager.h>
#include "IO/T3DDataStream.h"

namespace Tiny3D
{
    class AssetDataStream : public DataStream
    {
    public:
        AssetDataStream();
        ~AssetDataStream() override;

        bool open(AAssetManager *mgr, const char *path,
                  int mode = AASSET_MODE_STREAMING);
        void close();

        bool isOpen() const;

        size_t read(void *pBuffer, size_t nSize) override;
        size_t write(void *pBuffer, size_t nSize) override;
        bool seek(long_t lPos, bool relative) override;
        long_t tell() const override;
        long_t size() const override;
        bool eof() const override;
        size_t read(uint8_t *&pData) override;

    private:
        AAsset *mAsset {nullptr};
        long_t  mSize  {0};
    };
}

#endif
```

#### 方法实现

```cpp
// T3DAssetDataStream.cpp

#include "T3DAssetDataStream.h"

namespace Tiny3D
{
    AssetDataStream::AssetDataStream()
    {
    }

    AssetDataStream::~AssetDataStream()
    {
    }

    bool AssetDataStream::open(AAssetManager *mgr, const char *path, int mode)
    {
        if (mAsset)
        {
            close();
        }

        mAsset = AAssetManager_open(mgr, path, mode);
        if (mAsset)
        {
            mSize = AAsset_getLength(mAsset);
            return true;
        }

        mSize = 0;
        return false;
    }

    void AssetDataStream::close()
    {
        if (mAsset)
        {
            AAsset_close(mAsset);
            mAsset = nullptr;
            mSize = 0;
        }
    }

    bool AssetDataStream::isOpen() const
    {
        return mAsset != nullptr;
    }

    size_t AssetDataStream::read(void *pBuffer, size_t nSize)
    {
        if (!mAsset) return 0;
        int bytesRead = AAsset_read(mAsset, pBuffer, nSize);
        return (bytesRead < 0) ? 0 : static_cast<size_t>(bytesRead);
    }

    size_t AssetDataStream::write(void *pBuffer, size_t nSize)
    {
        T3D_ASSERT(false && "AssetDataStream does not support write operations!");
        return 0;
    }

    bool AssetDataStream::seek(long_t lPos, bool relative)
    {
        if (!mAsset) return false;
        off_t result = AAsset_seek(mAsset, lPos, relative ? SEEK_CUR : SEEK_SET);
        return result != (off_t)-1;
    }

    long_t AssetDataStream::tell() const
    {
        if (!mAsset) return 0;
        return mSize - AAsset_getRemainingLength(mAsset);
    }

    long_t AssetDataStream::size() const
    {
        return mSize;
    }

    bool AssetDataStream::eof() const
    {
        if (!mAsset) return true;
        return AAsset_getRemainingLength(mAsset) <= 0;
    }

    size_t AssetDataStream::read(uint8_t *&pData)
    {
        if (!mAsset) return 0;

        // 尝试获取 mmap 缓冲区（对未压缩资源零拷贝）
        const void *buffer = AAsset_getBuffer(mAsset);
        if (buffer)
        {
            pData = const_cast<uint8_t *>(static_cast<const uint8_t *>(buffer));
            return static_cast<size_t>(mSize);
        }

        // 压缩资源回退：分配内存后全量读取
        pData = T3D_NEW_ARRAY uint8_t[mSize];
        AAsset_seek(mAsset, 0, SEEK_SET);
        int bytesRead = AAsset_read(mAsset, pData, static_cast<size_t>(mSize));
        return (bytesRead < 0) ? 0 : static_cast<size_t>(bytesRead);
    }
}
```

#### AASSET_MODE 选择策略

| 模式 | 适用场景 |
|------|----------|
| `AASSET_MODE_STREAMING` | 默认，小文件顺序读取（如 cfg、shader） |
| `AASSET_MODE_RANDOM` | 大文件需要随机 seek（如 mesh） |
| `AASSET_MODE_BUFFER` | 需要 mmap 零拷贝整块内存映射（如纹理，需配合 `noCompress`） |

---

### 4.2 AndroidAssetArchive — Archive 子类

```cpp
#ifndef __T3D_ANDROID_ASSET_ARCHIVE_H__
#define __T3D_ANDROID_ASSET_ARCHIVE_H__

#include "T3DAndroidAssetArchivePrerequisites.h"
#include <android/asset_manager.h>

namespace Tiny3D
{
    class AndroidAssetArchive : public Archive
    {
    public:
        static const char * const ARCHIVE_TYPE;  // "AndroidAsset"

        static AndroidAssetArchivePtr create(const String &name, AccessMode mode);
        ~AndroidAssetArchive() override;

        String getArchiveType() const override;
        String getPath() const override;
        ArchivePtr clone() const override;
        bool exists(const String &name) const override;

        TResult read(const String &name,
                     const ArchiveReadCallback &callback, void *userData) override;
        TResult write(const String &name,
                      const ArchiveWriteCallback &callback, void *userData) override;
        TResult read(const UUID &uuid,
                     const ArchiveReadCallback &callback, void *userData) override;
        TResult write(const UUID &uuid,
                      const ArchiveWriteCallback &callback, void *userData) override;

    protected:
        AndroidAssetArchive(const String &name, AccessMode mode);

    private:
        AAssetManager *mAssetMgr {nullptr};
    };
}

#endif
```

#### 方法实现要点

```cpp
const char * const AndroidAssetArchive::ARCHIVE_TYPE = "AndroidAsset";

AndroidAssetArchivePtr AndroidAssetArchive::create(const String &name, AccessMode mode)
{
    AndroidAssetArchivePtr archive = T3D_NEW AndroidAssetArchive(name, mode);
    return archive;
}

AndroidAssetArchive::AndroidAssetArchive(const String &name, AccessMode mode)
    : Archive(name, mode)
    , mAssetMgr(getGlobalAssetManager())  // 见第五节
{
}

String AndroidAssetArchive::getPath() const
{
    return getName();  // assets/ 下的子目录前缀，空串代表根目录
}

bool AndroidAssetArchive::exists(const String &name) const
{
    String fullPath = getPath().empty() ? name : (getPath() + "/" + name);
    AAsset *asset = AAssetManager_open(mAssetMgr, fullPath.c_str(), AASSET_MODE_UNKNOWN);
    if (asset)
    {
        AAsset_close(asset);
        return true;
    }
    return false;
}

TResult AndroidAssetArchive::read(const String &name,
                                   const ArchiveReadCallback &callback,
                                   void *userData)
{
    TResult ret = T3D_OK;

    do
    {
        if (callback == nullptr)
        {
            ret = T3D_ERR_INVALID_PARAM;
            break;
        }

        String fullPath = getPath().empty() ? name : (getPath() + "/" + name);
        AssetDataStream stream;
        if (!stream.open(mAssetMgr, fullPath.c_str(), AASSET_MODE_STREAMING))
        {
            ret = T3D_ERR_FILE_NOT_EXIST;
            break;
        }

        ret = callback(stream, fullPath, userData);

        stream.close();
    } while (false);

    return ret;
}

TResult AndroidAssetArchive::write(const String &name,
                                    const ArchiveWriteCallback &callback,
                                    void *userData)
{
    T3D_ASSERT(false && "AndroidAssetArchive does not support write operations!");
    return T3D_ERR_NOT_IMPLEMENT;
}

TResult AndroidAssetArchive::read(const UUID &uuid,
                                   const ArchiveReadCallback &callback,
                                   void *userData)
{
    T3D_ASSERT(false && "AndroidAssetArchive does not support read by UUID!");
    return T3D_ERR_NOT_IMPLEMENT;
}

TResult AndroidAssetArchive::write(const UUID &uuid,
                                    const ArchiveWriteCallback &callback,
                                    void *userData)
{
    T3D_ASSERT(false && "AndroidAssetArchive does not support write operations!");
    return T3D_ERR_NOT_IMPLEMENT;
}
```

---

### 4.3 Plugin 注册

完全模仿 `FileSystemArchivePlugin` 模式：

```cpp
// T3DAndroidAssetArchivePlugin.cpp
TResult AndroidAssetArchivePlugin::install()
{
    return T3D_ARCHIVE_MGR.addArchiveCreator(
        AndroidAssetArchive::ARCHIVE_TYPE,
        [](const String &name, Archive::AccessMode mode)
        {
            AndroidAssetArchivePtr archive = AndroidAssetArchive::create(name, mode);
            return archive;
        });
}

TResult AndroidAssetArchivePlugin::uninstall()
{
    return T3D_ARCHIVE_MGR.removeArchiveCreator(AndroidAssetArchive::ARCHIVE_TYPE);
}
```

```cpp
// T3DAndroidAssetArchivePluginDLL.cpp
extern "C"
{
    T3D_ANDROIDASSET_API TResult dllStartPlugin()
    {
        gPlugin = T3D_NEW Tiny3D::AndroidAssetArchivePlugin();
        return Tiny3D::Agent::getInstance().installPlugin(gPlugin);
    }

    T3D_ANDROIDASSET_API TResult dllStopPlugin()
    {
        TResult ret = Tiny3D::Agent::getInstance().uninstallPlugin(gPlugin);
        if (ret == Tiny3D::T3D_OK)
        {
            delete gPlugin;
            gPlugin = nullptr;
        }
        return ret;
    }
}
```

---

## 五、AAssetManager 的获取机制

`AAssetManager` 的生命周期与 Application 绑定，需要在 Java 层启动时通过 JNI 传递到 Native 层。

### 5.1 Java 端

在 `Tiny3DGlobal.java` 中新增：

```java
public class Tiny3DGlobal {
    // 已有的静态方法...

    private static AssetManager sAssetManager;

    public static void initAssetManager(Context context) {
        sAssetManager = context.getAssets();
        nativeSetAssetManager(sAssetManager);
    }

    private static native void nativeSetAssetManager(AssetManager assetManager);
}
```

在 `Tiny3DActivity.onCreate()` 中调用：

```java
@Override
protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    Tiny3DGlobal.initAssetManager(this);
    // ...
}
```

### 5.2 JNI 端

在 `T3DJniApi.cpp`（或新建 `T3DAndroidAssetJni.cpp`）中：

```cpp
#include <android/asset_manager_jni.h>

namespace Tiny3D
{
    static AAssetManager *g_AssetManager = nullptr;

    AAssetManager* getGlobalAssetManager()
    {
        return g_AssetManager;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_tiny3d_lib_Tiny3DGlobal_nativeSetAssetManager(
    JNIEnv *env, jclass /*clazz*/, jobject assetManager)
{
    Tiny3D::g_AssetManager = AAssetManager_fromJava(env, assetManager);
}
```

### 5.3 时序保证

```
Tiny3DActivity.onCreate()
    → Tiny3DGlobal.initAssetManager(this)
        → JNI: g_AssetManager = AAssetManager_fromJava(...)
    → SDL_main() / native 引擎启动
        → Agent::init()
            → loadPlugin("AndroidAssetArchive")
            → loadConfig() 使用 mAssetMgr（此时已初始化）
```

必须确保 `nativeSetAssetManager` 在 native 引擎启动（`SDL_main`）之前调用。当前 `Tiny3DActivity` 继承 `SDLActivity`，`SDL_main` 在 SDL 线程中启动，`onCreate` 中注册 AssetManager 在主线程且先于 SDL 线程创建，时序安全。

---

## 六、Agent 端改动

### 6.1 `T3DAgent.cpp` — loadConfig

```cpp
TResult Agent::loadConfig(const String &cfgPath)
{
    TResult ret = T3D_OK;

    do
    {
#if defined (T3D_OS_ANDROID)
        // 使用 AAssetManager 读取 assets/ 中的配置
        ret = loadPlugin("AndroidAssetArchive");
        if (T3D_FAILED(ret))
        {
            break;
        }

        // name 为空串表示 assets/ 根目录
        ArchivePtr archive = mArchiveMgr->loadArchive(
            "", "AndroidAsset", Archive::AccessMode::kRead);
#else
        // 桌面平台：文件系统
        ret = loadPlugin("FileSystemArchive");
        if (T3D_FAILED(ret))
        {
            break;
        }

        ArchivePtr archive = mArchiveMgr->loadArchive(
            mAppPath, "FileSystem", Archive::AccessMode::kRead);
#endif
        // 读取配置文件（接口完全一致，无需修改）
        ret = archive->read(cfgPath,
            [this](DataStream &stream, const String &name, void *userData)
        {
            return T3D_SERIALIZER_MGR.deserialize(stream, mSettings);
        },
        nullptr);

        if (T3D_FAILED(ret))
        {
            break;
        }

    } while (false);

    return ret;
}
```

### 6.2 资源加载统一

其他资源（mesh、texture、shader 等）加载时也可以通过同一个 Archive 实例：

```cpp
#if defined (T3D_OS_ANDROID)
    ArchivePtr resArchive = T3D_ARCHIVE_MGR.loadArchive(
        "Assets", "AndroidAsset", Archive::AccessMode::kRead);
#else
    ArchivePtr resArchive = T3D_ARCHIVE_MGR.loadArchive(
        resourcePath, "FileSystem", Archive::AccessMode::kRead);
#endif

// 后续代码完全一致
T3D_MESH_MGR.loadMesh(resArchive, "models/tortoise.tmesh");
```

---

## 七、CMake 构建配置

### 7.1 `source/Plugins/Archive/AndroidAsset/CMakeLists.txt`

CMakeLists.txt 直接放在插件根目录，不再嵌套 Runtime 子目录：

```cmake
set_project_name(AndroidAssetArchive)

set(TINY3D_ANDROIDASSET_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    "${TINY3D_CORE_SOURCE_DIR}/Include"
    "${TINY3D_MATH_SOURCE_DIR}/Include"
    "${TINY3D_FRAMEWORK_SOURCE_DIR}/Include"
    "${TINY3D_PLATFORM_SOURCE_DIR}/Include"
    "${TINY3D_SYSTEM_SOURCE_DIR}/Include"
    "${TINY3D_LOG_SOURCE_DIR}/Include"
    "${TINY3D_UTILS_SOURCE_DIR}/Include"
    "${SDL2_INCLUDE_DIR}"
    "${RTTR_INCLUDE_DIR}"
    "${TINY3D_ANDROIDASSET_SOURCE_DIR}/Include"
)

set_project_files(Include ${TINY3D_ANDROIDASSET_SOURCE_DIR}/Include/ .h)
set_project_files(Source ${TINY3D_ANDROIDASSET_SOURCE_DIR}/Source/ .cpp)

add_library(${LIB_NAME} SHARED ${SOURCE_FILES})

set_property(TARGET ${LIB_NAME} PROPERTY FOLDER "Plugins/Archive")

target_link_libraries(
    ${LIB_NAME}
    T3DPlatform
    T3DSystem
    T3DLog
    T3DUtils
    T3DMath
    T3DCore
    rttr_core
    android   # NDK libandroid.so，提供 AAssetManager API
)

add_dependencies(AndroidAssetArchive T3DCore)
```

### 7.2 在上层 CMakeLists 中启用

`source/Plugins/Archive/CMakeLists.txt` 中新增：

```cmake
add_subdirectory(FileSystem)

if (TINY3D_OS_ANDROID)
    add_subdirectory(AndroidAsset)
endif()
```

---

## 八、Gradle 配置优化

在 Sample 工程或主工程的 `build.gradle` 中配置 `noCompress`，使大文件在 APK 中不被压缩，`AAssetManager` 可对其执行 mmap：

```groovy
android {
    aaptOptions {
        noCompress 'tmesh', 'tmat', 'tshader', 'tskel', 'tani',
                   'tscene', 'tprefab', 'ttex', 'timg', 'png', 'cfg'
    }
}
```

---

## 九、Google Play 大包兼容（AAB / Play Asset Delivery）

### 9.1 兼容性矩阵

| 分发方式 | AAssetManager 是否直接可用 | 备注 |
|----------|---------------------------|------|
| 普通 APK | 是 | 直接读 `assets/` |
| AAB (App Bundle) | 是 | Google Play 自动拆分，AAssetManager 透明访问 |
| PAD install-time | 是 | 与普通 assets 行为一致 |
| PAD fast-follow | 需等待下载 | 下载完成后 AAssetManager 可访问 |
| PAD on-demand | 需等待下载 | 同上 |

### 9.2 Play Asset Delivery 集成方案（可选，后续扩展）

对于 fast-follow 和 on-demand 资源包：

**Java 端：**

```java
public class AssetPackHelper {
    private AssetPackManager packManager;

    public void requestPack(String packName, AssetPackCallback callback) {
        AssetPackStates states = packManager.getPackStates(Arrays.asList(packName));
        int status = states.packStates().get(packName).status();

        if (status == AssetPackStatus.COMPLETED) {
            callback.onReady(packName);
        } else {
            packManager.fetch(Arrays.asList(packName));
            // 注册监听，完成后回调
        }
    }
}
```

**Native 端扩展：**

```cpp
// 资源包就绪后通知引擎
extern "C" JNIEXPORT void JNICALL
Java_com_tiny3d_lib_AssetPackHelper_nativeOnPackReady(
    JNIEnv *env, jclass, jstring packName)
{
    // 触发引擎事件，通知资源管理器可以加载该包的资源
    String name = jstringToString(env, packName);
    T3D_EVENT_MGR.postEvent(Event::E_ASSET_PACK_READY, name);
}
```

此部分为后续扩展，初期实现只需支持 install-time assets 即可。

---

## 十、旧 ZipArchive 插件处理

ZipArchive 插件保持原样不动，不删除、不标记 deprecated。仅在 Android 平台的代码路径中不再使用它：

1. `T3DAgent.cpp` 中 Android 分支改为加载 `AndroidAssetArchive`，不再调用 `loadPlugin("ZipArchive")`
2. Android 的 `Tiny3D.cfg` 插件列表中不再包含 `ZipArchive`
3. `source/Plugins/Archive/Zip/` 保持现状，桌面端或其他场景如需读取 zip 资源包仍可继续使用
4. ZipArchive 后续若需迁移到新 Archive API，作为独立任务另行处理，与本次改动无关

---

## 十一、实施步骤与优先级

| 步骤 | 内容 | 优先级 | 预估工作量 |
|------|------|--------|-----------|
| 1 | 创建目录结构 + CMakeLists | P0 | 小 |
| 2 | 实现 `AssetDataStream` | P0 | 中 |
| 3 | 实现 `AndroidAssetArchive` | P0 | 中 |
| 4 | 实现 Plugin + DLL 入口 | P0 | 小 |
| 5 | Java 层 `nativeSetAssetManager` JNI | P0 | 小 |
| 6 | 修改 `T3DAgent.cpp` loadConfig | P0 | 小 |
| 7 | Gradle `noCompress` 配置 | P1 | 小 |
| 8 | Sample 工程验证 | P0 | 中 |
| 9 | 废弃/清理 ZipArchive Android 代码 | P2 | 小 |
| 10 | Play Asset Delivery on-demand 支持 | P3 | 大 |

---

## 十二、与旧方案对比

| 对比项 | ZipArchive (minizip) | AndroidAssetArchive (AAssetManager) |
|--------|---------------------|-------------------------------------|
| 第三方依赖 | minizip | 无，NDK 原生 |
| 未压缩资源读取 | 读到 MemoryDataStream | mmap 零拷贝（`AAsset_getBuffer`） |
| 压缩资源读取 | minizip 解压 | NDK 内部解压，API 一致 |
| AAB 支持 | 需要自行定位拆分后的路径 | 透明支持 |
| Play Asset Delivery | 不支持 | 天然支持 install-time |
| 随机读取 | zip 条目不支持 seek | `AASSET_MODE_RANDOM` 支持 |
| 新 Archive API 兼容 | 未迁移，接口不一致 | 完全遵循新 API |
| 构建状态 | CMake 中已注释 | 仅 Android 平台启用 |

---

## 十三、风险与注意事项

1. **AAssetManager 线程安全**：`AAssetManager_open` 本身是线程安全的，但单个 `AAsset` 句柄不可跨线程使用。每次 `read()` 调用都在回调内创建和销毁 `AssetDataStream`，天然线程安全。

2. **路径分隔符**：`AAssetManager` 使用 `/` 作为路径分隔符，不使用平台 `Dir::getNativeSeparator()`。在 `AndroidAssetArchive` 内部统一用 `/` 拼接。

3. **文件大小限制**：单个 asset 文件建议不超过 1GB（`AAsset_getLength` 返回 `off_t`）。超大资源应使用 Play Asset Delivery 或 OBB。

4. **noCompress 与 APK 大小**：不压缩的文件会增大 APK 体积，但 Google Play 对 AAB 使用差分更新，实际下载增量可控。需要在 APK 大小和运行时性能间权衡。

5. **调试便利性**：开发阶段可以在 PC 上继续使用 `FileSystemArchive` 读取相同的资源目录，Android 上使用 `AndroidAssetArchive`，两者对上层完全透明。
