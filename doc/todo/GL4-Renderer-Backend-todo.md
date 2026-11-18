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
> - GL4 Window 实现：`source/Plugins/Renderer/OpenGL4/Window/Source/T3DGL4Context.cpp`（5029 行）
> - GL4 Window 头文件：`source/Plugins/Renderer/OpenGL4/Window/Include/T3DGL4Context.h`
> - GL4 缓冲资源定义：`source/Plugins/Renderer/OpenGL4/Window/Include/T3DGL4RenderBuffer.h`
> - GL4 基类实现：`source/Plugins/Renderer/OpenGL4/Base/Source/T3DGL4ContextBase.cpp`（512 行）
> - GL4 Console 实现：`source/Plugins/Renderer/OpenGL4/Console/Source/T3DGL4ConsoleContext.cpp`（228 行）
> - 对标基线：`doc/todo/D3D11-Renderer-Backend-todo.md` + `D3D11Context`（Window 版）
>
> 本附录中未标注文件名的行号均指 `T3DGL4Context.cpp`。

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
| **签名** | `void fillCapabilities()`（`GL4Context` 专有，非 RHI 接口） |
| **功能** | 在 `init()` 末尾按 GL 版本与实现限制填充 `mCapabilities`，供上层通过 `getCapabilities()` 查询后决定走 GPU 路径还是 CPU 降级路径 |
| **实现位置** | T3DGL4Context.cpp:106（调用点 T3DGL4Context.cpp:98） |
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
| **实现位置** | T3DGL4Context.cpp:991 |
| **备注** | 须配合 `RasterizerDesc::ScissorEnable = true` 才生效；与 `setViewport` 相互独立 |

### A.4.2 getDepthRemapMatrix

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `const Matrix4& getDepthRemapMatrix() const` |
| **功能** | 返回把光空间 Z 从平台 NDC 范围映射到 [0,1] 的矩阵。GL 系列保持原生 [-1,1] NDC，因此返回 Z 缩放 0.5 + 平移 0.5 的矩阵；D3D11 的投影矩阵已内含 Z remap，用基类默认的单位矩阵 |
| **返回值** | Z remap 矩阵的常引用 |
| **实现位置** | T3DGL4Context.cpp:511 |
| **备注** | 主要供阴影贴图路径把光空间深度写成与采样端一致的范围 |

### A.4.3 createPixelBufferCubemap

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBufferCubemapPtr createPixelBufferCubemap(PixelBufferCubemap *buffer)` |
| **功能** | 创建 RHI 立方体贴图。`glGenTextures` + `glBindTexture(GL_TEXTURE_CUBE_MAP)` + 逐面 `glTexImage2D`（6 个 `GL_TEXTURE_CUBE_MAP_POSITIVE_X + i`）+ `glGenerateMipmap` |
| **参数** | `buffer` — 引擎立方体贴图像素缓冲对象 |
| **返回值** | 成功返回 `GL4PixelBufferCubemapPtr`，失败返回 `nullptr` |
| **实现位置** | T3DGL4Context.cpp:1849 |
| **备注** | Cubemap 是天空盒的前置依赖，见 `doc/todo/Skybox-Support-Design-todo.md` |

### A.4.4 renderIndexedInstanced

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult renderIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance)` |
| **功能** | 索引实例化绘制。`startInstance == 0` 时走 `glDrawElementsInstancedBaseVertex`；非零时走 GL 4.2 的 `glDrawElementsInstancedBaseVertexBaseInstance` |
| **返回值** | `T3D_OK`；`startInstance != 0` 且 `!mCapabilities.supportsBaseInstance`（GL < 4.2）时返回 `T3D_ERR_NOT_IMPLEMENT` |
| **实现位置** | T3DGL4Context.cpp:2670（能力位检查在 T3DGL4Context.cpp:2673） |
| **备注** | 顶点属性的 instance divisor 在 `setVertexBuffers` 中配置 |

### A.4.5 renderInstanced

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult renderInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance)` |
| **功能** | 非索引实例化绘制，`glDrawArraysInstanced` / `glDrawArraysInstancedBaseInstance` |
| **返回值** | `T3D_OK`；`startInstance != 0` 且 GL < 4.2 时返回 `T3D_ERR_NOT_IMPLEMENT` |
| **实现位置** | T3DGL4Context.cpp:2723（能力位检查在 T3DGL4Context.cpp:2726） |

### A.4.6 结构化缓冲与 GPU-driven 接口（11 个）

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（全部 11 个接口均已完整实现） |
| **接口** | `createStructuredBuffer`、`setVSStructuredBuffers`、`setPSStructuredBuffers`、`setCSStructuredBuffers`、`setCSUnorderedAccessBuffers`、`dispatch`、`dispatchIndirect`、`uavBarrier`、`copyStructureCount`、`renderIndexedIndirect`、`renderIndirect` |
| **实现位置** | T3DGL4Context.cpp:2705-3115 |
| **功能** | 1) **能力位守卫**：低版本 GL 不支持时通过 `T3D_RHI_UNSUPPORTED` / `T3D_RHI_UNSUPPORTED_PTR` 返回错误；<br>2) **StructuredBuffer / Raw SRV**：`createStructuredBuffer` 创建 `GL_SHADER_STORAGE_BUFFER`（带 counter/append 时额外创建 4 字节 `GL_ATOMIC_COUNTER_BUFFER`）；`set{VS,PS,CS}StructuredBuffers` 委托 `bindStructuredBuffers`，通过 `getGLBufferHandle` 同时支持 `StructuredBuffer`、`VertexBuffer`、`IndexBuffer` 绑定到 SSBO；<br>3) **Buffer & Texture UAV (`setCSUnorderedAccessBuffers`)**：校验 `kGPUUnorderedAccess` 标志，对线性缓冲（`StructuredBuffer`/`VB`/`IB`）绑定 `GL_SHADER_STORAGE_BUFFER`（及 `GL_ATOMIC_COUNTER_BUFFER`），对纹理资源（`PixelBuffer1D/2D/3D`）通过 `getGLImageInfo` 调用 `glBindImageTexture` 并记录到 `mBoundCSImages`；<br>4) **Compute Dispatch & Indirect**：`dispatch` 含组数非零与硬件上限校验，链接激活 `mCurrentComputeProgram` 后调 `glDispatchCompute` 并恢复 `mCurrentProgram`；`dispatchIndirect` / `renderIndexedIndirect` / `renderIndirect` 统一经 `validateIndirectArgs` 校验 `kGPUIndirectArgs` 与 4 字节对齐；<br>5) **Counter 拷贝与 Barrier**：`copyStructureCount` 通过 `glCopyNamedBufferSubData` 将 `GLCounterBuffer` 的 4 字节计数拷入目标缓冲；`uavBarrier` 调用 `glMemoryBarrier`（含 `GL_PIXEL_BUFFER_BARRIER_BIT`）并解绑 `mBoundCSImages` 中的 image unit |

### A.4.7 resizeRenderTexture / resizeRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult resizeRenderTexture(RenderTexture *rt, uint32_t width, uint32_t height)`<br>`TResult resizeRenderTarget(RenderTarget *rt, uint32_t width, uint32_t height)` |
| **实现位置** | T3DGL4Context.cpp:953（`resizeRenderTexture`）、T3DGL4Context.cpp:988（`resizeRenderTarget`） |
| **功能** | `resizeRenderTexture` 校验非空与描述符尺寸一致性后，在 RHI 线程调用 `releaseRenderTextureResources` + `buildRenderTextureResources` 重建 `GLTexture` / `GLFBO` / `GLDepthRBO` / `GLResolveTex` / `GLResolveFBO`；`resizeRenderTarget` 对窗口类型转调 `resizeRenderWindow`，对纹理类型逐个重建所有颜色 RT 与 DepthStencil RT |

### A.4.8 clearStencil（新增接口）

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult clearStencil(uint32_t stencil)` |
| **实现位置** | T3DGL4Context.cpp:1313 |
| **功能** | 仅清除模板缓冲而不动颜色与深度（供 RmlUi 等 UI 裁剪掩码在场景绘制后、后处理前使用）。与 `clearColor` / `clearDepth` / `clearDepthStencil` 一样配备了 `ClearMaskGuard`（T3DGL4Context.cpp:1181-1235），在 `glClear` 前临时关闭 `GL_SCISSOR_TEST` 并打开全写掩码，结束后恢复管线状态，严格对齐 D3D11 `ClearDepthStencilView` 语义 |

### A.4.9 map / unmap（GPU → CPU 读回，3 个接口）

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `ReadbackHandle map(RenderBuffer *src, size_t offset, size_t size)`<br>`ReadbackHandle map(RenderBuffer *src, const ReadbackRegion &region)`<br>`TResult unmap(ReadbackHandle handle, Buffer &dst)` |
| **实现位置** | T3DGL4Context.cpp:3902（线性缓冲 `map`）、T3DGL4Context.cpp:3959（纹理 `map`）、T3DGL4Context.cpp:4060（`unmap`）、T3DGL4Context.cpp:4907-5025（辅助函数） |
| **功能** | 1) `allocReadbackRequest` 強制校验 `kCPURead` 标志与资源类型匹配；<br>2) 线性缓冲 `map`：使用 DSA `glCreateBuffers` + `glNamedBufferData(GL_STREAM_READ)` + `glCopyNamedBufferSubData` 录制拷贝；<br>3) 2D 纹理 `map`：若为 MSAA 源先通过 `glBlitFramebuffer` resolve 到 `GLResolveTex`，再绑定 `GL_PIXEL_PACK_BUFFER` 调用 `glGetTextureSubImage` 录制子区域读取；<br>4) `unmap`：同步 RHI 线程（`syncRHIThread`），校验 `CopyRecorded` 时序，调用 `glMapNamedBuffer(GL_READ_ONLY)` 拷出紧凑数据后销毁 Staging 缓冲 |
| **注意** | 纹理读回当前支持 `kPixelBuffer2D`（含 MSAA 自动 resolve）；1D/3D/Cubemap 返回 `T3D_ERR_GL4_UNSUPPORTED_OPERATION` |

---

## A.5 对上文第 1～21 章历史条目的修正对照

上文第 1～21 章的描述与状态判断已随代码演进而过时，此处给出与当前最新代码（`T3DGL4Context.cpp`）的对照修正：

| 上文章节 | 上文历史说法 | 当前代码实际实现 |
|---------|-------------|-----------------|
| §1.1 `setViewProjectionTransform` | 使用 `glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE)` 并通过 `conversionMat` 将 Z 重映射到 `[0,1]` | ✅ **已改为保持 OpenGL 原生 `[-1,1]` NDC 深度**（不调用 `glClipControl`，T3DGL4Context.cpp:504-530），仅在 `mRenderingToFBO` 时翻转投影矩阵 Y 轴；光空间深度 `[0,1]` 映射改由 `getDepthRemapMatrix()`（T3DGL4Context.cpp:534）提供 |
| §2.4 `resetRenderTarget` | 仅调用 `glBindFramebuffer(GL_FRAMEBUFFER, 0)` | ✅ **已补齐重置 `mRenderingToFBO = false`**（T3DGL4Context.cpp:741），避免后续 `setRasterizerState` 残留上一个 FBO 的正反面翻转状态 |
| §4.1~4.3 `clearColor` / `clearDepth` / `clearDepthStencil` | 直接调 `glClear*` | ✅ **已引入 `ClearMaskGuard`**（T3DGL4Context.cpp:1181-1235），临时禁用 Scissor 并开启写掩码，防止被前一个 Pass 的 `ColorMask=0` 或 `DepthWrite=false` 拦截 |
| §5.1 / §5.5 `createBlendState` / `setBlendState` | 仅读取 `RenderTargetStates[0]`，不支持独立 RT 混合 | ✅ **已支持 `IndependentBlendEnable`**（T3DGL4Context.cpp:1337-1363、1593-1611），按 `glEnablei` / `glBlendFuncSeparateiARB` / `glBlendEquationSeparateiARB` / `glColorMaski` 逐 RT 配置 |
| §8.2 `setVertexShader` | ⚠️ 缺 `nullptr` 检查，传入 null 会崩溃 | ✅ **已修复**（T3DGL4Context.cpp:2268-2283），支持 `shader == nullptr` 解绑，且非空时改用 `detachShaderStage` 避免冲掉先绑定的 PS |
| §10 / §11 Hull & Domain Shader（10 个接口） | 🔇 按设计为空（返回空对象 / `T3D_OK`） | ✅ **已全部真实现**（T3DGL4Context.cpp:2447-2525），使用 `GL_TESS_CONTROL_SHADER` / `GL_TESS_EVALUATION_SHADER` 编译，通过 `attachGraphicsShader` 挂载 |
| §12.2 `setGeometryShader` | 💬 实现代码被注释禁用 | ✅ **已启用**（T3DGL4Context.cpp:2582-2585），委托 `attachGraphicsShader(shader, mCurrentGSVariant)` |
| §13 Compute Shader（5 个接口） | 🔇 按设计为空 | ✅ **已全部真实现**（T3DGL4Context.cpp:2608-2689），独立维护 `mCurrentComputeProgram` 与 `ensureComputeProgramLinked()` |
| §14.1 `compileShader` (`glslangCompileAndReflect`) | glslang 仅支持 Vertex/Fragment/Geometry 三个阶段 | ✅ **已支持全部 6 个阶段**（含 Hull/Domain/Compute，T3DGL4Context.cpp:2710-2719），并用实测 GL 硬件上限覆盖 `GetDefaultResources()` |
| §17.1~17.4 四个 `blit` 重载 | 仅 `Tex→RT` 实现，其余 3 个 ❌ 未实现 | ✅ **四个重载全部完成**（T3DGL4Context.cpp:3706-3822），统一走 `resolveBlitEndpoint` + `doBlit`（4697-4903），支持 `size == Vector3::ZERO` 整张拷贝、普通纹理临时挂 `mScratchReadFBO`/`mScratchDrawFBO`、以及 MSAA 源直接 resolve |
| §17.5 `copyBuffer` | ❌ 未实现（仅 TODO 注释） | ✅ **已完成**（T3DGL4Context.cpp:3826-3898），使用 DSA `glCopyNamedBufferSubData`，含越界与 `Usage::kImmutable` 检查 |
| §17.6 `writeBuffer` | ⚠️ 纹理分支 width/height 写死为 0 | ✅ **已修复**（T3DGL4Context.cpp:4096-4126），从 `PixelBuffer2D` 描述符读取真实宽高与像素格式，且线性缓冲支持 `kStructuredBuffer` |

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

> **2026-11 源码实测统计（含第三轮部分实现补齐）**：按 `T3DRHIContext.h` 当前全部 **97 个虚接口**统计 `GL4Context`（Window 变体）：
> - 按**接口覆盖**（均已重写并具备核心路径）：**97 / 97 = 100%**
> - 按**细节与边界完整度**：**93 ✅ + 0 ⚠️ + 4 🔇 = 97**（原 19 个 ⚠️ 部分完成接口以及 `GL4Mapping`、纹理数组、`ShaderResourceParams` 反射、Console 变体缺口均已补齐；仅剩余非 `RHIContext` 接口 `GL4Renderer::getEditorInfo` 见附录 C）

| 状态 | 数量 | 接口明细 |
|------|------|----------|
| ✅ 已完成 | **93** | `T3DRHIContext.h` 中除 4 个按设计为空外的全部 93 个虚接口均已完整实现（含第三轮补齐的 `setRenderTarget`、`setViewport`、`setRasterizerState`、`setVertexBuffers`、`createPixelBuffer1D`/`3D`/`Cubemap` 含数组与 Mipmap、`setPixelShader`/`setHullShader`/`setDomainShader`/`setGeometryShader` 空指针 detach、`reflectShaderAllBindings` 含全类型与 `ShaderResourceParams`/`reflectSamplerBindings`、四个 `blit` 重载、`writeBuffer`、`map(region)`） |
| ⚠️ 部分完成 | **0** | 已全部清零 |
| 🔇 按设计为空 | **4** | `beginRender`、`endRender`、`beginPass`、`endPass` |
| **合计** | **97** | `T3DRHIContext.h` 全部 97 个虚接口（92 纯虚 + 5 虚默认） |

### 换算成完成度

- **接口覆盖率**（✅ + ⚠️ + 🔇）：**97 / 97 = 100%**
- **完全无缺口实现率**（✅ + 🔇）：**97 / 97 = 100%**（`RHIContext` 全量 97 个虚接口全部无缺口；仅剩非 `RHIContext` 编辑器对接接口 `GL4Renderer::getEditorInfo` 待编辑器侧扩展）
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

## A.9 待办优先级与完成状态跟踪（第一/二轮已清零，剩余缺口见附录 C）

### 已完成项总表

| 原优先级 | 接口 / 模块 | 解决的问题 | 状态 |
|:--------:|-------------|------------|:----:|
| **P0** | `writeBuffer` 纹理与 SSBO 分支 | 从 `PixelBuffer2D` 描述符取真实 width/height 与像素格式；线性缓冲支持 `kStructuredBuffer` | ✅ 已完成 |
| **P0** | `setVertexShader` | 补齐 `nullptr` 解绑分支，使用 `detachShaderStage` 避免冲掉已挂载的 PS | ✅ 已完成 |
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

**核对结果：这个问题已经修好了。** `T3DGL4Context.cpp:4439-4442` 的分支正确 cast 到 `GL4PixelBufferCubemap` 并使用 `GL_TEXTURE_CUBE_MAP` target。Skybox 文档的 §2.2 / §4.5 待办项可以关闭。

### A.10.4 Shader 变体链路：GL4 侧已验证

`doc/todo/Shader-MultiBackend-Variant-Design-todo.md` 记录 `OPENGL4` → `SHADER_LANGUAGE::kGLSL`（scc target `glsl`），且 ResourceApp 在 D3D11 / GL4 / VK 三个后端间切换均验证成功、GL4 正确取到 `kGLSL` 变体。这条链路无待办。

### A.10.5 相机后处理：四个 `blit` 重载与 MSAA 采样已对齐

`doc/todo/Camera-PostProcess-Design-todo.md` B1–B5 已合，且 GL4 Window 后端的四个 `blit` 重载、`size == Vector3::ZERO` 整张拷贝语义、`bindPixelBuffers` 对 MSAA 渲染纹理优先绑定 `GLResolveTex`（`T3DGL4Context.cpp:4418-4422`）以及 `map`/`unmap` 读回**均已补齐**：

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

在 `source/Plugins/Renderer/OpenGL4/Window/Source/T3DGL4Context.cpp:3374-3405`（及 `T3DGLES3Context.cpp` 同名函数）中：

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
| C.1.1 | `GL4Renderer::getEditorInfo` | ❌ 未实现 | `Window/Source/T3DGL4Renderer.cpp:84-87` | 函数体仅有 `// TODO: 填充编辑器需要的 GL 上下文信息`，未填充任何编辑器信息；编辑器侧（`EditorApp.cpp` / `LauncherApp.cpp`）目前亦仅对接 `EditorInfoDX11`，尚无 GL4 的 Editor 信息结构体与对接实现 | **P2** |
| C.1.2 | `ShaderResourceParams` 反射（SSBO / Image UAV / Atomic Counter） | ✅ 已完成 | `Base/Source/T3DGL4ContextBase.cpp` | `glslangCompileAndReflect` 已采集 `bufferBlocks`（含 `isReadOnly` 与 stride）、`atomicCounterBindings` 与 `image*` 绑定，`reflectShaderAllBindings` 已填充 `ShaderResourceParams` 并调用 `shader->setShaderResourceParams(...)` | **P2** |
| C.1.3 | 纹理数组（`TT_2D_ARRAY` / `TT_CUBE_ARRAY` / 1D Array） | ✅ 已完成 | `Window/Source/T3DGL4Context.cpp` | `createPixelBuffer1D`/`2D`/`Cubemap`、`bindPixelBuffers`、`getGLImageInfo`、`unbindTextureUnits`、`writeBuffer`、`map(region)`、`blit` 均已补齐 `GL_TEXTURE_1D_ARRAY`、`GL_TEXTURE_2D_ARRAY`、`GL_TEXTURE_CUBE_MAP_ARRAY` 支持 | **P2** |

---

## C.2 部分完成 / 存在边界缺口项（⚠️，已全部补齐 ✅）

### C.2.1 管线状态、渲染目标与顶点输入

| 编号 | 接口 | 状态 | 实现位置 | 实现补齐说明 | 优先级 |
|:----:|------|:----:|----------|--------------|:------:|
| C.2.1 | `setVertexBuffers` | ✅ 已完成 | `T3DGL4Context.cpp` | 1) 叠加 `offsets[i]` 与 `attrib.getOffset()`（与 D3D11 语义一致）；<br>2) 循环内对 `buffers[i]` 与 `getRHIResource()` 做判空，同时保持 `mPendingVertexDecl` 至下一次 `setVertexDeclaration` / `reset()` | **P0** |
| C.2.2 | `setPixelShader` / `setHullShader` / `setDomainShader` / `setGeometryShader` | ✅ 已完成 | `T3DGL4Context.cpp` | 统一走 `attachGraphicsShader(..., stageType)`；传入 `nullptr` 时调用 `detachShaderStageByType` 从 `mCurrentProgram` 摘除对应阶段并置 `mProgramDirty = true` | **P0** |
| C.2.3 | `setRenderTarget` | ✅ 已完成 | `T3DGL4Context.cpp` | 1) `renderTarget == nullptr` 转调 `resetRenderTarget()`；<br>2) MRT 支持 MSAA 目标，切回较少 RT 时清理残留 `GL_COLOR_ATTACHMENT` 并始终刷新 `glDrawBuffers`，且无外部深度时恢复 `GLDepthRBO`；<br>3) Depth-only FBO 改为成员 `mDepthOnlyFBO` 并在 `~GL4Context()` 释放，支持 MSAA 与无模板深度格式 | **P1** |
| C.2.4 | `setRasterizerState` | ✅ 已完成 | `T3DGL4Context.cpp` | 根据 `d.multisampleEnabled` 调用 `glEnable/glDisable(GL_MULTISAMPLE)` | **P2** |
| C.2.5 | `setViewport` | ✅ 已完成 | `T3DGL4Context.cpp` | 主路径与 fallback 路径均透传 `glDepthRange(vp.MinDepth, vp.MaxDepth)` | **P3** |

### C.2.2 纹理资源创建与数据传输 / 读回

| 编号 | 接口 | 状态 | 实现位置 | 实现补齐说明 | 优先级 |
|:----:|------|:----:|----------|--------------|:------:|
| C.2.6 | `writeBuffer` | ✅ 已完成 | `T3DGL4Context.cpp` | 1) 纹理分支已完整支持 `1D`/`1D_ARRAY`/`2D`/`2D_ARRAY`/`3D`/`CUBE`/`CUBE_ARRAY`（拒绝 MSAA）；<br>2) 支持单层 mip 0 及完整多级/多层链上传，设 `GL_UNPACK_ALIGNMENT = 1`；<br>3) 增加 `renderBuffer`/`buffer.Data`/`DataSize == 0`/`Usage::kImmutable` 与线性缓冲越界检查 | **P1** |
| C.2.7 | `map(RenderBuffer*, const ReadbackRegion&)` | ✅ 已完成 | `T3DGL4Context.cpp` | 1) 支持 `1D`/`1D_ARRAY`/`2D`（含 MSAA resolve）/`2D_ARRAY`/`3D`/`CUBE`/`CUBE_ARRAY`；<br>2) 正确处理 `region.arraySlice` 与边界校验；<br>3) 显式拦截深度模板格式（`isDepthFormat`）并设 `GL_PACK_ALIGNMENT = 1` | **P1** |
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

