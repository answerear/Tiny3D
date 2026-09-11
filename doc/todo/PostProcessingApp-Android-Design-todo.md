# PostProcessingApp 移动端（Android）：设计与实施计划

> 让 `PostProcessingApp` 在 Android 真机上跑起来。桌面版靠 `0-7` / `O` / `L` 热键切后处理预设，手机上没有键盘，需要把「切预设」从键盘输入里解耦出来，换成触摸手势 + 自动轮播。
>
> **结论先行：原本判定为最大阻塞的 GLES3 blit 缺口，已经在工作区补完了。** 见 §5 的 review 结论。剩下的三块工作互相独立：输入抽象（sample 内）、GLES3 收尾与真机验证（引擎内）、Android Gradle 脚手架（工程配置）。
>
> **iOS 本轮不做**，理由见 §3。
>
> 相关文档：
>
> - Sample 本体：`doc/todo/PostProcessingApp-Design-todo.md`（场景、预设表、热键语义；本文只改输入层与平台，预设语义不动）
> - 后处理施工蓝图：`doc/todo/Camera-PostProcess-Design-todo.md`（§12 记 GL4 / GLES3 blit 契约；**§12.2 / §12.3 关于 GL4 和 GLES3 的记录都已过期**，见本文 §5.6）
> - GLES3 后端：`doc/todo/GLES3-Renderer-Backend-todo.md` A.8 / A.10.5
> - GPU 读回：`doc/todo/GPU-Readback-onRender-Design-todo.md`（**其中「GLES3 `supportsReadback` 为 false」已过期**，见本文 §5.5）
> - 手势参考实现：`source/Samples/InputApp/CubeControllerBehaviour.cpp`
> - **Android 工程参考：`source/Samples/BehaviourApp/Android/`**（整份照抄，理由与差异分析见 §6.0）
>
> **本文档的 GLES3 部分基于 2026-09-09 的工作区快照**（`source/Plugins/Renderer/OpenGLES3/` 下 13 个文件处于未提交的 `M` 状态）。这批改动还没在 Android 上编译验证过，行号可能随提交漂移。

---

## 0. 结论先行

| 工作 | 范围 | 状态 | 阻塞什么 | 可并行 |
|------|------|------|----------|--------|
| **一：输入抽象 + 触摸手势** | `source/Samples/PostProcessingApp/` | **已落地**（Windows Debug 已编过；手势待真机验） | — | — |
| **二：GLES3 blit / 回读** | `source/Plugins/Renderer/OpenGLES3/` | **主体已完成**；R3 / R4 已补；R1 / R2 / R5 待真机 | 真机核对 | 独立 |
| **三：Android Gradle 脚手架** | `source/Samples/PostProcessingApp/Android/` | **已落地**（照抄 BehaviourApp，6 处已改；APK 待真机构建） | 真机打 APK | — |

工作二的主体已经在工作区里做完了：四个 `blit` 重载全部收口到 `resolveBlitEndpoint` + `doBlit`，`size == ZERO` 语义、MSAA resolve、scratch FBO 都在，`supportsReadback` 也已经打开并有真实的 `map` / `unmap` 实现。所以 `assets/config/Android/Tiny3D.cfg` 的 `MSAA: 4` **不用改**，预设 0–7 理论上全部可用。

原计划里「移植 GL4 那份实现」这一步不用做了，工作二从**实现**变成**核对 + 真机验证**：§5.4 列了 5 条与 GL4 实现有分歧、需要在真机上确认的点。

---

## 1. 目标与非目标

### 1.1 目标

| # | 目标 | 验收方式 |
|---|------|----------|
| 1 | Android 真机能装能跑 `PostProcessingApp` | APK 安装启动，画面与桌面版预设 0 一致 |
| 2 | 触摸手势切预设，覆盖桌面版 `0-7` / `O` / `L` 全部语义 | §4.3 手势表逐条过 |
| 3 | 自动轮播兜底，用户不碰屏幕也能看到全部效果 | 启动后每 N 秒自动跳下一个预设 |
| 4 | 桌面版键盘行为**一字不变** | Windows 上回归 `0-7` / `O` / `L` |
| 5 | MSAA 相机下效果链正确 | 保持 cfg `MSAA: 4`，预设 2/3/4/5/7 画面正确 |
| 6 | Copy 预设（preset 1）在 Android 上与预设 0 观感一致 | 不是黑屏 |

### 1.2 非目标

- **不做 iOS。** 见 §3。缺口是 Metal 后端和 shader 变体，属于另一个立项。
- **不做移动端像素断言**（但已经可做了）。GLES3 `supportsReadback` 现在是 `true`，`map` / `unmap` 是真实现，技术上不再有障碍。本轮仍不建这套断言，是范围取舍不是能力缺失；`PostProcessingApp` 的验收继续用人眼 + logcat。想做的话入口见 §5.5。
- **不做屏上 UI / 文字提示。** 运行时没有 2D UI 能力（`TinyImGui` 是编辑器侧的 D3D11 插件，移动端用不了）。当前预设靠 logcat 的 `logPreset()` 输出确认。
- **不改预设语义。** 八个预设的定义、`applyPreset` / `swapEffectOrder` 的实现保持不动，只换驱动它们的输入源。
- **不把 command source 做成反射组件。** 它们是 sample 内部的裸接口实现，不进 `TCLASS()` / 序列化。
- **不动 GL4。** GL4 和 GLES3 的 blit 现在都已补齐，本轮只核对两者的分歧（§5.4）。
- **不重构 GLES3 已有的 blit 实现。** §5.4 那 5 条按「真机验证 → 有问题再改」处理，不为了对齐 GL4 而重写能跑的代码。
- **不改 `assets/config/Android/Tiny3D.cfg`。** MSAA 路径已经通了，不需要降 MSAA。

---

## 2. 现状盘点

### 2.1 已经就绪

| 项 | 位置 | 说明 |
|----|------|------|
| GLES / Vulkan shader 变体 | `PostProcessShaderSources.cpp:33-57`、`:140` / `:166` / `:192` | `T3D_OS_ANDROID` 分支齐全，ESSL 与 SPIR-V 两套都能选到 |
| CMake Android 分支 | `PostProcessingApp/CMakeLists.txt:87-127` | 建 `Android/app/libs/${ANDROID_ABI}`、拷 SDL2 与 `GLES3Renderer.so` |
| `Tiny3D.cfg` 自动部署 | 同上 `:114-120` | POST_BUILD 从 `assets/config/Android/Tiny3D.cfg` 拷进 assets，不用手放 |
| 参与 Android 构建 | `Samples/CMakeLists.txt:29` | 没有被 `TINY3D_OS_DESKTOP` 挡住 |
| 触摸输入 API | `T3DInput.h:51-70`、`:120-121` | `TouchPhase` / `Touch` / `getTouchCount` / `getTouch` |
| 手势参考实现 | `InputApp/CubeControllerBehaviour.cpp:427-502` | 单指 tap / 双击 / 拖拽 + 双指 pinch 的完整状态机 |
| 预设状态机 | `PostProcessControllerBehaviour.cpp:112-283` | `applyPreset` / `swapEffectOrder` / `logPreset` 纯逻辑，无输入依赖 |
| **GLES3 blit 全链路** | `T3DGLES3Context.cpp:3113-3229`、`:4312-4521` | 四个重载全部收口，详见 §5.1 |
| **GLES3 GPU 回读** | `T3DGLES3Context.cpp:117`、`:3310-3530` | `supportsReadback = true` + 真实 `map` / `unmap`，详见 §5.5 |
| **GLES3 shader 反射** | `T3DGLES3ContextBase.cpp:288` / `:423` | `reflectShaderAllBindings` / `reflectSamplerBindings` 走 glslang 反射缓存，不再是空 `T3D_OK` |
| **GLES3 MSAA 采样** | `T3DGLES3Context.cpp:3911-3936` | `bindPixelBuffers` 对 MSAA 2D 会改绑 `GLResolveTex`，详见 §5.3 |

### 2.2 缺的

| 项 | 现状 |
|----|------|
| Android Gradle 工程 | `PostProcessingApp/` 下**没有 `Android/` 目录**，整套 gradle / manifest / java 都不存在 |
| 触摸输入 | `PostProcessControllerBehaviour::onUpdate`（`:60-110`）只读 `T3D_INPUT.getKeyDown` |
| GLES3 真机验证 | §5.1 那批改动**没有在 Android 上编译运行过**，13 个文件还是未提交状态 |
| GLES3 与 GL4 的 5 处实现分歧 | 见 §5.4，都需要真机确认 |

---

## 3. 为什么 iOS 本轮不做

三条各自独立、都足以卡死：

1. **shader 变体没有 iOS 分支。** `PostProcessShaderSources.cpp` 只有 `T3D_OS_WINDOWS` 和 `T3D_OS_ANDROID`，iOS 下三个 `get*EffectShader()` 直接返回 false，效果链拿不到 shader。
2. **Metal 后端远未完成。** `T3DMetalContext.mm` 里有 75 处 TODO / stub / 空返回。
3. **工程文件也没有。** 这个 sample 只有 `OSX/Info.plist`，连 `iOS/Info.plist` 都没建（对比 14 个其它 sample 都有）。

iOS 的前置是 `doc/todo/Metal-Renderer-Backend-todo.md`。等 Metal 后端能跑通前向管线和 blit，再回来加 MSL shader 变体和 Xcode 工程。

---

## 4. 工作一：输入抽象 + 触摸手势 + 自动轮播

全部改动在 `source/Samples/PostProcessingApp/` 内，不碰引擎。可以先在 Windows 上验收桌面回归，再上真机。

### 4.1 命令层

新增 `PostProcessCommand.h`：

```cpp
enum class PresetCommandType
{
    kNone = 0,
    kSetPreset,     // value = 目标预设 0-7
    kNext,          // 下一个预设（7 之后回 0）
    kPrev,
    kSwapOrder,     // 对应桌面 O 键
    kToggleLog      // 对应桌面 L 键
};

struct PresetCommand
{
    PresetCommandType type {PresetCommandType::kNone};
    int32_t           value {0};
};

class IPresetCommandSource
{
public:
    virtual ~IPresetCommandSource() = default;
    /// 取出一条待处理命令，队列空返回 false
    virtual bool poll(PresetCommand &cmd) = 0;
    /// 启动时打进日志的操作说明
    virtual const char *usage() const = 0;
};
```

命令用枚举、不传原始 scancode——这是「解耦」的实际含义。`kNext` / `kPrev` 是给手势用的，桌面键盘只发 `kSetPreset`。

### 4.2 `PostProcessControllerBehaviour` 的改动

**只改 `onUpdate`**（`PostProcessControllerBehaviour.cpp:60-110`）。`applyPreset` / `swapEffectOrder` / `logPreset` / `presetName` 一行不动：

```cpp
void PostProcessControllerBehaviour::onUpdate()
{
    PresetCommand cmd;
    for (IPresetCommandSource *src : mSources)
    {
        while (src != nullptr && src->poll(cmd))
        {
            dispatch(cmd);
        }
    }
}
```

`dispatch` 把 `kNext` / `kPrev` 换算成 `applyPreset((mPreset + 1) % 8)` 之类，其余直连现有方法。

头文件加 `void addCommandSource(IPresetCommandSource *src);` 和一个 `TArray<IPresetCommandSource*> mSources;`。

**注意**：这个类带 `TCLASS()` / `TRTTI_ENABLE(Behaviour)`（`PostProcessControllerBehaviour.h:43-47`），改成员会触发 `tiny3d_enable_reflection`（`CMakeLists.txt:255-261`）重新生成。用裸接口指针、不要用智能指针容器或反射类型，避免被拖进序列化。命令源的生命周期由 `PostProcessingApp` 持有。

### 4.3 三个命令源

| 类 | 文件 | 说明 |
|----|------|------|
| `KeyboardCommandSource` | `KeyboardCommandSource.{h,cpp}` | 把现有 `onUpdate` 里那串 `T3D_INPUT.getKeyDown(APP_SCANCODE_0..7 / O / L)` 原样搬进来 |
| `TouchCommandSource` | `TouchCommandSource.{h,cpp}` | 手势状态机，见下 |
| `AutoCycleCommandSource` | `AutoCycleCommandSource.{h,cpp}` | 每 N 秒发一条 `kNext`；被通知「用户已交互」后自我禁用 |

**手势映射：**

| 手势 | 命令 | 对应桌面键 |
|------|------|-----------|
| 单指横向滑动（右 / 左） | `kNext` / `kPrev` | 遍历 `0-7` |
| 双击 | `kSetPreset(0)` | `0` |
| 双指点击 | `kSwapOrder` | `O` |
| 长按（> 800 ms 不移动） | `kToggleLog` | `L` |

`TouchCommandSource` 的状态机照抄 `InputApp/CubeControllerBehaviour.cpp:427-502`，那份代码的骨架直接可用：

- `getTouchCount() >= 2` 先分出双指分支，并清掉单指的 pending 状态（`:431-448`）
- 单指走 `TouchPhase::Began / Moved / Ended / Canceled` 四段（`:459-501`）
- `mTapMoveThreshold` 区分 tap 和 drag（`:475`）
- `Time::unscaledTime()` 配 `mDoubleTapWindowMs` 判双击（`:461-469`）

**一个必须改的地方**：`CubeControllerBehaviour` 的阈值是绝对像素（`mTapMoveThreshold`），手机 DPI 差异大，直接抄会在高分屏上误判。改成按屏宽百分比算，屏宽从 `T3D_AGENT.getDefaultRenderWindow()->getDescriptor().Width` 取——`PostProcessingApp.cpp:245` 算宽高比时已经这么用了。建议：滑动阈值取屏宽的 8%，tap 判定阈值取 2%。

**自动轮播与手势的关系**：`AutoCycleCommandSource` 默认开启，`TouchCommandSource` 一旦产生任何命令就调它的 `disable()`。这样手机拿到手不用猜操作方式，碰一下又能自己控制。

### 4.4 装配点

`PostProcessingApp.cpp::buildCamera` 末尾（`:285-287`）：现在是 `go->addComponent<PostProcessControllerBehaviour>()` 加一句硬编码日志

```
T3D_LOG_INFO(LOG_TAG_APP, "[PostProcess] keys: 0-7 preset, O swap order, L toggle camera log");
```

改成按平台装配 source，并把日志换成遍历 source 的 `usage()` 输出，移动端自然就打手势说明。桌面装 `Keyboard`，移动端装 `Touch` + `AutoCycle`（也可以三个都装，桌面上自动轮播默认关）。

---

## 5. 工作二：GLES3 后端 —— review 结论与收尾项

> **本节在 2026-09-09 重写。** 上一版写的是「移植 GL4 的收口实现到 GLES3」，重新 review 代码后发现这件事**已经做完了**，而且做得比原计划多（回读、反射、MSAA 采样都补了）。本节改为记录已完成的部分、以及 review 中发现的、需要在真机上确认的分歧点。

### 5.1 已完成：blit 四重载收口

`T3DGLES3Context` 现在和 GL4 / D3D11 是同一套结构：

| 位置 | 内容 |
|------|------|
| `T3DGLES3Context.h:206-223` | `struct BlitEndpoint`（`fbo` / `texture` / `resolveFbo` / `resolveTex` / `width` / `height` / `sampleCount` / `isWindow` / `isDepth` / `needsScratchFbo`）+ 两个 `resolveBlitEndpoint` + `doBlit` 声明 |
| `T3DGLES3Context.h:268-269` | `mScratchReadFBO` / `mScratchDrawFBO` |
| `T3DGLES3Context.cpp:3113-3229` | 四个 `blit` 重载，每个都是「校验 → 解析两端 → `ENQUEUE_UNIQUE_COMMAND(doBlit)`」十几行 |
| `T3DGLES3Context.cpp:4312-4382` | 两个 `resolveBlitEndpoint`：`Texture*` 走 `GLES3PixelBuffer2D` 取四件套；`RenderTarget*` 分 window / 颜色附件 / 深度附件三支 |
| `T3DGLES3Context.cpp:4406-4521` | `doBlit`：scratch FBO、`size == ZERO` 语义、MSAA resolve、Y 翻转、depth mask、`glInvalidateFramebuffer` |

上一版文档点名的三个缺口逐条核对：

| 上一版记的缺口 | 现状 |
|----------------|------|
| `blit(Texture*, Texture*)` 是 `return T3D_OK` 空壳 | 已实现（`:3203-3229`）。这是原判定里的「第一杀手」——`ForwardRenderPipeline::resolveIfMultisampled`（`T3DForwardRenderPipeline.cpp:1352-1378`）在 MSAA 相机下靠它写临时 RT，空实现会让整条效果链采样一张没写过的贴图 |
| `size == ZERO` 被当成 0×0 | 已按契约实现（`:4443-4455`），含 `copyW == 0` 的显式报错。这条决定 preset 1（Copy）能不能用——`CameraEffectBehaviour::blitCopy`（`T3DCameraEffectBehaviour.cpp:62`）三个位置参数全走默认值 |
| `blit(RT*, RT*)` / `blit(RT*, Tex*)` 静默成功 | 已实现（`:3113` / `:3173`），走同一个 `doBlit` |

GLES3 与 GL4 的 API 差异也处理对了：`glDrawBuffer` 单数形式在 GLES3 里不存在，`doBlit:4505` 用的是 `glDrawBuffers(1, &drawBuf)`；`glBlitFramebuffer` 的 filter 是 `GL_NEAREST`；`GL_TEXTURE_2D_MULTISAMPLE` 的解绑在 `unbindTextureUnits:4395-4398` 里按 `mGLESMinor >= 1` 做了版本保护。

### 5.2 已完成：MSAA 渲染纹理的资源建法

`buildRenderTextureResources`（`T3DGLES3Context.cpp:366-500`）在 `msaaCount > 1` 时建的是完整的一对：

- `GLTexture`（`GL_TEXTURE_2D_MULTISAMPLE`）+ `GLFBO` + `GLDepthRBO`（`glRenderbufferStorageMultisample`）
- `GLResolveTex`（普通 `GL_TEXTURE_2D`，带 `GL_LINEAR` / `GL_CLAMP_TO_EDGE`）+ `GLResolveFBO`

两个 FBO 都有 `glCheckFramebufferStatus` 检查，失败返回 `T3D_ERR_GLES3_CREATE_FBO` 并打日志。资源释放在 `releaseRenderTextureResources:351-356` 和 `GLES3PixelBuffer2D` 析构（`T3DGLES3RenderBuffer.cpp:139-141`）里成对做了。

### 5.3 已完成：`bindPixelBuffers` 的 MSAA 采样

上一版 §5.4 说这条「本轮不需要动」，实际上也已经补了。`T3DGLES3Context.cpp:3911-3936` 分三支：

1. `GLMSAACount > 1 && GLResolveTex != 0` → 绑 `GLResolveTex` + `GL_TEXTURE_2D`（正常路径）
2. `GLMSAACount > 1` 但没有 resolve 贴图 → 需要 ES 3.1 的 `GL_TEXTURE_2D_MULTISAMPLE`，`mGLESMinor < 1` 时打错误日志并绑 0
3. 非 MSAA → 绑 `GLTexture` + `GL_TEXTURE_2D`

第 2 支的降级是「绑 0 + 明确报错」，不是静默黑屏，符合 §10 对失败路径的要求。

### 5.4 待核：GLES3 与 GL4 的 5 处实现分歧

这 5 条都不是「明显的 bug」，是 review 出来的、两边实现不一致且 GLES3 这边有理由怀疑的地方。**处理原则是先在真机上验，有问题再改**，不为了对齐 GL4 而重写能跑的代码。

**R1｜`doBlit` 里的 `glInvalidateFramebuffer`（`:4510-4514`），GL4 没有**

```cpp
if (readFBO != 0)
{
    GLenum discards[] = { src.isDepth ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0 };
    glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, 1, discards);
}
```

这是 TBR（移动端瓦片渲染）的带宽优化，前提是「读完源之后同帧不再用它」。上一版文档明确写了「移植时不要抄这句」，现在它在。

效果链里的实际情况：`runCameraPostprocessing` 每个 effect 之后 `cur` 会换到新的临时 RT，源确实不再读；MSAA 分支里被作废的是纹理自己的 `GLResolveFBO`，而链上采样的是 `acquireTempRT` 出来的另一张。所以**按当前调用模式是安全的**，但它对调用者施加了一条没写在 `RHIContext::blit` 契约里的隐含约束。

- 验证：§9.3 单列一条，多 effect 串联（预设 4 / 5）下看有没有闪烁或残影。
- 若要改：给 `doBlit` 加个 `bool discardSource` 参数，默认 false，或者把这条约束写进 `RHIContext::blit` 的注释。

**R2｜MSAA 源走两跳，GL4 的注释明确反对（`:4457-4472`）**

GLES3 的做法是：先 `src.fbo → src.resolveFbo` 做一次全屏 resolve，再把 `src.resolveFbo` 当 read FBO blit 到 dst。GL4 的 `doBlit`（`T3DGL4Context.cpp:4749-4754`）在注释里专门说了不要这么干——中间 resolve 贴图常被 `bindPixelBuffers` 绑在 texture unit 上，再当 FBO 附件写就是 feedback loop。

GLES3 这边挡住了：`doBlit` 开头（`:4413`）无条件调 `unbindTextureUnits()`，把所有 unit 的 2D / 3D / Cube / 2D_MS 和 sampler 全解绑。所以 feedback 不会发生。代价是两条：

1. **多一次全屏拷贝。** dst 是非 MSAA 的（`acquireTempRT` 传 msaaCount=1，`T3DForwardRenderPipeline.cpp:1401`），`glBlitFramebuffer` 从多重采样 read FBO 直接到单采样 draw FBO 本身就隐式 resolve，矩形一致 + 格式匹配 + `GL_NEAREST` 都满足。一跳就够。移动端上一次 1080p+ 的多余全屏拷贝不是小钱。
2. **`unbindTextureUnits` 每次 blit 都全量跑一遍**，循环次数是 `mMaxTextureImageUnits`，每次 4 个 `glBindTexture` + 1 个 `glBindSampler`。而且解绑之后引擎侧的纹理绑定状态被打乱，下次 draw 必须重新绑——需要确认 `bindPixelBuffers` 是每次 draw 都调、而不是有缓存跳过。

- 验证：真机抓帧看 blit 的耗时占比；确认解绑后下一个 draw 的纹理绑定正常（画面不发黑即可）。
- 若要改：MSAA 分支直接 `src.fbo → dst`，然后 `unbindTextureUnits()` 就只在 dst 可能被绑过时才需要。

**R3｜`doBlit` 的 scratch FBO 没有 completeness 检查（`:4421-4441`）**

`map(texture)` 那边 attach 完会 `glCheckFramebufferStatus`（`:3444-3450`），`doBlit` 这边两个 scratch FBO attach 完直接就用了。附件格式不可渲染时会静默失败，只能靠末尾的 `GL_CHECK_ERROR` 兜。补两行检查的成本很低，建议顺手加上。

**R4｜`mScratchReadFBO` 被 `doBlit` 和 `map(texture)` 共用**

两处（`:3437` / `:4423`）用的是同一个 FBO 对象，各自在使用前重新 `glFramebufferTexture2D` attach，所以当前是安全的。但这是个隐式约定，一旦有人为了省 attach 调用加个「附件没变就跳过」的缓存就会串。至少在成员声明处（`T3DGLES3Context.h:268`）写一行注释。

**R5｜Y 翻转的条件和翻法（`:4484-4489`）**

```cpp
if (dst.isWindow && mProjectionFlipped)
{
    GLint tmp = srcY0; srcY0 = srcY1; srcY1 = tmp;
}
```

只在「目标是窗口」时翻，翻的是源矩形。GL4 那边靠 `glClipControl` 建立 FBO 投影翻转，GLES3 没有 `glClipControl`，`mProjectionFlipped` 的语义（`:265-270`）是自己的一套。这条纯靠真机看方向。

- 验证：真机上预设 0 和预设 1（Copy）的画面上下方向；两者不一致就说明 RT→RT 和 RT→window 两条路径的翻转约定对不上。
- **排查时不要去改 shader。** 全屏 VS 的 UV 约定见 `Camera-PostProcess-Design-todo.md` §12.4，改那边会把桌面 GL4 一起弄坏。

**附：ES 3.0 设备**

`glTexStorage2DMultisample`（`:392` / `:481`）没有按 `has31` 保护。但 `T3DGLES3PrerequisitesBase.h:15-17` 无条件 include 了 `gl31.h` / `gl32.h`，编译链接都过得去，ES 3.0 上下文里调用只是 `GL_INVALID_OPERATION`，随后被 `glCheckFramebufferStatus` 捕获成 `T3D_ERR_GLES3_CREATE_FBO`——有明确失败路径，不是静默黑屏。

而且这个 sample 的 GLES shader（含前向 pass 和后处理，`SampleShaders_gles3.h` 全文 16 处 `#version`）**全部是 `#version 310 es`**，ES 3.1 本来就是整个 sample 的地板，不是后处理独有的限制。所以这条不单独处理，只要求测试机确认 ES 3.1+。

### 5.5 已完成：GPU 回读（可选地开启像素断言）

`GPU-Readback-onRender-Design-todo.md` 里「GLES3 `supportsReadback` 为 false、`map` / `unmap` 是 stub」的记录**已经过期**。现状：

| 项 | 位置 | 说明 |
|----|------|------|
| `supportsReadback = true` | `T3DGLES3Context.cpp:117` | 无版本门槛，ES 3.0 起 PBO 就有 |
| `map(RenderBuffer*, offset, size)` | `:3310-3371` | buffer 回读 |
| `map(RenderBuffer*, ReadbackRegion)` | `:3373-3519` | 2D 纹理回读，走 PBO + `glReadPixels` |
| `unmap` | `:3523-3530` | 转 `finishReadback` |

纹理回读这条路径考虑得比较周全：MSAA 源会先 resolve 再读（`:3423-3435`）；scratch FBO 有 completeness 检查（`:3444`）；`ReadbackRegion.size` 为 0 按整图算（`:3478-3485`），和 blit 的 ZERO 语义一致；还会拿 `GL_IMPLEMENTATION_COLOR_READ_FORMAT` / `_TYPE` 和请求格式协商（`:3453-3470`），这是 ES 上很容易漏的一步。

两条限制要记住：**只支持 2D 彩色纹理**（`:3393-3399`），**深度 / 模板明确不支持**（`:3403-3409`，ES 上 `glReadPixels` 读不了 depth）。

所以移动端像素断言现在技术上可做了。本轮仍不建这套东西（§1.2），但如果后面要做，入口就是对效果链最终 RT 调 `map` + `unmap`，判据用 `PostProcessingApp-Design-todo.md` §3 的预设表——比如预设 2（Grayscale）断言 `R == G == B`，预设 3（Invert）断言与预设 0 的同点像素之和接近 255。

### 5.6 文档回填：GL4 和 GLES3 的记录都已过期

`Camera-PostProcess-Design-todo.md` §12.2 的对照表把 GL4 和 GLES3 并列成同一个缺口（「`blit(Texture*, Texture*)` ❌ 空实现」「`size == ZERO` 不认」），§12.3 / §12.5 同理。**两边都不成立了**：GL4 早已收口，GLES3 这轮也收口了。

本轮要回填三处：

| 文档 | 改什么 |
|------|--------|
| `Camera-PostProcess-Design-todo.md` | §12.2 表格 GL4 / GLES3 两组行、§12.3 措辞、§12.5 补齐顺序，改成「D3D11 / GL4 / GLES3 三家已对齐，Metal 待做」 |
| `GLES3-Renderer-Backend-todo.md` | A.8 / A.10.5 标记 blit 收口完成 |
| `GPU-Readback-onRender-Design-todo.md` | 「GLES3 `supportsReadback` 为 false」改成已支持，并记上「仅 2D 彩色、不支持深度」 |

### 5.7 收尾清单

| 项 | 来源 | 优先级 |
|----|------|--------|
| 这 13 个文件在 Android 上编译通过 | §2.2 | **必做**，就是 §8 的 M0 |
| R1 `glInvalidateFramebuffer` 真机确认 | §5.4 | 必做（验证，不一定改） |
| R2 MSAA 两跳的开销确认 | §5.4 | 必做（验证，不一定改） |
| R3 scratch FBO 加 completeness 检查 | §5.4 | 建议做，两行 |
| R4 `mScratchReadFBO` 共用加注释 | §5.4 | 建议做，一行 |
| R5 Y 翻转方向真机确认 | §5.4 | **必做** |
| 三份文档回填 | §5.6 | M6 |

---

## 6. 工作三：Android Gradle 脚手架（照抄 `BehaviourApp`）

> **原则：整份复制 `source/Samples/BehaviourApp/Android/`，只改必须改的 6 处。不要自己拼工程，不要「顺手优化」。** 全仓 17 个 sample 的 Android 工程是同一套模板，任何自创的偏差以后都会变成只有这一个 sample 有的怪毛病。

### 6.0 为什么是 `BehaviourApp`

全仓有 Android 工程的 sample 共 17 个，`PostProcessingApp` 是少数几个没有的之一。`BehaviourApp` 是其中和它最贴的一个：

| 对照项 | 结论 |
|--------|------|
| **`CMakeLists.txt`** | `BehaviourApp/CMakeLists.txt` 与 `PostProcessingApp/CMakeLists.txt` **逐行相同**，只差 `set_project_name()` 和 `GENERATED_DIR` 里的项目名。Android 段（`:87-127`）连行号都一模一样 |
| **构建位置** | `Samples/CMakeLists.txt:28` / `:29` 紧挨着，都在 `TINY3D_OS_DESKTOP` 守卫之外 |
| **反射依赖** | 两者都有 `TCLASS()` 组件（`RotateBehaviour` / `PostProcessControllerBehaviour`），`tiny3d_enable_reflection` 的调用参数除 `GENERATED_DIR` 外完全一致 → 对宿主 `rpp.exe` 的依赖形态相同 |
| **额外插件** | 两者的 Android 段都拷 `GLES3Renderer`（`:122-127`） |

顺带一提，`LightApp/Android/app/build.gradle` 和 `BehaviourApp` 版的差异**只有 `namespace` 与 `applicationId` 两行**，所以把参照从 `LightApp` 换成 `BehaviourApp` 不影响本节以外的任何结论——但 `BehaviourApp` 在 CMake 和反射上的同构性更强，照抄时更不容易出意外。

### 6.1 要复制的东西

`BehaviourApp/Android/` 下纳入版本管理的共 42 个文件。整目录复制过来，然后**删掉未跟踪的产物**（`app/build/`、`app/.cxx/`、`app/libs/`、`.gradle/`、`.idea/workspace.xml`、`local.properties`）——`.gitignore` 已经把这些挡住了，但复制文件夹时会一起带过来。

| 组 | 内容 | 说明 |
|----|------|------|
| 根目录 | `build.gradle`、`settings.gradle`、`gradle.properties`、`gradlew`、`gradlew.bat`、`.gitignore` | 全部原样 |
| Gradle wrapper | `gradle/wrapper/gradle-wrapper.jar` + `.properties` | Gradle 8.9；**别漏 jar**，漏了 `gradlew` 跑不起来 |
| app 配置 | `app/build.gradle`、`app/CMakeLists.txt`、`app/proguard-rules.pro`、`app/.gitignore` | `app/CMakeLists.txt` 见 §6.3 第 1 条 |
| Java | `app/src/main/java/com/tiny3d/behaviourapp/BehaviourAppActivity.java` | 要改，见 §6.2 |
| 清单 / 资源 | `AndroidManifest.xml`、`res/values/{strings,colors,styles}.xml`、`res/drawable*/`、`res/mipmap*/`（图标 png 共 10 个） | 只有 `strings.xml` 要改 |
| assets | `app/src/main/assets/Tiny3D.cfg` | 见 §6.4 |
| IDE | `.idea/`（10 个文件） | 见 §6.3 第 3 条 |

### 6.2 只改这 6 处

| # | 文件 | 改什么 |
|---|------|--------|
| 1 | `app/build.gradle:10` | `namespace 'com.tiny3d.behaviourapp'` → `'com.tiny3d.postprocessingapp'` |
| 2 | `app/build.gradle:13` | `applicationId "com.tiny3d.behaviourapp"` → `"com.tiny3d.postprocessingapp"` |
| 3 | java 文件路径 | 目录改成 `java/com/tiny3d/postprocessingapp/`，文件名改成 `PostProcessingAppActivity.java` |
| 4 | java 文件内容 | `package` 行、类名、`System.loadLibrary("BehaviourApp")` → `"PostProcessingApp"`、`getMainSharedObject()` 返回 `"libPostProcessingApp.so"` |
| 5 | `AndroidManifest.xml:12` | `android:name=".BehaviourAppActivity"` → `".PostProcessingAppActivity"` |
| 6 | `res/values/strings.xml` | `app_name` → `PostProcessingApp` |

改完的 Activity 就是这 28 行，除三处名字外与 `BehaviourAppActivity.java` 一字不差：

```java
package com.tiny3d.postprocessingapp;

import com.tiny3d.lib.Tiny3DActivity;

public class PostProcessingAppActivity extends Tiny3DActivity {
    static {
        System.loadLibrary("T3DPlatform");
        System.loadLibrary("T3DCore");
        System.loadLibrary("PostProcessingApp");
    }

    @Override
    protected String[] getLibraries() {
        return new String[] { "SDL2" };
    }

    @Override
    protected String getMainSharedObject() {
        return "libPostProcessingApp.so";
    }

    @Override
    protected String getMainFunction() {
        return "main";
    }
}
```

注意 `getLibraries()` 里**只有 `SDL2`**，没有 `GLES3Renderer`——渲染插件是引擎按 `Tiny3D.cfg` 运行时加载的，不走 Java 侧 `loadLibrary`。

**不要改**的地方（这几处看着像要改，其实层级完全一样）：

| 位置 | 值 | 为什么不用改 |
|------|-----|-------------|
| `settings.gradle:3` | `new File('../../../Platform/Android/')` | 从 `Samples/<X>/Android/` 上溯三级到 `source/`，两个 sample 同深度 |
| `app/build.gradle:6` | `file("../../../../")` | 从 `Samples/<X>/Android/app/` 上溯四级到 `source/` |
| `app/build.gradle:52` | `path "../../../../CMakeLists.txt"` | 同上，指向 `source/CMakeLists.txt` |
| `app/build.gradle:22` | `-DTINY3D_BUILD_SAMPLES=TRUE` | 整个 Samples 目录一起构建，不是单 sample |

### 6.3 三点更正（上一版参考 `LightApp` 时写错了）

1. **`app/CMakeLists.txt` 不要删。** 上一版说它是 AS 模板残留、应该删掉。它确实是残留（`add_library(native-lib ... src/main/cpp/native-lib.cpp)` 引用的文件不存在，实际生效的是 `app/build.gradle:50-54` 指向引擎根 CMake 的 `externalNativeBuild`），但**全仓 17 个 sample 一个不落全都留着它**。删掉就成了唯一的偏差，不值得。照抄。
2. **`local.properties` 本来就没被提交。** 上一版说「`LightApp/Android/local.properties` 现在在仓库里」——查错了，`LightApp` 没有提交它，`BehaviourApp` 也没有；`BehaviourApp/Android/.gitignore:3` 的 `/local.properties` 已经挡住了。全仓只有 `InputApp` 误提交了一份（`source/Samples/InputApp/Android/local.properties`），那是个孤例。照抄 `.gitignore` 即可，无需额外处理。
3. **`.idea/` 要一起复制。** 17 个 sample 里有 16 个提交了 `.idea/`（只有 `PlatformApp` 例外），是既有约定。里面的路径都是 `$PROJECT_DIR$` 相对形式（见 `.idea/gradle.xml:12-15`），可移植。`.idea/workspace.xml` 被 `.gitignore` 挡着，不会跟进来。

### 6.4 `assets/Tiny3D.cfg`：跟着提交一份

这里有个看起来矛盾的地方：`PostProcessingApp/CMakeLists.txt:114-120` 的 POST_BUILD 会自动把 `assets/config/Android/Tiny3D.cfg` 拷进 `Android/app/src/main/assets/`，按说不必提交；但 `BehaviourApp/Android/app/src/main/assets/Tiny3D.cfg` **是提交了的**，而且与源文件字节相同（17 个里有 12 个这么做）。

**建议跟随，提交一份。** 好处是没跑过 native 构建也能直接开 Android Studio 同步工程。代价是这份副本会随源文件漂移——改了 `assets/config/Android/Tiny3D.cfg` 之后记得跑一次构建让 POST_BUILD 覆盖，再一起提交。

### 6.5 CMake 侧零改动

`PostProcessingApp/CMakeLists.txt` 一行都不用动，Android 段已经齐了：

| 行 | 做什么 |
|----|--------|
| `:89-101` | `add_library(${LIB_NAME} SHARED ...)` + 链接引擎各模块 |
| `:104-110` | PRE_LINK 建 `Android/app/libs/${ANDROID_ABI}/` 并拷 SDL2 |
| `:112-120` | POST_BUILD 建 assets 目录并拷 `Tiny3D.cfg` |
| `:122-127` | POST_BUILD 拷 `GLES3Renderer` 的 `.so` |

（`${BIN_NAME}` 和 `${LIB_NAME}` 是同一个值，见 `source/CMake/Utils/ProjectCMakeModule.cmake:12-16`，`add_custom_command(TARGET ${BIN_NAME})` 挂在 `add_library(${LIB_NAME})` 建出的目标上不是笔误。）

所以 §6.1 复制完 + §6.2 改完 6 处，`Android/app/libs/` 和 assets 都会在第一次 native 构建时自动填好，不用手放任何 `.so`。

### 6.6 首次构建会很慢

`app/build.gradle:66-106` 有一段宿主 `rpp.exe`（反射预处理器）的自动构建逻辑：路径写死 `source/bin/Windows/{Debug,Release}/rpp.exe`，不存在时用 `Visual Studio 17 2022` 生成器现建一遍，再作为 `externalNativeBuild` 的前置依赖。交叉编译时反射代码生成必须用宿主机的 rpp，这是既有机制，照抄，别动。

`PostProcessControllerBehaviour` 带 `TCLASS()`，一定会走到这条路径——这也是选 `BehaviourApp` 当参照的原因之一（`RotateBehaviour` 同样带反射，这套流程在它身上是验证过的）。

工具链版本跟着模板走：`compileSdk 33` / `minSdk 26` / `ndk 24.0.8215888` / AGP 8.5.0 / Gradle 8.9 / `abiFilters 'arm64-v8a', 'x86_64'`。

---

## 7. 文件改动清单

### 7.1 新增

| 文件 | 归属 |
|------|------|
| `source/Samples/PostProcessingApp/PostProcessCommand.h` | 工作一 |
| `source/Samples/PostProcessingApp/KeyboardCommandSource.{h,cpp}` | 工作一 |
| `source/Samples/PostProcessingApp/TouchCommandSource.{h,cpp}` | 工作一 |
| `source/Samples/PostProcessingApp/AutoCycleCommandSource.{h,cpp}` | 工作一 |
| `source/Samples/PostProcessingApp/Android/**`（42 个文件） | 工作三，整份照抄 `BehaviourApp/Android/`，只改 §6.2 那 6 处 |

### 7.2 修改

| 文件 | 改什么 |
|------|--------|
| `PostProcessControllerBehaviour.{h,cpp}` | `onUpdate` 改成遍历 source；加 `addCommandSource` / `dispatch`；状态机方法不动 |
| `PostProcessingApp.{h,cpp}` | `buildCamera` 装配 source（`:285-287`）；持有 source 生命周期；`applicationWillTerminate` 里清理 |
| `T3DGLES3Context.h` | 只加一行注释：`mScratchReadFBO`（`:268`）被 `doBlit` 和 `map(texture)` 共用（R4） |
| `T3DGLES3Context.cpp` | 只补 `doBlit` 两个 scratch FBO 的 completeness 检查（`:4421-4441`，R3）。R1 / R2 / R5 视真机结果再定 |
| `doc/todo/Camera-PostProcess-Design-todo.md` | §12.2 / §12.3 / §12.5 回填 GL4 与 GLES3 都已完成（§5.6） |
| `doc/todo/GLES3-Renderer-Backend-todo.md` | A.8 / A.10.5 标记 blit 收口完成 |
| `doc/todo/GPU-Readback-onRender-Design-todo.md` | GLES3 `supportsReadback` 已为 true，补记「仅 2D 彩色、不支持深度」（§5.5） |
| `doc/todo/PostProcessingApp-Design-todo.md` | 补一节指向本文，说明输入层已抽象、移动端形态 |

### 7.3 明确不改

- `assets/config/Android/Tiny3D.cfg`（MSAA 路径已通，不需要降 MSAA）
- `source/Core/**`（预设语义、效果链、`CameraEffectBehaviour` 都不动）
- `source/Plugins/Renderer/OpenGL4/**`（已完成）
- **GLES3 已经写好的 blit / 回读实现**（除 §7.2 那两处小修外不动，理由见 §5.4 的处理原则）
- iOS 相关的一切

---

## 8. 分步实现顺序

| 步 | 内容 | 依赖 | 可验收点 |
|----|------|------|----------|
| **M0** | GLES3 那 13 个未提交文件在 Android ABI 下编译通过 | — | `.so` 出得来 |
| **M1** | 命令层 + `KeyboardCommandSource`，`onUpdate` 改成遍历 source | — | **Windows 上 `0-7` / `O` / `L` 行为与改造前完全一致** |
| **M2** | `TouchCommandSource` + `AutoCycleCommandSource` | M1 | Windows 上自动轮播能跑；触摸暂时无法验 |
| **M3** | Android Gradle 脚手架（照抄 `BehaviourApp`，§6） | M0 | APK 能装能启动，画面 = 预设 0 |
| **M4** | GLES3 收尾：R3 / R4 两处小修 + R1 / R2 / R5 真机核对 | M3 | §9.3 全过 |
| **M5** | 真机联调 | M2 + M4 | §9 全过 |
| **M6** | 文档回填 | M4 | §7.2 四份 todo |

M1 是唯一有桌面回归风险的一步，先单独验收 Windows。M1 / M2 与 M0 / M3 互不依赖，可以并行。

**M0 提到了最前面**：GLES3 那批改动只在桌面侧看过代码，没在 Android NDK 下编译过。真机联调时最不想遇到的就是编译错误和渲染问题混在一起分不清。M0 的最省事做法就是**直接构建 `BehaviourApp` 的 Android 工程**——它已经在仓库里、CMake 与 `PostProcessingApp` 逐行相同，能出 APK 就说明 NDK 工具链、宿主 rpp、GLES3 那批改动三样都没问题，同时还顺带验证了 §6 要照抄的那份模板本身是好的。

M4 的性质从上一版的「实现」变成了「验证」——`doBlit` 已经在了，M4 是拿真机确认 §5.4 那 5 条分歧点。**但仍然不能跳过**：R5（Y 翻转）出问题会直接表现为画面上下颠倒，R1（`glInvalidateFramebuffer`）出问题会表现为多 effect 串联时闪烁，两者都很容易被误判成手势或 shader 的问题。

---

## 9. 测试要点

### 9.1 桌面回归（M1 之后立刻做）

| 用例 | 期望 |
|------|------|
| `0-7` 逐个按 | 与 `PostProcessingApp-Design-todo.md` §3 的预设表一致 |
| `O` | Gray / Invert 的 order 对调，preset 在 4 / 5 之间翻 |
| `L` | 逐帧日志开关 |
| 日志格式 | `[PostProcess] preset=N ... enabled={...} order={...}` 一字不变 |

### 9.2 手势（真机）

| 用例 | 期望 | 易错点 |
|------|------|--------|
| 右滑 / 左滑 | 下一个 / 上一个预设，7 之后回 0 | 滑动距离阈值在高 DPI 屏上会不会太灵敏 |
| 双击 | 回到预设 0 | 与「两次快速滑动」的区分 |
| 双指点击 | order 对调 | 双指落地时序有先后，别被识别成两次单指 tap |
| 长按 | 逐帧日志开关 | 长按期间手指微动，别掉进 drag 分支 |
| 不碰屏幕 | 每 N 秒自动跳下一个预设 | — |
| 碰一下之后 | 自动轮播停止 | — |

### 9.3 渲染（真机，M4）

这一节同时是 §5.4 那 5 条分歧点的验证清单，末列标出对应项。

| 用例 | 期望 | 验哪条 |
|------|------|--------|
| 保持 cfg `MSAA: 4`，预设 0 | 与桌面预设 0 观感一致，不花屏 | §5.1 MSAA resolve、§5.2 资源建法 |
| 预设 1（Copy） | **整屏正常画面**，不是黑屏 | §5.1 的 `size == ZERO` 语义 |
| **预设 0 与预设 1 的上下方向一致** | 都不颠倒 | **R5** —— 不一致说明 RT→RT 与 RT→window 的翻转约定对不上 |
| 预设 2 / 3 / 7 | 灰度 / 反相 / 染色，一眼可辨 | §5.1 |
| 预设 4 / 5 | 都是灰图，但明暗**不同**（红块反相后更亮） | §5.1 |
| **预设 4 / 5 连续看 10 秒以上** | 画面稳定，无闪烁 / 残影 / 隔帧发黑 | **R1** —— 两个 effect 串联，源被 `glInvalidateFramebuffer` 作废的场景 |
| 预设 6 | 与预设 0 相同（Gray 挂着但 disabled） | — |
| 预设间快速来回切 | 不黑一帧、不残留上个效果 | **R2** —— `unbindTextureUnits` 之后纹理有没有正确重绑 |
| 抓一帧看 blit 的 GPU 耗时占比 | MSAA resolve 不应出现两次全屏拷贝 | **R2** |
| 临时改 cfg `MSAA: 1` 再跑一遍 | 全部预设仍正确 | 非 MSAA 路径（`needsScratchFbo` 分支）没被 MSAA 那条盖住 |

### 9.4 明确不测

- 像素级断言（能做但本轮不做，见 §1.2 / §5.5）
- 深度纹理回读（GLES3 明确不支持，`T3DGLES3Context.cpp:3403-3409`）
- iOS
- ES 3.0 设备（整个 sample 的 GLES shader 都是 `#version 310 es`，见 §5.4 附）
- 桌面 GL4（本轮不动）

---

## 10. 风险

| 风险 | 缓解 |
|------|------|
| **GLES3 那 13 个未提交文件在 NDK 下编不过** | M0 单独做掉，别和渲染问题混在一起排查。桌面上看不出来 |
| Y 翻转方向不对（R5） | 上屏 blit 上下颠倒时查 `doBlit:4484-4489` 的条件，**不要去改 shader**——全屏 VS 的 UV 约定见 `Camera-PostProcess-Design-todo.md` §12.4，改那边会把桌面 GL4 一起弄坏 |
| `glInvalidateFramebuffer` 作废了同帧还要用的源（R1） | §9.3 有专门用例；真出问题就给 `doBlit` 加 `discardSource` 开关，默认 false |
| MSAA 两跳的额外全屏拷贝（R2） | 功能正确性有 `unbindTextureUnits` 兜底，只是性能问题；抓帧确认后再决定是否改一跳 |
| **自己拼 Android 工程 / 顺手「优化」模板** | 整份照抄 `BehaviourApp/Android/`，只改 §6.2 那 6 处。全仓 17 个 sample 是同一套模板，自创偏差以后只会体现为「只有这个 sample 有的怪毛病」。特别是 §6.3 那三条——`app/CMakeLists.txt` 别删、`.idea/` 要带上、`local.properties` 本来就没提交 |
| 复制工程时把构建产物一起带过来 | `app/build/`、`app/.cxx/`、`app/libs/`、`.gradle/`、`local.properties` 都要删掉；`.gitignore` 挡得住提交，但挡不住文件夹复制 |
| 手势阈值用绝对像素 | 按屏宽百分比算，见 §4.3 |
| `TCLASS()` 反射重新生成失败 | 加成员后确认 `tiny3d_enable_reflection` 跑过；命令源不进反射 |
| ES 3.0 真机 | 整个 sample 的 GLES shader 都是 `#version 310 es`，ES 3.1 是地板。MSAA RT 创建有 `glCheckFramebufferStatus` 兜成明确报错，不会静默黑屏。测试机确认 ES 3.1+ 即可 |
| 首次 Android 构建卡在 rpp | 见 §6.1 第 3 条，是既有机制不是新问题 |

---

## 11. 与既有文档的关系

| 文档 | 关系 |
|------|------|
| `PostProcessingApp-Design-todo.md` | 本文是它的移动端延伸。预设表、效果组件、验收标准全部沿用；只替换输入层、增加 Android 平台 |
| `Camera-PostProcess-Design-todo.md` | §12 的 GLES3 缺口已由工作区实现补齐；**§12.2 / §12.3 / §12.5 关于 GL4 和 GLES3 的记录都已过期，本轮回填**（§5.6） |
| `GLES3-Renderer-Backend-todo.md` | A.8 / A.10.5 的 blit 条目已收口，本轮标记完成 |
| `GPU-Readback-onRender-Design-todo.md` | **「GLES3 `supportsReadback` 为 false」已过期**，现在是 true 且有真实现；本轮回填，并补记「仅 2D 彩色、不支持深度」（§5.5） |
| `Metal-Renderer-Backend-todo.md` | iOS 的前置，见 §3 |
| `InputApp` | 手势层的参考实现来源，不是依赖；`CubeControllerBehaviour` 不动 |
| `BehaviourApp` | **Android 工程的照抄源**（§6），也是 M0 验工具链的冒烟对象。`BehaviourApp` 本身不动 |

---

## 12. 一句话

**原本判定为最大阻塞的 GLES3 blit 缺口已经在工作区补完了（还顺带补了回读、反射、MSAA 采样），这个 sample 上 Android 现在只剩三件事：把切预设从键盘解耦成触摸手势、抄一套 Gradle 脚手架、以及拿真机核对 GLES3 与 GL4 之间那 5 处实现分歧。**
