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
| **返回值** | 成功返回 `GL4PixelBuffer2DPtr`（内含 GLTexture/GLFBO/GLResolveTex/GLResolveFBO；颜色 RT 不带深度附件），失败返回 `nullptr` |
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
| **待确认** | SRV（`set*StructuredBuffers`）与 UAV（`setCSUnorderedAccessBuffers`）都把 `slot` 直接当作 `GL_SHADER_STORAGE_BUFFER` 绑定点，D3D11 的 `t#` / `u#` 两个独立编号空间在 GL 里会撞槽，见附录 D.3.2。scc 生成的 GLSL 已改为链接后按 t# / u# 重新分配绑定点（附录 E.2.4 遗留 ②），只有手写 GLSL 仍按此约定 |
| **功能** | 1) **能力位守卫**：低版本 GL 不支持时通过 `T3D_RHI_UNSUPPORTED` / `T3D_RHI_UNSUPPORTED_PTR` 返回错误；<br>2) **StructuredBuffer / Raw SRV**：`createStructuredBuffer` 创建 `GL_SHADER_STORAGE_BUFFER`（带 counter/append 时额外创建 4 字节 `GL_ATOMIC_COUNTER_BUFFER`）；`set{VS,PS,CS}StructuredBuffers` 委托 `bindStructuredBuffers`，通过 `getGLBufferHandle` 同时支持 `StructuredBuffer`、`VertexBuffer`、`IndexBuffer` 绑定到 SSBO；<br>3) **Buffer & Texture UAV (`setCSUnorderedAccessBuffers`)**：校验 `kGPUUnorderedAccess` 标志，对线性缓冲（`StructuredBuffer`/`VB`/`IB`）绑定 `GL_SHADER_STORAGE_BUFFER`（及 `GL_ATOMIC_COUNTER_BUFFER`），对纹理资源（`PixelBuffer1D/2D/3D`）通过 `getGLImageInfo` 调用 `glBindImageTexture` 并记录到 `mBoundCSImages`；<br>4) **Compute Dispatch & Indirect**：`dispatch` 含组数非零与硬件上限校验，链接激活 `mCurrentComputeProgram` 后调 `glDispatchCompute` 并恢复 `mCurrentProgram`；`dispatchIndirect` / `renderIndexedIndirect` / `renderIndirect` 统一经 `validateIndirectArgs` 校验 `kGPUIndirectArgs` 与 4 字节对齐；<br>5) **Counter 拷贝与 Barrier**：`copyStructureCount` 通过 `glCopyNamedBufferSubData` 将 `GLCounterBuffer` 的 4 字节计数拷入目标缓冲；`uavBarrier` 调用 `glMemoryBarrier`（含 `GL_PIXEL_BUFFER_BARRIER_BIT`）并解绑 `mBoundCSImages` 中的 image unit |

### A.4.7 resizeRenderTexture / resizeRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult resizeRenderTexture(RenderTexture *rt, uint32_t width, uint32_t height)`<br>`TResult resizeRenderTarget(RenderTarget *rt, uint32_t width, uint32_t height)` |
| **实现位置** | T3DGL4Context.cpp:613（`resizeRenderTexture`）、T3DGL4Context.cpp:648（`resizeRenderTarget`） |
| **功能** | `resizeRenderTexture` 校验非空与描述符尺寸一致性后，在 RHI 线程调用 `releaseRenderTextureResources` + `buildRenderTextureResources` 重建 `GLTexture` / `GLFBO` / `GLResolveTex` / `GLResolveFBO`；`resizeRenderTarget` 对窗口类型转调 `resizeRenderWindow`，对纹理类型逐个重建所有颜色 RT 与 DepthStencil RT |

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
| §5.7 `setRasterizerState` | ✅ 调用 `glPolygonMode` + `glCullFace` + ... + DepthClamp | ✅ `DepthClipEnable` 与 `GL_DEPTH_CLAMP` 的映射原先是反的，已于附录 D.1.1 修复。✅ `DepthBiasClamp` 与 `AntialiasedLineEnable` 已于附录 E.2.3 补上 |
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
>
> **2026-11-20 补充**：之后又由 ComputeApp K17 发现第 8 个缺口（E.1.8，深度 RT 作为纹理采样恒为 0，同样落在 `createRenderTexture`）。E.1.1～E.1.8 均已修复，上述接口按行为口径恢复为 ✅；附录 E.2 / E.3 的次要差距仍待处理。

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
| **P1** | `resizeRenderTexture` / `resizeRenderTarget` | 重建 `GLTexture` / `GLFBO` / `GLResolveTex` / `GLResolveFBO` 全套附件 | ✅ 已完成 |
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
| C.2.3 | `setRenderTarget` | ✅ 已完成 | `T3DGL4Context.cpp` | 1) `renderTarget == nullptr` 转调 `resetRenderTarget()`；<br>2) MRT 支持 MSAA 目标，切回较少 RT 时清理残留 `GL_COLOR_ATTACHMENT` 并始终刷新 `glDrawBuffers`，且无外部深度时卸载残留的深度附件；<br>3) Depth-only FBO 改为成员 `mDepthOnlyFBO` 并在 `~GL4Context()` 释放，支持 MSAA 与无模板深度格式 | **P1** |
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
| — | 附录 E | 2026-11-19 对照 D3D11 的差距 review 新发现的问题 | ⚠️ E.1.1～E.1.8、E.2.1～E.2.7 已修复，其余 E.2 / E.3 待修复，见附录 E.5 |

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
- ~~每个颜色渲染纹理都会自带一个 D24S8 深度 RBO（`buildRenderTextureResources`，478-482、538-541 行），MRT 中第 1 个以后的纹理、以及会挂外部深度的 RT 也一样，浪费显存。~~ ✅ 已移除（2026-11-23），见 E.4。
- `render(indexed)` / `renderIndexedInstanced` 每次 draw 都 `glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING)` 做安全检查，可改为由 `setIndexBuffer` 在 RHI 线程记录状态。
- `uavBarrier` 始终使用全量 barrier 位，可按资源类型收窄。

> **处理结论：隐式深度 RBO 已移除（见 E.4），其余三项暂不改动。** `unbindTextureUnits` 是为了防止池化的临时 RT 同时挂在纹理单元和 FBO 附件上形成反馈环，去掉有回归风险。隐式深度 RBO 原先认为被没有外挂深度的 RT 依赖，核查后上层需要深度的目标都已外挂深度纹理，「是否外挂深度」本身就区分了需要深度与不需要深度的 RT，与 D3D11 语义一致。EBO 检查与全量 barrier 开销很小，并且是安全兜底，等 profile 显示它们是热点再收窄。

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
| **结论** | ✔️ 不是缺陷，是调用方约定。GL 只有一套 SSBO 绑定编号，调用方需要把 UAV 槽号排在 SRV 之后，见 `source/Samples/ComputeApp/ComputeCases.cpp:46-50` 的 `uavSlot(srvCount, u)`。后端不做重映射，以免与着色器中由该约定生成的 binding 冲突。新写的 compute 调用方必须遵守这条约定。<br>**更新（2026-11-20）**：scc 生成的 GLSL 已由后端按 t# / u# 重新分配绑定点（见 E.2.4 遗留 ②），这条约定现在只适用于手写 GLSL |

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
| **状态** | ✅ 已修复（2026-11-19），采用中间 FBO 方案。<br>**切换**：窗口目标挂了外部深度时，`setRenderTarget` 改绑 `mWindowProxyFBO`（RGBA8 颜色附件，与窗口同尺寸、同采样数，加外部深度），首次切入时把 backbuffer 当前内容拷进来，保留之前不带深度画上去的内容；切到其他目标、`resetRenderTarget`、`reset` 时拷回 backbuffer 并摘掉外部深度。`swapBackBuffer` 在 present 前拷回，中间 FBO 保持生效，present 之后的 draw 仍按外部深度画。`resizeRenderWindow` 按新尺寸重建颜色附件。窗口不挂深度时行为不变，仍直接画 FBO 0。<br>**方向**：中间 FBO 与纹理目标一样按上原点布局，`mRenderingToFBO` 置 true，投影翻转 Y，所以同一张深度可以在窗口目标与纹理目标之间共用；与 backbuffer 互拷时翻转 Y。MSAA 缓冲之间的 `glBlitFramebuffer` 要求矩形一致、不能翻转，MSAA 窗口先 resolve 到单采样中转纹理再翻转拷贝，拷入时每个样本取同一值，之前内容的子样本细节会丢失，resolve 后的结果不变。<br>**blit**：`blit` 的窗口端点若正是当前画到中间 FBO 的窗口，改指向中间 FBO（`BlitEndpoint` 新增 `window` 字段），读到的是最新内容。<br>**校验**（主线程，失败返回错误且不切换目标）：深度必须是深度格式、采样数与窗口实际采样数一致、尺寸不小于窗口，与 D3D11 的约束对齐。<br>**代价**：每次切入、切出与 present 各一次整屏 blit（MSAA 时两次），只发生在挂了外部深度的窗口目标上。<br>**回归用例**：`ComputeApp` 新增 K19 `window external depth`（D3D11 与 GL4）：窗口不挂深度时清黑、左半屏画白；纹理 RT 挂同一张深度，清 1.0 后上半屏写入 0.25；窗口挂这张深度，全屏以 0.5、LESS 画绿。期望左上白、右上黑、下半屏绿，同时检查窗口原有内容的保留和深度上下方向的一致。窗口为 4x MSAA，深度同为 4x。修复前 GL4 下半屏读回白 / 黑（用的是窗口自带深度）；修复后 D3D11 与 GL4 均 18/18 通过。<br>**顺带改动**：K17 的 `ndcZ` / `createFullscreenVB` 提为文件内自由函数，并新增 `bindVertexBuffer`，供 K19 复用 |

### E.1.5 非 RT 的 MSAA 纹理被静默降为 1x

| 项目 | 内容 |
|------|------|
| **严重度** | 中 |
| **位置** | `createPixelBuffer2D`（`T3DGL4Context.cpp:1836-2003`），整个文件只有 `buildRenderTextureResources`（459 行）读取 `sampleDesc` |
| **问题** | `createPixelBuffer2D` 从不读取 `desc.sampleDesc`，总是创建 `GL_TEXTURE_2D` / `GL_TEXTURE_2D_ARRAY` |
| **D3D11** | `SampleDesc.Count = max(1, desc.sampleDesc.Count)`（`T3DD3D11Context.cpp:2716`） |
| **修复建议** | `sampleDesc.Count > 1` 时走 `glTexStorage2DMultisample`（数组用 `glTexStorage3DMultisample`），`GLTarget` 设为 `GL_TEXTURE_2D_MULTISAMPLE(_ARRAY)`，并拒绝初始数据上传与 UAV |
| **状态** | ✅ 已修复（2026-11-19）。<br>**创建**：`createPixelBuffer2D` 在采样数 > 1 时用 `glTexStorage2DMultisample` / `glTexStorage3DMultisample` 建不可变存储（`fixedsamplelocations = GL_TRUE`），`GLTarget` 设为 `GL_TEXTURE_2D_MULTISAMPLE(_ARRAY)`，`GLMSAACount` 记录采样数。创建前用 `glGetInternalformativ(GL_SAMPLES)` 查该格式的采样数上限，超出时报错。<br>**校验**（主线程，失败返回 `nullptr`）：MSAA 纹理带 mipmaps > 1 或带初始数据时拒绝创建；带 `kGPUUnorderedAccess` 时打 warning 并忽略，与 D3D11 一致（没有按修复建议直接拒绝）。<br>**消费侧**：`bindPixelBuffers` 的 MSAA 分支改用 `GLTarget`，原先写死 `GL_TEXTURE_2D_MULTISAMPLE`，数组会绑错 target；`unbindTextureUnits` 补解绑 `GL_TEXTURE_2D_MULTISAMPLE_ARRAY`；`attachScratchFBO` 补上两个多重采样 target，非 RT 的 MSAA 纹理可以作为 blit 端点挂到临时 FBO。`writeBuffer` 本来就拒绝 MSAA，`map` 对没有 resolve FBO 的 MSAA 纹理返回不支持，`getGLImageInfo` 对 MSAA 返回 0，都保持原样。<br>**D3D11 同类问题**：表中「D3D11 能做」的说法不成立。D3D11 的 `createPixelBuffer2D` 虽然设置了 `SampleDesc.Count`，SRV 维度却写死为 `TEXTURE2D` / `TEXTURE2DARRAY`，MSAA 纹理的 `CreateShaderResourceView` 返回 `E_INVALIDARG`。现改为 `TEXTURE2DMS` / `TEXTURE2DMSARRAY`，并与 GL4 一样在主线程拒绝 mipmaps > 1 与初始数据（原先这两种情况都会在 RHI 线程上 `CreateTexture2D` 失败）。<br>**回归用例**：`ComputeApp` 新增 K20 `MSAA texture`（D3D11 与 GL4）：用 `loadPixelBuffer2D` 建 16x8、4x、`Usage::kStatic`、不带数据的 Texture2DMS 与 3 层 Texture2DMSArray，PS 用 `GetDimensions` / `textureSize` + `textureSamples` 把采样数、宽高、层数编码进颜色后读回。Texture2D 资源固定为 `kImmutable`，不能作为 blit 目标，所以引擎层还没有给非 RT 的 MSAA 纹理写入内容的途径，用例只校验维度。临时去掉修复后，GL4 读回采样数 1、尺寸 1x1，D3D11 SRV 创建失败、读回全 0；修复后 D3D11 与 GL4 均 19/19 通过。<br>**未处理**：GLES3 的 `createPixelBuffer2D` 同样不读 `sampleDesc`，未改 |

### E.1.6 RenderTexture 不能作为 UAV

| 项目 | 内容 |
|------|------|
| **严重度** | 中（compute 写 RT 的后处理路径不可用） |
| **位置** | `buildRenderTextureResources`（`T3DGL4Context.cpp:450-614`） |
| **问题** | 颜色 RT 用 `glTexImage2D` 创建可变存储，且不记录可用于 `glBindImageTexture` 的格式；没有 `kGPUUnorderedAccess` 分支。普通 `createPixelBuffer*` 在 1709、1870、2037、2164 行有该分支，RT 路径没有 |
| **D3D11** | MSAA=1 时给 RT 加 `D3D11_BIND_UNORDERED_ACCESS` 并创建 UAV（`T3DD3D11Context.cpp:638-649`） |
| **修复建议** | 非 MSAA 颜色 RT 带 `kGPUUnorderedAccess` 时改用 `glTexStorage2D` 并写入 `GLInternalFormat`；`getGLImageInfo` 已支持 `GL4PixelBuffer2D`，补齐创建侧即可 |
| **状态** | ✅ 已修复（2026-11-19）。<br>**结论修正**：「不能作为 UAV」的判断不成立。`buildRenderTextureResources` 本来就写入了 `GLInternalFormat`，`setCSUnorderedAccessBuffers` → `getGLImageInfo` 对非 MSAA 的 RT 也会返回纹理句柄；GL 规范只要求被 `glBindImageTexture` 绑定的纹理是完整的，并不要求不可变存储，而 RT 已设置 `MAX_LEVEL = 0`。新增的 K21 在修复前的 GL4 上同样通过（20/20），实际缺的只是创建侧与其他纹理路径不一致，以及 barrier 位不全。<br>**创建**：非 MSAA 颜色 RT 带 `kGPUUnorderedAccess` 时改用 `glTexStorage2D(levels = 1)` 建不可变存储，与 `createPixelBuffer*` 的 UAV 分支一致；MSAA 颜色 RT 或深度 RT 带该标志时打 warning 并忽略，与 D3D11 一致（D3D11 对深度 RT 不报警告，GL4 多报一条）。<br>**barrier**：`uavBarrier` 补上 `GL_FRAMEBUFFER_BARRIER_BIT`（compute 写完后作为 FBO 附件渲染、blit 或 `glReadPixels`）与 `GL_TEXTURE_UPDATE_BARRIER_BIT`（`glGetTextureSubImage` 读回）。原来缺这两位，按规范 image store 的结果对这些操作不保证可见，RTX 5060 上没有表现出来。<br>**回归用例**：`ComputeApp` 新增 K21 `render texture UAV`（D3D11 与 GL4），复用 K5 的 kernel，纹理改用 `RenderBufferManager::loadRenderTexture` 创建（走后端 `createRenderTexture`），compute 写入 (x, y, 128, 255) 后读回逐像素校验。D3D11 与 GL4 均 20/20 通过。<br>**引擎层入口**（2026-11-20）：`TextureManager::createRenderTexture` 与 `RenderTexture::create` / 构造函数末尾新增 `gpuAccess` 参数（默认 `kGPUNone`，原有调用点不受影响），写入 `mDesc.gpuAccess`，由 `PixelBufferT` 与 `shaderReadable` 合并后交给后端；`createResource` 的 `TT_RENDER_TEXTURE` 分支参数个数由 9 改为 10；`RenderTexture::clone` 一并传递 `gpuAccess`。带 UAV 的 RT 现在可以直接构造 `RenderTarget`，`resize` 后后端按同一描述重建，UAV 保留。<br>**回归用例**：`ComputeApp` 新增 K22 `render texture UAV as render target`（D3D11 与 GL4）：用新参数建 32x32 的 RT 并 resize 到 64x64；作为渲染目标清白 → `resetRenderTarget`（D3D11 不允许同一资源同时作为 RTV 与 CS UAV 绑定）→ 作为 UAV 由 compute 写上半部分的行 → `uavBarrier` 并解绑 UAV → 再作为渲染目标在左半屏画绿。期望左半绿、右上为 compute 图案、右下保持白色，覆盖 RT 与 UAV 两个方向的切换以及 resize 后 UAV 的保留。D3D11 与 GL4 均 21/21 通过。<br>**遗留限制**：GLES3 未处理 |

### E.1.7 UAV 槽位不做越界检查

| 项目 | 内容 |
|------|------|
| **严重度** | 中低 |
| **位置** | `setCSUnorderedAccessBuffers`（`T3DGL4Context.cpp:3200`）；能力位 `Base/Source/T3DGL4ContextBase.cpp:123` |
| **问题** | `T3DRHIContext.h:610-611` 约定槽位越界返回 `T3D_ERR_INVALID_PARAM`。GL4 只把 `GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS` 填进 `maxUnorderedAccessSlots`，绑定时从不检查，越界后由驱动报 `GL_INVALID_VALUE`，函数仍返回 `T3D_OK` |
| **D3D11** | `startSlot + size > D3D11_1_UAV_SLOT_COUNT` 时报错返回 |
| **修复建议** | 入口按 `maxUnorderedAccessSlots`（SSBO）与 `GL_MAX_IMAGE_UNITS`（image）校验，越界返回 `T3D_ERR_INVALID_PARAM` |
| **状态** | ✅ 已修复（2026-11-20）。<br>**校验**（主线程入队前，失败返回 `T3D_ERR_INVALID_PARAM`）：一个 UAV 槽同时对应 SSBO 绑定点、image unit 与 atomic counter 绑定点，三者上限不同，按实际用到的分别检查。槽位范围 `[startSlot, startSlot + size)` 不得超过 `maxUnorderedAccessSlots`（按 64 位计算，避免 `startSlot` 很大时加法溢出）；像素缓冲与 `kTyped` 结构化缓冲走 image，槽号还须小于 `GL_MAX_IMAGE_UNITS`；带计数器或 Append/Consume 的结构化缓冲，槽号还须小于 `GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS`。资源类型与计数器标志都从引擎层的 `RenderBuffer` / `StructuredBufferDesc` 读取，不依赖 RHI 线程上才创建的底层资源。<br>**能力位**：`RHICapabilities` 新增 `maxImageUnorderedAccessSlots` 与 `maxCounterUnorderedAccessSlots`，上层可以查到纹理 / Typed 缓冲 UAV 与带计数器缓冲 UAV 各自可用的槽位数。GL4 在 `fillCapabilities` 中取 `GL_MAX_IMAGE_UNITS` / `GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS` 与 `maxUnorderedAccessSlots` 的较小值，`setCSUnorderedAccessBuffers` 按这两个字段校验；D3D11 没有额外限制，两者都等于 `maxUnorderedAccessSlots`。RTX 5060 上 GL4 为 16 / 8 / 8，D3D11 为 64 / 64 / 64。Vulkan 不支持 UAV，GLES3 留待后续，两者暂保持默认值 0。<br>**没有按修复建议把整个范围卡到 image 上限**：image unit 往往比 SSBO 绑定点少，统一卡到最小值会让纯 SSBO 的 UAV 少掉一批可用槽位。<br>**绑定**：原实现对每个槽无条件解绑 image 与 atomic counter，槽号超过对应上限时驱动报 `GL_INVALID_VALUE`。现在只在槽号低于对应上限时才调用 `glBindImageTexture(…, 0, …)` / `glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, …, 0)`，超出的槽本来就不可能绑过这两类资源。<br>**回归用例**：`ComputeApp` 新增 K23 `UAV slot range`（D3D11 与 GL4），记 N = `maxUnorderedAccessSlots`：起点为 N、范围 [N-1, N+1)、起点为 `UINT32_MAX` 三种调用期望返回 `T3D_ERR_INVALID_PARAM`；在 N-1 上绑定、解绑结构化缓冲期望返回 `T3D_OK`，确认检查没有收得过紧。纹理 UAV 与带计数器的结构化缓冲再按各自的上限 I / C 各测一遍：槽 I-1 / C-1 上绑定、解绑期望成功；I / C 小于 N 时，槽 I / C 虽在 N 以内也期望被拒绝。GL4 上 I = C = 8 < N = 16，这两条拒绝分支实际被执行；D3D11 上三者相等，只测成功分支。两个后端均 22/22 通过。<br>**D3D11 同步修改**：越界检查原先按常量 `D3D11_1_UAV_SLOT_COUNT`（64）判断，Feature Level 11.0 下 `maxUnorderedAccessSlots` 只有 8，槽 8～63 会通过检查后由运行时报错。现改为按 `mCapabilities.maxUnorderedAccessSlots` 校验，并同样按 64 位计算范围上界。<br>**遗留**：GLES3 的 `setCSUnorderedAccessBuffers` 同样不做越界检查，也没有填写两个新能力字段，留待单独处理 |

### E.1.8 深度 RT 作为纹理采样恒为 0

| 项目 | 内容 |
|------|------|
| **严重度** | 中（用带 mip 过滤的采样器读深度纹理时必现，如 SSAO、软粒子、深度可视化） |
| **位置** | `buildRenderTextureResources` 的非 MSAA 深度分支（`T3DGL4Context.cpp:902-912`，行号基于 2026-11-20 源码） |
| **问题** | 深度纹理没有设置 `GL_TEXTURE_MAX_LEVEL`，默认 1000 而只有 mip 0。sampler object 会覆盖纹理自身的 `GL_NEAREST`，引擎的 `kPoint` + `MipFilter = kPoint` 映射为 `GL_NEAREST_MIPMAP_NEAREST`，纹理因 mip 链不完整而采样恒返回 0。颜色 RT 早已设置 `MAX_LEVEL = 0`，深度 RT 漏了。由 ComputeApp K17 发现：4 种深度格式 `shaderReadable` 的组读回 R 全为 0，而深度/模板测试的竖带全部通过 |
| **D3D11** | 深度 SRV 只有一级 mip，不受影响 |
| **修复建议** | 非 MSAA 深度分支补 `GL_TEXTURE_MAX_LEVEL = 0`，与颜色 RT 一致 |
| **状态** | ✅ 已修复（2026-11-19），深度分支补 `glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0)`。<br>**同类路径核对**（2026-11-20）：GL4 中只有 mip 0 的 RT 纹理均已封顶，包括非 MSAA 颜色 RT、MSAA 颜色 RT 的 resolve 纹理、深度 RT 以及 E.1.4 的窗口中间 FBO 颜色附件；MSAA 深度 RT 是多重采样纹理，不参与 mip 过滤；`createPixelBuffer*` 按实际上传或分配的层数设置 `MAX_LEVEL`；`GL4ConsoleContext::createRenderTexture` 只返回桩对象。GLES3 用 `glTexStorage2D(levels = 1)` 建不可变纹理，不受影响。<br>**验证**：ComputeApp K17 `depth stencil formats` 的 `shaderReadable` 组（D24S8 / D32F / D32FS8 / D16）读回深度值正确；后续 K18～K23 的回归中 GL4 均为全部用例通过，K17 包含在内 |

---

## E.2 次要差距

| 编号 | 项 | 位置 | 说明与修复建议 | 优先级 |
|:----:|----|------|----------------|:------:|
| E.2.1 | 常量缓冲忽略 `startSlot`，`nullptr` 不解绑 | `stageConstantBuffers`（`T3DGL4Context.cpp:4687-4719`，`nullptr` 分支 4697-4700）；`bindPendingUniformBlocks`（4536-4572） | 所有阶段的 CB 按 cbuffer 名放进同一个 `mPendingUBOs`，`startSlot` 不起作用。GLSL 中同名 uniform block 在一个 program 内本来就是同一个 block，跨阶段同名覆盖通常无害。但传入 `nullptr` 时直接跳过，旧 UBO 一直留在表里，无法解绑。建议 `nullptr` 时按槽位找到对应名字并移除，或在接口注释中写明 GL 按名绑定、`startSlot` 无效<br>**✅ 已修复（2026-11-20）**，两条建议都采纳了。<br>**为什么仍按名字绑定**：GL4 反射给 cbuffer 的 binding 是 glslang 的 block 下标，不是 HLSL 的 `register(b#)`，按槽位去对 program 里的 block 并不可靠。<br>**槽位表**：`mPendingUBOs` 改为每阶段一张「槽位 → cbuffer 名 + UBO」表，`stageConstantBuffers` 增加阶段与 `startSlot` 参数。传入 `nullptr`（或 RHI 资源为空）时移除该槽位；同一阶段只保留同名 CB 最后设置的那个槽位；起始槽位加数量溢出 32 位时返回 `T3D_ERR_INVALID_PARAM`。<br>**绑定**：`bindPendingUniformBlocks` 增加 `isCompute` 参数。compute program 只查 CS 的表；graphics program 只查当前挂了 shader 的 PS / VS / GS / HS / DS 的表，按这个顺序取第一个同名的，因此以 PS 为准，与 sampler 一致。多个阶段给同名 cbuffer 设置了不同的缓冲时打 warning。找不到同名 CB 时绑定 0 并打 warning，不再沿用上一个 program 留在 binding point 上的 UBO。graphics 与 compute 原先共用一张表，CS 的 cbuffer 可能用到 VS / PS 设置的同名缓冲，现在两者分开。<br>**D3D11 同步修改**：`setConstantBuffers` 遇到 `nullptr` 会解引用空指针，现改为传空缓冲，即解绑该槽位；起始槽位加数量超过 `D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT`（14）时返回 `T3D_ERR_INVALID_PARAM`，原先越界的调用由运行时丢弃，没有任何报错。<br>**接口注释**：`T3DRHIContext.h` 中 6 个 `set*ConstantBuffers` 的注释写明 `nullptr` 表示解绑、各阶段槽位相互独立，以及 GL 按 cbuffer 名绑定，要求 `ConstantBuffer` 的名字与 shader 中的 cbuffer 名一致。<br>**回归用例**：`ComputeApp` 新增 K24 `constant buffer slots`（D3D11 与 GL4）。CS 的 b0 / b1 与 PS 的 b0 使用同名 cbuffer（`CBSlotA` / `CBSlotB`），检查三部分：一是返回码，起点 `UINT32_MAX` 期望返回 `T3D_ERR_INVALID_PARAM`，槽 13 上绑定、用 `nullptr` 解绑期望返回 `T3D_OK`；二是 graphics，PS b0 设绿色后 CS 再以 [0, 2) 设同名 CB，左竖带期望仍为绿，PS b0 先用 `nullptr` 解绑再设白色，右竖带期望为白；三是 compute，PS 的同名 CB 都在 CS 之后设置，再以起点 1 单独替换 CS 的 b1，期望读回 b0 = (1, 2, 3, 4)、b1 = (9, 10, 11, 12)。临时换回修复前的 GL4 后三处都失败：起点 `UINT32_MAX` 返回 `T3D_OK`，左竖带读回白色（CS 的值截断），CS 的 b0 读回 PS 的 (1, 1, 1, 1)。修复后 D3D11 与 GL4 均 23/23 通过。修复前的 D3D11 遇到 `nullptr` 会在 RHI 线程崩溃，这一点只按代码确认，没有实际复现。<br>**后续修复（2026-11-21）：以 PS 为准时会取到残留的 CB**。RmlUiApp 在 GL4 下纯色几何（面板背景、边框、圆、按钮）位置与旋转全错，文字正常。`RmlUI-Color` 只有 VS 读 `RmlUIPerDraw`，但 PS 阶段的槽位表里还留着上一次文字绘制（`RmlUI-Texture` 的 PS 也读这个 cbuffer）设的同名 CB，按「以 PS 为准」取到了它，于是纯色几何用的是上一次文字的 Transform / Translation。D3D11 的 PS 不读这个槽位，所以不受影响。与 E.2.5 无关，临时换回 E.2.5 之前的 GL4 截图完全一样。修复：`linkProgram` 用 `glGetActiveUniformBlockiv(GL_UNIFORM_BLOCK_REFERENCED_BY_*_SHADER)` 记下每个 block 被哪些阶段引用（`mProgramUBONames` 改为存 `ProgramUBO { name, stageMask }`），`bindPendingUniformBlocks` 只在引用该 block 的阶段里找同名 CB。另外「多个阶段设置了不同缓冲」的 warning 原来每次重新绑定都打（RmlUiApp 一次运行近两万行），改为每个 program 的每个 block 只打一次。修复后 RmlUiApp 在 GL4 下与 D3D11 截图一致，ComputeApp 在 D3D11 与 GL4 下均 31/31 通过。<br>**遗留**：GLES3 未处理 | ✅ |
| E.2.2 | 一张纹理配两个采样器时第二个丢失 | 反射 `T3DGL4ContextBase.cpp:950-977`；`setupSamplerBindings`（`T3DGL4Context.cpp:4583-4596`） | SPIRV-Cross 为「纹理 A + 采样器 S1」与「纹理 A + 采样器 S2」生成两个 combined sampler，反射按纹理名 `A` 建键，后者覆盖前者；`setupSamplerBindings` 把两个 uniform 指到同一个 unit，而一个 unit 只能绑一个 sampler object。建议以「纹理名 + 采样器名」为键，各分配一个 unit<br>**✅ 已修复（2026-11-20）**。按建议给每一对「纹理 + 采样器」各分配一个单元，纹理与采样器的 GL 绑定改到 draw / dispatch 时进行。<br>**为什么推迟到 draw**：D3D11 的 t# 与 s# 相互独立，同一张纹理可以配多个采样器，一个采样器也可以服务多张纹理，某个单元该挂哪张纹理、哪个 sampler object，要看当前 program 里有哪些合并采样器。原实现假设 t_i 总与 s_i 配对，在 `set*PixelBuffers` / `set*Samplers` 里直接绑到「阶段起点 + 槽位」，这个假设不成立时无法表达。<br>**反射**：新增 `GL4ContextBase::parseCombinedSamplerName` 从 uniform 名拆出纹理名与采样器名，优先按 `<T>sampler<T>`（纹理自带的采样器）拆分，纹理名里含 `sampler` 时也不会拆错；SPIRV-Cross 的 `SPIRV_Cross_DummySampler`（只做 texelFetch）识别为没有采样器；手写 GLSL 的纹理与采样器同名。新增 `assignSamplerBindings` 按 D3D11 的方式编号：纹理按纹理名、采样器按采样器键（采样器名去掉 `sampler` 前缀，与 D3D11 反射一致）各自去重，按首次出现的顺序从 0 编号，分别写入纹理参数的 `texBinding` 与采样器键参数的 `samplerBinding`。`reflectShaderAllBindings` 与 `reflectSamplerBindings` 共用这套逻辑。每张纹理配一个同名采样器的常见写法，编号与原来相同。<br>**链接**：`setupSamplerBindings` 给 program 的每个 sampler uniform 在所属阶段的区间内分配一个单元，把「阶段、纹理槽位、采样器槽位、单元、target」记入 `mProgramSamplerUnits`。samplerBuffer 仍由 `bindStructuredBuffers` 直接绑在「阶段起点 + 槽位」上，分配时跳过这些单元。<br>**绑定**：`bindPixelBuffers` / `bindSamplers` 只把资源记入各阶段的 `mStageTextures` / `mStageSamplers`，并像 D3D11 一样持有引用。`ensureProgramLinked` / `ensureComputeProgramLinked` 调用新增的 `bindPendingTextures`，按当前 program 的表把纹理与 sampler object 绑到各单元，槽位为空时按 sampler 类型的 target 解绑。每次 draw / dispatch 都会重绑，因为纹理创建与上传会在 `GL_TEXTURE0` 上绑定后再解绑。原实现中，绑好 PS t0 之后再创建纹理，PS t0 的纹理就会丢失，这个问题现在一并消除。代价是每次 draw 时每个 sampler uniform 多三次 GL 调用。`reset` 与析构会清空这两张表。<br>**回归用例**：`ComputeApp` 新增 K25 `two samplers one texture`（D3D11 与 GL4）。2x1 纹理左黑右白，PS 用 s0、s1 两个采样器在 u = 0.375 处采样 t0，R 取 s0、G 取 s1；点采样得 0，线性采样在两个纹素中心之间按 0.25 插值得约 64。竖带 0 绑 [点, 线性]，期望 (0, 64)；竖带 1 绑 [线性, 点]，期望 (64, 0)，以确认采样器按槽位而不是按顺序对应。GL 版 shader 按 SPIRV-Cross 的命名写成两个合并采样器。临时换回修复前的 GL4 后，两条竖带都读回 (0, 0)：两个合并采样器写同一个反射参数，纹理槽位被后者改成 1，两个 uniform 都指到没有绑纹理的单元，比原描述的「第二个丢失」更严重。修复后 D3D11 与 GL4 均 24/24 通过。<br>**遗留**：一是槽位按 glslang 反射中首次出现的顺序编号，而不是 HLSL 的 `register(t#/s#)`，因为 SPIRV-Cross 生成的合并采样器不带 binding，GLSL 里拿不到原寄存器号。按反射结果绑定的引擎路径（`ShaderVariantInstance`）不受影响；直接按寄存器号调用 RHI 的代码，若寄存器声明顺序与使用顺序不一致会错位。这是原有限制，本项没有改变。**已修复（2026-11-20）**：DXC 生成 SPIR-V 时不加 binding shift，纹理与采样器的 binding 就是 t# / s#（没写 `register` 时由 DXC 自动编号，D3D11 用的 HLSL 也是从同一份 SPIR-V 经 SPIRV-Cross 生成的，两边号码相同）。`T3DSpirvCrossDriver.cpp` 合并采样器后把两个号码写进名字：`T3D_Combined_t<t#>s<s#|x>l<纹理名长度><纹理名><采样器名>`，`x` 表示只做 texelFetch、没有采样器，例如 `T3D_Combined_t0s1l8_MainTexsampler_MainTex`。纹理名长度让拆分没有歧义；纹理名前不放 `_`，因为纹理名可能以 `_` 开头，SPIRV-Cross 会合并连续的 `__`。纹理或采样器缺 binding 时仍输出旧名。GL4 新增 `parseRegisterCombinedName`，`assignSamplerBindings` 先登记名字里的寄存器号，旧名字再按顺序取未占用的最小槽位，旧资源照常可用。GLES3 仍按合并采样器顺序编号，只在 `stripCombinedSamplerName` 里识别新格式取出纹理名，运行期的重复解析改为调用它（本机没有 GLES3 工程，这部分未编译验证）。回归用例 `ComputeApp` K26 `slots follow HLSL registers`：t0 左黑右白、t1 左白右黑，s0 点、s1 线性，PS 先用 t1 + s1、再用 t0 + s0、最后 Load t0，期望 (191, 0, 255)；GL 版是 scc 的原样输出，按顺序编号会读回约 (0, 191, 0)。K25 的 GL 版保留旧名，覆盖兼容路径。D3D11 与 GL4 均 25/25；TextureApp、ShadowApp、PostProcessingApp、RmlUiApp、SkyboxApp 在 GL4 上运行正常。**已重新生成（2026-11-20）**，全部由 scc 从源 shader 生成，没有手改产物：`assets/samples/meshes` 下的 Default-Material、Skybox-Cubemap、Test-Material、Tiny3DStandard，以及 `RmlUI-Texture.tshader`（含 RmlUiApp Android 工程里的副本），按各自原 UUID 用 `scc <X>.shader -t hlsl,glsl,essl,spirv -u <UUID>` 重烘；RmlUI-Color 没有合并采样器，不用重烘。HLSL 部分与原来相同，GLSL / ESSL 除了改名，还有 SPIRV-Cross 临时变量重新编号、Default-Material / Test-Material 多出新版序列化字段，均来自工具版本差异。`SampleShaders_gl4.h` / `SampleShaders_gles3.h`：`embed-sample-shaders.ps1` 补上 GeometryApp 与 Invert / Tint / Grayscale 三个后处理 PS 的 GL / ES 常量，新增 `-Headers` 参数只处理指定头文件（HLSL / Vulkan 的输出目录仍不完整），先 `scc <X>.pshader -t glsl|essl -N -O0` 生成再嵌入。GL4 版的三个后处理 PS 原先是手改的 `texelFetch(gl_FragCoord)`（46f43158，修 GL4 下后处理异常），与源文件的 `Sample(uv)` 不一致；现把这个修改移到源文件，改为 `_MainTex.Load(int3(input.position.xy, 0))`，scc 生成的 GL / ES 版都是 `texelFetch(T3D_Combined_t0sxl8_MainTex, ...)`（ES 版原来仍是 `texture(uv)`，一并改了）。D3D11 / Vulkan 的嵌入常量没有重新生成，仍是 `Sample(uv)`，全屏同尺寸的后处理下两者结果相同。Lit / Forward PS 除改名外，新版 SPIRV-Cross 把 `max` 展开为处理 NaN 的写法、三目运算展开为 if，语义不变；`COMPUTE_LINEAR_WRITE_GL(ES)` 由占位串变成实际代码（目前没有地方使用）。验证：15 个用到这些常量的 sample 编译通过，ComputeApp 在 D3D11 与 GL4 上均 26/26，GeometryApp、TextureApp、LightApp、ShadowApp、SkyboxApp、PostProcessingApp、SceneApp、DirectionalLightApp 在 GL4 上启动运行正常，没有新增错误或警告（SkyboxApp 每帧一条 `tiny3d_ProjectionParamsCB` 跨阶段槽位不同的 warning，重烘前就有，属于 E.2.1 的提示，与本项无关）。二是 `ShaderVariantInstance` 按「每张纹理一个采样器」组织，第二个采样器在反射中是一个只有 `samplerBinding` 的参数，与 D3D11 反射的结果相同，材质系统仍不能直接使用这种写法。**第一步已完成（2026-11-20）**：`ShaderSamplerParam` 新增不序列化的 `hasTexture` / `hasSampler`，各后端的 `reflectShaderAllBindings` / `reflectSamplerBindings` 先 `clearUsage()` 再按反射结果置位（D3D11、GL4 分别置位；GLES3 仍是合并编号，找到即两者都置；Vulkan 的 combined image sampler 两者都置，refill 为空操作，沿用默认值）。`ShaderVariantInstance` 的纹理表与采样器表分开建，各按自己的槽位从最小到最大排列，同一张表里槽位冲突时记警告并忽略后者，替代原来的断言；只有同一个参数同时拥有纹理与采样器时，`setTexture` 才把纹理的 SamplerState 写到该采样器槽位，其余采样器槽位预先填默认 SamplerState。`PassInstance::bindSamplers` 对采样器与纹理分开判断，只用 `Load` 的 shader 也能绑上纹理；`Material::init` 不再给没有纹理的独立采样器建材质参数。由此修掉了「只用 Load 的纹理」或「独立采样器」与其他资源在槽位上相撞时触发的断言（D3D11 与 GL4 都有）。验证：ComputeApp 在 D3D11 与 GL4 上均 24/24；TextureApp、ShadowApp、PostProcessingApp、RmlUiApp 在两个后端上运行正常，没有断言和槽位冲突警告。**第二步已完成（2026-11-20）**：新增 `SamplerState::inferDescFromName`，按 Unity 内联采样器的写法从名字推断独立采样器：名字按 `_`、数字与大小写边界拆词，不区分大小写；过滤取 point / linear（bilinear，mip 点采样）/ trilinear，寻址取 repeat（wrap）/ clamp / mirror / mirror once / border，紧跟 u、v、w 组成的词时只作用于这些轴（如 `LinearClampU_RepeatV`），compare 启用比较采样（LessEqual）。`ShaderVariantInstance` 建采样器表时，不属于任何纹理的采样器用推断结果，推断不出时用默认（线性、Wrap）并记 debug 日志；纹理自带的采样器仍跟随纹理。顺带修正第一步的遗漏：反射中新建的参数默认两个标志都为真，D3D11、GL4、Vulkan 新建参数后先 `clearUsage()`，否则只有采样器的参数会占纹理表的 0 号槽位、只做 Load 的纹理会占采样器表的 0 号槽位（日志里出现槽位冲突警告）。回归用例 `ComputeApp` K27 `inline sampler by name`：PS 有 t0 + 自带的 samplertex、独立的 samplerPointClamp、只做 Load 的 t1，用 `ShaderVariantInstance` 建表后按 `PassInstance::bindSamplers` 的方式绑定，期望 (0, 255, 255, 64)（独立采样器仍用默认的线性 + Wrap 时约为 (64, 0, 255, 64)）；同时检查三个参数的标志、两张表的起点与大小、s1 的描述，以及几种名字写法的推断结果。D3D11 与 GL4 均 26/26，没有槽位冲突警告；TextureApp、ShadowApp、PostProcessingApp、RmlUiApp、SkyboxApp 在两个后端上运行正常。仍未做：材质里可序列化、可在 Inspector 编辑的采样器属性，各向异性过滤的关键字（GL4 未单独处理 `kAnisotropic`），比较函数只有 LessEqual。**纹理数组已验证（2026-11-20）**：回归用例 `ComputeApp` K28 `texture array slots`，t1 是 2 层 2x1 的 Texture2DArray（第 0 层左白右黑、第 1 层左黑右白），t0 是 2x1 的 RT，s0 点、s1 线性，声明顺序与寄存器号相反；PS 线性采样第 1 层、点采样第 0 层、Load 第 1 层右纹素、线性采样 t0，期望 (64, 255, 255, 191)。GL 版为 scc 原样输出，三个 `sampler2DArray` uniform 共用 t1、各配一个采样器，分到各自的单元。D3D11 与 GL4 均 27/27，GL4 实现不需要改动。另外修正 `SamplerState` / `RasterizerState` / `DepthStencilState::getType()` 都返回 `kBlendState` 的错误，目前没有代码按这个类型分支，不影响行为。三是 GLES3 未处理 | ✅ |
| E.2.3 | `DepthBiasClamp`、`AntialiasedLineEnable` 未处理 | `createRasterizerState`（`T3DGL4Context.cpp:1094-1120`）；`GL4RasterizerStateData`（`T3DGL4RenderState.h`） | D3D11 两个字段都使用（`T3DD3D11Context.cpp:1692`、`1697`）。GL4 不存也不用。前者可用 `glPolygonOffsetClamp`（GL 4.6 / `ARB_polygon_offset_clamp`，需按扩展判断），后者对应 `GL_LINE_SMOOTH`<br>**✅ 已修复（2026-11-20）**。<br>**存储**：`GL4RasterizerStateData` 新增 `depthBiasClamp` 与 `antialiasedLineEnabled`，`createRasterizerState`（`T3DGL4Context.cpp:1426`）从 `RasterizerDesc` 读入。<br>**DepthBiasClamp**：`setRasterizerState`（1669 行起）在开启深度偏移时，clamp 为 0 仍调用 `glPolygonOffset`；非 0 时依次尝试 `glPolygonOffsetClamp`（`ARB_polygon_offset_clamp`）与 `glPolygonOffsetClampEXT`（`EXT_polygon_offset_clamp`），以函数指针是否加载为准。glad 按 GL 3.3 core + 扩展生成，`glPolygonOffsetClamp` 只在驱动列出 ARB 扩展时才加载，所以没有按 GL 4.6 版本号判断。两者都不支持时退回 `glPolygonOffset`，只打一次 warning。GL 的 clamp 语义与 D3D11 相同：大于 0 取 `min`，小于 0 取 `max`，等于 0 不钳制。<br>**AntialiasedLineEnable**：D3D11 只在 `MultisampleEnable` 为 false 时按该字段做 alpha 抗锯齿线；GL 开启多重采样时也会忽略 `GL_LINE_SMOOTH`。这里按 D3D11 的组合，只有 `AntialiasedLineEnable && !MultisampleEnable` 时才 `glEnable(GL_LINE_SMOOTH)`，否则显式关闭。Core profile 去掉的只是宽线，`GL_LINE_SMOOTH` 仍然有效。<br>**回归用例**：`ComputeApp` 新增 K29 `depth bias clamp`（D3D11 与 GL4）。48x8 的颜色 RT 外挂 D24S8 深度，深度清成 0.6，在深度 0.5 处以 LESS、不写深度画绿，分 3 条竖带：不加偏移（期望绿）；常量偏移 0.25 × 2^24，即约 0.25（0.75 不通过，期望黑）；同样的偏移加 `DepthBiasClamp = 0.05`（0.55 通过，期望绿）。临时让 GL4 忽略 clamp 后读回 `GKK`，用例失败；修复后 D3D11 与 GL4 均 28/28 通过。<br>**未验证**：抗锯齿线的效果依赖驱动，没有做读回用例，只确认了 K1～K29 运行时没有新增 GL 错误。<br>**遗留**：GLES3 的 `setRasterizerState` 同样不处理这两个字段，未改。GLES 没有 `GL_LINE_SMOOTH`，clamp 只能用 `GL_EXT_polygon_offset_clamp` | ✅ |
| E.2.4 | 反射没有 ByteAddress / Append / Consume 种类 | `reflectShaderAllBindings`（`T3DGL4ContextBase.cpp:1015-1038`） | D3D11 能反射出 `kByteAddressSRV/UAV`、`kAppendUAV`、`kConsumeUAV`；GL4 只有 `kStructuredSRV/UAV`（加 `hasCounter`）、`kTypedSRV/UAV`、`kTextureUAV`。目前只有 ComputeApp 依赖这些 Kind。GLSL 没有直接对应的语义，若上层要跨后端使用，需要约定从命名或 counter 推断<br>**✅ 已修复（2026-11-20）**，采用按命名推断，约定由 scc 写入、GL4 解析。<br>**原状比描述更差**：用 Vulkan SDK 的 dxc + spirv-cross 实测，DXC 只把种类写在块类型名上（`type_ByteAddressBuffer`、`type_AppendStructuredBuffer_<T>` 等），同一种类的缓冲共用一个块类型，SPIRV-Cross 只有第一个块用类型名声明，其余退化成变量名；计数器是单独的 SSBO 块 `counter_var_<名字>`。glslang 反射按块名上报，所以 scc 生成的 GLSL 在 GL4 上不仅种类全错，资源名也是 `type_RWStructuredBuffer_uint` 这类类型名，计数器块还被当成一个独立的 `kStructuredUAV` 资源。ComputeApp 之前的 compute 用例全部是手写 GLSL，没有暴露出来。<br>**scc**：`T3DSpirvCrossDriver.cpp` 新增 `renameStorageBuffers`，只在生成 GLSL / ESSL 时执行，按 DXC 的块类型名把每个存储缓冲改名为 `T3D_Buffer_<种类>l<原名长度><原名>`，种类为 `sb` / `rwsb` / `bab` / `rwbab` / `asb` / `csb`，计数器为 `cnt` 加所属缓冲的原名；同时清掉块类型名，让每个块都以自己的变量名声明，同类型的第二个缓冲也带种类。例如 `layout(binding = 1, std430) readonly buffer T3D_Buffer_babl8gByteSRV { uint _m0[]; } T3D_Buffer_babl8gByteSRV_1;`。块类型名不在表里的缓冲（如 RasterizerOrdered 系列）保持原样。HLSL / MSL 输出不受影响。<br>**GL4 反射**：新增 `GL4ContextBase::parseBufferBlockName`（`T3DGL4ContextBase.cpp:1212`），`reflectShaderAllBindings` 的缓冲块循环（1037 行起）先收集计数器块，再逐块解析：资源名取 HLSL 原名；种类取自名字；Append / Consume 恒带计数器，RWStructuredBuffer 有对应计数器块时置 `hasCounter`；计数器块本身不作为资源上报；字节地址缓冲的元素字节数置 0，与 D3D11 一致。不带这个格式的块（手写 GLSL、旧资源）保持原有推断：按 `readonly` 区分 Structured SRV / UAV，与缓冲同 binding 的 `atomic_uint` 视为计数器，K6 仍走这条路径。SSBO 名字在运行期没有其他用途（运行期按名字查找的只有 samplerBuffer），改名不影响绑定。<br>**回归用例**：`ComputeApp` 新增 K30 `buffer kinds reflect`（D3D11 与 GL4），只反射不派发。HLSL 声明 Structured SRV、两个同类型的 ByteAddress SRV、Structured UAV、ByteAddress UAV、用了 `IncrementCounter` 的 Structured UAV、Append、Consume 共 8 个缓冲，GL 版是 scc `-t glsl` 的原样输出（含 3 个计数器块）。按名字逐个检查种类、绑定、元素字节数与 `hasCounter`，并要求资源总数为 8（计数器块不能混进来）。修复前的 GL4 连名字都对不上，8 项全部找不到。修复后 D3D11 与 GL4 均 29/29 通过。<br>**遗留（2026-11-20 同日处理）**：<br>① **计数器绑定对不上（✅ 已修复）**：scc 生成的 GLSL 里计数器是独立的 SSBO 块（K30 中 binding 为 5～7），而 GL4 运行期原来把计数器作为 atomic counter buffer 绑在 UAV 槽位上，Append / Consume / `IncrementCounter` 不可用。现在 GL4 在 program 链接后识别 `T3D_Buffer_cnt*` 块，按所属缓冲的 u# 找到 UAV，在 draw / dispatch 时把该缓冲的 `GLCounterBuffer` 作为 SSBO 绑上去。初始计数仍由 `setCSUnorderedAccessBuffers` 写入计数器缓冲，`copyStructureCount` 读的也是它，两条路径共用同一块存储。<br>② **t# 与 u# 撞绑定点（✅ 已修复）**：DXC 的 binding 就是寄存器号，t0 与 u0 会落到同一个 SSBO 绑定点。改为与 E.2.2 纹理相同的思路，SSBO 的 GL 绑定推迟到 draw / dispatch：`set*StructuredBuffers` / `setCSUnorderedAccessBuffers` 只把缓冲记进每阶段的 SRV 表与 CS 的 UAV 表；`linkProgram` 新增 `setupStorageBlockBindings`，枚举 program 的 `GL_SHADER_STORAGE_BLOCK`，能按 `T3D_Buffer_` 约定解析的块从「手写块最大 binding + 1」起依次重新分配绑定点（`glShaderStorageBlockBinding`），并记下它来自哪张表、哪个槽位（SRV 块的阶段按 PS / VS / GS / HS / DS 反射查找，compute 用 CS）；`ensureProgramLinked` / `ensureComputeProgramLinked` 调用新增的 `bindPendingStorageBuffers` 按记录绑定。scc 生成的 shader 因此可以直接按 HLSL 的寄存器号设置槽位，不再需要 `uavSlot()`。手写 GLSL 的块无法解析，保持 binding = 槽位，SRV 与 UAV 共用一张表、后写覆盖先写，D.3.2 / E.2.9 的约定只对手写 GLSL 仍然有效。新增 `GL4ContextBase::mMaxStorageBufferBindings`（`GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS`），SRV 槽位越界时返回 `T3D_ERR_INVALID_PARAM`，重分配超出上限时报错。<br>**行为变化**：`reset` 现在会清空 SRV / UAV 表并解绑手写块用过的 SSBO 绑定点，与 D3D11 的 `ClearState` 一致；原来 reset 之后 SSBO 仍保持绑定。<br>③ **`COMPUTE_LINEAR_WRITE_GL` 是旧格式（✅ 已修复）**：用当前 scc 从 `assets/samples/shaders/ComputeLinearWrite.cshader` 重新生成，再用 `embed-sample-shaders.ps1` 写回 `SampleShaders_gl4.h`，块名变为 `T3D_Buffer_rwsbl7gOutput`，文件里只有这一个常量有改动。<br>**回归用例**：`ComputeApp` 新增 K31 `scc buffer slots and counters`，GL 版是 scc 的原样输出：t0 `StructuredBuffer` 与 u0 `RWStructuredBuffer` 在 GLSL 里都是 binding 0，u1 Append、u2 Consume、u3 带计数器的 `RWStructuredBuffer`，计数器块 binding 为 4～6。槽位直接按 HLSL 寄存器设置，初始计数为 Consume 256、其余 0；dispatch 后校验 u0 的输出（撞槽时会读到自己的输出）、三个计数器经 `copyStructureCount` 读回为 128 / 0 / 256，以及 Append 与计数缓冲的内容。D3D11 与 GL4 均 30/30 通过，K8 / K12 等手写 atomic counter 用例与 K23 / K24 的越界用例不受影响。<br>④ **GLES3（待处理）**：GLES3 不反射存储缓冲，运行期按 binding 绑定，ESSL 输出的块名虽然也改了，但 ① ② 两个问题在 GLES3 上同样存在；本机没有 GLES3 工程，留待后续 | ✅ |
| E.2.5 | 顶点格式不校验 VS 输入 | `createVertexDeclaration`（`T3DGL4Context.cpp:1409-1428`） | D3D11 的 `CreateInputLayout` 需要 VS 字节码，属性不匹配时创建失败；GL4 只创建 VAO，不匹配时静默画错。可在 Program 链接后用 `glGetProgramResource(GL_PROGRAM_INPUT)` 对照 VertexDeclaration，Debug 构建下打警告<br>**✅ 已修复（2026-11-21）**。除了按建议在 Debug 构建下打警告，还把属性改为按语义对到 VS 输入的 location，消除了与 D3D11 的行为差异。<br>**原状比描述更严重**：GL4 把属性在声明里的下标直接当作 attribute location（原 `setVertexBuffers` 里的 `glEnableVertexAttribArray(j)`），而 DXC 按 VS 输入结构体的成员顺序分配 location，两者顺序不同时 GL4 静默读错。实测 ResourceApp 就是这样：`tortoise.tmesh` 的声明顺序为 `POSITION0, COLOR0, TEXCOORD0, NORMAL0, BINORMAL0, TANGENT0, BLENDWEIGHT0, BLENDINDICES0`，`Tiny3DStandard` 的 VS 输入为 `0:POSITION, 1:NORMAL, 2:TEXCOORD0`，GL4 在 location 1 读到的是顶点颜色而不是法线，乌龟的光照一直是错的；D3D11 按语义匹配，结果正确。<br>**D3D11 的实际规则**：输入布局在 `CreateInputLayout` 时按声明关联的 VS 的输入签名把语义对到寄存器。之后换一个签名与之兼容的 VS（如签名是它的前缀）可以复用；换一个输入顺序不同的 VS，D3D11 同样读错（K32 的早期版本在 D3D11 上读回过 "GK"）。<br>**收集**：`createVertexDeclaration`（`T3DGL4Context.cpp:1779`）把每个属性的语义名、语义序号、slot、偏移、类型与实例步进拷进 `GL4VertexDeclaration::Attributes`，语义名由新增的 `GL4Mapping::getVertexSemanticName`（`T3DGL4Mapping.cpp:466`）给出，与 D3D11 输入布局一致（DIFFUSE / SPECULAR 都是 COLOR）。`linkProgram` 在 graphics program 链接成功后调用新增的 `setupVertexInputs`（5697 行），用 `GL_PROGRAM_INPUT` 枚举 VS 输入的名字、location、类型与数组长度，跳过没有 location 的 `gl_` 内建变量，矩阵按列数、数组按元素数计算占用的 location。名字为 `in_var_<语义>`（HLSLCross 的输出）时拆出语义名与序号。结果存入 `mProgramVertexInputs`，并按输入的语义与 location 生成一个签名号，签名相同的 program 对同一个声明得到相同的映射。<br>**映射**（新增 `applyVertexLayout`，5770 行）：`setVertexBuffers`（1887 行）不再直接配置 VAO，只把各 slot 的缓冲、stride、offset 记入 `mVertexBufferBindings` 并递增修订号，与 D3D11 一样独立于顶点声明（原实现要求先设声明再设缓冲）；起始槽位加数量超过 32（`D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT`）时返回 `T3D_ERR_INVALID_PARAM`。`ensureProgramLinked` 在每次 draw 前按当前 program 配置当前声明的 VAO：program 有带语义的输入时，每个属性按「语义名 + 序号」找 VS 输入的 location，矩阵与数组的各列对应依次递增的序号，VS 不读的属性不启用，与 D3D11 忽略多余元素一致；手写 GLSL 拿不到语义，仍以属性下标为 location。VAO 记下自己按哪个签名、哪个修订号配置过（`AppliedSignature` / `AppliedBufferRevision`），两者都没变时 draw 不重设；换签名时先关闭上次启用的 location。GL4 按当前 program 映射，所以 D3D11 读错的「同一声明配输入顺序不同的 VS」在 GL4 上也是对的，这一点比 D3D11 宽松。<br>**校验**（Debug 构建，新增 `validateVertexInputs`，5881 行）：在映射之后执行，每个声明对每个 program 只查一次（记在 `GL4VertexDeclaration::ValidatedPrograms`，随声明释放）。对 VS 输入占用的每个 location 检查：声明里没有对应语义的属性（手写 GLSL 为 location 超出属性个数），输入读到常量 (0, 0, 0, 1)，对应 D3D11 的「缺少语义」；对应属性所在 slot 没设置过顶点缓冲；整数输入配了浮点属性或反过来，GL 下读到的值未定义；double 输入，因为属性只经 `glVertexAttribPointer` / `glVertexAttribIPointer` 提供。分量个数不一致不报，与 D3D11 一致（缺的分量补 0 / 1）。有任一不匹配时再打一条汇总，列出声明的全部属性及其映射到的 location，以及 VS 的全部输入。没有绑定 VAO 时不查，core profile 下 draw 本身就会报 `GL_INVALID_OPERATION`。<br>**回归用例**：`ComputeApp` 新增 K32 `vertex attributes by semantic`（D3D11 与 GL4）。交错顶点为 POSITION + TEXCOORD0 + COLOR0（绿），声明按 COLOR0、TEXCOORD0、POSITION 排列，TEXCOORD0 是 VS 不读的多余属性。同一个声明画 3 条竖带，依次配 VS1（输入 POSITION、COLOR0，输出顶点色）、VS2（只读 POSITION，输出常量绿）、VS1，覆盖换签名后重设 VAO、切回来重新启用 COLOR0 的路径；GL 版都是 scc 的原样输出。期望 "GGG"。临时换回修复前的 GL4 后读回 "KKK"（位置读到的都是 COLOR0 这个常量，三角形退化）；修复后 D3D11 与 GL4 均 31/31 通过。<br>**验证**：GL4 Debug 下运行 ResourceApp、GeometryApp、TextureApp、LightApp、DirectionalLightApp、PointLightApp、SpotLightApp、ShadowApp、SkyboxApp、SceneApp、SkeletalAnimationApp、DynamicBatchApp、PostProcessingApp、RmlUiApp、PrefabApp、CameraApp、TransformationApp、HelloApp、IntersectionApp、BehaviourApp、InputApp，没有 GL 错误，也没有新增警告（SkyboxApp、DynamicBatchApp、RmlUiApp 大量的跨阶段同名 cbuffer 提示属于 E.2.1，改动前就有）。ResourceApp 的乌龟与 D3D11 下的光照一致。校验里实际触发过的只有语义不一致（映射改造前的 ResourceApp），其余几项没有现成用例覆盖，只按代码确认。<br>**遗留**：一是 Release 构建不做校验，映射在所有构建中都生效。二是只有 Debug 构建跑过运行时验证。三是 GLES3 未处理，仍以属性下标为 location | ✅ |
| E.2.6 | 创建资源时缺少参数校验 | `createVertexBuffer` / `createIndexBuffer` / `createPixelBuffer*`；`GL4Mapping::getBufferUsage`（`T3DGL4Mapping.cpp:309-318`） | D3D11 会拒绝非法的 Usage / CPUAccess 组合与宽高为 0 的纹理（`T3DD3D11Context.cpp:2561-2593` 等）；GL4 不读 `getCPUAccessMode()`，也不校验尺寸，非法描述只会在驱动层报错。另外 `getInternalFormat` 遇到未知格式回落为 `GL_RGBA8`，建议改为报错<br>**✅ 已修复（2026-11-23）**。校验都在主线程入队前完成，失败时返回 `nullptr`，与 D3D11 的像素缓冲一致。<br>**Usage / CPUAccessMode**：`GL4Mapping` 新增 `checkUsage`（`T3DGL4Mapping.cpp:310`），规则照搬 `D3D11Mapping::get`：kImmutable 只允许 kCPUNone / kCPURead；kStatic 任意；kDynamic 只允许 kCPUWrite，kCPUNone 打 warning 后放行（D3D11 映射为 DEFAULT）；kCopy 至少要有 kCPURead 或 kCPUWrite。新增 `isDefaultUsage`，对应 D3D11 的 `D3D11_USAGE_DEFAULT`（kStatic，或 kDynamic + kCPUNone），带 `kGPUUnorderedAccess` 的资源必须满足它，与 D3D11 的「UAV 要求 Usage::kStatic」一致。引擎层 `RenderBuffer::validateGPUAccess` 已经会对非 kStatic 的资源去掉 UAV 位，这里只是后端兜底。<br>**接入点**：`T3DGL4Context.cpp` 新增文件内函数 `checkCreateUsage`（284 行）与 `checkCreateFormat`（308 行）。`createVertexBuffer` / `createIndexBuffer` / `createStructuredBuffer` / `createPixelBuffer1D/2D/3D/Cubemap` 检查组合与 UAV；`createConstantBuffer` 只查组合；`createRenderTexture` 只查组合，MSAA / 深度 RT 带 UAV 仍按 E.1.6 打 warning 后忽略。<br>**尺寸**：1D 宽、2D / cubemap / RT 宽高、3D 宽高深为 0 时拒绝，与 D3D11 的 `createPixelBuffer*` 一致。<br>**格式**：`getInternalFormat` 遇到不支持的格式（目前只有 `E_PF_UNKNOWN`）返回 `GL_NONE`，所有纹理创建与 Typed 结构化缓冲在创建前检查，返回 `GL_NONE` 时拒绝。<br>**缓冲 usage hint**：`getBufferUsage` 增加 `accessMode` 参数，kDynamic + kCPUNone 改用 `GL_STATIC_DRAW`，kCopy 只写时改用 `GL_STREAM_DRAW`，只影响驱动的放置策略。<br>**D3D11 同步修改**：一是 `createVertexBuffer` / `createIndexBuffer` / `createConstantBuffer` 的 Usage 映射失败时只 `break`，仍返回一个没有 `D3DBuffer` 的空壳对象，现改为返回 `nullptr`（`createConstantBuffer` 的日志原来误写成 index buffer，一并改正）。二是 `createRenderTexture` 原来把 Usage 校验放在 RHI 线程的 `buildRenderTextureResources` 里，失败时主线程仍拿到空壳对象，现在主线程先查尺寸与 Usage 组合，失败返回 `nullptr`；`buildRenderTextureResources` 里的检查保留，供 resize 路径使用。<br>**回归用例**：`ComputeApp` 新增 K33 `create parameter validation`（D3D11 与 GL4），只建资源不派发，检查 RHI 资源是否为空。期望被拒绝的有：VB kDynamic + kCPURead、VB kImmutable + kCPUWrite、IB kCopy + kCPUNone、CB kDynamic + kCPUReadWrite、2D 纹理 kDynamic + kCPURead、RT kImmutable + kCPUWrite，以及 2D 4x0、1D 宽 0、3D 4x4x0、cubemap 0x0、RT 0x4；对照组 VB kDynamic + kCPUWrite、IB kStatic + kCPURead、CB kDynamic + kCPUWrite、4x4 的 2D 纹理与 RT 期望创建成功。未知格式（2D `E_PF_UNKNOWN`）只在 GL4 上测，D3D11 映射为 `DXGI_FORMAT_UNKNOWN` 后在 RHI 线程失败，主线程仍拿到空壳对象。D3D11 与 GL4 均 32/32 通过，日志中每个非法描述各有一条对应的拒绝记录。没有临时换回修复前的代码跑 K33；按代码，修复前的 GL4 对这些描述都会返回非空资源，D3D11 的 VB / IB / CB 与 RT 也是如此。<br>**验证**：GL4 Debug 下运行 TextureApp、ResourceApp、ShadowApp、SkyboxApp、PostProcessingApp、RmlUiApp、DynamicBatchApp、SceneApp、SkeletalAnimationApp，都能正常退出，没有触发新增的校验错误（TextureApp 的读回拒绝与 SkeletalAnimationApp 的找不到动画片段是 sample 自带的反向测试，改动前就有）。<br>**后续修复（2026-11-23）**：首轮修复留下的两项遗留一并处理。<br>① **D3D11 纹理格式**：`T3DD3D11Context.cpp` 新增文件内函数 `checkCreateFormat`，`D3D11Mapping::get(format)` 为 `DXGI_FORMAT_UNKNOWN`（目前是 `E_PF_UNKNOWN` 与 `E_PF_PALETTE8`）时在主线程拒绝。接入 `createPixelBuffer1D/2D/3D/Cubemap`、`createRenderTexture` 与 Typed 结构化缓冲，原来这些都要到 RHI 线程 `CreateTexture*` / 建视图时才失败，主线程拿到的是空壳对象。GL4 把 `E_PF_PALETTE8` 映射为 `GL_R8`，仍可创建，两个后端在这一点上的差异保留。<br>② **零字节缓冲**：两个后端新增 `checkCreateBufferSize`，`createVertexBuffer` / `createIndexBuffer` / `createConstantBuffer` 在缓冲大小为 0 时拒绝。GL 本身允许大小为 0 的 `glBufferData`，但这样的缓冲什么也写不进去，D3D11 的 `CreateBuffer` 则直接失败，这里统一为拒绝。结构化缓冲不需要另外检查，引擎层 `StructuredBuffer::validateDescriptor` 已保证元素数与元素大小都不为 0。<br>③ **D3D11 绑定空资源会崩溃**：首轮修复让 D3D11 的 VB / IB 校验失败时返回 `nullptr`，零字节拒绝又扩大了这个范围，而 D3D11 的 `setVertexBuffers` / `setIndexBuffer` 会直接解引用 RHI 资源，引擎把这样的缓冲交给它们时会在 RHI 线程崩溃。现在两者对 RHI 资源为空的缓冲（以及 `setIndexBuffer(nullptr)`）按解绑处理，与 GL4 一致。D3D11 中按 RHI 资源取 `ID3D11Buffer` / SRV / UAV 的其余路径原本就先判空。<br>**回归用例**：K33 新增 0 字节的 VB / IB / CB（期望拒绝）；2D `E_PF_UNKNOWN` 改为两个后端都测；新增 2D `E_PF_PALETTE8`，D3D11 期望拒绝、GL4 期望创建成功；`record()` 把被拒绝的 VB / IB 交给 `setVertexBuffers` / `setIndexBuffer`，期望返回 `T3D_OK`，且 RHI 线程执行这两条命令时不崩溃。D3D11 与 GL4 均 32/32 通过，关闭窗口后进程正常退出（退出码 0）。D3D11 修复前的崩溃只按代码确认，没有实际复现。<br>**验证**：TextureApp、ResourceApp、ShadowApp、SkyboxApp、PostProcessingApp、RmlUiApp、DynamicBatchApp、SceneApp、SkeletalAnimationApp、GeometryApp、PrefabApp 在 D3D11 与 GL4 的 Debug 下都能正常退出，没有触发任何新增的校验错误。<br>**另外发现（与本项无关，未处理）**：批量连续启动 sample 时，GL4 偶发一次启动时 `RHI thread: wglMakeCurrent failed !`，之后每帧所有 GL 调用都报 `GL_INVALID_OPERATION`（0x0502）。本轮 16 次 GL4 启动里出现 2 次（ResourceApp、GeometryApp 各一次），单独重跑这两个 sample 各两次都没有复现。失败发生在创建任何资源之前，怀疑是主线程把 context 交给 RHI 线程时的时序问题，需要单独排查。<br>**遗留**：GLES3 未处理，留待后续统一处理 | ✅ |
| E.2.7 | 读回没有 staging 池 | `map`（`T3DGL4Context.cpp:3893`、`3951`）、`finishReadback`（5577） | 每次 `map` 新建 staging buffer，`unmap` 后删除。D3D11 有 `acquireStagingBuffer/Texture` 复用池。只影响频繁读回时的分配开销<br>**✅ 已修复（2026-11-23）**，按 D3D11 的做法加了复用池。<br>**池**：缓冲读回与纹理读回（经 `GL_PIXEL_PACK_BUFFER`）的中转都是 GL buffer，所以只需要一种条目。`GL4Context` 新增 `StagingEntry { Buffer, Capacity, InUse }` 与 `mStagingPool`（`T3DGL4Context.h`），以及 `acquireStagingBuffer`（`T3DGL4Context.cpp:7257`）、`releaseStaging`（7319）、`destroyStagingPool`（7400）。租借时在空闲条目里取能装下的最小一块（D3D11 原来取第一块够大的）；没有就新建，容量向上取到 2 的幂、最小 256 字节，便于大小不同的读回复用，用法仍是 `GL_STREAM_READ`。首轮修复时池只增不减，后续修复见下文。<br>**租借与归还**：两个 `map` 的拷贝命令（4613、4833 行）在 RHI 线程上租借，原来的 `glCreateBuffers` + `glNamedBufferData` 移进 `acquireStagingBuffer`，创建失败时请求返回 `T3D_ERR_GL4_CREATE_BUFFER`。`finishReadback`（7169 行）在 `syncRHIThread` 之后由主线程归还，此时 RHI 线程空闲，与 D3D11 的 `releaseStaging` 时机相同；原来经 `releaseGLObjects` 删除 staging 的路径去掉了。拷贝失败或命令没有执行时 `Staging` 为 0，归还是空操作。<br>**映射**：池里的缓冲可能比本次读回大，`glMapNamedBuffer` 改为 `glMapNamedBufferRange(0, TotalBytes, GL_MAP_READ_BIT)`，只映射用到的部分。<br>**析构**：`~GL4Context` 原来逐个删除未完成请求的 staging，现在这些都是池里的条目，改为清空请求表后调用 `destroyStagingPool`，仍在 `makeDummyCurrent()` 之后执行。<br>**回归用例**：`ComputeApp` 新增 K34 `readback staging pool`（D3D11 与 GL4，`ComputeCases.cpp:4822`）。同一帧内发起 5 个读回：16 B、1000 B、16 KB 三个结构化缓冲各读整块，16 KB 那块再读 [4 KB, 4 KB + 400 B) 的部分区间，外加 compute（复用 K5 的 kernel）写满的 8x8 纹理；unmap 按与 map 不同的顺序进行，逐个校验内容。**更正**：首轮修复时认为 K34 排在 K1～K33 之后会走复用路径，这不成立。自检时全部用例在同一帧 map，所有读回同时在途，首轮的 staging 全是新建的（两个后端都是 50 块），K34 实际覆盖的是「并发租借互不覆盖」；复用发生在按 R 重跑时，见下文后续修复的验证。临时让 `acquireStagingBuffer` 忽略 `InUse`（模拟在途请求共用同一块）后，GL4 上 K34 读回的 16 B / 1000 B 缓冲与纹理都被 16 KB 缓冲的数据覆盖，用例失败，前面依赖读回的用例也大面积失败（8/33）；还原后 D3D11 与 GL4 均 33/33 通过，GL4 日志中的错误都来自 K23 / K24 / K33 的反向用例。<br>**后续修复（2026-11-23）：池只增不减、没有上限**。池的条目数取决于同时在途的读回数，通常不大；真正的问题是一次大读回会永久占住显存，以及池里只剩大块时小读回也会租走它。GL4 与 D3D11 按同一套规则修改，都在后端内部完成，不改 RHI 接口。<br>**规则**（文件内常量，`T3DGL4Context.cpp:54` 起、`T3DD3D11Context.cpp:50` 起）：① 单次超过 `kStagingMaxPooledBytes`（16 MB）的读回按实际大小建一次性 staging，归还时直接删除，不进池；② 线性缓冲的容量超过需要的 `kStagingMaxReuseRatio`（4）倍时不复用（下限按 256 字节算，16 字节的请求仍可复用 256 字节的块），D3D11 的纹理本来就要求尺寸精确匹配；③ 闲置超过 `kStagingIdleTimeoutMs`（2000 ms）的条目删除；④ 闲置总量超过 `kStagingMaxIdleBytes`（32 MB）时从最久没用的开始删除。在途条目永远不淘汰。D3D11 的线性 staging 缓冲也改为按 2 的幂建，与 GL4 一致。<br>**数据结构**：两个后端的 `StagingEntry` 新增 `Pooled`（false 为一次性）与 `LastReleaseMs`（steady clock 毫秒）；D3D11 另加 `Bytes`，纹理按宽 × 高 × 深 × 每像素字节数估算，用于总量统计。D3D11 新建条目统一走新增的 `addStagingEntry`（4648 行）。<br>**淘汰时机**：新增 `trimStagingPool`（GL4 7351 行、D3D11 4691 行），在 `swapBackBuffer` 的 RHI 线程命令里、present 之后调用（GL4 181 行、D3D11 446 行）。放在这里是因为读回停止后 `finishReadback` 不会再被调用；有 current context，GL4 可以直接 `glDeleteBuffers`。超时按毫秒计，多窗口下每帧多次 present 不影响。<br>**一次性资源的释放**：`releaseStaging`（GL4 7319 行、D3D11 4660 行）遇到一次性条目时从池里移除并释放。GL4 主线程调用时没有 context，交给 `releaseGLObjects` 投递到 RHI 线程删除；D3D11 直接 `Release`（设备没有 `SINGLETHREADED` 标志，COM 释放可跨线程）。<br>**线程**：主线程只在 `finishReadback` 的 `syncRHIThread` 之后改池子，此时 RHI 线程空闲；`trimStagingPool` 与租借都在 RHI 线程上串行执行，两边不会同时修改。D3D11 头文件里「只在 RHI 线程上租借 / 归还」的注释与实际不符（读回结果一直由主线程归还），一并改正。<br>**日志**：新建（区分入池 / 一次性）、一次性释放、超时淘汰、超总量淘汰都打 `T3D_LOG_DEBUG`，带字节数与当前条目数。<br>**回归用例**：`ComputeApp` 新增 K35 `large readback`（D3D11 与 GL4，`ComputeCases.cpp:5012`），同一帧读回 18 MB 与 64 B 两个结构化缓冲并逐元素校验，前者走一次性路径、后者走池。<br>**验证**：一是常规运行，D3D11 与 GL4 均 34/34。日志显示首轮新建 50 块入池 staging 和 1 块一次性 staging（18 MB），一次性的在 unmap 后立即释放；约 2 秒后 50 块全部超时淘汰，池子清空到 0。二是临时把超时改为 60 秒、总量上限改为 1 MB，并让 ComputeApp 首轮结束后自动重跑一次（发送 R 键没有送达 SDL 窗口，所以改了代码）：首轮后的 present 按总量淘汰 29 块、剩 21 块；第二轮只新建 29 块，复用了剩下的 21 块；两轮均 34/34，两个后端结果一致。临时改动已全部还原并重新编译，还原后再跑一次仍是 34/34。<br>**遗留**：没有测量频繁读回时的实际开销变化；参数是常量，没有做成可配置；没有窗口、从不 present 的用法（目前两个后端都没有）不会触发淘汰。GLES3 的读回仍是每次新建 | ✅ |
| E.2.8 | 窗口深度格式固定、忽略 MSAA Quality | `T3DGL4RenderWindow.cpp:128-129`、`159-160` | 像素格式写死 24 位深度 + 8 位模板，D3D11 按 `DepthStencilFormat` 创建；MSAA 只用 Count。属于 WGL 像素格式的限制。E.1.4 修复后，需要其他深度格式时可以给窗口目标外挂深度绕开；窗口自带深度本身仍是固定格式<br>**✅ 已修复（2026-11-23）**。<br>**结论修正**：标题所说的两点与 D3D11 并无差异。一是深度格式：`RenderWindowDesc` 没有深度格式字段，`D3D11RenderWindow::DepthStencilFormat` 只有默认值 D24S8，全工程没有赋值的地方，两个后端的窗口深度都固定为 D24S8。二是 MSAA Quality：D3D11 的窗口与 RT 都不读 `MSAA.Quality`，一律取 `CheckMultisampleQualityLevels` 返回的最高档；GL 没有 quality 概念，只用 Count 正是与之对应的行为。因此没有给 `RenderWindowDesc` 新增深度格式字段，需要其他深度格式时仍按 E.1.4 给窗口目标外挂深度。<br>**实际缺陷**：WGL / GLX 的 `WGL_SAMPLES_ARB`、`WGL_DEPTH_BITS_ARB` 等属性都是「不低于」匹配，驱动可能返回更多采样或不同位数的格式，而 GL4 把 `mMSAACount` 直接记成请求值。E.1.4 的外挂深度校验（`checkWindowDepthStencil`）与窗口中间 FBO 都按 `mMSAACount` 工作，与 FBO 0 的实际采样数不一致时，校验会放过采样数不对的深度，中间 FBO 与 backbuffer 之间的 blit 会因采样数不同而失败。另外请求的采样数不支持时直接退回 1x。<br>**GL4**（`T3DGL4RenderWindow.cpp`）：请求的采样数取 `max(Count, 1)`（103 行）；WGL（122 行）与 GLX（272 行）都改为从请求值开始逐级减半尝试，都不支持时才退回不带 MSAA 的格式。窗口上下文 current 之后，以默认帧缓冲的实际属性为准（400 行起）：`glGetIntegerv(GL_SAMPLES)` 写入 `mMSAACount`，深度 / 模板位数用 `glGetFramebufferAttachmentParameteriv(GL_DEPTH / GL_STENCIL)` 查询（先查 `OBJECT_TYPE`，没有该附件时不查位数，避免产生 GL 错误）。实际采样数与请求不同，或深度 / 模板不是 24 / 8 位时打 warning；创建日志带上实际的采样数与位数。`GL_MULTISAMPLE` 也按实际采样数开启。<br>**D3D11 同步修改**（`T3DD3D11Context.cpp:487`）：原实现在请求的采样数不支持时，`CheckMultisampleQualityLevels` 返回 0 档，`uNumQuality - 1` 下溢成 `UINT_MAX`，交换链创建失败；查询本身失败时 `break` 了却没有设置错误码，`createRenderWindow` 仍返回 `T3D_OK`。现改为与 GL4 相同的逐级减半，都不支持时退回 1x，实际采样数与请求不同时打 warning；查询失败返回 `T3D_ERR_D3D11_CREATE_FAILED`。`Count` 为 1 时也直接走 1x，不再查询。<br>**验证**：一是常规运行（4x），ComputeApp 在 D3D11 与 GL4 上均 34/34，GL4 日志为「4x MSAA, depth 24 bits, stencil 8 bits」，窗口创建阶段没有新增警告。二是临时把 `T3DAgent.cpp` 中写死的 4x 改为 32x：GL4 依次尝试 32x、16x，选中 16x 并打出「Window MSAA 32x requested, got 16x」；K19 按窗口描述的 32x 创建外挂深度，被 `checkWindowDepthStencil` 以「sample count 32 does not match window 16」拒绝，用例失败（33/34），这是新校验的预期行为，修复前 GL4 会把窗口记成 32x 而放过这张深度。临时改动已还原并重新编译。<br>**另外发现（与本项无关，未处理）**：32x 请求下 D3D11 的 ComputeApp 在用例执行阶段以退出码 2173 异常退出，日志尾部丢失，窗口创建日志也没写出。临时换回修复前的 D3D11 同样如此，与本项改动无关。怀疑是 `createRenderTexture` 的同类问题（`T3DD3D11Context.cpp` 约 700 行，同样是 `uNumQuality - 1`，相机与用例会按窗口描述的采样数创建 RT），需要单独排查。因此 D3D11 窗口的回退路径只按代码确认，没有实际跑到。（后续修复后已不再出现，见下文）<br>**后续修复（2026-11-23）：窗口降级后，按窗口描述创建的资源对不上**。`Camera::setupRenderTexture`（`T3DCamera.cpp:415`、`419`）与 K19 都按 `RenderWindowDesc::MSAA.Count` 创建 RT / 深度，窗口实际采样数被降级后两者不一致。与窗口尺寸的做法相同，改为在 `RenderWindow::init` 中把实际值回填到窗口描述，调用方不用改。<br>**RHI 接口**：`RHIRenderWindow` 新增受保护成员 `mMSAA` 与 `getMSAADesc()`，后端在 `createRenderWindow` 返回前写入实际值；构造时 `Count = 0`，表示后端没有提供。<br>**引擎层**：`RenderWindow::init`（`T3DRenderWindow.cpp`）在 RHI 窗口创建成功后，`getMSAADesc().Count > 0` 时执行 `mDesc.MSAA = getMSAADesc()`；`RenderWindowDesc::MSAA` 的注释写明 init 成功后可能被实际值覆盖。<br>**GL4**：`GL4RenderWindow::init` 查到默认帧缓冲的采样数后同时写入 `mMSAA`（Quality 为 0）。<br>**D3D11**：交换链在 RHI 线程上异步创建，`createRenderWindow` 返回时主线程还不知道结果，所以把逐级减半的采样数选择挪到主线程、在投递命令之前完成（设备未带 `SINGLETHREADED` 标志，`CheckMultisampleQualityLevels` 可以跨线程调用），结果经 `D3D11RenderWindow::setMSAADesc` 写入，再由 lambda 捕获传给交换链创建。查询失败时直接返回 `nullptr`。回填后描述中的 Quality 是实际的最高档，而不是原来的 0；`Camera` 会把它传给 `createRenderTexture`，但 D3D11 的 RT 路径本来就忽略传入的 quality、取最高档，结果不变。<br>**其他后端**：GLES3 的 EGL 配置与 Vulkan 的窗口都不带 MSAA，实际为 1x。若回填 1x，Android 等平台上相机 RT 的 MSAA 也会随之丢失，所以这两个后端不写 `mMSAA`，沿用请求值，行为与原来相同；Metal、Null 与各 Console 后端同样不写。<br>**验证**：一是常规 4x，ComputeApp 在 D3D11 与 GL4 上均 34/34；PostProcessingApp、ShadowApp 在两个后端上正常退出，没有错误或警告。二是临时把 `T3DAgent.cpp` 中写死的 4x 改为 32x：GL4 降到 16x，D3D11 降到 8x，相机 RT 随之按 16x / 8x 创建，K19 的外挂深度与窗口一致，两个后端均 34/34。D3D11 在 32x 下的异常退出也不再出现，因为相机不再去创建 32x 的 RT，D3D11 窗口的回退路径也因此实际跑到了。临时改动已还原，并做了一次全量 Debug 编译（`RHIRenderWindow` 布局变化，所有插件都需要重编）。<br>**后续修复（2026-11-23）：RT 与 MSAA 纹理请求不支持的采样数**。D3D11 `buildRenderTextureResources` 在 RHI 线程上查询 quality，采样数不支持时 `CheckMultisampleQualityLevels` 返回 0 档，`uNumQuality - 1` 下溢，`CreateTexture2D` 失败，而主线程早已拿到没有底层资源的空壳对象（32x 时 D3D11 异常退出很可能就是这个原因）。同类问题：D3D11 `createPixelBuffer2D` 的 MSAA 纹理要到 RHI 线程上才失败；GL4 的 RT 路径完全不检查采样数；GL4 `createPixelBuffer2D` 虽然有检查，但在 RHI 线程的命令里执行，主线程同样拿到空壳。<br>**处理方式：拒绝而不是降级**。一是同一渲染目标的颜色与深度采样数必须一致，不同格式支持的采样数可能不同，各自降级后组合起来仍不可用；二是 RT 的 `sampleDesc` 会被管线与外挂深度校验读取，降级后要回填，又会碰到跨线程问题；三是窗口已回填实际采样数，相机 RT 不会再请求不支持的值，剩下的只有显式请求的调用方，明确报错更好查。与 E.2.6 一致，在主线程校验，失败返回 `nullptr`。<br>**D3D11**：新增文件内函数 `checkCreateSamples`，在主线程用 `CheckMultisampleQualityLevels` 检查该格式是否支持请求的采样数，查询失败或 0 档时拒绝；接入 `createRenderTexture` 与 `createPixelBuffer2D`。`buildRenderTextureResources` 在 resize 时也会执行，保留查询，0 档时返回 `T3D_ERR_D3D11_CHECK_MULTISAMPLE`，不再减一。Quality 仍取最高档，所有 RT 与窗口都这样取，颜色与深度的 quality 才能一致。<br>**GL4**：开启 RHI 线程后主线程没有 GL context，不能调用 `glGetInternalformativ`。改为在 `GL4ContextBase::fillCapabilities` 中按 `PixelFormat` 查好 2D 与 2D 数组多重采样纹理的最大采样数（`GL_NUM_SAMPLE_COUNTS` 为 0 时记为 0），以及颜色 RT 自带的 `GL_DEPTH24_STENCIL8` renderbuffer 的上限；`ARB_internalformat_query` 不可用时退回 `GL_MAX_COLOR_TEXTURE_SAMPLES` / `GL_MAX_DEPTH_TEXTURE_SAMPLES` / `GL_MAX_SAMPLES`。新增 `getMaxTextureSamples` / `getMaxDepthRBOSamples`，任意线程可调用。新增文件内函数 `checkCreateSamples`，颜色 RT 同时受 renderbuffer 上限约束；接入 `createRenderTexture`、`createPixelBuffer2D`（原来在 RHI 线程上的查询删除），`buildRenderTextureResources` 再查一次作为 resize 路径的兜底。RTX 5060 上 RGBA8 / D24S8 / D32F / 深度 RBO 均为 32。<br>**回归用例**：K33 新增 7 项：颜色 RT、深度 RT（D24S8）、2D 纹理、2D 纹理数组（2 层）请求 64x 期望拒绝，颜色 RT、深度 RT、2D 纹理 4x 期望成功。D3D11 与 GL4 均 34/34。D3D11 对超过 32 的采样数查询本身返回 `E_FAIL`，走的不是原来下溢的分支；临时把颜色 RT 改为 16x（本机 D3D11 的 RGBA8 不支持 16x）只跑 D3D11，日志为「does not support 16x MSAA ! DX ERROR [0]」，确认「查询成功但 0 档」的分支也被拒绝，临时改动已还原。没有临时换回修复前的代码跑 K33。<br>**验证**：全量 Debug 编译（`GL4ContextBase` 布局变化，GL4 Console 插件也要重编）；ComputeApp、PostProcessingApp、ShadowApp 在两个后端上正常退出，没有新增错误或警告。<br>**遗留**：一是 GLES3 / Vulkan 的窗口没有 MSAA，按窗口描述创建的相机 RT 仍是请求的采样数，与窗口不一致，这是原有行为。二是 Linux 路径没有编译验证。三是 GLES3 未处理（包括 RT 与 MSAA 纹理的采样数校验） | ✅ |
| E.2.9 | SRV / UAV 共用 SSBO 绑定点的约定没有写进 RHI 接口 | `T3DRHIContext.h` `setCSStructuredBuffers` / `setCSUnorderedAccessBuffers` 注释 | 附录 D.3.2 的结论是「调用方把 UAV 槽号排在 SRV 之后」，但这条约定只体现在 `ComputeCases.cpp:46-50` 的 `uavSlot()`。建议写进接口注释，避免新的 compute 调用方踩坑。<br>**范围缩小（2026-11-20）**：scc 生成的 GLSL 已按 t# / u# 自动重映射（E.2.4 遗留 ②），约定只剩手写 GLSL 需要遵守<br>**✅ 已修复（2026-11-23）**，只改注释，不改行为。<br>**接口注释**：`T3DRHIContext.h` 中 `setCSUnorderedAccessBuffers` 新增一条 `\note`，写明 GL 后端的两种情况：一是 scc 生成的 GLSL（块名带 `T3D_Buffer_` 前缀）由后端在链接后按 t# / u# 重新分配绑定点，SRV 与 UAV 的插槽直接取 HLSL 寄存器号；二是手写 GLSL 的插槽就是 shader 中的 binding，VS / PS / CS 的 `set*StructuredBuffers` 与 UAV 共用一套编号（GL4 的 `mLegacySSBOs`），后设置的覆盖先设置的，调用方须把 UAV 排在 SRV 之后（GL 下取「SRV 个数 + u#」，D3D11 等后端仍取 u#），shader 中的 binding 也按此编写，计数器是与缓冲同 binding 的 `atomic_uint`。`setVSStructuredBuffers` / `setPSStructuredBuffers` / `setCSStructuredBuffers` 各加一条 `\note` 指向这里。原描述只提到 CS，实际上 VS / PS 的 SRV 在手写 GLSL 下也落在同一张表里，所以三个 SRV 接口都加了说明。<br>**示例**：现有的 `uavSlot(srvCount, u)` 在 `ComputeCases.cpp:50-53` 与 `ParticleSystem.cpp:107-110`，行为不变。<br>**遗留**：GLES3 不反射存储缓冲，scc 生成的 GLSL 也按 binding 直接绑定（见 E.2.4 遗留 ④），注释中「scc 生成的 GLSL 由后端重新分配」目前只对 GL4 成立 | ✅ |

---

## E.3 插件外围

| 编号 | 项 | 位置 | 说明 | 优先级 |
|:----:|----|------|------|:------:|
| E.3.1 | 编辑器与启动器绑定 D3D11 | `GL4Renderer::getEditorInfo`（`T3DGL4Renderer.cpp`，TODO）；`EditorApp.cpp:387`；`LauncherApp.cpp:44`、`327`；`PlayerApp.cpp:124-129` | 与附录 C.1.1 / D.4.4 为同一项，这里补充现状：默认的 `ImGuiTiny3D` 路径（`USE_DX_IMGUI` 未开启）不依赖 `getEditorInfo`，Editor 只要把 `EditorApp.cpp:387` 改为 `OPENGL4` 即可在 GL4 上运行；Launcher 固定加载 `ImGuiDX11` 并使用 `EditorInfoDX11`，Player 固定使用 D3D11，这两处需要改成按配置选择渲染器 | P2 |
| E.3.2 | 没有 GL 调试输出 | `T3DGL4RenderWindow.cpp:191`；`GL_CHECK_ERROR`（`T3DGL4PrerequisitesBase.h:96-113`） | Debug 构建设置了 `WGL_CONTEXT_DEBUG_BIT_ARB`，但没有调用 `glDebugMessageCallback`，这个标志没有实际作用。D3D11 有 debug layer 与 `ReportLiveDeviceObjects`。建议 Debug 构建下启用 `GL_DEBUG_OUTPUT_SYNCHRONOUS` 并把消息转到日志；可顺带用 `glObjectLabel` 给资源命名 | P3 |
| E.3.3 | Linux 没有请求 Core 4.5 上下文 | `T3DGL4RenderWindow.cpp:310` | Linux 用旧的 `glXCreateContext`，拿到的上下文版本与剖面由驱动决定，也没有 debug 标志；Windows 走 `wglCreateContextAttribsARB` 请求 Core 4.5。建议改用 `glXCreateContextAttribsARB` 与 Windows 对齐 | P3 |

---

## E.4 已知且维持现状的差异（不重复跟踪）

- ~~颜色 RT 自带 D24S8 深度 RBO，与 D3D11「颜色 RT 不带深度」不一致~~ **✅ 已修复（2026-11-23）**。GL4 与 GLES3 的 `buildRenderTextureResources` 不再给颜色 RT 建深度 RBO，`GL4PixelBuffer2D` / `GLES3PixelBuffer2D` 去掉 `GLDepthRBO` 字段（窗口自带的 `GLDepthRBO` 不变）。语义与 D3D11 一致：渲染目标没挂深度模板纹理就没有深度，深度测试与模板测试恒通过，`clearDepth*` 不起作用。上层需要深度的目标（相机、编辑器 Scene / Game 视图、阴影）都已显式挂深度纹理，后处理等不挂深度的目标原先会拿未初始化的 RBO 做深度测试。<br>**`setRenderTarget` 同步修改**：FBO 随颜色纹理复用，原先「无外部深度」分支只在 `GLDepthRBO != 0` 时卸下外部深度并挂回 RBO；现改为无条件对 `GL_DEPTH_STENCIL_ATTACHMENT` 挂 0，深度与模板两个挂点一起卸掉。GLES3 原先此分支什么都不做，上次挂的外部深度会一直残留，一并修复。<br>**回归用例**：ComputeApp 新增 K36 `color-only target has no depth`（D3D11 与 GL4）：同一张颜色纹理先配外部 D24S8 并把深度清成 0，再换成只有颜色的目标，以 LESS 在深度 0.5 处画绿，期望全绿。D3D11 与 GL4 均 35 项全部通过；临时去掉卸载深度那一行时 K36 读回全黑，用例能抓住残留深度。GLES3 只做了 Android arm64 编译，未跑运行时验证。
- 压缩纹理：见附录 D.4.2，引擎尚无压缩格式。
- `glBlitFramebuffer` 不能在整数与浮点格式之间转换，D3D11 有全屏四边形 blit 兜底。引擎当前没有整数颜色格式，暂不影响。

---

## E.5 修复优先级

| 优先级 | 编号 | 项 | 改动量 |
|:------:|:----:|----|:------:|
| **P0** | E.1.1 | 资源析构改为投递到 RHI 线程删除（✅ 已修复） | 中 |
| **P0** | E.1.2 | D32FS8 的 `getPixelType` 改为 `GL_FLOAT_32_UNSIGNED_INT_24_8_REV`（✅ 已修复） | 1 行 |
| **P1** | E.1.3 | 纹理单元按阶段分区，消除跨阶段冲突（✅ 已修复） | 中 |
| **P1** | E.1.4 | 窗口目标支持外部深度（中间 FBO）（✅ 已修复） | 中 |
| **P1** | E.1.5 | `createPixelBuffer2D` 支持 MSAA（✅ 已修复） | 小 |
| **P1** | E.1.7 | UAV 槽位越界检查（✅ 已修复） | 小 |
| **P1** | E.1.8 | 深度 RT 补 `GL_TEXTURE_MAX_LEVEL = 0`，修复采样恒为 0（✅ 已修复） | 1 行 |
| **P2** | E.1.6 | RenderTexture 支持 UAV（✅ 已修复） | 小 |
| **P2** | E.2.1 | CB 按阶段与槽位记录，`nullptr` 解绑（✅ 已修复） | 小 |
| **P2** | E.2.3 | DepthBiasClamp 与抗锯齿线（✅ 已修复） | 小 |
| **P2** | E.2.6 | 创建资源时校验 Usage / CPUAccessMode 组合、尺寸与格式（✅ 已修复，GLES3 待处理） | 小 |
| **P2** | E.3.1 | 编辑器与启动器的渲染器选择 | 小～中 |
| **P3** | E.2.2 | 一张纹理配多个采样器，每对纹理 + 采样器各占一个单元，draw 时绑定（✅ 已修复） | 中 |
| **P3** | E.2.4 | 缓冲种类按 scc 写入块名的约定反射；计数器 SSBO 与 t# / u# 绑定点在 draw / dispatch 时按反射重映射（✅ 已修复，GLES3 待处理） | 小 |
| **P3** | E.2.5 | 顶点属性按语义对到 VS 输入的 location，draw 时按 program 配置 VAO；Debug 构建下校验不匹配（✅ 已修复，GLES3 待处理） | 中 |
| **P3** | E.2.7 | 读回 staging 按容量复用，大读回不进池，闲置按超时与总量淘汰；D3D11 同步（✅ 已修复，GLES3 未处理） | 小 |
| **P3** | E.2.8 | 窗口采样数与深度位数以默认帧缓冲的实际值为准，不支持的采样数逐级减半；D3D11 同步修复 quality 下溢（✅ 已修复，GLES3 未处理） | 小 |
| **P3** | E.2.9 | SRV / UAV 共用 SSBO 绑定点的约定写进 RHI 接口注释（✅ 已修复，只改注释） | 注释 |
| **P3** | E.3.2 / E.3.3 | 其余次要项 | 视需求 |

