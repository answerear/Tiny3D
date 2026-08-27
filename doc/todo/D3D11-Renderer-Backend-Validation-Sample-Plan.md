# D3D11 后端验证 Sample 实现计划

> 本文规划如何用 Sample 验证 `D3D11-Renderer-Backend-Implementation-Plan.md` 中已落地的接口。
>
> - 实现方案：`doc/todo/D3D11-Renderer-Backend-Implementation-Plan.md`
> - 接口状态：`doc/todo/D3D11-Renderer-Backend-todo.md`
>
> 结论先行：本方案**不做**逐像素/逐字节的自动断言，因为引擎层目前没有 GPU 读回能力（见 §1.1）。正确性判定靠「程序化生成的可辨识图案 + 屏幕观察 + RenderDoc 抓帧 + D3D11 debug layer」四件套。这是当前工具链下能做到的上限，不是偷懒。

---

## 0. 目标与非目标

### 目标

覆盖本轮 D3D11 后端补完的三组接口，让每一条新增的代码路径至少被跑到一次，且错误能被人观察到：

- **资源创建**：`createPixelBuffer1D` / `2D` / `3D` / `Cubemap` 的 mip 链与 array slice 切分
- **数据传输**：`blit` 四个重载、`copyBuffer`
- **尺寸重建**：`resizeRenderTexture` / `resizeRenderTarget`
- **窗口深度模板**：恢复启用的窗口自带 DepthStencil（Plan §3.4 的 A4）

### 非目标

- 不做性能测试
- 不做跨后端一致性对比（GL4 / Vulkan 的对应接口尚未 override，见 §6.3）
- 不追求 CI 可跑的无人值守自动化（缺 readback，做不到；见 §9）

---

## 1. 三条决定方案形态的前置结论

这三条是写代码前必须知道的，否则方向会跑偏。

### 1.1 引擎层没有 GPU readback，无法做自动断言

排查过引擎里所有可能的读回路径，结论是都不可用：

| 候选路径 | 位置 | 为什么不可用 |
|---------|------|-------------|
| `Image::save` | T3DImage.h:129-131 | 声明与实现都是注释状态 |
| `RenderBuffer::readData`（同步） | T3DRenderBuffer.cpp:83-103 | 仅当 `kStatic && memType != kVRAM` 时读 CPU 镜像；纹理默认 `kVRAM + kCPUNone`，读不到 GPU 实际内容 |
| `RenderBuffer::readData`（异步） | T3DRenderBuffer.cpp:137 | `// TODO: 通过 RHIContext 读数据`，校验全过之后就是空的 |
| `RenderBuffer::copyData` | T3DRenderBuffer.cpp:189-201 | 两个重载都是 `// TODO: 暂不支持`，恒返回 0 |
| `TextureManager::saveTexture` | T3DTextureManager.h:192-200 | 序列化磁盘资源，不是从 GPU 读像素 |

**推论**：sample 里写不出 `T3D_ASSERT(pixel == expected)` 这种断言。因此像素图案的设计必须让**人眼和 RenderDoc 能一眼判读**，这是 §3 存在的理由。

### 1.2 应用层唯一的 RHI 挂钩点是 `runForEditor`

`Agent::run()`（T3DAgent.cpp:640-665）每帧固定跑：

```
beginFrame -> pollEvents -> update -> renderOneFrame -> endFrame
```

其中没有任何暴露给应用的渲染前后回调。`Application` 基类（T3DApplication.h:95-103）只有五个生命周期纯虚函数，没有 `onUpdate` / `onRender`。

唯一的口子是 `Agent::runForEditor(const EditorRunningData &)`（T3DAgent.h:178）：

```cpp
struct EditorRunningData
{
    Update              update {nullptr};       // 在 Agent::update 之后、渲染之前
    PreEngineRender     preRender {nullptr};    // 在 ctx->beginRender() 之前
    PostEngineRender    postRender {nullptr};   // 在 ctx->endRender() 之后、swapBuffers 之前
};
```

调用点在 T3DAgent.cpp:678-681（preRender）与 T3DAgent.cpp:700-703（postRender）。

**推论**：BlitApp 要手动发 `ctx->blit()`，必须重写 `go()` 改用 `runForEditor`，不能用 `SampleWindowApp::go()` 里的 `run()`。

> 不要试图在 `Behaviour::onUpdate()` 里发 RHI 绘制命令。`update` 发生在渲染 pass 之外，时序不对，`setRenderTarget` 之类的调用会和管线自己的状态互相踩。`onUpdate` 只用来读输入、切用例。

### 1.3 常规 Camera 路径绕开了窗口的 DepthStencil

`Camera::setRenderTarget` 在目标类型是 `E_RT_WINDOW` 时，会调 `setupRenderTexture` 另建一张中间 RenderTexture（T3DCamera.cpp:303-334、389-422），相机实际画在中间 RT 上，画完由 `ForwardRenderPipeline::renderForward` blit 到窗口（T3DForwardRenderPipeline.cpp:567）。

这意味着**所有走常规 Camera 的 sample，窗口那套 DSV 从头到尾都不参与渲染**。A4 恢复的窗口 DepthStencil 与 `clearDepthStencilView` 的回退分支，在默认路径下一次都跑不到。

**推论**：验证 A4 必须在 `postRender` 里手动 `ctx->setRenderTarget(windowRT)` 直接画，见 §5.5。

---

## 2. 接口覆盖矩阵

| 接口 / 改动 | 承载用例 | 验证手段 | 判读标准 |
|------------|---------|---------|---------|
| `createPixelBuffer2D` 逐 mip pitch | TextureApp 用例 1 | 屏幕 + RenderDoc | 指定 mip 采样出的颜色与色表一致 |
| `createPixelBuffer2D` array slice | TextureApp 用例 2 | 屏幕 + RenderDoc | 各 slice 颜色按编码规则递增 |
| `createPixelBuffer1D` | TextureApp 用例 3 | 屏幕 + RenderDoc | 横条纹颜色随 mip 变化 |
| `createPixelBuffer3D` | TextureApp 用例 4 | 屏幕 + RenderDoc | 沿 W 切片颜色正确 |
| `createPixelBufferCubemap` | TextureApp 用例 5 | 屏幕 + RenderDoc | 六面色序为 +X/-X/+Y/-Y/+Z/-Z |
| `TEXTURECUBEARRAY` 分支 | TextureApp 用例 6 | RenderDoc | SRV 维度为 TEXTURECUBEARRAY |
| `TEXTURE1DARRAY` 分支 | TextureApp 用例 3 附带项 | RenderDoc | SRV 维度为 TEXTURE1DARRAY |
| `blit(Texture→RenderTarget)` | BlitApp 用例 1 | 屏幕 | 右半屏出现源图内容 |
| `blit(RenderTarget→RenderTarget)` | BlitApp 用例 2 | 屏幕 | 同上 |
| `blit(RenderTarget→Texture)` | BlitApp 用例 3 | 屏幕 | 同上 |
| `blit(Texture→Texture)` | BlitApp 用例 4 | 屏幕 | 同上 |
| `doBlit` 的 CopyResource 分支 | BlitApp 用例 1-4 整资源模式 | RenderDoc 事件列表 | 抓到 CopyResource 调用 |
| `doBlit` 的区域拷贝分支 | BlitApp 用例 5（带 srcOffset） | 屏幕 | 只有指定矩形被搬过去 |
| `doBlit` 的全屏四边形分支 | BlitApp 用例 6（带缩放） | 屏幕 + RenderDoc | 出现 Draw(4) 而非 Copy |
| `ResolveSubresource` 分支 | BlitApp 用例 7（MSAA 源） | RenderDoc | 抓到 ResolveSubresource |
| `T3D_ERR_D3D11_INCOMPATIBLE_COPY` | BlitApp 用例 8 | 日志 | 返回该错误码并打印两端描述 |
| `copyBuffer` 整段与带偏移 | BlitApp 用例 9 | 屏幕（顶点缓冲载体） | 画出的三角形位置正确 |
| `copyBuffer` 边界校验 | BlitApp 用例 10 | 日志 | 越界参数被 `ByteWidth` 校验拦下 |
| `resizeRenderTexture` | BlitApp 用例 11 | 屏幕 + debug layer | 重建后画面正常，无 live object 泄漏 |
| `resizeRenderTarget` | BlitApp 用例 12 | 屏幕 | 颜色与深度附件同步改尺寸 |
| 窗口自带 DepthStencil | BlitApp 用例 13 | 屏幕 | 遮挡关系正确 |
| `clearDepthStencilView` 回退 | BlitApp 用例 13 | 屏幕 | 未挂外部 DS 时仍能清深度 |
| VSync（`PresentSyncInterval`） | 任意 sample | 帧率 | 改 cfg 的 vsync 后帧率被锁到刷新率 |

---

## 3. 通用设计：可辨识像素图案

这是整个方案的地基。没有 readback，判读全靠图案本身携带位置信息。

### 3.1 数据排布必须与 `buildSubresourceData` 一致

`D3D11Context::buildSubresourceData`（T3DD3D11Context.cpp:1959）对扁平数据的切分规则是**外层 array slice、内层 mip level**：

```cpp
const uint8_t *cursor = data;
for (uint32_t slice = 0; slice < arraySize; ++slice)
{
    for (uint32_t level = 0; level < mipLevels; ++level)
    {
        const uint32_t mipW = std::max<uint32_t>(1, width >> level);
        const uint32_t mipH = std::max<uint32_t>(1, height >> level);
        const uint32_t mipD = std::max<uint32_t>(1, depth >> level);
        const uint32_t rowPitch = mipW * bpp;
        const uint32_t slicePitch = rowPitch * mipH;

        // D3D11 的子资源索引恒为 arraySlice * mipLevels + mipLevel
        outSubresources[slice * mipLevels + level].pSysMem = cursor;
        // ...
        cursor += slicePitch * mipD;
    }
}
```

所以 sample 生成数据时的写入顺序必须是：

```
slice0[mip0, mip1, ..., mipN], slice1[mip0, ...], ..., sliceM[...]
```

Cubemap 复用同一套逻辑，`arraySize` 存的是**总面数**（`cubeCount * 6`），面序 +X/-X/+Y/-Y/+Z/-Z。

> 这里有个自我印证的意味：sample 的数据生成函数和被验证的 `buildSubresourceData` 用的是同一套排布假设。如果两边同时理解错了，测不出来。所以 §8 要求必须用 RenderDoc 对照 D3D11 自己解释出来的 mip/slice，那才是第三方裁判。

### 3.2 色表编码规则

用 `E_PF_A8R8G8B8`（或 `E_PF_B8G8R8A8`，与现有 sample 一致即可），每个子资源填单一纯色：

```
R 通道 = mipLevel 的编码：mip0=0xFF, mip1=0xC0, mip2=0x80, mip3=0x40, mip4=0x20 ...
G 通道 = arraySlice 的编码：slice * (0xFF / max(1, arraySize - 1))
B 通道 = 维度标记：1D=0x20, 2D=0x40, 3D=0x60, Cube=0x80
A 通道 = 0xFF
```

Cubemap 单独一套，因为面序比 mip 更值得看：

```
+X = 红   (0xFF, 0x00, 0x00)
-X = 青   (0x00, 0xFF, 0xFF)
+Y = 绿   (0x00, 0xFF, 0x00)
-Y = 品红 (0xFF, 0x00, 0xFF)
+Z = 蓝   (0x00, 0x00, 0xFF)
-Z = 黄   (0xFF, 0xFF, 0x00)
```

mip 通过整体降低亮度体现：`color * (1.0 - mipLevel * 0.15)`。

3D 纹理的 depth 方向额外在 B 通道叠加 `z * 0x10`，这样沿 W 切片能看出层次。

**为什么用纯色而不是棋盘格或渐变**：纯色在缩放、mip 过滤、blit 之后仍然保持可判读；渐变一旦被双线性过滤就无法区分「采样到了错误的 mip」和「采样到了正确 mip 的边缘」。

### 3.3 生成函数骨架

放在 `source/Samples/Common/` 下新建 `SamplePixelPatterns.h/cpp`，供 TextureApp 与 BlitApp 共用：

```cpp
namespace SamplePattern
{
    /// 计算 [arraySlice][mipLevel] 排布下的总字节数
    size_t calcTotalBytes(uint32_t width, uint32_t height, uint32_t depth,
                          uint32_t mipLevels, uint32_t arraySize, uint32_t bpp);

    /// 按 §3.1 的顺序、§3.2 的色表生成像素数据
    /// 返回的 Buffer 由调用方转交给 TextureManager（会接管所有权）
    Tiny3D::Buffer buildPattern(uint32_t width, uint32_t height, uint32_t depth,
                                uint32_t mipLevels, uint32_t arraySize,
                                Tiny3D::TEXTURE_TYPE type);

    /// Cubemap 专用，按 +X/-X/+Y/-Y/+Z/-Z 面序
    Tiny3D::Buffer buildCubePattern(uint32_t faceSize, uint32_t mipLevels, uint32_t cubeCount);

    /// 给定 mip/slice，返回期望颜色，用于日志打印与 RenderDoc 对照
    Tiny3D::ColorRGB expectedColor(uint32_t mipLevel, uint32_t arraySlice,
                                   Tiny3D::TEXTURE_TYPE type);
}
```

`expectedColor` 很重要：用例切换时把期望颜色打进日志，看屏幕时不用回头翻代码算。

> 不引入任何新美术资源，全部程序化生成。现有 `assets/samples/textures/blocks.png` 在仓库里的状态不确定（TextureApp 的 CMake 只在 Android 分支拷贝纹理目录），新用例不依赖它。

---

## 4. 任务一：扩展 TextureApp

### 4.1 改造目标

`TextureApp` 现在是「从 blocks.png 加载 2D 纹理 + 手工 24 顶点立方体」（TextureApp.cpp:62-82），只覆盖 `createPixelBuffer2D` 的 mip=1 / array=1 最简单分支。

改造为按数字键切换纹理类型的验证台，保留立方体作为显示载体（六个面正好能一次看到 cubemap 的三个面）。

原有的 blocks.png 用例保留为用例 0，作为回归基线——如果改造后连它都不对，说明是改坏了框架而不是新代码有问题。

### 4.2 用例清单

下表的「用例号」是 0-based 的内部索引，「按键」是实际要按的数字键，两者差 1（键 `1` 触发用例 0）。

| 按键 | 用例号 | 用例 | 创建方式 | 参数 | 验证点 |
|------|-------|------|---------|------|-------|
| 1 | 0 | 基线 | `createTexture2D(name, image)` | blocks.png | 回归基线，确认框架没改坏 |
| 2 | 1 | 2D + mip | `createTexture2D(name, w, h, fmt, data, mipmaps)` | 256x256, mip=5 | 逐 mip pitch 计算 |
| 3 | 2 | 2D Array + mip | `createTexture2DArray(...)` | 128x128, array=4, mip=4 | `slice * mipLevels + level` 排布 |
| 4 | 3 | 1D + mip | `createTexture1D(name, w, fmt, data, mipmaps)` | 256, mip=5 | 1D 的 SRV 维度与 pitch |
| 5 | 4 | 3D | `createTexture3D(name, w, h, d, fmt, data, mipmaps)` | 64x64x16, mip=3 | `SysMemSlicePitch` 与 depth 递推 |
| 6 | 5 | Cubemap + mip | `createCubemap(name, w, h, fmt, data, mipmaps)` | 128x128, mip=4 | 面序与 mip 链 |
| 7 | 6 | Cubemap Array | `createCubemapArray(name, w, h, fmt, arraySize, data, mipmaps)` | 64x64, cubeCount=2, mip=3 | `TEXTURECUBEARRAY` 分支 |

签名出处见 T3DTextureManager.h:61-167。本文其余章节提到「用例 N」时一律指用例号。

### 4.3 1D 纹理数组的限制

`TextureManager::createTexture1D`（T3DTextureManager.h:72）**没有暴露 `arraySize` 参数**，`Texture1DArray` 在 T3DTypedef.h 里只有 typedef 没有实现类。

因此 1D array（对应 D3D11 侧的 `D3D11_SRV_DIMENSION_TEXTURE1DARRAY` 分支）无法通过 TextureManager 创建。两个选择：

- **推荐**：手填 `PixelBuffer1DDesc`（T3DRenderBufferDesc.h:124-149，有 `arraySize` 字段）直接走 `T3D_RENDER_BUFFER_MGR.loadPixelBuffer1D`，绕开 Texture 层。这样能测到 RHI 分支，代价是拿不到 `Texture` 对象、不能直接挂 Material，只能靠 RenderDoc 看
- 或者接受 1D array 分支不覆盖，在文档里标注为盲区

本计划采用推荐方案，用例 3 里附带创建一个 array=3 的 1D PixelBuffer，仅用于 RenderDoc 观察，不上屏。

手填 Desc 时注意**用 `gpuAccess` 而不是 `shaderReadable`**：后者已标注 `\deprecated`，构造时会被折叠进 `gpuAccess`，待调用点迁移完就删除（T3DRenderBufferDesc.h:141-148）。

```cpp
PixelBuffer1DDesc desc;
desc.width     = 256;
desc.mipmaps   = 5;
desc.arraySize = 3;
desc.format    = PixelFormat::E_PF_B8G8R8A8;
desc.buffer    = SamplePattern::buildPattern(256, 1, 1, 5, 3, TEXTURE_TYPE::TT_1D);
desc.gpuAccess = kGPUShaderResource;   // GPUAccessFlags，T3DRenderConstant.h:278-288
```

### 4.4 用例切换

新建 `TextureCaseBehaviour`，挂在立方体的 GameObject 上，模式照抄 `CubeControllerBehaviour::onUpdate`（InputApp/CubeControllerBehaviour.cpp:100-120）：

```cpp
void TextureCaseBehaviour::onUpdate()
{
    // caseIndex 是 0-based，数字键 1 对应用例 0
    for (int caseIndex = 0; caseIndex < kCaseCount; ++caseIndex)
    {
        if (T3D_INPUT.getKeyDown(static_cast<ScanCode>(APP_SCANCODE_1 + caseIndex)))
        {
            switchCase(caseIndex);
            break;
        }
    }

    // M / N 切换当前显示的 mip level，K / L 切换 array slice
    if (T3D_INPUT.getKeyDown(APP_SCANCODE_M)) { stepMipLevel(+1); }
    if (T3D_INPUT.getKeyDown(APP_SCANCODE_N)) { stepMipLevel(-1); }
    if (T3D_INPUT.getKeyDown(APP_SCANCODE_K)) { stepArraySlice(+1); }
    if (T3D_INPUT.getKeyDown(APP_SCANCODE_L)) { stepArraySlice(-1); }
}
```

ScanCode 定义在 T3DAppKeyboard.h:69-71（`APP_SCANCODE_1 = 30` 起连续）。用 `getKeyDown` 而非 `getKey`，取单帧边沿。

**mip / slice 的切换靠常量缓冲传给 shader，用 `SampleLevel` 显式指定**，不能靠自动 mip 选择——自动选择由屏幕上的三角形密度决定，测不到指定的那一层。

切换时打日志，把期望颜色一并打出来：

```
[TextureApp] case 2 (2D Array), mip=1/4, slice=2/4, expected RGB = (0xC0, 0xAA, 0x40)
```

### 4.5 判读标准

- 立方体表面颜色与日志里的 expected RGB 一致 -> 该 mip/slice 的子资源偏移正确
- 颜色对但亮度错一档 -> mip 偏移差了一级，查 `buildSubresourceData` 的 `cursor` 累加
- 颜色完全不对或是噪声 -> pitch 算错，读到了相邻子资源的数据
- 花屏或 debug layer 报 `D3D11 ERROR: ... CopySubresourceRegion` -> 数据量校验没拦住越界

---

## 5. 任务二：新建 BlitApp

### 5.1 骨架与主循环

新建 `source/Samples/BlitApp/{BlitApp.h, BlitApp.cpp, CMakeLists.txt}`。

与其它 sample 的**唯一结构性差异**是重写 `go()`（理由见 §1.2）：

```cpp
TResult BlitApp::go(int32_t argc, char *argv[])
{
    TResult ret;
    Tiny3D::Agent *theEngine = T3D_NEW Tiny3D::Agent();

    ret = theEngine->init(argc, argv, true, true);
    if (ret == T3D_OK)
    {
        // blit 必须发生在 RHI 的 beginRender/endRender 之外，
        // 用 postRender 钩子拿到 endRender 之后、swapBuffers 之前的时机
        EditorRunningData data;
        data.postRender = [this]() { this->onPostRender(); };
        theEngine->runForEditor(data);
    }

    T3D_SAFE_DELETE(theEngine);
    return ret;
}
```

`onPostRender()` 里按当前用例分派，拿 context 的写法：

```cpp
RHIContextPtr ctx = T3D_AGENT.getActiveRHIContext();
```

> `getActiveRHIContext()` 在 `mActiveRHIRenderer` 为空时会崩（T3DRHIContext.h 相关注释），但 `postRender` 阶段渲染器必然已就绪，不必额外判空。

### 5.2 场景与资源准备

`applicationDidFinishLaunching` 里准备好所有用例共用的资源，避免运行时反复创建：

```cpp
// 源：一张离屏 RT，用相机渲染一个转动的立方体进去，内容随时间变化，
// 这样 blit 结果是不是"活的"一眼能看出来（静态图看不出 blit 到底执行没有）
mSrcColorRT = T3D_TEXTURE_MGR.createRenderTexture("BlitSrcColor", 512, 512,
                    PixelFormat::E_PF_B8G8R8A8, 1, 1, 0, true);
mSrcDepthRT = T3D_TEXTURE_MGR.createRenderTexture("BlitSrcDepth", 512, 512,
                    PixelFormat::E_PF_D24_UNORM_S8_UINT, 1, 1, 0, false);
// RenderTarget::create 收的是裸指针，智能指针要 .get()（T3DRenderTarget.h:63-79）
mSrcRT = RenderTarget::create(mSrcColorRT.get(), mSrcDepthRT.get());

// 相机渲染到这张离屏 RT。注意传 E_RT_TEXTURE 类型的 RenderTarget，
// Camera 不会再另建中间 RT，也就不会触发管线自动 blit（见 T3DCamera.cpp:319-325）
camera->setRenderTarget(mSrcRT);

// 目标：普通 2D 纹理 / 另一张 RT / MSAA RT
mDstTexture = T3D_TEXTURE_MGR.createTexture2D("BlitDst", 512, 512, fmt, emptyBuffer);
mMsaaColorRT = T3D_TEXTURE_MGR.createRenderTexture("BlitMsaaSrc", 512, 512,
                    PixelFormat::E_PF_B8G8R8A8, 1, 4, 0, true);

// 窗口 RT，作为最终显示目标。第二参数留空表示不挂外部深度模板，
// 于是 setRenderTarget 会用窗口自带的 DSV —— 这正是 §5.6 要验证的分支
mWindowRT = RenderTarget::create(T3D_AGENT.getDefaultRenderWindow().get());
```

**这里有个关键取舍**：相机渲染到 `E_RT_TEXTURE` 后，画面不会自动出现在屏幕上。这正是我们要的——屏幕上看到的每一个像素都是 BlitApp 自己 blit 过去的，没有管线的自动路径掺和，验证结果不会被误判。

### 5.3 用例清单

每个用例把结果 blit 到窗口，左半屏放源、右半屏放结果做对照（靠 `dstOffset` 控制）。BlitApp 的按键与用例号一致，数字键 1 就是用例 1。

四个 `blit` 重载的参数表完全相同（T3DRHIContext.h:600-633），差别只在 src / dst 的类型：

```cpp
blit(src, dst,
     const Vector3 &srcOffset = Vector3::ZERO,   // 源区域起点，按 src 维度解释
     const Vector3 &size      = Vector3::ZERO,   // 传输区域尺寸，全零表示整资源
     const Vector3  dstOffset = Vector3::ZERO);  // 目标区域起点，按 dst 维度解释
```

| 键 | 用例 | 调用 | 期望路径 | 判读 |
|----|------|------|---------|------|
| 1 | Texture -> RenderTarget | `blit(mSrcColorRT, mWindowRT)` | CopyResource 或全屏四边形 | 右半屏出现转动的立方体 |
| 2 | RenderTarget -> RenderTarget | `blit(mSrcRT, mWindowRT)` | 同上 | 同上 |
| 3 | RenderTarget -> Texture | `blit(mSrcRT, mDstTexture)` 再把 `mDstTexture` blit 上屏 | CopyResource | 两跳之后画面仍正确 |
| 4 | Texture -> Texture | `blit(mSrcColorRT, mDstTexture)` 再上屏 | CopyResource | 同上 |
| 5 | 区域拷贝 | `blit(src, dst, Vector3(128,128,0), Vector3(256,256,0), Vector3(0,0,0))` | CopySubresourceRegion | 只有源图中心矩形被搬到左上角 |
| 6 | 带缩放 | 源 512x512 blit 到 256x256 目标 | `blitRegion` 全屏四边形 | 内容被正确缩小，RenderDoc 里能看到 Draw(4) |
| 7 | MSAA 源 | 先渲染进 `mMsaaColorRT`，再 blit 上屏 | ResolveSubresource | 边缘平滑，RenderDoc 抓到 Resolve |
| 8 | 不兼容组合 | 窗口 BackBuffer 作源 + 需要缩放 | 应失败 | 日志出现 `T3D_ERR_D3D11_INCOMPATIBLE_COPY` 与两端描述，不崩 |
| 9 | copyBuffer | 见 §5.4 | CopyResource / CopySubresourceRegion | 三角形位置正确 |
| 10 | copyBuffer 越界 | 见 §5.4 | 应失败 | 被 `ByteWidth` 校验拦下并打日志 |
| 11 | resizeRenderTexture | 见 §5.5 | 资源重建 | 重建后画面正常 |
| 12 | resizeRenderTarget | 见 §5.5 | 颜色+深度同步 | 同上 |
| 13 | 窗口 DepthStencil | 见 §5.6 | 窗口原生 DSV | 遮挡关系正确 |

用例 8 是**故意制造失败**的用例。SwapChain 的 `BufferUsage` 只有 `DXGI_USAGE_RENDER_TARGET_OUTPUT` 没有 `DXGI_USAGE_SHADER_INPUT`（见 createRenderWindow 的 `d3dSwapChainDesc.BufferUsage`），BackBuffer 拿不到 SRV，走不了全屏四边形。要确认这种情况下是明确报错而不是静默拿到空指针。

### 5.4 copyBuffer 的可视化技巧

`copyBuffer` 拷的是顶点/索引/常量缓冲的裸字节，无法直接观察。用**顶点缓冲当载体**把它变成可视的：

```
源 VB：  [三角形 A 的 3 个顶点][三角形 B 的 3 个顶点]   共 6 个顶点
目标 VB：先填满退化顶点（全 0，画出来什么都看不见）
```

用例 9 分三步：

1. `copyBuffer(srcVB, dstVB)` 整段拷贝，画 6 个顶点 -> 应看到 A、B 两个三角形
2. `copyBuffer(srcVB, dstVB, 3*stride, 3*stride, 0)` 只拷 B 到目标开头，画前 3 个顶点 -> 应看到 B 出现在 A 的位置
3. `copyBuffer(srcVB, dstVB, 0, 3*stride, 3*stride)` 只拷 A 到目标后半，画后 3 个顶点 -> 应看到 A 出现在 B 的位置

**偏移或长度算错，画出来的就是错位的三角形或者什么都没有**，比读字节直观。三角形用不同顶点色区分 A 和 B。

用例 10 传越界参数（`srcOffset + size > ByteWidth`），确认被 `ID3D11Buffer::GetDesc` 拿到的真实 `ByteWidth` 校验拦下，返回 `T3D_ERR_INVALID_PARAM` 或 `T3D_ERR_D3D11_INCOMPATIBLE_COPY`，且 debug layer 不报错。

同时要验证纹理类资源被拒绝：传一个 PixelBuffer 进 `copyBuffer`，应返回 `T3D_ERR_D3D11_UNSUPPORTED_OPERATION` 并提示改走 `blit`。

### 5.5 resize 用例

用例 11 按 R 键在几个尺寸间循环（512 -> 800 -> 256 -> 512），调引擎侧入口：

```cpp
mSrcColorRT->resize(w, h);   // RenderTexture::resize，T3DRenderTexture.cpp:74
mSrcDepthRT->resize(w, h);
```

**`RenderTexture::resize` 有两条早退路径，测试时不要被它们误导**（T3DRenderTexture.cpp:82-94）：

- 新旧尺寸相同时直接返回 `T3D_OK`，不碰 GPU 资源。所以循环里每一档尺寸都必须不同，连着按两次 R 停在同一档会看不到任何变化，这是设计如此，不是 bug
- `mPixelBuffer` 或其 `getRHIResource()` 为空时，只改描述就返回 `T3D_OK`。因此 resize 必须在 RT 真正创建之后再调（至少渲染过一帧），否则走不到 `resizeRenderTexture`，等于什么都没测

用例里把实际返回值和是否走到 RHI 打进日志，避免"看着没反应"分不清是早退还是真出错。

用例 12 直接调 RHI 层的 `resizeRenderTarget`，一次性改整个 RenderTarget 的所有附件：

```cpp
// resizeRenderTarget 同样收裸指针（T3DRHIContext.h:123）
T3D_AGENT.getActiveRHIContext()->resizeRenderTarget(mSrcRT.get(), w, h);
```

注意基类给 `resizeRenderTarget` / `resizeRenderTexture` 的默认实现是直接返回 `T3D_ERR_NOT_IMPLEMENT`（T3DRHIContext.h:123），只有 D3D11 做了 override。所以这两个用例在别的后端上拿到的是错误码而不是崩溃，符合 §6.3 的要求。

判读要点，三条都要过：

- 重建后画面立刻正常，没有花屏、没有黑帧残留（说明旧 RTV/SRV 解绑干净了）
- 相机的 aspect ratio 要跟着改，否则画面会拉伸——这是 sample 自己的责任，不是被测代码的 bug，别误判
- 退出时 `ReportLiveDeviceObjects` 不应因为反复 resize 而出现数量递增的 live object

> `releaseRenderTextureResources`（T3DD3D11Context.cpp:788）会先把管线上残留的 RTV/SRV 绑定解掉再释放 COM。如果这一步有问题，症状是「resize 后画面还是旧内容」或者「live object 计数随 resize 次数线性增长」。这个用例就是冲着它去的。

### 5.6 窗口 DepthStencil 用例

理由见 §1.3——这是唯一能跑到窗口原生 DSV 的路径。

在 `postRender` 里手动绘制，不经过 Camera：

```cpp
void BlitApp::testWindowDepthStencil(RHIContext *ctx)
{
    // 直接绑窗口 RT，不传外部 depthStencil，
    // 走 setRenderTarget 里「没有外挂 RenderTexture 就用窗口自带 DSV」的分支
    // setRenderTarget 收裸指针（T3DRHIContext.h:130），智能指针要 .get()
    ctx->setRenderTarget(mWindowRT.get());

    Viewport vp {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    ctx->setViewport(vp);

    ctx->clearColor(ColorRGB::BLACK);
    ctx->clearDepthStencil(1.0f, 0);   // 走 clearDepthStencilView 的窗口回退分支

    ctx->beginPass();
    // 画两个 z 不同、屏幕上重叠的四边形，先画远的再画近的
    drawQuad(ctx, /*z=*/0.8f, ColorRGB::RED);
    drawQuad(ctx, /*z=*/0.3f, ColorRGB::GREEN);
    ctx->endPass();

    ctx->reset();
}
```

判读：

- 近的绿色四边形遮住远的红色 -> 深度测试生效，窗口 DSV 工作正常
- 绘制顺序反过来（先近后远）结果不变 -> 确实是深度测试在起作用，不是绘制顺序的假象。**这一步必须做**，否则无法区分「深度测试生效」和「后画的覆盖先画的」
- debug layer 不报 RTV/DSV 尺寸不匹配 -> A4 里「深度模板尺寸从 BackBuffer 实际描述反查」那处加固有效

注意 `setViewport` 必须在 `setRenderTarget` 之后调用，它依赖当前绑定的 RT 尺寸把归一化比例换算成像素（`Viewport` 全部字段都是 [0,1] 归一化比例，见 T3DViewport.h:36-60）。

---

## 6. Shader 增补清单

### 6.1 需要新增的常量

往 `source/Samples/Common/SampleShaders_hlsl.h` 加：

| 常量名 | 用途 | 关键点 |
|-------|------|-------|
| `SAMPLE_TEX1D_PIXEL_SHADER` | 1D 采样 | `Texture1D<float4>`，`SampleLevel(s, u, mip)` |
| `SAMPLE_TEX2D_LOD_PIXEL_SHADER` | 2D 指定 mip | `SampleLevel(s, uv, mip)` |
| `SAMPLE_TEX2DARRAY_PIXEL_SHADER` | 2D Array | `Texture2DArray`，`SampleLevel(s, float3(uv, slice), mip)` |
| `SAMPLE_TEX3D_PIXEL_SHADER` | 3D | `Texture3D`，`SampleLevel(s, float3(uv, w), mip)` |
| `SAMPLE_TEXCUBE_PIXEL_SHADER` | Cubemap | `TextureCube`，方向向量用立方体的物体空间法线 |
| `SAMPLE_TEXCUBEARRAY_PIXEL_SHADER` | Cubemap Array | `TextureCubeArray`，`float4(dir, cubeIndex)` |
| `SAMPLE_UNLIT_COLOR_PIXEL_SHADER` | copyBuffer 用例画三角形 | 直接输出顶点色，不采样 |

顶点着色器可复用现有 `SAMPLE_VERTEX_SHADER`；cubemap 用例需要一个额外传法线的变体。

mip / slice 通过一个新的 cbuffer 传入。

### 6.2 命名硬约束（写错会直接失败）

现有 HLSL 常量是 **SPIRV-Cross 的交叉编译产物**（可以从 `SPIRV_Cross_Input` / `out_var_TEXCOORD0` 这些命名看出来）。手写新 shader 时，有两条命名规则**不是风格问题而是硬约束**，因为 `reflectShaderAllBindings`（T3DD3D11ContextBase.cpp:144）按它们解析：

**其一，cbuffer 必须命名为 `type_XXX`，且内部变量以 `XXX_` 为前缀。**

`getOriginalName` 的逻辑是：cbuffer 名去掉前 5 个字符（`type_`）得到真名，变量名再去掉「真名 + 下划线」得到变量真名。特例是 `type_Globals` 去掉前 4 个字符。

```
"cbuffer type_SampleTexParams : register(b2)\n"
"{\n"
"    int SampleTexParams_mipLevel : packoffset(c0);\n"
"    int SampleTexParams_arraySlice : packoffset(c0.y);\n"
"};\n"
```

不遵守的话 `getOriginalName` 会切出乱码名字，而它的返回值被 `T3D_ASSERT(rval)` 检查，Debug 下直接断言失败。

**其二，采样器必须命名为 `sampler` + 纹理名，中间没有下划线。**

```
"Texture2DArray<float4> texArray : register(t0);\n"
"SamplerState samplertexArray : register(s1);\n"
```

反射代码里是 `StringUtil::startsWith(name, "sampler")` 加 `name.substr(7)`，去掉的正好是 `sampler` 这 7 个字符。不匹配会返回 `T3D_ERR_D3D11_INVALID_SHADER_SAMPLER_NAME`。

> 注意该错误的日志文案写的是 "It must start with 'sampler_'"，带下划线，与代码实际行为（不带下划线）不符。照日志写反而会错。这处文案本身值得顺手修，但不在本计划范围内。

### 6.3 多后端策略：只写 HLSL

GL4 / GLES3 的 GLSL 与 Vulkan 的 SPIR-V **不做**。理由：

- Vulkan 那份是 `alignas(4) const unsigned char[]` 的 SPIR-V 字节码数组，由 `scc.exe` 编译 + `embed_spirv.py` 内嵌生成，需要跑完整工具链
- 本轮验证目标是 D3D11 后端，其它后端的 `resizeRenderTexture` / `resizeRenderTarget` 都还没 override，跑过去只会拿到 `T3D_ERR_NOT_IMPLEMENT`

**运行在非 D3D11 后端时的行为要求**：用例初始化阶段做后端检查，不匹配就打一条 warning 并把该用例标记为不可用，按键切过去时只显示提示不执行。

```cpp
// 用 RHIRenderer::DIRECT3D11 常量（T3DRHIRenderer.h:48），不要写字符串字面量
const bool isD3D11 = (T3D_AGENT.getActiveRHIRenderer()->getName() == RHIRenderer::DIRECT3D11);
```

活动后端由 `Settings::renderer` 决定，默认就是 `"Direct3D11"`（T3DSettings.h:71），改 `Tiny3D.cfg` 即可切换。

**不能崩，也不能默默显示黑屏。** 黑屏最容易被误判成「blit 没生效」，浪费排查时间。

---

## 7. 构建与资源改动清单

### 新增文件

```
source/Samples/Common/SamplePixelPatterns.h      # 图案生成，TextureApp 与 BlitApp 共用
source/Samples/Common/SamplePixelPatterns.cpp
source/Samples/BlitApp/BlitApp.h
source/Samples/BlitApp/BlitApp.cpp
source/Samples/BlitApp/CMakeLists.txt
source/Samples/TextureApp/TextureCaseBehaviour.h
source/Samples/TextureApp/TextureCaseBehaviour.cpp
```

`Common/` 下的新文件会被所有 sample 自动编入（各 sample 的 CMakeLists 都有 `set_project_files(common ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ .cpp)`），所以 `SamplePixelPatterns.cpp` 不能引入 sample 专有的依赖。

### 修改文件

| 文件 | 改动 |
|------|------|
| `source/Samples/CMakeLists.txt` | 加 `add_subdirectory(BlitApp)` |
| `source/CMakeLists.txt` | `TINY3D_BUILD_SAMPLES` 块内加 `add_dependencies(BlitApp T3DCore)`（参照现有 `add_dependencies(TextureApp T3DCore)`） |
| `source/Samples/Common/SampleShaders_hlsl.h` | 加 §6.1 的常量 |
| `source/Samples/TextureApp/TextureApp.h/cpp` | 改造为多用例 |

`BlitApp/CMakeLists.txt` 从 `source/Samples/TextureApp/CMakeLists.txt` 复制，改 `set_project_name(BlitApp)`（第 8 行）。两处需要调整：

- Android 分支的 POST_BUILD 里去掉 `copy_directory .../assets/samples/textures` 那两行（TextureApp/CMakeLists.txt:120-122），BlitApp 不依赖磁盘资源；同一块里的 `Tiny3D.cfg` 与 `GLES3Renderer` 拷贝要保留
- 第 33-53 行按平台过滤 shader 头文件的整块要原样保留。Windows 下只移除 `SampleShaders_gles3.h`，hlsl / gl4 / vk 三份都在工程里

`set_project_files(common ${CMAKE_CURRENT_SOURCE_DIR}/../Common/ .cpp)`（第 59 行）会自动收进新增的 `SamplePixelPatterns.cpp`，不需要额外登记。

### 资源

**无新增资源。** 全部程序化生成。

---

## 8. 验证执行清单

### 8.1 每个用例都要过的基线

1. **D3D11 debug layer 全程无 ERROR/WARNING**。Debug 构建下 `D3D11Context::init()` 已经带了 `D3D11_CREATE_DEVICE_DEBUG`，跑完每个用例检查 output 窗口
2. **退出时 `ReportLiveDeviceObjects(D3D11_RLDO_DETAIL)` 无异常 live object**。资源创建类和 resize 类用例尤其要看
3. **RHI 线程开启与关闭两种模式各跑一遍**

第 3 条不能省。Implementation Plan §0.1 / §0.2 描述的坑只在多线程模式下暴露：

- 单线程模式：`ENQUEUE_UNIQUE_COMMAND` 同步执行，返回真实错误码，CPU 数据指针必然有效，深拷贝缺失和 SmartPtr 漏持有都被掩盖
- 多线程模式：返回值恒为 `T3D_OK`，lambda 延后执行，`cloneSubresourceData` 如果漏了哪条路径，这里才会崩

纹理创建类用例（TextureApp 全部）在多线程模式下尤其关键，因为 `createPixelBufferXD` 的初始数据全都依赖 `cloneSubresourceData` 的深拷贝。

### 8.2 RenderDoc 核对清单

项目已配置 RenderDoc MCP。抓帧后要看的：

| 用例 | 在 RenderDoc 里看什么 |
|------|---------------------|
| TextureApp 1-6 | Texture Viewer 逐 mip / 逐 slice 切换，颜色与 §3.2 色表对照 |
| TextureApp 6 | Pipeline State 里 SRV 维度应为 `TEXTURECUBEARRAY` |
| TextureApp 3 附带项 | 1D array 的 SRV 维度应为 `TEXTURE1DARRAY` |
| BlitApp 1-4（整资源） | 事件列表里应是 `CopyResource` |
| BlitApp 5 | 应是 `CopySubresourceRegion`，检查 `D3D11_BOX` 参数 |
| BlitApp 6 | 应是 `Draw(4)` 走全屏四边形，检查 VB 里被改写的 UV |
| BlitApp 7 | 应有 `ResolveSubresource` |
| BlitApp 9 | Mesh Viewer 看 copy 之后的 VB 内容 |

**RenderDoc 是这套方案里唯一的第三方裁判**。sample 自己生成数据、自己判读的闭环有共同模式失效的风险（§3.1 已提及），只有 RenderDoc 展示的是 D3D11 自己解释出来的子资源布局。TextureApp 的 mip/slice 核对不能只靠屏幕。

### 8.3 回归范围

改造 TextureApp 有波及风险，改完要确认这些没坏：

- `TextureApp` 用例 0（blocks.png 基线）
- 其它复用 `SampleShaders_hlsl.h` 的 sample：`GeometryApp`、`ShadowApp`、`InputApp`、`SkeletalAnimationApp`（只增常量不改现有常量，风险低，但要编译过）
- `Common/` 下新增 cpp 后所有 18 个 sample 都要能编过

---

## 9. 已知盲区与后续

写在这里是为了避免给出「已验证」的错觉。以下几点本方案覆盖不到：

### 9.1 无法自动化回归

没有 readback，全部判读靠人。改动 D3D11 后端后需要人工重跑一遍这些用例，无法挂 CI。

**建议后续单独立项做 RHI readback**：在 `RHIContext` 加 `readTexture` / `readBuffer` 接口，D3D11 侧用 `D3D11_USAGE_STAGING` + `CopyResource` + `Map` 实现。做完之后，这批 sample 可以原地升级——图案生成逻辑和期望值计算（`SamplePattern::expectedColor`）都能直接复用，只需把「看屏幕」换成 `T3D_ASSERT(actual == expected)`。这也是 §3.3 要求 `expectedColor` 独立成函数的原因。

### 9.2 Cubemap 的带缩放 blit 本身未实现

`resolveBlitEndpoint` 对 `TT_CUBE` / `TT_CUBE_ARRAY` 不提供 SRV，因为全屏四边形路径需要专门的 cube 采样 shader。本方案只验证它**是否正确返回错误码**，不验证功能。

### 9.3 不覆盖的分支

- Compute dispatch / UAV、Instanced / Indirect Draw：**描述层已经就位，执行接口还没有**。`StructuredBufferDesc` 与 `StructuredBufferKind`（T3DRenderBufferDesc.h:79-117）、`GPUAccessFlags` 的 `kGPUUnorderedAccess` / `kGPUIndirectArgs`（T3DRenderConstant.h:284-287）、`RHICapabilities` 的 `supportsStructuredBuffer` / `supportsInstancing` / `supportsIndirectDraw` 等字段都已定义，但 `RHIContext` 上还没有 `dispatch` / `createStructuredBuffer` / `drawInstanced` / `drawIndirect`，所以没有可测的执行路径。见 Implementation Plan §8
- 压缩纹理格式（BC1-BC7）的子资源切分：`buildSubresourceData` 用 `Image::getBPP(format) / 8` 算 pitch，对块压缩格式不成立。当前引擎是否支持压缩格式需另行确认，本方案只测非压缩格式
- 非 D3D11 后端的对应实现

### 9.4 时序上的一个保留意见

BlitApp 在 `postRender`（即 `ctx->endRender()` 之后）发 blit 命令，对 D3D11 是安全的——D3D11 后端的 `beginRender` / `endRender` 按设计是空实现（T3DD3D11Context.h:586-590）。

但这个位置对 Vulkan 不成立，Vulkan 的 `endRender` 会 `endCommandBuffer` + `queueSubmit`，之后再发渲染命令是错的。所以 BlitApp 的这个结构**是 D3D11 专用的**，将来若要扩展到 Vulkan 需要改用 `preRender` 或者引入新的挂钩点。这一点在 §6.3 的后端检查里已经拦住了，但要在代码注释里写明原因，避免后来者照搬。

---

## 10. 工作量估算

| 任务 | 预估 | 依赖 |
|------|------|------|
| `SamplePixelPatterns` 图案生成 | 0.5d | 无 |
| Shader 常量增补（7 个 HLSL） | 1d | 无 |
| TextureApp 改造（7 个用例 + Behaviour） | 1.5d | 上面两项 |
| BlitApp 骨架 + 场景资源准备 | 0.5d | 无 |
| BlitApp 用例 1-8（blit 家族） | 1d | 骨架 |
| BlitApp 用例 9-10（copyBuffer） | 0.5d | 骨架 |
| BlitApp 用例 11-13（resize + 窗口 DS） | 0.5d | 骨架 |
| 构建配置 | 0.2d | 无 |
| 两种线程模式跑测 + RenderDoc 核对 | 1d | 全部 |
| 合计 | 约 6.7d | |

建议顺序：`SamplePixelPatterns` -> Shader 常量 -> TextureApp -> BlitApp 骨架 -> BlitApp 各用例。TextureApp 先做是因为它验证的是资源创建，而 BlitApp 的所有用例都建立在「纹理内容是对的」这个前提上；资源创建没验证过就去测 blit，出了问题分不清是谁的锅。
