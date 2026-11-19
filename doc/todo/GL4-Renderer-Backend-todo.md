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
| **功能** | 设置深度模板状态。调用 `glEnable/glDisable(GL_DEPTH_TEST)` + `glDepthFunc` + `glDepthMask` + `glStencilFuncSeparate` + `glStencilOpSeparate`。`StencilRef` 取自 `DepthStencilDesc`（2026-09 与 GLES3 同步修复，不再硬编码为 1） |
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

---
---

# 附录 A：完整度复核（2026-11 源码级核验更新）

> **本附录的作用**：上文第 1～21 章及其汇总统计是按早期 `RHIContext` 口径（64 个接口）编写的历史记录。此后 RHI 层经历了多轮扩容——新增了裁剪矩形、独立模板清除（`clearStencil`）、结构化缓冲、Cubemap、实例化/间接绘制、Compute Dispatch/UAV、渲染目标 Resize、GPU 读回（`map`/`unmap`）以及后端能力集查询；同时 GL4 Window 后端在 2026-09 完成了 P0/P1/P2 全量补齐。
>
> 本附录以 **2026-11 当前最新源码**为准，给出全量口径（**97 个虚接口**）下的真实实现状态、修正上文及早期复核中已过时的条目，并盘点剩余的未完成项（如纹理 UAV `glBindImageTexture` 与 `GL4ContextBase` 去重）。上文第 1～21 章保留不动，仅作历史演进参考。
>
> **复核基准**
>
> - 接口定义：`source/Core/Include/RHI/T3DRHIContext.h`
> - 能力集定义：`source/Core/Include/RHI/T3DRHICapabilities.h`
> - GL4 Window 实现：`source/Plugins/Renderer/OpenGL4/Window/Source/T3DGL4Context.cpp`（5650 行）
> - GL4 Window 头文件：`source/Plugins/Renderer/OpenGL4/Window/Include/T3DGL4Context.h`
> - GL4 缓冲资源定义：`source/Plugins/Renderer/OpenGL4/Window/Include/T3DGL4RenderBuffer.h`
> - GL4 基类实现：`source/Plugins/Renderer/OpenGL4/Base/Source/T3DGL4ContextBase.cpp`（1102 行）
> - GL4 Console 实现：`source/Plugins/Renderer/OpenGL4/Console/Source/T3DGL4ConsoleContext.cpp`（343 行）
> - 对标基线：`doc/todo/D3D11-Renderer-Backend-todo.md` + `D3D11Context`（Window 版）
>
> 本附录中未标注文件名的行号均指 `T3DGL4Context.cpp`，并已按 2026-11-18 的源码（提交 `05c4cb34`）重新核对。
>
> **注意**：本附录统计的是「接口是否有真实实现」。2026-11-18 的行为级 review 又发现了一批语义错误、线程竞态与性能问题，详见**附录 D**；A.7 的统计已据此修正。

---

## A.1 接口口径的变化

`RHIContext` 当前共 **97 个虚接口**（不含析构函数）：**92 个纯虚 + 5 个带默认实现**（`getDepthRemapMatrix`、`resizeRenderTexture`、`resizeRenderTarget`、`getNativeContext`、`restoreNativeContext`）。此外 `getCapabilities()` 是非虚 inline getter，读取由后端在 `init()` 中填充的 `mCapabilities` 成员。

上文第 1～21 章**未覆盖**的新增接口共 **26 个**：

| 分类 | 新接口 | 数量 | GL4 Window 当前状态 |
|------|--------|------|---------------------|
| 裁剪 | `setScissorRect` | 1 | ✅ 已完成 |
| 清除 | `clearStencil` | 1 | ✅ 已完成（含 `ClearMaskGuard`） |
| 渲染目标 | `resizeRenderTexture`、`resizeRenderTarget` | 2 | ✅ 已完成 |
| 深度约定 | `getDepthRemapMatrix` | 1 | ✅ 已完成 |
| 资源 | `createStructuredBuffer`、`createPixelBufferCubemap` | 2 | ✅ 已完成 |
| 结构化缓冲绑定 | `setVSStructuredBuffers`、`setPSStructuredBuffers`、`setCSStructuredBuffers` | 3 | ✅ 已完成（支持 `StructuredBuffer` 与 `VB`/`IB` Raw SRV） |
| GPU-driven / Compute | `setCSUnorderedAccessBuffers`、`dispatch`、`dispatchIndirect`、`uavBarrier`、`copyStructureCount` | 5 | ✅ 已完成（同时支持 Buffer SSBO UAV、Atomic Counter 与 1D/2D/3D 纹理 `glBindImageTexture` UAV） |
| 实例化绘制 | `renderIndexedInstanced`、`renderInstanced` | 2 | ✅ 已完成 |
| 间接绘制 | `renderIndexedIndirect`、`renderIndirect` | 2 | ✅ 已完成 |
| GPU 读回 | `map(buffer)`、`map(region)`、`unmap` | 3 | ✅ 已完成（PBO / Staging Buffer） |
| 合计（相比旧 64 接口 + 7 项统计口径调整） | — | **26** | — |

---

## A.2 新增的状态图例

上文的五档图例（✅ / ⚠️ / ❌ / 🔇 / 💬）不足以描述当前代码，因为引擎引入了**正式的「不支持」契约**。本附录使用扩展图例：

| 标记 | 含义 |
|------|------|
| ✅ 已完成 | 功能完整实现 |
| ⚠️ 部分完成 | 有逻辑但缺关键分支、边界处理或存在已知缺陷 |
| ❌ 未实现 | 函数体直接返回 `T3D_OK` / 空对象 / `nullptr`，**且未声明不支持**——调用方拿到"成功"却什么也没发生 |
| 🚧 契约式不支持 | 走 `T3D_RHI_UNSUPPORTED` 宏：断言能力位为 false、打警告日志、返回 `T3D_ERR_NOT_IMPLEMENT`。这是**合规状态**，不是缺陷 |
| 🔇 按设计为空 | GL4 按设计意图不需要实现（如 Vulkan 专用的 begin/endRender） |
| 💬 被注释禁用 | 代码已写但被注释掉，功能未生效 |
| ⛔ 未 override | 沿用 `RHIContext` 基类默认实现，返回 `T3D_ERR_NOT_IMPLEMENT` |

### ❌ 与 🚧 的区别很重要

`T3D_RHI_UNSUPPORTED` 宏定义在 `source/Core/Include/T3DPrerequisites.h:92`：

```cpp
#define T3D_RHI_UNSUPPORTED(capField)                                       \
    do {                                                                    \
        T3D_ASSERT(!getCapabilities().capField);                            \
        T3D_LOG_WARNING(LOG_TAG_RENDER,                                     \
            "%s is not supported by this RHI backend", __FUNCTION__);        \
        return T3D_ERR_NOT_IMPLEMENT;                                       \
    } while (false)
```

它一次做三件事：断言能力位确实为 false（防止「能力位说支持但接口是空壳」）、打警告日志、返回明确的错误码。配合 `RHICapabilities` 的「默认全 false」设计，上层查到不支持就走降级路径，**只损失性能不产生错误结果**。

而 ❌ 类接口（如 `blit(RT→RT)`、`copyBuffer`、Hull/Domain/Compute 的 set 系列）直接返回 `T3D_OK`，上层无法察觉操作被静默丢弃。**这是 GL4 后端当前最需要收口的一类问题**，详见 A.6。

---

## A.3 后端能力集（新增章节）

### A.3.1 `fillCapabilities`

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（能力位取值保守） |
| **签名** | `void fillCapabilities()`（`GL4ContextBase` 专有，非 RHI 接口） |
| **功能** | 在 `init()` 末尾按 GL 版本与实现限制填充 `mCapabilities`，供上层通过 `getCapabilities()` 查询后决定走 GPU 路径还是 CPU 降级路径 |
| **实现位置** | T3DGL4ContextBase.cpp:80（调用点 T3DGL4ContextBase.cpp:59，`GL4Context::init()` 直接转调基类） |
| **备注** | 末尾用 `while (glGetError() != GL_NO_ERROR) {}` 清理 compute limit 查询在低版本 GL 上产生的错误标志，避免污染后续 `GL_CHECK_ERROR` |

### A.3.2 各能力位取值

| 字段 | GL4 Window 取值 | 说明 |
|------|----------------|------|
| `supportsInstancing` | `true` | 实例化与 divisor 是 GL 3.3 核心功能 |
| `supportsBaseInstance` | `major>4 \|\| (major==4 && minor>=2)` | 非零 `startInstance` 需要 GL 4.2 的 `*BaseInstance` 系列 |
| `supportsCompute` | `major>4 \|\| (major==4 && minor>=3)` | GL 4.3+ 真实现 |
| `supportsUnorderedAccess` | 同 compute | SSBO / atomic counter |
| `supportsStructuredBuffer` | 同 compute | `GL_SHADER_STORAGE_BUFFER` |
| `supportsIndirectDraw` | `major>4 \|\| (major==4 && minor>=0)` | GL 4.0+；与 compute 分开判定 |
| `supportsIndirectDispatch` | 同 compute | `glDispatchComputeIndirect` |
| `supportsAppendConsumeBuffer` | 同 compute | 4 字节 counter buffer + `copyStructureCount` |
| `supportsReadback` | `true` | `map` / `unmap` 走 staging PBO |
| `maxDispatchGroupCount[3]` | 从 `GL_MAX_COMPUTE_WORK_GROUP_COUNT` 查询 | `dispatch` 越界校验使用 |
| `maxComputeGroupSize[3]` | 从 `GL_MAX_COMPUTE_WORK_GROUP_SIZE` 查询 | 供上层查询 |
| `maxComputeSharedMemory` | 从 `GL_MAX_COMPUTE_SHARED_MEMORY_SIZE` 查询 | 供上层查询 |
| `maxUnorderedAccessSlots` | 从 `GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS` 查询 | 供上层查询 |

**关键结论（2026-09 已落地）**：`fillCapabilities` 现按版本动态打开能力位——GL 4.3+ 打开 compute / SSBO / UAV / indirect dispatch / append-consume；GL 4.0+ 打开 `supportsIndirectDraw`；`supportsReadback` 恒为 true。对应 RHI 接口已是真实现，不再是空壳 stub。

---

## A.4 新接口的实现状态

### A.4.1 setScissorRect

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height)` |
| **功能** | 设置裁剪矩形。RHI 约定入参为**左上原点、Y 向下**的像素坐标，GL4 内部按当前渲染目标高度翻转为 `glScissor` 的左下原点。无引擎 RenderTarget 绑定时（ImGui 子 viewport）有 `glGetIntegerv(GL_VIEWPORT)` 回退路径 |
| **参数** | `x` / `y` — 矩形左上角像素坐标；`width` / `height` — 矩形宽高 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DGL4Context.cpp:782 |
| **备注** | 须配合 `RasterizerDesc::ScissorEnable = true` 才生效；与 `setViewport` 相互独立。`setViewport` 渲染到窗口时原先没有做同样的 Y 翻转，已于附录 D.1.3 修复 |

### A.4.2 getDepthRemapMatrix

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `const Matrix4& getDepthRemapMatrix() const` |
| **功能** | 返回把光空间 Z 从平台 NDC 范围映射到 [0,1] 的矩阵。GL 系列保持原生 [-1,1] NDC，因此返回 Z 缩放 0.5 + 平移 0.5 的矩阵；D3D11 的投影矩阵已内含 Z remap，用基类默认的单位矩阵 |
| **返回值** | Z remap 矩阵的常引用 |
| **实现位置** | T3DGL4ContextBase.cpp:67 |
| **备注** | 主要供阴影贴图路径把光空间深度写成与采样端一致的范围 |

### A.4.3 createPixelBufferCubemap

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBufferCubemapPtr createPixelBufferCubemap(PixelBufferCubemap *buffer)` |
| **功能** | 创建 RHI 立方体贴图。`glGenTextures` + 逐面 `glTexImage2D`（6 个 `GL_TEXTURE_CUBE_MAP_POSITIVE_X + i`）；`arraySize > 6` 时走 `GL_TEXTURE_CUBE_MAP_ARRAY`；带 `kGPUUnorderedAccess` 时走 `glTexStorage2D/3D`；提供了完整 mip 链则逐级上传，否则按需 `glGenerateMipmap` |
| **参数** | `buffer` — 引擎立方体贴图像素缓冲对象 |
| **返回值** | 成功返回 `GL4PixelBufferCubemapPtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:2137 |
| **备注** | Cubemap 是天空盒的前置依赖，见 `doc/todo/Skybox-Support-Design-todo.md` |

### A.4.4 renderIndexedInstanced

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult renderIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance)` |
| **功能** | 索引实例化绘制。`startInstance == 0` 时走 `glDrawElementsInstancedBaseVertex`；非零时走 GL 4.2 的 `glDrawElementsInstancedBaseVertexBaseInstance` |
| **返回值** | `T3D_OK`；`startInstance != 0` 且 `!mCapabilities.supportsBaseInstance`（GL < 4.2）时返回 `T3D_ERR_NOT_IMPLEMENT` |
| **实现位置** | T3DGL4Context.cpp:3007（能力位检查在 T3DGL4Context.cpp:3010） |
| **备注** | 顶点属性的 instance divisor 在 `setVertexBuffers` 中配置 |

### A.4.5 renderInstanced

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult renderInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance)` |
| **功能** | 非索引实例化绘制，`glDrawArraysInstanced` / `glDrawArraysInstancedBaseInstance` |
| **返回值** | `T3D_OK`；`startInstance != 0` 且 GL < 4.2 时返回 `T3D_ERR_NOT_IMPLEMENT` |
| **实现位置** | T3DGL4Context.cpp:3060（能力位检查在 T3DGL4Context.cpp:3063） |

### A.4.6 结构化缓冲与 GPU-driven 接口（11 个）

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（全部 11 个接口均已完整实现） |
| **接口** | `createStructuredBuffer`、`setVSStructuredBuffers`、`setPSStructuredBuffers`、`setCSStructuredBuffers`、`setCSUnorderedAccessBuffers`、`dispatch`、`dispatchIndirect`、`uavBarrier`、`copyStructureCount`、`renderIndexedIndirect`、`renderIndirect` |
| **实现位置** | T3DGL4Context.cpp:3101-3572 |
| **待确认** | SRV（`set*StructuredBuffers`）与 UAV（`setCSUnorderedAccessBuffers`）都把 `slot` 直接当作 `GL_SHADER_STORAGE_BUFFER` 绑定点，D3D11 的 `t#` / `u#` 两个独立编号空间在 GL 里会撞槽，见附录 D.3.2 |
| **功能** | 1) **能力位守卫**：低版本 GL 不支持时通过 `T3D_RHI_UNSUPPORTED` / `T3D_RHI_UNSUPPORTED_PTR` 返回错误；<br>2) **StructuredBuffer / Raw SRV**：`createStructuredBuffer` 创建 `GL_SHADER_STORAGE_BUFFER`（带 counter/append 时额外创建 4 字节 `GL_ATOMIC_COUNTER_BUFFER`）；`set{VS,PS,CS}StructuredBuffers` 委托 `bindStructuredBuffers`，通过 `getGLBufferHandle` 同时支持 `StructuredBuffer`、`VertexBuffer`、`IndexBuffer` 绑定到 SSBO；<br>3) **Buffer & Texture UAV (`setCSUnorderedAccessBuffers`)**：校验 `kGPUUnorderedAccess` 标志，对线性缓冲（`StructuredBuffer`/`VB`/`IB`）绑定 `GL_SHADER_STORAGE_BUFFER`（及 `GL_ATOMIC_COUNTER_BUFFER`），对纹理资源（`PixelBuffer1D/2D/3D`）通过 `getGLImageInfo` 调用 `glBindImageTexture` 并记录到 `mBoundCSImages`；<br>4) **Compute Dispatch & Indirect**：`dispatch` 含组数非零与硬件上限校验，链接激活 `mCurrentComputeProgram` 后调 `glDispatchCompute` 并恢复 `mCurrentProgram`；`dispatchIndirect` / `renderIndexedIndirect` / `renderIndirect` 统一经 `validateIndirectArgs` 校验 `kGPUIndirectArgs` 与 4 字节对齐；<br>5) **Counter 拷贝与 Barrier**：`copyStructureCount` 通过 `glCopyNamedBufferSubData` 将 `GLCounterBuffer` 的 4 字节计数拷入目标缓冲；`uavBarrier` 调用 `glMemoryBarrier`（含 `GL_PIXEL_BUFFER_BARRIER_BIT`）并解绑 `mBoundCSImages` 中的 image unit |

### A.4.7 resizeRenderTexture / resizeRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult resizeRenderTexture(RenderTexture *rt, uint32_t width, uint32_t height)`<br>`TResult resizeRenderTarget(RenderTarget *rt, uint32_t width, uint32_t height)` |
| **实现位置** | T3DGL4Context.cpp:613（`resizeRenderTexture`）、T3DGL4Context.cpp:648（`resizeRenderTarget`） |
| **功能** | `resizeRenderTexture` 校验非空与描述符尺寸一致性后，在 RHI 线程调用 `releaseRenderTextureResources` + `buildRenderTextureResources` 重建 `GLTexture` / `GLFBO` / `GLDepthRBO` / `GLResolveTex` / `GLResolveFBO`；`resizeRenderTarget` 对窗口类型转调 `resizeRenderWindow`，对纹理类型逐个重建所有颜色 RT 与 DepthStencil RT |

### A.4.8 clearStencil（新增接口）

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult clearStencil(uint32_t stencil)` |
| **实现位置** | T3DGL4Context.cpp:977 |
| **功能** | 仅清除模板缓冲而不动颜色与深度（供 RmlUi 等 UI 裁剪掩码在场景绘制后、后处理前使用）。与 `clearColor` / `clearDepth` / `clearDepthStencil` 一样配备了 `ClearMaskGuard`（T3DGL4Context.cpp:846-900），在 `glClear` 前临时关闭 `GL_SCISSOR_TEST` 并打开全写掩码，结束后恢复管线状态，严格对齐 D3D11 `ClearDepthStencilView` 语义 |

### A.4.9 map / unmap（GPU → CPU 读回，3 个接口）

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `ReadbackHandle map(RenderBuffer *src, size_t offset, size_t size)`<br>`ReadbackHandle map(RenderBuffer *src, const ReadbackRegion &region)`<br>`TResult unmap(ReadbackHandle handle, Buffer &dst)` |
| **实现位置** | T3DGL4Context.cpp:3829（线性缓冲 `map`）、T3DGL4Context.cpp:3887（纹理 `map`）、T3DGL4Context.cpp:4108（`unmap`）、T3DGL4Context.cpp:5529-5647（`allocReadbackRequest` / `finishReadback`） |
| **功能** | 1) `allocReadbackRequest` 强制校验 `kCPURead` 标志与资源类型匹配；<br>2) 线性缓冲 `map`：使用 DSA `glCreateBuffers` + `glNamedBufferData(GL_STREAM_READ)` + `glCopyNamedBufferSubData` 录制拷贝；<br>3) 纹理 `map`：支持 1D / 1D Array / 2D / 2D Array / 3D / Cube / Cube Array，MSAA 2D 源先 `glBlitFramebuffer` resolve 到 `GLResolveTex`，再绑定 `GL_PIXEL_PACK_BUFFER` 调用 `glGetTextureSubImage` 录制子区域读取；<br>4) `unmap`：同步 RHI 线程（`syncRHIThread`），校验 `CopyRecorded` 时序，调用 `glMapNamedBuffer(GL_READ_ONLY)` 拷出紧凑数据后销毁 Staging 缓冲 |
| **注意** | 深度/模板与压缩格式返回 `T3D_ERR_GL4_UNSUPPORTED_OPERATION`。MSAA resolve 之后原先把 `GL_FRAMEBUFFER` 绑回 0 而不恢复原 FBO，已于附录 D.1.6 修复 |

---

## A.5 对上文第 1～21 章历史条目的修正对照

上文第 1～21 章的描述与状态判断已随代码演进而过时，此处给出与当前最新代码（`T3DGL4Context.cpp`）的对照修正：

| 上文章节 | 上文历史说法 | 当前代码实际实现 |
|---------|-------------|-----------------|
| §1.1 `setViewProjectionTransform` | 使用 `glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE)` 并通过 `conversionMat` 将 Z 重映射到 `[0,1]` | ✅ **已改为保持 OpenGL 原生 `[-1,1]` NDC 深度**（不调用 `glClipControl`，T3DGL4Context.cpp:148-174），仅在 `mRenderingToFBO` 时翻转投影矩阵 Y 轴；光空间深度 `[0,1]` 映射改由 `getDepthRemapMatrix()`（T3DGL4ContextBase.cpp:67）提供 |
| §2.4 `resetRenderTarget` | 仅调用 `glBindFramebuffer(GL_FRAMEBUFFER, 0)` | ✅ **已补齐重置 `mRenderingToFBO = false`**（T3DGL4Context.cpp:398-416），避免后续 `setRasterizerState` 残留上一个 FBO 的正反面翻转状态 |
| §3.1 `setViewport` | 按归一化比例计算视口 | ✅ 渲染到窗口时原先未做 Y 翻转，非全屏视口会上下错位，已于附录 D.1.3 修复 |
| §4.1~4.3 `clearColor` / `clearDepth` / `clearDepthStencil` | 直接调 `glClear*` | ✅ **已引入 `ClearMaskGuard`**（T3DGL4Context.cpp:846-900），临时禁用 Scissor 并开启写掩码，防止被前一个 Pass 的 `ColorMask=0` 或 `DepthWrite=false` 拦截 |
| §5.1 / §5.5 `createBlendState` / `setBlendState` | 仅读取 `RenderTargetStates[0]`，不支持独立 RT 混合 | ✅ **已支持 `IndependentBlendEnable`**（T3DGL4Context.cpp:1002-1049、1238-1301），按 `glEnablei` / `glBlendFuncSeparateiARB` / `glBlendEquationSeparateiARB` / `glColorMaski` 逐 RT 配置。三个 `create*State` 把 `glState->data` 写入放进 RHI 命令队列的竞态已于附录 D.1.2 修复 |
| §5.4 `createSamplerState` | ✅ 设置 Wrap/Filter/.../BorderColor 等全部参数 | ✅ BorderColor 的 R/B 分量原先被交换，已于附录 D.1.4 修复 |
| §5.7 `setRasterizerState` | ✅ 调用 `glPolygonMode` + `glCullFace` + ... + DepthClamp | ✅ `DepthClipEnable` 与 `GL_DEPTH_CLAMP` 的映射原先是反的，已于附录 D.1.1 修复。⚠️ `DepthBiasClamp` 与 `AntialiasedLineEnable` 仍未处理，见附录 E.2.3 |
| §7.7 `createPixelBuffer2D` | `glGenTextures` + `glTexImage2D` + `glGenerateMipmap` | ⚠️ **2D 纹理不调用 `glGenerateMipmap`**（T3DGL4Context.cpp:1854-2022），只上传调用方提供的 mip 级别；`mipmaps > 1` 但只给了 mip 0 数据时 `MAX_LEVEL` 被设为 0。1D / 3D 同理，只有 Cubemap 会自动生成，见附录 D.4.3 |
| §8.2 `setVertexShader` | ⚠️ 缺 `nullptr` 检查，传入 null 会崩溃 | ✅ **已修复崩溃**；`nullptr` 分支原先会删除整个 Program，已于附录 D.1.7 / D.2.1 改为只清空本阶段句柄 |
| §10 / §11 Hull & Domain Shader（10 个接口） | 🔇 按设计为空（返回空对象 / `T3D_OK`） | ✅ **已全部真实现**（T3DGL4Context.cpp:2590-2668），使用 `GL_TESS_CONTROL_SHADER` / `GL_TESS_EVALUATION_SHADER` 编译，通过 `attachGraphicsShader` 挂载 |
| §12.2 `setGeometryShader` | 💬 实现代码被注释禁用 | ✅ **已启用**（T3DGL4Context.cpp:2726-2729），委托 `attachGraphicsShader(shader, mCurrentGSVariant, GL_GEOMETRY_SHADER)` |
| §13 Compute Shader（5 个接口） | 🔇 按设计为空 | ✅ **已全部真实现**（T3DGL4Context.cpp:2752-2833），独立维护 `mCurrentComputeProgram` 与 `ensureComputeProgramLinked()` |
| §14.1 `compileShader` (`glslangCompileAndReflect`) | glslang 仅支持 Vertex/Fragment/Geometry 三个阶段 | ✅ **已支持全部 6 个阶段**（含 Hull/Domain/Compute，T3DGL4ContextBase.cpp:447-602），并用实测 GL 硬件上限覆盖 `GetDefaultResources()` |
| §17.1~17.4 四个 `blit` 重载 | 仅 `Tex→RT` 实现，其余 3 个 ❌ 未实现 | ✅ **四个重载全部完成**（T3DGL4Context.cpp:3634-3750），统一走 `resolveBlitEndpoint`（5093、5206）+ `doBlit`（5314-5525），支持 `size == Vector3::ZERO` 整张拷贝、普通纹理临时挂 `mScratchReadFBO`/`mScratchDrawFBO`、以及 MSAA 源直接 resolve。MSAA 源带缩放或翻转时的合法性待验证，见附录 D.3.1 |
| §17.5 `copyBuffer` | ❌ 未实现（仅 TODO 注释） | ✅ **已完成**（T3DGL4Context.cpp:3754-3825），使用 DSA `glCopyNamedBufferSubData`，含越界与 `Usage::kImmutable` 检查 |
| §17.6 `writeBuffer` | ⚠️ 纹理分支 width/height 写死为 0 | ✅ **已修复**（T3DGL4Context.cpp:4115-4438），从各类 PixelBuffer 描述符读取真实宽高与像素格式，且线性缓冲支持 `kStructuredBuffer` |

---

## A.6 GL4 后端第二轮收尾修复记录（2026-11 已全部清零）

在 2026-09 完成第一轮 P0/P1/P2 补齐后，2026-11 源码审计发现的最后 5 项遗留问题均已修复并验证通过：

1. **纹理 UAV 支持（`RWTexture2D` / `glBindImageTexture`，即 T3-1）**：✅ **已完成**
   - `GL4PixelBuffer1D/2D/3D`（及 `GLES3PixelBuffer1D/2D/3D`）新增 `GLInternalFormat` 字段，并在带 `kGPUUnorderedAccess` 时通过 `glTexStorage1D/2D/3D` 创建不可变格式纹理；
   - `setCSUnorderedAccessBuffers` 新增 `getGLImageInfo` 分支，对 1D/2D/3D 纹理调用 `glBindImageTexture` 并记录到 `mBoundCSImages`；
   - `uavBarrier` 补充 `GL_PIXEL_BUFFER_BARRIER_BIT` 并在屏障后自动解绑 `mBoundCSImages`；
   - `ComputeApp` 的 K5（`K5 texture UAV`）已在 OpenGL 4.5 下实测 `PASS`。
2. **`reset()` 状态复位补齐**：✅ **已完成**
   - `GL4Context::reset()`（及 `GLES3Context::reset()`）已补齐 `mRenderingToFBO = false;`、`mProjectionFlipped = false;` 以及 `mBoundCSImages` 解绑清理。
3. **合并 `GL4Context` 与 `GL4ContextBase` 重复实现**：✅ **已完成**
   - 将 Core Profile 4.5 Dummy Context 升级、`fillCapabilities()`、`getDepthRemapMatrix()`、6 阶段 `glslangCompileAndReflect`（含 Compute `local_size` 线程组尺寸反射与 `mGLSLangLimits` 硬件上限抬升）、`reflectShaderAllBindings`、`reflectSamplerBindings` 全部统一收归 `GL4ContextBase`，消除了 `GL4Context` 中约 350 行重复代码；
   - `GL4ConsoleContext` 同步补齐了 `createHullShader`、`createDomainShader`、`createComputeShader` 的真实 `glCompileShader` 实现。
4. **窗口销毁后 GL 上下文悬空导致退出崩溃修复（`makeDummyCurrent`）**：✅ **已完成**
   - `Agent::~Agent()` 先销毁 `RenderWindow`（触发 `GL4RenderWindow::~GL4RenderWindow()` 调用 `wglMakeCurrent(nullptr, nullptr)` 并销毁 `GLContext`），随后再释放残留材质/着色器/缓冲并卸载 `GL4Renderer`；
   - 新增 `GL4ContextBase::makeDummyCurrent()`，在主窗口 `GLContext` 销毁后及 `~GL4Context()` 析构前切回共享了对象空间的 `mDummyHGLRC`，彻底消除了退出时无 GL 上下文调用 `glDelete*` 引发的 `0xC0000005` 崩溃。
5. **清理 `T3DGL4Context.h` 过时注释**：✅ **已完成**

---

## A.7 全量口径汇总统计（97 个 RHIContext 虚接口）

> **2026-11-18 源码实测统计（含附录 D 行为级 review 修正）**：按 `T3DRHIContext.h` 当前全部 **97 个虚接口**统计 `GL4Context`（Window 变体）：
> - 按**接口覆盖**（均已重写并具备核心路径）：**97 / 97 = 100%**
> - 按**细节与边界完整度**：**93 ✅ + 0 ⚠️ + 4 🔇 = 97**。附录 D 的行为级 review 一度把 9 个接口改回 ⚠️，同日已全部修复（见 D.6）。另有非 `RHIContext` 接口 `GL4Renderer::getEditorInfo` 未实现（见附录 C.1.1）
>
> **2026-11-19 更正**：上面的统计只反映「接口是否有真实实现」。对照 D3D11 的实际行为逐项核对后，仍有 7 个会产生错误结果的缺口和一批次要差距，详见**附录 E**。按行为口径，`setRenderTarget`、`createRenderTexture`、`createPixelBuffer2D`、`setCSUnorderedAccessBuffers`、`set*PixelBuffers` / `set*Samplers`（跨阶段）应视为 ⚠️。

| 状态 | 数量 | 接口明细 |
|------|------|----------|
| ✅ 已完成 | **93** | 除下列 4 个 🔇 之外的全部虚接口。其中以下 9 个为附录 D 修复后转回 ✅：`createBlendState`、`createDepthStencilState`、`createRasterizerState`（D.1.2）、`setRasterizerState`（D.1.1）、`createSamplerState`（D.1.4）、`setViewport`（D.1.3）、`setVertexShader`（D.1.7）、`map(region)`（D.1.6）、`writeBuffer`（D.2.2）。四个 `blit` 重载的 MSAA 路径已按 D.3.1 修复；SRV/UAV 槽位按 D.3.2 的调用方约定处理 |
| ⚠️ 部分完成 | **0** | — |
| 🔇 按设计为空 | **4** | `beginRender`、`endRender`、`beginPass`、`endPass` |
| **合计** | **97** | `T3DRHIContext.h` 全部 97 个虚接口（92 纯虚 + 5 虚默认） |

### 换算成完成度

- **接口覆盖率**（✅ + ⚠️ + 🔇）：**97 / 97 = 100%**
- **完全无缺口实现率**（✅ + 🔇）：**97 / 97 = 100%**（附录 D 修复已完成，但尚未跑运行时回归，见 D.6；另有非 `RHIContext` 接口 `GL4Renderer::getEditorInfo` 未实现。Program 重复链接问题已由 D.2.1 的缓存解决）
- **对比 D3D11 基线**：GL4 Window 后端在常规渲染、MRT、MSAA、后处理 Blit、Tessellation、Geometry Shader、Instancing、Indirect Draw/Dispatch、SSBO、纹理数组、纹理 UAV（`RWTexture2D`）、Append/Consume Counter、GPU 全类型纹理读回与资源反射方面已全面对齐，`ComputeApp` 自检用例（K1、K2、K4、K5、K7、K8、K9）在 OpenGL 4 下 7/7 全部 `PASS`。

---

## A.8 Console 变体（GL4ConsoleContext）

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（6 阶段编译与反射齐全，能力位契约与资源工厂桩已对齐） |
| **位置** | `source/Plugins/Renderer/OpenGL4/Console/Source/T3DGL4ConsoleContext.cpp` |
| **有效接口** | `createRenderWindow`（返回空的 `GL4ConsoleWindow`）、全部 6 个阶段的 `create*Shader`（VS/PS/GS/HS/DS/CS 真实 `glCompileShader`，同步执行不走命令队列）、以及继承自 `GL4ContextBase` 的 `getDepthRemapMatrix`、`compileShader`（含 6 阶段 glslang 反射、CS `local_size` 与 `ShaderResourceParams` 提取）、`reflectShaderAllBindings`、`reflectSamplerBindings` |
| **其余接口** | 全部为单行空实现、`GL4Console*` 桩对象或 `T3D_RHI_UNSUPPORTED` |
| **说明** | `GL4ConsoleContext::init()` 已重写：保留 `mSupportsComputeShader` 与 `mGLSLangLimits` 供离线编译/反射使用，同时将 `mCapabilities` 置为全 false，与 `T3D_RHI_UNSUPPORTED` 宏内的 `T3D_ASSERT(!getCapabilities().capField)` 完全一致；`createRenderTexture` / `createPixelBuffer3D` / `createPixelBufferCubemap` 已统一返回桩对象 |

---

## A.9 待办优先级与完成状态跟踪（第一/二轮已清零，剩余缺口见附录 C 与附录 D）

### 已完成项总表

| 原优先级 | 接口 / 模块 | 解决的问题 | 状态 |
|:--------:|-------------|------------|:----:|
| **P0** | `writeBuffer` 纹理与 SSBO 分支 | 从 `PixelBuffer2D` 描述符取真实 width/height 与像素格式；线性缓冲支持 `kStructuredBuffer` | ✅ 已完成 |
| **P0** | `setVertexShader` | 补齐 `nullptr` 解绑分支，使用 `detachShaderStage` 避免冲掉已挂载的 PS（2026-11-18 起改为 Program 缓存方案，`detachShaderStage` 已删除，见 D.2.1） | ✅ 已完成 |
| **P0** | Hull / Domain / Compute 的 create & set 系列 | 全部改为真实编译与挂载；能力不足时 `createComputeShader` 返回 `nullptr` | ✅ 已完成 |
| **P1** | `setGeometryShader` | 启用并统一走 `attachGraphicsShader(shader, mCurrentGSVariant)` | ✅ 已完成 |
| **P1** | 四个 `blit` 重载 + `ZERO` 语义 + MSAA | 统一走 `resolveBlitEndpoint` + `doBlit`；`ZERO` 当整张；支持普通纹理 Scratch FBO 与 MSAA resolve | ✅ 已完成 |
| **P1** | `copyBuffer` | 使用 `glCopyNamedBufferSubData` 实现，支持 `size == 0` 与 `StructuredBuffer` | ✅ 已完成 |
| **P1** | `resizeRenderTexture` / `resizeRenderTarget` | 重建 `GLTexture` / `GLFBO` / `GLDepthRBO` / `GLResolveTex` / `GLResolveFBO` 全套附件 | ✅ 已完成 |
| **P1** | `resetRenderTarget` & `clearStencil` | `resetRenderTarget` 重置 `mRenderingToFBO`；新增 `clearStencil` 与 `ClearMaskGuard` | ✅ 已完成 |
| **P1 (T3-1)** | **纹理 UAV 支持（`RWTexture2D` / `glBindImageTexture`）** | `GL4PixelBuffer1D/2D/3D` 与 `GLES3PixelBuffer1D/2D/3D` 记录 `GLInternalFormat` 并在 UAV 时走 `glTexStorage*`；`setCSUnorderedAccessBuffers` 支持 `glBindImageTexture`；`uavBarrier` 补 `GL_PIXEL_BUFFER_BARRIER_BIT` 并解绑 image unit；`ComputeApp` K5 验证通过 | ✅ 已完成 |
| **P2** | Compute + SSBO + Indirect + Counter 一整套 | GL 4.3+ 真实现（含 `VertexBuffer`/`IndexBuffer` 作为 Raw SRV 绑定）；GL 4.0+ 支持 `supportsIndirectDraw` | ✅ 已完成 |
| **P2** | Tessellation（Hull / Domain） | `GL_TESS_CONTROL_SHADER` / `GL_TESS_EVALUATION_SHADER` + glslang 反射支持 | ✅ 已完成 |
| **P2** | `createBlendState` / `setBlendState` 独立 RT 混合 | 支持 `IndependentBlendEnable` + `glEnablei` / `glBlendFuncSeparateiARB` / `glColorMaski` | ✅ 已完成 |
| **P2** | GPU 读回 `map` / `unmap` | 线性缓冲与 2D 纹理（含 MSAA 自动 resolve）PBO/Staging 读回，`supportsReadback = true` | ✅ 已完成 |
| **P2** | `reset()` 补齐翻转标志与 Image Unit 复位 | 在 `GL4Context::reset()` 与 `GLES3Context::reset()` 中复位 `mRenderingToFBO`、`mProjectionFlipped` 并解绑 `mBoundCSImages` | ✅ 已完成 |
| **P3** | 合并 `GL4Context` 与 `GL4ContextBase` 重复代码 | 将 Core Profile 4.5 Dummy Context、`fillCapabilities`、`getDepthRemapMatrix`、6 阶段 `glslangCompileAndReflect`（含 CS `localSize`）及 `reflect*` 统一收归 `GL4ContextBase`，补齐 `GL4ConsoleContext` 的 HS/DS/CS 编译与 `makeDummyCurrent()` 析构保护 | ✅ 已完成 |
| **P3** | 注释清理与 `ensureProgramLinked` 错误检查 | 移除过时注释；`ensureProgramLinked` / `ensureComputeProgramLinked` 已补 `GL_CHECK_ERROR` | ✅ 已完成 |

---

## A.10 跨文档一致性与回填清单

GL4 后端的需求分散在多份设计文档中，本节记录它们与当前代码的最新对应状态：

### A.10.1 GPU Readback：GL4 Window 已落地真实现

`doc/todo/GPU-Readback-onRender-Design-todo.md` 定义的三个纯虚接口：

- `RHIContext::map(RenderBuffer *, size_t, size_t)`
- `RHIContext::map(RenderBuffer *, const ReadbackRegion &)`
- `RHIContext::unmap(ReadbackHandle, Buffer &)`

- **当前状态**：`T3DGL4Context` **已完成真实现**（`supportsReadback = true`，基于 `glCreateBuffers` + `GL_STREAM_READ` + `glCopyNamedBufferSubData` / `GL_PIXEL_PACK_BUFFER` + `glGetTextureSubImage`）；`T3DGL4ConsoleContext` 按 null backend 定位保持 `T3D_RHI_UNSUPPORTED` stub。

### A.10.2 Compute / UAV / Indirect：E2 与 T3-1 已全部完成

`doc/todo/RHI-Compute-UAV-Indirect-Draw-Design-todo.md` 第五期任务 E2（GL 4.3+ 全量真实现 + 低版本按能力位 stub）、T0-1（`VertexBuffer`/`IndexBuffer` 作为 `ByteAddressBuffer` SRV 绑定）以及 T3-1（纹理 UAV `glBindImageTexture`）在 `GL4Context` 与 `GLES3Context` 中**均已落地**，`ComputeApp` 的 K1、K2、K4、K5、K7、K8、K9 在 OpenGL 4 下全部实测通过（7 passed, 0 failed）。

### A.10.3 Cubemap：Skybox 文档中的「死代码」说法已过时

`doc/todo/Skybox-Support-Design-todo.md` §2.2 称 GL4 的 `setPixelBuffers` 虽有 `kPixelBufferCubemap` 分支但是**死代码**（cast 错误），§4.5 把「修正 `setPixelBuffers` cast」列为待办。

**核对结果：这个问题已经修好了。** `T3DGL4Context.cpp:4717-4723`（`bindPixelBuffers`）的分支正确 cast 到 `GL4PixelBufferCubemap` 并使用 `GL_TEXTURE_CUBE_MAP` target。Skybox 文档的 §2.2 / §4.5 待办项可以关闭。

### A.10.4 Shader 变体链路：GL4 侧已验证

`doc/todo/Shader-MultiBackend-Variant-Design-todo.md` 记录 `OPENGL4` → `SHADER_LANGUAGE::kGLSL`（scc target `glsl`），且 ResourceApp 在 D3D11 / GL4 / VK 三个后端间切换均验证成功、GL4 正确取到 `kGLSL` 变体。这条链路无待办。

### A.10.5 相机后处理：四个 `blit` 重载与 MSAA 采样已对齐

`doc/todo/Camera-PostProcess-Design-todo.md` B1–B5 已合，且 GL4 Window 后端的四个 `blit` 重载、`size == Vector3::ZERO` 整张拷贝语义、`bindPixelBuffers` 对 MSAA 渲染纹理优先绑定 `GLResolveTex`（`T3DGL4Context.cpp:4696-4700`）以及 `map`/`unmap` 读回**均已补齐**：

| 调用点 | 重载 | GL4 Window 当前状态 |
|--------|------|---------------------|
| `resolveIfMultisampled` | `blit(Texture*, Texture*)`，`size` 默认 `ZERO` | ✅ 已实现（`doBlit` 自动处理 `ZERO` 尺寸与 MSAA 源 resolve） |
| `CameraEffectBehaviour::blitCopy` | `blit(Texture*, RenderTarget*)`，`size` 默认 `ZERO` | ✅ 已实现（`ZERO` 映射为源纹理剩余宽高） |
| `drawFullscreen`（灰度 / 反相 / 染色） | `setRenderTarget` + VS/PS + `_MainTex` + `render(3,0)` | ✅ 已实现 |
| 无效果 / 链结束后上屏 | `blit(Texture*, RenderTarget*)`，带明确 size | ✅ 已实现 |

### A.10.6 需要修正的上游文档

以下不是 GL4 后端的代码问题，但会误导读者，建议一并修掉：

| 文档 | 问题 |
|------|------|
| `doc/Tiny3D-Architecture.md` §3.6 | RHI 后端表里只有 "OpenGL 3" 和 "OpenGL ES 2/3"，**完全没有 OpenGL 4 / GL4Renderer**，而 GL4 是 Windows 上的默认渲染器（`assets/config/Windows/Tiny3D.cfg`） |
| `doc/GL4-Renderer-Backend-Architecture.md` | 架构描述（三变体、Dummy Context、Core Profile 4.5、glslang、延迟 link）仍然准确，但未涵盖 compute / UAV / indirect / instancing / cubemap / scissor 等新增 RHI 能力，也未写后处理依赖的四个 blit 契约 |
| `doc/refs/D3D11-vs-OpenGL4-API-Mapping.md` | 缺 SSBO / `glBindImageTexture` 专节，`DispatchIndirect` 未列入 §12 映射表，也没有 4.0 与 4.3 的版本分级。实现 E2 时建议参考 `doc/refs/D3D11-vs-OpenGLES3-API-Mapping.md` 的 §16（完整 compute 章节）与附录 B（版本能力矩阵），那份写得更系统 |

---
---

# 附录 B：T3-1 实现计划 —— GL4 / GLES3 纹理 UAV（`RWTexture2D` / `glBindImageTexture`）（✅ 已完成）

> **状态**：✅ **已于 2026-11 落地并验证通过**（`ComputeApp` K5 `texture UAV` 在 OpenGL 4.5 下实测 `PASS`）。
>
> **目标**：补齐 `GL4Context` 与 `GLES3Context` 中 `setCSUnorderedAccessBuffers` 对纹理资源（`PixelBuffer2D` 等）的 Image Load/Store 绑定支持，消除原先的「静默绑 0」缺陷，使 `ComputeApp` 的 **K5（纹理 UAV 写入）** 在 OpenGL 4.3+ 与 OpenGL ES 3.1+ 上通过验证。

## B.1 现状与缺陷根因

在 `source/Plugins/Renderer/OpenGL4/Window/Source/T3DGL4Context.cpp:3374-3405`（修复前的行号；当前实现位于 `setCSUnorderedAccessBuffers`，T3DGL4Context.cpp:3188-3296）及 `T3DGLES3Context.cpp` 同名函数中：

```cpp
RenderBuffer *rb = buffers[i].get();
if (rb != nullptr && rb->getRHIResource() != nullptr
    && rb->getRHIResource()->getResourceType() == RHIResource::ResourceType::kStructuredBuffer)
{
    // 只处理了 kStructuredBuffer！
}
const uint32_t slot = startSlot + i;
glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, ssbo);
```

当传入 `PixelBuffer2D`（`kPixelBuffer2D`）时：
1. `if` 不命中，`ssbo` 为 `0`，只执行了 `glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, 0)`；
2. 完全没有调用 `glBindImageTexture`，且返回 `T3D_OK`；
3. Compute Shader 中的 `layout(rgba8, binding = 0) uniform writeonly image2D` 写往未绑定的 image unit，读回全 0。

## B.2 分步改动方案

### Step 1：在 `GL4PixelBuffer` / `GLES3PixelBuffer` 记录 Image Internal Format
- 文件：`source/Plugins/Renderer/OpenGL4/Window/Include/T3DGL4RenderBuffer.h`、`OpenGLES3/Runtime/Include/T3DGLES3RenderBuffer.h`
- `GL4PixelBuffer2D`（以及 1D/3D）已有 `GLuint GLTexture {0};`，新增 `GLenum GLInternalFormat {GL_RGBA8};`。
- 在 `createPixelBuffer2D` 与 `createRenderTexture` 中，将 `GL4Mapping::getPixelFormat` 算出的 `internalFormat`（如 `GL_RGBA8`、`GL_R32F`、`GL_RGBA16F`、`GL_RGBA32F`）存入 `glTex->GLInternalFormat`。
  - **注意 GLES 3.1 限制**：`glTexImage2D` 若用非 sized internalFormat（如 `GL_RGBA`），`glBindImageTexture` 会报 `GL_INVALID_VALUE`！当 `buffer->getGPUAccess() & kGPUUnorderedAccess` 时，必须改用 `glTexStorage2D(GL_TEXTURE_2D, mipLevels, internalFormat, width, height)` + `glTexSubImage2D` 创建不可变存储纹理（GL4 同样推荐在带 UAV 时走 `glTexStorage2D`，兼容性最好）。

### Step 2：扩展 `setCSUnorderedAccessBuffers` 支持纹理与顶点缓冲
- 文件：`T3DGL4Context.cpp`、`T3DGLES3Context.cpp`
- 在遍历 `buffers[i]` 时按 `RHIResource::ResourceType` 分派：
  - **`kStructuredBuffer` / `kVertexBuffer` / `kIndexBuffer`**（缓冲类 UAV）：
    - 用 `getGLBufferHandle(rb)` 取出 `GLuint`（配合 T0-1），调用 `glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, glBuf)`；
    - 同时解绑同槽位的 image unit：`glBindImageTexture(slot, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8)`。
  - **`kPixelBuffer1D` / `kPixelBuffer2D` / `kPixelBuffer3D`**（纹理类 UAV）：
    - 校验 `(rb->getGPUAccess() & kGPUUnorderedAccess) != 0`，否则打 `T3D_LOG_ERROR`；
    - 取出 `GLTexture` 与 `GLInternalFormat`（MSAA 渲染纹理不支持 UAV，直接报错）；
    - 调用 `glBindImageTexture(slot, glTex, 0 /*mip 0*/, GL_FALSE, 0, GL_READ_WRITE, internalFormat)`；
    - 记录到 `mBoundCSImages[slot]` 以便 `uavBarrier` 解绑。
  - **`nullptr`（显式解绑该槽）**：
    - 同时解绑 `GL_SHADER_STORAGE_BUFFER` 与 `glBindImageTexture(slot, 0, ...)`。

### Step 3：`uavBarrier` 补充 `GL_SHADER_IMAGE_ACCESS_BARRIER_BIT` 与解绑
- 确认 `uavBarrier` 中的 `glMemoryBarrier` 掩码包含 `GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT | GL_PIXEL_BUFFER_BARRIER_BIT`（`GL_PIXEL_BUFFER_BARRIER_BIT` 保证后续 `map/unmap` 通过 PBO 读回纹理时能看到 CS 写入的数据！）；
- 将 `buffers` 列表中纹理占用的 image unit 解绑（`glBindImageTexture(slot, 0, ...)`），对齐 D3D11 的解绑语义。

### Step 4：开启 `ComputeApp` K5 的 GL4 / GLES3 验证
- 文件：`source/Samples/ComputeApp/ComputeCases.cpp`
- 将 K5（`ComputeTextureWrite`）的 `backendMask` 从 `kBackendD3D11` 放宽为 `kBackendD3D11 | kBackendGL4 | kBackendGLES3`。
- 验证启动 `ComputeApp`（GL4 后端）时 K5 读回 $256 \times 256$ 纹理像素与期望值 `(x, y, 128, 255)` 逐像素一致。

---
---

# 附录 C：未实现与未完整实现项清单（源码核验）

> 本附录基于对 `source/Plugins/Renderer/OpenGL4/` 全量源码的逐项核验整理，列出当前 GL4 后端（含 Window、Base、Mapping 与 Console 变体）中**尚未实现**或**仅部分实现 / 存在边界缺口**的功能，并标注对应状态与待办优先级。
>
> **状态说明**（沿用附录 A 图例）：
> - ❌ **未实现**：函数体为空/仅 TODO 注释，或核心能力路径完全缺失
> - ⚠️ **部分完成**：主路径可用，但缺少部分资源类型、参数分支、边界保护或存在已知缺陷

---

## C.1 未实现项（❌）

| 编号 | 模块 / 接口 | 状态 | 实现位置 | 缺失内容与影响说明 | 优先级 |
|:----:|-------------|:----:|----------|--------------------|:------:|
| C.1.1 | `GL4Renderer::getEditorInfo` | ❌ 未实现 | `Window/Source/T3DGL4Renderer.cpp:84-87` | 函数体仅有 `// TODO: 填充编辑器需要的 GL 上下文信息`，未填充任何编辑器信息；编辑器侧（`EditorApp.cpp` / `LauncherApp.cpp`）目前无条件构造 `EditorInfoDX11` 并交给当前渲染器填充，GL4 下该结构体保持未填写状态，随后被 `ImGuiImplDX11` 使用；尚无 GL4 的 Editor 信息结构体与对接实现 | **P2** |
| C.1.2 | `ShaderResourceParams` 反射（SSBO / Image UAV / Atomic Counter） | ✅ 已完成 | `Base/Source/T3DGL4ContextBase.cpp` | `glslangCompileAndReflect` 已采集 `bufferBlocks`（含 `isReadOnly` 与 stride）、`atomicCounterBindings` 与 `image*` 绑定，`reflectShaderAllBindings` 已填充 `ShaderResourceParams` 并调用 `shader->setShaderResourceParams(...)` | **P2** |
| C.1.3 | 纹理数组（`TT_2D_ARRAY` / `TT_CUBE_ARRAY` / 1D Array） | ✅ 已完成 | `Window/Source/T3DGL4Context.cpp` | `createPixelBuffer1D`/`2D`/`Cubemap`、`bindPixelBuffers`、`getGLImageInfo`、`unbindTextureUnits`、`writeBuffer`、`map(region)`、`blit` 均已补齐 `GL_TEXTURE_1D_ARRAY`、`GL_TEXTURE_2D_ARRAY`、`GL_TEXTURE_CUBE_MAP_ARRAY` 支持 | **P2** |

---

## C.2 部分完成 / 存在边界缺口项（⚠️，已全部补齐 ✅）

### C.2.1 管线状态、渲染目标与顶点输入

| 编号 | 接口 | 状态 | 实现位置 | 实现补齐说明 | 优先级 |
|:----:|------|:----:|----------|--------------|:------:|
| C.2.1 | `setVertexBuffers` | ✅ 已完成 | `T3DGL4Context.cpp` | 1) 叠加 `offsets[i]` 与 `attrib.getOffset()`（与 D3D11 语义一致）；<br>2) 循环内对 `buffers[i]` 与 `getRHIResource()` 做判空，同时保持 `mPendingVertexDecl` 至下一次 `setVertexDeclaration` / `reset()` | **P0** |
| C.2.2 | `setPixelShader` / `setHullShader` / `setDomainShader` / `setGeometryShader` | ✅ 已完成 | `T3DGL4Context.cpp` | 统一走 `attachGraphicsShader(..., stageType)`；传入 `nullptr` 时只清空 `mStageShaders` 中对应阶段的句柄并置 `mProgramDirty = true`，draw 前按新组合查 Program 缓存（`detachShaderStageByType` 已随 D.2.1 删除） | **P0** |
| C.2.3 | `setRenderTarget` | ✅ 已完成 | `T3DGL4Context.cpp` | 1) `renderTarget == nullptr` 转调 `resetRenderTarget()`；<br>2) MRT 支持 MSAA 目标，切回较少 RT 时清理残留 `GL_COLOR_ATTACHMENT` 并始终刷新 `glDrawBuffers`，且无外部深度时恢复 `GLDepthRBO`；<br>3) Depth-only FBO 改为成员 `mDepthOnlyFBO` 并在 `~GL4Context()` 释放，支持 MSAA 与无模板深度格式 | **P1** |
| C.2.4 | `setRasterizerState` | ✅ 已完成 | `T3DGL4Context.cpp` | 根据 `d.multisampleEnabled` 调用 `glEnable/glDisable(GL_MULTISAMPLE)`。DepthClamp 映射反向问题已于附录 D.1.1 修复 | **P2** |
| C.2.5 | `setViewport` | ✅ 已完成 | `T3DGL4Context.cpp` | 主路径与 fallback 路径均透传 `glDepthRange(vp.MinDepth, vp.MaxDepth)`。窗口目标 Y 未翻转问题已于附录 D.1.3 修复 | **P3** |

### C.2.2 纹理资源创建与数据传输 / 读回

| 编号 | 接口 | 状态 | 实现位置 | 实现补齐说明 | 优先级 |
|:----:|------|:----:|----------|--------------|:------:|
| C.2.6 | `writeBuffer` | ✅ 已完成 | `T3DGL4Context.cpp` | 1) 纹理分支已完整支持 `1D`/`1D_ARRAY`/`2D`/`2D_ARRAY`/`3D`/`CUBE`/`CUBE_ARRAY`（拒绝 MSAA）；<br>2) 支持单层 mip 0 及完整多级/多层链上传，设 `GL_UNPACK_ALIGNMENT = 1`；<br>3) 增加 `renderBuffer`/`buffer.Data`/`DataSize == 0`/`Usage::kImmutable` 与线性缓冲越界检查。纹理分支解绑活动纹理单元的问题已于附录 D.2.2 改用 DSA 修复；压缩格式暂不支持，见附录 D.4.2 | **P1** |
| C.2.7 | `map(RenderBuffer*, const ReadbackRegion&)` | ✅ 已完成 | `T3DGL4Context.cpp` | 1) 支持 `1D`/`1D_ARRAY`/`2D`（含 MSAA resolve）/`2D_ARRAY`/`3D`/`CUBE`/`CUBE_ARRAY`；<br>2) 正确处理 `region.arraySlice` 与边界校验；<br>3) 显式拦截深度模板格式（`isDepthFormat`）并设 `GL_PACK_ALIGNMENT = 1`。MSAA resolve 后未恢复原 FBO 的问题已于附录 D.1.6 修复 | **P1** |
| C.2.8 | `blit`（四个重载） | ✅ 已完成 | `T3DGL4Context.cpp` | 1) `resolveBlitEndpoint` 支持 `1D`/`2D`/`2D_ARRAY`/`3D`/`CUBE`/`CUBE_ARRAY`/`RENDER_TEXTURE`，校验 `kImmutable` 目标与深度/颜色匹配；<br>2) 支持同格式 1:1 走 `glCopyImageSubData`、异格式/跨层走 `attachScratchFBO`（含 1D/Layer/Depth/Stencil）；<br>3) 支持 `size == ZERO` 缩放与彩色缩放 `GL_LINEAR` 过滤，异常路径亦恢复 `prevFBO` | **P2** |
| C.2.9 | `createPixelBuffer1D` / `createPixelBuffer3D` | ✅ 已完成 | `T3DGL4Context.cpp` | 完整支持 `desc.mipmaps` 多级分配/上传与 `BASE_LEVEL`/`MAX_LEVEL`（1D 同时支持 `arraySize` 与 UAV `glTexStorage*`），设 `GL_UNPACK_ALIGNMENT = 1` | **P2** |
| C.2.10 | `createPixelBufferCubemap` | ✅ 已完成 | `T3DGL4Context.cpp` | `GL4PixelBufferCubemap` 记录 `GLInternalFormat` 与 `GLTarget`；支持 `CubemapArray`、UAV `glTexStorage2D/3D`、预生成多级 mip 上传与按需 `glGenerateMipmap` | **P2** |

### C.2.3 Shader 反射、采样绑定与枚举映射

| 编号 | 接口 / 模块 | 状态 | 实现位置 | 实现补齐说明 | 优先级 |
|:----:|-------------|:----:|----------|--------------|:------:|
| C.2.11 | `reflectShaderAllBindings` / `reflectSamplerBindings` / `setupSamplerBindings` | ✅ 已完成 | `T3DGL4ContextBase.cpp`<br>`T3DGL4Context.cpp` | 1) `reflectShaderAllBindings` 补齐 `vec2/3`、`mat2/3`、`mat*x*`、`uint`/`ivec*`/`uvec*`/`bvec*`、`double/dvec*/dmat*` 映射与准确字节大小；<br>2) 三处统一调用 `GL4ContextBase::mapSamplerGLType`，覆盖 `1D/2D/3D/Cube/1DArray/2DArray/CubeArray/Shadow/Multisample` 及整型采样器 | **P2** |
| C.2.12 | `GL4Mapping`（顶点属性 & 像素格式映射） | ✅ 已完成 | `Base/Source/T3DGL4Mapping.cpp` | 1) 补齐 `E_VAT_INT4`、`E_VAT_UINT1~4`、`E_VAT_FLOAT16_2`/`4`；<br>2) `isIntegerAttrib()` 补齐 `E_VAT_INT1~4` 与 `E_VAT_UINT1~4`；<br>3) `PixelFormat::E_PF_PALETTE8` 映射为 `GL_RED`/`GL_R8`/`GL_UNSIGNED_BYTE` | **P1** |

---

## C.3 Console 变体（`GL4ConsoleContext`）遗留问题（已全部补齐 ✅）

| 编号 | 接口 / 模块 | 状态 | 实现位置 | 实现补齐说明 | 优先级 |
|:----:|-------------|:----:|----------|--------------|:------:|
| C.3.1 | `GL4ConsoleContext::init` 与能力位断言冲突 | ✅ 已完成 | `Console/Source/T3DGL4ConsoleContext.cpp`<br>`Base/Source/T3DGL4ContextBase.cpp` | 重写 `GL4ConsoleContext::init()`，保留 `mSupportsComputeShader` 与 `mGLSLangLimits` 后重置 `mCapabilities = RHICapabilities{}`，不再触发 `T3D_RHI_UNSUPPORTED` 断言失败 | **P1** |
| C.3.2 | `GL4ConsoleContext` 部分资源工厂返回 `nullptr` | ✅ 已完成 | `Console/Source/T3DGL4ConsoleContext.cpp`<br>`Console/Include/T3DGL4ConsoleRenderBuffer.h` | 补齐 `GL4ConsolePixelBuffer3D::create()` 与 `GL4ConsolePixelBufferCubemap`，`createRenderTexture`/`createPixelBuffer3D`/`createPixelBufferCubemap` 统一返回桩对象 | **P3** |

---

## C.4 后续建议修复优先级汇总

| 优先级 | 编号 | 项 | 状态 |
|:------:|:----:|----|:----:|
| **P0** | C.2.1 | `setVertexBuffers` 补齐 `offsets[i]` 与判空 | ✅ 已完成 |
| **P0** | C.2.2 | `set{PS,HS,DS,GS}Shader(nullptr)` 真正 detach | ✅ 已完成 |
| **P1** | C.2.3 | `setRenderTarget` 判空、MRT 清理与成员 `mDepthOnlyFBO` | ✅ 已完成 |
| **P1** | C.2.12 | `GL4Mapping` 补齐整型/半精度顶点属性与 `PALETTE8` 映射 | ✅ 已完成 |
| **P1** | C.2.6 / C.2.7 | `writeBuffer` 与 `map(region)` 补齐 1D/3D/Cube/Array、判空及格式检查 | ✅ 已完成 |
| **P1** | C.3.1 | `GL4ConsoleContext` 能力位与 `T3D_RHI_UNSUPPORTED` 对齐 | ✅ 已完成 |
| **P2** | C.1.2 / C.1.3 / C.2.4 / C.2.8~C.2.11 | 纹理数组、`ShaderResourceParams` 反射、3D/Cube Blit、Mipmap、Sampler/Uniform 类型、MSAA 状态 | ✅ 已完成 |
| **P2** | C.1.1 | `GL4Renderer::getEditorInfo`（需编辑器侧同步新增 GL4 对接结构，属于 ❌ 未实现项） | ❌ 待编辑器侧接入 |
| **P3** | C.2.5 / C.3.2 | `setViewport` 深度范围、`GL4ConsoleContext` 桩对象统一 | ✅ 已完成 |
| — | 附录 D | 2026-11-18 行为级 review 新发现的语义错误、竞态与性能问题 | ✅ 已处理，见附录 D.6 |
| — | 附录 E | 2026-11-19 对照 D3D11 的差距 review 新发现的问题 | ❌ 待修复，见附录 E.5 |

---
---

# 附录 D：行为级 review 新发现问题（2026-11-18）

> 附录 A～C 关注的是「接口有没有真实实现」。本附录是逐段阅读 `T3DGL4Context.cpp`（5650 行，提交 `05c4cb34`）后得到的**行为级**问题：接口有实现，但语义与 D3D11 / RHI 约定不一致、存在主线程与 RHI 线程的数据竞态，或有明显性能问题。
>
> 对照基线：GLES3 后端今天的提交 `771887da`（修复 `GLES3Context` RHI 线程竞态与 `doBlit` 翻转），以及 `D3D11Context` 的同名接口。
>
> 分类：D.1 确认的正确性缺陷；D.2 性能与副作用；D.3 需实测确认的风险；D.4 相对 D3D11 的功能缺口。
>
> **修复状态（2026-11-18 同日更新）**：D.1 全部 7 项、D.2.1、D.2.2、D.3.1、D.4.1（vsync）已修复；D.3.2 经核对为调用方约定，不是缺陷；D.2.3、D.4.2、D.4.3、D.4.4 按下文说明暂不改动。汇总见 D.6。各条目「位置」中的行号是**修复前**的位置。修改后 GL4 的三个源文件已用 MSVC 做过语法检查（`cl /Zs`），尚未完整链接，也未跑运行时用例。

---

## D.1 确认的正确性缺陷

### D.1.1 `setRasterizerState`：`DepthClipEnable` 与 `GL_DEPTH_CLAMP` 映射反向

| 项目 | 内容 |
|------|------|
| **严重度** | 高 |
| **位置** | T3DGL4Context.cpp:1394-1397 |
| **现状** | `if (d.depthClipEnabled) glEnable(GL_DEPTH_CLAMP); else glDisable(GL_DEPTH_CLAMP);` |
| **问题** | `RasterizerDesc::DepthClipEnable` 默认是 `true`（`T3DRasterizerState.h:73`），语义是「启用近/远平面裁剪」，与 D3D11 一致；而 `GL_DEPTH_CLAMP` 开启时恰恰会**关闭**近/远平面裁剪，改为钳制深度。结果是 GL4 下默认所有物体都不做近/远平面裁剪，超出远平面的几何体会被压到深度 1.0 继续绘制，只有显式写 `DepthClipEnable = false` 的状态（如 RmlUi）反而开启了裁剪 |
| **修复** | 取反：`depthClipEnabled ? glDisable(GL_DEPTH_CLAMP) : glEnable(GL_DEPTH_CLAMP)` |
| **状态** | ✅ 已修复 |

### D.1.2 `createBlendState` / `createDepthStencilState` / `createRasterizerState`：状态数据写入存在竞态

| 项目 | 内容 |
|------|------|
| **严重度** | 高（RHI 线程异步运行时） |
| **位置** | T3DGL4Context.cpp:1030-1048、1077-1095、1118-1136 |
| **现状** | 主线程把描述符提取成 POD 后，通过 `ENQUEUE_UNIQUE_COMMAND` 在 RHI 线程里执行 `glState->data = d` |
| **问题** | 对应的 `setBlendState` / `setDepthStencilState` / `setRasterizerState`（1240、1307、1356 行）在**主线程**直接读取 `glState->data` 并按值捕获。若状态对象在同一帧内先创建后使用，主线程读到的是还没被 RHI 线程写入的默认零值（例如 `colorMask` 全为 `GL_FALSE`、深度测试关闭），且读写本身构成数据竞态 |
| **修复** | 与 GLES3 提交 `771887da` 一致：`data` 是纯 CPU 数据，不涉及 GL 调用，直接在主线程赋值 `glState->data = d`，删掉入队的 lambda |
| **状态** | ✅ 已修复（三个接口均改为主线程赋值） |

### D.1.3 `setViewport`：渲染到窗口时没有做 Y 翻转

| 项目 | 内容 |
|------|------|
| **严重度** | 中 |
| **位置** | T3DGL4Context.cpp:756-757（主路径）、743-744（fallback 路径） |
| **现状** | `y = viewport.Top * height`，对窗口和 FBO 一视同仁 |
| **问题** | RHI 约定 `Viewport::Top` 是从上往下的比例（D3D11 `TopLeftY = vp.Top * height`）。渲染到 FBO 时投影矩阵已翻转 Y，直接使用是对的；但渲染到窗口（默认 FBO 0，原点在左下）时投影未翻转，应使用 `y = height - (Top + Height) * height`。`setScissorRect`（782 行）对窗口做了这个翻转，两者不一致。全屏视口不受影响，所以一直没暴露；分屏、画中画相机等非全屏视口直接画到窗口时会上下错位 |
| **修复** | 按 `mRenderingToFBO` 分支：FBO 保持现状，窗口按上式翻转；fallback 路径按值捕获 `mRenderingToFBO` 后同样处理。GLES3（`T3DGLES3Context.cpp:832`、`844`）有同样问题 |
| **状态** | ✅ 已修复（GL4 与 GLES3 的主路径和 fallback 路径都已改） |

### D.1.4 `createSamplerState`：BorderColor 的 R/B 分量被交换

| 项目 | 内容 |
|------|------|
| **严重度** | 中（默认边框色为白色，暂未暴露） |
| **位置** | T3DGL4Context.cpp:1156-1159 |
| **现状** | `GLfloat borderColor[4] = { blue(), green(), red(), alpha() };` |
| **问题** | `GL_TEXTURE_BORDER_COLOR` 要求 RGBA 顺序，`ColorRGBA::red()/blue()` 返回的就是对应通道，此处交换后彩色边框会红蓝颠倒。GLES3（`T3DGLES3Context.cpp:1203-1206`）是正确的 RGBA 顺序；D3D11（`T3DD3D11Context.cpp:1744-1746`）有同样的交换，看起来是同源错误 |
| **修复** | 改为 `{ red(), green(), blue(), alpha() }`，D3D11 一并修正 |
| **状态** | ✅ 已修复（GL4 与 D3D11） |

### D.1.5 `GL4Renderer::init()`：忽略上下文创建失败

| 项目 | 内容 |
|------|------|
| **严重度** | 低 |
| **位置** | `Window/Source/T3DGL4Renderer.cpp:55-65` |
| **问题** | `mContext = GL4Context::create()` 失败时 `mContext` 为 `nullptr`，但 `init()` 仍返回 `T3D_OK`，上层拿到一个没有 Context 的渲染器，后续才以空指针形式崩溃 |
| **修复** | `mContext == nullptr` 时返回错误码（如 `T3D_ERR_GL4_CREATE_CONTEXT` 或通用失败码） |
| **状态** | ✅ 已修复（记录错误日志并返回 `T3D_ERR_GL4_CREATE_CONTEXT`） |

### D.1.6 `map(RenderBuffer*, const ReadbackRegion&)`：MSAA resolve 后冲掉当前渲染目标

| 项目 | 内容 |
|------|------|
| **严重度** | 中 |
| **位置** | T3DGL4Context.cpp:3950-3956 |
| **现状** | resolve 完成后执行 `glBindFramebuffer(GL_FRAMEBUFFER, 0)` |
| **问题** | `map` 要求在 `onRender` 中调用，此时通常还有后续 draw。绑回 0 而不恢复原 FBO，会让同一帧之后的绘制全部落到窗口上。`doBlit`（5405-5406、5523 行）已经正确地保存并恢复 `prevFBO`，这里遗漏了 |
| **修复** | resolve 前 `glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO)`，结束后恢复 |
| **状态** | ✅ 已修复 |

### D.1.7 `setVertexShader(nullptr)`：删除整个 Program

| 项目 | 内容 |
|------|------|
| **严重度** | 中低 |
| **位置** | T3DGL4Context.cpp:2447-2459 |
| **问题** | `nullptr` 分支直接 `GL_SAFE_DELETE_PROGRAM(mCurrentProgram)`，已挂载的 PS/GS/HS/DS 随之丢失，但 `mCurrentPSVariant` 等记录还保留着；之后再设置 VS 时会新建一个只有 VS 的 Program，fragment 输出未定义（画面全黑）。其他阶段走 `attachGraphicsShader(nullptr)`，只摘除自身，两者语义不一致 |
| **修复** | 改为 `attachGraphicsShader(shader, mCurrentVSVariant, GL_VERTEX_SHADER)`，与其他阶段统一 |
| **状态** | ✅ 已修复（随 D.2.1 一起，`set*Shader(nullptr)` 现在只清空本阶段的句柄） |

### D.1.8 四个 `blit` 重载：主线程读取 RHI 线程写入的 GL 句柄

| 项目 | 内容 |
|------|------|
| **严重度** | 中 |
| **位置** | `GL4Context::blit` 四个重载（T3DGL4Context.cpp:3650-3815）；`GLES3Context::blit` 同样写法（T3DGLES3Context.cpp:3384-3549） |
| **问题** | `resolveBlitEndpoint` 在主线程执行，把 `GLTexture` / `GLFBO` / `GLResolveFBO` 按值拷进 lambda。这些句柄由 RHI 线程在创建资源时写入，资源刚创建、命令还没执行时主线程读到的是 0。结果是 blit 两端都变成 FBO 0：被判为同一子资源而跳过（"source and destination are the same subresource"），或 `GL_INVALID_FRAMEBUFFER_OPERATION`（0x0506）。lambda 也没有持有 src/dst 的引用。正常渲染没有暴露，是因为相机 RT 都在很多帧之前就建好了。由 ComputeApp K11 / K13 发现 |
| **修复** | 主线程那次 `resolveBlitEndpoint` 只做参数校验；lambda 改为捕获 `RenderTargetPtr` / `TexturePtr`，在 RHI 线程重新解析端点再 `doBlit`。智能指针顺带保证命令执行前资源不会被释放 |
| **状态** | ✅ 已修复（GL4 与 GLES3） |

---

## D.2 性能问题与副作用

### D.2.1 Program 没有缓存，切换 shader 就重新链接

| 项目 | 内容 |
|------|------|
| **严重度** | 高（性能） |
| **位置** | `setVertexShader`（2445）、`attachGraphicsShader`（4852）、`setComputeShader`（2776）、`ensureProgramLinked`（2858）、`ensureComputeProgramLinked`（2900） |
| **问题** | 所有材质共用一个可变的 `mCurrentProgram`。每次 `set*Shader` 都会 detach / attach 并置 `mProgramDirty = true`，**即使传入的是同一个 shader**；下一次 draw 就触发 `glLinkProgram`，链接后还要重跑 `setupSamplerBindings`（遍历全部 active uniform 调用 `glGetActiveUniform` / `glGetUniformLocation`）和 `bindPendingUniformBlocks`（`mUBOBoundProgram` 被清零）。`setComputeShader` 更是每次都删掉 Program 重建。材质数量上去后，每帧会出现大量链接，是 GL4 后端最大的 CPU 开销来源 |
| **修复** | 按 (VS, HS, DS, GS, PS) 的 shader handle 组合建立 Program 缓存（`HashMap<ProgramKey, GLuint>`），`set*Shader` 只更新当前组合，draw 时查表命中直接 `glUseProgram`；sampler 绑定只在首次链接时设置一次。最低限度：`set*Shader` 传入的 variant 与当前相同时直接返回 |
| **状态** | ✅ 已修复（完整缓存方案，见下） |
| **实现** | `set*Shader` 在 RHI 线程只更新 `mStageShaders[阶段]`，句柄真正变化时才置 `mProgramDirty`。draw 前 `ensureProgramLinked` 以 `std::array<GLuint, 5>`（VS/HS/DS/GS/PS）为键查 `mGraphicsProgramCache`；未命中时新建 Program、attach、`linkProgram`。`linkProgram` 一次性完成 `glUniformBlockBinding`（block i → binding i）、记录 block 名单到 `mProgramUBONames`、调用 `setupSamplerBindings`。`bindPendingUniformBlocks` 改为查名单，不再每次 `glGetActiveUniformBlockName`。链接失败的组合在缓存中记为 0，之后直接返回错误，不会每帧重复链接。Compute 侧同理，以 CS 句柄为键查 `mComputeProgramCache`。`reset()` 只清当前引用，缓存保留；析构时统一删除缓存中的 Program。已删除 `detachShaderStage` / `detachShaderStageByType` |
| **注意** | 缓存的 Program 永远不 detach shader，所以 shader 对象被删除后 GL 句柄仍被占用，不会被新 shader 复用，句柄可以安全地当键。代价是缓存不会自动收缩：热重载 shader 时旧组合会留在缓存里，直到 Context 析构 |

### D.2.2 `writeBuffer` 纹理分支解绑当前活动纹理单元

| 项目 | 内容 |
|------|------|
| **严重度** | 低 |
| **位置** | T3DGL4Context.cpp:4309、4355（另有 4419-4421 的 `GL_TEXTURE_BUFFER`） |
| **问题** | 用 `glBindTexture(glTarget, glTex)` 上传后 `glBindTexture(glTarget, 0)`，会把当前活动纹理单元（通常是 unit 0）上已经绑定给材质的纹理解绑。若在两次 draw 之间更新纹理（如 UI 字体图集），后一次 draw 可能采样到空纹理。`createPixelBuffer*` 也有同样写法，但创建通常不会穿插在 draw 中间 |
| **修复** | 改用 DSA：`glTextureSubImage1D/2D/3D`、`glTextureBuffer`，不触碰绑定状态 |
| **状态** | ✅ 已修复（`writeBuffer` 的纹理分支与 typed 结构化缓冲分支；Cubemap 在 DSA 下按 layer 访问，用 `glTextureSubImage3D` 且 zoffset = face。`createPixelBuffer*` 保持原写法） |

### D.2.3 其他

- `doBlit`（5321 行）开头调用 `unbindTextureUnits()`，清掉 16 个纹理单元的全部 target 与 sampler 绑定。依赖上层每次 draw 都重新绑定资源；若上层将来做状态缓存，会出现采样到空纹理的问题。
- 每个颜色渲染纹理都会自带一个 D24S8 深度 RBO（`buildRenderTextureResources`，478-482、538-541 行），MRT 中第 1 个以后的纹理、以及会挂外部深度的 RT 也一样，浪费显存。
- `render(indexed)` / `renderIndexedInstanced` 每次 draw 都 `glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING)` 做安全检查，可改为由 `setIndexBuffer` 在 RHI 线程记录状态。
- `uavBarrier` 始终使用全量 barrier 位，可按资源类型收窄。

> **处理结论：以上四项暂不改动。** `unbindTextureUnits` 是为了防止池化的临时 RT 同时挂在纹理单元和 FBO 附件上形成反馈环，去掉有回归风险。隐式深度 RBO 被没有外挂深度的 RT 依赖，要去掉需要先在 RHI 层区分「需要深度」与「不需要深度」的 RT。EBO 检查与全量 barrier 开销很小，并且是安全兜底，等 profile 显示它们是热点再收窄。

---

## D.3 需要实测确认的风险

### D.3.1 MSAA 源的 `blit`

| 项目 | 内容 |
|------|------|
| **位置** | `doBlit`，T3DGL4Context.cpp:5413-5417、5452-5465、5515-5517 |
| **问题** | MSAA 源直接 `glBlitFramebuffer` 到目标。按 GL 规范：读 FBO 多重采样时，源与目标矩形尺寸不一致（缩放）必然 `GL_INVALID_OPERATION`；拷贝到窗口时还要做 Y 翻转，矩形不再完全相同，是否报错取决于驱动。典型场景：MSAA 离屏 RT 尺寸与窗口不同时整张拷贝上屏 |
| **建议** | 先写一个 MSAA RT → 不同尺寸窗口的用例验证。稳妥做法是 MSAA 源先 resolve 到 `GLResolveTex`（同尺寸、不翻转），再从 resolve 纹理做翻转/缩放 blit |
| **状态** | ✅ 已按稳妥做法修复。MSAA 颜色源在以下任一情况下，会先把 `src.fbo` 整张 blit 到 `src.resolveFbo`，再把 resolve FBO 当单采样源走普通 blit（此时缩放可用 `GL_LINEAR`）：需要缩放、目标是窗口（要做 Y 翻转）、源与目标格式不同。矩形完全一致的同尺寸 MSAA → FBO 仍直接 resolve，不多拷一次。MSAA 深度源不受影响 |

### D.3.2 SRV 与 UAV 共用 SSBO 绑定点

| 项目 | 内容 |
|------|------|
| **位置** | `bindStructuredBuffers`（4775）、`setCSUnorderedAccessBuffers`（3188） |
| **问题** | 两者都把 `startSlot + i` 直接当作 `GL_SHADER_STORAGE_BUFFER` 的 binding。D3D11 中 `t0` 与 `u0` 是独立的编号空间，在 GL 中会落到同一个绑定点互相覆盖。除非着色器交叉编译器在生成 GLSL 时已为 UAV 加了 binding 偏移，否则同时使用 `t0` 和 `u0` 会出错。ComputeApp 的用例通过，可能只是因为没有撞槽 |
| **建议** | 核对 scc 生成的 GLSL 中 SSBO binding 的分配规则；若未做偏移，在后端按约定的偏移（如 UAV 从 `maxSRVSlots` 开始）重映射，或由反射结果提供真实 binding |
| **结论** | ✔️ 不是缺陷，是调用方约定。GL 只有一套 SSBO 绑定编号，调用方需要把 UAV 槽号排在 SRV 之后，见 `source/Samples/ComputeApp/ComputeCases.cpp:46-50` 的 `uavSlot(srvCount, u)`。后端不做重映射，以免与着色器中由该约定生成的 binding 冲突。新写的 compute 调用方必须遵守这条约定 |

---

## D.4 相对 D3D11 的功能缺口

| 编号 | 项 | 说明 |
|:----:|----|------|
| D.4.1 | 垂直同步与全屏 | `RenderWindowDesc::IsVsync` / `IsFullscreen`（`T3DRenderTargetDesc.h:71`、`74`）在 `T3DGL4RenderWindow.cpp` 中完全没有被读取；没有 `wglSwapIntervalEXT` / `glXSwapIntervalEXT`，也没有切换全屏的逻辑。D3D11 使用了 `IsVsync`（`T3DD3D11Context.cpp:483`）。<br>**✅ vsync 已补**：`GL4RenderWindow::init` 在上下文 make current、GLAD 加载之后，按 `IsVsync` 设置交换间隔。Windows 使用 `wglSwapIntervalEXT`；Linux 优先 `glXSwapIntervalEXT`，回退到 `glXSwapIntervalMESA`；扩展都不存在时打 warning 并保持驱动默认值。**全屏无需后端处理**：引擎窗口层在 `IsFullscreen` 为真时给 SDL 窗口加 `WINDOW_FULLSCREEN`（`T3DRenderWindow.cpp:99-101`），GL 窗口随之全屏。D3D11 另外设置了 `SwapChainDesc.Windowed = !IsFullscreen`（`T3DD3D11Context.cpp:478`），这是 DXGI 独占全屏特有的设置，GL 没有对应概念 |
| D.4.2 | 压缩纹理 | 没有任何 `glCompressedTexImage*` / `glCompressedTexSubImage*` 路径，`bpp == 0` 的格式在创建与 `writeBuffer` 中被跳过或拒绝。当前引擎 `PixelFormat` 尚未定义 BC/ETC/ASTC，暂不影响，引入压缩格式时需要补齐。**暂不处理** |
| D.4.3 | 2D/1D/3D 自动生成 mip | 只有 Cubemap 会调用 `glGenerateMipmap`（2237、2283、2340 行）。2D 纹理 `mipmaps > 1` 但只提供 mip 0 数据时，`MAX_LEVEL` 被设为 0，不会生成 mip 链。<br>**结论：与 D3D11 一致，不改。** D3D11 后端同样不调用 `GenerateMips`，非 Cubemap 纹理的 mip 链由资源侧提供数据 |
| D.4.4 | `GL4Renderer::getEditorInfo` | 见附录 C.1.1。编辑器目前固定传入 `EditorInfoDX11`，GL4 的实现需要编辑器侧先接入 GL 视口，超出后端范围，**暂不处理** |

---

## D.5 修复优先级

| 优先级 | 编号 | 项 | 改动量 |
|:------:|:----:|----|:------:|
| **P0** | D.1.1 | `DepthClipEnable` / `GL_DEPTH_CLAMP` 取反 | 1 行 |
| **P0** | D.1.2 | 三个 `create*State` 改为主线程直接写 `data`（同步 GLES3 `771887da`） | 小 |
| **P1** | D.1.3 | `setViewport` 窗口目标 Y 翻转（GL4 + GLES3） | 小 |
| **P1** | D.1.6 | `map(region)` MSAA resolve 后恢复原 FBO | 小 |
| **P1** | D.2.1 | Program 缓存（最低限度：同 variant 直接返回） | 中～大 |
| **P2** | D.1.4 | BorderColor R/B 顺序（GL4 + D3D11） | 1 行 |
| **P2** | D.1.7 | `setVertexShader(nullptr)` 统一走 `attachGraphicsShader` | 小 |
| **P2** | D.3.1 / D.3.2 | MSAA blit、SRV/UAV 撞槽：先写用例验证 | 视验证结果 |
| **P2** | D.4.1 | VSync / 全屏 | 中 |
| **P3** | D.1.5 / D.2.2 / D.2.3 | `init()` 失败返回、DSA 上传、冗余 RBO 等 | 小 |
| **P3** | D.4.2 / D.4.3 | 压缩纹理、自动 mip | 视需求 |

---

## D.6 修复记录（2026-11-18）

| 编号 | 处理 | 改动文件 |
|:----:|:----:|----------|
| D.1.1 | ✅ 已修复 | `OpenGL4/Window/Source/T3DGL4Context.cpp` |
| D.1.2 | ✅ 已修复 | 同上 |
| D.1.3 | ✅ 已修复 | 同上；`OpenGLES3/Runtime/Source/T3DGLES3Context.cpp` |
| D.1.4 | ✅ 已修复 | 同上；`Direct3D11/Window/Source/T3DD3D11Context.cpp` |
| D.1.5 | ✅ 已修复 | `OpenGL4/Window/Source/T3DGL4Renderer.cpp` |
| D.1.6 | ✅ 已修复 | `T3DGL4Context.cpp` |
| D.1.7 | ✅ 已修复 | `T3DGL4Context.cpp` |
| D.1.8 | ✅ 已修复 | `T3DGL4Context.cpp`；`T3DGLES3Context.cpp` |
| D.2.1 | ✅ 已修复 | `T3DGL4Context.cpp`、`OpenGL4/Window/Include/T3DGL4Context.h` |
| D.2.2 | ✅ 已修复 | `T3DGL4Context.cpp` |
| D.2.3 | ⏸ 保留现状 | 理由见 D.2.3 |
| D.3.1 | ✅ 已修复 | `T3DGL4Context.cpp` |
| D.3.2 | ✔️ 调用方约定 | 无改动 |
| D.4.1 | ✅ vsync 已补；全屏由窗口层处理 | `OpenGL4/Window/Source/T3DGL4RenderWindow.cpp` |
| D.4.2 | ⏸ 暂不处理 | 引擎尚无压缩格式 |
| D.4.3 | ✔️ 与 D3D11 一致 | 无改动 |
| D.4.4 | ⏸ 暂不处理 | 需编辑器侧接入 |

**回归验证**：`ComputeApp` 新增四个图形自检用例，与 K1～K9 一起在 OpenGL 4 与 Direct3D11 下均为 13 passed, 0 failed。

| 用例 | 覆盖 | 做法 |
|------|------|------|
| K1～K9 | compute program 缓存、UBO 名单 | 原有用例 |
| K11 MSAA scaled blit | D.3.1 | 64×64 4x MSAA RT 清白，`blit` 到 32×32 单采样 RT，读回要求全白 |
| K12 shader bind order | D.2.1、D.1.7 | 64×8 RT 按 1/4 宽视口分四条：先 PS 后 VS（绿）、只换 PS（白）、`setVertexShader(nullptr)` 后再设 VS（绿）、不画（黑），读回要求 `GWGK` |
| K13 window viewport | D.1.3 | 窗口清黑后以视口 `{0, 0, 0.5, 0.5}` 画白，`blit` 到同尺寸 RT 读回，要求只有左上象限为白 |
| K14 sampler border color | D.1.4 | Border 寻址、BorderColor 纯红，在 UV (1.5, 1.5) 采样，读回字节须与纯红对照竖带一致；与纯蓝对照一致即判为 R/B 交换。把 D3D11 改回旧写法时该用例实测失败 |

- 这四个用例只在 D3D11 与 GL4 上运行，GLES3 不在掩码内。
- 远平面裁剪（D.1.1）仍需在 RmlUi 和普通场景中目测确认。

---
---

# 附录 E：对照 D3D11 的差距 review（2026-11-19）

> 附录 A～D 回答的是「接口有没有实现、实现有没有语义错误」。本附录以 `D3D11Context`（Window 版）为行为基线，逐模块对比同名接口内部的行为，列出 **D3D11 能做而 GL4 做不到、或结果与 D3D11 不一致**的地方。两个后端 override 的 `RHIContext` 接口集合完全相同（97 个），所以这里的差距全部出在函数内部。
>
> 行号基于 2026-11-19 的源码（`T3DGL4Context.cpp` 5638 行，提交 `cdebf4a8`）。
>
> 同一轮 review 发现的 D3D11 映射错误（`kGreaterEqual`、四个模板自增/自减操作、`StencilRef` 写死为 1、缺 `kMirrorOnce`）与 GL 无关，已直接在 D3D11 后端修复，不在本附录跟踪。GL4 这几处的映射本来就是正确的。

## E.1 会产生错误结果的缺口

### E.1.1 资源析构在主线程调用 `glDelete*`

| 项目 | 内容 |
|------|------|
| **严重度** | 高 |
| **位置** | `Window/Source/T3DGL4RenderBuffer.cpp:36-73` 等全部 `~GL4*` 析构；删除宏 `Window/Include/T3DGL4Prerequisites.h:46-62`；context 转移 `T3DGL4Context.cpp:209-211` |
| **问题** | Runtime 与 Editor 默认开启 RHI 线程（`Core/Runtime/CMakeLists.txt:31`、`Core/Editor/CMakeLists.txt:30` 定义 `T3D_ENABLE_RHI_THREAD=1`）。`createRenderWindow` 会让主线程 `wglMakeCurrent(nullptr, nullptr)`，把 context 交给 RHI 线程。引擎在主线程执行 `mRHIResource = nullptr`（如 `T3DVertexBuffer.cpp:70`、`T3DPixelBuffer.cpp:119`），最后一个引用在主线程释放时，析构函数会在**没有 current context** 的线程上调用 `glDeleteBuffers` / `glDeleteTextures` / `glDeleteFramebuffers`。结果是对象泄漏（显存持续增长），部分驱动上直接崩溃。命令 lambda 捕获的智能指针让最后一个引用可能落在任意一个线程，问题时有时无 |
| **D3D11** | COM `Release` 可以跨线程调用，不受影响 |
| **修复建议** | 析构时不直接删除，而是把 GL 句柄（类型 + 名称）投递到 RHI 线程执行删除，例如在 `GL4Context` 维护一个延迟删除队列，在 `swapBackBuffer` 或每条命令执行前清空；RHI 线程未启用时保持直接删除。Program 缓存中引用的 shader 句柄删除时机需一并考虑 |
| **验证** | 反复创建/销毁纹理与缓冲，观察显存与 `glIsBuffer` / `glIsTexture` |
| **状态** | ✅ 已修复（2026-11-19）。新增 `releaseGLObjects`（声明在 `T3DGL4Prerequisites.h`，实现在 `T3DGL4RenderBuffer.cpp`），全部 RHI 资源析构、Shader 析构、`GL4SamplerState`、`GL4RenderWindow` 的深度 RBO 以及 `finishReadback` 的 staging buffer 都改走它：当前线程有 current context 时直接删除；否则作为一条命令投递到 RHI 线程，排在已入队命令之后执行，不会删掉后续命令还在用的句柄；RHI 线程已停止时借 dummy context 在当前线程删除，仍无 context 则打 warning。`~GL4Context` 把 `makeDummyCurrent()` 提到释放成员引用之前。只做了 `cl /Zs` 语法检查，尚未跑运行时验证。GLES3 后端的资源析构写法相同（`T3DGLES3RenderBuffer.cpp`），是否受影响取决于其 context 归属，未处理 |

### E.1.2 D32FS8 深度渲染纹理创建失败

| 项目 | 内容 |
|------|------|
| **严重度** | 高（使用该格式时必现） |
| **位置** | `Base/Source/T3DGL4Mapping.cpp:80-82`；调用点 `T3DGL4Context.cpp:467`、`579` |
| **问题** | `getPixelType(E_PF_D32_FLOAT_S8X24_UINT)` 返回 `GL_FLOAT`。非 MSAA 深度 RT 会调用 `glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, w, h, 0, GL_DEPTH_STENCIL, GL_FLOAT, nullptr)`，而 `GL_DEPTH_STENCIL` 格式只接受 `GL_UNSIGNED_INT_24_8` 或 `GL_FLOAT_32_UNSIGNED_INT_24_8_REV`，驱动报 `GL_INVALID_OPERATION`，纹理没有存储，FBO 不完整。MSAA 路径走 `glTexImage2DMultisample` 不需要 type，不受影响 |
| **D3D11** | `DXGI_FORMAT_D32_FLOAT_S8X24_UINT` 映射正确 |
| **修复建议** | `E_PF_D32_FLOAT_S8X24_UINT` 单独返回 `GL_FLOAT_32_UNSIGNED_INT_24_8_REV`；GLES3 的 Mapping 一并核对 |
| **状态** | ✅ 已修复（2026-11-19）。`GL4Mapping::getPixelType` 与 `GLES3Mapping::getPixelType`（GLES3 有同样的错误）都改为返回 `GL_FLOAT_32_UNSIGNED_INT_24_8_REV`。该 type 每像素 8 字节，与 `getBytesPerPixel` 一致，`writeBuffer` 的数据量校验不受影响；`map(region)` 本来就拒绝深度格式。GL4 已做 `cl /Zs` 语法检查，未跑运行时验证。<br>**回归用例**：`ComputeApp` 新增 K17 `depth stencil formats`（D3D11 与 GL4），D24S8 / D32F / D32FS8 / D16 各测 `shaderReadable` 为 false / true，以颜色读回校验深度测试、模板测试与深度纹理采样。<br>**D3D11 同类问题**：`createRenderTexture` 中 D32FS8 + `shaderReadable` 的资源格式误用了视图格式 `DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS`，无法与 `D3D11_BIND_DEPTH_STENCIL` 搭配，已改为 `DXGI_FORMAT_R32G8X24_TYPELESS`（SRV 仍用 `R32_FLOAT_X8X24_TYPELESS`） |

### E.1.3 各 shader 阶段共用纹理单元，VS 与 PS 同时采样会冲突

| 项目 | 内容 |
|------|------|
| **严重度** | 中（VS 采样纹理时必现，如顶点动画贴图、地形高度图） |
| **位置** | 反射 `Base/Source/T3DGL4ContextBase.cpp:943`、`976-977`；绑定 `T3DGL4Context.cpp:4783`（`bindPixelBuffers`）、`4816`（`bindSamplers`）；`setupSamplerBindings` 的 `findSlot`（4599-4621） |
| **问题** | 每个 ShaderVariant 的采样器序号都从 0 开始，`texBinding` 与 `samplerBinding` 取同一个值。`set{VS,PS,GS,HS,DS,CS}PixelBuffers` / `Samplers` 全部绑到全局的 `GL_TEXTURE0 + slot` / `glBindSampler(slot)`，GL 的纹理单元又是所有阶段共享的，于是 VS 的 t0 与 PS 的 t0 落到同一个 unit，后绑的覆盖先绑的。`findSlot` 先查 PS 再查 VS，VS 的 sampler uniform 也会指到 PS 的 unit |
| **D3D11** | 每个阶段的 t# / s# 是独立的编号空间 |
| **修复建议** | 在反射阶段按 program 统一分配 unit：链接时把各阶段的 sampler 依次排开（如 PS 从 0 开始，VS 接在 PS 之后），并记录「阶段 + 原 slot → unit」的映射；`bindPixelBuffers` / `bindSamplers` 增加阶段参数，按映射换算 unit。另一种做法是按阶段划分固定区间（如每阶段 16 个 unit），需要 `GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS` 足够大 |
| **状态** | ✅ 已修复（2026-11-19），采用按阶段划分固定区间的方案。纹理绑定发生在 draw 之前、program 链接之前，而且 program 按 shader 组合缓存，按 program 分配需要在链接后回头重绑；固定区间下 unit 只取决于「阶段 + slot」，与链接时机无关。<br>**分区**：`GL4ContextBase::fillCapabilities` 查询 `GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS` 与各阶段上限，PS 从 0 开始取满片元阶段上限，VS / GS / HS / DS / CS 平分剩余单元并受各自上限约束，结果存入 `mTexUnitBase` / `mTexUnitCount`（RTX 5060 上为每阶段 32 个，PS@0、VS@32 … CS@160）。CS 也单独占一段，dispatch 不会冲掉图形管线的纹理。<br>**绑定**：`bindPixelBuffers` / `bindSamplers` / `bindStructuredBuffers` 增加阶段参数，unit = 阶段起点 + slot；槽位超出本阶段区间时返回 `T3D_ERR_INVALID_PARAM`。`unbindTextureUnits` 改为按各阶段实际用过的最高 slot 解绑，不再写死前 16 个单元。<br>**sampler uniform**：`setupSamplerBindings` 增加 `isCompute` 参数，graphics program 只查 PS / VS / GS / HS / DS 的反射，compute program 只查 CS，按命中的阶段换算 unit（原实现会让 graphics program 去查当前 CS 的反射）。samplerBuffer 同样按阶段换算。<br>**遗留限制**：GLSL 中同名 sampler 在一个 program 里只有一个 uniform，若 VS 与 PS 声明了同名纹理，只能指向一个 unit（以 PS 为准），链接时打 warning。SSBO 绑定点仍是各阶段共享的，不在本项范围内。<br>**回归用例**：`ComputeApp` 新增 K18 `cross stage textures`（D3D11 与 GL4），VS 的 t0 绑绿色纹理、PS 的 t0 绑红色纹理，PS 输出 (PS 纹理 R, VS 纹理 G)，两条竖带分别先绑 VS / 先绑 PS，期望均为黄色。修复前 GL4 两条竖带分别读回红、绿；修复后 D3D11 与 GL4 均 17/17 通过。<br>**样例侧配套**：`ComputeKernel::compileGraphics` 原先只编译不反射，GL4 只能按 uniform 顺序回退分配 unit，现补上 `reflect()`；D3D11 反射要求采样器命名为 `sampler<纹理名>`，样例 HLSL 的采样器随之改名 |

### E.1.4 窗口渲染目标忽略外部深度

| 项目 | 内容 |
|------|------|
| **严重度** | 中 |
| **位置** | `T3DGL4Context.cpp:284-288` |
| **问题** | `E_RT_WINDOW` 分支只执行 `glBindFramebuffer(GL_FRAMEBUFFER, 0)`，不读 `renderTarget->getDepthStencil()`。默认 framebuffer 不能挂外部附件，所以挂了外部深度的窗口目标在 GL4 上仍然使用窗口自带的深度 |
| **D3D11** | 外挂的 DepthStencil 优先，没有时才退回窗口自带的 DSV（`T3DD3D11Context.cpp:1250-1253`） |
| **修复建议** | 带外部深度时，改为渲染到一个与窗口同尺寸的中间 FBO（颜色附件 + 外部深度），`swapBackBuffer` 前 blit 到 FBO 0；或在 RHI 层明确禁止该用法并在 GL4 上报错，而不是静默忽略 |

### E.1.5 非 RT 的 MSAA 纹理被静默降为 1x

| 项目 | 内容 |
|------|------|
| **严重度** | 中 |
| **位置** | `createPixelBuffer2D`（`T3DGL4Context.cpp:1836-2003`），整个文件只有 `buildRenderTextureResources`（459 行）读取 `sampleDesc` |
| **问题** | `createPixelBuffer2D` 从不读取 `desc.sampleDesc`，总是创建 `GL_TEXTURE_2D` / `GL_TEXTURE_2D_ARRAY` |
| **D3D11** | `SampleDesc.Count = max(1, desc.sampleDesc.Count)`（`T3DD3D11Context.cpp:2716`） |
| **修复建议** | `sampleDesc.Count > 1` 时走 `glTexStorage2DMultisample`（数组用 `glTexStorage3DMultisample`），`GLTarget` 设为 `GL_TEXTURE_2D_MULTISAMPLE(_ARRAY)`，并拒绝初始数据上传与 UAV |

### E.1.6 RenderTexture 不能作为 UAV

| 项目 | 内容 |
|------|------|
| **严重度** | 中（compute 写 RT 的后处理路径不可用） |
| **位置** | `buildRenderTextureResources`（`T3DGL4Context.cpp:450-614`） |
| **问题** | 颜色 RT 用 `glTexImage2D` 创建可变存储，且不记录可用于 `glBindImageTexture` 的格式；没有 `kGPUUnorderedAccess` 分支。普通 `createPixelBuffer*` 在 1709、1870、2037、2164 行有该分支，RT 路径没有 |
| **D3D11** | MSAA=1 时给 RT 加 `D3D11_BIND_UNORDERED_ACCESS` 并创建 UAV（`T3DD3D11Context.cpp:638-649`） |
| **修复建议** | 非 MSAA 颜色 RT 带 `kGPUUnorderedAccess` 时改用 `glTexStorage2D` 并写入 `GLInternalFormat`；`getGLImageInfo` 已支持 `GL4PixelBuffer2D`，补齐创建侧即可 |

### E.1.7 UAV 槽位不做越界检查

| 项目 | 内容 |
|------|------|
| **严重度** | 中低 |
| **位置** | `setCSUnorderedAccessBuffers`（`T3DGL4Context.cpp:3200`）；能力位 `Base/Source/T3DGL4ContextBase.cpp:123` |
| **问题** | `T3DRHIContext.h:610-611` 约定槽位越界返回 `T3D_ERR_INVALID_PARAM`。GL4 只把 `GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS` 填进 `maxUnorderedAccessSlots`，绑定时从不检查，越界后由驱动报 `GL_INVALID_VALUE`，函数仍返回 `T3D_OK` |
| **D3D11** | `startSlot + size > D3D11_1_UAV_SLOT_COUNT` 时报错返回 |
| **修复建议** | 入口按 `maxUnorderedAccessSlots`（SSBO）与 `GL_MAX_IMAGE_UNITS`（image）校验，越界返回 `T3D_ERR_INVALID_PARAM` |

### E.1.8 深度 RT 作为纹理采样恒为 0

| 项目 | 内容 |
|------|------|
| **严重度** | 中（用带 mip 过滤的采样器读深度纹理时必现，如 SSAO、软粒子、深度可视化） |
| **位置** | `buildRenderTextureResources` 的非 MSAA 深度分支（`T3DGL4Context.cpp`） |
| **问题** | 深度纹理没有设置 `GL_TEXTURE_MAX_LEVEL`，默认 1000 而只有 mip 0。sampler object 会覆盖纹理自身的 `GL_NEAREST`，引擎的 `kPoint` + `MipFilter = kPoint` 映射为 `GL_NEAREST_MIPMAP_NEAREST`，纹理因 mip 链不完整而采样恒返回 0。颜色 RT 早已设置 `MAX_LEVEL = 0`，深度 RT 漏了。由 ComputeApp K17 发现：4 种深度格式 `shaderReadable` 的组读回 R 全为 0，而深度/模板测试的竖带全部通过 |
| **D3D11** | 深度 SRV 只有一级 mip，不受影响 |
| **状态** | ✅ 已修复（2026-11-19），深度分支补 `glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0)`。GLES3 用 `glTexStorage2D(levels = 1)` 建不可变纹理，不受影响 |

---

## E.2 次要差距

| 编号 | 项 | 位置 | 说明与修复建议 | 优先级 |
|:----:|----|------|----------------|:------:|
| E.2.1 | 常量缓冲忽略 `startSlot`，`nullptr` 不解绑 | `stageConstantBuffers`（`T3DGL4Context.cpp:4687-4719`，`nullptr` 分支 4697-4700）；`bindPendingUniformBlocks`（4536-4572） | 所有阶段的 CB 按 cbuffer 名放进同一个 `mPendingUBOs`，`startSlot` 不起作用。GLSL 中同名 uniform block 在一个 program 内本来就是同一个 block，跨阶段同名覆盖通常无害。但传入 `nullptr` 时直接跳过，旧 UBO 一直留在表里，无法解绑。建议 `nullptr` 时按槽位找到对应名字并移除，或在接口注释中写明 GL 按名绑定、`startSlot` 无效 | P2 |
| E.2.2 | 一张纹理配两个采样器时第二个丢失 | 反射 `T3DGL4ContextBase.cpp:950-977`；`setupSamplerBindings`（`T3DGL4Context.cpp:4583-4596`） | SPIRV-Cross 为「纹理 A + 采样器 S1」与「纹理 A + 采样器 S2」生成两个 combined sampler，反射按纹理名 `A` 建键，后者覆盖前者；`setupSamplerBindings` 把两个 uniform 指到同一个 unit，而一个 unit 只能绑一个 sampler object。建议以「纹理名 + 采样器名」为键，各分配一个 unit | P3 |
| E.2.3 | `DepthBiasClamp`、`AntialiasedLineEnable` 未处理 | `createRasterizerState`（`T3DGL4Context.cpp:1094-1120`）；`GL4RasterizerStateData`（`T3DGL4RenderState.h`） | D3D11 两个字段都使用（`T3DD3D11Context.cpp:1692`、`1697`）。GL4 不存也不用。前者可用 `glPolygonOffsetClamp`（GL 4.6 / `ARB_polygon_offset_clamp`，需按扩展判断），后者对应 `GL_LINE_SMOOTH` | P2 |
| E.2.4 | 反射没有 ByteAddress / Append / Consume 种类 | `reflectShaderAllBindings`（`T3DGL4ContextBase.cpp:1015-1038`） | D3D11 能反射出 `kByteAddressSRV/UAV`、`kAppendUAV`、`kConsumeUAV`；GL4 只有 `kStructuredSRV/UAV`（加 `hasCounter`）、`kTypedSRV/UAV`、`kTextureUAV`。目前只有 ComputeApp 依赖这些 Kind。GLSL 没有直接对应的语义，若上层要跨后端使用，需要约定从命名或 counter 推断 | P3 |
| E.2.5 | 顶点格式不校验 VS 输入 | `createVertexDeclaration`（`T3DGL4Context.cpp:1409-1428`） | D3D11 的 `CreateInputLayout` 需要 VS 字节码，属性不匹配时创建失败；GL4 只创建 VAO，不匹配时静默画错。可在 Program 链接后用 `glGetProgramResource(GL_PROGRAM_INPUT)` 对照 VertexDeclaration，Debug 构建下打警告 | P3 |
| E.2.6 | 创建资源时缺少参数校验 | `createVertexBuffer` / `createIndexBuffer` / `createPixelBuffer*`；`GL4Mapping::getBufferUsage`（`T3DGL4Mapping.cpp:309-318`） | D3D11 会拒绝非法的 Usage / CPUAccess 组合与宽高为 0 的纹理（`T3DD3D11Context.cpp:2561-2593` 等）；GL4 不读 `getCPUAccessMode()`，也不校验尺寸，非法描述只会在驱动层报错。另外 `getInternalFormat` 遇到未知格式回落为 `GL_RGBA8`，建议改为报错 | P2 |
| E.2.7 | 读回没有 staging 池 | `map`（`T3DGL4Context.cpp:3893`、`3951`）、`finishReadback`（5577） | 每次 `map` 新建 staging buffer，`unmap` 后删除。D3D11 有 `acquireStagingBuffer/Texture` 复用池。只影响频繁读回时的分配开销 | P3 |
| E.2.8 | 窗口深度格式固定、忽略 MSAA Quality | `T3DGL4RenderWindow.cpp:128-129`、`159-160` | 像素格式写死 24 位深度 + 8 位模板，D3D11 按 `DepthStencilFormat` 创建；MSAA 只用 Count。属于 WGL 像素格式的限制，可通过 E.1.4 的中间 FBO 方案一并解决 | P3 |
| E.2.9 | SRV / UAV 共用 SSBO 绑定点的约定没有写进 RHI 接口 | `T3DRHIContext.h` `setCSStructuredBuffers` / `setCSUnorderedAccessBuffers` 注释 | 附录 D.3.2 的结论是「调用方把 UAV 槽号排在 SRV 之后」，但这条约定只体现在 `ComputeCases.cpp:46-50` 的 `uavSlot()`。建议写进接口注释，避免新的 compute 调用方踩坑 | P3 |

---

## E.3 插件外围

| 编号 | 项 | 位置 | 说明 | 优先级 |
|:----:|----|------|------|:------:|
| E.3.1 | 编辑器与启动器绑定 D3D11 | `GL4Renderer::getEditorInfo`（`T3DGL4Renderer.cpp`，TODO）；`EditorApp.cpp:387`；`LauncherApp.cpp:44`、`327`；`PlayerApp.cpp:124-129` | 与附录 C.1.1 / D.4.4 为同一项，这里补充现状：默认的 `ImGuiTiny3D` 路径（`USE_DX_IMGUI` 未开启）不依赖 `getEditorInfo`，Editor 只要把 `EditorApp.cpp:387` 改为 `OPENGL4` 即可在 GL4 上运行；Launcher 固定加载 `ImGuiDX11` 并使用 `EditorInfoDX11`，Player 固定使用 D3D11，这两处需要改成按配置选择渲染器 | P2 |
| E.3.2 | 没有 GL 调试输出 | `T3DGL4RenderWindow.cpp:191`；`GL_CHECK_ERROR`（`T3DGL4PrerequisitesBase.h:96-113`） | Debug 构建设置了 `WGL_CONTEXT_DEBUG_BIT_ARB`，但没有调用 `glDebugMessageCallback`，这个标志没有实际作用。D3D11 有 debug layer 与 `ReportLiveDeviceObjects`。建议 Debug 构建下启用 `GL_DEBUG_OUTPUT_SYNCHRONOUS` 并把消息转到日志；可顺带用 `glObjectLabel` 给资源命名 | P3 |
| E.3.3 | Linux 没有请求 Core 4.5 上下文 | `T3DGL4RenderWindow.cpp:310` | Linux 用旧的 `glXCreateContext`，拿到的上下文版本与剖面由驱动决定，也没有 debug 标志；Windows 走 `wglCreateContextAttribsARB` 请求 Core 4.5。建议改用 `glXCreateContextAttribsARB` 与 Windows 对齐 | P3 |

---

## E.4 已知且维持现状的差异（不重复跟踪）

- 颜色 RT 自带 D24S8 深度 RBO，与 D3D11「颜色 RT 不带深度」不一致：见附录 D.2.3，保留现状。
- 压缩纹理：见附录 D.4.2，引擎尚无压缩格式。
- `glBlitFramebuffer` 不能在整数与浮点格式之间转换，D3D11 有全屏四边形 blit 兜底。引擎当前没有整数颜色格式，暂不影响。

---

## E.5 修复优先级

| 优先级 | 编号 | 项 | 改动量 |
|:------:|:----:|----|:------:|
| **P0** | E.1.1 | 资源析构改为投递到 RHI 线程删除（✅ 已修复） | 中 |
| **P0** | E.1.2 | D32FS8 的 `getPixelType` 改为 `GL_FLOAT_32_UNSIGNED_INT_24_8_REV`（✅ 已修复） | 1 行 |
| **P1** | E.1.3 | 纹理单元按阶段分区，消除跨阶段冲突（✅ 已修复） | 中 |
| **P1** | E.1.4 | 窗口目标支持外部深度（中间 FBO）或显式报错 | 中 |
| **P1** | E.1.5 | `createPixelBuffer2D` 支持 MSAA | 小 |
| **P1** | E.1.7 | UAV 槽位越界检查 | 小 |
| **P2** | E.1.6 | RenderTexture 支持 UAV | 小 |
| **P2** | E.2.1 / E.2.3 / E.2.6 / E.3.1 | CB 解绑、DepthBiasClamp 与抗锯齿线、创建参数校验、编辑器与启动器的渲染器选择 | 小～中 |
| **P3** | E.2.2 / E.2.4 / E.2.5 / E.2.7 / E.2.8 / E.2.9 / E.3.2 / E.3.3 | 其余次要项 | 视需求 |

