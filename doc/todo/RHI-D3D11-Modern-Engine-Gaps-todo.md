# RHI 与 D3D11 后端：支撑现代 3D 引擎的能力缺口

> **登记日期**：2026-11-24
>
> **定位**：`D3D11-Renderer-Backend-todo.md` 逐个核对的是「`RHIContext` 每个接口在 D3D11 上有没有实现」，结论是全部实现、没有空壳。本文回答另一个问题：**这套接口和实现，够不够支撑一个现代 PBR / HDR 引擎**。这里登记的是「接口层没有暴露」或「实现只做了最小形态」的能力，不是 bug。
>
> - **接口定义**：`source/Core/Include/RHI/T3DRHIContext.h`
> - **能力集**：`source/Core/Include/RHI/T3DRHICapabilities.h`
> - **引擎枚举**：`source/Core/Include/Kernel/T3DConstant.h`、`source/Core/Include/Render/T3DRenderConstant.h`
> - **D3D11 实现**：`source/Plugins/Renderer/Direct3D11/Window/Source/T3DD3D11Context.cpp`
> - **D3D11 映射**：`source/Plugins/Renderer/Direct3D11/Base/Source/T3DD3D11Mapping.cpp`
>
> 行号按 2026-11-24 的代码登记，文件后续变动后请以函数名搜索为准。

---

## 0. 结论

RHI 的**执行层**已经成熟：6 个着色器阶段、实例化 / Indirect Draw / Indirect Dispatch、StructuredBuffer / UAV / Append-Consume、`copyStructureCount`、MRT、MSAA + Resolve、三级降级的 `blit`、异步 GPU 读回，D3D11 都已完整实现，足以支撑「前向渲染 + 方向光阴影 + 后处理 + GPU 剔除 / 粒子」这一档引擎。

缺口集中在**资源描述层**：

1. 像素格式只有 8 位 LDR 与深度格式，做不了 HDR、PBR、G-Buffer、sRGB 校正，也没有块压缩纹理；
2. 渲染目标只能是单张 2D、单 mip，做不了点光源阴影、CSM、反射探针、IBL 预过滤、Hi-Z、Bloom 降采样链；
3. 缺 mip 生成、GPU 查询、调试标注等工具类能力。

D3D11 本身全部支持这些能力，瓶颈在引擎侧的枚举与描述结构。所以大多数条目都是**跨后端改动**：先扩 RHI / 引擎描述，再让 D3D11、GL4、GLES3、Vulkan、Metal 跟进。

### 按引擎功能对照

| 引擎功能 | 现状 | 阻塞条目 |
|---|---|---|
| 前向渲染、方向光阴影、天空盒、UI | ✅ 可用 | — |
| GPU 剔除、Indirect Draw、Compute 粒子 | ✅ 可用 | — |
| 曲面细分、几何着色器 | ✅ 可用 | — |
| PBR + HDR + 色调映射 / Bloom | ❌ | G-01、G-02、G-04 |
| 延迟渲染 G-Buffer | ⚠️ 勉强 | G-01 |
| 点光源阴影 / CSM / 反射探针 / IBL | ❌ | G-03、G-04 |
| SSAO / SSR / TAA / 运动模糊 | ❌ | G-01、G-03、G-05 |
| 软粒子、贴花（边测深度边采样深度） | ❌ | G-06 |
| GPU 性能分析、RenderDoc 分组、遮挡剔除原语 | ✅ 可用（D3D11、GL4） | — |
| 显存 / 带宽效率 | ⚠️ | G-02、G-13 |

---

## 1. 状态图例

| 标记 | 含义 |
|---|---|
| ✅ 已完成 | RHI 与后端已实现，剩余工作在「实现」行注明 |
| ❌ 缺失 | 接口或枚举层完全没有 |
| ⚠️ 受限 | 有实现，但只覆盖最小形态 |
| 🔧 后端 | 只需改 D3D11 后端，不动 RHI 接口 |

---

## 2. P0：决定能否做现代画质

### G-01 像素格式缺浮点、单 / 双通道与整数格式

| 项目 | 内容 |
|---|---|
| **状态** | ✅ 已完成（`ImageConverter` / FreeImage 浮点导入除外） |
| **实现** | 1) `PixelFormat` 按格式族重排并补齐上述 15 种格式，末尾加 `E_PF_MAX` 哨兵。序列化按枚举名写入，重排不影响已有资源；<br/>2) 新增 `PixelFormatUtil`（`Kernel/T3DPixelFormat.h`）集中维护字节数、通道数、类型、是否深度 / 模板，`Image::getBPP`、各后端的深度判断与字节数表都改为调用它；<br/>3) `RHICapabilities::formatSupport[]` 按格式记录 `kFormatSupport*` 位（纹理、线性过滤、RT、混合、MSAA、深度模板、UAV、类型化缓冲），提供 `supportsFormat()`，并由 `checkRenderTextureFormat()` 在各后端 `createRenderTexture` 入口统一校验。各后端启动时把整张表打到日志里；<br/>4) 填表来源：D3D11 用 `CheckFormatSupport`（深度格式查 typeless / SRV 格式）；GL4.3+ 用 `glGetInternalformativ`，低版本用静态表；GLES3 按 `EXT_color_buffer_float`、`EXT_color_buffer_half_float`、`OES_texture_float_linear`、`EXT_float_blend`、`EXT_texture_norm16` 查表，MSAA 用 ES3.1 的 `GL_NUM_SAMPLE_COUNTS`；Vulkan 用 `vkGetPhysicalDeviceFormatProperties`；Metal 没有查询接口，按 Feature Set Tables 保守填表；<br/>5) GL4 / GLES3 的整数格式纹理创建时强制 `GL_NEAREST`，否则纹理不完整；<br/>6) 整数格式在所有后端都不报告 MSAA（D3D11 不能 Resolve 整数格式，保持各后端一致）。<br/>**待做**：`ImageConverter` 与 FreeImage 解码路径仍只输出 8 位格式，`.hdr` / `.exr` 导入随 G-02 的 `imgconv` 扩展一起做；`R8G8B8` / `B8G8R8` 在 D3D11 / Vulkan 上映射到 4 字节格式，而 CPU 侧按 3 字节算，这是原有问题，未在本条处理 |
| **现状** | `PixelFormat`（`T3DConstant.h:38`）只有 `PALETTE8`、5 种 16 位、6 种 8 位 RGBA / BGRA 变体与 4 种深度格式。`D3D11Mapping::get(PixelFormat)`（`T3DD3D11Mapping.cpp:49-89`）也只映射这些 |
| **缺少** | 浮点：`R16G16B16A16_FLOAT`、`R11G11B10_FLOAT`、`R32_FLOAT`、`R16_FLOAT`、`R16G16_FLOAT`、`R32G32_FLOAT`、`R32G32B32A32_FLOAT`<br/>单 / 双通道：`R8_UNORM`、`R8G8_UNORM`、`R16_UNORM`、`R16G16_UNORM`<br/>10 位：`R10G10B10A2_UNORM`<br/>整数：`R32_UINT`、`R16G16_UINT`、`R8G8B8A8_UINT` |
| **影响** | HDR 场景缓冲、G-Buffer 的法线 / 粗糙度通道、VSM / ESM 阴影、SSAO、速度缓冲、ID 缓冲（编辑器拾取）、浮点 LUT 都无法创建。Compute 的 `kTyped` 结构化缓冲也受同一枚举限制 |
| **要做** | 1) 扩 `PixelFormat`，按「格式族」分组排列，避免继续用 `format >= E_PF_D24_UNORM_S8_UINT && format <= E_PF_D16_UNORM` 这种区间判断识别深度格式（`T3DD3D11Context.cpp:755`），改为 `isDepthFormat()` 之类的辅助函数；<br/>2) 补全 D3D11 / GL4 / GLES3 / Vulkan / Metal 的映射表与每像素字节数表（`buildSubresourceData`、读回的 `BytesPerPixel` 都依赖它）；<br/>3) `RHICapabilities` 增加按格式查询的能力（是否可作 RT、是否可混合、是否可作 UAV、是否支持线性过滤），D3D11 用 `CheckFormatSupport` 填充。GLES3 上浮点 RT 需要 `EXT_color_buffer_float`，必须可查询 |
| **涉及** | RHI、引擎枚举与序列化、全部后端、`ImageConverter`、FreeImage 解码路径 |

### G-02 sRGB 与块压缩（BC）格式

| 项目 | 内容 |
|---|---|
| **状态** | ✅ 已完成（sRGB 后缓冲随 G-09 处理） |
| **实现** | 1) `PixelFormat` 新增 `R8G8B8A8_SRGB`、`B8G8R8A8_SRGB` 与 `BC1`～`BC7` 族（`BC1/2/3/7` 带 `_SRGB`，`BC4/5` 带 `_SNORM`，`BC6H_UF16/SF16`）。`PixelFormatUtil` 增加 `isSRGB`、`isCompressed`、`getBlockSize`、`getBytesPerBlock`、`toLinear` / `toSRGB`，以及按块计算的 `getRowPitch`、`getRowCount`、`getSliceSize`、`getMipChainSize`。压缩格式的 bpp 为 0，凡是按像素算字节的地方都要改走这组函数；<br/>2) `RHICapabilities::checkCompressedTextureFormat()` 在各后端创建纹理的入口统一校验压缩格式：只允许 2D / Cube、mip 0 宽高是 4 的倍数、不能 MSAA、不能 UAV、能力表要有 Texture 位；<br/>3) D3D11：`D3D11Mapping::getResourceFormat()` 在 sRGB 纹理需要 UAV 时把资源建成 typeless，SRV / RTV 仍用 sRGB 视图，`getUAVFormat()` 给出线性视图格式；`buildSubresourceData` 与读回按块行距计算，读回逐行拷贝；<br/>4) GL4：sRGB 映射到 `GL_SRGB8_ALPHA8`，BC 映射到 S3TC（`EXT_texture_compression_s3tc`，sRGB 变体还要 `EXT_texture_sRGB`）、RGTC（核心）、BPTC（4.2+ 或 `ARB_texture_compression_bptc`）。压缩纹理用 `glTexStorage*` + `glCompressedTexSubImage*` 上传，读回用 `glGetCompressedTextureSubImage`。D3D11 由 RTV 格式决定是否做 sRGB 编码，GL 由 `GL_FRAMEBUFFER_SRGB` 开关，所以每次切换绘制 FBO 后同步：绑定离屏 FBO 时打开，绑定默认帧缓冲时关闭（后缓冲仍是 UNORM，保持和 D3D11 一致）。sRGB 格式不报告 UAV（GL 的 image 格式没有 sRGB）。压缩纹理不能挂到 FBO，所以 blit 只保留同格式的 `glCopyImageSubData` 路径；<br/>5) GLES3：sRGB 为核心格式，不报告 UAV；BC 暂不报告（桌面 ES 驱动才有 S3TC，移动端走 GAP-10 的 ETC2 / ASTC）；<br/>6) Vulkan：映射到 `VK_FORMAT_*_SRGB` 与 `VK_FORMAT_BC*_BLOCK`，2D / Cube 上传按 `getSliceSize` 计算每级大小；<br/>7) Metal：映射到 `*Unorm_sRGB`，BC 只在 macOS 上可用（按 `supportsBCTextureCompression` 填表），`bytesPerRow` 按块行计算；<br/>8) `imgconv` 新增 `--srgb` / `--no-srgb` 与 `--compression none\|bc1\|bc3\|bc4\|bc5\|bc7`，对应 `.meta` 的 `sRGB` / `Compression`。内置编码器 `T3DBlockCompressor` 不依赖第三方库：BC1 / BC3 / BC4 / BC5 用 PCA 端点加最小二乘迭代，BC7 只用 mode 6。输出 sRGB 时 mip 在线性空间降采样。`Compression` 为空时保持原来的 `B8G8R8A8` UNORM 输出并忽略 `sRGB`，已有资源不受影响；BC4 / BC5 总是线性。<br/>**验证**：整个解决方案 Debug 编译通过；`imgconv` 把 `blocks.png` 转成 none / BC1 / BC3 / BC4 / BC5 / BC7 以及 sRGB 变体，在 TextureApp 里替换后分别用 D3D11 与 GL4 截图：BC1 / BC3 / BC7 与原图一致、没有块错位，BC4 只剩红色、BC5 只剩红绿（说明确实走了块路径），sRGB 变体在两个后端上一致地变暗（后缓冲还是 UNORM，符合预期）。PostProcessingApp、HDRApp、ComputeApp 在 D3D11 与 GL4 上对比无回归。新增独立的 `SRGBApp`（见 7.3）做 sRGB 读回自检：D3D11 8 项全过，GL4 7 项通过、1 项（sRGB UAV）按能力表跳过。<br/>**待做**：sRGB 后缓冲（G-09）；GL4 / GLES3 / Metal 的 sRGB UAV（需要 texture view 或另建线性别名）；BC6H 编码（需要浮点源，随 `.hdr` / `.exr` 导入一起做）；GLES3 的 BC；GL4 压缩纹理的缩放 / 跨格式 blit；Vulkan 后端本身没有 UAV、1D / 3D 以及 Cube 的 mip 上传路径，压缩格式在这些路径上同样不可用；Metal 与 GLES3 的改动未在本机编译 |
| **现状** | `PixelFormat` 没有 sRGB 与 BC 格式。`T3DD3D11Mapping.cpp:178-196` 已经有处理 `*_UNORM_SRGB` 的代码，只是引擎侧到不了这些格式 |
| **缺少** | `R8G8B8A8_UNORM_SRGB`、`B8G8R8A8_UNORM_SRGB`、`BC1`～`BC7`（及其 sRGB 变体）。移动端对应的 ETC2 / ASTC 在 `Tiny3D-Capability-Status-todo.md` GAP-10 里已登记，建议与本条一起设计 |
| **影响** | 1) 没有 sRGB 纹理与 sRGB 后缓冲，光照只能在伽马空间里算，或者在 shader 里手写转换，PBR 结果不正确；<br/>2) 没有 BC 压缩，纹理显存与带宽是压缩格式的 4～8 倍 |
| **要做** | 1) 扩枚举；<br/>2) 后端区分「资源格式」与「视图格式」：sRGB 纹理在创建 UAV 时必须用 typeless 或线性视图（`buildTextureUAView` 的注释里已写明「不能是 typeless / sRGB」）；<br/>3) `buildSubresourceData` 和读回的行距计算改为按块计算（BC 是 4×4 块，不能用 `width * bpp`）；<br/>4) 后缓冲 sRGB 与 G-09 的 Flip 交换链一起处理：Flip 模型下交换链格式不能是 `_SRGB`，要用 RTV 的 sRGB 视图 |
| **涉及** | RHI、全部后端、`ImageConverter`（离线压缩）、`.ttex` 资源格式 |

### G-03 渲染目标只能是单张 2D、单 mip

| 项目 | 内容 |
|---|---|
| **状态** | ✅ 已完成（D3D11、GL4；Vulkan 部分；GLES3 / Metal 显式拒绝） |
| **实现** | 1) 引擎侧：`T3DRenderConstant.h` 新增 `TextureSubresource {mipLevel, mipCount, firstSlice, sliceCount}`（计数为 0 表示「到末尾」），提供 `mip()`、`slice()`、`resolve()`、`key()` 等辅助函数，和 `TextureSubresources` 数组。`TextureManager::createRenderTexture(name, dimension, w, h, arraySize, format, mipmaps, ...)` 接受 `TT_2D` / `TT_2D_ARRAY` / `TT_CUBE` / `TT_CUBE_ARRAY`，`mipmaps = 0` 表示完整 mip 链；`RenderTexture` 记录维度与层数（`getRenderTextureType`、`getArraySize`）；<br/>2) `RenderTarget::create(color, colorSub, depth, depthSub)` 与 `setColorSubresource` / `setDepthStencilSubresource` 让附件选中某一级 mip 和层范围。选中多层就是分层渲染，要由 GS 输出 `SV_RenderTargetArrayIndex`，并且后端要有 `supportsLayeredRendering`。`getAttachmentSize` 返回所选 mip 的尺寸，后端的视口、清屏和 framebuffer 尺寸都按它计算；<br/>3) `RHICapabilities::checkRenderTextureLayout` 在各后端 `createRenderTexture` 入口统一校验：Cube 宽高相等、层数是 6 的倍数，MSAA 只能配单层、单 mip 的 2D（Cube 与 MSAA 互斥），mip 数不超过完整链，带 `kGPUGenerateMips` 时不能是深度格式、格式要可作 RT 且可线性过滤；<br/>4) 视图维度规则在各后端一致：资源只有一层时用 2D 视图；选中全部层时用资源本身的维度（Cube 仍是 Cube）；选中部分层时用 2D 数组；UAV 永远不用 Cube 维度；<br/>5) D3D11：`D3D11PixelBuffer2D` 的 SRV / UAV / RTV / DSV 各有一张 `TMap<uint64_t, View*>`，按 `TextureSubresource::key()` 懒创建、随资源释放。Cube 带 `MISC_TEXTURECUBE`，默认 SRV 覆盖全部 mip（以前写死 `MipLevels = 1`）。`setRenderTarget` 按附件子资源取 RTV / DSV，`setViewport` 按所选 mip 计算像素尺寸；<br/>6) GL4：数组 / Cube / 多 mip 的渲染纹理用 `glTexStorage2D/3D` 建不可变存储（`glTextureView` 只认不可变存储）。FBO 单层用 `glFramebufferTextureLayer` 挂，Cube 单面用面 target 挂（GL 4.5 之前 `glFramebufferTextureLayer` 不接受 Cube），全部层用 `glFramebufferTexture` 做分层挂载；GL 不能挂「部分多层」，这种选择退化为全部层并打 warning。每次 `setRenderTarget` 按附件子资源重新挂载；<br/>7) Vulkan：image 按 mip 与层数创建，Cube 带 `CUBE_COMPATIBLE`；采样视图覆盖全部 mip，附件视图和子资源视图按 key 缓存（第 63 位区分附件视图），framebuffer 的哈希带上子资源，尺寸按 `getAttachmentSize` 计算；<br/>8) GLES3、Metal：`createRenderTexture` 遇到 `arraySize > 1`、Cube 或 `mipmaps > 1` 时打 error 并失败。<br/>**验证**：整个解决方案 Debug 编译通过；新建 `PointLightShadowApp`（见 7.3），六个 Cube 面各用一个 `RenderTarget` 选中对应层，颜色 Cube 和深度 Cube 同时按面挂载，D3D11 与 GL4 上阴影方向正确，日志无 error / warning。ShadowApp、ComputeApp（35 项全过）在 D3D11 与 GL4 上无回归。<br/>**待做**：Vulkan 按整张 image 追踪 layout，同一张 image 一级读、另一级写的情况没有处理，也不支持分层渲染；GLES3 与 Metal 的数组 / Cube / 多 mip 渲染纹理；还没有 Sample 用到分层渲染（GS 一次画六个面）和 `TT_2D_ARRAY`，留给 `CascadedShadowApp` |
| **现状** | `RHIContext::createRenderTexture(PixelBuffer2D *)`（`T3DRHIContext.h:115`）只接受 2D。`buildRenderTextureResources`（`T3DD3D11Context.cpp:698`）只建一个 `TEXTURE2D` / `TEXTURE2DMS` 的 RTV / DSV，SRV 写死 `MipLevels = 1`（:843），RenderTarget 的附件也只能引用整张纹理 |
| **缺少** | 1) 以 Cube 的单个面、Texture2DArray 的单层或某个 mip 级别作为 RTV / DSV；<br/>2) 一次绑定整个数组，配合 GS / `SV_RenderTargetArrayIndex` 做单 Pass 分层渲染；<br/>3) 带完整 mip 链的渲染纹理 |
| **影响** | 点光源阴影（Cube 深度）、CSM（深度数组）、反射探针、IBL 辐照度 / 预过滤 Cubemap、Bloom 降采样链、Hi-Z、SSR 的 mip 链都做不了 |
| **要做** | 1) 渲染纹理描述支持 `TT_2D_ARRAY` / `TT_CUBE` / `TT_CUBE_ARRAY` 与 `mipmaps > 1`；<br/>2) RenderTarget 附件改为「纹理 + 子资源选择（mip、首层、层数）」，后端按需缓存多个 RTV / DSV；<br/>3) 视口尺寸按所选 mip 级别计算（`setViewport` 现在按纹理顶层宽高算）；<br/>4) MSAA 与 Cube 互斥（`D3D11_RESOURCE_MISC_TEXTURECUBE` 不能配 MSAA），创建时显式拒绝 |
| **涉及** | RHI、`RenderTexture` / `RenderTarget`、全部后端、`ForwardRenderPipeline` 的阴影与后处理 |

### G-04 没有 mip 生成

| 项目 | 内容 |
|---|---|
| **状态** | ✅ 已完成（D3D11、GL4、Vulkan；GLES3 / Metal 返回不支持） |
| **实现** | 1) 新增 GPU 访问位 `kGPUGenerateMips`（`T3DRenderConstant.h`），渲染纹理创建时带上它才允许生成 mip；入口 `RHIContext::generateMips(PixelBuffer *)`，引擎侧包装为 `RenderTexture::generateMips()`；<br/>2) `RHICapabilities::checkGenerateMips` 统一校验：必须是已创建 RHI 资源的渲染纹理、带 `kGPUGenerateMips`、`mipmaps > 1`；创建时的格式要求见 G-03 的 `checkRenderTextureLayout`；<br/>3) D3D11：带 `kGPUGenerateMips` 时资源加 `BIND_RENDER_TARGET | BIND_SHADER_RESOURCE` 与 `MISC_GENERATE_MIPS`，`generateMips` 对默认 SRV（覆盖全部 mip 和层）调 `GenerateMips`；<br/>4) GL4：`glGenerateTextureMipmap`，MSAA 纹理直接拒绝；<br/>5) Vulkan：整张 image 先转到 `TRANSFER_DST`，逐级把上一级转成 `TRANSFER_SRC` 后 `vkCmdBlitImage`（LINEAR），再转到 `SHADER_READ_ONLY`。有当前帧时录进帧命令缓冲，否则用一次性命令缓冲；render pass 进行中调用会报错；<br/>6) GLES3、Metal：打 error，返回不支持（这两个后端也建不出多 mip 渲染纹理）；Console 等无窗口上下文是空实现，返回 `T3D_OK`。<br/>**验证**：`PointLightShadowApp` 每帧画完六个面后对 512² 的 `R32G32_FLOAT` 距离 Cube（10 级 mip）调用 `generateMips`，主 Pass 按 mip 1.5 采样做 VSM 软阴影；D3D11 与 GL4 上阴影边缘柔和、随光源移动每帧更新，日志无 error / warning。<br/>**待做**：Vulkan 的 `generateMips` 只编译通过、未运行验证（`PointLightShadowApp` 只带 HLSL / GLSL 源码，在 Vulkan 上打 error 后不画阴影）；非渲染纹理（运行时上传的普通 2D 纹理）还不能生成 mip；`ReflectionProbeApp` 的粗糙度 mip 链还没做 |
| **现状** | D3D11 后端没有调用 `GenerateMips`，也没有设置 `D3D11_RESOURCE_MISC_GENERATE_MIPS`。运行时纹理的 mip 只能靠离线工具预先生成 |
| **影响** | 渲染纹理、运行时生成的纹理（程序化纹理、RmlUi 动态纹理、反射探针）没有 mip，远处采样出现闪烁和锯齿，IBL 预过滤也无从谈起 |
| **要做** | 增加 `RHIContext::generateMips(PixelBuffer *)`。D3D11 要求资源同时带 `BIND_RENDER_TARGET | BIND_SHADER_RESOURCE` 与 `MISC_GENERATE_MIPS`，所以需要在纹理描述里加「需要自动 mip」标记，创建时打上这些标志；GL 用 `glGenerateMipmap`；Vulkan 用逐级 `vkCmdBlitImage` 或 Compute。依赖 G-03 的多 mip 渲染纹理 |
| **涉及** | RHI、全部后端 |

---

## 3. P1：决定渲染技术覆盖面与调试效率

### G-05 纹理 UAV 只有 mip 0

| 项目 | 内容 |
|---|---|
| **状态** | ✅ 已完成（D3D11、GL4.3+；Vulkan 只有 PS 的 SRV 子资源） |
| **实现** | 1) `setCSUnorderedAccessBuffers`、`setVS/HS/DS/GS/PS/CSPixelBuffers` 末尾增加可选参数 `const TextureSubresources &subresources`，长度为 0 或与 buffers 等长；为空或某项为默认值时沿用整张纹理的视图，不影响已有调用。只有 `RHICapabilities::supportsTextureSubresourceViews` 为 true 的后端接受非默认值，其它后端遇到非默认值打 error 并返回 `T3D_ERR_INVALID_PARAM`。选中部分层的数组 / Cube 在 shader 里要声明为 `Texture2DArray`；<br/>2) UAV 只能选一级 mip（`mipCount` 大于 1 时报错），层范围可选；SRV 可以选 mip 范围和层范围。视图维度规则同 G-03；<br/>3) D3D11：SRV / UAV 视图按 `TextureSubresource::key()` 缓存在 `D3D11PixelBuffer2D` 上（与 G-03 的 RTV / DSV 共用一套）。Cube 选中全部层时 SRV 仍是 `TEXTURECUBE`，UAV 用 `TEXTURE2DARRAY`；<br/>4) GL4：SRV 的子资源用 `glTextureView` 建视图纹理（需要 4.3，所以 `supportsTextureSubresourceViews = has43`），按 key 缓存在 `GL4PixelBuffer2D` / `GL4PixelBufferCubemap` 的 `GLViews` 上；UAV 用 `glBindImageTexture(unit, tex, level, layered, layer, ...)` 直接选 mip 和层。精确模式的 `uavBarrier` 匹配资源时把视图纹理也算作同一资源；<br/>5) Vulkan：PS 阶段的 SRV 子资源走 `getVkSubresourceView`；Vulkan 后端本身没有 compute 和 UAV，其余阶段不涉及；<br/>6) GLES3、Metal：`supportsTextureSubresourceViews = false`，只接受默认值。<br/>**验证**：HDRApp 的 Bloom 改为 Compute（`HDRBloomTonemapBehaviour`，kernel 源码见 `assets/samples/shaders/HDRBloomDownsample.cshader` / `HDRBloomUpsample.cshader`）：降采样链和升采样链各是一张 6 级 mip 的 `R16G16B16A16_FLOAT` 渲染纹理，降采样每次 SRV 选 mip N-1、UAV 写 mip N，升采样 SRV 选两级、UAV 写一级。D3D11 与 GL4 上日志为「6 bloom mips ... run as compute」，光晕与原来像素着色器版本一致，无新增 error / warning。后端不支持 compute、UAV、子资源视图或该格式不能作 UAV 时，自动退回像素着色器链并打日志。<br/>**待做**：Vulkan 的 compute / UAV（属于 Vulkan 后端的整体缺口）；GLES3 3.1 的 `glBindImageTexture` 也能选 mip / 层，可以单独打开 UAV 子资源；GL 4.3 以下没有 SRV 子资源 |
| **现状** | `buildTextureUAView`（`T3DD3D11Context.cpp:2550`）只为 mip 0 建 UAV，`setCSUnorderedAccessBuffers` 的接口注释也写明了这一点。GL4 / GLES3 同样只绑 mip 0 |
| **影响** | 用 Compute 逐级生成 mip、Hi-Z 金字塔、Bloom 降采样、体素化都做不了 |
| **要做** | UAV / SRV 绑定接口增加子资源参数（mip、首层、层数），后端按 `(mip, slice)` 缓存视图。和 G-03 共用同一套「子资源视图」设计 |
| **涉及** | RHI、全部后端 |

### G-06 没有只读深度视图

| 项目 | 内容 |
|---|---|
| **状态** | ❌ 缺失 |
| **现状** | 深度纹理已经用 typeless 格式（`R24G8_TYPELESS` / `R32_TYPELESS`，`T3DD3D11Context.cpp:892-900`）创建，可以被 shader 采样，但 DSV 没有 `D3D11_DSV_READ_ONLY_DEPTH` / `READ_ONLY_STENCIL` 版本 |
| **影响** | 同一张深度不能同时用于深度测试和 shader 采样，软粒子、贴花、体积雾必须先拷贝一份深度 |
| **要做** | 深度渲染纹理额外建一个只读 DSV，RenderTarget 绑定时可选「只读深度」，此时允许同一资源作为 SRV 绑定。GL / Vulkan 对应的是关闭深度写入 + `DEPTH_STENCIL_READ_ONLY_OPTIMAL` 布局 |
| **涉及** | RHI、全部后端 |

### G-07 没有 GPU 查询

| 项目 | 内容 |
|---|---|
| **状态** | ✅ 已完成（D3D11、GL4 运行验证；Vulkan、GLES3 部分类型；Metal 未实现） |
| **实现** | 1) 新资源 `RHIQuery`（`Core/Include/RHI/T3DRHIQuery.h`），类型 `kOcclusion` / `kBinaryOcclusion` / `kTimestamp` / `kPipelineStatistics`；接口 `createQuery` / `beginQuery` / `endQuery` / `resolveQuery` / `setPredication`。`kTimestamp` 只调 `endQuery`，结果统一换算成纳秒，disjoint 由后端内部处理（期间的结果返回 `kDisjoint`）；`kPipelineStatistics` 返回 `PipelineStatistics` 结构（IA 顶点 / 图元、各着色器调用次数、裁剪前后图元）；<br/>2) 读结果非阻塞：主线程 `endQuery` 时记一个序号，后端在 GPU 结果就绪后 `publish(序号, 结果)`，`resolveQuery` 只看最近一次发起的序号，返回 `kPending` / `kReady` / `kDisjoint`，从不等待。同一个查询每帧重发会一直取不到结果，所以调用方要按帧轮换（`GPUProfiler` 用 5 组，`OcclusionCullingApp` 用 4 组）；<br/>3) 能力进 `RHICapabilities`：`supportsOcclusionQuery` / `supportsBinaryOcclusionQuery` / `supportsTimestampQuery` / `supportsPipelineStatisticsQuery` / `supportsPredication`，`supportsQuery(type)` 汇总，`createQuery` 不支持时打 warning 返回空；<br/>4) 引擎侧 `GPUProfiler`（`Render/T3DGPUProfiler.h`）：按作用域嵌套记录 timestamp，延迟几帧取回后给出每段耗时与整帧 GPU 时间，`RenderPipeline::enableGPUProfiling(true)` 打开，`ForwardRenderPipeline` 的各个分组（见 G-08）同时是 profiler 作用域；<br/>5) D3D11：`ID3D11Query` / `ID3D11Predicate`，每帧第一个 timestamp 开启一个 `TIMESTAMP_DISJOINT`，帧末结束，当帧的 timestamp 共用它换算；`GetData` 带 `DONOTFLUSH` 每帧轮询；`SetPredication(pred, FALSE)`；<br/>6) GL4：`glGenQueries`，`GL_SAMPLES_PASSED` / `GL_ANY_SAMPLES_PASSED`，timestamp 用 `glQueryCounter(GL_TIMESTAMP)`，pipeline statistics 需要 `ARB_pipeline_statistics_query`（每个统计项一个 GL 查询）；用 `GL_QUERY_RESULT_AVAILABLE` 轮询；条件渲染用 `glBeginConditionalRender(GL_QUERY_WAIT)`（GPU 上等，CPU 不阻塞。`NO_WAIT` 在结果未就绪时照常绘制，紧挨着的查询几乎总是未就绪，等于不剔除）；<br/>7) Vulkan：每个查询一个 `VkQueryPool`，每帧在飞一个槽（`MAX_FRAMES_IN_FLIGHT`）；帧 fence 之后 `vkGetQueryPoolResults` 取结果，下一次复用前在命令缓冲开头 `vkCmdResetQueryPool`。begin / end 必须同在或同不在 render pass 内，同一查询每帧只能发一次。timestamp 依赖队列的 `timestampValidBits` 与 `timestampPeriod`，pipeline statistics 依赖设备特性。没有条件渲染（`supportsPredication = false`）；<br/>8) GLES3：只有 `kBinaryOcclusion`（`GL_ANY_SAMPLES_PASSED`）；timestamp 依赖 `EXT_disjoint_timer_query`，每次轮询读 `GL_GPU_DISJOINT_EXT`，出现 disjoint 时把未完成的 timestamp 都标为 `kDisjoint`；<br/>9) Metal：全部返回不支持。<br/>**验证**：`OcclusionCullingApp` 初版（见 §7.3）。D3D11 与 GL4 上 5 项能力全为 1；1180 个物件逐个「画包围盒代理 + 遮挡查询 → 条件渲染画本体」，每帧可见约 50～80 个，pending 0；pipeline statistics 的 IA 顶点约 4.6～4.8 万（1180 个代理 + 可见物件，全部绘制是 12.7 万），说明条件渲染确实跳过了绘制；`GPUProfiler` 各段耗时稳定，顶层分组之和与整帧 GPU 时间吻合（误差 < 0.01 ms），无丢帧，日志无 error / warning。<br/>**待做**：Metal 的查询（`MTLCounterSampleBuffer` / visibility result buffer）；Vulkan 条件渲染需要 `VK_EXT_conditional_rendering`；Vulkan、GLES3、Metal 只编译、未运行验证（GLES3、Metal 本机未编译）；GL 上逐物件同帧「测完就画」要在 GPU 上串行等待 1180 次，City Objects 段从 6.4 ms 涨到约 11 ms，真实引擎应改用上一帧的查询结果，Sample 后续调整；`OcclusionCullingApp` 的 RmlUi 性能面板未做 |
| **现状** | 后端没有任何 `ID3D11Query` |
| **缺少** | Timestamp + Disjoint（GPU 计时）、Occlusion / Occlusion Predicate（遮挡查询）、Pipeline Statistics、`SetPredication`（条件渲染） |
| **影响** | 无法按 Pass 统计 GPU 耗时，编辑器做不了 Profiler 面板；遮挡剔除（`Tiny3D-Capability-Status-todo.md` GAP-10）缺底层原语 |
| **要做** | 增加 `RHIQuery` 资源与 `beginQuery` / `endQuery` / `resolveQuery`。结果读取必须是非阻塞的（延迟 N 帧取），接口语义参照现有 `map` / `unmap` 的票据模型，但不能同步等待 |
| **涉及** | RHI、全部后端（GLES3 的 timestamp 依赖 `EXT_disjoint_timer_query`，需进 `RHICapabilities`） |

### G-08 没有调试标注

| 项目 | 内容 |
|---|---|
| **状态** | ✅ 已完成（全部后端已实现；D3D11、GL4 运行验证，RenderDoc 截图待补） |
| **实现** | 1) `RHIContext::beginDebugGroup(name)` / `endDebugGroup()` / `setDebugMarker(name)` / `setDebugName(RenderResource *)`，默认空实现返回 `T3D_OK`；能力位 `RHICapabilities::supportsDebugAnnotations`；<br/>2) 上层用宏 `T3D_RHI_DEBUG_GROUP`（作用域，`RHIDebugGroupScope`）/ `T3D_RHI_DEBUG_GROUP_BEGIN` / `_END` / `T3D_RHI_DEBUG_MARKER`。`T3D_ENABLE_RHI_DEBUG_ANNOTATION`（`T3DConfig.h`，默认 1）定义为 0 时宏为空，`RenderResource::setDebugName` 只记录名字不转发，Release 可在编译选项里关掉；<br/>3) `ForwardRenderPipeline` 分组：`Camera <名字>` 下依次是 `Shadow`、`PreRender Behaviours`、`Opaque`、`Skybox`、`Transparent`、`PostRender Behaviours`、`UI`、`PostProcess`、`Blit`、`UI Overlay`；连续的不透明 / 透明队列各合并成一个组。每个分组同时是 `GPUProfiler` 的计时作用域（见 G-07）；<br/>4) 资源调试名：`RenderResource::setDebugName(name)` 转发给后端；`Texture` / `RenderTexture` 的像素缓冲用资源名，`Mesh` / `SubMesh` 的顶点 / 索引缓冲用「网格名 VB0 / IB」，`RenderResourceManager` 建的常量缓冲用 cbuffer 名，`ForwardRenderPipeline` 的动态合批缓冲与全屏三角形也有名字；<br/>5) D3D11：`ID3DUserDefinedAnnotation` 的 `BeginEvent` / `EndEvent` / `SetMarker`，调试名用 `SetPrivateData(WKPDID_D3DDebugObjectName)`（同时设到 SRV / RTV / DSV / UAV 视图上）；<br/>6) GL4 / GLES3：`KHR_debug` 的 `glPushDebugGroup` / `glPopDebugGroup` / `glDebugMessageInsert`，调试名用 `glObjectLabel`；GLES3 运行时用 `eglGetProcAddress` 先取核心名（ES 3.2）、再取 `KHR` 后缀，都取不到时 `supportsDebugAnnotations = false`；<br/>7) Vulkan：`VK_EXT_debug_utils`（实例扩展可用时才开启），`vkCmdBeginDebugUtilsLabelEXT` / `End` / `Insert`，调试名用 `vkSetDebugUtilsObjectNameEXT`；不在帧命令缓冲录制期间的分组 / 标记直接忽略；<br/>8) Metal：`pushDebugGroup` / `popDebugGroup` / `insertDebugSignpost`，调试名写 `label`。<br/>**验证**：D3D11 与 GL4 上 `supportsDebugAnnotations = 1`；`OcclusionCullingApp` 的 `GPUProfiler` 输出与分组一一对应（`Camera` → `Shadow` / `PostRender Behaviours` → `City Ground` / `City Objects` …），说明分组的开闭配对正确；运行无 error / warning。<br/>**待做**：用 RenderDoc 抓帧截图作为验证记录（本机未装 RenderDoc）；Vulkan、GLES3、Metal 未运行验证（GLES3、Metal 本机未编译）；着色器、采样器和渲染状态对象还没有调试名 |
| **现状** | 没有 `ID3DUserDefinedAnnotation`，也没有给资源设置调试名称（`SetPrivateData(WKPDID_D3DDebugObjectName)`） |
| **影响** | RenderDoc / PIX 里看到的是一长串不分组的 draw call，资源也全是匿名的，排查渲染问题成本很高 |
| **要做** | 1) `RHIContext::beginDebugGroup(name)` / `endDebugGroup()` / `setDebugMarker(name)`，`ForwardRenderPipeline` 在阴影、不透明、天空盒、透明、后处理各阶段打组；<br/>2) 资源创建时把引擎侧名字传给后端设为调试名。GL 用 `KHR_debug`，Vulkan 用 `VK_EXT_debug_utils`。Release 构建可编译期关闭 |
| **涉及** | RHI、全部后端、`ForwardRenderPipeline` |

### G-09 交换链使用老的 Blt 模型

| 项目 | 内容 |
|---|---|
| **状态** | 🔧 后端 |
| **现状** | `createRenderWindow` 使用 `DXGI_SWAP_EFFECT_DISCARD` + `BufferCount = 1`（`T3DD3D11Context.cpp:540-543`），`BufferUsage` 只有 `RENDER_TARGET_OUTPUT`（:539） |
| **影响** | 1) 比 Flip 模型多一次合成拷贝，延迟更高；Win10+ 推荐用 `FLIP_DISCARD`，HDR 输出与可变刷新率（`ALLOW_TEARING`）也只在 Flip 模型下可用；<br/>2) 后缓冲不能作 SRV，`blit` 以窗口为源时只能直接拷贝（`T3DD3D11Context.cpp:5653` 的注释） |
| **要做** | 改为 `DXGI_SWAP_EFFECT_FLIP_DISCARD` + `BufferCount >= 2`，Present 支持 `DXGI_PRESENT_ALLOW_TEARING`。**注意**：Flip 模型不允许 MSAA 后缓冲，也不允许 `_SRGB` 交换链格式。当前窗口自带 MSAA 的路径要改成「MSAA 离屏 RT → Resolve 到后缓冲」，sRGB 改用 RTV 视图格式（见 G-02）。`resizeRenderWindow` 的 `ResizeBuffers` 参数要同步调整 |
| **涉及** | D3D11 后端；窗口 MSAA 的语义变化会影响 `RenderWindowDesc` 的使用方 |

---

## 4. P2：零散补齐

### G-10 混合常量因子与双源混合

| 项目 | 内容 |
|---|---|
| **状态** | ❌ 缺失 |
| **现状** | `BlendFactor`（`T3DRenderConstant.h:95`）只有 10 个基础因子，没有 `kBlendFactor` / `kOneMinusBlendFactor`、`kSrcAlphaSaturate`、`kSrc1Color` / `kSrc1Alpha`（双源混合）。D3D11 的 `setBlendState` 把 blend factor 写死为 `nullptr`、sample mask 写死为 `-1`（`T3DD3D11Context.cpp:1925`） |
| **影响** | 无法用常量系数做淡入淡出与过渡混合；双源混合常用于次表面散射近似与某些 OIT 方案 |
| **要做** | 扩 `BlendFactor`；`BlendDesc` 增加 `BlendConstant` 与 `SampleMask`，或者 `RHIContext` 增加 `setBlendConstant`。后者更接近 Vulkan 的动态状态，推荐后者 |
| **涉及** | 引擎枚举、全部后端、`ShaderCrossCompiler` 的 shaderlab `Blend` 指令解析 |

### G-11 PS 阶段不能绑 UAV

| 项目 | 内容 |
|---|---|
| **状态** | ❌ 缺失 |
| **现状** | UAV 只能绑到 CS（`setCSUnorderedAccessBuffers`），没有 `OMSetRenderTargetsAndUnorderedAccessViews` |
| **影响** | 逐像素链表 OIT、PS 中写 Append 缓冲收集可见性、GPU 拾取等技术做不了 |
| **要做** | 增加 `setPSUnorderedAccessBuffers`。D3D11 的 PS UAV 与 RTV 共享槽位编号（UAV 起始槽不能小于 RTV 数量），需要在 `setRenderTarget` 时合并提交；`uavBarrier` 的语义也要覆盖 PS 槽 |
| **涉及** | RHI、全部后端 |

### G-12 没有 UAV 清除

| 项目 | 内容 |
|---|---|
| **状态** | ❌ 缺失 |
| **现状** | 没有 `ClearUnorderedAccessViewUint` / `ClearUnorderedAccessViewFloat` |
| **影响** | 每帧清空计数器缓冲、直方图、体素网格要么写一个 CS，要么从 CPU 重新上传 |
| **要做** | 增加 `clearUnorderedAccess(buffer, uint4 / float4)`。GL 用 `glClearBufferData` / `glClearTexImage`，Vulkan 用 `vkCmdFillBuffer` / `vkCmdClearColorImage` |
| **涉及** | RHI、全部后端 |

### G-13 常量缓冲不能按偏移绑定

| 项目 | 内容 |
|---|---|
| **状态** | 🔧 后端 |
| **现状** | 设备用 `D3D11CreateDevice`（`T3DD3D11Context.cpp:166`）创建，只用了 `ID3D11DeviceContext`，没有查询 `ID3D11DeviceContext1`，所以没有 `VSSetConstantBuffers1` 这类按偏移绑定的接口 |
| **影响** | 每物体常量只能每个物体一个小 CB 或每次 `Map(WRITE_DISCARD)`，draw call 多时 CPU 开销偏高。不影响正确性 |
| **要做** | 初始化时查询 `ID3D11DeviceContext1`（Win8+ / FL 11.1 可用），RHI 的 `set*ConstantBuffers` 增加可选的偏移 / 大小参数（16 个常量即 256 字节对齐），配合一个大的环形动态 CB 使用。GL 对应 `glBindBufferRange`，Vulkan 对应动态 UBO 偏移 |
| **涉及** | RHI（可选参数，保持向后兼容）、全部后端、材质系统的常量上传 |

### G-14 Cubemap 带缩放的 blit

已在 `D3D11-Renderer-Backend-todo.md`「待办」中登记：全屏四边形路径缺一个 cube 采样 shader。G-03 落地后，IBL 预过滤会频繁需要 Cube → Cube 的缩放拷贝，届时一起处理。

---

## 5. 明确不做

| 项目 | 原因 |
|---|---|
| D3D11 deferred context | 引擎已经有 RHI 线程，D3D11 的 deferred context 在大多数驱动上没有真正的并行收益，引入后的状态管理成本更高 |
| Stream Output | 用 Compute + UAV 可以完全替代，且其它后端（Metal、GLES）没有对应能力 |
| Tiled Resources / 稀疏纹理 | 需要 FL 11.2+，且属于虚拟纹理这类大特性，等有实际需求再评估 |

---

## 6. 推进顺序

| 顺序 | 条目 | 理由 |
|:---:|---|---|
| 1 | G-01 + G-02 | 投入最小、解锁最多。D3D11 侧基本只是补映射表；主要工作量在枚举分组、按格式查询能力，以及其它后端同步跟进 |
| 2 | G-03 + G-05 + G-04 | 三者共用一套「子资源视图」设计，应一次设计、分步实现。落地后才能做点光源阴影、CSM、IBL。**已完成**（D3D11、GL4），各后端剩余工作见对应条目 |
| 3 | G-08 + G-07 | 工具链能力，越早做越省调试时间。G-08 改动小，可以插到任意阶段先做。**已完成**（D3D11、GL4 运行验证，其余后端见对应条目） |
| 4 | G-06、G-09 | 各自独立。G-09 会改变窗口 MSAA 的语义，需要单独验证 |
| 5 | G-10～G-13 | 按需逐个补 |

验证方案见 §7。

---

## 7. 验证方案

### 7.1 原则

1. **不再往 `ComputeApp` 里加用例。** 它已经接近 1 万行，而且是「跑完看 PASS / FAIL」的数据比对，看不到画面。
2. **用真实场景里的真实效果验证。** 每个 Sample 都是一个具体场景（地下仓库、夜晚街道、汽车展厅……），被验证的功能在场景里是真正在用的，就像 `SkyboxApp` 里真的有一个天空盒。看这个效果对不对，就是验证。**不做**调试可视化：不把 Cube 六个面展开显示，不把 mip 链排成一条，不放测试图网格，也不用按级联编号着色这类调试着色模式。
3. **按真实用法组合条目。** 一个场景里本来就会一起出现的功能放在同一个 Sample，例如 HDR、Bloom 和 sRGB 纹理都放在夜景里，不为了验证某一项单独搭场景。
4. **场景要动。** 光源、物体或相机持续运动。「只渲染了第一帧」「读到上一帧的旧数据」这类错误，在静止画面里看不出来。
5. **相机按预设路径自动移动。** 现有 Samples 都没有按键处理，所以相机沿固定路线运动，路线要经过需要观察的位置（贴近物体、远眺、穿过明暗交界等）。
6. **检查点用场景里的效果来描述。** 写清楚「正确时这个场景看起来是什么样」和「出错时通常会看到什么」，让人看画面就能判断，不用读日志。
7. **数据校验只作补充。** 画面上看不出精确值的内容（例如 timestamp 的数值是否合理），可以在对应 Sample 的 `onPostRender` 里校验并打日志，但不能取代画面验证。
8. **跨后端。** 每个 Sample 至少在 D3D11 和 GL4 上跑通。后端不支持某项能力时，场景退回到不带该效果的版本，并在日志里说明原因，不能黑屏。
9. **场景资产。** 优先用内置几何体和现有 Samples 的资源搭建；需要新模型或贴图时放在对应 Sample 自己的资源目录下。
10. **按主要验证的功能命名。** 与现有 `ShadowApp`、`LightApp`、`SkyboxApp` 一致，用功能名加 `App`，不用场景名。同一功能已有 Sample 时加限定词区分，例如方向光阴影已是 `ShadowApp`，点光源阴影叫 `PointLightShadowApp`。场景内容写在各 Sample 的说明里。

### 7.2 Sample 与条目对应

| Sample（新建） | 场景 | 覆盖条目 |
|---|---|---|
| `HDRApp` | 夜晚的街道，路灯、霓虹招牌、砖墙建筑 | G-01、G-02、G-05 |
| `SRGBApp` | 静态测试图（灰阶、色条、暗部渐变、棋盘格），附读回自检 | G-02（sRGB） |
| `PointLightShadowApp` | 地下仓库，一盏吊灯来回摆动，墙上挂着监控屏幕 | G-03、G-04 |
| `CascadedShadowApp` | 阳光下的大片山地森林，相机低空飞越 | G-03 |
| `ReflectionProbeApp` | 汽车展厅，转台上的汽车和金属展品 | G-02、G-03、G-04、G-14 |
| `SoftParticleApp` | 夜间营地，篝火、烟雾、帐篷 | G-06 |
| `OcclusionCullingApp` | 高楼林立的城市街区，相机沿街行驶 | G-07、G-08、G-09、G-13 |
| `OITApp` | 酒吧吧台，玻璃杯、彩色酒瓶、玻璃展柜 | G-10、G-11、G-12 |

### 7.3 各 Sample 的场景与检查点

#### `HDRApp`：雨后夜晚的街道（G-01、G-02、G-05）

**场景**：一条两侧是砖墙建筑的街道，有一排路灯和几块彩色霓虹招牌，沥青路面在路灯下有高光。相机沿街道缓慢前进，偶尔贴近墙面。路面不做倒影：引擎目前没有屏幕空间反射或平面反射，这一项不在本文范围内。

**功能怎么用上**：场景渲染到 `R16G16B16A16_FLOAT` 的 HDR 缓冲（路灯和霓虹的亮度远大于 1），经过 Bloom 和色调映射输出。Bloom 的降采样链用 Compute 逐级写 mip（G-05）。砖墙、路面、招牌贴图使用 sRGB 的 BC7 / BC1 格式（G-02），输出到 sRGB 后缓冲。

| 检查点 | 正确的效果 | 出错时通常看到 |
|---|---|---|
| HDR 缓冲（G-01） | 路灯和霓虹中心明亮但仍保留颜色，四周有自然的光晕；暗处的墙面仍看得到砖缝 | 灯芯是一块纯白，霓虹失去颜色，说明实际仍是 8 位缓冲，亮度被截断 |
| Bloom 降采样链（G-05） | 光晕柔和、圆润，相机移动时稳定 | 光晕呈方块状、只有一圈很小的光晕，或者闪烁跳动，说明某几级 mip 没写进去 |
| sRGB 纹理与后缓冲（G-02） | 砖墙和路面颜色自然，明暗过渡平顺 | 整个画面发灰发白（多做了一次转换），或暗部糊成一片黑（少做了一次转换） |
| BC 压缩纹理（G-02） | 贴近墙面时砖块纹理清晰，只有压缩带来的轻微块感 | 贴图花屏、出现斜条纹，或只有一角正常，说明按像素而不是按 4×4 块计算了行距 |

**当前进度（初版，只覆盖 G-01）**：`source/Samples/HDRApp` 已实现，D3D11 与 GL4 人眼验收通过，「HDR 缓冲」一行的检查点成立。
- **场景**：街道、砖墙、路灯、霓虹招牌全部由代码生成，贴图是程序生成的 8 位纹理（带 CPU 生成的 mip 链）。四块霓虹招牌的灯管贴图是 `R16G16B16A16_FLOAT`，峰值约 6；格式不可过滤时退回 8 位，并把峰值乘回自发光强度。相机沿 10 个路点做 Catmull-Rom 循环，青色招牌会周期性闪烁（招牌亮度和点光源同步变化）。
- **HDR 缓冲**：`Camera` 新增 `AllowHDR` 属性。开启后，窗口相机的中间 RT 依次尝试 `R16G16B16A16_FLOAT`、`R11G11B10_FLOAT`（按 `supportsFormat` 检查 Texture | RenderTarget，MSAA 时再加 Multisample），都不支持时退回 8 位并打 warning。
- **后处理**：`HDRBloomTonemapBehaviour` 依次做半分辨率亮部提取（soft knee）、像素着色器盒式降采样 6 级、tent 升采样，最后与场景合成并做 ACES 色调映射。源格式不能作为可线性过滤的 RT 时，退回 blit 拷贝并打 warning。
- **Bloom 改 Compute（G-05）**：后端有 compute、纹理 UAV、子资源视图，且 HDR 格式带 `kFormatSupportUnorderedAccess` 时，降采样链和升采样链各是一张 6 级 mip 的渲染纹理，kernel 每次用 SRV 读一级、UAV 写一级（源码见 `HDRBloomComputeSources.cpp`，与 `assets/samples/shaders/HDRBloom*.cshader` 同步）。D3D11 与 GL4 走 Compute，日志打出「6 bloom mips in E_PF_R16G16B16A16_FLOAT ... run as compute」；其它后端退回像素着色器链。「Bloom 降采样链」一行的检查点在两个后端上成立。
- **与最终方案的差距**：贴图暂用未压缩的 8 位 UNORM、在 gamma 空间做光照（G-02 完成后改为 sRGB + BC，并输出到 sRGB 后缓冲）。Vulkan 下 HDRApp 与 PostProcessingApp 都在后处理阶段崩溃在 NVIDIA 驱动 `nvoglv64.dll` 内（同一偏移），属于 Vulkan 后处理路径的既有问题，另行排查。

#### `SRGBApp`：sRGB 测试图（G-02）

独立的功能验证 Sample，只依赖 `Samples/Common` 的全屏顶点着色器，不引用其它 Sample 的代码和资源。它是静态测试图，不遵守 7.1 的「场景要动」：sRGB 的编码、解码都是逐像素的确定变换，静态画面配合读回比运动场景更容易判断。场景为空，相机上挂 `SRGBTestBehaviour`（相机特效），每帧把测试图画满整个窗口。

**画面检查点**（后缓冲是 UNORM，所以测试图在 shader 里手动做最后一次线性 → sRGB 编码）：

| 行 | 内容 | 正确时 | 出错时 |
|---|---|---|---|
| 1 | 灰阶：UNORM 参考 \| sRGB 纹理采样后编码 \| sRGB 纹理不编码 | 前两段完全接上、看不出分界；第三段明显更暗 | 前两段有明显接缝，说明采样没有解码或解码了两次 |
| 2 | 八色条：UNORM 参考 \| `R8G8B8A8_SRGB` \| `B8G8R8A8_SRGB` | 三段颜色一致 | BGRA 段红蓝互换，或某段整体发灰 / 偏暗 |
| 3 | 暗部线性渐变分别写进 UNORM RT（上半）和 sRGB RT（下半） | 上半有明显色带，下半平滑 | 两半一样，说明 sRGB RT 没有编码 |
| 4 | 1 像素黑白棋盘 \| sRGB 棋盘双线性取平均后编码 \| UNORM 棋盘双线性平均 | 中段与棋盘的视觉亮度一致；右段明显偏暗 | 中段和右段一样暗，说明过滤发生在解码之前 |

**读回自检**：第 3 帧对 8 项检查发起读回，结果以 `SRGBApp : PASS / FAIL / SKIP` 打到日志，最后一行汇总。检查项有：sRGB 纹理按原字节存储（两种格式）、采样解码到 UNORM RT（两种格式）、sRGB RT 编码（两种格式）、带 UAV 位的 `R8G8B8A8_SRGB` RT 编码（D3D11 走 typeless 资源）、sRGB 双线性过滤在线性空间平均（期望 128）。能力表不支持的格式记为 SKIP，不会黑屏。

**当前结果**：D3D11 8 项全部 PASS；GL4 7 项 PASS、1 项 SKIP（GL 不报告 sRGB UAV，符合预期）；Vulkan 与 HDRApp 一样，在后处理阶段崩溃在 `nvoglv64.dll` 内的同一偏移，属于上面提到的既有问题。

#### `PointLightShadowApp`：地下仓库（G-03、G-04）

**场景**：一个低矮的地下仓库，四周堆着货架和木箱，天花板中央吊着一盏点光灯，灯来回摆动。墙上挂着一块监控屏幕，实时显示另一台摄像机拍到的仓库角落画面。相机在仓库里绕行，会从斜侧和远处经过监控屏幕。

**功能怎么用上**：点光源阴影渲染到 Cube 深度图的六个面（G-03）。监控画面渲染到一张带 mip 链的渲染纹理，每帧渲染完调用 `generateMips` 后贴到屏幕上（G-04）。

| 检查点 | 正确的效果 | 出错时通常看到 |
|---|---|---|
| 点光源阴影（G-03） | 地面、四面墙和天花板上都有货架和木箱的影子，影子随灯的摆动同步移动、拉长和缩短 | 只有一个方向有影子（只渲染了 Cube 的一个面），影子落在错误的墙上，或方向颠倒 |
| 阴影在墙角的连续性 | 影子跨过墙角、从地面延伸到墙面时是连续的 | 在墙角处断开、错位，或出现一条亮缝 |
| 监控屏幕（G-04） | 从远处或斜着看屏幕时，画面稳定、略微模糊 | 屏幕画面闪烁，出现细碎的噪点和摩尔纹，说明 mip 没生成或没用上 |
| mip 每帧更新 | 角落里有人或物体移动时，远看屏幕也能看到同步的画面变化 | 远看时屏幕画面停住不动，近看才更新，说明 mip 只生成了一次 |

**当前进度（初版，覆盖 G-03 Cube 按面渲染与 G-04）**：`source/Samples/PointLightShadowApp` 已实现，D3D11 与 GL4 人眼验收通过，「点光源阴影」一行的检查点成立（地面、墙面上的影子都从光源向外投射、随光源移动；「墙角连续性」还没有专门对着墙角检查）。
- **场景**：一个 12 × 6 × 12 的房间（法线朝内），里面有两根柱子、几个箱子和一块悬空的搁板，全部由代码生成，顶点直接是世界坐标，不进引擎场景。点光源绕房间中心做椭圆轨道并上下浮动，位置上画一个不受光照的小方块。相机固定。
- **G-03**：`PointLightShadowBehaviour`（相机上的 `CameraBehaviour`，在 `onPostRender` 里用 RHI 直接绘制）建一张 512² `TT_CUBE` 的距离纹理（`R32G32_FLOAT`，不支持时用 `R16G16_FLOAT`）和一张同尺寸的 `D24S8` 深度 Cube，六个 `RenderTarget` 各用 `TextureSubresource::slice(face)` 同时选中两张 Cube 的同一个面。每面按 D3D 的 Cube 朝向用 `lookAt_LH` + 90° `perspective_LH` 渲染，像素着色器写线性距离的 `(d, d²)`。GL4 渲染到 FBO 时投影会翻转 Y，所以同一套矩阵在两个后端上得到相同的面布局。
- **G-04**：六个面画完后调用 `RenderTexture::generateMips()`，主 Pass 用 `SampleLevel` 采样 mip 1.5 做 VSM，影子边缘柔和；每帧重新生成，影子随光源连续移动。
- **与最终方案的差距**：还没有监控屏幕（「监控屏幕」「mip 每帧更新」两行未验证）、货架 / 木箱贴图和摆动的吊灯，相机也还不动；只带 HLSL / GLSL 源码，Vulkan 等后端上打 error 后只剩清屏颜色，不满足 7.1 第 8 条「退回不带该效果的版本」。

#### `CascadedShadowApp`：阳光下的山地森林（G-03）

**场景**：起伏的山地上分布着大片树木、岩石和木栅栏，阳光斜射。相机从贴近地面的位置起飞，低空飞越整片森林，再爬升远眺。太阳角度随时间缓慢变化。

**功能怎么用上**：方向光使用 4 级级联阴影（CSM），各级阴影图渲染到同一个深度纹理数组的不同层（G-03）。

| 检查点 | 正确的效果 | 出错时通常看到 |
|---|---|---|
| 近处阴影 | 相机贴近地面时，树叶和栅栏的影子轮廓清晰 | 近处影子很模糊，和远处一样粗糙，说明只有一级在起作用 |
| 远处阴影 | 远处山坡上的树林同样有影子，整片森林有立体感 | 一定距离之外完全没有影子，说明后几层没有渲染 |
| 级联过渡 | 相机前进时，影子从清晰到柔和平滑过渡 | 在某个固定距离上影子突然变化、跳动或整片消失 |
| 太阳移动 | 影子方向随太阳角度连续变化 | 某一段距离上的影子方向和其它地方不一致 |

#### `ReflectionProbeApp`：汽车展厅（G-02、G-03、G-04、G-14）

**场景**：明亮的展厅中央，一辆汽车停在缓慢旋转的转台上，旁边陈列着镀铬、拉丝铝、磨砂金属几种展品，展厅里有几块会移动的灯箱广告。相机绕展台缓慢环绕。

**功能怎么用上**：展台中心放一个实时反射探针，每帧把展厅渲染到 Cube 颜色渲染目标（G-03），再 `generateMips` 生成不同粗糙度用的模糊级别（G-04），并用 Cube 缩放 blit 拷贝出低分辨率版本作为漫反射环境光（G-14）。引擎内置的 `Tiny3DStandard` 仍是 Blinn-Phong，没有 PBR（`Tiny3D-Capability-Status-todo.md` GAP-05），所以车漆和展品使用本 Sample 自带的反射 shader：按材质粗糙度选择环境 Cube 的 mip 级别采样。基础色贴图是 sRGB（G-02）。

| 检查点 | 正确的效果 | 出错时通常看到 |
|---|---|---|
| 实时反射（G-03） | 车身和镀铬展品上能看到展厅和灯箱的倒影，灯箱移动、转台旋转时倒影同步变化 | 倒影是黑的、静止不动，或者只有一个方向有内容 |
| 反射方向 | 灯箱在车的左边时，倒影也出现在车身左侧 | 倒影上下或左右颠倒，说明 Cube 面序或朝向错了 |
| 粗糙度（G-04） | 镀铬是清晰的镜面，拉丝铝的倒影略模糊，磨砂金属只剩柔和的明暗 | 所有材质的倒影都一样清晰，或粗糙表面上出现闪烁的亮点 |
| 环境光（G-14） | 车身暗部带有展厅墙面颜色的环境光，整体协调 | 车身某些朝向有明显的色块或接缝，说明 Cube 缩放 blit 有面丢失或错位 |
| sRGB 基础色（G-02） | 车漆颜色饱满，和参考色一致 | 车漆发灰或颜色过深 |

#### `SoftParticleApp`：夜间营地（G-06）

**场景**：林间空地上有一堆篝火，烟雾从火堆升起，穿过旁边的帐篷边缘和树干，飘向夜空。风向会缓慢变化，带着烟雾左右飘动。相机绕营地慢慢移动，有时从帐篷后方看过去。

**功能怎么用上**：烟雾和火焰粒子做软粒子：渲染时绑定只读深度视图，粒子 shader 同时采样场景深度，在接近地面和物体的地方淡出（G-06）。

| 检查点 | 正确的效果 | 出错时通常看到 |
|---|---|---|
| 软粒子 | 烟雾贴近地面、穿过帐篷边缘和树干的地方是柔和的过渡 | 烟雾与地面、树干相交处有一条生硬的直线，说明深度没读到 |
| 遮挡关系 | 从帐篷后面看，烟雾被帐篷正确挡住 | 烟雾画在了帐篷前面，说明为了读深度把深度测试关掉了 |
| 没有资源冲突 | D3D11 debug layer 没有深度同时作为 SRV 和 DSV 的警告 | 日志里出现 hazard 警告，或烟雾整片消失 |

#### `OcclusionCullingApp`：城市街区（G-07、G-08、G-09、G-13）

**场景**：一片高楼林立的城市街区，街道两侧有大量独立的路灯、车辆、指示牌、长椅和空中的电线。相机像车一样沿街道行驶并转过街角。画面角落用 RmlUi 显示一个 GPU 性能面板。

**功能怎么用上**：

- 被近处高楼挡住的远处建筑和街道物件，用遮挡查询加条件渲染跳过绘制（G-07）；
- 性能面板显示各 Pass 的 GPU 耗时，数据来自 timestamp 查询（G-07）；
- 上千个街道物件逐个绘制，各自的变换和颜色常量放在一个大环形常量缓冲里按偏移绑定（G-13）；
- 窗口使用 Flip 交换链和 4×MSAA（G-09）；
- 渲染管线各阶段打了调试分组，资源都有名字（G-08）。

| 检查点 | 正确的效果 | 出错时通常看到 |
|---|---|---|
| 遮挡剔除（G-07） | 转过街角时，原本被挡住的建筑立即正常出现，画面上不会有物体凭空闪现或消失；性能面板显示的绘制数量随视野变化 | 刚转过街角时建筑延迟几帧才出现（「穿帮」），或绘制数量始终不变 |
| GPU 耗时面板（G-07） | 各 Pass 的耗时稳定，视野里建筑多时不透明 Pass 的耗时明显增加 | 数值全为 0、为负数或剧烈跳动 |
| 大量物件（G-13） | 每个路灯、车辆、指示牌的位置和颜色都正确，帧率明显高于每物体单独一个常量缓冲的旧路径 | 物件颜色串到别的物件上、位置错乱或闪烁，说明偏移没按 256 字节对齐，或环形缓冲覆盖了 GPU 还在读的数据 |
| 窗口 MSAA（G-09） | 空中的电线和指示牌边缘平滑 | 电线断断续续、边缘有锯齿，说明改成「离屏 MSAA + Resolve」后 Resolve 没接上 |
| 交换链（G-09） | 配置里打开 VSync 时转弯平稳无撕裂；关闭 VSync 并允许撕裂时帧率不受刷新率限制；拖动窗口改变大小后画面正常 | 改变窗口大小后画面拉伸或黑屏，日志出现 DXGI 警告 |
| 调试分组（G-08） | 用 RenderDoc 抓一帧，事件树按阴影、不透明、天空盒、透明、后处理、UI 分组，资源都显示名字 | 事件树是平铺的长列表，资源都是匿名的 |

调试分组本身不产生画面，这一项以 RenderDoc 截图作为验证记录。

**当前进度（初版，覆盖 G-07 与 G-08）**：`source/Samples/OcclusionCullingApp` 已实现，D3D11 与 GL4 上数据校验通过；人眼验收和 RenderDoc 截图还没做。
- **场景**：8 × 8 个街区（街区 40 m、街道 14 m），226 栋高楼、768 盏路灯、186 辆车，共 1180 个物件，全部由代码生成，顶点直接是世界坐标，放在一个大顶点缓冲里（每个物件后面跟着它的包围盒代理），地面单独一个缓冲；带雾和天空色的简单光照 shader（`OcclusionCullingShaders.cpp`）。相机以 9 m/s 沿 1080 m 的环形路线行驶并转过街角，视线看向前方 10 m。
- **G-07 遮挡剔除**：`OcclusionCullingBehaviour`（相机上的 `CameraBehaviour`，在 `onPostRender` 里用 RHI 直接绘制）每帧按距离从近到远排序，每个物件先取消条件渲染、关深度写入和颜色写入画包围盒代理并包一个 `kBinaryOcclusion` 查询，再以这个查询为谓词画本体。查询按 4 帧轮换，非阻塞地取回上几帧的结果统计可见数量。整个物件 Pass 包一个 `kPipelineStatistics` 查询。代理必须在取消谓词后画，否则会被上一个物件的谓词跳过，查询读到 0。
- **G-07 GPU 计时**：`pipeline->enableGPUProfiling(true)` 打开 `GPUProfiler`，Sample 在管线分组下再加 `City Ground`、`City Objects` 两个作用域，每 2 秒把各段耗时、整帧 GPU 时间、可见数量和 pipeline statistics 打到日志。
- **G-08**：管线自带分组，Sample 再加 `City` / `City Ground` / `City Objects` 分组，两个顶点缓冲设置了调试名（常量缓冲由 `RenderResourceManager` 按 cbuffer 名命名）。
- **结果**：D3D11 与 GL4 每帧可见约 50～80 / 1180，IA 顶点约 4.6～4.8 万（全部绘制是 12.7 万），`City Objects` 在 D3D11 上约 6～8 ms、GL4 上约 11 ms（GPU 上逐物件串行等待查询结果的代价，见 G-07 待做），顶层分组耗时之和与整帧 GPU 时间吻合，无丢帧，日志无 error / warning。
- **与最终方案的差距**：没有 RmlUi 性能面板（数据只进日志）、指示牌 / 长椅 / 电线；「遮挡剔除」用的是同帧测完就画，不会出现「穿帮」，但在 GL 上 GPU 开销偏大，后续改成用上一帧结果并对新进入视野的物件保守绘制；G-09、G-13 未做，对应检查点未验证；只带 HLSL / GLSL 源码。

#### `OITApp`：酒吧吧台（G-10、G-11、G-12）

**场景**：吧台上摆着一排装着不同颜色酒水的玻璃杯和酒瓶，有几只杯子放在缓慢旋转的展示托盘上，彼此前后交错。吧台后方是彩色玻璃装饰窗，一侧是玻璃展柜。相机沿吧台缓慢移动，经过展柜时会靠近。

**功能怎么用上**：

- 玻璃杯、酒瓶和酒水使用逐像素链表的顺序无关透明（OIT）：PS 中写 UAV 构建每个像素的片段链表（G-11），链表头指针缓冲每帧用 `clearUnorderedAccess` 清空（G-12）；
- 彩色玻璃窗使用双源混合，按颜色通道分别控制透过率（G-10）；
- 相机靠近展柜时，用常量混合因子把展柜玻璃整体淡出，避免挡住视线（G-10）。

| 检查点 | 正确的效果 | 出错时通常看到 |
|---|---|---|
| 顺序无关透明（G-11） | 托盘旋转时，交错的杯子和酒水前后关系始终正确，透过前一只杯子能看到后一只的颜色 | 旋转过程中杯子的前后关系突然跳变，或某只杯子整只消失 |
| 每帧清空（G-12） | 画面干净，杯子移动后原位置不留痕迹 | 出现拖影，并且越来越亮、越来越花，说明上一帧的链表没清掉 |
| 与不透明物体共存 | 吧台、酒瓶标签等不透明部分正常显示 | 不透明部分消失，说明 PS 的 UAV 与 RTV 槽位冲突 |
| 彩色玻璃窗（G-10） | 透过红色玻璃看到的后方物体偏红，透过蓝色玻璃偏蓝 | 所有玻璃只是整体变暗，没有颜色倾向，说明双源混合没生效 |
| 展柜淡出（G-10） | 相机靠近时展柜玻璃平滑淡出，离开时淡入 | 展柜玻璃突然出现或消失，或者始终不变 |

### 7.4 补充的数据校验

以下内容在画面上看不出精确值，作为补充放在对应 Sample 的 `onPostRender` 里，结果输出到日志：

| 条目 | 所在 Sample | 校验内容 |
|---|---|---|
| G-07 | `OcclusionCullingApp` | 各 Pass 的 timestamp 之和与 CPU 侧测得的 GPU 帧时间处于同一量级。**已实现**：顶层分组之和与整帧 GPU 时间相差 < 0.01 ms；同时打出 pipeline statistics 与可见数量 |
| G-13 | `OcclusionCullingApp` | 同一帧中，按偏移绑定路径和旧路径的 CPU 提交耗时对比 |

### 7.5 场景资源从哪来

#### 7.5.1 现状

仓库里能直接用的样例资源很少：

| 类别 | 现有内容 | 位置 |
|---|---|---|
| 模型 | 1 个乌龟 FBX；内置的 cube、sphere、capsule、cylinder、plane、quad | `assets/samples/raw/`、`assets/editor/builtin/meshes/` |
| 贴图 | `blocks.png` 一张；默认天空盒 | `assets/samples/textures/`、`assets/editor/builtin/textures/` |
| 代码生成的网格 | `ShadowApp` 等 Sample 在代码里手写顶点，用 `SubMesh::create` 建网格；`Samples/Common/SampleMeshes.cpp` 目前是空文件 | `source/Samples/` |

资源导入链路是：原始文件放在 `assets/samples/raw/`，用 `mconv` / `imgconv` 转成 `.tmesh` / `.ttex` 放进 `assets/samples/meshes/`，构建时由 `ResourceApp` / `SkyboxApp` 的 POST_BUILD 调 `bundlebuilder` 打成共享的 `assets/samples/bundle`。这条链路对新场景有四个限制：

1. `mconv` 只能导入 FBX，没有 glTF / OBJ 导入（glTF 已在 `Tiny3D-Capability-Status-todo.md` GAP-10 登记）；
2. `imgconv` 只能输出 8 位格式，不能做 BC 压缩，也不能导入 `.hdr` / `.exr`。这两点正是 G-01、G-02 本身要补的，所以**验证 G-01 / G-02 所需的资源，要等 `imgconv` 的格式扩展完成后才能生成**；
3. Git LFS 只追踪 `dll` / `lib` / `dylib` / `so`，模型和贴图会直接进 Git 历史；
4. 所有 Sample 共用一份 bundle，`build-vs2019-x64-debug.bat` 已经为此关掉了并行构建。新 Sample 继续往里加，打包时间和冲突都会增加。

#### 7.5.2 获取方式

两种来源结合使用：

1. **在代码里生成**：建筑外壳、地形、货架、玻璃杯这类形状规则、需要精确控制尺寸和布局的物体，在 Sample 代码里生成。好处是不占仓库体积、没有许可证问题，而且布局可控。比如遮挡剔除需要「近处高楼正好挡住远处街区」，用代码摆放最容易做到。
2. **使用 CC0 公共素材**：树木、汽车、路灯、帐篷这类形状复杂的物件，以及砖墙、沥青、木材这类写实贴图和 HDR 环境图，从 CC0 素材库下载。CC0 允许修改、再分发和商用，不要求署名。

| 来源 | 内容 | 许可证 | 可用格式 | 适合的场景 |
|---|---|---|---|---|
| [Kenney](https://kenney.nl/assets) | 低多边形模型套件：City Kit (Commercial)、City Kit (Roads)、Car Kit、Nature Kit、Survival Kit 等 | CC0 | FBX、OBJ、glTF | 城市建筑、路灯、车辆、树木、岩石、栅栏、帐篷、篝火 |
| [Poly Haven](https://polyhaven.com) | HDR 环境图、写实 PBR 贴图、扫描模型 | CC0 | HDRI 为 `.hdr` / `.exr`；模型为 glTF、FBX、Blend | 夜景和展厅的环境图、展厅里的写实展品 |
| [ambientCG](https://ambientcg.com) | 写实 PBR 贴图（颜色、法线、粗糙度等） | CC0 | 按分辨率打包的 JPG / PNG | 砖墙、沥青、混凝土、木箱、金属 |

Quaternius 的早期套件是 CC0，但较新的套件改用了自有许可证（Quaternius Asset License），**不能默认当作 CC0**，使用前要逐个核对。

#### 7.5.3 各 Sample 的资源方案

| Sample | 代码生成 | CC0 素材 | 依赖的工具链改动 |
|---|---|---|---|
| `HDRApp` | 街道两侧的建筑外壳（按世界坐标平铺 UV 的长方体）、路面、霓虹招牌面片 | ambientCG 的砖墙、沥青贴图；Kenney City Kit (Roads) 的路灯；Poly Haven 的夜景 HDRI 作天空 | `imgconv` 支持 sRGB + BC 输出、导入 `.hdr`（G-01、G-02） |
| `PointLightShadowApp` | 仓库房间、货架、木箱、监控屏幕 | ambientCG 的混凝土、木材贴图 | `imgconv` 支持 sRGB 输出（G-02），没有的话先用 8 位线性贴图 |
| `CascadedShadowApp` | 用噪声生成的高度场地形网格 | Kenney Nature Kit 的树木、岩石、栅栏 | 无 |
| `ReflectionProbeApp` | 展厅房间、转台、灯箱、展台上的金属球和圆环 | Kenney Car Kit 的车（低多边形），或 Poly Haven 的写实模型 | 写实模型若只有 glTF 版本，需先在 Blender 里转成 FBX |
| `SoftParticleApp` | 地面、帐篷（三棱柱）、粒子贴图（柔边圆形与噪声纹理，可在代码里生成） | Kenney Survival Kit 的篝火和帐篷；Kenney Nature Kit 的树 | 无 |
| `OcclusionCullingApp` | 城市街区布局、高楼外壳（高度不一的长方体）、电线 | Kenney City Kit (Commercial) 的建筑；City Kit (Roads) 的道路和路灯；Car Kit 的车辆。上千个物件复用这几十种模型，但每个都单独绘制（G-13 需要逐个绘制，不能用实例化） | 无 |
| `OITApp` | 玻璃杯、酒瓶（用旋转体 / lathe 生成）、吧台、展柜、彩色玻璃窗 | ambientCG 的木材贴图（吧台） | 无 |

#### 7.5.4 落地要求

1. **先做导入试验。** 正式开工前，从 Kenney 和 Poly Haven 各取一个 FBX 模型走一遍 `mconv` → `bundlebuilder` → 加载显示，确认顶点、UV、材质和贴图引用都能正确导入。`mconv` 目前只用乌龟模型验证过，第三方 FBX 的材质引用方式可能不同。
2. **代码生成的网格放进公共库。** 在 `Samples/Common/SampleMeshes.cpp`（目前为空）里实现长方体（带平铺 UV）、高度场、旋转体、三棱柱等生成函数，各 Sample 共用，不要每个 Sample 各写一份 `SubMesh::create`。
3. **只提交用到的文件。** 不把整个素材包压缩文件放进仓库，只提交实际用到的模型和贴图；贴图优先用 1K 分辨率，HDRI 优先用 1K 或 2K。
4. **记录来源。** 每个第三方素材目录下放一个 `SOURCE.txt`，写明素材名、来源 URL、作者、许可证和下载日期，并保留素材包自带的许可证文件。CC0 虽然不要求署名，但来源要可追溯，方便以后核对或替换。
5. **补 LFS 规则。** 为 `assets/samples/raw/` 下的 `*.fbx`、`*.png`、`*.jpg`、`*.hdr`、`*.exr` 增加 Git LFS 追踪规则，避免二进制资源直接进 Git 历史。转换后的 `.tmesh` / `.ttex` 和 bundle 是否也走 LFS，按体积再定。
6. **新 Sample 用自己的 bundle。** 新 Sample 的资源打进各自的 bundle 目录（如 `assets/samples/<AppName>/bundle`），不再往共享 bundle 里加，避免继续放大打包时间和并行构建冲突。

