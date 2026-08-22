# D3D11 渲染后端接口实现状态

> 本文档基于 `RHIContext` 纯虚接口定义，逐一分析 `D3D11Context`（Window 版本）的实现完成度。
>
> - **接口定义**：`source/Core/Include/RHI/T3DRHIContext.h`
> - **D3D11 基类实现**：`source/Plugins/Renderer/Direct3D11/Base/Source/T3DD3D11ContextBase.cpp`
> - **D3D11 Window 实现**：`source/Plugins/Renderer/Direct3D11/Window/Source/T3DD3D11Context.cpp`
> - **D3D11 Window 头文件**：`source/Plugins/Renderer/Direct3D11/Window/Include/T3DD3D11Context.h`

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

---

## 2. 渲染目标 (Render Target)

### 2.1 createRenderWindow

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIRenderTargetPtr createRenderWindow(RenderWindow *renderWindow)` |
| **功能** | 创建 RHI 渲染窗口。内部创建 DXGI SwapChain、BackBuffer 纹理和 RenderTargetView，支持 MSAA |
| **参数** | `renderWindow` — 引擎渲染窗口对象，包含窗口句柄、宽高、全屏模式、MSAA 等描述信息 |
| **返回值** | 成功返回 `D3D11RenderWindowPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:453 |

### 2.2 createRenderTexture

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2D *buffer)` |
| **功能** | 创建 RHI 渲染纹理。根据像素格式自动判断创建颜色 RT 或深度模板 RT，支持 MSAA（会额外创建 ResolveTexture）和 shaderReadable 深度纹理 |
| **参数** | `buffer` — 引擎像素缓冲区对象，包含宽高、像素格式、MSAA 描述、shaderReadable 等信息 |
| **返回值** | 成功返回 `D3D11PixelBuffer2DPtr`（内含 D3DTexture/D3DRTView/D3DDSView/D3DSRView），失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:582 |

### 2.3 setRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setRenderTarget(RenderTarget *renderTarget)` |
| **功能** | 设置当前渲染目标。根据 RenderTarget 类型（Window/Texture）调用不同的内部重载，自动备份当前 D3D11 管线状态，支持 MRT（Multiple Render Targets） |
| **参数** | `renderTarget` — 渲染目标对象，可以是窗口类型或纹理类型 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1125 |

### 2.4 resetRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult resetRenderTarget()` |
| **功能** | 清除当前渲染目标绑定，将 OMSetRenderTargets 设为 null |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1156 |

---

## 3. 视口 (Viewport)

### 3.1 setViewport

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setViewport(const Viewport &viewport)` |
| **功能** | 设置视口区域。根据当前 RenderTarget 类型（Window 或 Texture）自动获取宽高，然后按 viewport 的归一化比例计算实际视口大小 |
| **参数** | `viewport` — 视口对象，包含 Left/Top 等归一化偏移值 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1171 |

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
| **实现位置** | T3DD3D11Context.cpp:1229 |

### 4.2 clearDepth

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult clearDepth(Real depth)` |
| **功能** | 用指定深度值填充当前渲染目标的 depth buffer（仅清除深度，不清除模板） |
| **参数** | `depth` — 深度值（通常为 1.0） |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1298 |

### 4.3 clearDepthStencil

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult clearDepthStencil(Real depth, uint32_t stencil)` |
| **功能** | 用指定深度值和模板值同时填充当前渲染目标的 depth buffer 和 stencil buffer |
| **参数** | `depth` — 深度值；`stencil` — 模板值 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1318 |

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
| **实现位置** | T3DD3D11Context.cpp:1345 |

### 5.2 createDepthStencilState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIDepthStencilStatePtr createDepthStencilState(DepthStencilState *state)` |
| **功能** | 创建 RHI 深度模板状态对象。将引擎 DepthStencilDesc 映射为 D3D11_DEPTH_STENCIL_DESC，支持前后面独立 Stencil 操作 |
| **参数** | `state` — 引擎深度模板状态对象，包含 DepthTestEnable/DepthWriteEnable/DepthFunc/StencilEnable/FrontFace/BackFace 等描述 |
| **返回值** | 成功返回 `D3D11DepthStencilStatePtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:1400 |

### 5.3 createRasterizerState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIRasterizerStatePtr createRasterizerState(RasterizerState *state)` |
| **功能** | 创建 RHI 光栅化状态对象。将引擎 RasterizerDesc 映射为 D3D11_RASTERIZER_DESC，支持 FillMode/CullMode/DepthBias/ScissorEnable/MSAA 等配置 |
| **参数** | `state` — 引擎光栅化状态对象 |
| **返回值** | 成功返回 `D3D11RasterizerStatePtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:1454 |

### 5.4 createSamplerState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHISamplerStatePtr createSamplerState(SamplerState *state)` |
| **功能** | 创建 RHI 纹理采样状态对象。将引擎 SamplerDesc 映射为 D3D11_SAMPLER_DESC，支持 Filter/AddressMode/LOD/ComparisonFunc/BorderColor 等配置 |
| **参数** | `state` — 引擎纹理采样状态对象 |
| **返回值** | 成功返回 `D3D11SamplerStatePtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:1503 |

### 5.5 setBlendState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setBlendState(BlendState *state)` |
| **功能** | 设置颜色混合状态，调用 OMSetBlendState |
| **参数** | `state` — 混合状态对象（内含已创建的 RHI 资源） |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1556 |

### 5.6 setDepthStencilState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setDepthStencilState(DepthStencilState *state)` |
| **功能** | 设置深度模板状态，调用 OMSetDepthStencilState |
| **参数** | `state` — 深度模板状态对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1569 |

### 5.7 setRasterizerState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setRasterizerState(RasterizerState *state)` |
| **功能** | 设置光栅化状态，调用 RSSetState |
| **参数** | `state` — 光栅化状态对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1581 |

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
| **实现位置** | T3DD3D11Context.cpp:1593 |

### 6.2 setVertexDeclaration

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVertexDeclaration(VertexDeclaration *decl)` |
| **功能** | 设置顶点格式，调用 IASetInputLayout |
| **参数** | `decl` — 顶点格式对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1649 |

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
| **实现位置** | T3DD3D11Context.cpp:1662 |

### 7.2 setVertexBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets)` |
| **功能** | 设置渲染用的顶点缓冲对象，支持多个缓冲区绑定到不同 slot，调用 IASetVertexBuffers |
| **参数** | `startSlot` — 缓冲区起始插槽，对应 shader 寄存器索引；`buffers` — 顶点缓冲对象数组；`strides` — 每个缓冲区的顶点跨度（一个顶点的字节大小）；`offsets` — 每个缓冲区的起始偏移 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1732 |

### 7.3 createIndexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIIndexBufferPtr createIndexBuffer(IndexBuffer *buffer)` |
| **功能** | 创建 RHI 索引缓冲对象。根据 Usage 和 CPUAccessMode 创建 D3D11 Buffer，支持初始数据和延迟填充 |
| **参数** | `buffer` — 引擎的索引缓冲对象，包含缓冲大小、索引类型、Usage、CPUAccessMode 和可选的初始数据 |
| **返回值** | 成功返回 `D3D11IndexBufferPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:1755 |

### 7.4 setIndexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setIndexBuffer(IndexBuffer *buffer)` |
| **功能** | 设置渲染用的索引缓冲对象，自动映射索引类型为 DXGI_FORMAT，调用 IASetIndexBuffer |
| **参数** | `buffer` — 索引缓冲对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:1825 |

### 7.5 createConstantBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIConstantBufferPtr createConstantBuffer(ConstantBuffer *buffer)` |
| **功能** | 创建 RHI 常量缓冲对象。使用初始数据创建 D3D11 Buffer（BindFlags 为 CONSTANT_BUFFER） |
| **参数** | `buffer` — 引擎的常量缓冲区对象，包含缓冲大小、Usage、CPUAccessMode 和初始数据 |
| **返回值** | 成功返回 `D3D11ConstantBufferPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:1841 |

### 7.6 createPixelBuffer1D

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `RHIPixelBuffer1DPtr createPixelBuffer1D(PixelBuffer1D *buffer)` |
| **功能** | 创建 RHI 1D 像素缓冲区对象（1D 纹理） |
| **参数** | `buffer` — 引擎 1D 像素缓冲区对象 |
| **返回值** | 当前返回空的 `D3D11PixelBuffer1D` 对象（未创建任何 D3D11 纹理资源或视图） |
| **实现位置** | T3DD3D11Context.cpp:1905 |
| **问题** | 只调用了 `D3D11PixelBuffer1D::create()` 就直接返回，没有创建 ID3D11Texture1D 和相应的 SRV。返回的对象内部资源指针全部为 null |

### 7.7 createPixelBuffer2D

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBuffer2DPtr createPixelBuffer2D(PixelBuffer2D *buffer)` |
| **功能** | 创建 RHI 2D 像素缓冲区对象（2D 纹理）。创建 ID3D11Texture2D 和 ID3D11ShaderResourceView |
| **参数** | `buffer` — 引擎 2D 像素缓冲区对象，包含宽高、格式、mipmap、arraySize、采样描述等信息 |
| **返回值** | 成功返回 `D3D11PixelBuffer2DPtr`，失败返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:1913 |

### 7.8 createPixelBuffer3D

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `RHIPixelBuffer3DPtr createPixelBuffer3D(PixelBuffer3D *buffer)` |
| **功能** | 创建 RHI 3D 像素缓冲区对象（3D 纹理/体积纹理） |
| **参数** | `buffer` — 引擎 3D 像素缓冲区对象 |
| **返回值** | 当前直接返回 `nullptr` |
| **实现位置** | T3DD3D11Context.cpp:2000 |
| **问题** | 完全未实现，直接 `return nullptr`，无法创建 3D 纹理资源 |

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
| **实现位置** | T3DD3D11Context.cpp:2007 |

### 8.2 setVertexShader

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成 |
| **签名** | `TResult setVertexShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的顶点着色器，调用 VSSetShader |
| **参数** | `shader` — 引擎使用的顶点着色器对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2049 |
| **问题** | 未处理 `shader == nullptr` 的情况。其他同类接口（setPixelShader/setHullShader/setDomainShader/setGeometryShader）都有 nullptr 检查并将着色器设为 null，但此处缺失，传入 nullptr 会导致崩溃 |

### 8.3 setVSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置顶点着色器的常量缓冲区，调用 VSSetConstantBuffers |
| **参数** | `startSlot` — 常量缓冲区起始插槽，对应 shader 中 cbuffer 寄存器索引；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2064 |

### 8.4 setVSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置顶点着色器的纹理资源（SRV），调用 VSSetShaderResources |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2071 |

### 8.5 setVSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置顶点着色器的纹理采样器，调用 VSSetSamplers |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2078 |

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
| **实现位置** | T3DD3D11Context.cpp:2085 |

### 9.2 setPixelShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPixelShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的像素着色器，调用 PSSetShader。支持传入 nullptr 解绑着色器 |
| **参数** | `shader` — 引擎使用的像素着色器对象，可为 nullptr |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2127 |

### 9.3 setPSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置像素着色器的常量缓冲区，调用 PSSetConstantBuffers |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2152 |

### 9.4 setPSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置像素着色器的纹理资源（SRV），调用 PSSetShaderResources |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2159 |

### 9.5 setPSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置像素着色器的纹理采样器，调用 PSSetSamplers |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2166 |

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
| **实现位置** | T3DD3D11Context.cpp:2172 |

### 10.2 setHullShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setHullShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的曲面细分着色器，调用 HSSetShader。支持传入 nullptr 解绑 |
| **参数** | `shader` — 引擎使用的曲面细分着色器对象，可为 nullptr |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2214 |

### 10.3 setHSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置 Hull 着色器的常量缓冲区，调用 HSSetConstantBuffers |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2239 |

### 10.4 setHSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置 Hull 着色器的纹理资源（SRV），调用 HSSetShaderResources |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2246 |

### 10.5 setHSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setHSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置 Hull 着色器的纹理采样器，调用 HSSetSamplers |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2253 |

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
| **实现位置** | T3DD3D11Context.cpp:2260 |

### 11.2 setDomainShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setDomainShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的域着色器，调用 DSSetShader。支持传入 nullptr 解绑 |
| **参数** | `shader` — 引擎使用的域着色器，可为 nullptr |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2302 |

### 11.3 setDSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置 Domain 着色器的常量缓冲区，调用 DSSetConstantBuffers |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2327 |

### 11.4 setDSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置 Domain 着色器的纹理资源（SRV），调用 DSSetShaderResources |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2334 |

### 11.5 setDSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setDSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置 Domain 着色器的纹理采样器，调用 DSSetSamplers |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2341 |

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
| **实现位置** | T3DD3D11Context.cpp:2348 |

### 12.2 setGeometryShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setGeometryShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的几何着色器，调用 GSSetShader。支持传入 nullptr 解绑 |
| **参数** | `shader` — 引擎使用的几何着色器，可为 nullptr |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2390 |

### 12.3 setGSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置几何着色器的常量缓冲区，调用 GSSetConstantBuffers |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2415 |

### 12.4 setGSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置几何着色器的纹理资源（SRV），调用 GSSetShaderResources |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2422 |

### 12.5 setGSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setGSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置几何着色器的纹理采样器，调用 GSSetSamplers |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2429 |

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
| **实现位置** | T3DD3D11Context.cpp:2436 |

### 13.2 setComputeShader

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成 |
| **签名** | `TResult setComputeShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的计算着色器，调用 CSSetShader |
| **参数** | `shader` — 引擎使用的计算着色器 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2478 |
| **问题** | 未处理 `shader == nullptr` 的情况。其他同类接口（setPixelShader/setHullShader/setDomainShader/setGeometryShader）都有 nullptr 检查并将着色器设为 null，但此处缺失，传入 nullptr 会导致崩溃 |

### 13.3 setCSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置计算着色器的常量缓冲区，调用 CSSetConstantBuffers |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2493 |

### 13.4 setCSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置计算着色器的纹理资源（SRV），调用 CSSetShaderResources |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2500 |

### 13.5 setCSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setCSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置计算着色器的纹理采样器，调用 CSSetSamplers |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2507 |

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
| **实现位置** | T3DD3D11Context.cpp:2514（override 了基类 D3D11ContextBase 的实现） |
| **备注** | D3D11ContextBase 和 D3D11Context 各有一份实现，D3D11Context 的版本包含更多注释说明 |

### 14.3 reflectSamplerBindings

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams)` |
| **功能** | 运行时反射着色器的纹理采样器绑定信息。遍历已有的 samplerParams，根据着色器字节码反射更新纹理和采样器的 binding 索引 |
| **参数** | `shader` — 要反射的着色器（RHI shader 已创建）；`samplerParams` — [in,out] 已有的纹理采样参数，函数会更新其 binding 索引 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2774 |
| **备注** | D3D11ContextBase 基类版本为空实现（直接返回 T3D_OK），D3D11Context 提供了完整实现 |

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
| **实现位置** | T3DD3D11Context.cpp:2850 |

### 15.2 render (indexed)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)` |
| **功能** | 根据上下文设置好的资源和状态进行带索引的绘制，调用 DrawIndexed |
| **参数** | `indexCount` — 索引数量；`startIndex` — 索引缓冲区中的起始位置；`baseVertex` — 基础顶点偏移，该值会被加到每个索引值上再用来索引顶点缓冲区 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2865 |

### 15.3 render (non-indexed)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult render(uint32_t vertexCount, uint32_t startVertex)` |
| **功能** | 根据上下文设置好的资源和状态进行不带索引的绘制，调用 Draw |
| **参数** | `vertexCount` — 顶点数量；`startVertex` — 顶点缓冲区中的起始位置 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2878 |

---

## 16. 状态重置 (Reset)

### 16.1 reset

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult reset()` |
| **功能** | 清除所有状态并还原之前 setRenderTarget 时备份的完整 D3D11 管线状态。包括：渲染目标、深度模板、所有 6 个着色器阶段的 SRV/ConstantBuffer/Sampler/Shader、输入布局、顶点和索引缓冲区、图元拓扑、混合/深度模板/光栅化状态 |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:2891 |

---

## 17. 数据传输 (Blit & Copy)

### 17.1 blit (RenderTarget → RenderTarget)

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源渲染目标传输图像数据到目标渲染目标 |
| **参数** | `src` — 源渲染目标；`dst` — 目标渲染目标；`srcOffset` — 源偏移（3D 偏移，按资源维度取值）；`size` — 传输大小（3D 体积，Vector3::ZERO 表示全部复制）；`dstOffset` — 目标偏移 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DD3D11Context.cpp:3091 |
| **问题** | 函数体仅 `return T3D_OK`，完全没有数据传输逻辑 |

### 17.2 blit (Texture → RenderTarget)

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成 |
| **签名** | `TResult blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源纹理传输图像数据到目标渲染目标。根据 size 是否为 ZERO 自动选择全量复制（CopyResource）或区域复制（通过 blit quad 绘制）。支持 MSAA ResolveSubresource |
| **参数** | `src` — 源纹理；`dst` — 目标渲染目标；`srcOffset` — 源偏移；`size` — 传输大小（Vector3::ZERO 表示全部复制）；`dstOffset` — 目标偏移 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DD3D11Context.cpp:3098 |
| **问题** | 纹理类型 switch 中仅实现了以下类型，其余分支为空 break（被静默忽略）：|

| 纹理类型 | 实现状态 |
|----------|----------|
| `TT_2D` | ✅ 已实现 |
| `TT_RENDER_TEXTURE` | ✅ 已实现（含 MSAA Resolve） |
| `TT_1D` | ❌ 空分支 |
| `TT_2D_ARRAY` | ❌ 空分支 |
| `TT_3D` | ❌ 空分支 |
| `TT_CUBE` | ❌ 空分支 |
| `TT_CUBE_ARRAY` | ❌ 空分支 |

### 17.3 blit (RenderTarget → Texture)

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源渲染目标传输图像数据到目标纹理 |
| **参数** | `src` — 源渲染目标；`dst` — 目标纹理；`srcOffset` — 源偏移；`size` — 传输大小；`dstOffset` — 目标偏移 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DD3D11Context.cpp:3242 |
| **问题** | 函数体仅 `return T3D_OK`，完全没有数据传输逻辑 |

### 17.4 blit (Texture → Texture)

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源纹理传输图像数据到目标纹理 |
| **参数** | `src` — 源纹理；`dst` — 目标纹理；`srcOffset` — 源偏移；`size` — 传输大小；`dstOffset` — 目标偏移 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DD3D11Context.cpp:3249 |
| **问题** | 函数体仅 `return T3D_OK`，完全没有数据传输逻辑 |

### 17.5 copyBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)` |
| **功能** | 从源缓冲区复制数据到目标缓冲区 |
| **参数** | `src` — 源渲染缓冲区；`dst` — 目标渲染缓冲区；`srcOffset` — 源缓冲区起始偏移（默认 0）；`size` — 复制数据的大小（默认 0 表示全部）；`dstOffset` — 目标缓冲区起始偏移（默认 0） |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DD3D11Context.cpp:3256 |
| **问题** | 函数体仅 `return T3D_OK`，完全没有缓冲区复制逻辑 |

### 17.6 writeBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer)` |
| **功能** | 写 GPU 缓冲区。通过 Map/memcpy/Unmap 将 CPU 端数据写入 GPU 端缓冲区。支持多种缓冲区类型（VertexBuffer/IndexBuffer/PixelBuffer1D-3D/ConstantBuffer），内部深拷贝数据以支持多线程命令队列 |
| **参数** | `renderBuffer` — GPU 缓冲区（必须绑定 CPUAccessWrite）；`buffer` — CPU 端数据缓冲区；`discardWholeBuffer` — 是否丢弃旧数据（true 时使用 WRITE_DISCARD，renderBuffer 必须绑定 Usage::kDynamic；false 时使用 WRITE_NO_OVERWRITE） |
| **返回值** | `T3D_OK`，失败返回 `T3D_ERR_D3D11_MAP_RESOURCE` 等 |
| **实现位置** | T3DD3D11Context.cpp:3263 |

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

## 19. 非 RHIContext 虚接口（有默认实现）

### 19.1 getNativeContext

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `void* getNativeContext() const` |
| **功能** | 获取主渲染 context 的原生句柄。GL 后端返回 HGLRC/GLXContext，D3D11 按设计返回 nullptr |
| **参数** | 无 |
| **返回值** | `nullptr`（D3D11 不需要） |
| **实现位置** | 基类默认实现 (T3DRHIContext.h:613) |

### 19.2 restoreNativeContext

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `void restoreNativeContext()` |
| **功能** | 恢复主窗口的渲染 context（multi-viewport 子窗口渲染后调用）。D3D11 不需要 context 切换 |
| **参数** | 无 |
| **返回值** | 无 |
| **实现位置** | 基类默认实现 (T3DRHIContext.h:618) |

---

## 20. D3D11Context 专有接口（非 RHIContext 纯虚接口）

### 20.1 resizeRenderTexture

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult resizeRenderTexture(RenderTexture *rt, uint32_t w, uint32_t h)` |
| **功能** | 改变渲染纹理的大小 |
| **参数** | `rt` — 要改变大小的渲染纹理对象；`w` — 新宽度；`h` — 新高度 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DD3D11Context.cpp:949 |
| **问题** | 函数体仅 `return T3D_OK`，没有重建纹理资源和视图的逻辑 |

### 20.2 resizeRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult resizeRenderTarget(RenderTarget *rt, uint32_t w, uint32_t h)` |
| **功能** | 改变渲染目标的大小 |
| **参数** | `rt` — 要改变大小的渲染目标；`w` — 新宽度；`h` — 新高度 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DD3D11Context.cpp:956 |
| **问题** | 函数体仅 `return T3D_OK`，没有根据 RenderTarget 类型分别处理 resize 的逻辑 |

---

## 21. 被注释禁用的功能

### 21.1 setupInternalCBuffers

| 项目 | 内容 |
|------|------|
| **状态** | 💬 被注释禁用 |
| **位置** | T3DD3D11Context.cpp:163-199 |
| **功能** | 创建引擎内部使用的 PerFrame 和 PerDraw 常量缓冲区（Tiny3DPerFrame / Tiny3DPerDraw），用于传递每帧/每次绘制的变换矩阵等公共数据 |
| **问题** | 整个函数体被注释掉，内部 CBuffer 的创建和管理功能完全未生效。相关的 `mPerFrameCBuffer` 和 `mPerDrawCBuffer` 成员变量也未在当前代码中使用 |

---

## 汇总统计

| 状态 | 数量 | 接口 |
|------|------|------|
| ✅ 已完成 | 43 | 大部分核心渲染接口 |
| ⚠️ 部分完成 | 3 | `setVertexShader`、`setComputeShader`、`blit(Texture→RenderTarget)` |
| ❌ 未实现 | 7 | `createPixelBuffer1D`、`createPixelBuffer3D`、`blit(RT→RT)`、`blit(RT→Tex)`、`blit(Tex→Tex)`、`copyBuffer`、`resizeRenderTexture`、`resizeRenderTarget` |
| 🔇 按设计为空 | 6 | `beginRender`、`endRender`、`beginPass`、`endPass`、`getNativeContext`、`restoreNativeContext` |
| 💬 被注释禁用 | 1 | `setupInternalCBuffers` |

### 待办优先级

| 优先级 | 接口 | 原因 |
|--------|------|------|
| **P0 - 修复缺陷** | `setVertexShader` / `setComputeShader` | 缺少 nullptr 检查，传入 null 会崩溃，与其他 setXxxShader 不一致 |
| **P1 - 核心功能** | `blit(RT→RT)` / `blit(RT→Tex)` / `blit(Tex→Tex)` / `copyBuffer` | 数据传输是渲染管线核心功能 |
| **P1 - 核心功能** | `blit(Tex→RT)` 补全缺失的纹理类型分支 | TT_1D/TT_2D_ARRAY/TT_3D/TT_CUBE/TT_CUBE_ARRAY |
| **P2 - 资源创建** | `createPixelBuffer1D` / `createPixelBuffer3D` | 1D/3D 纹理资源创建缺失 |
| **P2 - 窗口管理** | `resizeRenderTexture` / `resizeRenderTarget` | 窗口大小变化时需要 |
| **P3 - 内部优化** | `setupInternalCBuffers` | 引擎内部 CBuffer 管理，视需求启用 |
