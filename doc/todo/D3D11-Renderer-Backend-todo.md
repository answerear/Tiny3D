# D3D11 渲染后端接口实现状态

> 本文档基于 `RHIContext` 纯虚接口定义，逐一分析 `D3D11Context`（Window 版本）的实现完成度。
>
> - **接口定义**：`source/Core/Include/RHI/T3DRHIContext.h`
> - **D3D11 基类实现**：`source/Plugins/Renderer/Direct3D11/Base/Source/T3DD3D11ContextBase.cpp`
> - **D3D11 Window 实现**：`source/Plugins/Renderer/Direct3D11/Window/Source/T3DD3D11Context.cpp`
> - **D3D11 Window 头文件**：`source/Plugins/Renderer/Direct3D11/Window/Include/T3DD3D11Context.h`
> - **D3D11 Console 实现**：`source/Plugins/Renderer/Direct3D11/Console/Source/T3DD3D11ConsoleContext.cpp`
>
> 本文中未特别标注文件名的「实现位置」行号均指 `T3DD3D11Context.cpp`。

---

## 实现状态图例

| 标记 | 含义 |
|------|------|
| ✅ 已完成 | 功能完整实现 |
| ⚠️ 部分完成 | 有条件分支但仅实现了部分分支，或缺少边界处理 |
| ❌ 未实现 | 函数体为空或直接返回空值/T3D_OK，无实际逻辑 |
| 🔇 按设计为空 | D3D11 后端按设计意图不需要实现（如 Vulkan 专用的 begin/endRender） |
| 💬 被注释禁用 | 代码已写但被注释掉，功能未生效 |

---

## 1. 变换 (Transform)

### 1.1 setViewProjectionTransform

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat)` |
| **功能** | 设置视图变换矩阵和投影变换矩阵。D3D11 深度范围为 [0,1]，内部会通过转换矩阵将 [-1,1] 映射到 [0,1] |
| **参数** | `viewMat` — 视图变换矩阵；`projMat` — 投影变换矩阵 |
| **返回值** | `T3D_OK` |
| **实现位置** | `D3D11ContextBase::setViewProjectionTransform` (T3DD3D11ContextBase.cpp:49) |
| **备注** | 在 D3D11ContextBase 层实现，D3D11Context 继承使用 |

### 1.2 getDepthRemapMatrix

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `const Matrix4& getDepthRemapMatrix() const` |
| **功能** | 返回把光空间 Z 从平台 NDC 范围映射到 [0,1] 的矩阵。D3D11 的投影矩阵已在 `setViewProjectionTransform` 中内含 Z remap，因此使用基类默认实现返回单位矩阵；GL 系列后端才需要 override |
| **参数** | 无 |
| **返回值** | 单位矩阵 |
| **实现位置** | 基类默认实现 (T3DRHIContext.h:90) |

---

## 2. 渲染目标 (Render Target)

### 2.1 createRenderWindow

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIRenderTargetPtr createRenderWindow(RenderWindow *renderWindow)` |
| **功能** | 创建 RHI 渲染窗口。内部创建 DXGI SwapChain、BackBuffer 纹理、RenderTargetView，以及窗口自带的深度模板缓冲与 DSV，支持 MSAA |
| **参数** | `renderWindow` — 引擎渲染窗口对象，包含窗口句柄、宽高、全屏模式、MSAA 等描述信息 |
| **返回值** | 成功返回 `D3D11RenderWindowPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:369（内部委托到 T3DD3D11Context.cpp:814 的私有重载） |
| **备注** | 深度模板的尺寸与 `SampleDesc` 从 BackBuffer 的实际描述反查而不是信入参 —— `ResizeBuffers(0, 0, 0, ...)` 让 DXGI 自己取窗口客户区尺寸，实际值可能与传入的 w/h 不同，不一致会让 `OMSetRenderTargets` 因 RTV/DSV 不匹配被拒绝。格式取 `D3D11RenderWindow::DepthStencilFormat`（默认 `E_PF_D24_UNORM_S8_UINT`） |

### 2.2 createRenderTexture

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2D *buffer)` |
| **功能** | 创建 RHI 渲染纹理。根据像素格式自动判断创建颜色 RT 或深度模板 RT，支持 MSAA（会额外创建 ResolveTexture）和 shaderReadable 深度纹理 |
| **参数** | `buffer` — 引擎像素缓冲区对象，包含宽高、像素格式、MSAA 描述、shaderReadable 等信息 |
| **返回值** | 成功返回 `D3D11PixelBuffer2DPtr`（内含 D3DTexture/D3DRTView/D3DDSView/D3DSRView），失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:501，资源创建逻辑抽到 `buildRenderTextureResources` (T3DD3D11Context.cpp:521) 与 `resizeRenderTexture` 共用 |
| **备注** | 任一步失败会释放已创建的半成品 COM 对象再返回错误，不会留下部分初始化的 `D3D11PixelBuffer2D` |

### 2.3 setRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setRenderTarget(RenderTarget *renderTarget)` |
| **功能** | 设置当前渲染目标。根据 RenderTarget 类型（Window/Texture）调用不同的内部重载，自动备份当前 D3D11 管线状态并解绑残留 SRV/RTV（避免 HAZARD 警告），支持 MRT（Multiple Render Targets） |
| **参数** | `renderTarget` — 渲染目标对象，可以是窗口类型或纹理类型 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1143；状态备份见 `backupRenderState` (T3DD3D11Context.cpp:1005) |

### 2.4 resetRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult resetRenderTarget()` |
| **功能** | 清除当前渲染目标绑定，将 OMSetRenderTargets 设为 null |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1174 |

---

## 3. 视口与裁剪 (Viewport & Scissor)

### 3.1 setViewport

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setViewport(const Viewport &viewport)` |
| **功能** | 设置视口区域。根据当前 RenderTarget 类型（Window 或 Texture）自动获取宽高，然后按 viewport 的归一化比例计算实际视口大小。无 RenderTarget 或纹理 RT 既无颜色纹理也无深度纹理时打日志并跳过 |
| **参数** | `viewport` — 视口对象，包含 Left/Top/Width/Height 等归一化值 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1189 |

### 3.2 setScissorRect

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height)` |
| **功能** | 设置裁剪矩形（像素坐标，原点左上，Y 向下），调用 RSSetScissorRects。须配合 `RasterizerDesc::ScissorEnable = true` 才生效 |
| **参数** | `x` / `y` — 矩形左上角像素坐标；`width` / `height` — 矩形宽高（像素） |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1247 |

---

## 4. 清除操作 (Clear)

### 4.1 clearColor

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult clearColor(const ColorRGB &color)` |
| **功能** | 用指定颜色填充当前渲染目标的 framebuffer。根据 RenderTarget 类型分别处理 Window 和 Texture（支持 MRT 多目标清除） |
| **参数** | `color` — RGB 颜色值（alpha 固定为 1.0） |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1265 |

### 4.2 clearDepth

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult clearDepth(Real depth)` |
| **功能** | 用指定深度值填充当前渲染目标的 depth buffer（仅清除深度，不清除模板）。当前 RenderTarget 没有挂 DepthStencil 时直接返回 |
| **参数** | `depth` — 深度值（通常为 1.0） |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1334 |

### 4.3 clearDepthStencil

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult clearDepthStencil(Real depth, uint32_t stencil)` |
| **功能** | 用指定深度值和模板值同时填充当前渲染目标的 depth buffer 和 stencil buffer |
| **参数** | `depth` — 深度值；`stencil` — 模板值 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1341 |

---

## 5. 渲染状态 (Render State)

### 5.1 createBlendState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIBlendStatePtr createBlendState(BlendState *state)` |
| **功能** | 创建 RHI 颜色混合状态对象。将引擎 BlendDesc 映射为 D3D11_BLEND_DESC，支持 AlphaToCoverage、独立混合、最多 8 个 RenderTarget 的独立混合配置 |
| **参数** | `state` — 引擎颜色混合状态对象，包含 SrcBlend/DestBlend/BlendOp/ColorMask 等描述 |
| **返回值** | 成功返回 `D3D11BlendStatePtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:1399 |

### 5.2 createDepthStencilState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIDepthStencilStatePtr createDepthStencilState(DepthStencilState *state)` |
| **功能** | 创建 RHI 深度模板状态对象。将引擎 DepthStencilDesc 映射为 D3D11_DEPTH_STENCIL_DESC，支持前后面独立 Stencil 操作 |
| **参数** | `state` — 引擎深度模板状态对象，包含 DepthTestEnable/DepthWriteEnable/DepthFunc/StencilEnable/FrontFace/BackFace 等描述 |
| **返回值** | 成功返回 `D3D11DepthStencilStatePtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:1454 |

### 5.3 createRasterizerState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIRasterizerStatePtr createRasterizerState(RasterizerState *state)` |
| **功能** | 创建 RHI 光栅化状态对象。将引擎 RasterizerDesc 映射为 D3D11_RASTERIZER_DESC，支持 FillMode/CullMode/DepthBias/ScissorEnable/MSAA 等配置 |
| **参数** | `state` — 引擎光栅化状态对象 |
| **返回值** | 成功返回 `D3D11RasterizerStatePtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:1508 |

### 5.4 createSamplerState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHISamplerStatePtr createSamplerState(SamplerState *state)` |
| **功能** | 创建 RHI 纹理采样状态对象。将引擎 SamplerDesc 映射为 D3D11_SAMPLER_DESC，支持 Filter/AddressMode/LOD/ComparisonFunc/BorderColor 等配置 |
| **参数** | `state` — 引擎纹理采样状态对象 |
| **返回值** | 成功返回 `D3D11SamplerStatePtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:1557 |

### 5.5 setBlendState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setBlendState(BlendState *state)` |
| **功能** | 设置颜色混合状态，调用 OMSetBlendState |
| **参数** | `state` — 混合状态对象（内含已创建的 RHI 资源） |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1610 |

### 5.6 setDepthStencilState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setDepthStencilState(DepthStencilState *state)` |
| **功能** | 设置深度模板状态，调用 OMSetDepthStencilState |
| **参数** | `state` — 深度模板状态对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1623 |

### 5.7 setRasterizerState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setRasterizerState(RasterizerState *state)` |
| **功能** | 设置光栅化状态，调用 RSSetState |
| **参数** | `state` — 光栅化状态对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1635 |

---

## 6. 顶点输入 (Vertex Input)

### 6.1 createVertexDeclaration

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIVertexDeclarationPtr createVertexDeclaration(VertexDeclaration *decl)` |
| **功能** | 创建 RHI 顶点格式对象。遍历所有 VertexAttribute 映射为 D3D11_INPUT_ELEMENT_DESC，使用顶点着色器字节码创建 ID3D11InputLayout |
| **参数** | `decl` — 顶点格式对象，包含顶点属性列表（Semantic/Format/Slot/Offset）和关联的顶点着色器 |
| **返回值** | 成功返回 `D3D11VertexDeclarationPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:1647 |

### 6.2 setVertexDeclaration

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVertexDeclaration(VertexDeclaration *decl)` |
| **功能** | 设置顶点格式，调用 IASetInputLayout |
| **参数** | `decl` — 顶点格式对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1703 |

---

## 7. 缓冲区 (Buffer)

### 7.1 createVertexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIVertexBufferPtr createVertexBuffer(VertexBuffer *buffer)` |
| **功能** | 创建 RHI 顶点缓冲区对象。根据 Usage 和 CPUAccessMode 创建 D3D11 Buffer，支持初始数据和延迟填充 |
| **参数** | `buffer` — 引擎的顶点缓冲区对象，包含缓冲大小、Usage、CPUAccessMode 和可选的初始数据 |
| **返回值** | 成功返回 `D3D11VertexBufferPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:1716 |

### 7.2 setVertexBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets)` |
| **功能** | 设置渲染用的顶点缓冲对象，支持多个缓冲区绑定到不同 slot，调用 IASetVertexBuffers |
| **参数** | `startSlot` — 缓冲区起始插槽，对应 shader 寄存器索引；`buffers` — 顶点缓冲对象数组；`strides` — 每个缓冲区的顶点跨度（一个顶点的字节大小）；`offsets` — 每个缓冲区的起始偏移 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1786 |

### 7.3 createIndexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIIndexBufferPtr createIndexBuffer(IndexBuffer *buffer)` |
| **功能** | 创建 RHI 索引缓冲对象。根据 Usage 和 CPUAccessMode 创建 D3D11 Buffer，支持初始数据和延迟填充 |
| **参数** | `buffer` — 引擎的索引缓冲对象，包含缓冲大小、索引类型、Usage、CPUAccessMode 和可选的初始数据 |
| **返回值** | 成功返回 `D3D11IndexBufferPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:1809 |

### 7.4 setIndexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setIndexBuffer(IndexBuffer *buffer)` |
| **功能** | 设置渲染用的索引缓冲对象，自动映射索引类型为 DXGI_FORMAT，调用 IASetIndexBuffer |
| **参数** | `buffer` — 索引缓冲对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1879 |

### 7.5 createConstantBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIConstantBufferPtr createConstantBuffer(ConstantBuffer *buffer)` |
| **功能** | 创建 RHI 常量缓冲对象。使用初始数据创建 D3D11 Buffer（BindFlags 为 CONSTANT_BUFFER） |
| **参数** | `buffer` — 引擎的常量缓冲区对象，包含缓冲大小、Usage、CPUAccessMode 和初始数据 |
| **返回值** | 成功返回 `D3D11ConstantBufferPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:1895 |

### 7.6 createPixelBuffer1D

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBuffer1DPtr createPixelBuffer1D(PixelBuffer1D *buffer)` |
| **功能** | 创建 RHI 1D 像素缓冲区对象（1D 纹理）。创建 ID3D11Texture1D 与 SRV，支持 mipmap 链和 1D 纹理数组（`arraySize > 1` 时 SRV 维度自动切到 `TEXTURE1DARRAY`） |
| **参数** | `buffer` — 引擎 1D 像素缓冲区对象 |
| **返回值** | 成功返回 `D3D11PixelBuffer1DPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:2051 |
| **备注** | 子资源数组由 `buildSubresourceData` 统一构建；初始数据在入队前深拷贝一份，由 RHI 线程上的 lambda 在末尾释放 |

### 7.7 createPixelBuffer2D

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBuffer2DPtr createPixelBuffer2D(PixelBuffer2D *buffer)` |
| **功能** | 创建 RHI 2D 像素缓冲区对象（2D 纹理）。创建 ID3D11Texture2D 和 ID3D11ShaderResourceView，支持完整 mip 链与 2D 纹理数组 |
| **参数** | `buffer` — 引擎 2D 像素缓冲区对象，包含宽高、格式、mipmap、arraySize、采样描述等信息 |
| **返回值** | 成功返回 `D3D11PixelBuffer2DPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:2151 |
| **备注** | 原先只构造一个 `D3D11_SUBRESOURCE_DATA`、`SysMemPitch` 按 `DataSize / height` 反推的越界读问题已修复：改由 `buildSubresourceData` 按 `arraySize * mipLevels` 逐个计算 pitch，并在入口校验尺寸非零、数据量足够；无初始数据时传 `nullptr` 创建空纹理 |

### 7.8 createPixelBuffer3D

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBuffer3DPtr createPixelBuffer3D(PixelBuffer3D *buffer)` |
| **功能** | 创建 RHI 3D 像素缓冲区对象（3D 纹理/体积纹理）。创建 ID3D11Texture3D 和 `TEXTURE3D` 维度的 SRV，支持 mip 链 |
| **参数** | `buffer` — 引擎 3D 像素缓冲区对象 |
| **返回值** | 成功返回 `D3D11PixelBuffer3DPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:2251 |
| **备注** | 3D 纹理在 D3D11 不支持数组与 MSAA，`arraySize` 恒为 1。配套补全的内容：`D3D11PixelBuffer3D` 的 `create()` 与析构（T3DD3D11RenderBuffer.h:132）、`D3DTexture` 字段类型从 `ID3D11Texture2D*` 改为 `ID3D11Texture3D*`、`D3D11Mapping::get(const PixelBuffer3DDesc &)` 的实现、`PixelBuffer3DDesc` 成员的默认初值 |

### 7.9 createPixelBufferCubemap

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBufferCubemapPtr createPixelBufferCubemap(PixelBufferCubemap *buffer)` |
| **功能** | 创建 RHI 立方体贴图像素缓冲区对象。创建带 `D3D11_RESOURCE_MISC_TEXTURECUBE` 标志的 ID3D11Texture2D 和 SRV，支持 mip 链与 Cubemap Array |
| **参数** | `buffer` — 引擎立方体贴图像素缓冲区对象 |
| **返回值** | 成功返回 `D3D11PixelBufferCubemapPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:2338 |
| **备注** | `desc.arraySize` 存的是总面数，除以 6 得到立方体个数；`cubeCount > 1` 时 SRV 维度用 `TEXTURECUBEARRAY`，否则用 `TEXTURECUBE`。`buildSubresourceData` 产出的「外层 array slice、内层 mip」顺序正好匹配 D3D11 对 cubemap 的 `face * mipLevels + mip` 布局与 +X,-X,+Y,-Y,+Z,-Z 面序 |
| **备注** | `SampleDesc.Count` 固定为 1 —— `D3D11_RESOURCE_MISC_TEXTURECUBE` 不能与 MSAA 组合，这是 API 限制而非实现缺口 |

---

## 8. 顶点着色器 (Vertex Shader)

### 8.1 createVertexShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIShaderPtr createVertexShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 顶点着色器对象。使用已编译的字节码调用 CreateVertexShader |
| **参数** | `shader` — 引擎着色器变体对象，包含已编译的 HLSL 字节码 |
| **返回值** | 成功返回 `D3D11VertexShaderPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:2448 |

### 8.2 setVertexShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVertexShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的顶点着色器，调用 VSSetShader。支持传入 nullptr 解绑着色器 |
| **参数** | `shader` — 引擎使用的顶点着色器对象，可为 nullptr |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2490 |

### 8.3 setVSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置顶点着色器的常量缓冲区，调用 VSSetConstantBuffers |
| **参数** | `startSlot` — 常量缓冲区起始插槽，对应 shader 中 cbuffer 寄存器索引；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2515 |

### 8.4 setVSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置顶点着色器的纹理资源（SRV），调用 VSSetShaderResources。支持 1D/2D/3D/Cubemap 四种像素缓冲类型 |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2522（公共实现 `setPixelBuffers` 位于 T3DD3D11Context.cpp:3931） |

### 8.5 setVSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置顶点着色器的纹理采样器，调用 VSSetSamplers |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2529 |

---

## 9. 像素着色器 (Pixel Shader)

### 9.1 createPixelShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIShaderPtr createPixelShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 像素着色器对象。使用已编译的字节码调用 CreatePixelShader |
| **参数** | `shader` — 引擎着色器变体对象，包含已编译的 HLSL 字节码 |
| **返回值** | 成功返回 `D3D11PixelShaderPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:2536 |

### 9.2 setPixelShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPixelShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的像素着色器，调用 PSSetShader。支持传入 nullptr 解绑着色器 |
| **参数** | `shader` — 引擎使用的像素着色器对象，可为 nullptr |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2578 |

### 9.3 setPSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置像素着色器的常量缓冲区，调用 PSSetConstantBuffers |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2603 |

### 9.4 setPSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置像素着色器的纹理资源（SRV），调用 PSSetShaderResources |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2610 |

### 9.5 setPSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置像素着色器的纹理采样器，调用 PSSetSamplers |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2617 |

---

## 10. Hull 着色器 (Tessellation - Hull Shader)

### 10.1 createHullShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIShaderPtr createHullShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 曲面细分着色器对象（Hull Shader）。使用已编译的字节码调用 CreateHullShader |
| **参数** | `shader` — 引擎着色器变体对象 |
| **返回值** | 成功返回 `D3D11HullShaderPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:2623 |

### 10.2 setHullShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setHullShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的曲面细分着色器，调用 HSSetShader。支持传入 nullptr 解绑 |
| **参数** | `shader` — 引擎使用的曲面细分着色器对象，可为 nullptr |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2665 |

### 10.3 setHSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置 Hull 着色器的常量缓冲区，调用 HSSetConstantBuffers |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2690 |

### 10.4 setHSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置 Hull 着色器的纹理资源（SRV），调用 HSSetShaderResources |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2697 |

### 10.5 setHSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setHSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置 Hull 着色器的纹理采样器，调用 HSSetSamplers |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2704 |

---

## 11. Domain 着色器 (Tessellation - Domain Shader)

### 11.1 createDomainShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIShaderPtr createDomainShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 域着色器对象（Domain Shader）。使用已编译的字节码调用 CreateDomainShader |
| **参数** | `shader` — 引擎着色器变体对象 |
| **返回值** | 成功返回 `D3D11DomainShaderPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:2711 |

### 11.2 setDomainShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setDomainShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的域着色器，调用 DSSetShader。支持传入 nullptr 解绑 |
| **参数** | `shader` — 引擎使用的域着色器，可为 nullptr |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2753 |

### 11.3 setDSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置 Domain 着色器的常量缓冲区，调用 DSSetConstantBuffers |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2778 |

### 11.4 setDSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置 Domain 着色器的纹理资源（SRV），调用 DSSetShaderResources |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2785 |

### 11.5 setDSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setDSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置 Domain 着色器的纹理采样器，调用 DSSetSamplers |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2792 |

---

## 12. 几何着色器 (Geometry Shader)

### 12.1 createGeometryShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIShaderPtr createGeometryShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 几何着色器对象。使用已编译的字节码调用 CreateGeometryShader |
| **参数** | `shader` — 引擎着色器变体对象 |
| **返回值** | 成功返回 `D3D11GeometryShaderPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:2799 |

### 12.2 setGeometryShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setGeometryShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的几何着色器，调用 GSSetShader。支持传入 nullptr 解绑 |
| **参数** | `shader` — 引擎使用的几何着色器，可为 nullptr |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2841 |

### 12.3 setGSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置几何着色器的常量缓冲区，调用 GSSetConstantBuffers |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2866 |

### 12.4 setGSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置几何着色器的纹理资源（SRV），调用 GSSetShaderResources |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2873 |

### 12.5 setGSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setGSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置几何着色器的纹理采样器，调用 GSSetSamplers |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2880 |

---

## 13. 计算着色器 (Compute Shader)

### 13.1 createComputeShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIShaderPtr createComputeShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 计算着色器对象。使用已编译的字节码调用 CreateComputeShader |
| **参数** | `shader` — 引擎着色器变体对象 |
| **返回值** | 成功返回 `D3D11ComputeShaderPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:2887 |

### 13.2 setComputeShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setComputeShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的计算着色器，调用 CSSetShader。支持传入 nullptr 解绑着色器 |
| **参数** | `shader` — 引擎使用的计算着色器，可为 nullptr |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2929 |

### 13.3 setCSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置计算着色器的常量缓冲区，调用 CSSetConstantBuffers |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2954 |

### 13.4 setCSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置计算着色器的纹理资源（SRV），调用 CSSetShaderResources |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2961 |

### 13.5 setCSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setCSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置计算着色器的纹理采样器，调用 CSSetSamplers |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2968 |

### 13.6 Dispatch（缺失的接口）

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 接口未定义 |
| **说明** | `RHIContext` 提供了完整的 Compute Shader 创建与资源绑定接口，但**没有定义 Dispatch 接口**，也没有 UAV（Unordered Access View）相关的创建/绑定接口。这意味着当前无法真正执行一次 compute 派发，计算着色器一整套接口目前处于「能绑定但无法运行」的状态 |
| **待办** | ⏸ **暂缓**。若要启用 Compute 管线，需要先在 `RHIContext` 增加 `dispatch(x, y, z)` / `dispatchIndirect` 与 UAV 资源接口，再在 D3D11 后端映射到 `CSSetUnorderedAccessViews` / `Dispatch`。**不单独排期** —— 与 UAV、结构化缓冲、Instanced/Indirect Draw 作为一个完整特性合并立项，范围见 `D3D11-Renderer-Backend-Implementation-Plan.md` §8 |

---

## 14. Shader 编译与反射 (Shader Compilation & Reflection)

### 14.1 compileShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult compileShader(ShaderVariant *shader)` |
| **功能** | 编译着色器。根据 ShaderStage 自动选择 profile（vs_5_0/ps_5_0/cs_5_0/gs_5_0/hs_5_0/ds_5_0），使用 D3DCompile 编译 HLSL 源码，编译后的字节码写回 shader 对象 |
| **参数** | `shader` — 着色器变体对象，包含 HLSL 源码和 ShaderStage 信息 |
| **返回值** | `T3D_OK`，失败返回 `T3D_ERR_D3D11_COMPILE_SHADER` |
| **实现位置** | `D3D11ContextBase::compileShader` (T3DD3D11ContextBase.cpp:66) |
| **备注** | 在 D3D11ContextBase 层实现。Debug 模式下开启调试编译（D3DCOMPILE_DEBUG + SKIP_OPTIMIZATION） |

### 14.2 reflectShaderAllBindings

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)` |
| **功能** | 反射着色器的所有绑定信息。使用 D3DReflect 获取 D3D11_SHADER_DESC，遍历 BoundResources 提取 CBuffer 常量、纹理绑定和采样器绑定信息 |
| **参数** | `shader` — 要反射的着色器（需已编译）；`constantParams` — [out] 常量缓冲区绑定信息（名称/binding/大小/偏移/类型）；`samplerParams` — [out] 纹理采样器绑定信息（名称/纹理 binding/采样器 binding/纹理类型） |
| **返回值** | `T3D_OK` |
| **实现位置** | `D3D11ContextBase::reflectShaderAllBindings` (T3DD3D11ContextBase.cpp:144) |
| **备注** | 原先 `D3D11ContextBase` 与 `D3D11Context` 各有一份功能等价实现，Window 侧那份只是多了注释和一大段早期 `ShaderTexSamplerBinding` 的注释残留。已删除 `D3D11Context` 的 override，统一走基类版本；顺带补上了基类原先在正常返回路径和 `GetDesc` 失败路径上漏放 `pReflection` / `pShaderBlob` 的泄漏 |

### 14.3 reflectSamplerBindings

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams)` |
| **功能** | 运行时反射着色器的纹理采样器绑定信息。遍历已有的 samplerParams，根据着色器字节码反射更新纹理和采样器的 binding 索引 |
| **参数** | `shader` — 要反射的着色器（RHI shader 已创建）；`samplerParams` — [in,out] 已有的纹理采样参数，函数会更新其 binding 索引 |
| **返回值** | `T3D_OK`，失败返回 `T3D_ERR_D3D11_CREATE_BLOB` / `T3D_ERR_D3D11_SHADER_REFLECTION` / `T3D_ERR_D3D11_GET_SHADER_DESC` |
| **实现位置** | `D3D11ContextBase::reflectSamplerBindings` (T3DD3D11ContextBase.cpp:321) |
| **备注** | 原先基类是空实现、完整实现只在 `D3D11Context`，导致 Console 后端的离线反射会丢掉 sampler 绑定。因为它只依赖 `ID3D11ShaderReflection` 不依赖 `ID3D11Device`，已整体下沉到基类并删掉 Window 侧的 override，Console 后端自动获得该能力。顺带修掉了原实现在 `GetDesc` 失败路径上漏放 `pReflection` / `pShaderBlob` 的泄漏 |

---

## 15. 图元与绘制 (Primitive & Draw)

### 15.1 setPrimitiveType

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPrimitiveType(PrimitiveType primitive)` |
| **功能** | 设置渲染图元类型（三角形列表、三角形带、线条列表等），调用 IASetPrimitiveTopology |
| **参数** | `primitive` — 图元类型枚举值 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2975 |

### 15.2 render (indexed)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)` |
| **功能** | 根据上下文设置好的资源和状态进行带索引的绘制，调用 DrawIndexed |
| **参数** | `indexCount` — 索引数量；`startIndex` — 索引缓冲区中的起始位置；`baseVertex` — 基础顶点偏移，该值会被加到每个索引值上再用来索引顶点缓冲区 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2990 |

### 15.3 render (non-indexed)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult render(uint32_t vertexCount, uint32_t startVertex)` |
| **功能** | 根据上下文设置好的资源和状态进行不带索引的绘制，调用 Draw |
| **参数** | `vertexCount` — 顶点数量；`startVertex` — 顶点缓冲区中的起始位置 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:3003 |

### 15.4 Instanced Draw（缺失的接口）

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 接口未定义 |
| **说明** | `RHIContext` 只定义了 `Draw` / `DrawIndexed` 两种绘制方式，没有实例化绘制（`DrawInstanced` / `DrawIndexedInstanced`）和间接绘制（`DrawIndirect`）接口。大批量重复几何体（草、粒子、植被）只能逐个 draw call |
| **待办** | ⏸ **暂缓**，与 Compute / UAV 合并立项，见 `D3D11-Renderer-Backend-Implementation-Plan.md` §8。Indirect Draw 本身就依赖 UAV / 结构化缓冲，无法先行 |
| **待办** | 属于 RHI 层接口扩展，需要先在 `RHIContext` 定义再由各后端实现，优先级低于现有 blit / 资源创建的缺口 |

---

## 16. 状态重置 (Reset)

### 16.1 reset

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult reset()` |
| **功能** | 清除所有状态并还原之前 setRenderTarget 时由 `backupRenderState` 备份的完整 D3D11 管线状态。包括：裁剪矩形、视口、渲染目标、深度模板、所有 6 个着色器阶段的 SRV/ConstantBuffer/Sampler/Shader、输入布局、顶点和索引缓冲区、图元拓扑、混合/深度模板/光栅化状态，并逐个 Release 备份时 AddRef 的 COM 对象 |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:3016 |
| **备注** | 函数开头旧版实现的大段注释代码已删除 |

---

## 17. 数据传输 (Blit & Copy)

> 四个 `blit` 重载现在都走同一条路径：在 RHI 线程上把源和目标各解析成一个 `BlitEndpoint`（资源指针 + SRV/RTV/DSV + 尺寸/格式/采样数），再交给 `doBlit` 按兼容性依次尝试 `CopyResource` → `ResolveSubresource` → 全屏四边形绘制，都不适用时返回 `T3D_ERR_D3D11_INCOMPATIBLE_COPY` 并把两端的描述打进日志。

### 17.1 blit (RenderTarget → RenderTarget)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源渲染目标传输图像数据到目标渲染目标。窗口型取 SwapChain BackBuffer，纹理型取 attachment 0 |
| **参数** | `src` — 源渲染目标；`dst` — 目标渲染目标；`srcOffset` — 源偏移（3D 偏移，按资源维度取值）；`size` — 传输大小（3D 体积，Vector3::ZERO 表示全部复制）；`dstOffset` — 目标偏移 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:3124 |
| **备注** | SwapChain 的 `BufferUsage` 只带 `DXGI_USAGE_RENDER_TARGET_OUTPUT` 没有 `DXGI_USAGE_SHADER_INPUT`，所以 BackBuffer 作为源时拿不到 SRV，走不了全屏四边形路径。跨格式或需要缩放的「窗口 → 其它」传输会明确报 `T3D_ERR_D3D11_INCOMPATIBLE_COPY`，而不是静默出错 |

### 17.2 blit (Texture → RenderTarget)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源纹理传输图像数据到目标渲染目标 |
| **参数** | `src` — 源纹理；`dst` — 目标渲染目标；`srcOffset` — 源偏移；`size` — 传输大小（Vector3::ZERO 表示全部复制）；`dstOffset` — 目标偏移 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:3156 |

源纹理类型的支持情况（由 `resolveBlitEndpoint(Texture *, ...)` 统一处理，T3DD3D11Context.cpp:3635）：

| 源纹理类型 | 实现状态 |
|----------|----------|
| `TT_2D` / `TT_2D_ARRAY` | ✅ 整资源拷贝 + 区域拷贝 + 全屏四边形 |
| `TT_RENDER_TEXTURE` | ✅ 同上，作为源时 MSAA 会先 `ResolveSubresource` 到解析副本 |
| `TT_1D` | ✅ 整资源拷贝 + 区域拷贝（SRV 维度为 1D，不参与全屏四边形） |
| `TT_3D` | ✅ 整资源拷贝 + 区域拷贝（`D3D11_BOX` 的 front/back 取 srcOffset.z / size.z） |
| `TT_CUBE` / `TT_CUBE_ARRAY` | ⚠️ 只支持整资源与区域直接拷贝；不提供 SRV，因为全屏四边形路径需要专门的 cube 采样 shader。需要缩放时会明确报错而不是静默走空指针 |
| 其它 | ✅ 返回 `T3D_ERR_D3D11_UNSUPPORTED_OPERATION` 并打日志 |

### 17.3 blit (RenderTarget → Texture)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源渲染目标传输图像数据到目标纹理 |
| **参数** | `src` — 源渲染目标；`dst` — 目标纹理；`srcOffset` — 源偏移；`size` — 传输大小；`dstOffset` — 目标偏移 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:3188 |
| **备注** | 目标纹理若是 `Usage::kImmutable` 会返回 `T3D_ERR_D3D11_INVALID_USAGE`；普通 2D 纹理没有 RTV，跨格式/缩放场景会报 `T3D_ERR_D3D11_INCOMPATIBLE_COPY` |

### 17.4 blit (Texture → Texture)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源纹理传输图像数据到目标纹理 |
| **参数** | `src` — 源纹理；`dst` — 目标纹理；`srcOffset` — 源偏移；`size` — 传输大小；`dstOffset` — 目标偏移 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:3220 |

### 17.5 copyBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)` |
| **功能** | 从源缓冲区复制数据到目标缓冲区。整段复制走 `CopyResource`，带偏移走 `CopySubresourceRegion` + `D3D11_BOX` |
| **参数** | `src` — 源渲染缓冲区；`dst` — 目标渲染缓冲区；`srcOffset` — 源缓冲区起始偏移（默认 0）；`size` — 复制数据的大小（默认 0 表示从 srcOffset 起的剩余部分）；`dstOffset` — 目标缓冲区起始偏移（默认 0） |
| **返回值** | `T3D_OK`，失败返回 `T3D_ERR_D3D11_UNSUPPORTED_OPERATION` / `T3D_ERR_D3D11_INVALID_USAGE` / `T3D_ERR_D3D11_INCOMPATIBLE_COPY` / `T3D_ERR_INVALID_PARAM` |
| **实现位置** | T3DD3D11Context.cpp:3252 |
| **备注** | 只接受 VertexBuffer / IndexBuffer / ConstantBuffer，纹理类资源会被拒绝并提示改走 `blit`。边界校验用 `ID3D11Buffer::GetDesc` 拿到的真实 `ByteWidth`，而不是引擎侧 CPU 镜像的大小。线性缓冲的 `D3D11_BOX` 只有 X 方向有意义，其余维度填 0..1 |

### 17.6 writeBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer)` |
| **功能** | 写 GPU 缓冲区。通过 Map/memcpy/Unmap 将 CPU 端数据写入 GPU 端缓冲区。支持全部 6 种缓冲区类型（VertexBuffer/IndexBuffer/PixelBuffer1D-3D/PixelBufferCubemap/ConstantBuffer），内部深拷贝数据以支持多线程命令队列 |
| **参数** | `renderBuffer` — GPU 缓冲区（必须绑定 CPUAccessWrite 且 Usage 为 kDynamic）；`buffer` — CPU 端数据缓冲区；`discardWholeBuffer` — 是否丢弃旧数据（true 时使用 WRITE_DISCARD，false 时使用 WRITE_NO_OVERWRITE） |
| **返回值** | `T3D_OK`，失败返回 `T3D_ERR_D3D11_INVALID_USAGE` / `T3D_ERR_D3D11_MAP_RESOURCE` |
| **实现位置** | T3DD3D11Context.cpp:3353 |

---

## 18. 帧命令 (Frame Commands)

### 18.1 beginRender

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult beginRender()` |
| **功能** | 开始一帧的渲染命令录制。Vulkan 后端执行 waitFence + acquireNextImage + beginCommandBuffer；D3D11 后端按设计不需要此操作 |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.h:586（内联空实现） |

### 18.2 endRender

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult endRender()` |
| **功能** | 结束一帧的渲染命令录制并提交。Vulkan 后端执行 endCommandBuffer + queueSubmit；D3D11 后端按设计不需要此操作 |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.h:587（内联空实现） |

### 18.3 beginPass

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult beginPass()` |
| **功能** | 开始一个渲染 Pass。Vulkan 后端执行 vkCmdBeginRenderPass；D3D11 后端按设计不需要此操作。应在 clear 操作之后、draw call 之前调用 |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.h:589（内联空实现） |

### 18.4 endPass

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult endPass()` |
| **功能** | 结束一个渲染 Pass。Vulkan 后端执行 vkCmdEndRenderPass；D3D11 后端按设计不需要此操作。应在所有 draw call 之后、blit/reset 之前调用 |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.h:590（内联空实现） |

---

## 19. 非 RHIContext 纯虚接口（有默认实现）

### 19.1 getNativeContext

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `void* getNativeContext() const` |
| **功能** | 获取主渲染 context 的原生句柄。GL 后端返回 HGLRC/GLXContext，D3D11 按设计返回 nullptr |
| **参数** | 无 |
| **返回值** | `nullptr`（D3D11 不需要） |
| **实现位置** | 基类默认实现 (T3DRHIContext.h:640) |

### 19.2 restoreNativeContext

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `void restoreNativeContext()` |
| **功能** | 恢复主窗口的渲染 context（multi-viewport 子窗口渲染后调用）。D3D11 不需要 context 切换 |
| **参数** | 无 |
| **返回值** | 无 |
| **实现位置** | 基类默认实现 (T3DRHIContext.h:645) |

---

## 20. D3D11Context 专有接口（非 RHIContext 虚接口）

### 20.1 init

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult init()` |
| **功能** | 创建 ID3D11Device / ID3D11DeviceContext（feature level 依次尝试 11_1 / 11_0 / 10_1 / 10_0），Debug 构建下额外获取 ID3D11Debug，随后调用 `setupBlitQuad()` |
| **返回值** | `T3D_OK`，失败返回 `T3D_ERR_D3D11_CREATE_FAILED` |
| **实现位置** | T3DD3D11Context.cpp:98，由 `D3D11Context::create()` (T3DD3D11Context.cpp:46) 自动调用 |
| **备注** | `D3D11Renderer::init()` 里 `// ret = mContext->init();` 的注释残留已删除，改成检查 `create()` 的返回值：为空时返回 `T3D_ERR_D3D11_CREATE_FAILED` 并打日志，而不是把 nullptr context 揣着往下走 |

### 20.2 swapBackBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult swapBackBuffer(D3D11RenderWindow *renderWindow)` |
| **功能** | 呈现后缓冲，调用 `IDXGISwapChain::Present(SyncInterval, 0)` |
| **返回值** | `T3D_OK`，失败返回 `T3D_ERR_D3D11_PRESENT` |
| **实现位置** | T3DD3D11Context.cpp:344 |
| **备注** | SyncInterval 不再硬编码：`createRenderWindow` 按 `RenderWindowDesc::IsVsync` 把 `D3D11RenderWindow::PresentSyncInterval` 置为 1 或 0，Present 时取该值。引擎侧的开关是 `Settings::renderSettings::vsync`（`T3DAgent.cpp` 里写进 `param.IsVsync`） |

### 20.3 resizeRenderWindow

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult resizeRenderWindow(D3D11RenderWindow *rw, uint32_t w, uint32_t h)` |
| **功能** | 改变渲染窗口大小。释放 RTView/BackBuffer/DSView/DSBuffer，调用 `ResizeBuffers`，再重建 BackBuffer 和 RenderTargetView |
| **参数** | `rw` — 要改变大小的渲染窗口对象；`w` / `h` — 新宽高 |
| **返回值** | `T3D_OK`，失败返回 `T3D_ERR_D3D11_RESIZE_BUFFERS` |
| **实现位置** | T3DD3D11Context.cpp:886 |

### 20.4 resizeRenderTexture

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult resizeRenderTexture(RenderTexture *rt, uint32_t w, uint32_t h)`（现为 `RHIContext` 虚接口） |
| **功能** | 按渲染纹理当前的描述重建底层 GPU 资源：先解绑管线上的残留引用并释放 Texture2D / RTView / DSView / SRView / ResolveTex，再调 `buildRenderTextureResources` 按新描述重建 |
| **参数** | `rt` — 要改变大小的渲染纹理对象；`w` — 新宽度；`h` — 新高度 |
| **返回值** | `T3D_OK`，参数非法或描述不匹配返回 `T3D_ERR_INVALID_PARAM` |
| **实现位置** | T3DD3D11Context.cpp:922；释放逻辑见 `releaseRenderTextureResources` (T3DD3D11Context.cpp:788) |
| **备注** | 引擎侧入口是 `RenderTexture::resize`（T3DRenderTexture.cpp:74）：它先改自己的 `mDesc`，再调进 RHI。因为 `PixelBufferT::mDesc` 存的是指向纹理描述的指针，改 `RenderTexture::mDesc` 等于同时改了 PixelBuffer 的描述，后端只负责重建、不碰引擎侧描述。RHI 侧会再校验一次描述与入参是否一致 |
| **备注** | 释放前必须把管线上可能残留的 RTV/SRV 绑定解掉，否则 D3D11 会保留内部引用，资源不会真正销毁，重建后还可能读到旧内容 |

### 20.5 resizeRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult resizeRenderTarget(RenderTarget *rt, uint32_t w, uint32_t h)`（现为 `RHIContext` 虚接口） |
| **功能** | 改变渲染目标及其全部附件的大小。窗口型分派到 `resizeRenderWindow` 重建 SwapChain，纹理型逐个调用颜色附件与 DepthStencil 的 `RenderTexture::resize` |
| **参数** | `rt` — 要改变大小的渲染目标；`w` — 新宽度；`h` — 新高度 |
| **返回值** | `T3D_OK`，任一附件失败立即返回对应错误码 |
| **实现位置** | T3DD3D11Context.cpp:958 |

### 20.6 内部辅助（均已实现）

| 函数 | 状态 | 位置 | 说明 |
|------|------|------|------|
| `setupBlitQuad` | ✅ | T3DD3D11Context.cpp:186 | 创建 blit 用的全屏 quad VB / InputLayout / VS / PS / Sampler / Blend / DepthStencil / Rasterizer 状态。VB 用 `D3D11_USAGE_DYNAMIC`，供 `blitRegion` 每次改写 UV |
| `backupRenderState` | ✅ | T3DD3D11Context.cpp:1005 | 完整备份 6 个着色器阶段的全部管线状态，并解绑残留 SRV/RTV 避免 HAZARD 警告 |
| `clearDepthStencilView` | ✅ | T3DD3D11Context.cpp:1348 | `clearDepth` / `clearDepthStencil` 共用；外挂的 RenderTexture 优先，没有时退回窗口自带的 DSV |
| `buildSubresourceData` | ✅ | T3DD3D11Context.cpp:1959 | 把扁平像素数据按 `[arraySlice][mipLevel]` 切分为 D3D11 子资源数组，逐层算 pitch 并做数据量校验 |
| `buildRenderTextureResources` / `releaseRenderTextureResources` | ✅ | T3DD3D11Context.cpp:521 / 788 | 渲染纹理的资源创建与释放，`createRenderTexture` 与 `resizeRenderTexture` 共用 |
| `blitRegion` | ✅ | T3DD3D11Context.cpp:3449 | 通过全屏 quad 绘制做区域拷贝，按 `srcOffset` / `srcSize` 改写顶点 UV、按 `dstOffset` / `dstSize` 设 viewport，自动保存与还原周边状态 |
| `describeD3DResource` | ✅ | T3DD3D11Context.cpp:3583 | 用资源自身的 `GetDesc` 填 BlitEndpoint 的尺寸/格式/采样数，能正确覆盖 MSAA 解析副本、BackBuffer 等场景 |
| `resolveBlitEndpoint`（Texture / RenderTarget 两个重载） | ✅ | T3DD3D11Context.cpp:3635 / 3733 | 把纹理或渲染目标解析成统一的 `BlitEndpoint` |
| `isDirectCopyCompatible` / `doBlit` | ✅ | T3DD3D11Context.cpp:3786 / 3804 | 判断能否用 D3D11 拷贝 API 直传，以及在两个端点间按兼容性选择传输路径 |
| `getD3DResource` | ✅ | T3DD3D11Context.cpp:3876 | 从 `RenderBuffer` 取出底层 `ID3D11Resource` |
| `setSamplers` / `setPixelBuffers` / `setConstantBuffers` | ✅ | — | 6 个着色器阶段共用的资源绑定实现，通过成员函数指针分发 |

---

## 21. 被注释禁用的代码

### 21.1 setupInternalCBuffers（已删除）

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已删除 |
| **原位置** | T3DD3D11Context.cpp:163-199（函数定义）、T3DD3D11Context.cpp:155（`init()` 中的调用）、T3DD3D11Context.h:625（声明）、T3DD3D11Context.cpp:441-449（配套的注释版 `setWorldTransform`） |
| **原功能** | 创建引擎内部使用的 PerFrame 和 PerDraw 常量缓冲区，用于传递每帧/每次绘制的变换矩阵等公共数据 |
| **处理** | 属于早期设计残留。引擎常量缓冲已统一走 `createConstantBuffer` + `setVSConstantBuffers` 通用路径由材质系统管理，这套绕过 RHI 抽象的裸 D3D11 内建 CBuffer 无用途，已整体删除。方案见 `D3D11-Renderer-Backend-Implementation-Plan.md` §7 |
| **处理** | 配套的 `setConstantBuffer` 单缓冲版本（原 T3DD3D11Context.cpp:369 与头文件里的声明）在删除注释版 `setWorldTransform` 后已无调用者，一并删除 |

### 21.2 窗口 DepthStencil 缓冲创建（已恢复）

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已恢复启用 |
| **位置** | T3DD3D11Context.cpp:814（`createRenderWindow` 私有重载内） |
| **功能** | 为 SwapChain 创建配套的深度模板纹理和 DepthStencilView |
| **处理** | 取消注释并做了三处加固：尺寸与 `SampleDesc` 改为从 BackBuffer 的实际描述反查；DSV 显式指定描述（MSAA 时用 `TEXTURE2DMS` 维度）；创建 DSV 失败时释放已创建的纹理。同时 `setRenderTarget` 在没有外挂 RenderTexture 时会退回窗口自带的 DSV，`clearDepth` / `clearDepthStencil` 也通过新的 `clearDepthStencilView` 走同一套回退逻辑 |

### 21.3 D3D11RenderWindow::init（已删除）

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已删除 |
| **原位置** | T3DD3D11RenderWindow.cpp:68-124（函数定义）、T3DD3D11RenderWindow.h:52（声明） |
| **原功能** | 旧版的渲染窗口初始化流程（MSAA 检查 / SwapChain / RTView / DSView 创建） |
| **处理** | 整个函数体被注释后只剩 `return true`，实际初始化职责早已迁移到 `D3D11Context::createRenderWindow`。函数与声明一并删除，`create()` 现在只负责 new 出空壳对象并注明资源由谁创建 |

### 21.4 reset 中的旧版还原代码（已清理）

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已删除 |
| **原位置** | `reset()` 与 `backupRenderState()` 开头 |
| **原功能** | 早期只备份/还原单个 slot 的简化版状态管理 |
| **处理** | 已被完整版实现取代的纯注释残留，整体删除；同时删掉了 `doBlit` 上线后再无调用者的 `blitAll` |

---

## 22. Console 后端 (D3D11ConsoleContext)

`source/Plugins/Renderer/Direct3D11/Console/` 下是一套面向无窗口/无头（headless）场景的 D3D11 后端，与 Window 版本共享 `D3D11ContextBase`（即 `setViewProjectionTransform` / `compileShader` / `reflectShaderAllBindings` / `reflectSamplerBindings` 四个接口有实际功能）。

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 定位已明确（按设计为 null backend） |
| **位置** | T3DD3D11ConsoleContext.cpp |
| **说明** | 除 `createRenderWindow` 返回一个空的 `D3D11ConsoleWindow` 外，其余全部 RHIContext 接口均为单行空实现，不创建任何 D3D11 设备与资源 |
| **结论** | 这是一个刻意的 null backend，用于让引擎在无 GPU 上下文的命令行环境（`BundleBuilder` / `BuiltinGenerator` 等资源导入与 shader 预编译工具）下跑通。**不应该按 Window 后端的标准去「补全」它** |
| **处理** | 已在 `D3D11ConsoleContext` 的类注释里写明定位与有效接口范围；所有返回 `nullptr` 的资源创建接口加了 `T3D_LOG_WARNING`，让工具链里的误用在日志里立刻可见，而不是静默拿到 nullptr 后在别处崩 |
| **处理** | `reflectSamplerBindings` 已从 `D3D11Context` 下沉到 `D3D11ContextBase`，Console 后端现在能做完整的 sampler binding 反射，离线编译产物的元数据不再缺失 |
| **备注** | 若将来离线工具确实需要真实设备（例如验证纹理能否创建），正确做法是用 `D3D_DRIVER_TYPE_NULL` 或 WARP 软件设备初始化真正的 `D3D11Context`，而不是往 null backend 里塞实现 |

---

## 汇总统计

### RHIContext 接口（含 3 个带默认实现的非纯虚接口，共 78 个）

| 状态 | 数量 | 接口 |
|------|------|------|
| ✅ 已完成 | 71 | 全部核心渲染、资源创建与数据传输接口 |
| ⚠️ 部分完成 | 0 | — |
| ❌ 未实现 | 0 | — |
| 🔇 按设计为空 | 7 | `beginRender`、`endRender`、`beginPass`、`endPass`、`getNativeContext`、`restoreNativeContext`、`getDepthRemapMatrix` |

> `blit(Texture → *)` 对 Cubemap 只支持直接拷贝、不支持带缩放的全屏四边形路径，这是缺一个 cube 采样 shader，不是接口缺口；不支持的组合都会明确返回错误码并打日志。

### D3D11Context 专有接口（5 个）

| 状态 | 数量 | 接口 |
|------|------|------|
| ✅ 已完成 | 5 | `init`、`swapBackBuffer`、`resizeRenderWindow`、`resizeRenderTexture`、`resizeRenderTarget` |

> `resizeRenderTexture` / `resizeRenderTarget` 已提升为 `RHIContext` 虚接口（默认返回 `T3D_ERR_NOT_IMPLEMENT`），引擎侧通过 `RenderTexture::resize` 统一入口调用，其它后端可按需 override。

### 被注释禁用的代码

| 位置 | 状态 |
|------|------|
| 窗口 DepthStencil 创建 | ✅ 已恢复启用并加固，见 §21.2 |
| `reset` / `backupRenderState` 旧版实现 | ✅ 已删除，见 §21.4 |
| `D3D11RenderWindow::init` | ✅ 已删除，见 §21.3 |

> `setupInternalCBuffers` + 注释版 `setWorldTransform` + `setConstantBuffer` 单缓冲版本已删除，见 §21.1。至此 D3D11 后端已无被注释禁用的代码块。

### RHI 层缺失的接口（非 D3D11 后端问题，⏸ 暂缓）

| 缺口 | 影响 |
|------|------|
| Compute `dispatch` + UAV 绑定 | Compute Shader 一整套接口无法真正执行 |
| Instanced / Indirect Draw | 大批量重复几何体只能逐个 draw call |

> 这两项**不单独排期**，与 UAV、结构化缓冲一起作为完整特性合并立项，范围见 `D3D11-Renderer-Backend-Implementation-Plan.md` §8。

---

## 待办优先级

### 已完成

| 优先级 | 接口 | 说明 |
|--------|------|------|
| **P0 - 修复缺陷** | `setVertexShader` / `setComputeShader` | ✅ 补上 nullptr 检查，与其他 setXxxShader 行为一致 |
| **P0 - 修复缺陷** | `createPixelBuffer2D` 的 subresource 数组 | ✅ 改由 `buildSubresourceData` 逐 slice / 逐 mip 构建并做数据量校验，越界读已消除 |
| **P0 - 修复缺陷** | 窗口自带 DepthStencil | ✅ 恢复创建并统一了 `setRenderTarget` / `clearDepth*` 的回退路径 |
| **P1 - 核心功能** | 四个 `blit` 重载 + `copyBuffer` | ✅ 统一走 `resolveBlitEndpoint` + `doBlit`，不支持的组合明确报错 |
| **P1 - 窗口管理** | `resizeRenderTexture` / `resizeRenderTarget` | ✅ 已实现并提升为 `RHIContext` 虚接口，引擎侧入口为 `RenderTexture::resize` |
| **P2 - 资源创建** | `createPixelBuffer1D` / `createPixelBuffer3D` | ✅ 已实现，`D3D11PixelBuffer3D` 类与 `D3D11Mapping::get(PixelBuffer3DDesc)` 一并补全 |
| **P2 - 资源创建** | `createPixelBufferCubemap` 支持 mipmap | ✅ 支持完整 mip 链与 Cubemap Array |
| **P2 - 架构清理** | `reflectSamplerBindings` 下沉到 `D3D11ContextBase` | ✅ 已下沉，Console 后端获得该能力，顺带修掉一处 COM 泄漏 |
| **P3 - 定位确认** | Console 后端 | ✅ 确认为永久 null backend，已加类注释与误用警告日志 |
| **P3 - 架构清理** | 合并两份重复的 `reflectShaderAllBindings` | ✅ 删除 `D3D11Context` 的 override，统一走基类，并补上基类的 COM 泄漏 |
| **P3 - 代码清理** | `D3D11RenderWindow::init`、`setConstantBuffer` 单缓冲版本、`D3D11Renderer::init` 冗余注释 | ✅ 三处全部删除，`D3D11Renderer::init` 改为检查 `create()` 返回值 |
| **P3 - 可配置性** | `swapBackBuffer` 的 VSync | ✅ SyncInterval 改由 `RenderWindowDesc::IsVsync` 决定 |

### 待办

| 优先级 | 接口 | 原因 |
|--------|------|------|
| **P2 - 功能补齐** | Cubemap 的带缩放 blit | 全屏四边形路径需要一个 cube 采样 shader，当前只支持整资源/区域直接拷贝 |
| **⏸ 暂缓** | Compute `dispatch` + UAV、Instanced / Indirect Draw | RHI 层接口缺口，与 UAV、结构化缓冲合并立项，见 Plan §8 |
