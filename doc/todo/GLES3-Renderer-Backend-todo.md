# GLES3 渲染后端接口实现计划

> 本文档基于 `RHIContext` 纯虚接口定义，逐一规划 `GLES3Context`（Runtime 版本）的实现方案。
>
> - **接口定义**：`source/Core/Include/RHI/T3DRHIContext.h`
> - **参考实现（GL4 基类）**：`source/Plugins/Renderer/OpenGL4/Base/Source/T3DGL4ContextBase.cpp`
> - **参考实现（GL4 Window）**：`source/Plugins/Renderer/OpenGL4/Window/Source/T3DGL4Context.cpp`
> - **API 映射参考**：`doc/refs/D3D11-vs-OpenGLES3-API-Mapping.md`
> - **目标目录**：`source/Plugins/Renderer/OpenGLES3/`

---

## 实现状态图例

| 标记 | 含义 |
|------|------|
| 📋 待实现 | 需要完整实现 |
| 🔇 按设计为空 | GLES3 后端按设计意图不需要实现（如 Vulkan 专用接口），或 GLES 3.0 不支持的 Shader 阶段 |
| ⚡ 需适配 | 与 GL4 逻辑类似，但需要针对 GLES3 差异进行适配 |

---

## 1. 变换 (Transform)

### 1.1 setViewProjectionTransform

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `TResult setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat)` |
| **GL4 实现要点** | 设置 View/Proj 矩阵。GL4 使用 `glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE)` 将 NDC 深度从 [-1,1] 映射到 [0,1]，并通过 conversionMat 重映射 Z。渲染到 FBO 时翻转 Y 轴 |
| **GLES3 适配方案** | GLES 3.x **不支持** `glClipControl`（仅 GL 4.5+ / `GL_EXT_clip_control` 扩展）。NDC 深度范围固定为 [-1,1]。需要：1) 移除 conversionMat 的 Z 重映射逻辑（保持 [-1,1]）；2) 在投影矩阵中直接处理深度范围；3) 渲染到 FBO 时的 Y 翻转逻辑保持不变 |
| **GLES3 API** | `glGetIntegerv(GL_MAJOR_VERSION)` 查询版本，无需 `glClipControl` |
| **关键差异** | GL4 通过 `glClipControl` 使用 [0,1] 深度与 D3D11 一致；GLES3 使用原生 [-1,1] 深度范围 |

---

## 2. 渲染目标 (Render Target)

### 2.1 createRenderWindow

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `RHIRenderTargetPtr createRenderWindow(RenderWindow *renderWindow)` |
| **GL4 实现要点** | 创建 GL4RenderWindow：WGL 创建 Core Profile 4.5 上下文 + MSAA 像素格式 + 多线程 Context 转移 |
| **GLES3 实现方案** | 创建 GLES3RenderWindow：1) 从 `SysWMInfo` 获取 `ANativeWindow*`；2) `eglCreateWindowSurface(display, config, nativeWindow, nullptr)`；3) `eglMakeCurrent` 绑定 Context 到 Surface；4) 存储 `EGLSurface` + 宽高到 GLES3RenderWindow 成员 |
| **GLES3 API** | `eglCreateWindowSurface`, `eglMakeCurrent`, `eglQuerySurface(EGL_WIDTH/EGL_HEIGHT)` |
| **参数** | `renderWindow` — 引擎渲染窗口对象，包含 ANativeWindow 句柄、宽高、MSAA 等描述信息 |
| **返回值** | 成功返回 `GLES3RenderWindowPtr`，失败返回 `nullptr` |

### 2.2 createRenderTexture

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2D *buffer)` |
| **GL4 实现要点** | 根据像素格式创建颜色 RT 或深度/模板 RT，支持 MSAA（额外创建 GLResolveTex + GLResolveFBO） |
| **GLES3 实现方案** | 1) `glGenTextures` + `glTexStorage2D` 创建不可变纹理；2) `glGenFramebuffers` + `glFramebufferTexture2D` 附加到 FBO；3) MSAA：使用 `glRenderbufferStorageMultisample` + `glFramebufferRenderbuffer`（GLES 3.0+）；4) Resolve 通过 `glBlitFramebuffer` |
| **GLES3 API** | `glTexStorage2D`（替代 `glTexImage2D` + Mutable），`glRenderbufferStorageMultisample`, `glBlitFramebuffer` |
| **关键差异** | GLES3 推荐使用 `glTexStorage2D` 创建不可变纹理（性能更好）；MSAA 纹理需要 `GL_TEXTURE_2D_MULTISAMPLE`（GLES 3.1+），3.0 仅支持 Renderbuffer MSAA |

### 2.3 setRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `TResult setRenderTarget(RenderTarget *renderTarget)` |
| **GL4 实现要点** | Window 类型绑定默认 FBO(0)；Texture 类型绑定颜色 FBO + MRT + depth-only FBO |
| **GLES3 实现方案** | 逻辑与 GL4 基本一致：1) Window → `glBindFramebuffer(GL_FRAMEBUFFER, 0)`；2) Texture → 绑定 FBO + `glDrawBuffers` 配置 MRT（GLES 3.0 最少支持 4 个 Color Attachment）；3) 额外调用 `glInvalidateFramebuffer` 优化 TBR 架构 |
| **GLES3 API** | `glBindFramebuffer`, `glDrawBuffers`, `glFramebufferTexture2D` |
| **关键差异** | 需在渲染结束后调用 `glInvalidateFramebuffer` 丢弃不再需要的附件（移动端 TBR 性能关键） |

### 2.4 resetRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult resetRenderTarget()` |
| **GL4 实现要点** | `glBindFramebuffer(GL_FRAMEBUFFER, 0)` |
| **GLES3 实现方案** | 与 GL4 完全一致：`glBindFramebuffer(GL_FRAMEBUFFER, 0)` |
| **GLES3 API** | `glBindFramebuffer` |

---

## 3. 视口 (Viewport)

### 3.1 setViewport

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult setViewport(const Viewport &viewport)` |
| **GL4 实现要点** | 根据 RenderTarget 类型获取宽高，按 viewport 的归一化比例计算实际视口大小 |
| **GLES3 实现方案** | 与 GL4 逻辑一致：1) 从当前 RenderTarget 获取宽高；2) 计算实际像素区域；3) `glViewport(x, y, w, h)` + `glScissor(x, y, w, h)` |
| **GLES3 API** | `glViewport`, `glScissor` |

---

## 4. 清除操作 (Clear)

### 4.1 clearColor

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult clearColor(const ColorRGB &color)` |
| **GL4 实现要点** | `glClearColor` + `glClear(GL_COLOR_BUFFER_BIT)` |
| **GLES3 实现方案** | 与 GL4 完全一致：`glClearColor(r, g, b, 1.0f)` + `glClear(GL_COLOR_BUFFER_BIT)` |
| **GLES3 API** | `glClearColor`, `glClear` |
| **移动端注意** | 在 TBR 架构上，每帧开头必须 Clear 所有使用的附件（通知驱动重置 tile） |

### 4.2 clearDepth

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `TResult clearDepth(Real depth)` |
| **GL4 实现要点** | `glClearDepth(depth)` + `glClear(GL_DEPTH_BUFFER_BIT)` |
| **GLES3 实现方案** | GLES3 无 `glClearDepth`，使用 `glClearDepthf(depth)` 替代 |
| **GLES3 API** | `glClearDepthf`（注意 `f` 后缀，GLES 仅支持 float 版本） |
| **关键差异** | GL4 使用 `glClearDepth`（double），GLES3 使用 `glClearDepthf`（float） |

### 4.3 clearDepthStencil

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `TResult clearDepthStencil(Real depth, uint32_t stencil)` |
| **GL4 实现要点** | 同时清除深度和模板 |
| **GLES3 实现方案** | `glClearDepthf(depth)` + `glClearStencil(stencil)` + `glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)` |
| **GLES3 API** | `glClearDepthf`, `glClearStencil`, `glClear` |

---

## 5. 渲染状态 (Render State)

### 5.1 createBlendState

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `RHIBlendStatePtr createBlendState(BlendState *state)` |
| **GL4 实现要点** | 将引擎 BlendDesc 映射为 `GL4BlendStateData`（srcRGB/dstRGB/srcAlpha/dstAlpha/opRGB/opAlpha/colorMask） |
| **GLES3 实现方案** | 与 GL4 逻辑一致，创建 `GLES3BlendStateData` POD：1) 通过 `GLES3Mapping` 转换 BlendFactor → `GL_SRC_ALPHA` 等；2) 转换 BlendOp → `GL_FUNC_ADD` 等；3) 存储 colorMask |
| **GLES3 API** | 创建时无 GL 调用（仅存储映射后的枚举值） |
| **枚举映射** | `D3D11_BLEND_SRC_ALPHA` → `GL_SRC_ALPHA`；`D3D11_BLEND_OP_ADD` → `GL_FUNC_ADD`（见 API Mapping 第15节） |

### 5.2 createDepthStencilState

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `RHIDepthStencilStatePtr createDepthStencilState(DepthStencilState *state)` |
| **GL4 实现要点** | 映射 DepthStencilDesc 为 `GL4DepthStencilStateData`，支持前后面独立 Stencil |
| **GLES3 实现方案** | 与 GL4 一致：映射 CompareFunc → `GL_LESS/GL_LEQUAL` 等；映射 StencilOp → `GL_KEEP/GL_REPLACE` 等 |
| **GLES3 API** | 创建时无 GL 调用 |

### 5.3 createRasterizerState

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `RHIRasterizerStatePtr createRasterizerState(RasterizerState *state)` |
| **GL4 实现要点** | 映射 RasterizerDesc：FillMode/CullMode/FrontFace/Scissor/DepthClip/DepthBias/MSAA |
| **GLES3 实现方案** | 移除 FillMode 映射（GLES 不支持 `glPolygonMode`，仅 Fill）；移除 `GL_DEPTH_CLAMP`（GLES 不支持）；其余 CullMode/FrontFace/Scissor/DepthBias 映射与 GL4 一致 |
| **关键差异** | 1) 无 `glPolygonMode`（固定 Fill）；2) 无 `GL_DEPTH_CLAMP`；3) Wireframe 模式需 fallback 为 `GL_LINES` 绘制或忽略 |

### 5.4 createSamplerState

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `RHISamplerStatePtr createSamplerState(SamplerState *state)` |
| **GL4 实现要点** | `glGenSamplers` + 设置 Wrap/Filter/Anisotropy/LOD/Comparison/BorderColor |
| **GLES3 实现方案** | 1) `glGenSamplers` + `glSamplerParameteri`（GLES 3.0+）；2) BorderColor 需要 GLES 3.2 或 `GL_EXT_texture_border_clamp` 扩展，运行时检测；3) 各向异性需要 `GL_EXT_texture_filter_anisotropic` 扩展 |
| **GLES3 API** | `glGenSamplers`, `glSamplerParameteri`, `glSamplerParameterf` |
| **关键差异** | 1) `GL_CLAMP_TO_BORDER` 需 GLES 3.2 或扩展；2) `GL_TEXTURE_MAX_ANISOTROPY_EXT` 需扩展；3) `GL_MIRROR_CLAMP_TO_EDGE` 不支持 |

### 5.5 setBlendState

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult setBlendState(BlendState *state)` |
| **GL4 实现要点** | `glEnable/glDisable(GL_BLEND)` + `glBlendFuncSeparate` + `glBlendEquationSeparate` + `glColorMask` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glEnable(GL_BLEND)`, `glBlendFuncSeparate`, `glBlendEquationSeparate`, `glColorMask` |

### 5.6 setDepthStencilState

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult setDepthStencilState(DepthStencilState *state)` |
| **GL4 实现要点** | `glEnable/Disable(GL_DEPTH_TEST)` + `glDepthFunc` + `glDepthMask` + `glStencilFuncSeparate` + `glStencilOpSeparate` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glEnable(GL_DEPTH_TEST)`, `glDepthFunc`, `glDepthMask`, `glStencilFuncSeparate`, `glStencilOpSeparate` |

### 5.7 setRasterizerState

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `TResult setRasterizerState(RasterizerState *state)` |
| **GL4 实现要点** | `glPolygonMode` + `glCullFace` + `glFrontFace` + Scissor/DepthClamp/PolygonOffset + FBO Y 翻转时交换 Front/Back |
| **GLES3 实现方案** | 1) 移除 `glPolygonMode`（GLES 不支持）；2) 移除 `glEnable(GL_DEPTH_CLAMP)`；3) 保留 CullFace/FrontFace/Scissor/PolygonOffset；4) FBO Y 翻转逻辑保持不变 |
| **GLES3 API** | `glEnable(GL_CULL_FACE)`, `glCullFace`, `glFrontFace`, `glEnable(GL_SCISSOR_TEST)`, `glScissor`, `glPolygonOffset` |
| **关键差异** | 无 Wireframe 模式、无 DepthClamp |

---

## 6. 顶点输入 (Vertex Input)

### 6.1 createVertexDeclaration

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `RHIVertexDeclarationPtr createVertexDeclaration(VertexDeclaration *decl)` |
| **GL4 实现要点** | `glGenVertexArrays` 创建 VAO |
| **GLES3 实现方案** | 与 GL4 完全一致：`glGenVertexArrays(1, &vao)`。GLES 3.0 **强制使用 VAO**（与 GL4 Core Profile 一致） |
| **GLES3 API** | `glGenVertexArrays` |

### 6.2 setVertexDeclaration

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult setVertexDeclaration(VertexDeclaration *decl)` |
| **GL4 实现要点** | `glBindVertexArray(VAO)` + 缓存到 `mPendingVertexDecl` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glBindVertexArray` |

---

## 7. 缓冲区 (Buffer)

### 7.1 createVertexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `RHIVertexBufferPtr createVertexBuffer(VertexBuffer *buffer)` |
| **GL4 实现要点** | `glGenBuffers` + `glBufferData(GL_ARRAY_BUFFER)` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glGenBuffers`, `glBindBuffer(GL_ARRAY_BUFFER)`, `glBufferData` |

### 7.2 setVertexBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `TResult setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets)` |
| **GL4 实现要点** | 若有 `mPendingVertexDecl`，延迟配置顶点属性（`glEnableVertexAttribArray` + `glVertexAttribPointer` / `glVertexAttribIPointer`） |
| **GLES3 实现方案** | 与 GL4 逻辑一致，但需注意：1) GLES 3.0 不支持 `glVertexAttribLPointer`（double 类型）；2) 仅支持 `glVertexAttribPointer`（float）和 `glVertexAttribIPointer`（integer） |
| **GLES3 API** | `glBindBuffer(GL_ARRAY_BUFFER)`, `glEnableVertexAttribArray`, `glVertexAttribPointer`, `glVertexAttribIPointer` |
| **关键差异** | 无 double 类型顶点属性支持 |

### 7.3 createIndexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `RHIIndexBufferPtr createIndexBuffer(IndexBuffer *buffer)` |
| **GL4 实现要点** | `glGenBuffers` + `glBufferData(GL_ELEMENT_ARRAY_BUFFER)` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glGenBuffers`, `glBindBuffer(GL_ELEMENT_ARRAY_BUFFER)`, `glBufferData` |

### 7.4 setIndexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult setIndexBuffer(IndexBuffer *buffer)` |
| **GL4 实现要点** | `glBindBuffer(GL_ELEMENT_ARRAY_BUFFER)` + 映射索引类型 |
| **GLES3 实现方案** | 与 GL4 一致。支持 `GL_UNSIGNED_SHORT` 和 `GL_UNSIGNED_INT`（GLES 3.0+ 支持 32 位索引） |
| **GLES3 API** | `glBindBuffer(GL_ELEMENT_ARRAY_BUFFER)` |

### 7.5 createConstantBuffer

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `RHIConstantBufferPtr createConstantBuffer(ConstantBuffer *buffer)` |
| **GL4 实现要点** | `glGenBuffers` + `glBufferData(GL_UNIFORM_BUFFER)` |
| **GLES3 实现方案** | 与 GL4 完全一致（UBO 是 GLES 3.0 核心功能） |
| **GLES3 API** | `glGenBuffers`, `glBindBuffer(GL_UNIFORM_BUFFER)`, `glBufferData` |
| **限制** | GLES 3.0 最小保证 UBO 大小为 16384 字节（`GL_MAX_UNIFORM_BLOCK_SIZE`），需运行时查询 |

### 7.6 createPixelBuffer1D

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `RHIPixelBuffer1DPtr createPixelBuffer1D(PixelBuffer1D *buffer)` |
| **GL4 实现要点** | `glGenTextures` + `glTexImage1D` |
| **GLES3 实现方案** | GLES 3.x **不支持** 1D 纹理（无 `GL_TEXTURE_1D`、无 `glTexImage1D`）。实现为：使用高度为 1 的 2D 纹理模拟（`glTexStorage2D(GL_TEXTURE_2D, levels, format, width, 1)`） |
| **GLES3 API** | `glGenTextures`, `glBindTexture(GL_TEXTURE_2D)`, `glTexStorage2D(... width, 1)` |
| **关键差异** | GLES 无原生 1D 纹理，需用 height=1 的 2D 纹理模拟 |

### 7.7 createPixelBuffer2D

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `RHIPixelBuffer2DPtr createPixelBuffer2D(PixelBuffer2D *buffer)` |
| **GL4 实现要点** | `glGenTextures` + `glTexImage2D` + `glGenerateMipmap` |
| **GLES3 实现方案** | 推荐使用 `glTexStorage2D`（不可变分配，性能更好）+ `glTexSubImage2D` 上传数据 + `glGenerateMipmap` |
| **GLES3 API** | `glGenTextures`, `glTexStorage2D`, `glTexSubImage2D`, `glGenerateMipmap` |
| **压缩格式** | GLES3 必须支持 ETC2/EAC，不支持 BC1-BC7（DXT）。需在 `GLES3Mapping` 中映射引擎格式到 ETC2/ASTC |

### 7.8 createPixelBuffer3D

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `RHIPixelBuffer3DPtr createPixelBuffer3D(PixelBuffer3D *buffer)` |
| **GL4 实现要点** | `glGenTextures` + `glTexImage3D` |
| **GLES3 实现方案** | 与 GL4 逻辑一致，推荐使用 `glTexStorage3D` + `glTexSubImage3D`（GLES 3.0+ 支持 3D 纹理） |
| **GLES3 API** | `glGenTextures`, `glTexStorage3D`, `glTexSubImage3D` |

---

## 8. 顶点着色器 (Vertex Shader)

### 8.1 createVertexShader

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `RHIShaderPtr createVertexShader(ShaderVariant *shader)` |
| **GL4 实现要点** | `glCreateShader(GL_VERTEX_SHADER)` + `glShaderSource` + `glCompileShader`，含编译错误日志 |
| **GLES3 实现方案** | 与 GL4 逻辑一致。着色器源码应为 ESSL 300 es / 310 es / 320 es（由 scc.exe 交叉编译生成）。需确保传入的着色器有 `precision highp float;` 声明 |
| **GLES3 API** | `glCreateShader(GL_VERTEX_SHADER)`, `glShaderSource`, `glCompileShader`, `glGetShaderiv`, `glGetShaderInfoLog` |
| **关键差异** | GLSL 400 → ESSL 300 es；需要 precision 限定符 |

### 8.2 setVertexShader

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult setVertexShader(ShaderVariant *shader)` |
| **GL4 实现要点** | 删除旧 Program，创建新 Program 并 attach shader，标记 `mProgramDirty = true` |
| **GLES3 实现方案** | 与 GL4 一致。额外增加 nullptr 检查（修复 GL4 的已知缺陷） |
| **GLES3 API** | `glCreateProgram`, `glAttachShader`, `glDeleteProgram` |

### 8.3 setVSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **GL4 实现要点** | 委托 `stageConstantBuffers()`，将 cbuffer 名→GL buffer handle 存入 `mPendingUBOs` |
| **GLES3 实现方案** | 与 GL4 完全一致（UBO 绑定机制相同） |
| **GLES3 API** | 无直接 GL 调用（仅缓存到 `mPendingUBOs`，在 `render()` 时通过 `glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo)` 绑定） |

### 8.4 setVSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **GL4 实现要点** | 委托 `bindPixelBuffers()` → `glActiveTexture` + `glBindTexture` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glActiveTexture(GL_TEXTURE0 + n)`, `glBindTexture(GL_TEXTURE_2D, tex)` |

### 8.5 setVSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult setVSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **GL4 实现要点** | 委托 `bindSamplers()` → `glBindSampler` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glBindSampler(unit, sampler)` |

---

## 9. 像素着色器 (Pixel Shader / Fragment Shader)

### 9.1 createPixelShader

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `RHIShaderPtr createPixelShader(ShaderVariant *shader)` |
| **GL4 实现要点** | `glCreateShader(GL_FRAGMENT_SHADER)` + 编译 |
| **GLES3 实现方案** | 与 GL4 一致。需确保 ESSL 源码有 `precision mediump float;`（或 highp）声明 |
| **GLES3 API** | `glCreateShader(GL_FRAGMENT_SHADER)`, `glShaderSource`, `glCompileShader` |
| **关键差异** | Fragment Shader 在 GLES 中必须声明 default precision |

### 9.2 setPixelShader

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult setPixelShader(ShaderVariant *shader)` |
| **GL4 实现要点** | 支持 nullptr 解绑；attach shader 到 Program |
| **GLES3 实现方案** | 与 GL4 一致 |
| **GLES3 API** | `glAttachShader`, `glDetachShader` |

### 9.3 setPSConstantBuffers / setPSPixelBuffers / setPSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | 同 VS 对应接口 |
| **GLES3 实现方案** | 与 VS 阶段完全一致（GL 中 UBO/Texture/Sampler 绑定不区分 shader stage） |

---

## 10. Hull 着色器 (Tessellation Control)

### 10.1 createHullShader / setHullShader / setHSConstantBuffers / setHSPixelBuffers / setHSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空（GLES 3.0/3.1），⚡ 需适配（GLES 3.2） |
| **签名** | 各自标准签名 |
| **GLES3 实现方案** | GLES 3.2 支持 `GL_TESS_CONTROL_SHADER`。当前阶段实现为空返回 `T3D_OK`（与 GL4 后端一致），后续可在检测到 GLES 3.2 时启用 |
| **GLES3 API** | `glCreateShader(GL_TESS_CONTROL_SHADER)`（仅 GLES 3.2） |

---

## 11. Domain 着色器 (Tessellation Evaluation)

### 11.1 createDomainShader / setDomainShader / setDSConstantBuffers / setDSPixelBuffers / setDSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空（GLES 3.0/3.1），⚡ 需适配（GLES 3.2） |
| **签名** | 各自标准签名 |
| **GLES3 实现方案** | 同 Hull Shader，当前阶段实现为空返回 |
| **GLES3 API** | `glCreateShader(GL_TESS_EVALUATION_SHADER)`（仅 GLES 3.2） |

---

## 12. 几何着色器 (Geometry Shader)

### 12.1 createGeometryShader

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `RHIShaderPtr createGeometryShader(ShaderVariant *shader)` |
| **GL4 实现要点** | `glCreateShader(GL_GEOMETRY_SHADER)` + 编译 |
| **GLES3 实现方案** | 仅 GLES 3.2（或 `GL_EXT_geometry_shader`）支持。运行时检查 `mGLESMinor >= 2`，不支持时返回 nullptr 并记录警告日志 |
| **GLES3 API** | `glCreateShader(GL_GEOMETRY_SHADER)`（GLES 3.2） |

### 12.2 setGeometryShader / setGSConstantBuffers / setGSPixelBuffers / setGSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | 各自标准签名 |
| **GLES3 实现方案** | 与 GL4 逻辑一致（attach shader 到 Program）。需运行时检查 GS 支持 |

---

## 13. 计算着色器 (Compute Shader)

### 13.1 createComputeShader

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `RHIShaderPtr createComputeShader(ShaderVariant *shader)` |
| **GL4 实现要点** | GL4 后端当前未实现（按设计为空） |
| **GLES3 实现方案** | GLES 3.1+ 支持 `GL_COMPUTE_SHADER`。运行时检查 `mGLESMinor >= 1`，支持时通过 `glCreateShader(GL_COMPUTE_SHADER)` 创建并编译。GLES3 后端优先于 GL4 实现 CS |
| **GLES3 API** | `glCreateShader(GL_COMPUTE_SHADER)`, `glShaderSource`, `glCompileShader`（GLES 3.1+） |

### 13.2 setComputeShader

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `TResult setComputeShader(ShaderVariant *shader)` |
| **GLES3 实现方案** | 计算着色器使用独立 Program（不与图形 Program 混合）。`glUseProgram(computeProgram)` → `glDispatchCompute` |
| **GLES3 API** | `glUseProgram`, `glDispatchCompute`（GLES 3.1+） |

### 13.3 setCSConstantBuffers / setCSPixelBuffers / setCSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **GLES3 实现方案** | SSBO 绑定使用 `glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo)`；Image 绑定使用 `glBindImageTexture`（GLES 3.1+） |
| **GLES3 API** | `glBindBufferBase(GL_SHADER_STORAGE_BUFFER)`, `glBindImageTexture` |

---

## 14. Shader 编译与反射 (Shader Compilation & Reflection)

### 14.1 compileShader

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `TResult compileShader(ShaderVariant *shader)` |
| **GL4 实现要点** | 委托 `glslangCompileAndReflect()`，使用 glslang 库解析 GLSL 400，提取 Uniform Block 和 Sampler 信息 |
| **GLES3 实现方案** | 复用 glslang 反射流程，但需将着色器 profile 设为 `EEsProfile`，版本设为 300/310/320（对应 ESSL）。glslang 已支持 ESSL 着色器解析 |
| **关键差异** | GL4 使用 `ENoProfile` + 400；GLES3 使用 `EEsProfile` + 300/310/320 |

### 14.2 reflectShaderAllBindings

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)` |
| **GL4 实现要点** | 从 glslang 缓存提取 Uniform Block 成员和 Sampler 绑定信息 |
| **GLES3 实现方案** | 与 GL4 逻辑一致。反射数据结构相同（GlslangBlockInfo/GlslangUniformInfo）。ESSL 的 `layout(std140)` uniform block 内存布局与桌面 GL 一致 |

### 14.3 reflectSamplerBindings

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams)` |
| **GL4 实现要点** | 更新已有 samplerParams 的 binding 索引 |
| **GLES3 实现方案** | 与 GL4 完全一致 |

---

## 15. 图元与绘制 (Primitive & Draw)

### 15.1 setPrimitiveType

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `TResult setPrimitiveType(PrimitiveType primitive)` |
| **GL4 实现要点** | 映射引擎枚举为 GL 枚举存储到 `mPrimitiveType` |
| **GLES3 实现方案** | 映射规则：`TRIANGLES`→`GL_TRIANGLES`, `LINES`→`GL_LINES`, `POINTS`→`GL_POINTS` 等。移除 GLES 不支持的枚举：`GL_QUADS`, `GL_POLYGON`。邻接图元（`GL_TRIANGLES_ADJACENCY`）仅 GLES 3.2 支持，`GL_PATCHES` 仅 GLES 3.2 支持 |
| **GLES3 API** | 无 GL 调用（仅存储枚举值） |
| **关键差异** | 无 `GL_QUADS`/`GL_POLYGON`；邻接/Patch 需 3.2 |

### 15.2 render (indexed)

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `TResult render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)` |
| **GL4 实现要点** | 延迟 Program Link + `glUseProgram` + `bindPendingUniformBlocks` + `setupSamplerBindings` → `glDrawElementsBaseVertex` |
| **GLES3 实现方案** | 1) 延迟 Link 逻辑与 GL4 一致；2) GLES 3.0 不支持 `glDrawElementsBaseVertex`（需 GLES 3.2 或 `GL_EXT_draw_elements_base_vertex`）。Fallback：当 `baseVertex == 0` 时使用 `glDrawElements`；非零时检测扩展或调整顶点数据 |
| **GLES3 API** | `glLinkProgram`, `glUseProgram`, `glBindBufferBase(GL_UNIFORM_BUFFER)`, `glDrawElements`（3.0）/ `glDrawElementsBaseVertex`（3.2） |
| **关键差异** | `glDrawElementsBaseVertex` 需 GLES 3.2 或扩展支持 |

### 15.3 render (non-indexed)

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult render(uint32_t vertexCount, uint32_t startVertex)` |
| **GL4 实现要点** | 延迟 Program Link → `glDrawArrays` |
| **GLES3 实现方案** | 与 GL4 完全一致：`glDrawArrays(mPrimitiveType, startVertex, vertexCount)` |
| **GLES3 API** | `glDrawArrays` |

---

## 16. 状态重置 (Reset)

### 16.1 reset

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult reset()` |
| **GL4 实现要点** | 解绑 Program/VAO/VBO/EBO/FBO，删除 Program，清空 `mPendingUBOs` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glUseProgram(0)`, `glBindVertexArray(0)`, `glBindBuffer(... 0)`, `glBindFramebuffer(... 0)`, `glDeleteProgram` |

---

## 17. 数据传输 (Blit & Copy)

### 17.1 blit (RenderTarget → RenderTarget)

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **GL4 实现要点** | ❌ GL4 未实现（返回 T3D_OK） |
| **GLES3 实现方案** | 使用 `glBlitFramebuffer`（GLES 3.0+）：1) 绑定 src FBO 到 `GL_READ_FRAMEBUFFER`；2) 绑定 dst FBO 到 `GL_DRAW_FRAMEBUFFER`；3) 调用 `glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter)` |
| **GLES3 API** | `glBindFramebuffer(GL_READ_FRAMEBUFFER)`, `glBindFramebuffer(GL_DRAW_FRAMEBUFFER)`, `glBlitFramebuffer` |
| **注意** | GLES3 `glBlitFramebuffer` 仅支持 `GL_NEAREST` 过滤（当涉及深度/模板时） |

### 17.2 blit (Texture → RenderTarget)

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `TResult blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **GL4 实现要点** | 支持 MSAA Resolve + Y 翻转 |
| **GLES3 实现方案** | 逻辑与 GL4 一致，通过临时 FBO + `glBlitFramebuffer` 实现。MSAA Resolve 路径使用 Renderbuffer |
| **GLES3 API** | `glBlitFramebuffer` |

### 17.3 blit (RenderTarget → Texture)

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult blit(RenderTarget *src, Texture *dst, ...)` |
| **GL4 实现要点** | ❌ GL4 未实现 |
| **GLES3 实现方案** | 通过 `glBlitFramebuffer` 将 src FBO blit 到挂载了 dst texture 的临时 FBO |
| **GLES3 API** | `glBlitFramebuffer` |

### 17.4 blit (Texture → Texture)

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult blit(Texture *src, Texture *dst, ...)` |
| **GL4 实现要点** | ❌ GL4 未实现 |
| **GLES3 实现方案** | 创建两个临时 FBO 分别挂载 src/dst 纹理，通过 `glBlitFramebuffer` 传输。或使用 `GL_EXT_copy_image` 扩展的 `glCopyImageSubDataEXT`（如可用） |
| **GLES3 API** | `glBlitFramebuffer`，或 `glCopyImageSubDataEXT`（扩展） |

### 17.5 copyBuffer

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `TResult copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)` |
| **GL4 实现要点** | ❌ GL4 未实现 |
| **GLES3 实现方案** | 使用 `glCopyBufferSubData`（GLES 3.0+ 核心功能）：`glBindBuffer(GL_COPY_READ_BUFFER, src)` + `glBindBuffer(GL_COPY_WRITE_BUFFER, dst)` + `glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOffset, dstOffset, size)` |
| **GLES3 API** | `glBindBuffer(GL_COPY_READ_BUFFER)`, `glBindBuffer(GL_COPY_WRITE_BUFFER)`, `glCopyBufferSubData` |

### 17.6 writeBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **签名** | `TResult writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer)` |
| **GL4 实现要点** | 使用 DSA（`glNamedBufferData`/`glNamedBufferSubData`）更新 VBO/IBO/UBO；纹理用 `glTexSubImage2D` |
| **GLES3 实现方案** | GLES 不支持 DSA（`glNamedBuffer*`）。替代方案：1) Buffer 类型：`glBindBuffer(target, handle)` + `glBufferSubData` 或 `glMapBufferRange` + `glUnmapBuffer`；2) 纹理类型：`glBindTexture` + `glTexSubImage2D`（修复 GL4 的 width/height=0 bug）；3) 注意绑定/解绑不能污染 VAO 的 EBO，需使用 `GL_COPY_WRITE_BUFFER` 作为临时绑定点 |
| **GLES3 API** | `glBindBuffer`, `glBufferSubData` / `glMapBufferRange`, `glTexSubImage2D` |
| **关键差异** | 无 DSA 支持，需显式 bind/unbind |

---

## 18. 帧命令 (Frame Commands)

### 18.1 beginRender

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult beginRender()` |
| **GLES3 实现方案** | 与 GL4 一致，GLES 即时模式不需要此操作。内联返回 `T3D_OK` |

### 18.2 endRender

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult endRender()` |
| **GLES3 实现方案** | 同 beginRender |

### 18.3 beginPass

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult beginPass()` |
| **GLES3 实现方案** | 同 beginRender |

### 18.4 endPass

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult endPass()` |
| **GLES3 实现方案** | 同 beginRender。但可考虑在此处调用 `glInvalidateFramebuffer` 优化 TBR |

---

## 19. 原生上下文接口 (Native Context)

### 19.1 getNativeContext

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `void* getNativeContext() const` |
| **GL4 实现要点** | 返回 `HGLRC`（Windows）/ `GLXContext`（Linux） |
| **GLES3 实现方案** | 返回 `mEGLContext`（EGLContext 句柄） |

### 19.2 restoreNativeContext

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **签名** | `void restoreNativeContext()` |
| **GL4 实现要点** | `wglMakeCurrent` / `glXMakeCurrent` |
| **GLES3 实现方案** | `eglMakeCurrent(mEGLDisplay, mMainSurface, mMainSurface, mEGLContext)` |
| **GLES3 API** | `eglMakeCurrent` |

---

## 20. GLES3Context 专有接口（非 RHIContext 纯虚接口）

### 20.1 initEGLContext

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现（新增） |
| **签名** | `TResult initEGLContext()` |
| **功能** | EGL 上下文初始化 + 版本降级策略 |
| **实现方案** | 1) `eglGetDisplay(EGL_DEFAULT_DISPLAY)` + `eglInitialize`；2) `eglChooseConfig`（EGL_RENDERABLE_TYPE=EGL_OPENGL_ES3_BIT, RGBA8, D24S8）；3) 循环尝试 {3,2} → {3,1} → {3,0} 调用 `eglCreateContext`；4) 成功后 `glGetIntegerv(GL_MAJOR_VERSION/GL_MINOR_VERSION)` 确认实际版本；5) 查询扩展字符串 |
| **GLES3 API** | `eglGetDisplay`, `eglInitialize`, `eglChooseConfig`, `eglCreateContext`, `eglMakeCurrent` |

### 20.2 destroyEGLContext

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现（新增） |
| **签名** | `void destroyEGLContext()` |
| **功能** | 销毁 EGL 上下文和 Display |
| **实现方案** | `eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)` → `eglDestroyContext` → `eglTerminate` |
| **GLES3 API** | `eglMakeCurrent`, `eglDestroyContext`, `eglTerminate` |

### 20.3 swapBackBuffer

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现（新增） |
| **签名** | `TResult swapBackBuffer(GLES3RenderWindow *renderWindow)` |
| **功能** | 交换前后缓冲区 |
| **实现方案** | `eglSwapBuffers(mEGLDisplay, renderWindow->getEGLSurface())` |
| **GLES3 API** | `eglSwapBuffers` |

### 20.4 resizeRenderWindow

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现（新增） |
| **签名** | `TResult resizeRenderWindow(GLES3RenderWindow *rw, uint32_t w, uint32_t h)` |
| **功能** | 窗口大小变更处理 |
| **实现方案** | 更新 RenderWindow 的 mWidth/mHeight + `glViewport(0, 0, w, h)`。EGL surface 自动跟随 ANativeWindow 大小变化，无需重建 |
| **GLES3 API** | `glViewport` |

---

## 21. 插件系统 (Plugin)

### 21.1 GLES3Plugin

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **功能** | 继承 `Plugin`，在 `install()` 中创建 GLES3Renderer 并通过 `T3D_AGENT.addRHIRenderer()` 注册；`uninstall()` 中移除并销毁 |
| **参考** | GL4Plugin 实现 |

### 21.2 GLES3Renderer

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **功能** | 继承 `RHIRenderer + Singleton`。`init()` 创建 GLES3Context；`destroy()` 销毁 Context。`mName = RHIRenderer::OPENGLES3` |
| **参考** | GL4Renderer 实现 |

### 21.3 PluginDLL 入口

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现 |
| **功能** | `extern "C"` 导出 `dllStartPlugin()` / `dllStopPlugin()`，创建/销毁 GLES3Plugin 实例 |
| **产物** | `libGLES3Renderer.so`（Android 共享库） |

---

## 22. CMake 构建系统

### 22.1 OpenGLES3/CMakeLists.txt

| 项目 | 内容 |
|------|------|
| **状态** | 📋 待实现（重写） |
| **当前内容** | 链接 OpenGL 库（错误，应链接 GLESv3+EGL） |
| **目标方案** | 1) `set_project_name(GLES3Renderer)`；2) include_directories 包含 Base/Include + Runtime/Include；3) 使用 `set_project_files` 收集 Base 和 Runtime 子目录；4) `target_link_libraries` 链接 `GLESv3 EGL T3DCore T3DMath T3DLog T3DPlatform ${GLSLANG_LIBRARIES}`；5) 设置为 SHARED 库 |

### 22.2 Renderer/CMakeLists.txt 修改

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **当前内容** | Android 段设置 `TINY3D_BUILD_RENDERSYSTEM_GLES3 TRUE` 并 find_package |
| **目标方案** | 移除 `find_package(OpenGLES3)` 依赖（Android NDK 直接提供头文件和库），`add_subdirectory(OpenGLES3)` |

---

## 汇总统计

| 状态 | 数量 | 说明 |
|------|------|------|
| 📋 待实现 | 35 | 需要从零编写的接口 |
| ⚡ 需适配 | 20 | 与 GL4 逻辑类似但需针对 GLES3 差异修改 |
| 🔇 按设计为空 | 9 | beginRender/endRender/beginPass/endPass + HS(5) + DS(5)（部分） |

### 与 GL4 后端的关键差异

| 项目 | GL4 | GLES3 | 说明 |
|------|-----|-------|------|
| 上下文管理 | WGL/GLX | EGL | 完全不同的 API |
| GL 函数加载 | glad 库 | 不需要（NDK 直接导出） | 简化 |
| 深度范围 | [0,1]（`glClipControl`） | [-1,1]（固定） | 投影矩阵处理差异 |
| 1D 纹理 | `GL_TEXTURE_1D` | 不支持（用 height=1 的 2D 模拟） | 需适配 |
| PolygonMode | `glPolygonMode(GL_LINE)` | 不支持 | 无 Wireframe |
| DepthClamp | `GL_DEPTH_CLAMP` | 不支持 | 需移除 |
| DSA | `glNamedBuffer*` | 不支持 | 需显式 bind |
| DrawElementsBaseVertex | 核心功能 | GLES 3.2 / 扩展 | 需 fallback |
| ClearDepth | `glClearDepth`（double） | `glClearDepthf`（float） | API 差异 |
| 计算着色器 | 未实现 | GLES 3.1+（新增实现） | GLES3 后端更完整 |
| 压缩纹理 | BC1-BC7 | ETC2/EAC + ASTC | 格式映射差异 |
| TBR 优化 | 不需要 | `glInvalidateFramebuffer` | 移动端性能关键 |
| Sampler Border | 核心功能 | GLES 3.2 / 扩展 | 需运行时检测 |
| glslang Profile | `ENoProfile` + 400 | `EEsProfile` + 300/310/320 | 编译参数差异 |

### 实现优先级

| 优先级 | 模块 | 接口数 | 原因 |
|--------|------|--------|------|
| **P0 - 核心框架** | Plugin/Renderer/CMake + EGL 初始化 | 5 | 基础骨架，其他一切依赖于此 |
| **P0 - 核心渲染** | RenderWindow + Context(Transform/Viewport/Clear) | 7 | 最小可运行渲染循环 |
| **P1 - 资源创建** | VBO/IBO/UBO/VAO/Texture2D | 8 | 能绘制几何体的最低要求 |
| **P1 - 着色器** | VS/PS Create/Set + Compile/Reflect | 10 | 着色器管线 |
| **P1 - 状态管理** | Blend/Depth/Rasterizer/Sampler Create/Set | 7 | 正确渲染所需 |
| **P1 - 绘制** | setPrimitiveType + render (indexed/non-indexed) + reset | 4 | 实际绘制 |
| **P2 - 数据传输** | blit(4 种) + copyBuffer + writeBuffer | 6 | 完善功能 |
| **P2 - 高级着色器** | GS/CS + 对应 set/bind | 10 | GLES 3.1/3.2 特性 |
| **P3 - 曲面细分** | HS/DS + 对应 set/bind | 10 | GLES 3.2 专属，优先级最低 |

---

## 23. CMake 工程集成方案（Android 平台）

> 本章节详细列出 GLES3Renderer 插件接入 Tiny3D 构建系统所需的全部 CMake 修改。
>
> **当前状态**：现有 `OpenGLES3/CMakeLists.txt` 是占位文件，错误链接了桌面端 OpenGL 库；源码文件均为空壳。
>
> **目标**：重写为可在 Android NDK 下正确编译、链接、加载的 GLES3 渲染插件。

---

### 23.1 目录结构重组

参照 GL4 的 `Base/Window/Console` 拆分模式，GLES3 仅面向 Android 移动端，不需要 Console/Editor 变体，采用 **Base + Runtime** 两级结构：

```
source/Plugins/Renderer/OpenGLES3/
├── CMakeLists.txt              ← 顶层入口（仅 add_subdirectory）
├── Base/
│   ├── Include/
│   │   ├── T3DGLES3ContextBase.h
│   │   ├── T3DGLES3Mapping.h
│   │   ├── T3DGLES3Error.h
│   │   └── T3DGLES3PrerequisitesBase.h
│   └── Source/
│       ├── T3DGLES3ContextBase.cpp
│       └── T3DGLES3Mapping.cpp
└── Runtime/
    ├── CMakeLists.txt          ← 实际构建 target
    ├── Include/
    │   ├── T3DGLES3Prerequisites.h
    │   ├── T3DGLES3Plugin.h
    │   ├── T3DGLES3Renderer.h
    │   ├── T3DGLES3Context.h
    │   ├── T3DGLES3RenderWindow.h
    │   ├── T3DGLES3RenderBuffer.h
    │   ├── T3DGLES3RenderState.h
    │   └── T3DGLES3Shader.h
    └── Source/
        ├── T3DGLES3Plugin.cpp
        ├── T3DGLES3PluginDLL.cpp
        ├── T3DGLES3Renderer.cpp
        ├── T3DGLES3Context.cpp
        ├── T3DGLES3RenderWindow.cpp
        ├── T3DGLES3RenderBuffer.cpp
        ├── T3DGLES3RenderState.cpp
        └── T3DGLES3Shader.cpp
```

---

### 23.2 `source/Plugins/Renderer/OpenGLES3/CMakeLists.txt`（顶层入口）

| 项目 | 内容 |
|------|------|
| **状态** | 📋 重写 |
| **当前内容** | 错误使用 `find_package(OpenGL)` 并链接桌面 `${OPENGL_LIBRARIES}` |
| **目标内容** | 仅作为子目录入口 |

```cmake
#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#-------------------------------------------------------------------------------

add_subdirectory(Runtime)
```

---

### 23.3 `source/Plugins/Renderer/OpenGLES3/Runtime/CMakeLists.txt`（核心构建文件）

| 项目 | 内容 |
|------|------|
| **状态** | 📋 新建 |
| **产物** | `libGLES3Renderer.so`（Android SHARED 库） |
| **链接库** | `GLESv3` `EGL` `T3DCore` `T3DMath` `T3DLog` `T3DPlatform` `T3DSystem` `T3DUtils` `rttr_core` `${GLSLANG_LIBRARIES}` |

```cmake
#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#-------------------------------------------------------------------------------

set_project_name(GLES3Renderer)

# Android 平台 .so 导出宏
add_definitions(-DGLES3RENDERER_EXPORT)

set(TINY3D_GLES3_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(TINY3D_GLES3_BASE_DIR ${TINY3D_GLES3_ROOT_DIR}/Base)
set(TINY3D_GLES3_RUNTIME_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Setup project include files path
include_directories(
    "${TINY3D_CORE_SOURCE_DIR}/Include"
    "${TINY3D_MATH_SOURCE_DIR}/Include"
    "${TINY3D_FRAMEWORK_SOURCE_DIR}/Include"
    "${TINY3D_PLATFORM_SOURCE_DIR}/Include"
    "${TINY3D_SYSTEM_SOURCE_DIR}/Include"
    "${TINY3D_LOG_SOURCE_DIR}/Include"
    "${TINY3D_UTILS_SOURCE_DIR}/Include"
    "${TINY3D_GLES3_BASE_DIR}/Include"
    "${TINY3D_GLES3_RUNTIME_DIR}/Include"
    "${RTTR_INCLUDE_DIR}"
    "${T3D_COMMON_INCLUDE_DIR}"
    "${GLSLANG_INCLUDE_DIR}"
    )

# Setup project header files - Base
set_project_files(Include\\\\Base ${TINY3D_GLES3_BASE_DIR}/Include/ .h)

# Setup project source files - Base
set_project_files(Source\\\\Base ${TINY3D_GLES3_BASE_DIR}/Source/ .cpp)

# Setup project header files - Runtime
set_project_files(Include ${TINY3D_GLES3_RUNTIME_DIR}/Include/ .h)

# Setup project source files - Runtime
set_project_files(Source ${TINY3D_GLES3_RUNTIME_DIR}/Source/ .cpp)

# Android 上始终构建为 SHARED 库（动态加载插件）
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})

# Android NDK 直接提供 GLESv3 和 EGL，无需 find_package
target_link_libraries(
    ${LIB_NAME}
    GLESv3
    EGL
    android
    log
    T3DMath
    T3DLog
    T3DUtils
    T3DPlatform
    T3DSystem
    T3DCore
    rttr_core
    ${GLSLANG_LIBRARIES}
    )

# Setup project folder
set_property(TARGET ${LIB_NAME} PROPERTY FOLDER "Plugins/Renderer")

install(TARGETS ${LIB_NAME}
    RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
    LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
    )
```

**关键设计决策说明**：

| 决策 | 原因 |
|------|------|
| 不使用 `find_package(OpenGLES3)` | Android NDK 交叉编译时，toolchain 自动提供 sysroot，`GLESv3`/`EGL` 可直接链接 |
| 不需要 GLAD | GLES3 函数由 NDK 导出头文件（`<GLES3/gl3.h>`、`<GLES3/gl31.h>`、`<GLES3/gl32.h>`），无需动态加载 |
| 链接 `android` 库 | 提供 `ANativeWindow` 等 Android 专用 API |
| 链接 `log` 库 | 提供 `__android_log_print`（可选，调试用） |
| 链接 `rttr_core` | 插件反射注册需要 RTTR |
| 链接 `${GLSLANG_LIBRARIES}` | 运行时 shader 反射（glslang 需提供 Android 预编译库） |

---

### 23.4 `source/Plugins/Renderer/CMakeLists.txt` 修改

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **修改范围** | 第 141-154 行（GLES3 段） |

**当前代码**（第 141-154 行）：
```cmake
if (TINY3D_BUILD_RENDERSYSTEM_GLES3)
    add_definitions(-DT3D_ERR_EGL3_RENDERER=T3D_ERR_RENDERER)

    find_package(OpenGLES3)

    if (NOT OPENGLES3_FOUND)
        message(STATUS "Could not find dependency: OpenGLES 3")
        message(STATUS "Skipping OpenGLES3 rendersystem build")
        set(TINY3D_BUILD_RENDERSYSTEM_GLES3 FALSE)
    else ()
        add_subdirectory(OpenGLES3)
        add_dependencies(GLES3Renderer T3DCore T3DMath T3DLog T3DPlatform)
    endif ()
endif (TINY3D_BUILD_RENDERSYSTEM_GLES3)
```

**修改为**：
```cmake
if (TINY3D_BUILD_RENDERSYSTEM_GLES3)
    add_definitions(-DT3D_ERR_EGL3_RENDERER=T3D_ERR_RENDERER)

    if (TINY3D_OS_ANDROID)
        # Android NDK 直接提供 GLES3 + EGL，无需 find_package
        add_subdirectory(OpenGLES3)
        add_dependencies(GLES3Renderer T3DCore T3DMath T3DLog T3DPlatform T3DSystem T3DUtils)
    else ()
        # 非 Android 平台通过 FindOpenGLES3.cmake 查找模拟器 SDK
        find_package(OpenGLES3)

        if (NOT OPENGLES3_FOUND)
            message(STATUS "Could not find dependency: OpenGLES 3")
            message(STATUS "Skipping OpenGLES3 rendersystem build")
            set(TINY3D_BUILD_RENDERSYSTEM_GLES3 FALSE)
        else ()
            add_subdirectory(OpenGLES3)
            add_dependencies(GLES3Renderer T3DCore T3DMath T3DLog T3DPlatform T3DSystem T3DUtils)
        endif ()
    endif ()
endif (TINY3D_BUILD_RENDERSYSTEM_GLES3)
```

**修改要点**：
1. Android 平台跳过 `find_package(OpenGLES3)` — NDK sysroot 已包含头文件和库
2. 增加 `T3DSystem` `T3DUtils` 依赖 — 与 GL4Renderer 保持一致
3. 非 Android 平台保留原有 find_package 流程（用于桌面端 GLES 模拟器开发）

---

### 23.5 `source/CMake/Packages/FindOpenGLES3.cmake` 修改

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **修改范围** | 增加 Android 平台短路逻辑 |

在文件顶部（`IF (WIN32)` 之前）增加 Android 短路判断：

```cmake
# Android NDK: GLES3 + EGL are provided by the sysroot; skip searching.
IF (ANDROID)
    SET(OPENGLES3_FOUND TRUE)
    SET(OPENGLES3_INCLUDE_DIR "")   # NDK sysroot includes GLES3/gl3.h
    SET(OPENGLES3_LIBRARIES GLESv3)
    SET(EGL_FOUND TRUE)
    SET(EGL_INCLUDE_DIR "")
    SET(EGL_LIBRARIES EGL)
    RETURN()
ENDIF (ANDROID)
```

这确保即使在 Android 上调用了 `find_package(OpenGLES3)` 也能正确短路返回。

---

### 23.6 `dependencies/glslang` Android 预编译库

| 项目 | 内容 |
|------|------|
| **状态** | 📋 需准备 |
| **当前状态** | 仅有 `prebuilt/Windows/x64/` 目录 |
| **需要新增** | `prebuilt/Android/arm64-v8a/` 和 `prebuilt/Android/x86_64/` |

**目标目录结构**：
```
dependencies/glslang/
├── include/                     ← 已有（平台无关头文件）
├── prebuilt/
│   ├── Windows/x64/             ← 已有
│   └── Android/
│       ├── arm64-v8a/
│       │   ├── libglslang.a
│       │   ├── libMachineIndependent.a
│       │   ├── libGenericCodeGen.a
│       │   ├── libOSDependent.a
│       │   ├── libSPIRV.a
│       │   ├── libSPIRV-Tools.a
│       │   ├── libSPIRV-Tools-opt.a
│       │   └── libglslang-default-resource-limits.a
│       └── x86_64/
│           └── ... (同上)
```

**`source/CMake/Packages/FindGlslang.cmake` 修改**：

在平台判断部分增加 Android 分支：

```cmake
if (WIN32)
    set(_GLSLANG_LIB_DIR "${GLSLANG_DIR}/prebuilt/Windows/x64")
elseif (ANDROID)
    set(_GLSLANG_LIB_DIR "${GLSLANG_DIR}/prebuilt/Android/${ANDROID_ABI}")
elseif (UNIX AND NOT APPLE)
    set(_GLSLANG_LIB_DIR "${GLSLANG_DIR}/prebuilt/Linux/x64")
elseif (APPLE)
    set(_GLSLANG_LIB_DIR "${GLSLANG_DIR}/prebuilt/OSX")
endif ()
```

Android 静态库命名规则调整（`.a` 后缀，无 debug/release 区分）：

```cmake
if (ANDROID)
    # Android 使用统一构建，无 debug/release 分离
    set(GLSLANG_LIBRARIES "")
    foreach(_lib ${_GLSLANG_LIB_NAMES})
        list(APPEND GLSLANG_LIBRARIES "${_GLSLANG_LIB_DIR}/lib${_lib}.a")
    endforeach()
else ()
    # 桌面平台保留 debug/optimized 分离
    set(GLSLANG_LIBRARIES "")
    foreach(_lib ${_GLSLANG_LIB_NAMES})
        list(APPEND GLSLANG_LIBRARIES debug "${_GLSLANG_LIB_DIR}/Debug/${_lib}d.lib")
        list(APPEND GLSLANG_LIBRARIES optimized "${_GLSLANG_LIB_DIR}/Release/${_lib}.lib")
    endforeach()
endif ()
```

> **备注**：如果暂不在 Android 端做运行时反射（defer 到离线工具），可先不链接 glslang，从 `target_link_libraries` 移除 `${GLSLANG_LIBRARIES}`，后续再补上。

---

### 23.7 Samples Android Gradle 集成

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **影响文件** | 各 Sample 的 `Android/app/build.gradle` 或 CMake POST_BUILD 命令 |

现有 Sample CMake 中 Android 段通过 `POST_BUILD` 将 .so 拷贝到 `Android/app/libs/${ANDROID_ABI}/`：

```cmake
set(T3D_ANDROID_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Android/app/libs/${ANDROID_ABI}")
add_custom_command(TARGET ${BIN_NAME}
    PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E make_directory ${T3D_ANDROID_LIB_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_BINARY} ${T3D_ANDROID_LIB_PATH}
    )
```

`libGLES3Renderer.so` 作为插件是被引擎在运行时通过 `dlopen` 加载的，需确保它被打包到 APK 中。有两种方式：

**方式 A（推荐）**：CMake `install` + Gradle `jniLibs` 目录

Gradle 的 `build.gradle` 中配置：
```groovy
android {
    sourceSets {
        main {
            jniLibs.srcDirs = ['libs']  // 已有配置
        }
    }
}
```

在 Sample 的 CMakeLists.txt `POST_BUILD` 中追加拷贝 GLES3Renderer.so：

```cmake
if (TINY3D_OS_ANDROID)
    add_custom_command(TARGET ${BIN_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:GLES3Renderer>
            ${T3D_ANDROID_LIB_PATH}
        )
endif ()
```

**方式 B**：统一到 `source/Samples/HelloApp/Android/` 的 Gradle 配置

通过 Gradle 的 `externalNativeBuild` 或 `ndk` 配置自动拉取所有 CMake 产出的 .so。

---

### 23.8 `assets/config/Android/Tiny3D.cfg` 修改

| 项目 | 内容 |
|------|------|
| **状态** | ⚡ 需适配 |
| **修改内容** | 将 `NullRenderer` 替换为 `GLES3Renderer` |

**当前配置**（插件列表和渲染器名称）：
```json
"plugins": [
    { "RTTI_Type": "std::string", "RTTI_Value": "FileSystemArchive" },
    { "RTTI_Type": "std::string", "RTTI_Value": "NullRenderer" },
    { "RTTI_Type": "std::string", "RTTI_Value": "FreeImageCodec" }
]
```
```json
"renderer": { "RTTI_Type": "std::string", "RTTI_Value": "NullRenderer" }
```

**修改为**：
```json
"plugins": [
    { "RTTI_Type": "std::string", "RTTI_Value": "FileSystemArchive" },
    { "RTTI_Type": "std::string", "RTTI_Value": "GLES3Renderer" },
    { "RTTI_Type": "std::string", "RTTI_Value": "FreeImageCodec" }
]
```
```json
"renderer": { "RTTI_Type": "std::string", "RTTI_Value": "GLES3Renderer" }
```

> **注意**：插件名称必须与 `GLES3Plugin::mName` 和动态库文件名（去掉 `lib` 前缀和 `.so` 后缀）一致。

---

### 23.9 DLL 导出宏定义

| 项目 | 内容 |
|------|------|
| **状态** | 📋 需在 `T3DGLES3PrerequisitesBase.h` 中定义 |

```cpp
#if defined(GLES3RENDERER_EXPORT)
    #define T3D_GLES3RENDERER_API   T3D_EXPORT_API
#else
    #define T3D_GLES3RENDERER_API   T3D_IMPORT_API
#endif
```

PluginDLL 入口使用此宏：

```cpp
extern "C"
{
    T3D_GLES3RENDERER_API TResult dllStartPlugin();
    T3D_GLES3RENDERER_API TResult dllStopPlugin();
}
```

> 在 Android 上 `T3D_EXPORT_API` 通常展开为 `__attribute__((visibility("default")))`。

---

### 23.10 CMake 修改清单汇总

| # | 文件 | 操作 | 说明 |
|---|------|------|------|
| 1 | `source/Plugins/Renderer/OpenGLES3/CMakeLists.txt` | **重写** | 改为仅 `add_subdirectory(Runtime)` |
| 2 | `source/Plugins/Renderer/OpenGLES3/Runtime/CMakeLists.txt` | **新建** | 核心构建 target，链接 GLESv3+EGL+引擎库 |
| 3 | `source/Plugins/Renderer/CMakeLists.txt` | **修改** | GLES3 段增加 Android 短路逻辑，补充依赖 |
| 4 | `source/CMake/Packages/FindOpenGLES3.cmake` | **修改** | 增加 Android 短路返回 |
| 5 | `source/CMake/Packages/FindGlslang.cmake` | **修改** | 增加 Android ABI 路径和 `.a` 链接方式 |
| 6 | `dependencies/glslang/prebuilt/Android/` | **新增** | 预编译 glslang 静态库（arm64-v8a, x86_64） |
| 7 | `assets/config/Android/Tiny3D.cfg` | **修改** | 插件名 NullRenderer → GLES3Renderer |
| 8 | Sample CMakeLists.txt（Android 段） | **修改** | POST_BUILD 拷贝 libGLES3Renderer.so |

---

### 23.11 验证构建的最小命令序列

```bash
# 在项目根目录（source/ 所在的父目录）
# 假设 ANDROID_NDK 和 CMAKE 已正确配置

# 1. CMake Configure（x86_64 模拟器）
cmake -S source -B build-android-x86_64 \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=x86_64 \
    -DANDROID_PLATFORM=android-24 \
    -DCMAKE_BUILD_TYPE=Debug

# 2. Build GLES3Renderer target
cmake --build build-android-x86_64 --target GLES3Renderer -j$(nproc)

# 3. 验证产物
ls build-android-x86_64/lib/Android/x86_64/libGLES3Renderer.so

# 4. Build 完整 Sample（如 HelloApp）
cmake --build build-android-x86_64 --target HelloApp -j$(nproc)
```

---

### 23.12 与 GL4 构建的对比

| 维度 | GL4Renderer | GLES3Renderer |
|------|-------------|---------------|
| 平台 | Windows / Linux / macOS | Android |
| GL 函数加载 | GLAD（源码编译进 target） | 无需（NDK 直接提供） |
| 上下文库 | `opengl32`（Win）/ `GL X11`（Linux） | `GLESv3` `EGL` `android` |
| 目录拆分 | Base + Window(Runtime/Editor) + Console | Base + Runtime |
| find_package | `find_package(OpenGL)` | 不需要（Android NDK sysroot） |
| 产物名 | `libGL4Renderer.so` / `GL4Renderer.dll` | `libGLES3Renderer.so` |
| Shader 反射 | glslang（Windows .lib） | glslang（Android .a）或离线 |
| Editor 变体 | GL4RendererEditor | 无（移动端无编辑器） |
| Console 变体 | GL4RendererConsole | 无（移动端无控制台渲染） |

---

## 24. 跨平台后端方案（Windows / Linux / macOS 桌面端支持）

> 本章节规划 GLES3Renderer 从 Android-only 扩展为跨平台统一后端的长期方案。
>
> **动机**：桌面端 GPU 驱动（NVIDIA / AMD / Intel）和 Google ANGLE 均原生支持 OpenGL ES 3.x。
> 将 GLES3Renderer 统一为跨平台后端可以：
> 1. 无需 Android 设备即可在桌面端快速开发调试渲染代码
> 2. 移动端和桌面端共享同一渲染路径，减少代码分支
> 3. 在 CI 服务器上进行渲染回归测试（Mesa/ANGLE 软渲染）

---

### 24.1 桌面端 OpenGL ES 3 可用性分析

| 平台 | 方式 | 提供方 | 说明 |
|------|------|--------|------|
| **Windows** | GPU 驱动原生 EGL | NVIDIA / AMD / Intel | 驱动安装后即提供 `libEGL.dll` + `libGLESv2.dll`，可直接创建 ES 3.x Context |
| **Windows** | ANGLE | Google (Chromium) | 将 GLES 3.x 翻译为 D3D11 / Vulkan，行为最接近移动端 |
| **Linux** | Mesa 驱动 | 开源 Mesa3D | `libEGL.so` + `libGLESv2.so`，Intel/AMD/llvmpipe 均支持 GLES 3.1/3.2 |
| **Linux** | ANGLE | Google | 翻译为 Vulkan / OpenGL |
| **macOS** | ANGLE | Google | 翻译为 Metal（macOS 无原生 GLES 支持，ANGLE 是唯一方案） |

### 24.2 架构分层设计

将 EGL 上下文管理中**平台相关**的部分抽象为一个薄层：

```
GLES3Context（平台无关 — EGL API 调用）
    │
    ├── GLES3ContextBase        ← EGL 初始化/销毁/MakeCurrent（完全跨平台）
    │
    └── NativeWindowProvider    ← 获取 EGLNativeWindowType 的平台适配
            │
            ├── Android:  ANativeWindow*（从 SDL SysWMInfo 获取）
            ├── Windows:  HWND（从 SDL SysWMInfo 获取）
            ├── Linux:    Window (X11) 或 wl_surface* (Wayland)
            └── macOS:    CALayer*（通过 ANGLE 的 EGL 扩展）
```

**关键认知**：EGL 本身是跨平台 API。`eglCreateContext`、`eglCreateWindowSurface`、`eglSwapBuffers` 等调用在所有平台上完全一致，唯一差异是：
1. **EGLNativeDisplayType** — Android: `EGL_DEFAULT_DISPLAY`；Linux: X11 Display* / Wayland display
2. **EGLNativeWindowType** — 各平台原生窗口句柄

---

### 24.3 目录结构扩展

从第 23 章的 Android-only 结构扩展为跨平台：

```
source/Plugins/Renderer/OpenGLES3/
├── CMakeLists.txt                  ← 顶层入口
├── Base/
│   ├── Include/
│   │   ├── T3DGLES3ContextBase.h   ← EGL 核心逻辑（平台无关）
│   │   ├── T3DGLES3Mapping.h
│   │   ├── T3DGLES3Error.h
│   │   └── T3DGLES3PrerequisitesBase.h
│   └── Source/
│       ├── T3DGLES3ContextBase.cpp
│       └── T3DGLES3Mapping.cpp
├── Runtime/                        ← Android 运行时（原有）
│   ├── CMakeLists.txt
│   ├── Include/
│   └── Source/
├── Desktop/                        ← 桌面端运行时（新增）
│   ├── CMakeLists.txt
│   ├── Include/
│   │   ├── T3DGLES3DesktopPrerequisites.h
│   │   ├── T3DGLES3DesktopPlugin.h
│   │   ├── T3DGLES3DesktopContext.h  ← 桌面端 EGL 上下文（HWND/X11）
│   │   └── T3DGLES3DesktopRenderWindow.h
│   └── Source/
│       ├── T3DGLES3DesktopPlugin.cpp
│       ├── T3DGLES3DesktopPluginDLL.cpp
│       ├── T3DGLES3DesktopContext.cpp
│       └── T3DGLES3DesktopRenderWindow.cpp
└── Editor/                         ← 编辑器变体（可选，未来）
    ├── CMakeLists.txt
    └── ...
```

**设计原则**：
- `Base/` 包含所有平台共享的 EGL 逻辑和 GLES3 映射代码
- `Runtime/` 仅处理 Android 特有逻辑（ANativeWindow、TBR 优化策略等）
- `Desktop/` 处理 Windows/Linux/macOS 特有逻辑（HWND/X11/ANGLE 集成）
- 目标名：Android → `GLES3Renderer`，桌面 → `GLES3DesktopRenderer`（或统一为一个目标，条件编译）

---

### 24.4 桌面端 EGL 库来源方案

| 方案 | 优点 | 缺点 | 适用场景 |
|------|------|------|----------|
| **A. GPU 驱动原生 EGL** | 零依赖，性能最优 | 各驱动行为差异大；macOS 不可用 | Windows/Linux 开发快速调试 |
| **B. ANGLE 静态/动态库** | 行为统一，接近移动端；支持全平台含 macOS | 需预编译 ANGLE（体积大）；翻译层有微小开销 | 正式跨平台发布、CI 测试 |
| **C. 混合模式（推荐）** | 运行时检测：优先驱动原生，fallback 到 ANGLE | 稍复杂 | 最终产品 |

**推荐路径**：

- **Phase 1**：使用 GPU 驱动原生 EGL（Windows/Linux），快速验证渲染正确性
- **Phase 2**：集成 ANGLE，覆盖 macOS 并统一行为
- **Phase 3**：混合模式，配置文件选择后端

---

### 24.5 桌面端 CMake 构建方案

#### 24.5.1 `source/Plugins/Renderer/OpenGLES3/CMakeLists.txt`（顶层入口改写）

```cmake
#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#-------------------------------------------------------------------------------

if (TINY3D_OS_ANDROID)
    add_subdirectory(Runtime)
elseif (TINY3D_OS_WINDOWS OR TINY3D_OS_LINUX OR TINY3D_OS_MACOSX)
    add_subdirectory(Desktop)
endif ()
```

#### 24.5.2 `source/Plugins/Renderer/OpenGLES3/Desktop/CMakeLists.txt`

```cmake
#-------------------------------------------------------------------------------
# This file is part of the CMake build system for Tiny3D
#-------------------------------------------------------------------------------

set_project_name(GLES3Renderer)

add_definitions(-DGLES3RENDERER_EXPORT)

set(TINY3D_GLES3_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(TINY3D_GLES3_BASE_DIR ${TINY3D_GLES3_ROOT_DIR}/Base)
set(TINY3D_GLES3_DESKTOP_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    "${TINY3D_CORE_SOURCE_DIR}/Include"
    "${TINY3D_MATH_SOURCE_DIR}/Include"
    "${TINY3D_FRAMEWORK_SOURCE_DIR}/Include"
    "${TINY3D_PLATFORM_SOURCE_DIR}/Include"
    "${TINY3D_SYSTEM_SOURCE_DIR}/Include"
    "${TINY3D_LOG_SOURCE_DIR}/Include"
    "${TINY3D_UTILS_SOURCE_DIR}/Include"
    "${TINY3D_GLES3_BASE_DIR}/Include"
    "${TINY3D_GLES3_DESKTOP_DIR}/Include"
    "${RTTR_INCLUDE_DIR}"
    "${T3D_COMMON_INCLUDE_DIR}"
    "${GLSLANG_INCLUDE_DIR}"
    "${EGL_INCLUDE_DIR}"
    "${OPENGLES3_INCLUDE_DIR}"
    )

# Base 源码
set_project_files(Include\\\\Base ${TINY3D_GLES3_BASE_DIR}/Include/ .h)
set_project_files(Source\\\\Base ${TINY3D_GLES3_BASE_DIR}/Source/ .cpp)

# Desktop 源码
set_project_files(Include ${TINY3D_GLES3_DESKTOP_DIR}/Include/ .h)
set_project_files(Source ${TINY3D_GLES3_DESKTOP_DIR}/Source/ .cpp)

if (TINY3D_BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
else ()
    add_library(${LIB_NAME} STATIC ${SOURCE_FILES})
endif ()

# 平台相关链接库
if (TINY3D_GLES3_USE_ANGLE)
    # ANGLE 模式：链接 ANGLE 提供的 libEGL/libGLESv2
    set(GLES3_PLATFORM_LIBS
        ${ANGLE_LIB_DIR}/libEGL${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${ANGLE_LIB_DIR}/libGLESv2${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
else ()
    # 驱动原生模式
    if (WIN32)
        set(GLES3_PLATFORM_LIBS libEGL libGLESv2)
    elseif (UNIX AND NOT APPLE)
        set(GLES3_PLATFORM_LIBS EGL GLESv2)
    endif ()
endif ()

target_link_libraries(
    ${LIB_NAME}
    ${GLES3_PLATFORM_LIBS}
    T3DMath
    T3DLog
    T3DUtils
    T3DPlatform
    T3DSystem
    T3DCore
    rttr_core
    ${GLSLANG_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )

set_property(TARGET ${LIB_NAME} PROPERTY FOLDER "Plugins/Renderer")

install(TARGETS ${LIB_NAME}
    RUNTIME DESTINATION bin/Debug CONFIGURATIONS Debug
    LIBRARY DESTINATION bin/Debug CONFIGURATIONS Debug
    )
```

> **注意**：桌面端链接的是 `libGLESv2`（不是 `GLESv3`）。
> 这是因为 EGL/GLES 的桌面实现（驱动和 ANGLE）都将 GLES 3.x 函数导出在 `libGLESv2` 中（历史命名），
> 通过 `eglCreateContext` 请求的 `EGL_CONTEXT_MAJOR_VERSION=3` 来获得 GLES 3.x 功能。

#### 24.5.3 `source/Plugins/Renderer/CMakeLists.txt` 修改

```cmake
if (TINY3D_BUILD_RENDERSYSTEM_GLES3)
    add_definitions(-DT3D_ERR_EGL3_RENDERER=T3D_ERR_RENDERER)

    if (TINY3D_OS_ANDROID)
        # Android NDK 直接提供 GLES3 + EGL
        add_subdirectory(OpenGLES3)
        add_dependencies(GLES3Renderer T3DCore T3DMath T3DLog T3DPlatform T3DSystem T3DUtils)
    elseif (TINY3D_OS_WINDOWS OR TINY3D_OS_LINUX)
        # 桌面端：通过 FindOpenGLES3 查找驱动 EGL/GLES 或 ANGLE
        find_package(OpenGLES3)

        if (NOT OPENGLES3_FOUND)
            message(STATUS "Could not find OpenGLES 3 (driver or ANGLE)")
            message(STATUS "Skipping GLES3 rendersystem build on desktop")
            set(TINY3D_BUILD_RENDERSYSTEM_GLES3 FALSE)
        else ()
            add_subdirectory(OpenGLES3)
            add_dependencies(GLES3Renderer T3DCore T3DMath T3DLog T3DPlatform T3DSystem T3DUtils)
        endif ()
    elseif (TINY3D_OS_MACOSX)
        # macOS 仅通过 ANGLE 支持
        if (TINY3D_GLES3_USE_ANGLE)
            add_subdirectory(OpenGLES3)
            add_dependencies(GLES3Renderer T3DCore T3DMath T3DLog T3DPlatform T3DSystem T3DUtils)
        else ()
            message(STATUS "macOS requires ANGLE for GLES3. Set TINY3D_GLES3_USE_ANGLE=ON")
            set(TINY3D_BUILD_RENDERSYSTEM_GLES3 FALSE)
        endif ()
    endif ()
endif (TINY3D_BUILD_RENDERSYSTEM_GLES3)
```

#### 24.5.4 平台开关扩展

在 `source/Plugins/Renderer/CMakeLists.txt` 的平台段增加桌面端 GLES3 可选开关：

```cmake
if (TINY3D_OS_WINDOWS)
    # ... 现有 D3D11/GL4 ...
    # 可选：桌面端 GLES3（开发调试用）
    option(TINY3D_BUILD_RENDERSYSTEM_GLES3 "Build GLES3 renderer on desktop (requires EGL)" FALSE)
elseif (TINY3D_OS_ANDROID)
    set (TINY3D_BUILD_RENDERSYSTEM_GLES3 TRUE)  # 保持强制开启
elseif (TINY3D_OS_LINUX)
    option(TINY3D_BUILD_RENDERSYSTEM_GLES3 "Build GLES3 renderer on Linux (Mesa EGL)" FALSE)
endif ()
```

---

### 24.6 `FindOpenGLES3.cmake` 跨平台改写

当前 Find 模块存在多个问题，需全面改写以支持所有平台：

```cmake
#-------------------------------------------------------------------
# FindOpenGLES3.cmake - Find EGL + OpenGL ES 3.x libraries
#-------------------------------------------------------------------
#
# Output:
#   OPENGLES3_FOUND
#   OPENGLES3_INCLUDE_DIR
#   OPENGLES3_LIBRARIES
#   EGL_FOUND
#   EGL_INCLUDE_DIR
#   EGL_LIBRARIES

# ---- Android: NDK sysroot 直接提供 ----
if (ANDROID)
    set(OPENGLES3_FOUND TRUE)
    set(OPENGLES3_INCLUDE_DIR "")
    set(OPENGLES3_LIBRARIES GLESv3)
    set(EGL_FOUND TRUE)
    set(EGL_INCLUDE_DIR "")
    set(EGL_LIBRARIES EGL)
    return()
endif ()

# ---- ANGLE 模式 ----
if (TINY3D_GLES3_USE_ANGLE AND DEFINED ANGLE_DIR)
    find_path(EGL_INCLUDE_DIR EGL/egl.h HINTS "${ANGLE_DIR}/include")
    find_path(OPENGLES3_INCLUDE_DIR GLES3/gl3.h HINTS "${ANGLE_DIR}/include")
    find_library(EGL_egl_LIBRARY NAMES libEGL EGL HINTS "${ANGLE_DIR}/lib")
    find_library(OPENGLES3_gl_LIBRARY NAMES libGLESv2 GLESv2 HINTS "${ANGLE_DIR}/lib")

    if (EGL_egl_LIBRARY AND OPENGLES3_gl_LIBRARY)
        set(OPENGLES3_FOUND TRUE)
        set(OPENGLES3_LIBRARIES ${OPENGLES3_gl_LIBRARY})
        set(EGL_FOUND TRUE)
        set(EGL_LIBRARIES ${EGL_egl_LIBRARY})
    endif ()
    return()
endif ()

# ---- 桌面驱动原生模式 ----
if (WIN32)
    # NVIDIA/AMD/Intel 驱动提供 libEGL.dll + libGLESv2.dll
    # 搜索路径：驱动安装目录 / 系统目录
    find_path(OPENGLES3_INCLUDE_DIR GLES3/gl3.h
        HINTS
            "$ENV{AMDAPPSDKROOT}/include"
            "$ENV{MALI_SDK_ROOT}/include"
            "$ENV{PROGRAMFILES}/ANGLE/include"
    )
    find_path(EGL_INCLUDE_DIR EGL/egl.h
        HINTS
            "$ENV{AMDAPPSDKROOT}/include"
            "$ENV{MALI_SDK_ROOT}/include"
    )
    find_library(OPENGLES3_gl_LIBRARY NAMES libGLESv2
        HINTS "$ENV{AMDAPPSDKROOT}/lib/x86_64" "$ENV{MALI_SDK_ROOT}/lib"
    )
    find_library(EGL_egl_LIBRARY NAMES libEGL
        HINTS "$ENV{AMDAPPSDKROOT}/lib/x86_64" "$ENV{MALI_SDK_ROOT}/lib"
    )

elseif (UNIX AND NOT APPLE)
    # Linux: Mesa 提供 libEGL.so + libGLESv2.so
    find_path(OPENGLES3_INCLUDE_DIR GLES3/gl3.h PATHS /usr/include)
    find_path(EGL_INCLUDE_DIR EGL/egl.h PATHS /usr/include)
    find_library(OPENGLES3_gl_LIBRARY NAMES GLESv2 PATHS /usr/lib/x86_64-linux-gnu /usr/lib)
    find_library(EGL_egl_LIBRARY NAMES EGL PATHS /usr/lib/x86_64-linux-gnu /usr/lib)
endif ()

# ---- 结果判定 ----
if (OPENGLES3_gl_LIBRARY AND EGL_egl_LIBRARY)
    set(OPENGLES3_FOUND TRUE)
    set(OPENGLES3_LIBRARIES ${OPENGLES3_gl_LIBRARY})
    set(EGL_FOUND TRUE)
    set(EGL_LIBRARIES ${EGL_egl_LIBRARY})
else ()
    set(OPENGLES3_FOUND FALSE)
endif ()

mark_as_advanced(
    OPENGLES3_INCLUDE_DIR OPENGLES3_gl_LIBRARY
    EGL_INCLUDE_DIR EGL_egl_LIBRARY
)
```

---

### 24.7 代码层面的平台抽象

#### 24.7.1 `EGLNativeWindow` 获取的平台差异

```cpp
// T3DGLES3ContextBase.cpp — 跨平台 EGL 初始化（共享代码）
TResult GLES3ContextBase::initEGL()
{
    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(mDisplay, nullptr, nullptr);

    EGLint configAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24, EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    eglChooseConfig(mDisplay, configAttribs, &mConfig, 1, &mNumConfigs);

    // 版本降级策略（各平台通用）
    int versions[][2] = { {3, 2}, {3, 1}, {3, 0} };
    for (auto& ver : versions) {
        EGLint ctxAttribs[] = {
            EGL_CONTEXT_MAJOR_VERSION, ver[0],
            EGL_CONTEXT_MINOR_VERSION, ver[1],
            EGL_NONE
        };
        mContext = eglCreateContext(mDisplay, mConfig, EGL_NO_CONTEXT, ctxAttribs);
        if (mContext != EGL_NO_CONTEXT) break;
    }
    return (mContext != EGL_NO_CONTEXT) ? T3D_OK : T3D_ERR_FAIL;
}
```

```cpp
// T3DGLES3Context.cpp (Android Runtime)
EGLNativeWindowType GLES3Context::getNativeWindow(RenderWindow *rw)
{
    SDL_SysWMinfo info;
    SDL_GetWindowWMInfo(sdlWindow, &info);
    return info.info.android.window;  // ANativeWindow*
}
```

```cpp
// T3DGLES3DesktopContext.cpp (Desktop)
EGLNativeWindowType GLES3DesktopContext::getNativeWindow(RenderWindow *rw)
{
    SDL_SysWMinfo info;
    SDL_GetWindowWMInfo(sdlWindow, &info);

#if defined(T3D_OS_WINDOWS)
    return info.info.win.window;       // HWND
#elif defined(T3D_OS_LINUX)
    return info.info.x11.window;       // X11 Window
#endif
}
```

#### 24.7.2 桌面端 vs Android 的行为差异处理

| 差异点 | Android | 桌面端 | 处理方式 |
|--------|---------|--------|----------|
| TBR 优化（`glInvalidateFramebuffer`） | 必须调用 | 不需要（IMR 架构），调用无害 | 统一调用，桌面端 driver 忽略 |
| Surface 生命周期 | Activity Pause/Resume 需销毁/重建 | 窗口 minimize 不销毁 | `#ifdef T3D_OS_ANDROID` 条件处理 |
| `eglSwapInterval` | 通常 1（VSync） | 可选 0/1 | 读取配置文件 |
| 分辨率/DPI | 运行时从 ANativeWindow 查询 | 从 SDL Window 查询 | 统一通过 SDL 抽象 |
| `GL_FRAGMENT_PRECISION_HIGH` | 部分 GPU 不支持 | 桌面 GPU 均支持 | 运行时查询，动态选择 precision |

---

### 24.8 ANGLE 集成方案

#### 24.8.1 ANGLE 预编译库准备

```
dependencies/angle/
├── include/
│   ├── EGL/
│   │   ├── egl.h
│   │   ├── eglext.h
│   │   └── eglplatform.h
│   ├── GLES3/
│   │   ├── gl3.h
│   │   ├── gl31.h
│   │   ├── gl32.h
│   │   └── gl3ext.h
│   └── KHR/
│       └── khrplatform.h
└── prebuilt/
    ├── Windows/x64/
    │   ├── libEGL.dll
    │   ├── libEGL.dll.lib        ← 导入库
    │   ├── libGLESv2.dll
    │   └── libGLESv2.dll.lib
    └── Linux/x64/
        ├── libEGL.so
        └── libGLESv2.so
```

#### 24.8.2 ANGLE 特有的 EGL 扩展

ANGLE 提供额外的 EGL 扩展用于选择后端：

```cpp
// 选择 ANGLE 的 D3D11 后端（Windows 推荐）
EGLint displayAttribs[] = {
    EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
    EGL_NONE
};
EGLDisplay display = eglGetPlatformDisplayEXT(
    EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, displayAttribs);

// 选择 ANGLE 的 Vulkan 后端（跨平台推荐）
EGLint displayAttribs[] = {
    EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_VULKAN_ANGLE,
    EGL_NONE
};
```

可在 `Tiny3D.cfg` 中通过配置项选择 ANGLE 后端类型。

---

### 24.9 配置文件扩展

`assets/config/Windows/Tiny3D.cfg` 和 `assets/config/Linux/Tiny3D.cfg` 增加 GLES3 支持：

```json
"plugins": [
    { "RTTI_Type": "std::string", "RTTI_Value": "FileSystemArchive" },
    { "RTTI_Type": "std::string", "RTTI_Value": "GLES3Renderer" },
    { "RTTI_Type": "std::string", "RTTI_Value": "FreeImageCodec" }
]
```
```json
"renderer": { "RTTI_Type": "std::string", "RTTI_Value": "GLES3Renderer" }
```

用户可在配置文件中选择使用 GL4Renderer（桌面原生 OpenGL 4.5）或 GLES3Renderer（桌面 GLES 3.x via EGL/ANGLE），实现渲染器热切换。

---

### 24.10 实现路线图

| 阶段 | 目标 | 前置条件 | 产出 |
|------|------|----------|------|
| **Phase 0**（当前，第 23 章） | Android GLES3 可运行 | NDK 构建通过 | `libGLES3Renderer.so`（Android） |
| **Phase 1** | Windows 桌面端可运行 | GPU 驱动原生 EGL（NVIDIA/AMD） | `GLES3Renderer.dll`（Windows） |
| **Phase 2** | Linux 桌面端可运行 | Mesa EGL | `libGLES3Renderer.so`（Linux） |
| **Phase 3** | ANGLE 集成 | 预编译 ANGLE 库 | 统一后端（含 macOS 支持） |
| **Phase 4** | 替代 GL4Renderer | 功能对等验证 | 可选移除 GL4 后端 |

#### Phase 1 详细步骤

1. 创建 `Desktop/` 子目录和 CMakeLists.txt
2. 实现 `GLES3DesktopContext`：从 SDL HWND 创建 EGL Surface
3. 复用 `Base/` 的所有 EGL 逻辑和 GLES3 映射代码
4. 在 Windows 上用 NVIDIA 驱动的 `libEGL.dll` 验证
5. 修改 `FindOpenGLES3.cmake` 支持 Windows 驱动 EGL 搜索
6. 验证全部 Sample 在桌面 GLES3 模式下正确渲染

#### Phase 4 评估标准

| 维度 | 要求 |
|------|------|
| 功能完整性 | 全部 Sample 通过（与 GL4 渲染结果像素级对比） |
| 性能 | ANGLE D3D11 后端性能不低于原生 GL4 的 80% |
| 稳定性 | 无内存泄漏、无 GL Error 残留 |
| 平台覆盖 | Windows + Linux + Android + macOS(ANGLE) |

---

### 24.11 跨平台 GLES3 与现有后端的关系

```
┌─────────────────────────────────────────────────────────────┐
│                     RHIContext (纯虚接口)                      │
└───────────────┬──────────────┬──────────────┬───────────────┘
                │              │              │
    ┌───────────┴───┐  ┌──────┴──────┐  ┌───┴────────────┐
    │  D3D11Context │  │  GL4Context │  │  GLES3Context  │
    │  (Windows)    │  │  (Win/Lin)  │  │  (全平台)       │
    └───────────────┘  └─────────────┘  └───┬────────────┘
                                            │
                              ┌──────────────┼──────────────┐
                              │              │              │
                    ┌─────────┴──┐  ┌───────┴──────┐  ┌───┴─────────┐
                    │  Android   │  │  Desktop     │  │  macOS      │
                    │  (NDK EGL) │  │  (Driver/    │  │  (ANGLE     │
                    │            │  │   ANGLE EGL) │  │   Metal)    │
                    └────────────┘  └──────────────┘  └─────────────┘
```

**长期愿景**：GLES3Renderer 成为 Tiny3D 的**主渲染后端**，覆盖移动端（Android）和桌面端（Windows/Linux/macOS），而 D3D11/GL4/Vulkan 作为特定平台的高性能备选。这与 Unity 的 "OpenGL ES → 全平台" 和 Godot 的 "GLES3 后端" 策略类似。

---
---

# 附录 A：完整度复核（2026-08）

> **本附录的作用**：上文第 1～24 章是在 GLES3 插件**尚未动工**时写的**实现计划**，全篇状态标记为「📋 待实现 / ⚡ 需适配」，第 23 章开头还写着「现有源码文件均为空壳」。
>
> **这个前提已经不成立。** GLES3 插件现在是一套真实、成体系的 Android 渲染后端：`T3DGLES3Context.cpp` 约 2983 行，EGL 初始化、版本降级、FBO/MSAA、UBO、实例化绘制、glslang 反射、命令队列全部落地。上文的「汇总统计」（35 📋 / 20 ⚡ / 9 🔇）与「实现优先级」表已完全失效。
>
> 本附录以当前代码为准，把这份文档从**计划**翻转为**状态**，并逐项交代计划中哪些落地了、哪些没落地、哪些实现方式与计划不同。上文保留不动，作为设计意图的历史记录。
>
> **复核基准**
>
> - 接口定义：`source/Core/Include/RHI/T3DRHIContext.h`（`T3DRHIContext.inl` 为空）
> - 能力集定义：`source/Core/Include/RHI/T3DRHICapabilities.h`
> - GLES3 Runtime 实现：`source/Plugins/Renderer/OpenGLES3/Runtime/Source/T3DGLES3Context.cpp`（约 2983 行）
> - GLES3 基类实现：`source/Plugins/Renderer/OpenGLES3/Base/Source/T3DGLES3ContextBase.cpp`（402 行）
> - 枚举映射：`source/Plugins/Renderer/OpenGLES3/Base/Source/T3DGLES3Mapping.cpp`（410 行）
> - 窗口实现：`source/Plugins/Renderer/OpenGLES3/Runtime/Source/T3DGLES3RenderWindow.cpp`（217 行）
> - 对标：`doc/todo/GL4-Renderer-Backend-todo.md` 附录 A、`doc/todo/D3D11-Renderer-Backend-todo.md`
>
> 本附录中未标注文件名的行号均指 `T3DGLES3Context.cpp`。

---

## A.1 首要结论

**GLES3 后端已经是可用状态，不是空壳。** 在 Android 上，一条完整的前向渲染管线（VS+PS、UBO 常量、2D/3D/Cubemap 纹理、FBO 离屏渲染、MSAA、实例化绘制、blit 到窗口）已有实质实现。

各源文件规模与实现情况：

| 文件 | 行数 | 实现情况 |
|------|------|---------|
| `Runtime/Source/T3DGLES3Context.cpp` | 2983 | RHI 主实现，实体代码 |
| `Base/Source/T3DGLES3Mapping.cpp` | 410 | 格式 / 状态 / 图元枚举映射 |
| `Base/Source/T3DGLES3ContextBase.cpp` | 402 | EGL 生命周期 + glslang 编译反射 |
| `Runtime/Source/T3DGLES3RenderWindow.cpp` | 217 | Android EGL Surface |
| `Runtime/Source/T3DGLES3RenderBuffer.cpp` | 168 | RHI 资源包装与析构 |
| `Runtime/Source/T3DGLES3Shader.cpp` | 126 | Shader 对象生命周期 |
| `Runtime/Source/T3DGLES3Plugin.cpp` | 94 | 插件注册 |
| `Runtime/Source/T3DGLES3Renderer.cpp` | 78 | Renderer 初始化 |
| `Runtime/Source/T3DGLES3RenderState.cpp` | 73 | 状态对象工厂 |
| `Runtime/Source/T3DGLES3PluginDLL.cpp` | 32 | DLL 导出入口 |

类层次与第 23 章的规划一致：

```
RHIContext
 └── GLES3ContextBase   // EGL 生命周期 + glslang（Base/）
      └── GLES3Context   // 绝大部分 RHI override（Runtime/）
```

**一个值得记录的结果：GLES3 后端在若干接口上已经超过了 GL4 后端**，详见 A.6。

---

## A.2 接口口径的变化

`RHIContext` 当前共 **93 个虚接口**：88 个纯虚 + 5 个带默认实现（`getDepthRemapMatrix`、`resizeRenderTexture`、`resizeRenderTarget`、`getNativeContext`、`restoreNativeContext`）。`getCapabilities()` 是非虚 inline getter，读取后端在 `init()` 中填充的 `mCapabilities`。

上文各章**未覆盖**的接口共 22 个，它们是在这份计划写完之后才加进 RHI 的：

| 分类 | 新接口 | 当前 GLES3 状态 |
|------|--------|----------------|
| 裁剪 | `setScissorRect` | ✅ 已实现 |
| 深度约定 | `getDepthRemapMatrix` | ✅ 已实现（override） |
| 渲染目标 | `resizeRenderTexture`、`resizeRenderTarget` | ⛔ 未 override |
| 资源 | `createPixelBufferCubemap` | ✅ 已实现 |
| 资源 | `createStructuredBuffer` | 🚧 契约式不支持 |
| 结构化缓冲绑定 | `setVSStructuredBuffers`、`setPSStructuredBuffers`、`setCSStructuredBuffers` | 🚧 契约式不支持 |
| GPU-driven | `setCSUnorderedAccessBuffers`、`dispatch`、`dispatchIndirect`、`uavBarrier`、`copyStructureCount` | 🚧 契约式不支持 |
| 实例化绘制 | `renderIndexedInstanced`、`renderInstanced` | ⚠️ 已实现（不支持非零 startInstance） |
| 间接绘制 | `renderIndexedIndirect`、`renderIndirect` | 🚧 契约式不支持 |

---

## A.3 状态图例（替换上文的计划态图例）

上文用的是计划态图例（📋 待实现 / ⚡ 需适配 / 🔇 按设计为空）。既然代码已落地，本附录改用与 GL4、D3D11 文档一致的状态态图例：

| 标记 | 含义 |
|------|------|
| ✅ 已完成 | 功能完整实现 |
| ⚠️ 部分完成 | 有逻辑但缺关键分支、受运行时版本限制，或存在已知缺陷 |
| ❌ 未实现 | 函数体直接返回 `T3D_OK` / `nullptr`，**且未声明不支持**——调用方拿到"成功"却什么也没发生 |
| 🚧 契约式不支持 | 走 `T3D_RHI_UNSUPPORTED` 宏：断言能力位为 false、打警告、返回 `T3D_ERR_NOT_IMPLEMENT`。这是**合规状态**，不是缺陷 |
| 🔇 按设计为空 | 按设计意图不需要实现（如 Vulkan 专用的 begin/endRender） |
| ⛔ 未 override | 沿用 `RHIContext` 基类默认实现，返回 `T3D_ERR_NOT_IMPLEMENT` |

`T3D_RHI_UNSUPPORTED` 定义在 `source/Core/Include/T3DPrerequisites.h:92`，它断言能力位为 false、打警告日志、返回 `T3D_ERR_NOT_IMPLEMENT`，保证不会出现「能力位说支持但接口是空壳」的矛盾状态。**🚧 与 ❌ 的区别是：前者诚实报错让上层降级，后者谎报成功。**

---

## A.4 平台与构建：计划 vs 实际

### A.4.1 已落地的构建改造

第 22、23 章规划的 CMake 改造**基本全部完成**：

| 计划项（第 23.10 节清单） | 状态 | 实际情况 |
|------------------------|------|---------|
| `OpenGLES3/CMakeLists.txt` 重写 | ✅ 已完成 | 不再 `find_package(OpenGL)`，不再链接桌面 OpenGL 库 |
| `OpenGLES3/Runtime/CMakeLists.txt` 新建 | ✅ 已完成 | 产出 SHARED 库 `GLES3Renderer`，链接 `GLESv3`、`EGL`、`android`、`log`、引擎库、glslang |
| `Plugins/Renderer/CMakeLists.txt` 增加 Android 短路 | ✅ 已完成 | Android 直接 `add_subdirectory(OpenGLES3)` 跳过 `find_package`；非 Android 保留 find_package 流程（`source/Plugins/Renderer/CMakeLists.txt:140`） |
| `FindOpenGLES3.cmake` 增加 Android 短路 | ✅ 已完成 | `ANDROID` 分支设 `OPENGLES3_FOUND=TRUE` 并 `RETURN()` |
| `FindGlslang.cmake` 增加 Android 分支 | ✅ 已完成 | 按 `prebuilt/Android/${ANDROID_ABI}/lib*.a` 查找 |
| `assets/config/Android/Tiny3D.cfg` 注册插件 | ✅ 已完成 | 插件列表已含 `GLES3Renderer`（`assets/config/Android/Tiny3D.cfg:56`） |
| Android 平台默认开启 | ✅ 已完成 | `TINY3D_BUILD_RENDERSYSTEM_GLES3 TRUE`（`source/Plugins/Renderer/CMakeLists.txt:41`） |

> 注：`assets/config/Android/Tiny3D.cfg:60` 处仍保留了 `NullRenderer` 条目。需确认这是刻意的备选项还是遗留，若是遗留应清掉，避免渲染器选择产生歧义。

### A.4.2 未落地的部分：第 24 章的跨平台方案

第 24 章规划的「GLES3 扩展为跨平台统一后端」**尚未开始**：

| 计划项 | 状态 | 说明 |
|--------|------|------|
| `Desktop/` 子目录 + `GLES3DesktopContext` | ❌ 未开始 | 目录不存在 |
| Windows/Linux 驱动原生 EGL 路径 | ❌ 未开始 | 无 HWND / X11 Window 的 `EGLNativeWindowType` 获取代码 |
| ANGLE 集成 | ❌ 未开始 | 无 `eglGetPlatformDisplayEXT`、无 `EGL_PLATFORM_ANGLE_*` 相关代码 |
| macOS 支持 | ❌ 未开始 | — |
| 桌面平台配置文件注册 GLES3Renderer | ❌ 未开始 | `assets/config/Windows/Tiny3D.cfg` 只有 `GL4Renderer` |

**当前的硬约束**：`GLES3RenderWindow` 的 native window 获取只有 Android 分支，其余平台直接报错返回：

```cpp
// T3DGLES3RenderWindow.cpp:117-158
#if defined(T3D_OS_ANDROID)
    EGLNativeWindowType nativeWindow = (EGLNativeWindowType)info.window;
    // ... ANativeWindow_setBuffersGeometry / eglCreateWindowSurface ...
#else
    T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Unsupported platform for GLES3 render window");
    return false;
#endif
```

也就是说：**CMake 允许在非 Android 平台编译这个插件，但运行时创建窗口必然失败**。这是第 24 章 Phase 1 的起点。

### A.4.3 EGL 与版本降级

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **实现位置** | `initEGLContext`（T3DGLES3ContextBase.cpp:39）、`createEGLContext`（T3DGLES3ContextBase.cpp:109）、`destroyEGLContext`（T3DGLES3ContextBase.cpp:155） |
| **功能** | `eglBindAPI(EGL_OPENGL_ES_API)` → `eglGetDisplay(EGL_DEFAULT_DISPLAY)` → `eglInitialize` → `eglChooseConfig`（RGBA8 + D24S8 + `EGL_OPENGL_ES3_BIT` + `EGL_WINDOW_BIT`）→ 按 `{3,2} → {3,1} → {3,0}` 顺序循环 `eglCreateContext`，成功即记录 `mGLESMajor` / `mGLESMinor` |
| **备注** | 与第 20.1 节的计划完全一致。`mGLESMinor` 是后续所有版本门控（GS 需 ≥2、CS 需 ≥1、BaseVertex 需 ≥2、compute limit 查询需 ≥1）的唯一依据 |

**一处与计划的偏差**：第 20.1 节计划「成功后 `glGetIntegerv(GL_MAJOR_VERSION/GL_MINOR_VERSION)` 确认实际版本」。实际代码**没有做这一步二次确认**，直接采信 `eglCreateContext` 请求的版本号。多数驱动行为一致，但严格来说驱动可以返回一个更高版本的兼容上下文，此时 `mGLESMinor` 会低估实际能力，导致 GS / CS 被不必要地禁用。

### A.4.4 扩展检测

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成 |
| **实现位置** | `GLES3Mapping::detectExtensions()`（T3DGLES3Mapping.cpp:24），调用点在 `GLES3RenderWindow`（T3DGLES3RenderWindow.cpp:182） |
| **实际检测** | 只检测一个扩展：`GL_EXT_texture_format_BGRA8888` |
| **计划但未检测** | 第 5.4 节计划的 `GL_EXT_texture_border_clamp`（BorderColor）、`GL_EXT_texture_filter_anisotropic`（各向异性）；第 15.2 节计划的 `GL_EXT_draw_elements_base_vertex`；第 17.4 节计划的 `GL_EXT_copy_image` |
| **备注** | 用 `glGetString(GL_EXTENSIONS)` 单串匹配，未用 GLES 3.0 起推荐的 `glGetStringi(GL_EXTENSIONS, i)` 逐条枚举 |

---

## A.5 后端能力集

### A.5.1 fillCapabilities

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（能力位取值保守） |
| **签名** | `void fillCapabilities()`（`GLES3Context` 专有，非 RHI 接口） |
| **实现位置** | T3DGLES3Context.cpp:101（调用点 T3DGLES3Context.cpp:93，在 `initEGLContext` 之后） |
| **备注** | `mGLESMinor < 1` 时提前返回（GLES 3.0 没有 compute 相关 limit 可查）；否则查询 limit 后用 `while (glGetError() != GL_NO_ERROR) {}` 清理错误标志 |

### A.5.2 各能力位取值

| 字段 | GLES3 取值 | GL4 取值 | 说明 |
|------|-----------|---------|------|
| `supportsInstancing` | `true` | `true` | 实例化与 divisor 是 GLES 3.0 核心功能 |
| `supportsBaseInstance` | `false`（硬编码） | GL 4.2+ 动态判定 | GLES 各版本均无 `*BaseInstance` |
| `supportsCompute` | `false`（硬编码） | `false` | GLES 3.1+ 有 `glDispatchCompute`，但 RHI 接口未实现 |
| `supportsUnorderedAccess` | `false`（硬编码） | `false` | GLES 3.1+ 有 SSBO / `glBindImageTexture`，未实现 |
| `supportsStructuredBuffer` | `false`（硬编码） | `false` | 同上 |
| `supportsIndirectDraw` | `false`（硬编码） | `false` | GLES 3.1+ 有 `glDrawElementsIndirect`，未实现 |
| `supportsIndirectDispatch` | `false`（硬编码） | `false` | GLES 3.1+ 有 `glDispatchComputeIndirect`，未实现 |
| `supportsAppendConsumeBuffer` | `false`（硬编码） | `false` | 需用 atomic counter 手工搭 |
| `maxDispatchGroupCount[3]` | GLES≥3.1 时从 GL 查询 | 从 GL 查询 | 当前无消费者 |
| `maxComputeGroupSize[3]` | GLES≥3.1 时从 GL 查询 | 从 GL 查询 | 当前无消费者 |
| `maxComputeSharedMemory` | GLES≥3.1 时从 GL 查询 | 从 GL 查询 | 当前无消费者 |
| `maxUnorderedAccessSlots` | GLES≥3.1 时从 GL 查询 | 从 GL 查询 | 当前无消费者 |

**与第 13 章计划的偏差**：上文第 13.1 节写「GLES 3.1+ 支持 `GL_COMPUTE_SHADER`…**GLES3 后端优先于 GL4 实现 CS**」。实际情况是「一半落地」：compute shader 的**编译与 attach 已实现**（见 A.6.2），但 `dispatch` / SSBO / UAV 一整套仍是契约式不支持，因此 compute 整体**跑不起来**，能力位也就只能保持 false。

---

## A.6 逐模块实现状态

### A.6.1 已完整落地的模块（与计划一致或更好）

| 模块 | 接口 | 实现位置 | 备注 |
|------|------|---------|------|
| 变换 | `setViewProjectionTransform` | T3DGLES3Context.cpp:199 | 保持 GLES 原生 [-1,1] 深度，无 `glClipControl`；渲染到 FBO 时翻转投影 Y 轴并置 `mRenderingToFBO` |
| 深度约定 | `getDepthRemapMatrix` | T3DGLES3Context.cpp:229 | override 返回 Z 由 [-1,1] 映射到 [0,1] 的矩阵 |
| 渲染纹理 | `createRenderTexture` | T3DGLES3Context.cpp:273 | 颜色 RT / 深度 RT、MSAA（`glTexStorage2DMultisample`）+ Resolve FBO，**按计划用了 `glTexStorage2D` 不可变分配** |
| 渲染目标 | `setRenderTarget` | T3DGLES3Context.cpp:454 | 窗口 / FBO / MRT / depth-only |
| 视口与裁剪 | `setViewport`、`setScissorRect` | T3DGLES3Context.cpp:592、T3DGLES3Context.cpp:674 | 均含左上→左下原点 Y 翻转与无 RT 回退 |
| 清除 | `clearColor`、`clearDepth`、`clearDepthStencil` | T3DGLES3Context.cpp:730 起 | 按计划使用 `glClearDepthf` |
| 管线状态 | `create*State` × 4、`set*State` × 3 | T3DGLES3Context.cpp:800 起 | 按计划移除 `glPolygonMode` 与 `GL_DEPTH_CLAMP`；`setRasterizerState` 在 Y 翻转时交换 Front/Back（T3DGLES3Context.cpp:1083） |
| 顶点输入 | `createVertexDeclaration`、`setVertexDeclaration` | T3DGLES3Context.cpp:1145、T3DGLES3Context.cpp:1169 | VAO |
| 缓冲 | VBO / IBO / UBO 的 create 与 set | T3DGLES3Context.cpp:1194 起 | `setVertexBuffers` 含 instance divisor 配置 |
| 纹理 | 1D（height=1 的 2D 模拟）/ 2D / 3D / Cubemap | T3DGLES3Context.cpp:1381、T3DGLES3Context.cpp:1437、T3DGLES3Context.cpp:1493、T3DGLES3Context.cpp:1530 | 1D 模拟方案与第 7.6 节计划一致；绑定侧在 `bindPixelBuffers` 中同样按 `GL_TEXTURE_2D` 处理 |
| VS / PS | create + set + 三类资源绑定 | T3DGLES3Context.cpp:1598 起 | 延迟 link，UBO 经 `mPendingUBOs` 在 link 后统一 `glBindBufferBase` |
| 编译反射 | `compileShader`、`reflectShaderAllBindings` | T3DGLES3ContextBase.cpp:174、T3DGLES3ContextBase.cpp:275 | glslang，`EEsProfile` + 300/310/320（按 `mGLESMinor` 选择，T3DGLES3ContextBase.cpp:213），与第 14.1 节计划一致 |
| 绘制 | `setPrimitiveType`、`render` × 2 | T3DGLES3Context.cpp:2080、T3DGLES3Context.cpp:2133、T3DGLES3Context.cpp:2176 | `glDrawElementsBaseVertex` 在 GLES 3.2 下启用（T3DGLES3Context.cpp:2158），低版本走 `glDrawElements` 回退，与第 15.2 节计划一致 |
| 状态重置 | `reset` | T3DGLES3Context.cpp:2306 | 解绑 program / VAO / VBO / EBO / FBO，删除 program，清 `mPendingUBOs` |
| 缓冲复制 | `copyBuffer` | T3DGLES3Context.cpp:2471 | `glCopyBufferSubData` + `GL_COPY_READ_BUFFER`/`GL_COPY_WRITE_BUFFER`，限 VBO/IBO/UBO |
| 原生上下文 | `getNativeContext`、`restoreNativeContext` | T3DGLES3Context.cpp:2648、T3DGLES3Context.cpp:2655 | 返回 `EGLContext` / `eglMakeCurrent` |
| 帧边界 | `beginRender`、`endRender`、`beginPass`、`endPass` | T3DGLES3Context.h:150-154 | 🔇 按设计为空（内联 `T3D_OK`） |
| 窗口专有 | `initEGLContext`、`destroyEGLContext`、`swapBackBuffer`、`resizeRenderWindow` | 见 A.4.3 与 T3DGLES3Context.cpp:144、T3DGLES3Context.cpp:176 | 第 20 章计划的四个专有接口全部落地 |

多线程方面，几乎所有 GL 调用都包在 `ENQUEUE_UNIQUE_COMMAND` 中提交到 RHI 线程，与 GL4、D3D11 的模式一致；`writeBuffer` 会深拷贝 CPU 数据以跨线程持有。

### A.6.2 GLES3 领先于 GL4 的地方

这几处是本次复核最值得记录的结果——**在几何着色器与计算着色器的管线接线上，GLES3 后端比 GL4 后端更完整**：

| 接口 | GLES3 状态 | GL4 状态 | 差异 |
|------|-----------|---------|------|
| `setGeometryShader` | ✅ 已实现（T3DGLES3Context.cpp:1897） | 💬 实现被整段注释（T3DGL4Context.cpp:2199） | GLES3 会 `glAttachShader` 并置 `mProgramDirty`，GL4 的 GS 编译得出来但永远 attach 不进 program |
| `createGeometryShader` | ⚠️ 需 GLES 3.2，不满足时返回 `nullptr` + 警告（T3DGLES3Context.cpp:1839） | ✅ 无版本门控 | GLES3 有明确的版本门控与告知 |
| `createComputeShader` | ⚠️ 需 GLES 3.1，真实 `glCreateShader(GL_COMPUTE_SHADER)` + 编译 + 错误日志（T3DGLES3Context.cpp:1950） | ❌ 只打警告并返回空对象（T3DGL4Context.cpp:2246） | GLES3 能真编译出 CS |
| `setComputeShader` | ⚠️ 真实 attach（T3DGLES3Context.cpp:2008） | ❌ `{ return T3D_OK; }`（T3DGL4Context.cpp:2252） | 见下方警告 |
| `copyBuffer` | ✅ 已实现（T3DGLES3Context.cpp:2471） | ❌ TODO + `return T3D_OK`（T3DGL4Context.cpp:2941） | GL4 可直接移植 GLES3 的实现 |
| `blit(Tex→RT)` 的 TBR 优化 | ✅ 含 `glInvalidateFramebuffer`（T3DGLES3Context.cpp:2439） | — | 第 2.3 / 18.4 节计划的移动端优化，仅在这一处落地 |

> **⚠️ `setComputeShader` 有一个设计问题**：它把 compute shader `glAttachShader` 到与图形着色器**共用的** `mCurrentProgram` 上（T3DGLES3Context.cpp:2025-2029）。OpenGL / OpenGL ES 规范要求含 compute shader 的 program **不能**同时包含其它着色阶段，这样的 program 链接必然失败。上文第 13.2 节的计划本来是对的——「计算着色器使用独立 Program（不与图形 Program 混合）」——实现时没有遵循。由于 `dispatch` 本身是契约式不支持，这个问题目前不会被触发，但补齐 compute 时必须先修掉。

### A.6.3 契约式不支持（11 个）

| 项目 | 内容 |
|------|------|
| **状态** | 🚧 契约式不支持 |
| **接口** | `createStructuredBuffer`、`setVSStructuredBuffers`、`setPSStructuredBuffers`、`setCSStructuredBuffers`、`setCSUnorderedAccessBuffers`、`dispatch`、`dispatchIndirect`、`uavBarrier`、`copyStructureCount`、`renderIndexedIndirect`、`renderIndirect` |
| **实现位置** | T3DGLES3Context.cpp:2292-2302（每个接口一行） |
| **返回值** | `T3D_ERR_NOT_IMPLEMENT`（指针版 `nullptr`），并打警告日志 |
| **备注** | 与 GL4 的清单**完全一致**。GLES 3.1+ 在 API 层面支持 SSBO、`glBindImageTexture`、`glDispatchCompute`、`glMemoryBarrier`、`glDrawElementsIndirect`，因此这是**尚未实现**而非 API 不支持。设计范围见 `doc/todo/RHI-Compute-UAV-Indirect-Draw-Design-todo.md` |

### A.6.4 静默失败的接口（14 个）

以下接口直接返回 `T3D_OK` 或 `nullptr`，**未走 `T3D_RHI_UNSUPPORTED` 契约，上层无法察觉操作没有发生**：

| 分组 | 接口 | 实现位置 | 说明 |
|------|------|---------|------|
| Hull Shader | `createHullShader` | T3DGLES3Context.cpp:1811 | 返回 `nullptr`，**无警告日志** |
| Hull Shader | `setHullShader`、`setHSConstantBuffers`、`setHSPixelBuffers`、`setHSSamplers` | T3DGLES3Context.cpp:1816-1819 | `{ return T3D_OK; }` |
| Domain Shader | `createDomainShader` | T3DGLES3Context.cpp:1825 | 返回 `nullptr`，**无警告日志** |
| Domain Shader | `setDomainShader`、`setDSConstantBuffers`、`setDSPixelBuffers`、`setDSSamplers` | T3DGLES3Context.cpp:1830-1833 | `{ return T3D_OK; }` |
| 反射 | `reflectSamplerBindings` | T3DGLES3ContextBase.cpp:396 | 空 `return T3D_OK`。**GL4 与 D3D11 均已实现此接口**（D3D11 还专门把它从 Context 下沉到 ContextBase 以便 Console 变体复用） |
| 数据传输 | `blit(RT→RT)` | T3DGLES3Context.cpp:2343 | `return T3D_OK`，连 TODO 注释都没有 |
| 数据传输 | `blit(RT→Tex)` | T3DGLES3Context.cpp:2457 | 同上 |
| 数据传输 | `blit(Tex→Tex)` | T3DGLES3Context.cpp:2464 | 同上 |

Hull/Domain 属于 GLES 3.2 才有的能力（`GL_TESS_CONTROL_SHADER` / `GL_TESS_EVALUATION_SHADER`），上文第 10、11 章按「当前阶段实现为空返回」规划，实现照做了。但**「按计划为空」不等于「可以静默成功」**：`RHICapabilities` 目前没有 tessellation 能力位，无法套用 `T3D_RHI_UNSUPPORTED` 宏，至少应补上警告日志，或给 `RHICapabilities` 增加 `supportsTessellation` 字段后走统一契约。

三个 blit 重载是**上文第 17.1 / 17.3 / 17.4 节明确规划要实现的**（计划里写着用 `glBlitFramebuffer` 或 `glCopyImageSubDataEXT`），实际未落地，且比 GL4 更糟——GL4 的 `blit(RT→RT)` 至少留了 `// TODO: implement using glBlitFramebuffer`，GLES3 这三个连注释都没有。

### A.6.5 已知功能缺陷

| 缺陷 | 位置 | 影响 |
|------|------|------|
| `writeBuffer` 纹理分支尺寸传 0、格式硬编码 | T3DGLES3Context.cpp:2576 | 纹理动态更新完全不可用。**与 GL4 是同一处缺陷的复制**（`glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, ...)`），上文第 17.6 节本来写明要「修复 GL4 的 width/height=0 bug」，结果连 bug 一起抄了过来 |
| `setVertexShader` 无 nullptr 检查 | T3DGLES3Context.cpp:1650 | 传 nullptr 崩溃。上文第 8.2 节写「额外增加 nullptr 检查（修复 GL4 的已知缺陷）」，实际**未实现**；对比 `setPixelShader`（T3DGLES3Context.cpp:1752）、`setGeometryShader`、`setComputeShader` 都有 nullptr 分支，只有 VS 漏了 |
| `resetRenderTarget` 不重置 `mRenderingToFBO` | T3DGLES3Context.cpp:571 | 面剔除方向可能残留上一个 FBO 的翻转状态。与 GL4 同一问题 |
| `setComputeShader` attach 到图形 program | T3DGLES3Context.cpp:2025 | 见 A.6.2 的警告，补齐 compute 前必须修 |
| 压缩纹理格式映射缺失 | `T3DGLES3Mapping.cpp` | 无任何 `glCompressedTexImage2D` 调用，也无 ETC2/EAC/ASTC 格式映射。上文第 7.7 节要求「GLES3 必须支持 ETC2/EAC…需在 GLES3Mapping 中映射引擎格式到 ETC2/ASTC」，**未落地**。移动端纹理内存与带宽依赖压缩格式，这是实际上线前必须补的一项 |
| `resizeRenderTexture` / `resizeRenderTarget` 未 override | 基类 T3DRHIContext.h:125 / T3DRHIContext.h:134 | 渲染纹理动态改分辨率走不通。D3D11 已实现，GL4 同样缺失 |

---

## A.7 全量口径汇总统计（93 个虚接口）

| 状态 | 数量 | 说明 |
|------|------|------|
| ✅ 已完成 | 52 | 变换 / 深度 remap / 渲染纹理 / 渲染目标 / 视口 / 裁剪 / 清除 / 全部管线状态 / 顶点输入 / VBO·IBO·UBO / 1D·2D·3D·Cubemap 纹理 / VS·PS 全套 / GS 与 CS 的资源绑定 / 编译与全量反射 / 图元与基础绘制 / reset / `blit(Tex→RT)` / `copyBuffer` / 原生上下文 |
| ⚠️ 部分完成 | 10 | `createRenderWindow`（仅 Android）、`createGeometryShader` 与 `setGeometryShader`（需 3.2）、`createComputeShader` 与 `setComputeShader`（需 3.1，且 program 混用）、`renderIndexedInstanced` 与 `renderInstanced`（不支持非零 startInstance）、`setVertexShader`（缺 nullptr 检查）、`resetRenderTarget`（不重置 FBO 标志）、`writeBuffer`（纹理分支尺寸与格式错误） |
| ❌ 未实现 | 14 | Hull(5) + Domain(5) + `reflectSamplerBindings` + `blit(RT→RT)` + `blit(RT→Tex)` + `blit(Tex→Tex)` |
| 🚧 契约式不支持 | 11 | StructuredBuffer(4) + UAV(2) + `dispatch` + `dispatchIndirect` + `copyStructureCount` + 间接绘制(2) |
| 🔇 按设计为空 | 4 | `beginRender`、`endRender`、`beginPass`、`endPass` |
| ⛔ 未 override | 2 | `resizeRenderTexture`、`resizeRenderTarget` |
| **合计** | **93** | |

### 换算成完成度

- **有效可用**（✅ + 🚧 + 🔇，即行为符合预期或明确报错）：**67 / 93 ≈ 72%**
- **纯 ✅ 完成度**：**52 / 93 ≈ 56%**

### 三个后端横向对比

| 维度 | D3D11（基线） | GL4 | GLES3 |
|------|-------------|-----|-------|
| ✅ 已完成 | 86 | 53 | 52 |
| ⚠️ 部分完成 | 0（4 项有文档化限制，计入 ✅） | 3 | 10 |
| ❌ 未实现（静默） | 0 | 19 | 14 |
| 🚧 契约式不支持 | 0 | 11 | 11 |
| 🔇 按设计为空 | 7（含沿用基类默认的 `getDepthRemapMatrix` 与两个原生上下文接口） | 4 | 4 |
| 💬 被注释禁用 | 0 | 1 | 0 |
| ⛔ 未 override | 0 | 2 | 2 |
| Compute / UAV / Indirect | ✅ 完整 | 🚧 | 🚧 |
| Tessellation | ✅ 完整 | ❌ | ❌（GLES 3.2 才有 API） |
| Geometry Shader | ✅ 完整 | 💬 绑定被注释 | ⚠️ 需 3.2，已可用 |
| 四个 blit 重载 | ✅ 统一收口 | ⚠️ 仅 `Tex→RT` | ⚠️ 仅 `Tex→RT` |
| `copyBuffer` | ✅ | ❌ | ✅ |
| `reflectSamplerBindings` | ✅ | ✅ | ❌ |
| RT resize | ✅ | ⛔ | ⛔ |
| 平台覆盖 | Windows | Windows / Linux | **仅 Android** |

> GLES3 的 ⚠️ 数量明显偏高，主要不是质量问题，而是**运行时版本门控**造成的：GS 需 3.2、CS 需 3.1、非零 startInstance GLES 根本没有。这类"取决于设备"的状态在 D3D11（Feature Level 一次判定）和 GL4（桌面驱动版本普遍够高）里不明显。

---

## A.8 更新后的待办优先级

### P0 — 修复缺陷

| 项 | 问题 | 建议 |
|----|------|------|
| `writeBuffer` 纹理分支 | 尺寸传 0、格式硬编码 `GL_RGBA` | 从 `PixelBuffer2D` 描述符取 width/height 与像素格式，经 `GLES3Mapping` 转换。**与 GL4 一并修** |
| `setVertexShader` | 缺 nullptr 检查 | 补 nullptr 解绑分支，与本文件中 `setPixelShader` / `setGeometryShader` / `setComputeShader` 对齐 |
| `resetRenderTarget` | 不重置 `mRenderingToFBO` | 一行修复 |
| Hull/Domain create 与 set 系列、三个 blit 重载、`reflectSamplerBindings` | 静默返回成功 | 至少补警告日志并返回 `T3D_ERR_NOT_IMPLEMENT`；或给 `RHICapabilities` 增加 `supportsTessellation` 后走 `T3D_RHI_UNSUPPORTED` 统一契约 |
| `setComputeShader` | CS attach 到图形 program，链接必然失败 | 改为独立 compute program（补齐 dispatch 时一并处理） |

### P1 — 核心功能

| 项 | 原因 | 实现要点 |
|----|------|---------|
| `blit(RT→RT)` / `blit(RT→Tex)` / `blit(Tex→Tex)` | 上文第 17 章已规划、数据传输是管线核心 | `glBlitFramebuffer`（临时 FBO 挂载目标纹理）；建议仿照 D3D11 的 `resolveBlitEndpoint` + `doBlit` 统一收口 |
| 压缩纹理（ETC2 / EAC / ASTC） | 移动端纹理内存与带宽的前置条件 | `GLES3Mapping` 补格式映射 + `glCompressedTexImage2D` 上传路径 |
| `reflectSamplerBindings` | GL4 与 D3D11 均已实现，GLES3 缺失会导致采样器 binding 元数据不全 | 可直接参考 `T3DGL4Context.cpp:2486` |
| `resizeRenderTexture` / `resizeRenderTarget` | 渲染纹理动态分辨率 | 重建 GLTexture / GLFBO / MSAA RBO / Resolve 附件全套 |
| TBR 优化推广到 `endPass` | 目前只有 `blit(Tex→RT)` 一处调了 `glInvalidateFramebuffer` | 上文第 18.4 节的计划：在 `endPass` 丢弃不再需要的附件 |
| Android 真机验证 ESSL 变体选取 | `Shader-MultiBackend-Variant-Design-todo.md` 的端到端验证只覆盖 D3D11 / GL4 / VK，GLES3 侧无记录（见 A.10.4） | 在 Android 上跑 ResourceApp，确认 Bundle 中 `kESSL` 变体被正确选中并编译通过 |

### P2 — 能力补齐

| 项 | 原因 |
|----|------|
| Compute + SSBO + UAV + Indirect 一整套 | GLES 3.1+ 原生支持，且 CS 编译链路已通，是本后端相对 GL4 的先发优势所在。实现后须把 `fillCapabilities` 的能力位改为按 `mGLESMinor` 动态判定。设计范围见 `doc/todo/RHI-Compute-UAV-Indirect-Draw-Design-todo.md` |
| 扩展检测补全 | BorderColor、各向异性、`GL_EXT_draw_elements_base_vertex`、`GL_EXT_copy_image` 均已在上文规划但未检测；建议改用 `glGetStringi` 枚举 |
| GL 版本二次确认 | `initEGLContext` 后补 `glGetIntegerv(GL_MAJOR_VERSION/GL_MINOR_VERSION)`，避免低估驱动实际能力而误禁 GS / CS |

### P3 — 跨平台（第 24 章 Phase 1）

| 项 | 说明 |
|----|------|
| `Desktop/` 变体 | 建 `GLES3DesktopContext`，从 SDL 取 HWND / X11 Window 作为 `EGLNativeWindowType`，复用 `Base/` 全部 EGL 与映射代码。这是「无 Android 设备也能调试 GLES3 渲染路径」的关键一步 |
| ANGLE 集成 | 覆盖 macOS 并统一各驱动行为 |
| 清理 `assets/config/Android/Tiny3D.cfg` 的 `NullRenderer` 条目 | 确认是备选项还是遗留 |

---

## A.9 上文各章的有效性说明

| 章节 | 当前有效性 |
|------|-----------|
| 第 1～19 章（逐接口实现方案） | ⚠️ **状态标记全部失效**，但「GLES3 适配方案」「关键差异」「GLES3 API」几列的技术判断基本都被实现验证为正确，仍有参考价值 |
| 第 20 章（专有接口） | ✅ 四个接口全部落地 |
| 第 21 章（插件系统） | ✅ 全部落地 |
| 第 22、23 章（CMake 集成） | ✅ 清单 8 项全部落地，可视为已完成记录 |
| 第 24 章（跨平台方案） | 📋 **仍然完全未开始**，是本后端最大的一块待办，内容依然适用 |
| 汇总统计与实现优先级表 | ❌ 已失效，以本附录 A.7 / A.8 为准 |

---

## A.10 跨文档一致性与回填清单

GLES3 后端的需求分散在多份设计文档中，本节记录它们与本文档的对应关系，以及哪些说法已经与代码脱节。

### A.10.1 GPU Readback：接口尚未进入 RHI

`doc/todo/GPU-Readback-onRender-Design-todo.md` §5 / §7.1 规划了四个 readback 接口，要求在 `T3DGLES3Context` 中先加 stub（第一期 GLES3 定位为 stub），并在 §7.3 要求实现后回填本文档。

**核对结果**：`T3DRHIContext.h` 中目前**搜不到任何 `beginRead*` / `endRead*` / readback 相关声明**，该设计尚未落到 RHI 接口层。因此本附录 A.7 的 93 个接口口径不含 readback，统计成立；这四个接口进入 RHI 后总数变为 97，需同步更新统计。届时 GLES3 的 `Usage::kCopy` 映射到 `GL_STREAM_READ`（该文档 §3.1 已指定）。

### A.10.2 Compute / UAV / Indirect：对应 RHI-Compute 的第五期 E4

`doc/todo/RHI-Compute-UAV-Indirect-Draw-Design-todo.md` 把 GLES3 的补齐工作单列为**第五期任务 E4**（状态 ⏸，预估 2 天），要求「ES3.1 分支真实现 + **设备黑名单**」。本附录 A.8 的 P2 项即对应 E4。

该文档 §10.1 记录的实现程度与本次复核一致：实例化绘制真实现、其余为 stub。另有三条约束值得在实现前记住：

- **§8.2 明确要求**：GLES 3.0 下 `startInstance != 0` **必须报错，不能静默忽略**。当前代码返回 `T3D_ERR_NOT_IMPLEMENT`（T3DGLES3Context.cpp:2209 / T3DGLES3Context.cpp:2266），**已符合该约束**。不过 GLES 各版本都没有 `*BaseInstance`，这个限制不只是 3.0 的问题，`supportsBaseInstance` 硬编码 false 是正确取值。
- **§8.2 的版本分级**：GLES 3.1+ 才有 compute / SSBO / indirect，3.0 全无。因此 E4 的能力位必须按 `mGLESMinor` 判定，而不是一个总开关。
- **§12.5 的风险提示**：GLES 3.1 的 compute 驱动质量参差不齐，建议配设备黑名单，或第一期只开 instancing。这一点对 Android 尤其重要。

其 §8.1 给出了 RHI 接口到 GLES API 的完整映射表，可直接作为 E4 的实现依据。

### A.10.3 Cubemap：Skybox 文档中的「死代码」说法已过时

`doc/todo/Skybox-Support-Design-todo.md` §2.2 称 GLES3 的 `setPixelBuffers` 虽有 `kPixelBufferCubemap` 分支但是**死代码**（引用 `T3DGLES3Context.cpp:2598-2601`）。

**核对结果：这个问题已经修好了。** T3DGLES3Context.cpp:2867-2870 的分支正确 cast 到 `GLES3PixelBufferCubemap` 并使用 `GL_TEXTURE_CUBE_MAP` target。该文档 §4.3 对 GLES3「`createPixelBufferCubemap` ✅ 完整实现 + 逐面 BGRA→RGBA swap」的判断与本次复核一致（见 A.6.1）。Skybox 文档的 §2.2 待办项可以关闭。

顺带一提：Skybox 文档引用具体行号这件事，本身就是「GLES3 源码远不止空壳」的旁证——`Metal-Renderer-Backend-todo.md` §1.2 也引用了 `GLES3Context::getDepthRemapMatrix` 的位置。本文正文第 23 章的「源码文件均为空壳」是这几份文档中唯一与代码不符的描述。

### A.10.4 Shader 变体链路：GLES3 侧缺少验证记录

`doc/todo/Shader-MultiBackend-Variant-Design-todo.md` 定义 `OPENGLES3` → `SHADER_LANGUAGE::kESSL`（scc target `essl`），Android 打包用 `--keep-languages essl,spirv`。

**但该文档的端到端验证只覆盖了 D3D11 / GL4 / VK 三个后端**，GLES3 运行时正确选取 `kESSL` 变体这条链路**没有同等的验证记录**。考虑到 GLES3 只能在 Android 上跑，这个验证缺口需要真机才能补。建议列为 A.8 的 P1 补充项：在 Android 上跑一次 ResourceApp，确认 Bundle 中的 ESSL 变体被正确选中并编译通过。

另外 `doc/todo/ShaderConductor-Replacement-todo.md` §2.3 要求 `convertToESSLVersion` 映射表原样保留，§6.2.2 指出 ESSL 100 需要 `flatten_buffer_block()`——当前 shader 均为 `#pragma target 4.0`，暂不触发，但 GLES 3.0 设备上若出现低版本 ESSL 产出需注意。

### A.10.5 需要修正的上游文档

| 文档 | 问题 |
|------|------|
| `doc/Tiny3D-Architecture.md` §3.6 | RHI 后端表写的是 "OpenGL ES **2/3**"、平台仅标「Android / 可选」，未反映 GLES3Renderer 已是 Android 上注册的主渲染器（`assets/config/Android/Tiny3D.cfg:56`）；同表也完全没有 OpenGL 4 |
| `doc/refs/D3D11-vs-OpenGLES3-API-Mapping.md` | 内容质量较好（§16 完整 compute 章节 + 附录 B 版本能力矩阵 + 附录 C 的 TBR/`glInvalidateFramebuffer` 说明），缺的是与 Tiny3D RHI 接口的一一对应，以及 EGL 生命周期与引擎 `RenderWindow` 的集成说明。实现 E4 与第 24 章跨平台方案时，这两块需要自行补齐 |
