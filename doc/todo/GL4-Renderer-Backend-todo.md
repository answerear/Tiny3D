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

---
---

# 附录 A：完整度复核（2026-08）

> **本附录的作用**：上文第 1～21 章及其汇总统计是按当时的 `RHIContext` 口径（约 64 个接口）写的，此后 RHI 层做过一轮较大的扩容——新增了裁剪矩形、结构化缓冲、Cubemap、实例化/间接绘制、compute dispatch/UAV、渲染目标 resize、以及后端能力集查询。这些新接口在上文中**完全没有出现**，因此上文的「汇总统计」（42 ✅ / 2 ⚠️ / 4 ❌ / 15 🔇 / 1 💬）已不能代表当前完成度。
>
> 本附录以当前代码为准，重新给出**全量口径**下的完整度结论，并修正上文中已经过时的条目。上文保留不动，作为历史记录。
>
> **复核基准**
>
> - 接口定义：`source/Core/Include/RHI/T3DRHIContext.h`（`T3DRHIContext.inl` 为空）
> - 能力集定义：`source/Core/Include/RHI/T3DRHICapabilities.h`
> - GL4 Window 实现：`source/Plugins/Renderer/OpenGL4/Window/Source/T3DGL4Context.cpp`（约 3376 行）
> - GL4 基类实现：`source/Plugins/Renderer/OpenGL4/Base/Source/T3DGL4ContextBase.cpp`
> - GL4 Console 实现：`source/Plugins/Renderer/OpenGL4/Console/Source/T3DGL4ConsoleContext.cpp`
> - 对标基线：`doc/todo/D3D11-Renderer-Backend-todo.md` + `D3D11Context`（Window 版）
>
> 本附录中未标注文件名的行号均指 `T3DGL4Context.cpp`。

---

## A.1 接口口径的变化

`RHIContext` 当前共 **93 个虚接口**：88 个纯虚 + 5 个带默认实现（`getDepthRemapMatrix`、`resizeRenderTexture`、`resizeRenderTarget`、`getNativeContext`、`restoreNativeContext`）。此外 `getCapabilities()` 是非虚 inline getter，读取由后端在 `init()` 中填充的 `mCapabilities` 成员。

上文各章**未覆盖**的新接口共 22 个：

| 分类 | 新接口 | 上文是否提及 |
|------|--------|-------------|
| 裁剪 | `setScissorRect` | ❌ 未提及 |
| 渲染目标 | `resizeRenderTexture`、`resizeRenderTarget` | ❌ 未提及 |
| 深度约定 | `getDepthRemapMatrix` | ❌ 未提及 |
| 资源 | `createStructuredBuffer`、`createPixelBufferCubemap` | ❌ 未提及 |
| 结构化缓冲绑定 | `setVSStructuredBuffers`、`setPSStructuredBuffers`、`setCSStructuredBuffers` | ❌ 未提及 |
| GPU-driven | `setCSUnorderedAccessBuffers`、`dispatch`、`dispatchIndirect`、`uavBarrier`、`copyStructureCount` | ❌ 未提及 |
| 实例化绘制 | `renderIndexedInstanced`、`renderInstanced` | ❌ 未提及 |
| 间接绘制 | `renderIndexedIndirect`、`renderIndirect` | ❌ 未提及 |

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
| `supportsCompute` | `false` | 对应 RHI 接口未实现，保持 false 让上层降级 |
| `supportsUnorderedAccess` | `false` | 同上 |
| `supportsStructuredBuffer` | `false` | 同上 |
| `supportsIndirectDraw` | `false` | 同上 |
| `supportsIndirectDispatch` | `false` | 同上 |
| `supportsAppendConsumeBuffer` | `false` | 同上 |
| `maxDispatchGroupCount[3]` | 从 `GL_MAX_COMPUTE_WORK_GROUP_COUNT` 查询 | **已查询但 `supportsCompute` 为 false**，数据当前无消费者 |
| `maxComputeGroupSize[3]` | 从 `GL_MAX_COMPUTE_WORK_GROUP_SIZE` 查询 | 同上 |
| `maxComputeSharedMemory` | 从 `GL_MAX_COMPUTE_SHARED_MEMORY_SIZE` 查询 | 同上 |
| `maxUnorderedAccessSlots` | 从 `GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS` 查询 | 同上 |

**关键结论**：GL 4.3+ 在 API 层面完全支持 compute shader、SSBO、`glDispatchCompute`、`glMemoryBarrier`、indirect draw，`fillCapabilities` 甚至已经把这些硬件 limit 查了出来。能力位置 false 的原因**不是 GL 不支持，而是 GL4 后端还没写对应实现**。这与 D3D11 的做法形成对比：D3D11 用 `mFeatureLevel >= D3D_FEATURE_LEVEL_11_0` 动态决定，能力位为 true 时接口是真的能跑（`T3DD3D11Context.cpp:164`）。

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
| **状态** | 🚧 契约式不支持 |
| **接口** | `createStructuredBuffer`、`setVSStructuredBuffers`、`setPSStructuredBuffers`、`setCSStructuredBuffers`、`setCSUnorderedAccessBuffers`、`dispatch`、`dispatchIndirect`、`uavBarrier`、`copyStructureCount`、`renderIndexedIndirect`、`renderIndirect` |
| **实现位置** | T3DGL4Context.cpp:2764-2774（每个接口一行） |
| **返回值** | `T3D_ERR_NOT_IMPLEMENT`（指针版返回 `nullptr`），并打警告日志 |
| **对应能力位** | `supportsStructuredBuffer`（4 个）、`supportsUnorderedAccess`（2 个）、`supportsCompute`（1 个）、`supportsIndirectDispatch`（1 个）、`supportsAppendConsumeBuffer`（1 个）、`supportsIndirectDraw`（2 个） |
| **备注** | 实现形态如下，是**规范做法**：<br>`TResult GL4Context::dispatch(...) { T3D_RHI_UNSUPPORTED(supportsCompute); }`<br>GL 4.3+ 原生支持这一整套（`glDispatchCompute` / `glDispatchComputeIndirect` / SSBO / `glMemoryBarrier` / `glDrawElementsIndirect`），属于**尚未实现**而非 API 不支持。设计范围见 `doc/todo/RHI-Compute-UAV-Indirect-Draw-Design-todo.md` |

### A.4.7 resizeRenderTexture / resizeRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ⛔ 未 override |
| **签名** | `TResult resizeRenderTexture(RenderTexture *rt, uint32_t width, uint32_t height)`<br>`TResult resizeRenderTarget(RenderTarget *rt, uint32_t width, uint32_t height)` |
| **返回值** | 基类默认 `T3D_ERR_NOT_IMPLEMENT`（T3DRHIContext.h:125 / T3DRHIContext.h:134） |
| **问题** | 引擎侧 `RenderTexture::resize` 是统一入口，D3D11 已 override 并实现（`T3DD3D11Context.cpp:993` / `T3DD3D11Context.cpp:1029`）。GL4 未实现，意味着**编辑器视口拖拽改变大小、渲染纹理动态调整分辨率这类路径在 GL4 下走不通**。<br>GL4 只有非 RHI 接口 `resizeRenderWindow(GL4RenderWindow*, w, h)`（T3DGL4Context.cpp:458），且仅调 `glViewport`，不重建 FBO/RBO/Resolve 附件 |

---

## A.5 对上文条目的修正

上文若干条目的状态判断已随代码演进而失效，此处逐条修正：

| 上文章节 | 上文说法 | 当前实际 |
|---------|---------|---------|
| §10.1 `createHullShader` | 🔇 按设计为空 | ❌ 未实现。返回**空的 `GL4HullShader` 对象**（非 nullptr）并打警告 `"Hull shader is not supported in OpenGL 3.3"`（T3DGL4Context.cpp:2117）。调用方拿到一个没有 GL handle 的 shader 对象，后续 attach 会失败。归类应为「未实现」而非「按设计为空」——GL 4.5 Core Profile 原生支持 Tessellation |
| §11.1 `createDomainShader` | 🔇 按设计为空 | ❌ 未实现，同上（T3DGL4Context.cpp:2132） |
| §13.1 `createComputeShader` | 🔇 按设计为空 | ❌ 未实现，同上（T3DGL4Context.cpp:2246） |
| §10.3 / §11.3 / §13.3 各 set 系列 | 🔇 按设计为空 | ❌ 未实现。均为 `{ return T3D_OK; }` 单行（T3DGL4Context.cpp:2123-2126、T3DGL4Context.cpp:2138-2141、T3DGL4Context.cpp:2252-2255）。谎报成功，未走 `T3D_RHI_UNSUPPORTED` 契约 |
| §17.6 `writeBuffer` | ⚠️ 纹理分支 width/height 写死为 0 | ⚠️ **缺陷仍然存在，且比上文描述更严重**。除 width/height 为 0 外，格式也硬编码为 `GL_RGBA` / `GL_UNSIGNED_BYTE`，未使用 `PixelBuffer2D` 描述符中的真实像素格式（T3DGL4Context.cpp:2987） |
| §8.2 `setVertexShader` | ⚠️ 缺 nullptr 检查 | ⚠️ **仍未修复**（T3DGL4Context.cpp:1956 直接 `shader->getRHIShader()`）。注意 D3D11 侧同名缺陷**已修复**，见 `D3D11-Renderer-Backend-todo.md` 待办优先级表 |
| §12.2 `setGeometryShader` | 💬 被注释禁用 | 💬 **仍被注释**（T3DGL4Context.cpp:2199-2223，函数体首行即 `return T3D_OK;`，其下 20 行实现全部注释）。对比：GLES3 后端的同名接口**已实现并可用** |
| §2.4 `resetRenderTarget` | ✅ 已完成 | ⚠️ 部分完成。只解绑 FBO，**未重置 `mRenderingToFBO`**（T3DGL4Context.cpp:885）。该标志由 `setRenderTarget` 设置、被 `setRasterizerState` 用于交换 Front/Back CullFace，`resetRenderTarget` 之后若在下一次 `setRenderTarget` 之前调用 `setRasterizerState`，会拿到过期的翻转状态 |
| §21 被注释禁用的功能 | 仅 `setGeometryShader` 一处 | 结论不变，`setGeometryShader` 仍是 GL4 唯一的被注释实现 |
| 汇总统计 | 42 ✅ / 2 ⚠️ / 4 ❌ / 15 🔇 / 1 💬 | 口径已过时，见 A.7 |

> 另外，上文多处把 Hull/Domain/Compute 的缺失归因于代码注释里的 "not supported in OpenGL 3.3"。这些注释本身是错的——GL4 后端创建的是 Core Profile 4.5 上下文，Tessellation（4.0+）与 Compute（4.3+）都在核心规范内。上文 §21 的 P3 项已经指出这点，此处再次确认注释仍未修正。

---

## A.6 GL4 后端当前的三类问题

### A.6.1 静默失败的接口（最高优先级）

以下 19 个接口直接返回 `T3D_OK` 或空对象，**上层无法察觉操作没有发生**：

| 分组 | 接口 | 实现位置 |
|------|------|---------|
| Hull Shader | `createHullShader`、`setHullShader`、`setHSConstantBuffers`、`setHSPixelBuffers`、`setHSSamplers` | T3DGL4Context.cpp:2117-2126 |
| Domain Shader | `createDomainShader`、`setDomainShader`、`setDSConstantBuffers`、`setDSPixelBuffers`、`setDSSamplers` | T3DGL4Context.cpp:2132-2141 |
| Compute Shader | `createComputeShader`、`setComputeShader`、`setCSConstantBuffers`、`setCSPixelBuffers`、`setCSSamplers` | T3DGL4Context.cpp:2246-2255 |
| 数据传输 | `blit(RT→RT)`、`blit(RT→Tex)`、`blit(Tex→Tex)` | T3DGL4Context.cpp:2815、T3DGL4Context.cpp:2927、T3DGL4Context.cpp:2934 |
| 数据传输 | `copyBuffer` | T3DGL4Context.cpp:2941 |

其中 `blit(RT→RT)` 与 `copyBuffer` 带 TODO 注释：

```cpp
// T3DGL4Context.cpp:2815
TResult GL4Context::blit(RenderTarget *src, RenderTarget *dst, ...)
{
    // TODO: implement using glBlitFramebuffer
    return T3D_OK;
}
```

**最低成本的改进不是实现它们，而是先让它们诚实报错**。参考 D3D11 的做法：`copyBuffer` 对不支持的资源类型明确返回 `T3D_ERR_D3D11_UNSUPPORTED_OPERATION` 并打日志（`T3DD3D11Context.cpp:4113`），而不是静默返回成功。

### A.6.2 已知功能缺陷

| 缺陷 | 位置 | 影响 |
|------|------|------|
| `writeBuffer` 纹理分支尺寸为 0、格式硬编码 | T3DGL4Context.cpp:2987 | 纹理动态更新完全不可用 |
| `setVertexShader` 无 nullptr 检查 | T3DGL4Context.cpp:1956 | 传 nullptr 崩溃，与 `setPixelShader`（T3DGL4Context.cpp:2058 有 nullptr 解绑分支）行为不一致 |
| `resetRenderTarget` 不重置 `mRenderingToFBO` | T3DGL4Context.cpp:885 | 面剔除方向可能残留上一个 FBO 的翻转状态 |
| `setGeometryShader` 实现被注释 | T3DGL4Context.cpp:2199 | GS 可编译但永远 attach 不进 program，几何着色器整体不可用 |
| `createBlendState` 仅读 `RenderTargetStates[0]` | T3DGL4Context.cpp:1119 | MRT 下无法为各 RT 配置独立混合 |

### A.6.3 能力缺口（与 D3D11 基线的差距）

| 能力 | D3D11 | GL4 | GL API 是否支持 |
|------|-------|-----|----------------|
| Compute + dispatch | ✅ 完整（FL≥11.0） | 🚧 契约式不支持 | ✅ GL 4.3+ |
| UAV / SSBO | ✅ 完整 | 🚧 契约式不支持 | ✅ GL 4.3+ |
| StructuredBuffer | ✅ 完整 | 🚧 契约式不支持 | ✅ GL 4.3+ SSBO |
| Indirect Draw / Dispatch | ✅ 完整 | 🚧 契约式不支持 | ✅ GL 4.0+ / 4.3+ |
| Append/Consume + `copyStructureCount` | ✅ 完整 | 🚧 契约式不支持 | ⚠️ 需用 atomic counter 手工搭 |
| 实例化绘制 | ✅ 完整 | ✅ 完整 | ✅ |
| 非零 startInstance | ✅ 完整 | ✅ GL 4.2+ 动态判定 | ✅ GL 4.2+ |
| Tessellation（Hull/Domain） | ✅ 完整 | ❌ 未实现 | ✅ GL 4.0+ |
| Geometry Shader | ✅ 完整 | 💬 编译可用、绑定被注释 | ✅ |
| `resizeRenderTexture` / `resizeRenderTarget` | ✅ 完整 | ⛔ 未 override | ✅ |
| 四个 blit 重载 | ✅ 统一走 `resolveBlitEndpoint`+`doBlit` | ⚠️ 仅 `Tex→RT` | ✅ `glBlitFramebuffer` / `glCopyImageSubData` |
| `copyBuffer` | ✅（限线性缓冲） | ❌ 未实现 | ✅ `glCopyBufferSubData` |
| Cubemap 创建 | ✅ 含 mip 链与 Cubemap Array | ✅ 含 6 面 + mipmap | ✅ |
| Shader 反射 | `D3DReflect`（含 CS thread group） | glslang，仅 VS/PS/GS | ⚠️ glslang 支持全阶段，是后端只接了三个 |

---

## A.7 全量口径汇总统计（93 个虚接口）

| 状态 | 数量 | 说明 |
|------|------|------|
| ✅ 已完成 | 53 | 变换 / 渲染目标 / 视口 / 裁剪 / 清除 / 全部管线状态 / 顶点输入 / VBO·IBO·UBO / 1D·2D·3D·Cubemap 纹理 / VS·PS 全套 / GS 创建与资源绑定 / shader 编译与反射 / 图元与基础绘制 / 实例化绘制 / reset / `blit(Tex→RT)` / 深度 remap / 原生上下文 |
| ⚠️ 部分完成 | 3 | `resetRenderTarget`（不重置 FBO 标志）、`setVertexShader`（缺 nullptr 检查）、`writeBuffer`（纹理分支尺寸与格式错误） |
| ❌ 未实现 | 19 | Hull(5) + Domain(5) + Compute set/create(5) + `blit(RT→RT)` + `blit(RT→Tex)` + `blit(Tex→Tex)` + `copyBuffer` |
| 🚧 契约式不支持 | 11 | StructuredBuffer(4) + UAV(2) + `dispatch` + `dispatchIndirect` + `copyStructureCount` + 间接绘制(2) |
| 🔇 按设计为空 | 4 | `beginRender`、`endRender`、`beginPass`、`endPass` |
| 💬 被注释禁用 | 1 | `setGeometryShader` |
| ⛔ 未 override | 2 | `resizeRenderTexture`、`resizeRenderTarget` |
| **合计** | **93** | |

### 换算成完成度

- **有效可用**（✅ + 🚧 + 🔇，即行为符合预期或明确报错）：**68 / 93 ≈ 73%**
- **纯 ✅ 完成度**：**53 / 93 ≈ 57%**
- **对比 D3D11 基线**：71 / 78（旧口径）→ 按 93 全量口径约 84 项可用，GL4 落后主要集中在 GPU-driven 一整套与 Tessellation

> 注：D3D11 的 `doc/todo/D3D11-Renderer-Backend-todo.md` 汇总统计一节仍写着「Compute dispatch + Instanced/Indirect 为 RHI 层缺口、暂缓」，那段话同样已过时——RHI 接口已就位、D3D11 侧也已实现。以代码为准。

---

## A.8 Console 变体（GL4ConsoleContext）

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 定位明确（按设计为 null backend） |
| **位置** | `source/Plugins/Renderer/OpenGL4/Console/Source/T3DGL4ConsoleContext.cpp` |
| **有效接口** | `createRenderWindow`（返回空的 `GL4ConsoleWindow`）、`createVertexShader` / `createPixelShader` / `createGeometryShader`（真实 `glCompileShader`，同步执行不走命令队列）、以及继承自 `GL4ContextBase` 的 `compileShader` / `reflectShaderAllBindings` / `reflectSamplerBindings` |
| **其余接口** | 全部为单行空实现或 `T3D_RHI_UNSUPPORTED` |
| **结论** | 这是刻意的 null backend，用于让 `BundleBuilder` / `BuiltinGenerator` 等命令行工具在无 GPU 上下文时跑通 shader 预编译与反射。**不应该按 Window 后端的标准去「补全」它**，判断口径与 D3D11 Console 一致（见 `D3D11-Renderer-Backend-todo.md` §22） |
| **需要注意的两点** | 1）`GL4ConsoleContext` **从不调用** `fillCapabilities()`，`mCapabilities` 全为默认 false/0，因此连实例化绘制都会返回 `T3D_ERR_NOT_IMPLEMENT`——对工具链场景无影响，但与 Window 变体行为不同<br>2）`getDepthRemapMatrix` 未 override，返回基类单位矩阵。若将来 Console 变体参与任何深度相关的离线烘焙，这里会给出错误的 Z 范围 |

---

## A.9 更新后的待办优先级

### P0 — 修复缺陷

| 接口 | 问题 | 建议 |
|------|------|------|
| `writeBuffer` 纹理分支 | 尺寸传 0、格式硬编码 `GL_RGBA` | 从 `PixelBuffer2D` 描述符取 width/height 与像素格式，经 `GL4Mapping` 转换 |
| `setVertexShader` | 缺 nullptr 检查 | 补 nullptr 解绑分支，与 `setPixelShader` 对齐 |
| Hull/Domain/Compute 的 create 系列 | 返回空对象让调用方误以为成功 | 至少改为返回 `nullptr` 并保留警告日志 |
| Hull/Domain/Compute 的 set 系列 + 三个 blit + `copyBuffer` | 静默返回 `T3D_OK` | 改为返回明确错误码（`T3D_ERR_NOT_IMPLEMENT`），让误用在日志里立刻可见 |

### P1 — 核心功能

| 接口 | 原因 | 实现要点 |
|------|------|---------|
| `setGeometryShader` | 取消注释即可启用，实现代码已写完 | 参考 GLES3 的同名实现（已补 nullptr 检查与 `mProgramDirty` 置位） |
| `blit(RT→RT)` / `blit(RT→Tex)` / `blit(Tex→Tex)` | 数据传输是渲染管线核心 | `glBlitFramebuffer`；建议仿照 D3D11 的 `resolveBlitEndpoint` + `doBlit` 统一收口，避免四个重载各写一遍 |
| `copyBuffer` | 常用操作 | `glCopyBufferSubData` + `GL_COPY_READ_BUFFER`/`GL_COPY_WRITE_BUFFER`；**GLES3 后端已实现，可直接移植** |
| `resizeRenderTexture` / `resizeRenderTarget` | 编辑器视口与动态分辨率依赖 | 重建 GLTexture/GLFBO/GLDepthRBO/GLResolveTex/GLResolveFBO 全套附件 |
| `resetRenderTarget` | 重置 `mRenderingToFBO` | 一行修复 |

### P2 — 能力补齐

| 模块 | 原因 |
|------|------|
| Compute + SSBO + UAV + Indirect 一整套 | GL 4.3+ 原生支持，是 GPU-driven 剔除、GPU readback 等特性的前置依赖。设计范围见 `doc/todo/RHI-Compute-UAV-Indirect-Draw-Design-todo.md`；实现后须把 `fillCapabilities` 中对应能力位改为按 GL 版本动态判定 |
| Tessellation（Hull / Domain） | GL 4.0+ 原生支持；glslang 侧还需放开 `EShLangTessControl` / `EShLangTessEvaluation` 阶段 |
| `createBlendState` 独立 RT 混合 | 当前仅读 `RenderTargetStates[0]`，MRT 下不正确 |

### P3 — 清理

| 项目 | 说明 |
|------|------|
| 修正 "not supported in OpenGL 3.3" 注释 | 三处（Hull / Domain / Compute），实际目标是 GL 4.5 Core Profile |
| 消除 `GL4Context` 与 `GL4ContextBase` 的重复实现 | `initDummyContext`、`glslangCompileAndReflect`、`reflectShader*` 在两处各有一份，且 `GL4Context::init()` 不调用 `GL4ContextBase::init()`。D3D11 已做过同类合并（见 `D3D11-Renderer-Backend-todo.md` P3 项） |
| `ensureProgramLinked` 补 `GL_CHECK_ERROR` | `glLinkProgram` / `glUseProgram` 后无错误检查 |
| Console 变体调用 `fillCapabilities` 或显式注释说明 | 避免「能力位全 false」成为隐式行为 |

---

## A.10 跨文档一致性与回填清单

GL4 后端的需求分散在多份设计文档中，本节记录它们与本文档的对应关系，以及哪些说法已经与代码脱节。

### A.10.1 GPU Readback：接口尚未进入 RHI

`doc/todo/GPU-Readback-onRender-Design-todo.md` §5 / §7.1 规划了四个 readback 接口，要求在 `T3DGL4Context`、`T3DGL4ConsoleContext`、`T3DGLES3Context` 中先加 stub，并在 §7.3 要求实现后回填本文档。

**核对结果**：`T3DRHIContext.h` 中目前**搜不到任何 `beginRead*` / `endRead*` / readback 相关声明**，该设计尚未落到 RHI 接口层。因此：

- 本文档 A.7 的 93 个接口口径**不包含** readback，统计成立；
- 这四个接口进入 RHI 之后，总数变为 97，本附录的统计需同步更新；
- 届时 GL4 的 `Usage::kCopy` 需映射到 `GL_STREAM_READ`（该文档 §3.1 已指定）。

### A.10.2 Compute / UAV / Indirect：对应 RHI-Compute 的第五期 E2

`doc/todo/RHI-Compute-UAV-Indirect-Draw-Design-todo.md` 把 GL4 的补齐工作单列为**第五期任务 E2**（状态 ⏸，预估 3 天），要求「GL 4.3+ 全量真实现 + 低版本按能力位 stub」。本文档 A.9 的 P2 项即对应 E2。

该文档 §10.1 记录的当前实现程度与本次复核**完全一致**：GL4 的实例化绘制是真实现，structured buffer / compute / UAV / indirect 是 stub。其 §8.1 给出了 RHI 接口到 GL API 的完整映射表（`glDispatchCompute`、`glMemoryBarrier`、`glBindBufferBase(GL_SHADER_STORAGE_BUFFER)`、`glDraw*Indirect`），可直接作为 E2 的实现依据。

需注意 §8.2 的降级要求：**GL 4.0–4.2 有 indirect draw 但没有 compute**，所以 `supportsIndirectDraw` 与 `supportsCompute` 必须分别按版本判定，不能合成一个开关。

### A.10.3 Cubemap：Skybox 文档中的「死代码」说法已过时

`doc/todo/Skybox-Support-Design-todo.md` §2.2 称 GL4 的 `setPixelBuffers` 虽有 `kPixelBufferCubemap` 分支但是**死代码**（cast 错误），§4.5 把「修正 `setPixelBuffers` cast」列为待办。

**核对结果：这个问题已经修好了。** T3DGL4Context.cpp:3295-3298 的分支正确 cast 到 `GL4PixelBufferCubemap` 并使用 `GL_TEXTURE_CUBE_MAP` target。该文档 §4.3 对 GL4「`createPixelBufferCubemap` ✅ 完整实现」的判断与本次复核一致（见 A.4.3）。Skybox 文档的 §2.2 / §4.5 待办项可以关闭。

`GL4ConsoleContext` 的 cubemap 为空实现，与 A.8 的 null backend 定位一致，不属缺陷。

### A.10.4 Shader 变体链路：GL4 侧已验证

`doc/todo/Shader-MultiBackend-Variant-Design-todo.md` 记录 `OPENGL4` → `SHADER_LANGUAGE::kGLSL`（scc target `glsl`），且 ResourceApp 在 D3D11 / GL4 / VK 三个后端间切换均验证成功、GL4 正确取到 `kGLSL` 变体。这条链路无待办。

### A.10.5 需要修正的上游文档

以下不是 GL4 后端的代码问题，但会误导读者，建议一并修掉：

| 文档 | 问题 |
|------|------|
| `doc/Tiny3D-Architecture.md` §3.6 | RHI 后端表里只有 "OpenGL 3" 和 "OpenGL ES 2/3"，**完全没有 OpenGL 4 / GL4Renderer**，而 GL4 是 Windows 上的默认渲染器（`assets/config/Windows/Tiny3D.cfg`） |
| `doc/GL4-Renderer-Backend-Architecture.md` | 架构描述（三变体、Dummy Context、Core Profile 4.5、glslang、延迟 link）仍然准确，但未涵盖 compute / UAV / indirect / instancing / cubemap / scissor 等新增 RHI 能力 |
| `doc/refs/D3D11-vs-OpenGL4-API-Mapping.md` | 缺 SSBO / `glBindImageTexture` 专节，`DispatchIndirect` 未列入 §12 映射表，也没有 4.0 与 4.3 的版本分级。实现 E2 时建议参考 `doc/refs/D3D11-vs-OpenGLES3-API-Mapping.md` 的 §16（完整 compute 章节）与附录 B（版本能力矩阵），那份写得更系统 |
