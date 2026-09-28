# ComputeApp 移动端（Android）：设计与实施计划

> 让 `ComputeApp` 在 Android 真机上跑起来，承接 `ComputeApp-Sample-Design-todo.md` §7 的 **S5** 期。桌面版靠 `R` / `1-8` / `C` / `V` / `P` 五类热键驱动自检轨与可视轨，手机上没有键盘，需要把这些操作从键盘输入里解耦出来换成触摸手势。
>
> **结论先行：这次移植的构建侧几乎不用动。** `ComputeApp/CMakeLists.txt` 的 Android 分支、`Samples/CMakeLists.txt` 的注册、两处 `pickSource` 的 `T3D_OS_ANDROID` 分支、11 个内核里 9 个的 ESSL 变体，全都已经在仓库里了。缺的是 Gradle 工程目录、触摸输入、以及真机核对。
>
> **但有一条会静默画错的路径必须在上机之前修掉**，见 §0。它会伪装成「GPU 算错了」，把真机调试引向完全错误的方向。
>
> **iOS 本轮不做**，理由见 §3。
>
> 相关文档：
>
> - Sample 本体：`doc/todo/ComputeApp-Sample-Design-todo.md`（用例矩阵 K0–K10、可视轨设计、D3D11 落地实测的 P1–P10；本文只改输入层与平台，用例语义不动）
> - RHI 能力蓝图：`doc/todo/RHI-Compute-UAV-Indirect-Draw-Design-todo.md`（§12.5 对移动端 compute 可用性的预警，本文 §6 是它的实际检验）
> - 移植模板与先例：`doc/todo/PostProcessingApp-Android-Design-todo.md`（同一套方法论：脚手架整份照抄 + 输入层解耦；其 §6 的照抄纪律本文全部沿用）
> - **Android 工程参考：`source/Samples/PostProcessingApp/Android/`**（整份照抄，理由与差异分析见 §4.0）
> - 命令源参考实现：`source/Samples/PostProcessingApp/PostProcessCommand.h`、`TouchCommandSource.{h,cpp}`
> - GPU 读回：`doc/todo/GPU-Readback-onRender-Design-todo.md`（自检轨的 `verify()` 全部依赖它）

---

## 0. 上机之前必须先修的一条：VS 侧 SSBO 在 ES 3.1 上可能根本不可用

`GLES3Context::setVSStructuredBuffers` 有一道设备门槛：

```2793:2798:source\Plugins\Renderer\OpenGLES3\Runtime\Source\T3DGLES3Context.cpp
        if (mMaxVertexShaderStorageBlocks == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER,
                "setVSStructuredBuffers : GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS is 0 on this device");
            return T3D_ERR_GLES3_UNSUPPORTED_OPERATION;
        }
```

这不是后端偷懒。**`GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS` 在 ES 3.1 规范里的最小值就是 0** —— 顶点阶段的 SSBO 是可选能力，compute 阶段的不是。设备支持 compute 不能推出它支持顶点阶段读 SSBO。

而 ComputeApp 可视轨的 GPU 路径恰好整条建立在这个能力上：`particleDrawVSSource()` 的 ESSL 变体用 `layout(std430, binding = 0) readonly buffer gParticles` 做 vertex pulling，VS 一个顶点属性都不读，粒子状态全靠 SSBO 索引。

问题在调用点没有检查返回值：

```660:663:source\Samples\ComputeApp\ParticleSystem.cpp
        StructuredBuffers vsSrvs;
        vsSrvs.push_back(mParticles);
        vsSrvs.push_back(mVisible);
        ctx->setVSStructuredBuffers(0, vsSrvs);
```

在 `mMaxVertexShaderStorageBlocks == 0` 的设备上，绑定失败而绘制照常发出，VS 从未绑定的 SSBO 里读到垃圾，表现是**粒子满屏乱飞、或者全部塌在原点**。日志里只有一行 ERROR 混在 logcat 的洪流中间。这个现象与「compute 把粒子位置算错了」几乎无法区分，而后者的嫌疑人（结构体布局、绑定号、`uavBarrier` 时序）一个比一个难查。

**处置**（属于工作三，但优先级高于一切，排在 M1）：

1. `ParticleSystem::setup` 在建完 `mParticles` 之后试探性调一次 `setVSStructuredBuffers`，失败就把 `mDrawVS` 置空、`mCpuMode = true`，并打一条明确的日志：本设备不支持 VS 侧 SSBO，可视轨退化为 CPU 参考实现。`mReady` 的判定（`ParticleSystem.cpp:339-343`）本来就允许「只有 CPU 路径可用」这个状态，改动落在既有分支里。
2. `ParticleSystem::record` 检查这次调用的返回值，失败就跳过绘制并返回错误码，让 `ComputeApp::onRender`（`ComputeApp.cpp:206-210`）把它打进日志。
3. `ComputeApp::logCapabilities`（K0）把这个上限值一并打出来。现在 K0 打了 `maxComputeSharedMemory` 与 `maxUnorderedAccessSlots`（`ComputeApp.cpp:270-271`），唯独没打它 —— 而它是本 Sample 在移动端上最关键的一个数。

这三条加起来不到 30 行，但它决定了真机首次运行时看到的是「一句明确的降级说明」还是「一屏乱飞的粒子」。

---

## 1. 目标与非目标

### 1.1 目标

| # | 目标 | 验收方式 |
|---|------|----------|
| 1 | Android 真机能装能跑 `ComputeApp` | APK 安装启动，logcat 出现 K0 能力位 |
| 2 | 自检轨在 GLES3 上给出**正确的**结论 | 3 passed / 0 failed / 5 skipped，且 5 条 SKIP 的原因逐条核对（§6.1） |
| 3 | 可视轨在 GLES3 上成立 | 粒子在动；切 CPU 参考实现后画面看不出区别 |
| 4 | 触摸手势覆盖桌面 `R` / `C` / `V` / `P` 的语义 | §5.3 手势表逐条过 |
| 5 | 桌面版键盘行为**一字不变** | Windows 上 D3D11 与 GL4 双后端回归 |
| 6 | 设备不支持某条路径时**明确降级而非静默画错** | §0 的三条处置 |

### 1.2 非目标

- **不做 iOS。** 见 §3。
- **不改任何用例的 `backendMask` / `requiredCaps`。** 它们已经写对了，Android 上不改一行就能得到正确的跳过行为（§6.1）。
- **不改 GLES3 后端。** §6.3 列了 5 条真机要盯的点，其中 R2（compute program 每帧重建）若确认成立，是引擎侧的独立课题，**单独立项单独提交**，不混进本次移植。
- **不补 K7 / K8 的 ESSL 变体。** 技术上可做（§6.2），但放在真机跑通之后，列为 M6 可选。
- **不做移动端像素断言。** 自检轨的回读断言已经是机器判定，可视轨继续用人眼 + CPU 对照。
- **不做自动轮播。** 桌面版没有这个功能，移动端也不加 —— 输入层两端分叉会给「移动端表现不同」多添一个不必要的嫌疑人。三条理由见 §5.3。
- **不改 `assets/config/Android/Tiny3D.cfg`。** MSAA 路径已由 PostProcessingApp 在真机上验过。
- **不把命令源做成反射组件。** 与 PostProcessingApp 同一规矩：sample 内部的裸接口实现，不进 `TCLASS()` / 序列化。

---

## 2. 现状盘点

### 2.1 已经就绪（这次移植比 PostProcessingApp 轻得多的原因）

| 项 | 位置 | 说明 |
|----|------|------|
| **CMake Android 分支** | `ComputeApp/CMakeLists.txt:86-124` | `add_library(SHARED)` + 链接引擎各模块；PRE_LINK 建 `Android/app/libs/${ANDROID_ABI}` 并拷 SDL2；POST_BUILD 拷 `Tiny3D.cfg` 与 `GLES3Renderer.so`。**与 `PostProcessingApp/CMakeLists.txt` 的 Android 段逐行相同** |
| 参与 Android 构建 | `Samples/CMakeLists.txt:35` | 在 `TINY3D_OS_DESKTOP` 守卫之外 |
| compute 内核的后端选源 | `ComputeKernel.cpp:77-87` | `T3D_OS_ANDROID` 分支已有，非 Vulkan 一律取 `source.gles` |
| 图形 shader 的后端选源 | `ParticleSystem.cpp:71-73` | 同上 |
| ESSL 变体 | `ComputeShaderSources.cpp` | 11 个内核写了 9 个。缺的两个是 `makeArgsSource:354` 与 `filterCountSource:388`，对应的 K7 / K8 本来就不在 GLES3 的 `backendMask` 里 |
| 用例的后端掩码 | `ComputeCases.cpp:110 / :187 / :301 / :397 / :484 / :569 / :644 / :734` | K1/K2/K4 是 `kBackendAll`，其余按后端能力精确标注，Android 上零改动 |
| 后端识别 | `ComputeCases.cpp:808-831` | `currentBackendBit()` 已认 `RHIRenderer::OPENGLES3` |
| GL 家族的 UAV 槽位换算 | `ComputeCases.cpp:46-50`、`ParticleSystem.cpp:107-110` | `srvCount + uRegister`，与 GLES3 的 `glBindBufferBase(GL_SHADER_STORAGE_BUFFER, startSlot + i)` 对得上（§6.4） |
| **GLES3 compute 全链路** | `T3DGLES3Context.cpp:2362-3089` | `createComputeShader` / `setComputeShader` / `dispatch` / `dispatchIndirect` / `uavBarrier` / `copyStructureCount` / `renderIndexedIndirect` 全是真实现，不是 stub |
| GLES3 能力位 | `T3DGLES3Context.cpp:109-146` | compute / UAV / structuredBuffer / indirectDraw / indirectDispatch / appendConsume 一律跟随 `has31`；`supportsReadback` 无条件 true |
| GLES3 缓冲回读 | `T3DGLES3Context.cpp:117` 起 | 自检轨全部 `verify()` 依赖的 `map` / `unmap` 是真实现 |
| 触摸输入 API | `T3DInput.h` | `TouchPhase` / `Touch` / `getTouchCount` / `getTouch` |
| 手势参考实现 | `PostProcessingApp/TouchCommandSource.cpp` | 单指 tap / 双击 / 拖拽 + 双指的完整状态机，阈值已按屏宽百分比算 |
| Android 工程模板 | `PostProcessingApp/Android/`（44 个已跟踪文件） | 最近一次落地的模板，§4 整份照抄 |

### 2.2 缺的

| 项 | 现状 |
|----|------|
| Android Gradle 工程 | `ComputeApp/` 下**没有 `Android/` 目录**，整套 gradle / manifest / java 都不存在 |
| 触摸输入 | `ComputeApp::pollKeys`（`ComputeApp.cpp:71-139`）只读 `T3D_INPUT.getKeyDown` |
| VS 侧 SSBO 的能力探测 | 见 §0，**这是唯一一条会静默画错的路径** |
| GLES3 真机验证 | ComputeApp 的落地实测（设计文档 §9.1 的 P1–P10）全部来自 D3D11；GL4 只在提交 `8e47e136` 里修过接口块命名。**GLES3 一次都没跑过** |

---

## 3. 为什么 iOS 本轮不做

三条各自独立、都足以卡死：

1. **shader 变体没有 iOS 分支。** `ComputeKernel.cpp:88-92` 与 `ParticleSystem.cpp:74-78` 的 `#else` 分支直接 `code = nullptr`，iOS 下所有内核创建失败。
2. **Metal 后端的 compute 是 stub。** `RHI-Compute-UAV-Indirect-Draw-Design-todo.md` §10 的 E3 未开工，`setComputeShader` / `setCS*` 四个方法当前是带断言的 stub。
3. **工程文件不存在。** `ComputeApp/CMakeLists.txt:152` 引用了 `iOS/Info.plist`，但该文件并未建立。

iOS 的前置是 `doc/todo/Metal-Renderer-Backend-todo.md` 的 E3。

---

## 4. 工作一：Android Gradle 脚手架（照抄 `PostProcessingApp`）

> **原则：整份复制 `source/Samples/PostProcessingApp/Android/`，只改必须改的 6 处。不要自己拼工程，不要「顺手优化」。** 全仓 18 个 sample 的 Android 工程是同一套模板，任何自创的偏差以后都会变成只有这一个 sample 有的怪毛病。

### 4.0 为什么是 `PostProcessingApp`

| 对照项 | 结论 |
|--------|------|
| **`CMakeLists.txt` Android 段** | `ComputeApp/CMakeLists.txt:86-124` 与 `PostProcessingApp/CMakeLists.txt` 的对应段**逐行相同**（同样建 libs 目录、拷 SDL2、拷 cfg、拷 `GLES3Renderer`），连 POST_BUILD 的写法都一致 |
| **构建位置** | `Samples/CMakeLists.txt:29` / `:35`，都在 `TINY3D_OS_DESKTOP` 守卫之外 |
| **模板新鲜度** | `PostProcessingApp/Android/` 是仓库里最近一次落地并在真机上验过的那份（44 个文件，比 `BehaviourApp` 的 42 个多了 `.idea/AndroidProjectSystem.xml` 与 `.idea/caches/deviceStreaming.xml`） |
| **输入层同构** | 本文工作二要照抄的命令源实现也在这个 sample 里，两处参照同源，减少心智负担 |

唯一的差异是反射：`PostProcessingApp` 有 `TCLASS()` 组件，`ComputeApp` 一个都没有（热键逻辑直接写在 `ComputeApp::pollKeys` 里，不是 Behaviour；`ComputeApp/CMakeLists.txt` 里也没有 `tiny3d_enable_reflection`）。**这不构成改动理由**，见 §4.3 第 1 条。

### 4.1 要复制的东西

`PostProcessingApp/Android/` 下纳入版本管理的共 44 个文件。整目录复制过来，然后**删掉复制时一起带过来的产物**：`app/build/`、`app/.cxx/`、`app/libs/`、`.gradle/`、`.idea/workspace.xml`、`local.properties`。`.gitignore` 挡得住提交，挡不住文件夹复制。

| 组 | 内容 |
|----|------|
| 根目录 | `build.gradle`、`settings.gradle`、`gradle.properties`、`gradlew`、`gradlew.bat`、`.gitignore` |
| Gradle wrapper | `gradle/wrapper/gradle-wrapper.jar` + `.properties`；**别漏 jar**，漏了 `gradlew` 跑不起来 |
| app 配置 | `app/build.gradle`、`app/CMakeLists.txt`、`app/proguard-rules.pro`、`app/.gitignore` |
| Java | `app/src/main/java/com/tiny3d/postprocessingapp/PostProcessingAppActivity.java` → 要改，见 §4.2 |
| 清单 / 资源 | `AndroidManifest.xml`、`res/values/{strings,colors,styles}.xml`、`res/drawable*/`、`res/mipmap*/`（图标 png 共 10 个） |
| assets | `app/src/main/assets/Tiny3D.cfg`，见 §4.4 |
| IDE | `.idea/`（13 个文件） |

### 4.2 只改这 6 处

| # | 文件 | 改什么 |
|---|------|--------|
| 1 | `app/build.gradle:9` | `namespace 'com.tiny3d.postprocessingapp'` → `'com.tiny3d.computeapp'` |
| 2 | `app/build.gradle:12` | `applicationId "com.tiny3d.postprocessingapp"` → `"com.tiny3d.computeapp"` |
| 3 | java 文件路径 | 目录改成 `java/com/tiny3d/computeapp/`，文件名改成 `ComputeAppActivity.java` |
| 4 | java 文件内容 | `package` 行、类名、`System.loadLibrary("ComputeApp")`、`getMainSharedObject()` 返回 `"libComputeApp.so"` |
| 5 | `AndroidManifest.xml` | `android:name=".PostProcessingAppActivity"` → `".ComputeAppActivity"` |
| 6 | `res/values/strings.xml` | `app_name` → `ComputeApp` |

改完的 Activity 就是这 28 行，除三处名字外与 `PostProcessingAppActivity.java` 一字不差：

```java
package com.tiny3d.computeapp;

import com.tiny3d.lib.Tiny3DActivity;

public class ComputeAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("ComputeApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] { "SDL2" };
    }

    @Override
    protected String getMainSharedObject() {
        return "libComputeApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
```

注意 `getLibraries()` 里**只有 `SDL2`**，没有 `GLES3Renderer` —— 渲染插件是引擎按 `Tiny3D.cfg` 运行时加载的，不走 Java 侧 `loadLibrary`。

**不要改**的地方（这几处看着像要跟着变，其实两个 sample 目录同深度）：

| 位置 | 值 | 为什么不用改 |
|------|-----|-------------|
| `settings.gradle:3` | `new File('../../../Platform/Android/')` | 从 `Samples/<X>/Android/` 上溯三级到 `source/` |
| `app/build.gradle:6` | `file("../../../../")` | 从 `Samples/<X>/Android/app/` 上溯四级到 `source/` |
| `app/build.gradle:51` | `path "../../../../CMakeLists.txt"` | 同上，指向 `source/CMakeLists.txt` |
| `app/build.gradle:21` | `-DTINY3D_BUILD_SAMPLES=TRUE` | 整个 Samples 目录一起构建，不是单 sample |

### 4.3 三条容易被「优化」掉的东西

1. **宿主 `rpp.exe` 的自动构建段不要删。** `app/build.gradle:30` / `:39` 的 `-DTINY3D_HOST_RPP=...` 与 `:62-63` 的 `apply from: tiny3d-build-host-rpp.gradle` 看起来与 ComputeApp 无关 —— 它确实一个 `TCLASS()` 都没有 —— 但 **`T3DCore` 自己依赖反射代码生成**，交叉编译时必须用宿主机的 rpp。删了就是编不过。
2. **`app/CMakeLists.txt` 不要删。** 它是 Android Studio 模板残留（`add_library(native-lib ... src/main/cpp/native-lib.cpp)` 引用的文件不存在，实际生效的是 `app/build.gradle:49-53` 指向引擎根 CMake 的 `externalNativeBuild`），但全仓 sample 一个不落全都留着它。删掉就成了唯一的偏差。
3. **`.idea/` 要一起复制。** 是既有约定，里面的路径都是 `$PROJECT_DIR$` 相对形式，可移植。`.idea/workspace.xml` 被 `.gitignore` 挡着，不会跟进来。

### 4.4 `assets/Tiny3D.cfg`：跟着提交一份

`ComputeApp/CMakeLists.txt:114-120` 的 POST_BUILD 会自动把 `assets/config/Android/Tiny3D.cfg` 拷进 `Android/app/src/main/assets/`，按说不必提交；但仓库惯例是提交一份（与源文件字节相同）。好处是没跑过 native 构建也能直接开 Android Studio 同步工程。代价是这份副本会随源文件漂移 —— 改了源 cfg 之后记得跑一次构建让 POST_BUILD 覆盖，再一起提交。

注意 `ComputeApp/CMakeLists.txt:113` 那行注释说明了它与其它 sample 的一处区别：**compute shader 走嵌入式常量，运行时不读磁盘**，所以 assets 里只需要 cfg，不需要 `assets/samples/shaders/`。

### 4.5 CMake 侧零改动

`ComputeApp/CMakeLists.txt` 一行都不用动：

| 行 | 做什么 |
|----|--------|
| `:88-100` | `add_library(${LIB_NAME} SHARED ...)` + 链接引擎各模块 |
| `:105-109` | PRE_LINK 建 `Android/app/libs/${ANDROID_ABI}/` 并拷 SDL2 |
| `:114-120` | POST_BUILD 建 assets 目录并拷 `Tiny3D.cfg` |
| `:121-123` | POST_BUILD 拷 `GLES3Renderer` 的 `.so` |

`Samples/CMakeLists.txt:35` 也已经注册在守卫之外。所以 §4.1 复制完 + §4.2 改完 6 处，`Android/app/libs/` 和 assets 都会在第一次 native 构建时自动填好，不用手放任何 `.so`。

---

## 5. 工作二：输入解耦 + 触摸手势

全部改动在 `source/Samples/ComputeApp/` 内，不碰引擎。可以先在 Windows 上验收桌面回归，再上真机。

### 5.1 命令层

新增 `ComputeCommand.h`，结构照抄 `PostProcessCommand.h`：

```cpp
enum class ComputeCommandType
{
    kNone = 0,
    kRetestAll,     // 桌面 R：重跑全部自检用例
    kRetestOne,     // 桌面 1-8：value = 用例下标
    kToggleCpu,     // 桌面 C：可视轨 GPU / CPU 切换
    kToggleCull,    // 桌面 V：GPU 剔除 + 间接绘制
    kTogglePause    // 桌面 P：冻结积分，方便抓帧
};

struct ComputeCommand
{
    ComputeCommandType type {ComputeCommandType::kNone};
    int32_t            value {0};
};

class IComputeCommandSource
{
public:
    virtual ~IComputeCommandSource() = default;
    /// 取出一条待处理命令，队列空返回 false
    virtual bool poll(ComputeCommand &cmd) = 0;
    /// 启动时打进日志的操作说明
    virtual const char *usage() const = 0;
};
```

命令用枚举、不传原始 scancode —— 这是「解耦」的实际含义。

带 `value` 的只有 `kRetestOne` 一条，而它**只给键盘用**。做了 §5.5.1 的增强后这里会再加两条不带 `value` 的相对命令（`kRetestNext` / `kRetestPrev`）供手势用，分工与 `PostProcessCommand.h` 的 `kSetPreset` vs `kNext` / `kPrev` 完全一致。

### 5.2 `ComputeApp` 的改动

**只改 `pollKeys`**（`ComputeApp.cpp:71-139`），`requestSelfTest` / `mParticles.setCpuMode` / `setCullEnabled` / `setPaused` 一行不动：

```cpp
void ComputeApp::pollCommands()
{
    ComputeCommand cmd;
    for (IComputeCommandSource *src : mSources)
    {
        while (src != nullptr && src->poll(cmd))
        {
            dispatchCommand(cmd);
        }
    }
}
```

`dispatchCommand` 把每类命令直连现有方法，其中 `kToggleCull` 保留现有的「剔除不可用时打日志而不是静默忽略」分支（`ComputeApp.cpp:120-123`）。

`mKeyFrame` 那套「一帧只响应一个键」的去抖（`ComputeApp.cpp:78-82`、每个分支末尾的 `mKeyFrame = frame`）**可以整体去掉** —— 命令源自带队列，一次 `poll` 一条命令，语义比按帧号去抖清楚得多。

头文件加 `void addCommandSource(IComputeCommandSource *src);` 和 `TArray<IComputeCommandSource*> mSources;`。命令源的生命周期由 `ComputeApp` 持有，在 `applicationWillTerminate()`（`ComputeApp.cpp:60-69`）里与 `mParticles.teardown()` 一起清掉。

**与 PostProcessingApp 的一处差异**：那边的宿主 `PostProcessControllerBehaviour` 带 `TCLASS()`，加成员会触发反射重新生成，所以文档专门叮嘱用裸接口指针。`ComputeApp` 不是反射类型，没有这个约束 —— 但**仍然用裸接口指针**，保持两个 sample 的写法一致。

### 5.3 两个命令源

| 类 | 文件 | 说明 |
|----|------|------|
| `KeyboardCommandSource` | `KeyboardCommandSource.{h,cpp}` | 把现有 `pollKeys` 里那串 `T3D_INPUT.getKeyDown(APP_SCANCODE_R / 1-8 / C / V / P)` 原样搬进来 |
| `TouchCommandSource` | `TouchCommandSource.{h,cpp}` | 手势状态机，照抄 `PostProcessingApp/TouchCommandSource.cpp` 的骨架，只换命令映射 |

**手势映射：**

| 手势 | 命令 | 对应桌面键 |
|------|------|-----------|
| 双击 | `kRetestAll` | `R` |
| 单指横向滑动 | `kToggleCpu` | `C` |
| 双指点击 | `kToggleCull` | `V` |
| 长按（> 800 ms 不移动） | `kTogglePause` | `P` |

**基线不映射 `1-8` 单跑用例。** 八个离散目标在触摸上没有自然对应，而这个功能本来就是桌面调试用的（`ComputeApp-Sample-Design-todo.md` §5.3 的键位表里就注明了「调试用」）。**若要做，不要照搬绝对下标 —— 正确形态是「在可跑用例间相对移动」，见 §5.5.1。**

`TouchCommandSource` 照抄时**不要改阈值算法**：`PostProcessingApp` 那份已经把阈值从绝对像素改成了按屏宽百分比（滑动 8%、tap 判定 2%，屏宽从 `T3D_AGENT.getDefaultRenderWindow()->getDescriptor().Width` 取），这是高 DPI 屏上不误判的前提。

**不做自动轮播，这条不留增强余地。** 三条理由，从强到弱：

1. **桌面版没有这个功能。** 加了就等于移动端多出一条桌面不存在的行为路径，与 §1.1 目标 5 要维护的两端对等性相冲突。这个 Sample 的价值在于「同一套用例在不同后端上的结果可比」，输入层两端分叉会让「移动端表现不同」多出一个不必要的嫌疑人。
2. **没有「不操作就什么都看不到」的问题。** PostProcessingApp 需要 `AutoCycleCommandSource` 是因为它的八个预设是纯画面效果、不轮播就看不到全部；ComputeApp 的可视轨默认就在动，自检轨启动即自动跑一遍。
3. **轮播自检用例更是无从谈起。** 自检轨的全部输出都在 logcat 里，屏上什么都不显示（§1.2 不做屏上 UI）。不盯 logcat 的人，轮播等于什么都没发生；而正在盯 logcat 的人，恰好就是能随手触发的人。

所以 `AutoCycleCommandSource` 这个类不需要移植过来，`TouchCommandSource` 也就不需要 `setAutoCycle()` / `notifyUserInteracted()` 那套接线 —— 照抄时把它们一并去掉，`enqueue` 只填 `mPending` 就够。

### 5.4 装配点

`ComputeApp::applicationDidFinishLaunching`（`ComputeApp.cpp:44-58`）末尾：现在是一句硬编码日志

```cpp
APP_LOG_DEBUG("[ComputeApp] keys: R retest, 1-8 single case, C CPU/GPU, V cull+indirect, P pause");
```

改成按平台装配 source，并把日志换成遍历 source 的 `usage()` 输出，移动端自然就打手势说明。桌面装 `Keyboard`，Android 装 `Touch`，**两端各一个，没有第三种组合**。

### 5.5 可选增强：手势选用例

> §5.3 的基线是「五个热键 → 四个手势」，本节把第五个手势通道也用上，让移动端补齐桌面 `1-8` 的语义。**§5.5.2 的报告修复不是可选项** —— 一旦做了 §5.5.1，它就是前置条件。
>
> **本节只有手势，没有自动轮播。** 给可视轨三个模式加轮播源这条路评估过、已否决，理由见 §5.3 —— 一句话是桌面没有这个功能，移动端也不该有。

#### 5.5.1 纵向滑动：在「可跑用例」之间移动

**手势通道是空着的，不需要发明新手势。** `PostProcessingApp/TouchCommandSource.cpp:192` 的滑动判定是 `absX >= swipeThreshold() && absX > absY` —— 纵向滑动被这个条件整个筛掉了。补一个对称的 `absY >= swipeThreshold() && absY > absX` 分支即可，比三指点击那类既难判定又难记的手势干净得多。

**方向对应关系在真机上定，不要按数学直觉猜符号。** `Touch.position` 是屏幕坐标，y 轴向下为正，所以「向下滑」数值上是 `moved.y() > 0`；但在列表语义里「向下滑」通常表示看**前**一项。两种直觉相反，只能上手试。

**关键设计：命令必须是相对的，不带绝对下标。**

桌面的 `1-8` 是绝对寻址，直接搬到移动端会很难用：GLES3 上 8 个用例只有 3 个 `runnable`（K1 / K2 / K4，下标 0 / 1 / 3），其余 5 个按 `backendMask` 跳过。滑到 K5 时 `ComputeApp::onRender` 的 `if (!status.runnable || !status.setupOk) continue;` 直接跳过，只在报告里多一行 SKIP —— **滑了跟没滑一样**，8 个位置里 5 个是死档，用户的第一反应是手势坏了。

所以 §5.1 的命令枚举增加两条不带 `value` 的：

```cpp
    kRetestNext,    // 跳到下一个「可跑」用例并重跑
    kRetestPrev,
```

游标推进的逻辑放在 `ComputeApp::dispatchCommand` 里，因为只有它能看到 `mStatus[i].runnable`：

```cpp
case ComputeCommandType::kRetestNext:
{
    const int32_t next = findRunnableCase(mSelfTestCursor, +1);
    if (next >= 0)
    {
        mSelfTestCursor = next;
        requestSelfTest(next);
    }
    break;
}
```

`findRunnableCase(from, step)` 在 `mStatus` 里按方向找下一个 `runnable && setupOk` 的下标，环绕，**一个都找不到时返回 -1** —— 此时手势应当沉默（并打一条日志），而不是空转或者卡在某个跑不了的用例上。

这个分工不是新发明，是 PostProcessingApp 已经确立的那条：

```34:42:source\Samples\PostProcessingApp\PostProcessCommand.h
    enum class PresetCommandType
    {
        kNone = 0,
        kSetPreset,     // value = 目标预设 0-7
        kNext,          // 下一个预设（7 之后回 0）
        kPrev,
        kSwapOrder,     // 对应桌面 O 键
        kToggleLog      // 对应桌面 L 键
    };
```

带绝对值的 `kSetPreset` 给键盘，不带值的 `kNext` / `kPrev` 给手势。ComputeApp 照此：`kRetestOne(value)` 仍然只给键盘的 `1-8`，新增两条只给手势。

**一个固有限制**：屏上没有任何反馈（§1.2 不做屏上 UI），当前选中第几个用例只能数 logcat。所以这个手势的适用场景是「手上只有一台机器，想快速扫一遍每个用例的表现」；如果目的是**调试**（某个用例在真机上挂了要反复跑 + 抓帧），把「单跑哪个」做成启动参数或 cfg 字段更可靠 —— 那种场景本来就在插线重装 APK，改一个值比在屏幕上划来划去准确。

加完之后的手势全貌，五个通道对齐桌面五类热键，一一对应无剩余：

| 手势 | 命令 | 对应桌面键 |
|------|------|-----------|
| 双击 | `kRetestAll` | `R` |
| 单指横滑 | `kToggleCpu` | `C` |
| **单指纵滑（上 / 下）** | **`kRetestPrev` / `kRetestNext`** | **`1-8`**（语义收窄为「在可跑用例间移动」） |
| 双指点击 | `kToggleCull` | `V` |
| 长按（> 800 ms 不移动） | `kTogglePause` | `P` |

#### 5.5.2 前置修复：`reportSelfTest` 的假阴性

**这条不是可选项。** 做了 §5.5.1 就必须一起修，否则手势一用就误报。

`ComputeApp::onRender` 的自检轨循环每轮开头无条件重置状态：

```176:187:source\Samples\ComputeApp\ComputeApp.cpp
            CaseStatus &status = mStatus[i];
            status.recordOk = false;
            status.passed = false;

            if (!status.runnable || !status.setupOk)
            {
                continue;
            }
            if (mSelfTestOnly >= 0 && int32_t(i) != mSelfTestOnly)
            {
                continue;
            }
```

单跑模式下，没被选中的用例 `passed` 被清成 false 却不会重跑，接着 `reportSelfTest` 的 `runnable && !passed` 这一支（`ComputeApp.cpp:336-341`）把它们全打成 FAIL。于是在 GLES3 上单跑 K1，报告会是 `1 passed, 2 failed, 5 skipped` —— **那两个 FAIL 是假的**。

桌面上这个缺陷影响有限，因为你知道自己刚按了 `1`。移动端完全不同：手势容易误触，而屏上没有反馈，唯一信息源就是 logcat —— 看到 "2 failed" 的第一反应必然是真挂了，然后去查一个不存在的问题。这与 §0 那条「静默画错」是同一类危害：**把排查引向错误方向的代价远大于功能本身的价值**。

修法取「给 `CaseStatus` 加一个本轮是否参与的标记」，让报告能区分「本轮没跑」和「本轮跑了但失败」，输出里前者显示为 `-- `（未参与）而不是 `FAIL`。选它而不是「单跑模式下只报那一个」，是因为它顺带把桌面 `1-8` 的同一个误报也修掉了。

---

## 6. 工作三：GLES3 上跑成什么样，以及要盯哪几条

### 6.1 自检轨的预期结果

各用例的 `backendMask` 已经写对了，Android 上不需要改任何一行：

| 用例 | GLES3 | 原因 |
|------|-------|------|
| K1 linear write | **跑** | `kBackendAll`（`ComputeCases.cpp:110`） |
| K2 saxpy | **跑** | `kBackendAll`（`:187`） |
| K3 raw typed | 跳过 | `kBackendD3D11`（`:301`）—— ByteAddress / Typed 在 GL 上靠折叠成 SSBO，未验证 |
| K4 reduce barrier | **跑** | `kBackendAll`（`:397`） |
| K5 texture UAV | 跳过 | `kBackendD3D11`（`:484`）—— 全仓没有 `glBindImageTexture`，GL 家族静默失效 |
| K6 reflect | 跳过 | `kBackendD3D11`（`:569`） |
| K7 dispatchIndirect | 跳过 | `kBackendD3D11 \| kBackendGL4`（`:644`） |
| K8 copyStructureCount | 跳过 | `kBackendD3D11 \| kBackendGL4`（`:734`） |

**真机上的期望日志是 `3 passed, 0 failed, 5 skipped`。**

有一个陷阱要写进验收流程：`reportSelfTest` 的那句总结

```345:348:source\Samples\ComputeApp\ComputeApp.cpp
    if (failed == 0)
    {
        APP_LOG_DEBUG("[ComputeApp] K1..K8 all passed");
    }
```

只看 `failed == 0`，跳过 5 个也照打「all passed」。**真机验收不能只看这一行**，必须逐条核对 5 条 SKIP 的原因串是 `backend not in case's backendMask`（`ComputeCases.cpp:838`）而不是 `requiredCaps` 没满足 —— 后者意味着 `has31` 为 false，即设备根本没有 ES 3.1，那是另一回事。

### 6.2 K7 / K8 能不能在 GLES3 上放开（M6 可选）

能。ES 3.1 的 `glDispatchComputeIndirect` 与 atomic counter 都是 core 能力，`GLES3Context::dispatchIndirect`（`:2941-2978`）与 `copyStructureCount`（`:3002-3056`）都是真实现，后者走 `glCopyBufferSubData` 把 counter 搬进 args buffer，路径完整。

缺的只是两份 ESSL 源码：`makeArgsSource()`（`ComputeShaderSources.cpp:354` 的 `make(hlsl, gl, nullptr)`）与 `filterCountSource()`（`:388`）。GL 变体已经写好，改成 ESSL 基本只是把 `#version 430` 换成 `#version 310 es`。连同放开两处 `backendMask` 大约半天。

**放在 M6，别和首次上机混在一起。** 首次上机要回答的问题是「这个 sample 在 Android 上能不能跑」，不是「能跑几个用例」。

### 6.3 真机要盯的五条

| # | 项 | 现象与处置 |
|---|----|-----------|
| **R1** | VS 侧 SSBO 不可用 | 见 §0。**上机前必须加保护**，否则会伪装成「GPU 算错」 |
| **R2** | compute program 每帧重建 | `setComputeShader` 每次都 `glCreateProgram` + `glAttachShader`（`T3DGLES3Context.cpp:2459-2465`），`dispatch` 里 `ensureComputeProgramLinked`（`:4183-4219`）走 `glLinkProgram`，`setComputeShader(nullptr)` 又 `GL_SAFE_DELETE_PROGRAM`（`:2431-2435`）。`ParticleSystem::record` 每帧这么来一到两轮（积分一次、开剔除时再一次）。移动端的 shader 编译器开销远大于桌面，可能直接掉到个位数帧率。**先测帧率，确认是它再考虑给后端加 program 缓存** —— 那是引擎改动，单独立项 |
| **R3** | `glUseProgram(0)` 之后图形 program 没重绑 | `setComputeShader(nullptr)` 会 `glUseProgram(0)`，而 `ensureProgramLinked` 开头是 `if (mCurrentProgram == 0 \|\| !mProgramDirty) return T3D_OK;`（`:2536-2539`）—— 不脏就直接返回，不会重新 `glUseProgram`。当前靠「每帧都调 `setVertexShader` / `setPixelShader` 把 dirty 置回来」（`:4336`）兜住了，但这是隐式依赖。真机上若出现「粒子偶尔整帧消失」，先查这里 |
| **R4** | `groupshared` 上限 | K4 用 `shared uint gs[256]` = 1KB，移动 GPU 普遍 16KB，安全。K0 已经打了 `maxComputeSharedMemory`（`ComputeApp.cpp:270`），真机核对一眼即可 |
| **R5** | MSAA + 手工绘制的深度 | cfg 是 `MSAA: 4`。`ParticleSystem::record` 在 `setRenderTarget` 之后自己 `clearDepthStencil(1.0f, 0)`（`ParticleSystem.cpp:607-613`，是 D3D11 上 P6 那个坑的处置）。这条在 GLES3 的 TBR 架构上要重新确认 —— 清深度在 tile 渲染里的代价和时机都和桌面不同。**若粒子整片不见，优先怀疑深度而不是 compute** |

### 6.4 已核对过、不需要担心的三条

写在这里是为了避免真机排查时把时间花在它们身上。

**绑定号对得上。** `ComputeApp` 的 ESSL 里手写了 `layout(std430, binding = N)`，C++ 侧 `uavSlot()` 在 GL 家族下做 `srvCount + uRegister` 偏移，而 GLES3 的 `setCSUnorderedAccessBuffers`（`:2868`）与 `bindStructuredBuffers`（`:4172`）都是 `glBindBufferBase(GL_SHADER_STORAGE_BUFFER, startSlot + i, ...)`。逐个对照：

| 内核 | ESSL binding | C++ 调用 |
|------|-------------|---------|
| particleUpdate | SSBO 0 = `gParticles` | `setCSUnorderedAccessBuffers(uavSlot(0, 0) = 0, ...)` |
| particleCull | SSBO 0 = `gParticles`（只读）、SSBO 1 = `gVisible`、atomic counter 1 | `setCSStructuredBuffers(0, ...)` + `setCSUnorderedAccessBuffers(uavSlot(1, 0) = 1, ...)`，后者同时把 counter 绑到 `GL_ATOMIC_COUNTER_BUFFER` 的同号槽位（`:2870-2884`） |
| particleDrawVS | SSBO 0 = `gParticles`、SSBO 1 = `gVisible` | `setVSStructuredBuffers(0, {particles, visible})` |

**常量缓冲按名字匹配，不按槽位。** `setCSConstantBuffers` 直接转 `stageConstantBuffers`（`:2477-2480`）并丢弃 `startSlot`，实际绑定发生在 `bindPendingUniformBlocks`（`:3744-3783`）：它枚举 program 的 active uniform block，剥掉 `type_` 前缀后按名字查 `mPendingUBOs`。ComputeApp 的三个 CB 名 `ParticleParams` / `CullParams` / `DrawParams` 与 ESSL 里的 block 名逐字一致，且每个 program 只有一个 block（index 0 → binding point 0），不会互相打架。ESSL 里写的 `layout(std140, binding = 0)` 会被 `glUniformBlockBinding` 覆盖，这不是问题。

**回读路径是真实现。** `supportsReadback` 无条件为 true（`:117`），自检轨 `verify()` 用的 buffer 回读走 PBO，不是 stub。GLES3 的回读限制是「纹理只支持 2D 彩色、不支持深度」—— 那条只影响 K5，而 K5 在 GLES3 上本来就跳过。

---

## 7. 文件改动清单

### 7.1 新增

| 文件 | 归属 |
|------|------|
| `source/Samples/ComputeApp/Android/**`（44 个文件） | 工作一，整份照抄 `PostProcessingApp/Android/`，只改 §4.2 那 6 处 |
| `source/Samples/ComputeApp/ComputeCommand.h` | 工作二 |
| `source/Samples/ComputeApp/KeyboardCommandSource.{h,cpp}` | 工作二 |
| `source/Samples/ComputeApp/TouchCommandSource.{h,cpp}` | 工作二 |
### 7.2 修改

| 文件 | 改什么 |
|------|--------|
| `ComputeApp.{h,cpp}` | `pollKeys` → `pollCommands` 遍历 source + `dispatchCommand`；加 `addCommandSource`；`applicationDidFinishLaunching` 按平台装配并改打 `usage()`；`applicationWillTerminate` 清理 source；**K0 增打 `GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS`**（§0 第 3 条） |
| `ComputeApp.{h,cpp}`（可选增强） | 加 `findRunnableCase` + `mSelfTestCursor`，`dispatchCommand` 处理 `kRetestNext` / `kRetestPrev`（§5.5.1）；**`CaseStatus` 加「本轮是否参与」标记并改 `reportSelfTest` 的输出分支**（§5.5.2，做了前者就必须做） |
| `ComputeCommand.h`（可选增强） | 枚举加 `kRetestNext` / `kRetestPrev` 两条，都不带 `value`（§5.5.1） |
| `TouchCommandSource.cpp`（可选增强） | 补纵向滑动分支 `absY >= swipeThreshold() && absY > absX`（§5.5.1） |
| `ParticleSystem.cpp` | **`setup` 探测 VS 侧 SSBO 可用性，不可用则强制 CPU 模式；`record` 检查 `setVSStructuredBuffers` 返回值**（§0 第 1、2 条） |
| `doc/todo/ComputeApp-Sample-Design-todo.md` | §1.2 与 §7 的 S5 行改指向本文；§10 文档关系表加一行 |

### 7.3 明确不改

- `ComputeApp/CMakeLists.txt`、`Samples/CMakeLists.txt`（Android 段与注册都已就绪，§4.5）
- `assets/config/Android/Tiny3D.cfg`
- 各用例的 `backendMask` / `requiredCaps`（§6.1 已经对了）
- `source/Core/**`、`source/Plugins/Renderer/**`（R2 若成立，单独立项）
- `ComputeShaderSources.cpp` 已有的 9 份 ESSL（M6 可选补两份，不动已有的）
- **不移植 `AutoCycleCommandSource`**：桌面没有自动轮播，移动端也不加，理由见 §5.3。连带 `TouchCommandSource` 的 `setAutoCycle()` / `notifyUserInteracted()` 一并不抄
- iOS 相关的一切

---

## 8. 分步实现顺序

| 步 | 内容 | 依赖 | 可验收点 | 状态 |
|----|------|------|----------|------|
| **M0** | 直接构建 `PostProcessingApp/Android` 出 APK | — | 证明 NDK 工具链、宿主 rpp、GLES3 后端三样都好，把工程问题与 ComputeApp 问题隔开 | 跳过，由 M4 直接验证（见下） |
| **M1** | §0 的三条：VS SSBO 探测 + 返回值检查 + K0 增打上限 | — | **Windows 回归**：D3D11 与 GL4 下行为一字不变 | ✅ 已落地 |
| **M2** | 命令层 + `KeyboardCommandSource`，`pollKeys` 改成遍历 source | — | **Windows 上 `R` / `1-8` / `C` / `V` / `P` 与改造前完全一致** | ✅ 已落地 |
| **M3** | `TouchCommandSource` | M2 | Windows 上编过；触摸暂时无法验 | ✅ 已落地 |
| **M4** | Android Gradle 脚手架（§4） | M0 | APK 能装能启动，logcat 出现 K0 能力位 | ✅ 出 APK；装机与 logcat 待 M5 |
| **M5** | 真机联调 | M1 + M3 + M4 | §9 全过 | 进行中：首次上机（模拟器）暴露 ESSL `#version` 位置问题，已修，待复跑 |
| **M6**（可选） | 补 K7 / K8 的 ESSL 变体，放开两处 `backendMask` | M5 | 5 passed / 3 skipped | 未开工 |
| **M7**（可选） | §5.5 的两项：报告假阴性修复 → 纵滑选用例 | M5 | §9.1 的「单跑 K1 不得出现 FAIL」一行 + §9.2 第 11 条 | 未开工 |

**M0 实际上被 M4 吸收了。** 原计划先拿 `PostProcessingApp/Android` 出一次 APK 来隔离工具链问题，实际执行时 M4 的首次构建一次就过（25 分 39 秒，`arm64-v8a` + `x86_64` 双 ABI），工具链、宿主 rpp、GLES3 后端三样同时得到验证，就没有必要再单独跑一遍 M0。**若 M4 当时失败，仍应退回先跑 M0**，这条隔离手段本身没有过时。

M4 的实测产物核对：`app/build/outputs/apk/debug/app-debug.apk`，`aapt dump badging` 显示 `package=com.tiny3d.computeapp`、`application-label=ComputeApp`、`launchable-activity=com.tiny3d.computeapp.ComputeAppActivity`、`native-code='arm64-v8a' 'x86_64'`；APK 内含两个 ABI 的 `libComputeApp.so`、`libSDL2.so`、`libGLES3Renderer.so`、`libT3DCore.so` 与 `assets/Tiny3D.cfg`。`.gitignore` 挡住了 `build/`、`.cxx/`、`.gradle/`、`libs/`、`local.properties`，纳入版本管理的正好是照抄过来的 44 个文件。

M1 与 M2 都有桌面回归风险，各自单独在 Windows 上验收完再上机。M0 与 M1 / M2 / M3 完全并行。

**M7 内部的顺序不能换**：先修 §5.5.2 的报告假阴性，再加 §5.5.1 的手势。反过来做的话，手势一上手就会看到假的 FAIL，而那时你还分不清是手势接错了、用例真挂了、还是报告在骗人。

**M0 提到最前面的理由与 PostProcessingApp 那次相同**：真机联调时最不想遇到的就是工具链问题和渲染问题混在一起分不清。`PostProcessingApp/Android` 已经在仓库里、CMake 与 ComputeApp 逐行相同，能出 APK 就说明三样都没问题，同时还顺带验证了 §4 要照抄的那份模板本身是好的。

---

## 9. 测试要点

### 9.1 桌面回归（M1 / M2 之后立刻做）

| 用例 | 期望 | M1–M3 实测 |
|------|------|-----------|
| D3D11 下 `R` / `1-8` / `C` / `V` / `P` | 与改造前完全一致；自检轨 8 passed | ✅ `8 passed, 0 failed, 0 skipped`，与改造前逐条相同 |
| D3D11 下 K0 新增字段 | 打出 `vertexStageStructuredBuffer` | ✅ `[K0] vertexStageStructuredBuffer=1`，`[particles] ready=1 cpuFallback=0 cullReady=1` |
| GL4 下同上 | 自检轨 **5 passed / 3 skipped**（K3 / K5 / K6 是 D3D11 only） | ✅ `5 passed, 0 failed, 3 skipped`，日志零 ERROR；修的是后端的跨线程读句柄，见 §9.1.1 |
| D3D11 debug layer | 跑完一轮自检 + 30 秒可视轨，零 ERROR / 零 WARNING（设计文档 §8.1 第 1 条） | ✅ 零 ERROR / 零 WARNING |
| 退出时 `ReportLiveDeviceObjects` | 无新增残留（§0 的改动碰了 `ParticleSystem`，P7 / P10 那两个坑要复核） | ✅ 只剩 `Live ID3D11Device`，与 TextureApp 对照一致 |
| **D3D11 下按 `1` 单跑 K1**（M7 之后） | 报告为 `1 passed, 0 failed, 0 skipped` + 7 条「本轮未参与」；**不得出现 FAIL** | 未做（M7 未开工），既有误报仍在 |

原表写的「GL4 6 passed / 2 skipped」是笔误：K3 的 `backendMask` 早在 S1b 就按实测收敛成 `kBackendD3D11`（`ComputeCases.cpp:301`），所以 GL4 的正确期望是 **5 passed / 3 skipped**。

#### 9.1.1 GL4 自检轨：主线程读 GL 句柄的竞态（已修）

M1 / M2 的桌面回归做到 GL4 时发现自检轨全线不过。先确认了这不是本次改动引入的：把 `source/Samples/ComputeApp` 的全部改动 `git stash` 掉、重新配置构建、再跑一次 GL4，得到逐条相同的 `0 passed, 5 failed, 3 skipped` 与相同的错误串，所以是 HEAD 上就坏的。

现象（NVIDIA / GLSL 4.50）：

```
ERROR|T3DGL4Context.cpp(3497)|copyStructureCount : underlying GL objects are not ready !
ERROR|T3DGL4Context.cpp(4859)|GL4Context::doBlit : GL Error 0x0506        // GL_INVALID_FRAMEBUFFER_OPERATION
ERROR|T3DGL4Context.cpp(2586)|GL4Context::setComputeShader : GL Error 0x0501   // ×7
---- 0 passed, 5 failed, 3 skipped ----
```

**根因**：`T3D_ENABLE_RHI_THREAD=1`，GL 对象的名字（`GLBuffer` / `GLCounterBuffer` / `GLShaderHandle` / `GLTexture` / `GLSampler`）是在 RHI 线程执行创建命令时才写进 RHI 资源的，而 GL4 后端的一大批 `setXXX` 把句柄**在主线程入队之前**就抽成 POD 数组再塞进 lambda（`bindPixelBuffers` 上方那句注释「避免在 lambda 中访问引擎对象」就是这个思路的出处）。命令队列本身是保序的，所以在 lambda 里取句柄一定拿得到；在主线程提前取，只要资源的创建和使用落在同一批命令里，读到的就是还没被写过的 0。

自检轨恰好就是这个形状：`setup()` 建缓冲和 shader、第一帧 `onRender()` 立刻就用。定位的决定性证据在日志顺序上 —— `RHI thread: GL context acquired successfully.` 出现在 `copyStructureCount` 报错**之后**，说明自检跑 `record()` 时 RHI 线程一条创建命令都还没执行。三条错误串由此全部解释得通：UAV/SSBO 以句柄 0 绑定，compute 写进了「零号缓冲」，于是 K1/K2/K4 回读全 0；`glAttachShader(program, 0)` 产生的正是 `0x0501 = GL_INVALID_VALUE`（不是先前猜的 `GL_INVALID_OPERATION`），七次对应七次真实的 `setComputeShader`；`doBlit` 的 `0x0506` 同理来自还没建出来的 FBO 附件。

**修法**：把句柄解析统一挪进 lambda 内部，参数改传智能指针（`RHICommandT` 对参数做 `remove_reference` 后按值存进 tuple，引用参数不会悬空）。`GL4Context::map` 一直是这么写的，可以当作范本。改到的位置：`setVertexShader` / `setPixelShader` / `setComputeShader` / `attachGraphicsShader`（新增文件级 `getGLShaderHandle` helper）、`stageConstantBuffers`、`bindPixelBuffers`、`bindSamplers`、`bindStructuredBuffers`、`setCSUnorderedAccessBuffers`、`dispatchIndirect`、`renderIndirect`、`renderIndexedIndirect`、`copyStructureCount`、`copyBuffer`。原先在主线程做的 0 值检查一并挪进 lambda，避免把「还没建好」误报成「参数非法」。

**结果**：GL4 `5 passed, 0 failed, 3 skipped`，全程零 ERROR / 零 WARNING，可视轨的 GPU 粒子与间接绘制正常出画；D3D11 仍为 `8 passed, 0 failed, 0 skipped`（只改了 GL4 插件，未触碰其他后端）。

#### 9.1.2 GLES3 的同源竞态（已同步平移，待真机验证）

GLES3 是 GL4 的同源分支，§9.1.1 那一套主线程取句柄的写法在它身上一处不差地存在。**桌面上没有任何办法验证它**，只有上机才会暴露，而暴露出来的症状极具误导性：K1 / K2 / K4 回读全 0 报 FAIL、粒子可能整片不出来，logcat 里却只有一句 `setComputeShader : shader has no RHI object !`——很容易被当成设备驱动或 ESSL 写错。为了不让 M5 的第一次上机浪费在这上面，**在上机前就照 GL4 的改法平移掉了**。

改到的位置与 GL4 一一对应：`setComputeShader` / `attachGraphicsShader`（`setVertexShader` 与 `setPixelShader` 都委托给后者，所以 shader 侧只有两个入口）、`stageConstantBuffers`、`bindPixelBuffers`、`bindSamplers`、`bindStructuredBuffers`、`setCSUnorderedAccessBuffers`、`dispatchIndirect`、`renderIndirect`、`renderIndexedIndirect`、`copyStructureCount`、`copyBuffer`，并新增同名的文件级 `getGLShaderHandle`。

两处与 GL4 不同、值得单独记一笔：

- `bindSamplers` 的 `remapUnit` 会去查 `mCurrentPSVariant` / `mCurrentVSVariant` 等，**那些也是 RHI 线程的状态**，在主线程查到的是上一帧的 variant。这次把整个 `remapUnit` 一起挪进了 lambda，顺带把六个分支合并成一次遍历。
- `setVSStructuredBuffers` 里 `mMaxVertexShaderStorageBlocks == 0` 的那道闸**保持同步不动**。它读的是初始化时查到的能力值而不是 GL 对象句柄，不属于这个竞态；而 §0 的 VS 侧 SSBO 探测正是靠这道闸同步返回错误码才成立的，挪进 lambda 会让探测永远返回成功。

**验证状态**：`assembleDebug` 通过（3 分 32 秒，双 ABI），`T3DGLES3Context.cpp` 自身零 warning。**运行时行为未验证**——真机上跑通之前，这条只能算「照着已验证的修法平移」，不能算已验收。

### 9.2 真机验收

| # | 用例 | 期望 | 易错点 |
|---|------|------|--------|
| 1 | 启动 | logcat 出现 `[K0] compute=1 unorderedAccess=1 structuredBuffer=1` | `has31` 为 false 说明设备不是 ES 3.1，换机 |
| 2 | K0 新增字段 | `maxComputeSharedMemory >= 16384`；VS SSBO 上限 > 0 | 上限为 0 时应看到明确的降级日志而非乱飞粒子（§0） |
| 3 | 自检轨 | `3 passed, 0 failed, 5 skipped` | **逐条核对 SKIP 原因是 `backendMask` 而非 `requiredCaps`**（§6.1） |
| 4 | 可视轨 | 粒子在动 | — |
| 5 | 单指横滑切 CPU 模式 | **画面看不出区别** | 这是可视轨唯一的判定依据，不是「好不好看」 |
| 6 | 长按 | 粒子冻结 | 长按期间手指微动，别掉进 drag 分支 |
| 7 | 双指点击开剔除 | 日志 `[K10] visible=N / 65536`，N 随相机自转变化且明显小于总数 | 双指落地有时序先后，别被识别成两次单指 tap |
| 8 | 双击 | 自检轨重跑，日志重新打一遍 | 与「两次快速滑动」的区分 |
| 9 | 记一次帧率 | GPU 路径不应明显慢于 CPU 路径 | 明显慢基本可以确认是 R2 的每帧 program 重建 |
| 10 | 全程 logcat | 不应有 `GLES3Context::` 开头的 `GL_CHECK_ERROR` 输出 | — |
| 11 | **纵滑选用例**（M7） | 每滑一次跳到下一个**可跑**用例并重跑，三次一循环（K1→K2→K4→K1）；**不应停在 K3 / K5-K8 上** | 停在跑不了的用例上说明游标没跳过 `!runnable`，退回 §5.5.1 |
| 12 | **不碰屏幕** | 粒子持续运动，模式**不自行改变** | 模式自己变了说明误抄了 `AutoCycleCommandSource`（§7.3） |

#### 9.2.1 首次上机发现：ESSL 的 `#version` 必须顶格

首次上机（Android 模拟器）结果 `0 passed, 3 failed, 5 skipped`，全部 ESSL 内核编译失败：

```
glslang parse error:
ERROR: #version: statement must appear first in es-profile shader; before comments or newlines
ERROR: 0:2: '#version' : must occur first in shader
```

**原因**：`ComputeShaderSources.cpp` 的 `gles` 串统一写成 `R"(` 换行再接 `#version 310 es`，于是源码第一个字符是换行、`#version` 落在第 2 行。桌面 GLSL 规范允许 `#version` 前面有空白和注释，**ES profile 不允许**，所以同样的写法 `gl` 串在 GL4 上一路绿灯，`gles` 串上机即死。代码库其余地方（`SampleShaders_gles3.h`）用的是 `"#version 310 es\n"` 拼接、天然顶格，只有 ComputeApp 用了 raw string 才踩到。

**修法**：10 处 `gles` 串一律改成 `R"(#version 310 es` 顶格起头，并在文件头的 `make()` 注释里写明原因，避免后人按 `gl` 串的缩进风格"顺手对齐"回去。`gl` 串保持原样——它们合法且已在 GL4 上验证通过，改了反而要重新回归。

**连带影响**：`ParticleDrawVS` / `ParticleDrawCpuVS` / `ParticleDrawPS` 三个图形着色器同样编不过，所以可视轨当时是被 `[particles] draw shaders unavailable, visual track disabled.` 整条关掉的——**§0 的降级保护按预期生效了**，没有伪装成"GPU 算错"或乱飞粒子。这条算是 M1 的意外验收。

#### 9.2.2 环境说明：Pixel 7 是 AVD 名字，不是真机

首次上机跑的是模拟器，logcat 里写得很清楚：

```
OpenGL ES Vendor: Google (NVIDIA Corporation)
OpenGL ES Renderer: Android Emulator OpenGL ES Translator (NVIDIA GeForce RTX 3060/PCIe/SSE2)
OpenGL ES Version: OpenGL ES 3.1 (4.5.0 NVIDIA 496.76)
```

这是 GLES-over-desktop-GL 的翻译层，底下是桌面 GL 驱动。**抓功能性 bug 有效**（§9.2.1 就是这么抓到的），但下面几条在模拟器上的结论一律不作数，必须真机复核：

- **R2 的帧率**：翻译层的 shader 编译开销与移动端编译器完全不是一回事
- **R5 的 MSAA + 深度清除**：模拟器不是 TBR 架构，tile 相关的代价和时机测不出来
- **`baseInstance=0`**：模拟器的 ES 3.1 不暴露 base instance，真机可能不同
- **驱动宽容度**：翻译层比 Mali / Adreno 原生驱动宽松，模拟器过了不代表真机过

另外 logcat 里有一条 `blit : scratch read FBO is incomplete !`（`T3DGLES3Context.cpp:4482`，渲染线程，每次启动一条）。它发生在可视轨被关掉之后，暂时归因不明，等 ESSL 修复后复跑再看是否自愈。

### 9.3 明确不测

- 像素级断言（自检轨的回读断言已经是机器判定）
- 深度纹理回读（GLES3 明确不支持）
- ES 3.0 设备（所有 ESSL 都是 `#version 310 es`，ES 3.1 是地板）
- iOS
- 桌面 D3D11 / GL4 之外的后端（VK / Metal / Console / Null 的 compute 是 stub，Sample 靠能力位跳过）

---

## 10. 风险

| 风险 | 缓解 |
|------|------|
| **VS 侧 SSBO 不可用却静默画错（R1）** | §0 的三条处置，排在 M1，上机前完成。本次移植有两条「不修就会误导排查方向」的问题，这是必做的那条，另一条见下面的报告假 FAIL（只在做 M7 时才存在） |
| **compute program 每帧重建拖垮帧率（R2）** | 先测再改。真是它的话，给 GLES3 后端加 program 缓存，**单独立项单独提交**，不混进本次移植 |
| 粒子整片不见 | 按 R5 → R3 → R1 的顺序查：先深度（`clearDepthStencil` 在 TBR 上的行为），再 program 绑定，最后才怀疑 compute 算错 |
| 自检轨「all passed」误读 | §6.1 的陷阱，验收流程里写死「逐条核对 SKIP 原因」 |
| **单跑手势导致报告假 FAIL** | §5.5.2 的修复是 §5.5.1 的硬前置，M7 内部顺序不能换。与 §0 同属「把排查引向错误方向」这一类危害，代价远大于功能本身的价值 |
| 照抄 `TouchCommandSource` 时把自动轮播一起抄过来 | 那份实现里 `setAutoCycle()` / `notifyUserInteracted()` 与 `AutoCycleCommandSource` 是耦合的，照抄时容易顺手带上。**桌面没有自动轮播，移动端也不要有**（§5.3）；§9.2 第 12 条就是这条的检查项 |
| 手势选用例停在跑不了的用例上 | 命令必须是相对的、游标必须跳过 `!runnable`（§5.5.1）。照搬桌面的绝对下标会让 8 个位置里 5 个变成死档 |
| **自己拼 Android 工程 / 顺手「优化」模板** | 整份照抄，只改 §4.2 那 6 处。特别是 §4.3 那三条 —— rpp 段别删（`T3DCore` 要）、`app/CMakeLists.txt` 别删、`.idea/` 要带上 |
| 复制工程时把构建产物一起带过来 | `app/build/`、`app/.cxx/`、`app/libs/`、`.gradle/`、`local.properties` 都要删；`.gitignore` 挡得住提交，挡不住文件夹复制 |
| 手势阈值用绝对像素 | 照抄 `PostProcessingApp/TouchCommandSource.cpp` 的百分比算法，别改 |
| 首次 Android 构建卡在 rpp | 既有机制，M0 先用 PostProcessingApp 验一遍 |
| 真机 GPU 驱动的 compute 质量 | `RHI-Compute-UAV-Indirect-Draw-Design-todo.md` §12.5 预警过「部分驱动 `glMemoryBarrier` 实现不完整」。K4（两级归约 + `uavBarrier`）是对这条最敏感的用例，它过了基本说明驱动可信 |

---

## 11. 与既有文档的关系

| 文档 | 关系 |
|------|------|
| `ComputeApp-Sample-Design-todo.md` | 本文是它 §7 **S5** 期的施工蓝图。用例矩阵、可视轨设计、`backendMask` 语义全部沿用；只替换输入层、增加 Android 平台。§9.1 的 P1–P10 是 D3D11 实测记录，本文 §6.3 是 GLES3 侧的对应清单 |
| `PostProcessingApp-Android-Design-todo.md` | 方法论来源与 Android 工程的照抄源。其 §6 的照抄纪律（只改 6 处、三条别优化、产物要删）本文 §4 全部沿用 |
| `RHI-Compute-UAV-Indirect-Draw-Design-todo.md` | §12.5 对移动端 compute 可用性的预警由本文实际检验；真机跑完后，§8.2 的 GLES3 能力表、§11.1 验证矩阵的 GLES3 列应据实回填。§10 的 **E4**（GLES3 ES3.1 分支真实现 + 设备黑名单）与本文 §0 / §6.3 发现的两条（VS SSBO 门槛、program 每帧重建）应合并考虑 |
| `GLES3-Renderer-Backend-todo.md` | R2 若确认成立，program 缓存记进该文档 |
| `GPU-Readback-onRender-Design-todo.md` | 自检轨全部 `verify()` 依赖它。ComputeApp 是它在 GLES3 上的第一个真实使用者 |
| `Metal-Renderer-Backend-todo.md` | iOS 的前置，见 §3 |
| `PostProcessingApp` | 命令源与 Android 工程的双重参照，本身不动 |

---

## 12. 一句话

**ComputeApp 上 Android 的构建侧早就准备好了（CMake Android 段、两处 `pickSource` 的 `T3D_OS_ANDROID` 分支、9 份 ESSL、用例的 `backendMask` 全在），真正要做的只有三件事：先把「VS 侧 SSBO 在 ES 3.1 上是可选能力」这条会静默画错的路径改成明确降级，再抄一套 Gradle 脚手架，最后把五个热键换成四个手势 —— 剩下的就是拿真机去看 GLES3 的 compute 是不是真的能算对。**
