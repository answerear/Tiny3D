# GL4 渲染后端接口实现状态

> 本文档基于 `RHIContext` 纯虚接口定义，逐一分析 `GL4Context`（Window 版本）的实现完成度。
>
> - **接口定义**：`source/Core/Include/RHI/T3DRHIContext.h`
> - **GL4 基类实现**：`source/Plugins/Renderer/OpenGL4/Base/Source/T3DGL4ContextBase.cpp`
> - **GL4 Window 实现**：`source/Plugins/Renderer/OpenGL4/Window/Source/T3DGL4Context.cpp`
> - **GL4 Window 头文件**：`source/Plugins/Renderer/OpenGL4/Window/Include/T3DGL4Context.h`

---

## 实现状态图例

| 标记 | 含义 |
|------|------|
| ✅ 已完成 | 功能完整实现 |
| ⚠️ 部分完成 | 有条件分支但仅实现了部分分支，或缺少边界处理 |
| ❌ 未实现 | 函数体为空或直接返回空值/T3D_OK，无实际逻辑 |
| 🔇 按设计为空 | GL4 后端按设计意图不需要实现（如 Vulkan 专用的 begin/endRender），或当前未支持的 Shader 阶段 |
| 💬 被注释禁用 | 代码已写但被注释掉，功能未生效 |

---

## 1. 变换 (Transform)

### 1.1 setViewProjectionTransform

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat)` |
| **功能** | 设置视图变换矩阵和投影变换矩阵。引擎投影矩阵产生 [-1,1] NDC 深度，但 GL4 后端使用 `glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE)` 将深度范围设为 [0,1]，因此需通过 conversionMat 将 Z 从 [-1,1] 重映射到 [0,1]。当渲染到 FBO 时，额外翻转投影矩阵 Y 轴（Unity 策略），使 FBO 中的画面方向与 D3D11 一致 |
| **参数** | `viewMat` — 视图变换矩阵；`projMat` — 投影变换矩阵 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:434 |
| **备注** | 在 GL4Context 层直接实现（非基类）。渲染到 FBO 时设置 `mProjectionFlipped = true`，后续 `setRasterizerState` 会据此交换 Front/Back CullFace |

---

## 2. 渲染目标 (Render Target)

### 2.1 createRenderWindow

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIRenderTargetPtr createRenderWindow(RenderWindow *renderWindow)` |
| **功能** | 创建 RHI 渲染窗口。内部调用 `GL4RenderWindow::create()` 创建平台 GL Context（WGL/GLX）+ MSAA 像素格式 + Core Profile 4.5 上下文。当 RHI 线程启用时，自动将 GL Context 从主线程转移到 RHI 线程 |
| **参数** | `renderWindow` — 引擎渲染窗口对象，包含窗口句柄、宽高、全屏模式、MSAA 等描述信息 |
| **返回值** | 成功返回 `GL4RenderWindowPtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:476 |

### 2.2 createRenderTexture

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2D *buffer)` |
| **功能** | 创建 RHI 渲染纹理。根据像素格式自动判断创建颜色 RT 或深度/模板 RT，支持 MSAA（额外创建 GLResolveTex + GLResolveFBO）和非 MSAA 两条路径。深度纹理支持 D24S8 和 D32FS8 格式 |
| **参数** | `buffer` — 引擎像素缓冲区对象，包含宽高、像素格式、MSAA 描述等信息 |
| **返回值** | 成功返回 `GL4PixelBuffer2DPtr`（内含 GLTexture/GLFBO/GLDepthRBO/GLResolveTex/GLResolveFBO），失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:531 |

### 2.3 setRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setRenderTarget(RenderTarget *renderTarget)` |
| **功能** | 设置当前渲染目标。根据 RenderTarget 类型分别处理：Window 类型绑定默认 FBO (0)；Texture 类型绑定颜色 FBO 并支持外部深度纹理挂载、MRT 多目标绑定（最多 8 个 Color Attachment）、depth-only FBO |
| **参数** | `renderTarget` — 渲染目标对象，可以是窗口类型或纹理类型 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:721 |

### 2.4 resetRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult resetRenderTarget()` |
| **功能** | 清除当前渲染目标绑定，调用 `glBindFramebuffer(GL_FRAMEBUFFER, 0)` |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:837 |

---

## 3. 视口 (Viewport)

### 3.1 setViewport

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setViewport(const Viewport &viewport)` |
| **功能** | 设置视口区域。根据当前 RenderTarget 类型（Window 或 Texture）自动获取宽高，按 viewport 的归一化比例计算实际视口大小。当无引擎 RenderTarget 绑定时（如 ImGui 子 viewport），有 fallback 路径通过 `glGetIntegerv(GL_VIEWPORT)` 查询当前尺寸 |
| **参数** | `viewport` — 视口对象，包含 Left/Top/Width/Height 归一化比例值 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:858 |

---

## 4. 清除操作 (Clear)

### 4.1 clearColor

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult clearColor(const ColorRGB &color)` |
| **功能** | 用指定颜色填充当前渲染目标的 framebuffer，调用 `glClearColor` + `glClear(GL_COLOR_BUFFER_BIT)` |
| **参数** | `color` — RGB 颜色值（alpha 固定为 1.0） |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:943 |

### 4.2 clearDepth

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult clearDepth(Real depth)` |
| **功能** | 用指定深度值填充当前渲染目标的 depth buffer（仅清除深度，不清除模板），调用 `glClearDepth` + `glClear(GL_DEPTH_BUFFER_BIT)` |
| **参数** | `depth` — 深度值（通常为 1.0） |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:964 |
| **备注** | 当 `mCurrentRenderTarget == nullptr` 时直接返回 `T3D_OK` |

### 4.3 clearDepthStencil

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult clearDepthStencil(Real depth, uint32_t stencil)` |
| **功能** | 用指定深度值和模板值同时填充当前渲染目标的 depth buffer 和 stencil buffer |
| **参数** | `depth` — 深度值；`stencil` — 模板值 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:988 |

---

## 5. 渲染状态 (Render State)

### 5.1 createBlendState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIBlendStatePtr createBlendState(BlendState *state)` |
| **功能** | 创建 RHI 颜色混合状态对象。在主线程将引擎 BlendDesc 映射为 `GL4BlendStateData` POD 结构体（srcRGB/dstRGB/srcAlpha/dstAlpha/opRGB/opAlpha/colorMask），通过 `ENQUEUE_UNIQUE_COMMAND` 存入 GL4BlendState |
| **参数** | `state` — 引擎颜色混合状态对象，包含 SrcBlend/DestBlend/BlendOp/ColorMask 等描述 |
| **返回值** | 成功返回 `GL4BlendStatePtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1013 |
| **备注** | 当前仅读取 `RenderTargetStates[0]`，不支持独立 RT 混合 |

### 5.2 createDepthStencilState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIDepthStencilStatePtr createDepthStencilState(DepthStencilState *state)` |
| **功能** | 创建 RHI 深度模板状态对象。将引擎 DepthStencilDesc 映射为 `GL4DepthStencilStateData`，支持前后面独立 Stencil 操作 |
| **参数** | `state` — 引擎深度模板状态对象 |
| **返回值** | 成功返回 `GL4DepthStencilStatePtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1057 |

### 5.3 createRasterizerState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIRasterizerStatePtr createRasterizerState(RasterizerState *state)` |
| **功能** | 创建 RHI 光栅化状态对象。将引擎 RasterizerDesc 映射为 `GL4RasterizerStateData`，包含 FillMode/CullMode/FrontFace/Scissor/DepthClip/DepthBias/MSAA |
| **参数** | `state` — 引擎光栅化状态对象 |
| **返回值** | 成功返回 `GL4RasterizerStatePtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1103 |

### 5.4 createSamplerState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHISamplerStatePtr createSamplerState(SamplerState *state)` |
| **功能** | 创建 RHI 纹理采样状态对象。使用 `glGenSamplers` 创建真正的 GL Sampler Object，设置 Wrap/Filter/Anisotropy/LOD/Comparison/BorderColor 等全部参数 |
| **参数** | `state` — 引擎纹理采样状态对象 |
| **返回值** | 成功返回 `GL4SamplerStatePtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1144 |

### 5.5 setBlendState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setBlendState(BlendState *state)` |
| **功能** | 设置颜色混合状态。调用 `glEnable/glDisable(GL_BLEND)` + `glBlendFuncSeparate` + `glBlendEquationSeparate` + `glColorMask` |
| **参数** | `state` — 混合状态对象（内含已创建的 RHI 资源） |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1241 |

### 5.6 setDepthStencilState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setDepthStencilState(DepthStencilState *state)` |
| **功能** | 设置深度模板状态。调用 `glEnable/glDisable(GL_DEPTH_TEST)` + `glDepthFunc` + `glDepthMask` + `glStencilFuncSeparate` + `glStencilOpSeparate` |
| **参数** | `state` — 深度模板状态对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1276 |

### 5.7 setRasterizerState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setRasterizerState(RasterizerState *state)` |
| **功能** | 设置光栅化状态。调用 `glPolygonMode` + `glCullFace` + `glFrontFace` + Scissor/DepthClamp/PolygonOffset。当 `mProjectionFlipped` 为 true（渲染到 FBO）时，自动交换 Front/Back CullFace |
| **参数** | `state` — 光栅化状态对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1325 |

---

## 6. 顶点输入 (Vertex Input)

### 6.1 createVertexDeclaration

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIVertexDeclarationPtr createVertexDeclaration(VertexDeclaration *decl)` |
| **功能** | 创建 RHI 顶点格式对象。调用 `glGenVertexArrays` 创建 VAO |
| **参数** | `decl` — 顶点格式对象，包含顶点属性列表 |
| **返回值** | 成功返回 `GL4VertexDeclarationPtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1391 |
| **备注** | 与 D3D11 不同：D3D11 使用 InputLayout（需 VS 字节码），GL4 使用 VAO（仅需 `glGenVertexArrays`），顶点属性在 `setVertexBuffers` 时延迟配置 |

### 6.2 setVertexDeclaration

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVertexDeclaration(VertexDeclaration *decl)` |
| **功能** | 设置顶点格式，调用 `glBindVertexArray(VAO)`。同时将 decl 缓存到 `mPendingVertexDecl`，在后续 `setVertexBuffers` 时配置顶点属性 |
| **参数** | `decl` — 顶点格式对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1415 |

---

## 7. 缓冲区 (Buffer)

### 7.1 createVertexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIVertexBufferPtr createVertexBuffer(VertexBuffer *buffer)` |
| **功能** | 创建 RHI 顶点缓冲区对象。调用 `glGenBuffers` + `glBufferData(GL_ARRAY_BUFFER)` |
| **参数** | `buffer` — 引擎的顶点缓冲区对象，包含缓冲大小、Usage 和初始数据 |
| **返回值** | 成功返回 `GL4VertexBufferPtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1440 |

### 7.2 setVertexBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets)` |
| **功能** | 设置渲染用的顶点缓冲对象。支持多个缓冲区绑定到不同 slot。若有 `mPendingVertexDecl`，在此处延迟配置顶点属性（`glEnableVertexAttribArray` + `glVertexAttribPointer` / `glVertexAttribIPointer`） |
| **参数** | `startSlot` — 缓冲区起始插槽；`buffers` — 顶点缓冲对象数组；`strides` — 每个缓冲区的顶点跨度；`offsets` — 每个缓冲区的起始偏移 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1472 |

### 7.3 createIndexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIIndexBufferPtr createIndexBuffer(IndexBuffer *buffer)` |
| **功能** | 创建 RHI 索引缓冲对象。调用 `glGenBuffers` + `glBufferData(GL_ELEMENT_ARRAY_BUFFER)` |
| **参数** | `buffer` — 引擎的索引缓冲对象 |
| **返回值** | 成功返回 `GL4IndexBufferPtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1528 |

### 7.4 setIndexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setIndexBuffer(IndexBuffer *buffer)` |
| **功能** | 设置渲染用的索引缓冲对象，调用 `glBindBuffer(GL_ELEMENT_ARRAY_BUFFER)`。自动映射索引类型为 GL 枚举 |
| **参数** | `buffer` — 索引缓冲对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1560 |

### 7.5 createConstantBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIConstantBufferPtr createConstantBuffer(ConstantBuffer *buffer)` |
| **功能** | 创建 RHI 常量缓冲对象（UBO）。调用 `glGenBuffers` + `glBufferData(GL_UNIFORM_BUFFER)` |
| **参数** | `buffer` — 引擎的常量缓冲区对象 |
| **返回值** | 成功返回 `GL4ConstantBufferPtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1585 |

### 7.6 createPixelBuffer1D

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBuffer1DPtr createPixelBuffer1D(PixelBuffer1D *buffer)` |
| **功能** | 创建 RHI 1D 像素缓冲区对象（1D 纹理）。调用 `glGenTextures` + `glTexImage1D` 并设置默认滤波 |
| **参数** | `buffer` — 引擎 1D 像素缓冲区对象 |
| **返回值** | 成功返回 `GL4PixelBuffer1DPtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1617 |
| **备注** | 与 D3D11 不同：D3D11 后端的 createPixelBuffer1D 未实现（返回空对象），GL4 后端已完整实现 |

### 7.7 createPixelBuffer2D

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBuffer2DPtr createPixelBuffer2D(PixelBuffer2D *buffer)` |
| **功能** | 创建 RHI 2D 像素缓冲区对象（2D 纹理）。调用 `glGenTextures` + `glTexImage2D` + `glGenerateMipmap` |
| **参数** | `buffer` — 引擎 2D 像素缓冲区对象 |
| **返回值** | 成功返回 `GL4PixelBuffer2DPtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1654 |

### 7.8 createPixelBuffer3D

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBuffer3DPtr createPixelBuffer3D(PixelBuffer3D *buffer)` |
| **功能** | 创建 RHI 3D 像素缓冲区对象（3D 纹理/体积纹理）。调用 `glGenTextures` + `glTexImage3D` |
| **参数** | `buffer` — 引擎 3D 像素缓冲区对象 |
| **返回值** | 成功返回 `GL4PixelBuffer3DPtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1694 |
| **备注** | 与 D3D11 不同：D3D11 后端的 createPixelBuffer3D 未实现（返回 nullptr），GL4 后端已完整实现 |

---

## 8. 顶点着色器 (Vertex Shader)

### 8.1 createVertexShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIShaderPtr createVertexShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 顶点着色器对象。将 GLSL 源码通过 `glCreateShader(GL_VERTEX_SHADER)` + `glShaderSource` + `glCompileShader` 编译为 GL Shader 对象，含完整的编译错误日志输出 |
| **参数** | `shader` — 引擎着色器变体对象，包含 GLSL 源码 |
| **返回值** | 成功返回 `GL4VertexShaderPtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1733 |

### 8.2 setVertexShader

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成 |
| **签名** | `TResult setVertexShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的顶点着色器。内部删除旧 Program，创建新 Program 并 attach shader，标记 `mProgramDirty = true` |
| **参数** | `shader` — 引擎使用的顶点着色器对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1786 |
| **问题** | 未处理 `shader == nullptr` 的情况。`setPixelShader` 有 nullptr 检查并解绑，但 `setVertexShader` 缺失，传入 nullptr 会导致崩溃 |

### 8.3 setVSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置顶点着色器的常量缓冲区。委托 `stageConstantBuffers()` 将 cbuffer 名→GL buffer handle 存入 `mPendingUBOs`，在 `render()` 时 link 后统一绑定 |
| **参数** | `startSlot` — 常量缓冲区起始插槽（GL4 中通过名称匹配而非 slot 索引）；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1817 |

### 8.4 setVSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置顶点着色器的纹理资源。委托 `bindPixelBuffers()` 调用 `glActiveTexture` + `glBindTexture` |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1822 |

### 8.5 setVSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置顶点着色器的纹理采样器。委托 `bindSamplers()` 调用 `glBindSampler` |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1827 |

---

## 9. 像素着色器 (Pixel Shader)

### 9.1 createPixelShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIShaderPtr createPixelShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 像素着色器对象。使用 `glCreateShader(GL_FRAGMENT_SHADER)` + 编译，含错误日志 |
| **参数** | `shader` — 引擎着色器变体对象 |
| **返回值** | 成功返回 `GL4PixelShaderPtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1836 |

### 9.2 setPixelShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPixelShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的像素着色器。支持传入 nullptr 解绑。将 shader attach 到当前 Program，标记 `mProgramDirty = true` |
| **参数** | `shader` — 引擎使用的像素着色器对象，可为 nullptr |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1888 |

### 9.3 setPSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置像素着色器的常量缓冲区，委托 `stageConstantBuffers()` |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1924 |

### 9.4 setPSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置像素着色器的纹理资源，委托 `bindPixelBuffers()` |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1929 |

### 9.5 setPSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置像素着色器的纹理采样器，委托 `bindSamplers()` |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1934 |

---

## 10. Hull 着色器 (Tessellation - Hull Shader)

### 10.1 createHullShader

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `RHIShaderPtr createHullShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 曲面细分着色器对象（Hull Shader / Tessellation Control Shader） |
| **参数** | `shader` — 引擎着色器变体对象 |
| **返回值** | 返回空的 `GL4HullShaderPtr` 对象（未创建 GL shader） |
| **实现位置** | T3DGL4Context.cpp:1943 |
| **备注** | 代码注释标记 "not supported in OpenGL 3.3"，但实际目标是 GL 4.5 Core Profile（已支持 Tessellation）。属于尚未实现，非 API 不支持 |

### 10.2 setHullShader

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult setHullShader(ShaderVariant *shader)` |
| **功能** | 设置 Hull 着色器 |
| **参数** | `shader` — Hull 着色器对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1949 |

### 10.3 setHSConstantBuffers / setHSPixelBuffers / setHSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | 同 D3D11 对应接口 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1950-1952 |

---

## 11. Domain 着色器 (Tessellation - Domain Shader)

### 11.1 createDomainShader

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `RHIShaderPtr createDomainShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 域着色器对象（Domain Shader / Tessellation Evaluation Shader） |
| **参数** | `shader` — 引擎着色器变体对象 |
| **返回值** | 返回空的 `GL4DomainShaderPtr` 对象（未创建 GL shader） |
| **实现位置** | T3DGL4Context.cpp:1958 |
| **备注** | 同 Hull Shader，代码注释标记 "not supported in OpenGL 3.3"，实际 GL 4.5 已支持 |

### 11.2 setDomainShader

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult setDomainShader(ShaderVariant *shader)` |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1964 |

### 11.3 setDSConstantBuffers / setDSPixelBuffers / setDSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:1965-1967 |

---

## 12. 几何着色器 (Geometry Shader)

### 12.1 createGeometryShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIShaderPtr createGeometryShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 几何着色器对象。使用 `glCreateShader(GL_GEOMETRY_SHADER)` + 编译，含错误日志 |
| **参数** | `shader` — 引擎着色器变体对象 |
| **返回值** | 成功返回 `GL4GeometryShaderPtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1973 |

### 12.2 setGeometryShader

| 项目 | 内容 |
|------|------|
| **状态** | 💬 被注释禁用 |
| **签名** | `TResult setGeometryShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的几何着色器 |
| **参数** | `shader` — 引擎使用的几何着色器 |
| **返回值** | `T3D_OK`（空实现） |
| **实现位置** | T3DGL4Context.cpp:2025 |
| **问题** | 实现代码已写完（attach shader 到 Program 的完整逻辑），但整段被注释掉（line 2028-2048），函数体只剩 `return T3D_OK`。GS 的创建可以成功，但设置绑定无法生效 |

### 12.3 setGSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置几何着色器的常量缓冲区，委托 `stageConstantBuffers()` |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:2053 |

### 12.4 setGSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置几何着色器的纹理资源，委托 `bindPixelBuffers()` |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:2058 |

### 12.5 setGSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setGSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置几何着色器的纹理采样器，委托 `bindSamplers()` |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:2063 |

---

## 13. 计算着色器 (Compute Shader)

### 13.1 createComputeShader

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `RHIShaderPtr createComputeShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 计算着色器对象 |
| **参数** | `shader` — 引擎着色器变体对象 |
| **返回值** | 返回空的 `GL4ComputeShaderPtr` 对象（未创建 GL shader） |
| **实现位置** | T3DGL4Context.cpp:2072 |
| **备注** | 代码注释标记 "not supported in OpenGL 3.3"，但 GL 4.3+ 已支持 Compute Shader |

### 13.2 setComputeShader

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult setComputeShader(ShaderVariant *shader)` |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:2078 |

### 13.3 setCSConstantBuffers / setCSPixelBuffers / setCSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:2079-2081 |

---

## 14. Shader 编译与反射 (Shader Compilation & Reflection)

### 14.1 compileShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult compileShader(ShaderVariant *shader)` |
| **功能** | 编译着色器。委托 `glslangCompileAndReflect()`，使用 glslang 库在 CPU 侧解析 GLSL 源码，执行反射提取 Uniform Block 和 Sampler 信息，缓存到 `mReflectionCache` |
| **参数** | `shader` — 着色器变体对象，包含 GLSL 源码和 ShaderStage 信息 |
| **返回值** | `T3D_OK`，失败返回 `T3D_ERR_GL4_SHADER_REFLECTION` |
| **实现位置** | T3DGL4Context.cpp:2085 |
| **备注** | 与 D3D11 不同：D3D11 使用 D3DCompile (HLSL→bytecode)，GL4 使用 glslang (GLSL→CPU 反射)。glslang 当前仅支持 Vertex/Fragment/Geometry 三个阶段 |

### 14.2 reflectShaderAllBindings

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)` |
| **功能** | 反射着色器的所有绑定信息。从 glslang 缓存中提取 Uniform Block 成员（支持 float/vec4/mat4/int/bool/struct 类型）和 Sampler 绑定（支持 1D/2D/3D/Cube/2D_Shadow）。自动处理 SPIRV-Cross 生成的命名约定（`type_` 前缀、`SPIRV_Cross_Combined` 采样器名） |
| **参数** | `shader` — 要反射的着色器（需已编译）；`constantParams` — [out] 常量绑定信息；`samplerParams` — [out] 采样器绑定信息 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:2182 |

### 14.3 reflectSamplerBindings

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams)` |
| **功能** | 运行时反射着色器的纹理采样器绑定信息。更新已有 samplerParams 的 binding 索引 |
| **参数** | `shader` — 要反射的着色器；`samplerParams` — [in,out] 已有的采样参数 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:2312 |

---

## 15. 图元与绘制 (Primitive & Draw)

### 15.1 setPrimitiveType

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPrimitiveType(PrimitiveType primitive)` |
| **功能** | 设置渲染图元类型，将引擎枚举映射为 GL 枚举并存储到 `mPrimitiveType` |
| **参数** | `primitive` — 图元类型枚举值 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:2381 |

### 15.2 render (indexed)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)` |
| **功能** | 带索引的绘制。执行延迟 Program Link（`glLinkProgram` + `glUseProgram` + `bindPendingUniformBlocks` + `setupSamplerBindings`），然后调用 `glDrawElementsBaseVertex`。包含 EBO 安全检查（验证 `GL_ELEMENT_ARRAY_BUFFER_BINDING` 非零） |
| **参数** | `indexCount` — 索引数量；`startIndex` — 索引缓冲区中的起始位置；`baseVertex` — 基础顶点偏移 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:2402 |

### 15.3 render (non-indexed)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult render(uint32_t vertexCount, uint32_t startVertex)` |
| **功能** | 不带索引的绘制。同样执行延迟 Program Link，然后调用 `glDrawArrays` |
| **参数** | `vertexCount` — 顶点数量；`startVertex` — 起始位置 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:2461 |

---

## 16. 状态重置 (Reset)

### 16.1 reset

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult reset()` |
| **功能** | 清除所有状态。解绑 Program/VAO/VBO/EBO/FBO，删除当前 Program，清空 `mPendingUBOs`，重置 dirty 标志 |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:2508 |

---

## 17. 数据传输 (Blit & Copy)

### 17.1 blit (RenderTarget → RenderTarget)

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源渲染目标传输图像数据到目标渲染目标 |
| **参数** | `src` — 源渲染目标；`dst` — 目标渲染目标；`srcOffset` — 源偏移；`size` — 传输大小（Vector3::ZERO 表示全部复制）；`dstOffset` — 目标偏移 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DGL4Context.cpp:2544 |
| **问题** | 函数体仅 `return T3D_OK` + TODO 注释 "implement using glBlitFramebuffer" |

### 17.2 blit (Texture → RenderTarget)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源纹理传输图像数据到目标渲染目标。支持 MSAA Resolve（先 blit MSAA FBO 到 Resolve FBO，再 blit 到目标）。支持 Y 翻转（当 `mProjectionFlipped` 且目标为窗口时）。支持窗口和纹理类型的目标 |
| **参数** | `src` — 源纹理；`dst` — 目标渲染目标；`srcOffset` — 源偏移；`size` — 传输大小；`dstOffset` — 目标偏移 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:2552 |
| **备注** | 当前仅支持 Texture2D 类型的源纹理，其他纹理类型（1D/3D/Cube 等）未处理 |

### 17.3 blit (RenderTarget → Texture)

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源渲染目标传输图像数据到目标纹理 |
| **参数** | `src` — 源渲染目标；`dst` — 目标纹理；`srcOffset` — 源偏移；`size` — 传输大小；`dstOffset` — 目标偏移 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DGL4Context.cpp:2656 |
| **问题** | 函数体仅 `return T3D_OK`，完全没有数据传输逻辑 |

### 17.4 blit (Texture → Texture)

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源纹理传输图像数据到目标纹理 |
| **参数** | `src` — 源纹理；`dst` — 目标纹理；`srcOffset` — 源偏移；`size` — 传输大小；`dstOffset` — 目标偏移 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DGL4Context.cpp:2663 |
| **问题** | 函数体仅 `return T3D_OK`，完全没有数据传输逻辑 |

### 17.5 copyBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)` |
| **功能** | 从源缓冲区复制数据到目标缓冲区 |
| **参数** | `src` — 源渲染缓冲区；`dst` — 目标渲染缓冲区；`srcOffset` — 源起始偏移（默认 0）；`size` — 复制大小（默认 0 表示全部）；`dstOffset` — 目标起始偏移（默认 0） |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DGL4Context.cpp:2670 |
| **问题** | 函数体仅 `return T3D_OK` + TODO 注释 "implement using glCopyBufferSubData" |

### 17.6 writeBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成 |
| **签名** | `TResult writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer)` |
| **功能** | 写 GPU 缓冲区。支持多种缓冲区类型（VertexBuffer/IndexBuffer/ConstantBuffer/PixelBuffer2D）。Buffer 类型使用 DSA（`glNamedBufferData`/`glNamedBufferSubData`）避免污染 VAO 的 EBO 绑定。内部深拷贝数据以支持多线程命令队列 |
| **参数** | `renderBuffer` — GPU 缓冲区；`buffer` — CPU 端数据缓冲区；`discardWholeBuffer` — 是否丢弃旧数据（true 时使用 GL_DYNAMIC_DRAW） |
| **返回值** | `T3D_OK`，失败返回 `T3D_ERR_GL4_INVALID_USAGE` |
| **实现位置** | T3DGL4Context.cpp:2678 |
| **问题** | Buffer 类型（VBO/IBO/UBO）的写入路径完整且正确。但纹理分支（PixelBuffer2D）的 `glTexSubImage2D` 调用中 width 和 height 参数写死为 0，导致纹理数据无法正确写入 |

---

## 18. 帧命令 (Frame Commands)

### 18.1 beginRender

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult beginRender()` |
| **功能** | 开始一帧的渲染命令录制。Vulkan 后端执行 waitFence + acquireNextImage + beginCommandBuffer；GL4 后端按设计不需要此操作 |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.h:151（内联空实现） |

### 18.2 endRender

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult endRender()` |
| **功能** | 结束一帧的渲染命令录制并提交；GL4 后端按设计不需要此操作 |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.h:152（内联空实现） |

### 18.3 beginPass

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult beginPass()` |
| **功能** | 开始一个渲染 Pass；GL4 后端按设计不需要此操作 |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.h:154（内联空实现） |

### 18.4 endPass

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult endPass()` |
| **功能** | 结束一个渲染 Pass；GL4 后端按设计不需要此操作 |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.h:155（内联空实现） |

---

## 19. 原生上下文接口 (Native Context)

### 19.1 getNativeContext

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `void* getNativeContext() const` |
| **功能** | 获取主渲染 context 的原生句柄。Windows 返回 `HGLRC`，Linux 返回 `GLXContext` |
| **参数** | 无 |
| **返回值** | `mSavedGLContext`（主窗口的 GL 上下文） |
| **实现位置** | T3DGL4Context.cpp:2786 |
| **备注** | 与 D3D11 不同：D3D11 返回 nullptr（按设计为空），GL4 返回真实的 GL Context 句柄 |

### 19.2 restoreNativeContext

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `void restoreNativeContext()` |
| **功能** | 恢复主窗口的渲染 context（multi-viewport 子窗口渲染后调用），调用 `wglMakeCurrent` / `glXMakeCurrent` |
| **参数** | 无 |
| **返回值** | 无 |
| **实现位置** | T3DGL4Context.cpp:2799 |

---

## 20. GL4Context 专有接口（非 RHIContext 纯虚接口）

### 20.1 resizeRenderWindow

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult resizeRenderWindow(GL4RenderWindow *rw, uint32_t w, uint32_t h)` |
| **功能** | 改变渲染窗口的大小。更新 GL4RenderWindow 的 mWidth/mHeight，调用 `glViewport(0, 0, w, h)` |
| **参数** | `rw` — GL4 渲染窗口对象；`w` — 新宽度；`h` — 新高度 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:411 |

---

## 21. 被注释禁用的功能

### 21.1 setGeometryShader 实现代码

| 项目 | 内容 |
|------|------|
| **状态** | 💬 被注释禁用 |
| **位置** | T3DGL4Context.cpp:2028-2048 |
| **功能** | 将几何着色器 attach 到当前 Program 的完整实现：获取 GLShaderHandle → 若 Program 不存在则创建 → glAttachShader → GL_CHECK_ERROR |
| **问题** | 整段实现被注释掉，函数体只剩 `return T3D_OK`。`createGeometryShader` 可以成功创建 GS 对象，但无法通过 `setGeometryShader` 绑定到渲染管线 |

---

## 汇总统计

| 状态 | 数量 | 接口 |
|------|------|------|
| ✅ 已完成 | 42 | 大部分核心渲染接口 |
| ⚠️ 部分完成 | 2 | `setVertexShader`（缺 nullptr 检查）、`writeBuffer`（纹理分支参数错误） |
| ❌ 未实现 | 4 | `blit(RT→RT)`、`blit(RT→Tex)`、`blit(Tex→Tex)`、`copyBuffer` |
| 🔇 按设计为空 | 15 | Hull(5)、Domain(5)、Compute(5)、beginRender、endRender、beginPass、endPass |
| 💬 被注释禁用 | 1 | `setGeometryShader` 实现代码 |

### 与 D3D11 后端的差异

| 项目 | D3D11 状态 | GL4 状态 | 说明 |
|------|-----------|---------|------|
| createPixelBuffer1D | ❌ 未实现 | ✅ 已完成 | GL4 已完整实现 1D 纹理 |
| createPixelBuffer3D | ❌ 未实现 | ✅ 已完成 | GL4 已完整实现 3D 纹理 |
| getNativeContext | 🔇 按设计为空 | ✅ 已完成 | GL4 返回真实 GL Context |
| restoreNativeContext | 🔇 按设计为空 | ✅ 已完成 | GL4 用于 multi-viewport context 切换 |
| setVertexShader | ⚠️ 缺 nullptr 检查 | ⚠️ 缺 nullptr 检查 | 两个后端同样的问题 |
| setComputeShader | ⚠️ 缺 nullptr 检查 | 🔇 按设计为空 | GL4 整个 CS 阶段未实现 |
| blit(Tex→RT) 纹理类型 | ⚠️ 仅实现 TT_2D/TT_RENDER_TEXTURE | ✅ 已完成（仅 2D） | GL4 实现完整但仅支持 2D 源 |

### 待办优先级

| 优先级 | 接口 | 原因 |
|--------|------|------|
| **P0 - 修复缺陷** | `setVertexShader` | 缺少 nullptr 检查，传入 null 会崩溃，与 setPixelShader 不一致 |
| **P0 - 修复缺陷** | `writeBuffer` 纹理分支 | `glTexSubImage2D` 的 width/height 参数写死为 0，纹理数据无法写入 |
| **P1 - 核心功能** | `blit(RT→RT)` / `blit(RT→Tex)` / `blit(Tex→Tex)` | 数据传输是渲染管线核心功能，已有 TODO 标注 |
| **P1 - 核心功能** | `copyBuffer` | 缓冲区复制是常用操作，可用 `glCopyBufferSubData` 实现 |
| **P1 - 核心功能** | `setGeometryShader` | 取消注释即可启用，实现代码已写完 |
| **P2 - Shader 阶段** | Hull / Domain / Compute Shader | GL 4.5 已支持 Tessellation 和 Compute，按需实现 |
| **P2 - 增强** | `createBlendState` 独立 RT 混合 | 当前仅读取 RenderTargetStates[0]，不支持多 RT 独立混合 |
| **P3 - 注释修正** | Hull/Domain/Compute 的 "not supported in OpenGL 3.3" | 注释不准确，实际目标是 GL 4.5 Core Profile |
