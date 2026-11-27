# 《一起动手实现3D渲染引擎》书稿现状分析与改版方案

> 对象：`doc/一起动手实现3D渲染引擎.md`
> 依据：当前 `source/` 源码（分析日期 2026-11-19）
> 状态：第 0 阶段（编辑问题修复）、第 1 阶段（章节骨架重排）、第 2 阶段（第 1–3 章按代码重写）已完成；下一步为第 3 阶段
>
> 第 2 阶段遗留：待补图 图1-1、图1-2、图3-1（平台层类图，04-platform.png 与代码不符）、图3-6（HelloApp 时序图）、图3-7/3-8（Windows/Android 运行截图）；待证实项见各章 book-writer 报告（macOS cmake 流程、Android APK 内插件 .so 是否齐全、Linux 下 rpp 能否编译等）。

---

## 一、现状

### 1.1 现有大纲与完成度

| 部分 | 章节 | 完成度 |
|---|---|---|
| 工程篇 | 一、整体设计（目标 20 条、四块架构图） | 有正文，内容为早期形态 |
| | 二、开发环境（Windows / Android / iOS·macOS / Linux） | 有正文，内容为早期形态 |
| | 三、第一个程序 HelloApp（目录结构、Android 工程、生成脚本、单例 / 抽象工厂 / 桥接，Application / Window / Factory / System / Engine） | 有正文，内容为早期形态 |
| 数学篇 | 四、坐标系 | 有正文 |
| | 五、向量 | 有正文 |
| | 六、矩阵（线性变换、行列式、逆、正交、齐次、透视、仿射分解） | 有正文 |
| | 七、欧拉角和四元数 | 7.4.18 之后为空，7.6 转换全空 |
| | 八、几何图元（直线、球、平面、三角形、包围体） | 有正文 |
| | 九、几何检测（15 种相交） | 仅标题 |
| | 十、渲染流水线 | 仅标题 |
| 基础系统篇 | 十一、平台层（Console / DeviceInfo / IO / 时间） | 部分 |
| | 十二、日志系统 | 仅设计部分 |
| | 十三、事件系统 | 仅 13.1 |
| | 十四、内存管理（内存池 / 对象池 / GC / 引用计数） | 仅标题 |
| | 十五、插件系统（按 Windows / Linux / macOS / iOS 分节） | 仅标题 |
| | 十六、文件管理（虚拟 / 物理） | 仅标题 |
| 核心系统篇 | 十七、渲染系统（软渲染 / D3D / GL / GLES / Vulkan / Metal） | 仅标题 |
| | 十八到二十四：场景、资源、纹理和材质、模型和动画、字体、地形、粒子 | 仅标题 |
| 其他 | 二十五、后记（优化）；附录 A 奔驰定理 | 附录有正文 |

### 1.2 总体判断

- 第一到三章描述的是项目早期形态，和现在的代码差距很大。
- 第四到八章的数学理论基本完整，可以保留，但坐标系约定与代码相反。
- 第九章以后基本只有标题。
- 当前代码中一半以上的模块在书稿里没有对应章节。

---

## 二、与当前代码不一致（附证据）

### 2.1 坐标系约定相反（影响最大）

- 书稿 4.2、4.6、7.3.1（"+z 向后"）、8.6 的推导都约定**右手系**。
- 代码默认**左手系**：`source/Core/Include/T3DConfig.h` 中 `#define T3D_COORDINATION_RH 0`；`source/Core/Source/Component/T3DCamera.cpp` 据此调用 `perspective_LH`。
- "列向量、矩阵左乘向量"的约定与代码一致。
- 欧拉角：代码为 `fromEulerAnglesZXY`（`source/Math/Include/T3DMatrix3.inl`，\(M = Y \cdot X \cdot Z\)），概念上等价于书稿的 heading-pitch-bank，但书稿没有给出两者的对应关系。

处理：数学篇里的透视矩阵、叉乘方向、旋转正方向需要按左手系重新核对，或者明确说明"推导用右手系，引擎默认左手系，差异在哪几处"。

### 2.2 工程环境过时（第二章、第三章 3.1）

| 书稿 | 代码现状 | 证据 |
|---|---|---|
| C++11 | C++17 | `source/CMakeLists.txt` 第 153 行 |
| VS2015 + `generate-vs2015.bat` | 脚本覆盖 vs2015 到 vs2022，另有 ninja、build 脚本 | `source/Projects/` |
| ABI：armeabi / armeabi-v7a / x86 | arm64-v8a / armeabi-v7a / x86 / x86_64 | `dependencies/sdl2/prebuilt/android/` |
| `dependencies/SDL2/prebuilt/win32`、`Android` | `dependencies/sdl2/prebuilt/windows`、`android` | `dependencies/sdl2/` |
| 依赖仅 SDL2 | 另有 dxc、fbxsdk、freeimage、glad、glslang、llvm、zlib | `dependencies/` |
| Android Studio 3.0.1、Ubuntu 17.10、CMake 3.10 | 需按现有 gradle 工程重写 | `source/Platform/Android/`、`source/Samples/*/Android/` |

### 2.3 第三章的目录、类名、接口

- **目录**：书稿只有 `Core`、`Platform`、`Samples`。实际有 `Platform`、`Log`、`Utils`、`Reflect`、`System`、`Math`、`Framework`、`Core`（Runtime / Editor 两个目标）、`Plugins`、`Tools`、`Editor`、`Player`。`Platform/Include` 已按功能分子目录（`Application/`、`Window/`、`IO/`、`Thread/` 等），书中列出的源码路径（如 `Include/T3DApplication.h`、`Win32Factory.h`）均已失效。
- **`System` → `Platform`**：`source/Platform/Include/T3DPlatform.h`。接口由 `process()` 变为 `init()`、`poll()`、`memoryBarrier()`，并持有 ThreadManager、TimerManager、Console、Locale、DeviceInfo、ZipAssetManager 等。
- **`Engine` → `Agent`**：`source/Core/Include/Kernel/T3DAgent.h`。`startup()` 改为 `init(argc, argv, autoCreateWindow, isWindowApp, config)`；新增 `runForEditor`、插件安装 / 卸载、RHI 切换、`postFrameEndTask` 等。
- **`Application`**：`source/Platform/Include/Application/T3DApplication.h`。新增 Console / Window 两种类型（`T3DConsoleApplication.h`、`T3DWindowApplication.h`）；`init` 返回 `TResult`；`applicationDidFinishLaunching(argc, argv)` 带参数；新增 `applicationLowMemory`、焦点回调、`onPreRender` / `onRender` / `onPostRender`、事件监听器。
- **`IFactory`**：`source/Platform/Include/Adapter/T3DFactoryInterface.h`。书稿写 3 个接口，实际约 19 个（时间、定时器、Dir、DeviceInfo、Console、线程与同步对象、进程、Locale、FSMonitor、ZipAssetManager、SharedLibrary 等）。具体工厂另有 `T3DLinuxFactory`。
- **`Singleton`**：`source/Platform/Include/T3DSingleton.h`。现继承 `Noncopyable`，并有 `T3D_EXTERN_SINGLETON` / `T3D_INSTANTIATE_SINGLETON`（解决非 Windows 平台模板静态成员跨动态库问题），值得在书中专门讲。所有类继承 `Allocator`，使用 `T3D_NEW`。
- **`Window`**：`source/Platform/Include/Window/T3DWindow.h`。`create` 返回 `TResult`，另增约 15 个接口（鼠标、文本输入、剪贴板、光标等）。

与代码一致的部分：第十一章 `Console`、`IDeviceInfo` 代码片段（仅 `getSystemRAM` 返回值已改为 `uint64_t`）；第十二章日志设计（异步刷盘、过期清理、前后台切换）与 `source/Log/Include/T3DLogger.h` 吻合。

### 2.4 第一章目标与实现状态

- **渲染后端**：书稿列 GL 3.0/4.0、GLES 2.0/3.0、Vulkan、D3D9、D3D11。实际在 RHI 上可用的是 D3D11、OpenGL4、Vulkan、GLES3、Metal、Null（`source/Plugins/Renderer/CMakeLists.txt`）。D3D9、Reference3D 仍是旧 `HardwareBufferManager` 接口，未迁移到 RHI，默认不编译；OpenGLES2 只有 42 行占位；Metal 不在书稿目标里。
- **场景管理**：书稿写"简单 Scene Graph"，实际是 GameObject-Component 架构（`source/Core/Include/Kernel/T3DGameObject.h`、`Component/`），Scene 是 Resource，另有 Prefab。
- **地形、字体**：代码中不存在。**粒子**仅 `source/Samples/ComputeApp/ParticleSystem.h` 示例。
- 1.2 节四块架构图（Platform / Plugins / 3rd Party / Core）已无法反映现有分层。

### 2.5 几何章节与数学库

- 第八章包围体缺 **Capsule**、**Frustum**（`source/Math/Include/T3DCapsule.h`、`T3DFrustum.h`；`source/Core/Include/Bound/`）。
- 第九章列"两条射线相交"，代码无。代码中大量存在的点-*、胶囊-*、**视锥-***、球-三角形、胶囊-三角形检测（`source/Math/Include/T3DIntr*.h`）书稿未列。视锥检测是场景剔除核心。
- 数学篇只有理论，缺数学库实现：模板化 `Real`（float32 / float64 / fix32 / fix64）、SIMD（`*_SIMD.inl`）、`Degree` / `Radian`、Color。

---

## 三、模块划分问题

1. **"渲染流水线"归在"3D 数学"下**：应作为渲染篇开篇。
2. **顺序与代码依赖脱节**：第三章搭好 Platform / Core 后插入七章数学，再回到平台层。第十二章开头仍写"搭建完开发环境并且验证正常运行后，接下来就是设计和实现一套日志系统"，说明原顺序已被打乱。代码依赖顺序：Platform → Log → Utils / System / Reflect → Math → Framework → Core → Plugins → Tools / Editor。
3. **平台层拆成两处**：第三章讲 Application / Window / Factory，第十一章又讲平台层，应合并。
4. **IO、文件管理、资源管理边界不清**：代码中数据流（`DataStream` / `Dir`）在 Platform；`Archive` / `ArchiveManager` 抽象在 Core，实现为插件（FileSystem、Zip、Bundle、BundleFileSystem、AndroidAsset、MetaFileSystem）；`Resource` / `ResourceManager` 在更上层（UUID + 文件名双索引）。"虚拟 / 物理文件管理"实际就是 Archive 插件体系。
5. **"内存管理"内容代码中没有**：代码是 `Object` 引用计数 + 侵入式 `SmartPtr` + `ObjectTracer` + `MemoryManager`（Windows 调试跟踪），没有通用内存池和 GC。建议改为"对象系统与内存管理"，与 UUID 一起讲。
6. **"插件系统"按 OS 分节不合适**：OS 差异只在 `ISharedLibrary`（Win32 / Unix）。应讲 Plugin 生命周期（install / startup / shutdown / uninstall）、`dllStartPlugin`、Dylib 作为资源由 `DylibManager` 管理、按配置驱动加载，以及 Android 从 APK 解出 `.so` 再加载的流程（见 `.cursor/rules/zip-asset-manager-design.mdc`）。iOS 渲染插件目前全部注释掉。
7. **"渲染系统"按图形 API 分节不合适**：核心是 RHI 抽象（`RHIRenderer`、`RHIContext`、`RHIThread` 命令队列与帧同步，`source/Core/Include/RHI/`）+ `RenderPipeline`（`ForwardRenderPipeline`：剔除、渲染队列、动态合批、阴影、天空盒、后处理）。各 API 只是后端，合并为一章"后端实现差异"。
8. **"纹理和材质"过挤**：材质系统为 ShaderLab 风格 Shader / Technique / Pass / Variant / Keyword（`source/Core/Include/Material/`），加上 HLSL 交叉编译工具链，应独立成章；纹理与图像编解码（FreeImage 插件）另起一章。
9. **光照无章节**：代码有 Ambient / Directional / Point / Spot / Indirect（`source/Core/Include/Light/`）与阴影；第一章目标第 16 条也列了光照。

---

## 四、缺失与超前主题

### 4.1 代码已有、书稿缺失

- 反射：`TCLASS` / `TPROPERTY` 宏、RTTR、`Tools/ReflectionPreprocessor`
- 序列化：二进制与 JSON（`source/Core/Include/Serializer/`）
- Meta 资产系统（`source/Core/Include/Meta/`）、Prefab
- 线程与任务：`RunnableThread`、`QueuedJobPool`、同步对象、RHI 渲染线程
- 时间与定时器（`Kernel/T3DTime.h`、`TimerManager`；书稿 11.4 为空）
- 输入（`source/Core/Include/Input/`）、UI（`Plugins/RmlUi`、`UI/T3DUISystem.h`）
- 骨骼动画：Skeleton、AnimationClip、Track、Keyframe、Player、`SkinnedGeometry`
- 工具链：MeshConverter（FBX）、ShaderCrossCompiler、HLSLCross、ImageConverter、BundleBuilder、MetaGenerator、BuiltinGenerator
- 编辑器：TinyEditor、TinyLauncher（ImGui）
- Utils（哈希、编码、Variant、Tree）、Network、Locale / Language、Performance

### 4.2 书稿规划、代码无可用实现

地形、字体、粒子（仅示例）、软渲染（Reference3D 旧接口）、D3D9（旧接口）、GLES2（占位）、内存池 / GC。

建议移到"规划中"或后记，不作为已实现内容描述。

---

## 五、修改方案

### 5.1 新全书大纲（到章一级）

**第一部分 工程篇**
1. 整体目标与架构（按现有分层重画架构图与模块依赖图）
2. 开发环境与构建（CMake 选项、各平台生成脚本、依赖目录、Android gradle 工程）
3. 第一个程序 HelloApp（Application / Window / Platform / Agent 最小闭环，三种设计模式）

**第二部分 基础系统篇**
4. 平台层（合并旧第三章平台部分与旧第十一章：工厂与适配层、Console、DeviceInfo、IO 流与 Dir、FSMonitor、Locale、进程、动态库）
5. 线程、任务与时间（线程与同步对象、QueuedJobPool、TimerManager、DateTime、Time）
6. 日志系统
7. 对象系统与内存管理（Object、引用计数、SmartPtr、ObjectTracer、MemoryManager、UUID）
8. 反射与序列化（反射宏、ReflectionPreprocessor、RTTR、Bin / JSON 序列化）
9. 事件系统（同步 / 异步、广播 / 多播 / 单播、双缓冲队列、时间与栈深保护）
10. 插件系统（SharedLibrary、Plugin 生命周期、Dylib / DylibManager、配置驱动加载、Android 特殊流程）
11. 档案与资源管理（Archive 抽象与插件、ArchiveManager、Resource / ResourceManager、AssetManager 门面）

**第三部分 数学篇**（保留旧第四到八章理论，统一坐标系约定）
12. 坐标系
13. 向量
14. 矩阵
15. 欧拉角和四元数（补全 7.4.18 之后与 7.6 转换）
16. 几何图元与包围体（补 Capsule、Frustum）
17. 相交检测（按 `T3DIntr*` 重排，重点视锥检测）
18. 数学库实现（模板化 Real、定点数、SIMD、角度类型、颜色）

**第四部分 渲染篇**
19. 渲染流水线概念（由旧第十章移入）
20. RHI 与渲染线程
21. 渲染资源与渲染状态（Buffer、Texture、RenderTarget、各类 State）
22. 渲染管线（剔除、渲染队列、动态合批、天空盒、后处理）
23. 材质与着色器（Shader / Technique / Pass / Variant / Keyword，着色器跨平台编译）
24. 纹理与图像（Image、ImageCodec、FreeImage 插件、ImageConverter）
25. 光照与阴影
26. 渲染后端实现差异（D3D11、OpenGL4、Vulkan、GLES3、Metal、Null）

**第五部分 场景与内容篇**
27. GameObject 与 Component（Transform、Camera、Geometry、Behaviour）
28. 场景与 Prefab
29. 模型与骨骼动画（Mesh、SubMesh、Skeleton、AnimationClip、蒙皮）
30. 输入与 UI（Input、UISystem、RmlUi）

**第六部分 工具与编辑器篇**
31. 资产工具链（MeshConverter、ShaderCrossCompiler、BundleBuilder 等）
32. Meta 资产系统
33. 编辑器（TinyEditor、TinyLauncher）

**后记**：优化、规划中的功能（地形、字体、粒子、软渲染等）
**附录**：奔驰定理等数学证明

### 5.2 旧章节迁移映射

| 旧章节 | 处理 | 新位置 |
|---|---|---|
| 一、整体设计 | 重写 | 第 1 章 |
| 二、开发环境 | 重写 | 第 2 章 |
| 三、HelloApp | 拆分：3.1 目录与脚本并入第 2 章；3.2–3.5 按现有代码重写 | 第 2、3、4 章 |
| 四到八、数学 | 保留，统一坐标系约定；八补包围体 | 第 12–16 章 |
| 九、几何检测 | 重写（按代码重排） | 第 17 章 |
| 十、渲染流水线 | 移动 | 第 19 章 |
| 十一、平台层 | 合并 | 第 4 章 |
| 十二、日志系统 | 保留设计，补实现 | 第 6 章 |
| 十三、事件系统 | 补全 | 第 9 章 |
| 十四、内存管理 | 重写为对象系统 | 第 7 章 |
| 十五、插件系统 | 重写（不按 OS 分节） | 第 10 章 |
| 十六、文件管理 | 合并 | 第 11 章 |
| 十七、渲染系统 | 拆分 | 第 20–22、26 章 |
| 十八、场景管理 | 重写 | 第 27、28 章 |
| 十九、资源管理 | 合并 | 第 11 章 |
| 二十、纹理和材质 | 拆分 | 第 23、24 章 |
| 二十一、模型和动画 | 保留标题，补骨骼动画 | 第 29 章 |
| 二十二、字体 / 二十三、地形 / 二十四、粒子 | 移入规划中 | 后记 |
| 二十五、后记 | 保留 | 后记 |
| 附录 A | 保留，修正编号 | 附录 |

### 5.3 分阶段执行计划

| 阶段 | 内容 | 产出 | 备注 |
|---|---|---|---|
| 0 | 编辑问题修复（见第六节） | 编号、标签、路径正确的书稿 | 不涉及代码，风险最低 |
| 1 | 调整目录骨架：按新大纲重排章节标题与分页，空章节保留标题 | 新目录结构 | 只移动不改写正文 |
| 2 | 重写第 1–3 章 | 与当前代码一致的开篇 | 需重画：架构图、模块依赖图、Platform 类图、Agent 类图 |
| 3 | 基础系统篇第 4–11 章 | 平台、线程、日志、对象、反射、事件、插件、资源 | 需补：工厂类图、事件系统类图、插件加载时序图、Archive 类图 |
| 4 | 数学篇统一坐标系，补第 15–18 章空缺 | 完整数学篇 | 需补：胶囊体、视锥示意图 |
| 5 | 渲染篇第 19–26 章 | 渲染部分 | 需补：RHI 分层图、帧时序图、材质结构图 |
| 6 | 场景与内容篇、工具与编辑器篇 | 第 27–33 章 | 需补：Component 继承图、编辑器截图 |

### 5.4 写作统一约定

- **坐标系**：以引擎默认左手系为准；如保留右手系推导，必须在第 12 章给出与引擎的差异对照。
- **术语**：类名用代码原名（`Agent`、`Platform`、`RHIContext`），首次出现给中文说明；"档案"统一对应 `Archive`，"资源"对应 `Resource`，"资产"对应 Meta / 编辑器侧概念。
- **源码路径**：统一写成 `source/<模块>/Include/...` 的相对路径，与实际大小写一致。
- **编号**：章、节、图、表、公式均按"章号-序号"连续编号；重排后统一重编。
- **代码片段**：以当前源码为准，可删减成员（用 `// ...` 标出），不改签名。
- **未实现内容**：统一写成"后续章节再展开"或放入后记，不作为已实现描述。

---

## 六、编辑问题清单

- 5.1.2 出现两次
- 6.4.2 写成"8.4.2"
- 第十一章出现两个 11.2（设备信息应为 11.3，后续顺延）
- 第十一章正文出现"图10-3"
- 附录奔驰定理的公式编号 10.28 / 10.29、"图10-19"实际属于第八章 8.5.4
- 8.5.4 下缺少 8.5.4.1（奔驰定理移至附录后留下的空位）
- 附录图片使用绝对路径 `G:/Workspace/MyGame/Tiny3D/steps/doc/images/triangle-benz.png`
- 第二十一章标题 `<center>` 未闭合；第二十二章写成 `<cneter>`
- "地型"应为"地形"
- 3.2 节"GoF 提出的 24 种设计模式"应为 23 种
- 3.2.1 节与 3.2.3 节提到"图3-1""图3-2""图3-7"，与实际图号（图3-18 起）不符
