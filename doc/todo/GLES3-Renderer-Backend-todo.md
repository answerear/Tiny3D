# GLES3 渲染后端接口实现计划

> 本文档基于 `RHIContext` 纯虚接口定义，逐一规划 `GLES3Context`（Runtime 版本）的实现方案。
>
> - **接口定义**：`source/Core/Include/RHI/T3DRHIContext.h`
> - **参考实现（GL4 基类）**：`source/Plugins/Renderer/OpenGL4/Base/Source/T3DGL4ContextBase.cpp`
> - **参考实现（GL4 Window）**：`source/Plugins/Renderer/OpenGL4/Window/Source/T3DGL4Context.cpp`
> - **API 映射参考**：`doc/refs/D3D11-vs-OpenGLES3-API-Mapping.md`
> - **目标目录**：`source/Plugins/Renderer/OpenGLES3/`
>
> **看实现状态请直接翻 §0「全接口实现状态速查表」**（2026-09 逐函数核对代码，带行号）；差距清单与修法见**附录 B**。第 1～20 章是当初的实施方案，状态列已同步为实际状态。
>
> **要动手实现请看 `doc/todo/GLES3-Backend-Alignment-Implementation-todo.md`**：本文档回答「差什么」，那份文档回答「怎么补」——按阶段给出每一项的代码骨架、GLES 版本门控、与 GL4 的「不能照抄」清单、验证方案与提交拆分。

---

## 实现状态图例

> **2026-09 更新**：本文档第 1～20 章原本是**实施计划**，状态列用的是计划期标记（📋 待实现 / ⚡ 需适配）。这批标记从未随代码更新，导致「`createRenderWindow` 已经实现了但文档写 📋 待实现」这类误读。现已把第 1～20 章的状态列全部改为**对照 2026-09 代码的实际状态**，并在状态里直接给出实现位置。
>
> 各章的「GL4 实现要点 / GLES3 适配方案 / 关键差异 / GLES3 API」几列仍是当初的规划内容，技术判断基本被实现验证为正确，保留作参考；但**不要**把这几列当成已实现的描述。

| 标记 | 含义 |
|------|------|
| ✅ 已完成 | 功能完整，行为与 D3D11 / GL4 基线一致 |
| ⚠️ 部分完成 | 接口可调用，但有已知缺陷、或受 GLES 版本门控、或行为与基线有差异 |
| ❌ 未实现 | 空实现且**静默返回成功**（`T3D_OK` / `nullptr`），上层无法感知 |
| 🚧 契约式不支持 | 走 `T3D_RHI_UNSUPPORTED` 系列宏，返回明确错误码并打日志，上层可走降级路径 |
| 🔇 按设计为空 | 按设计不需要实现（GLES 即时模式无对应概念） |
| ⛔ 未 override | 沿用 `RHIContext` 默认实现（返回 `T3D_ERR_NOT_IMPLEMENT`） |

---

## 0. 全接口实现状态速查表（2026-09，96 个接口）

> 口径：93 个 `RHIContext` 虚接口 + 3 个 readback 接口，与 `GL4-Renderer-Backend-todo.md` 一致。
> 行号均为 2026-09 时点的 `source/Plugins/Renderer/OpenGLES3/` 代码位置。
> 差距的成因分析、修法与参考实现见**附录 B**；本表只回答「现在是什么状态」。

### 0.1 变换（2）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `setViewProjectionTransform` | ✅ | `T3DGLES3Context.cpp:199` | 深度保持 [-1,1]，渲染到 FBO 时翻 Y |
| `getDepthRemapMatrix` | ✅ | `T3DGLES3Context.cpp:229` | Z 从 [-1,1] 重映射到 [0,1] |

### 0.2 渲染目标（6）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `createRenderWindow` | ⚠️ | `T3DGLES3Context.cpp:242` | **实现完整**（含 RHI 线程 EGL 转移）；⚠ 只是因为 `GLES3RenderWindow::init`（`T3DGLES3RenderWindow.cpp:104`）非 Android 平台直接报错（`156`） |
| `createRenderTexture` | ✅ | `T3DGLES3Context.cpp:273` | 颜色 / 深度 / MSAA + Resolve FBO 全套 |
| `setRenderTarget` | ✅ | `T3DGLES3Context.cpp:454` | 含 MRT `glDrawBuffers` 与 depth-only FBO |
| `resetRenderTarget` | ✅ | `T3DGLES3Context.cpp` | 已复位 `mRenderingToFBO` |
| `resizeRenderTexture` | ✅ | `T3DGLES3Context.cpp` | `release` + `build` helper；当前 RT 命中时重绑 |
| `resizeRenderTarget` | ✅ | `T3DGLES3Context.cpp` | 窗口转发 `resizeRenderWindow`，离屏遍历颜色 / 深度附件 |

### 0.3 视口与裁剪（2）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `setViewport` | ✅ | `T3DGLES3Context.cpp:592` | 带「无 RT 时用当前 GL viewport」回退路径 |
| `setScissorRect` | ✅ | `T3DGLES3Context.cpp:674` | 已按 GL 左下原点翻 Y |

### 0.4 清除（3）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `clearColor` | ✅ | `T3DGLES3Context.cpp:730` | |
| `clearDepth` | ✅ | `T3DGLES3Context.cpp:751` | 用 `glClearDepthf` |
| `clearDepthStencil` | ✅ | `T3DGLES3Context.cpp:775` | |

### 0.5 渲染状态（7）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `createBlendState` | ✅ | `T3DGLES3Context.cpp` | 独立混合 + A2C；3.2 走 `glBlendFuncSeparatei`，低版本降级到 RT0 |
| `createDepthStencilState` | ✅ | `T3DGLES3Context.cpp:843` | |
| `createRasterizerState` | ✅ | `T3DGLES3Context.cpp:888` | 按 GLES 特性去掉 FillMode / DepthClamp |
| `createSamplerState` | ✅ | `T3DGLES3Context.cpp` | 各向异性 / 边框色按扩展检测；无扩展回落到 CLAMP_TO_EDGE |
| `setBlendState` | ✅ | `T3DGLES3Context.cpp` | 同 `createBlendState` |
| `setDepthStencilState` | ✅ | `T3DGLES3Context.cpp` | 使用描述符 `StencilRef`（GL4 同步修复） |
| `setRasterizerState` | ✅ | `T3DGLES3Context.cpp:1083` | 含 FBO 翻 Y 时交换 Front/Back |

### 0.6 顶点输入（2）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `createVertexDeclaration` | ✅ | `T3DGLES3Context.cpp:1145` | VAO |
| `setVertexDeclaration` | ✅ | `T3DGLES3Context.cpp:1169` | |

### 0.7 缓冲区与纹理（9）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `createVertexBuffer` | ✅ | `T3DGLES3Context.cpp:1194` | |
| `setVertexBuffers` | ✅ | `T3DGLES3Context.cpp:1226` | 含 `glVertexAttribDivisor` 实例化步进 |
| `createIndexBuffer` | ✅ | `T3DGLES3Context.cpp:1292` | |
| `setIndexBuffer` | ✅ | `T3DGLES3Context.cpp:1324` | 支持 16/32 位索引 |
| `createConstantBuffer` | ✅ | `T3DGLES3Context.cpp:1349` | UBO |
| `createPixelBuffer1D` | ✅ | `T3DGLES3Context.cpp:1381` | height=1 的 2D 纹理模拟；含 BGRA→RGBA 转换 |
| `createPixelBuffer2D` | ⚠️ | `T3DGLES3Context.cpp:1437` | 只上传 mip 0 后 `glGenerateMipmap`，忽略 `desc.mipmaps` / `arraySize`（**GL4 同样**），见 B.6 |
| `createPixelBuffer3D` | ✅ | `T3DGLES3Context.cpp:1493` | |
| `createPixelBufferCubemap` | ✅ | `T3DGLES3Context.cpp:1530` | 逐面上传 + BGRA 转换 |

### 0.8 顶点 / 像素着色器（10）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `createVertexShader` | ✅ | `T3DGLES3Context.cpp:1598` | |
| `setVertexShader` | ✅ | `T3DGLES3Context.cpp` | 收口到 `attachGraphicsShader`，含 nullptr |
| `setVSConstantBuffers` | ✅ | `T3DGLES3Context.cpp:1681` | → `stageConstantBuffers:2822` |
| `setVSPixelBuffers` | ✅ | `T3DGLES3Context.cpp:1686` | → `bindPixelBuffers:2854`，该 helper 有 MSAA / 解绑缺陷，见 B.3#6 |
| `setVSSamplers` | ✅ | `T3DGLES3Context.cpp:1691` | → `bindSamplers:2922`，不解绑，见 B.3#7 |
| `createPixelShader` | ✅ | `T3DGLES3Context.cpp:1700` | |
| `setPixelShader` | ✅ | `T3DGLES3Context.cpp:1752` | 含 nullptr 解绑分支 |
| `setPSConstantBuffers` | ✅ | `T3DGLES3Context.cpp:1792` | |
| `setPSPixelBuffers` | ✅ | `T3DGLES3Context.cpp:1797` | 同 `setVSPixelBuffers` |
| `setPSSamplers` | ✅ | `T3DGLES3Context.cpp:1802` | 同 `setVSSamplers` |

### 0.9 Hull / Domain 着色器（10）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `createHullShader` | ⚠️ | `T3DGLES3Context.cpp` | GLES 3.2 / `GL_EXT_tessellation_shader`；低版本首次警告 + `nullptr`。绘制侧 patch 图元仍待引擎扩枚举 |
| `setHullShader` / `setHSConstantBuffers` / `setHSPixelBuffers` / `setHSSamplers` | ⚠️ | `T3DGLES3Context.cpp` | 3.2 转发 `attachGraphicsShader` / 绑定 helper；低版本 `T3D_ERR_NOT_IMPLEMENT` |
| `createDomainShader` | ⚠️ | `T3DGLES3Context.cpp` | 同 Hull |
| `setDomainShader` / `setDSConstantBuffers` / `setDSPixelBuffers` / `setDSSamplers` | ⚠️ | `T3DGLES3Context.cpp` | 同 Hull |

> ES 3.2 core 已含 tessellation，可按 GS 同样的版本门控实现，见 B.2.3。

### 0.10 几何 / 计算着色器（10）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `createGeometryShader` | ⚠️ | `T3DGLES3Context.cpp:1839` | 需 GLES 3.2，低版本返回 nullptr + 警告 |
| `setGeometryShader` | ⚠️ | `T3DGLES3Context.cpp:1897` | 需 GLES 3.2 |
| `setGSConstantBuffers` / `setGSPixelBuffers` / `setGSSamplers` | ✅ | `T3DGLES3Context.cpp:1931`-`1944` | 与 VS/PS 共用绑定路径 |
| `createComputeShader` | ✅ | `T3DGLES3Context.cpp` | GLES 3.1；glslang 已认 `kCompute` |
| `setComputeShader` | ✅ | `T3DGLES3Context.cpp` | 独立 compute program，dispatch 后恢复图形 program |
| `setCSConstantBuffers` / `setCSPixelBuffers` / `setCSSamplers` | ✅ | `T3DGLES3Context.cpp:2042`-`2055` | 复用图形绑定路径；SSBO / Image 绑定未实现 |

### 0.11 Shader 编译与反射（3）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `compileShader` | ✅ | `T3DGLES3ContextBase.cpp` | Vertex / Pixel / Geometry / Compute / Hull / Domain，按设备版本与 stage 最低要求取大 |
| `reflectShaderAllBindings` | ✅ | `T3DGLES3ContextBase.cpp` | 含 SPIRV-Cross 合并名还原（`stripCombinedSamplerName`） |
| `reflectSamplerBindings` | ✅ | `T3DGLES3ContextBase.cpp` | 与 GL4 同构，sampler 类型表按 GLES 裁剪 |

### 0.12 结构化缓冲 / UAV / Compute 派发（9）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `createStructuredBuffer` | ✅ | `T3DGLES3Context.cpp` | GLES 3.1 SSBO + 可选 atomic counter；3.0 走 `T3D_RHI_UNSUPPORTED` |
| `setVSStructuredBuffers` / `setPSStructuredBuffers` / `setCSStructuredBuffers` | ✅ | `T3DGLES3Context.cpp` | VS 额外检查 `GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS` |
| `setCSUnorderedAccessBuffers` | ✅ | `T3DGLES3Context.cpp` | bind 版初值写入，无 DSA |
| `dispatch` | ✅ | `T3DGLES3Context.cpp` | 独立 compute program + 恢复图形 program |
| `dispatchIndirect` | ✅ | `T3DGLES3Context.cpp` | |
| `uavBarrier` | ✅ | `T3DGLES3Context.cpp` | `glMemoryBarrier` 六位 |
| `copyStructureCount` | ✅ | `T3DGLES3Context.cpp` | `glCopyBufferSubData` |

> 全部走契约式不支持，能力位在 `fillCapabilities`（`T3DGLES3Context.cpp:101`）硬编码 false。GLES 3.1 原生支持这一整套，补齐方案见 B.5。

### 0.13 图元与绘制（7）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `setPrimitiveType` | ✅ | `T3DGLES3Context.cpp:2080` | 引擎枚举 5 项全覆盖 |
| `render`（indexed） | ✅ | `T3DGLES3Context.cpp:2133` | 3.2 走 `glDrawElementsBaseVertex`，低版本回落 |
| `render`（non-indexed） | ✅ | `T3DGLES3Context.cpp:2176` | |
| `renderIndexedInstanced` | ⚠️ | `T3DGLES3Context.cpp:2201` | `startInstance != 0` 返回 `T3D_ERR_NOT_IMPLEMENT`（GLES 无 base instance，这是正确处理） |
| `renderInstanced` | ⚠️ | `T3DGLES3Context.cpp:2259` | 同上 |
| `renderIndexedIndirect` | ✅ | `T3DGLES3Context.cpp` | GLES 3.1；args 第 4 字段必须为 0 |
| `renderIndirect` | ✅ | `T3DGLES3Context.cpp` | 同上 |

### 0.14 状态重置（1）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `reset` | ✅ | `T3DGLES3Context.cpp` | 复位 FBO 标志、全部 stage 变体、独立 compute program、scratch FBO |

### 0.15 数据传输（6）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `blit`（RT→RT） | ✅ | `T3DGLES3Context.cpp` | 三段式：`resolveBlitEndpoint` + `doBlit` |
| `blit`（Tex→RT） | ✅ | `T3DGLES3Context.cpp` | MSAA 先等尺寸 resolve；`size==ZERO` 为整张；普通 2D 走 scratch FBO |
| `blit`（RT→Tex） | ✅ | `T3DGLES3Context.cpp` | 同上 |
| `blit`（Tex→Tex） | ✅ | `T3DGLES3Context.cpp` | 同上 |
| `copyBuffer` | ✅ | `T3DGLES3Context.cpp` | 六道校验 + `GL_COPY_READ/WRITE_BUFFER` |
| `writeBuffer` | ✅ | `T3DGLES3Context.cpp` | 纹理分支按描述符取宽高 / 格式；含 SSBO |

### 0.16 GPU Readback（3）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `map(offset, size)` | ✅ | `T3DGLES3Context.cpp` | staging + `glCopyBufferSubData` |
| `map(region)` | ✅ | `T3DGLES3Context.cpp` | FBO + `glReadPixels` + PBO；深度 / Cubemap / 压缩明确报错 |
| `unmap` | ✅ | `T3DGLES3Context.cpp` | `glMapBufferRange` + `syncRHIThread` |

### 0.17 帧命令与原生上下文（6）

| 接口 | 状态 | 实现位置 | 备注 |
|------|------|---------|------|
| `beginRender` / `endRender` | 🔇 | `T3DGLES3Context.h:155` / `156` | 内联 `return T3D_OK` |
| `beginPass` / `endPass` | 🔇 | `T3DGLES3Context.h:158` / `159` | `endPass` 可考虑放 TBR `glInvalidateFramebuffer` |
| `getNativeContext` | ✅ | `T3DGLES3Context.cpp:2669` | 返回 `EGLContext` |
| `restoreNativeContext` | ✅ | `T3DGLES3Context.cpp:2676` | `eglMakeCurrent` 回主 surface |

### 0.18 汇总

| 状态 | 数量 | 占比 |
|------|------|------|
| ✅ 已完成 | 76 | 79% |
| ⚠️ 部分完成 | 16 | 17% |
| ❌ 未实现（静默成功） | 0 | 0% |
| 🚧 契约式不支持 | 0 | 0% |
| 🔇 按设计为空 | 4 | 4% |
| ⛔ 未 override | 0 | 0% |
| **合计** | **96** | |

- **有效可用**（✅ + 🔇，行为符合预期或明确报错）：**80 / 96 ≈ 83%**
- 剩余 16 个 ⚠️：`createRenderWindow`（非 Android 平台报错）、`createPixelBuffer2D`（mip / array 与 GL4 同缺）、GS 2 个与 Hull / Domain 10 个受 3.2 / tessellation 门控且引擎尚无 patch 图元、Instanced 2 个无 base instance（永久缺口，处理正确）。
- 对比：D3D11 约 89 ✅ / 7 🔇，GL4 Window 96 个全覆盖。GLES3 已按 `GLES3-Backend-Alignment-Implementation-todo.md` 对齐到同族能力（3.1 compute / SSBO / indirect / readback，3.2 独立混合与 tess shader）。

### 0.19 GLES3Context 专有接口（不计入 96）

| 接口 | 状态 | 实现位置 |
|------|------|---------|
| `initEGLContext` | ✅ | `T3DGLES3ContextBase.cpp:39` |
| `createEGLContext` | ✅ | `T3DGLES3ContextBase.cpp:109`（3.2 → 3.1 → 3.0 降级） |
| `destroyEGLContext` | ✅ | `T3DGLES3ContextBase.cpp:155` |
| `init` | ✅ | `T3DGLES3Context.cpp:80` |
| `fillCapabilities` | ✅ | `T3DGLES3Context.cpp`（`confirmDeviceVersion` 后按 3.1 / 3.2 动态赋值） |
| `swapBackBuffer` | ✅ | `T3DGLES3Context.cpp:144` |
| `resizeRenderWindow` | ✅ | `T3DGLES3Context.cpp:176` |

---

## 1. 变换 (Transform)

### 1.1 setViewProjectionTransform

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:199`） |
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
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:242`）— **接口本身已完整实现**：`GLES3RenderWindow::create` + 记录 `mMainSurface` + RHI 线程下把 EGL context 转移到渲染线程（`250`-`265`）。⚠ 的唯一原因是 `GLES3RenderWindow::init`（`T3DGLES3RenderWindow.cpp:104`）只有 Android 分支，其它平台走到 `156` 直接报 "Unsupported platform" |
| **签名** | `RHIRenderTargetPtr createRenderWindow(RenderWindow *renderWindow)` |
| **GL4 实现要点** | 创建 GL4RenderWindow：WGL 创建 Core Profile 4.5 上下文 + MSAA 像素格式 + 多线程 Context 转移 |
| **GLES3 实现方案** | 创建 GLES3RenderWindow：1) 从 `SysWMInfo` 获取 `ANativeWindow*`；2) `eglCreateWindowSurface(display, config, nativeWindow, nullptr)`；3) `eglMakeCurrent` 绑定 Context 到 Surface；4) 存储 `EGLSurface` + 宽高到 GLES3RenderWindow 成员 |
| **GLES3 API** | `eglCreateWindowSurface`, `eglMakeCurrent`, `eglQuerySurface(EGL_WIDTH/EGL_HEIGHT)` |
| **参数** | `renderWindow` — 引擎渲染窗口对象，包含 ANativeWindow 句柄、宽高、MSAA 等描述信息 |
| **返回值** | 成功返回 `GLES3RenderWindowPtr`，失败返回 `nullptr` |

### 2.2 createRenderTexture

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:273`）— 颜色 / 深度 / MSAA + Resolve FBO 全套；MSAA 走 `glTexStorage2DMultisample`（需 GLES 3.1+） |
| **签名** | `RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2D *buffer)` |
| **GL4 实现要点** | 根据像素格式创建颜色 RT 或深度/模板 RT，支持 MSAA（额外创建 GLResolveTex + GLResolveFBO） |
| **GLES3 实现方案** | 1) `glGenTextures` + `glTexStorage2D` 创建不可变纹理；2) `glGenFramebuffers` + `glFramebufferTexture2D` 附加到 FBO；3) MSAA：使用 `glRenderbufferStorageMultisample` + `glFramebufferRenderbuffer`（GLES 3.0+）；4) Resolve 通过 `glBlitFramebuffer` |
| **GLES3 API** | `glTexStorage2D`（替代 `glTexImage2D` + Mutable），`glRenderbufferStorageMultisample`, `glBlitFramebuffer` |
| **关键差异** | GLES3 推荐使用 `glTexStorage2D` 创建不可变纹理（性能更好）；MSAA 纹理需要 `GL_TEXTURE_2D_MULTISAMPLE`（GLES 3.1+），3.0 仅支持 Renderbuffer MSAA |

### 2.3 setRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:454`）— 含 MRT `glDrawBuffers` 与 depth-only FBO；`glInvalidateFramebuffer` 目前只在 `blit(Tex→RT)` 里调，未推广到此处 |
| **签名** | `TResult setRenderTarget(RenderTarget *renderTarget)` |
| **GL4 实现要点** | Window 类型绑定默认 FBO(0)；Texture 类型绑定颜色 FBO + MRT + depth-only FBO |
| **GLES3 实现方案** | 逻辑与 GL4 基本一致：1) Window → `glBindFramebuffer(GL_FRAMEBUFFER, 0)`；2) Texture → 绑定 FBO + `glDrawBuffers` 配置 MRT（GLES 3.0 最少支持 4 个 Color Attachment）；3) 额外调用 `glInvalidateFramebuffer` 优化 TBR 架构 |
| **GLES3 API** | `glBindFramebuffer`, `glDrawBuffers`, `glFramebufferTexture2D` |
| **关键差异** | 需在渲染结束后调用 `glInvalidateFramebuffer` 丢弃不再需要的附件（移动端 TBR 性能关键） |

### 2.4 resetRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:571`）— FBO 已解绑，但不复位 `mRenderingToFBO`，见附录 B.3#3 |
| **签名** | `TResult resetRenderTarget()` |
| **GL4 实现要点** | `glBindFramebuffer(GL_FRAMEBUFFER, 0)` |
| **GLES3 实现方案** | 与 GL4 完全一致：`glBindFramebuffer(GL_FRAMEBUFFER, 0)` |
| **GLES3 API** | `glBindFramebuffer` |

---

## 3. 视口 (Viewport)

### 3.1 setViewport

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:592`）— 带「无当前 RT 时取 GL viewport」回退路径。注意实现里只调 `glViewport`，`glScissor` 由 `setScissorRect` 单独负责 |
| **签名** | `TResult setViewport(const Viewport &viewport)` |
| **GL4 实现要点** | 根据 RenderTarget 类型获取宽高，按 viewport 的归一化比例计算实际视口大小 |
| **GLES3 实现方案** | 与 GL4 逻辑一致：1) 从当前 RenderTarget 获取宽高；2) 计算实际像素区域；3) `glViewport(x, y, w, h)` + `glScissor(x, y, w, h)` |
| **GLES3 API** | `glViewport`, `glScissor` |

### 3.2 setScissorRect

> 本接口在写作本计划时尚未进入 `RHIContext`，属后补条目。

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:674`） |
| **签名** | `TResult setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height)` |
| **实现要点** | 从当前 RenderTarget（窗口 / 颜色纹理 / 深度纹理）取帧缓冲高度，按 GL 左下原点换算 `glY = fbHeight - (y + height)`；无当前 RT 时回退到 `glGetIntegerv(GL_VIEWPORT)` 的高度 |
| **备注** | 裁剪开关本身由 `setRasterizerState` 的 `ScissorEnable` 控制 |

---

## 4. 清除操作 (Clear)

### 4.1 clearColor

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:730`） |
| **签名** | `TResult clearColor(const ColorRGB &color)` |
| **GL4 实现要点** | `glClearColor` + `glClear(GL_COLOR_BUFFER_BIT)` |
| **GLES3 实现方案** | 与 GL4 完全一致：`glClearColor(r, g, b, 1.0f)` + `glClear(GL_COLOR_BUFFER_BIT)` |
| **GLES3 API** | `glClearColor`, `glClear` |
| **移动端注意** | 在 TBR 架构上，每帧开头必须 Clear 所有使用的附件（通知驱动重置 tile） |

### 4.2 clearDepth

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:751`）— 已用 `glClearDepthf`；无当前 RT 时直接返回 |
| **签名** | `TResult clearDepth(Real depth)` |
| **GL4 实现要点** | `glClearDepth(depth)` + `glClear(GL_DEPTH_BUFFER_BIT)` |
| **GLES3 实现方案** | GLES3 无 `glClearDepth`，使用 `glClearDepthf(depth)` 替代 |
| **GLES3 API** | `glClearDepthf`（注意 `f` 后缀，GLES 仅支持 float 版本） |
| **关键差异** | GL4 使用 `glClearDepth`（double），GLES3 使用 `glClearDepthf`（float） |

### 4.3 clearDepthStencil

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:775`） |
| **签名** | `TResult clearDepthStencil(Real depth, uint32_t stencil)` |
| **GL4 实现要点** | 同时清除深度和模板 |
| **GLES3 实现方案** | `glClearDepthf(depth)` + `glClearStencil(stencil)` + `glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)` |
| **GLES3 API** | `glClearDepthf`, `glClearStencil`, `glClear` |

---

## 5. 渲染状态 (Render State)

### 5.1 createBlendState

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:800`）— 只读 `RenderTargetStates[0]`，忽略 `IndependentBlendEnable` 与 `AlphaToCoverageEnable`（D3D11 / GL4 均支持），见附录 B.4.5 |
| **签名** | `RHIBlendStatePtr createBlendState(BlendState *state)` |
| **GL4 实现要点** | 将引擎 BlendDesc 映射为 `GL4BlendStateData`（srcRGB/dstRGB/srcAlpha/dstAlpha/opRGB/opAlpha/colorMask） |
| **GLES3 实现方案** | 与 GL4 逻辑一致，创建 `GLES3BlendStateData` POD：1) 通过 `GLES3Mapping` 转换 BlendFactor → `GL_SRC_ALPHA` 等；2) 转换 BlendOp → `GL_FUNC_ADD` 等；3) 存储 colorMask |
| **GLES3 API** | 创建时无 GL 调用（仅存储映射后的枚举值） |
| **枚举映射** | `D3D11_BLEND_SRC_ALPHA` → `GL_SRC_ALPHA`；`D3D11_BLEND_OP_ADD` → `GL_FUNC_ADD`（见 API Mapping 第15节） |

### 5.2 createDepthStencilState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:843`） |
| **签名** | `RHIDepthStencilStatePtr createDepthStencilState(DepthStencilState *state)` |
| **GL4 实现要点** | 映射 DepthStencilDesc 为 `GL4DepthStencilStateData`，支持前后面独立 Stencil |
| **GLES3 实现方案** | 与 GL4 一致：映射 CompareFunc → `GL_LESS/GL_LEQUAL` 等；映射 StencilOp → `GL_KEEP/GL_REPLACE` 等 |
| **GLES3 API** | 创建时无 GL 调用 |

### 5.3 createRasterizerState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:888`）— 按计划去掉了 FillMode 与 DepthClamp；`MultisampleEnable` 已存入 POD 但 `setRasterizerState` 未使用（GLES 无 `GL_MULTISAMPLE` 开关，属正常） |
| **签名** | `RHIRasterizerStatePtr createRasterizerState(RasterizerState *state)` |
| **GL4 实现要点** | 映射 RasterizerDesc：FillMode/CullMode/FrontFace/Scissor/DepthClip/DepthBias/MSAA |
| **GLES3 实现方案** | 移除 FillMode 映射（GLES 不支持 `glPolygonMode`，仅 Fill）；移除 `GL_DEPTH_CLAMP`（GLES 不支持）；其余 CullMode/FrontFace/Scissor/DepthBias 映射与 GL4 一致 |
| **关键差异** | 1) 无 `glPolygonMode`（固定 Fill）；2) 无 `GL_DEPTH_CLAMP`；3) Wireframe 模式需 fallback 为 `GL_LINES` 绘制或忽略 |

### 5.4 createSamplerState

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:926`）— Wrap / Filter / LOD / Comparison 都做了；但计划里的**扩展运行时检测没做**：`GL_TEXTURE_MAX_ANISOTROPY_EXT` 被无条件调用（`963`），BorderColor 完全未处理 |
| **签名** | `RHISamplerStatePtr createSamplerState(SamplerState *state)` |
| **GL4 实现要点** | `glGenSamplers` + 设置 Wrap/Filter/Anisotropy/LOD/Comparison/BorderColor |
| **GLES3 实现方案** | 1) `glGenSamplers` + `glSamplerParameteri`（GLES 3.0+）；2) BorderColor 需要 GLES 3.2 或 `GL_EXT_texture_border_clamp` 扩展，运行时检测；3) 各向异性需要 `GL_EXT_texture_filter_anisotropic` 扩展 |
| **GLES3 API** | `glGenSamplers`, `glSamplerParameteri`, `glSamplerParameterf` |
| **关键差异** | 1) `GL_CLAMP_TO_BORDER` 需 GLES 3.2 或扩展；2) `GL_TEXTURE_MAX_ANISOTROPY_EXT` 需扩展；3) `GL_MIRROR_CLAMP_TO_EDGE` 不支持 |

### 5.5 setBlendState

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:999`）— 只有全局 `glBlendFuncSeparate` + `glColorMask`；per-RT 独立混合（3.2 的 `glBlendFuncSeparatei` / `glColorMaski`）与 `GL_SAMPLE_ALPHA_TO_COVERAGE`（3.0 core）都缺，见附录 B.4.5 |
| **签名** | `TResult setBlendState(BlendState *state)` |
| **GL4 实现要点** | `glEnable/glDisable(GL_BLEND)` + `glBlendFuncSeparate` + `glBlendEquationSeparate` + `glColorMask` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glEnable(GL_BLEND)`, `glBlendFuncSeparate`, `glBlendEquationSeparate`, `glColorMask` |

### 5.6 setDepthStencilState

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:1034`）— 深度与前后面 Stencil 全套已实现；但 `glStencilFuncSeparate` 的 ref 硬编码为 1（`1062`-`1063`），未取 `DepthStencilDesc::StencilRef`。**GL4 同样有此问题**（`T3DGL4Context.cpp:1627`），D3D11 / Vulkan / Metal 都用了真实 ref |
| **签名** | `TResult setDepthStencilState(DepthStencilState *state)` |
| **GL4 实现要点** | `glEnable/Disable(GL_DEPTH_TEST)` + `glDepthFunc` + `glDepthMask` + `glStencilFuncSeparate` + `glStencilOpSeparate` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glEnable(GL_DEPTH_TEST)`, `glDepthFunc`, `glDepthMask`, `glStencilFuncSeparate`, `glStencilOpSeparate` |

### 5.7 setRasterizerState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1083`）— 含 FBO 翻 Y 时交换 Front/Back 的处理 |
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
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1145`） |
| **签名** | `RHIVertexDeclarationPtr createVertexDeclaration(VertexDeclaration *decl)` |
| **GL4 实现要点** | `glGenVertexArrays` 创建 VAO |
| **GLES3 实现方案** | 与 GL4 完全一致：`glGenVertexArrays(1, &vao)`。GLES 3.0 **强制使用 VAO**（与 GL4 Core Profile 一致） |
| **GLES3 API** | `glGenVertexArrays` |

### 6.2 setVertexDeclaration

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1169`） |
| **签名** | `TResult setVertexDeclaration(VertexDeclaration *decl)` |
| **GL4 实现要点** | `glBindVertexArray(VAO)` + 缓存到 `mPendingVertexDecl` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glBindVertexArray` |

---

## 7. 缓冲区 (Buffer)

### 7.1 createVertexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1194`） |
| **签名** | `RHIVertexBufferPtr createVertexBuffer(VertexBuffer *buffer)` |
| **GL4 实现要点** | `glGenBuffers` + `glBufferData(GL_ARRAY_BUFFER)` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glGenBuffers`, `glBindBuffer(GL_ARRAY_BUFFER)`, `glBufferData` |

### 7.2 setVertexBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1226`）— 延迟配置顶点属性；含 `glVertexAttribDivisor` 的按实例步进 |
| **签名** | `TResult setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets)` |
| **GL4 实现要点** | 若有 `mPendingVertexDecl`，延迟配置顶点属性（`glEnableVertexAttribArray` + `glVertexAttribPointer` / `glVertexAttribIPointer`） |
| **GLES3 实现方案** | 与 GL4 逻辑一致，但需注意：1) GLES 3.0 不支持 `glVertexAttribLPointer`（double 类型）；2) 仅支持 `glVertexAttribPointer`（float）和 `glVertexAttribIPointer`（integer） |
| **GLES3 API** | `glBindBuffer(GL_ARRAY_BUFFER)`, `glEnableVertexAttribArray`, `glVertexAttribPointer`, `glVertexAttribIPointer` |
| **关键差异** | 无 double 类型顶点属性支持 |

### 7.3 createIndexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1292`） |
| **签名** | `RHIIndexBufferPtr createIndexBuffer(IndexBuffer *buffer)` |
| **GL4 实现要点** | `glGenBuffers` + `glBufferData(GL_ELEMENT_ARRAY_BUFFER)` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glGenBuffers`, `glBindBuffer(GL_ELEMENT_ARRAY_BUFFER)`, `glBufferData` |

### 7.4 setIndexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1324`）— 16 / 32 位索引都支持 |
| **签名** | `TResult setIndexBuffer(IndexBuffer *buffer)` |
| **GL4 实现要点** | `glBindBuffer(GL_ELEMENT_ARRAY_BUFFER)` + 映射索引类型 |
| **GLES3 实现方案** | 与 GL4 一致。支持 `GL_UNSIGNED_SHORT` 和 `GL_UNSIGNED_INT`（GLES 3.0+ 支持 32 位索引） |
| **GLES3 API** | `glBindBuffer(GL_ELEMENT_ARRAY_BUFFER)` |

### 7.5 createConstantBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1349`）— 未做 `GL_MAX_UNIFORM_BLOCK_SIZE` 运行时校验 |
| **签名** | `RHIConstantBufferPtr createConstantBuffer(ConstantBuffer *buffer)` |
| **GL4 实现要点** | `glGenBuffers` + `glBufferData(GL_UNIFORM_BUFFER)` |
| **GLES3 实现方案** | 与 GL4 完全一致（UBO 是 GLES 3.0 核心功能） |
| **GLES3 API** | `glGenBuffers`, `glBindBuffer(GL_UNIFORM_BUFFER)`, `glBufferData` |
| **限制** | GLES 3.0 最小保证 UBO 大小为 16384 字节（`GL_MAX_UNIFORM_BLOCK_SIZE`），需运行时查询 |

### 7.6 createPixelBuffer1D

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1381`）— 按计划用 height=1 的 2D 纹理模拟；实际用的是 `glTexImage2D` 而不是计划里的 `glTexStorage2D`；额外做了 BGRA→RGBA 的 CPU 侧转换 |
| **签名** | `RHIPixelBuffer1DPtr createPixelBuffer1D(PixelBuffer1D *buffer)` |
| **GL4 实现要点** | `glGenTextures` + `glTexImage1D` |
| **GLES3 实现方案** | GLES 3.x **不支持** 1D 纹理（无 `GL_TEXTURE_1D`、无 `glTexImage1D`）。实现为：使用高度为 1 的 2D 纹理模拟（`glTexStorage2D(GL_TEXTURE_2D, levels, format, width, 1)`） |
| **GLES3 API** | `glGenTextures`, `glBindTexture(GL_TEXTURE_2D)`, `glTexStorage2D(... width, 1)` |
| **关键差异** | GLES 无原生 1D 纹理，需用 height=1 的 2D 纹理模拟 |

### 7.7 createPixelBuffer2D

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:1437`）— 用 `glTexImage2D` + `glGenerateMipmap`（非计划里的 `glTexStorage2D`）；只上传 mip 0，**忽略 `desc.mipmaps` 与 `desc.arraySize`**（GL4 同样如此，见附录 B.6）；压缩格式路径不存在（`PixelFormat` 枚举里也没有压缩格式，见附录 B.2.2） |
| **签名** | `RHIPixelBuffer2DPtr createPixelBuffer2D(PixelBuffer2D *buffer)` |
| **GL4 实现要点** | `glGenTextures` + `glTexImage2D` + `glGenerateMipmap` |
| **GLES3 实现方案** | 推荐使用 `glTexStorage2D`（不可变分配，性能更好）+ `glTexSubImage2D` 上传数据 + `glGenerateMipmap` |
| **GLES3 API** | `glGenTextures`, `glTexStorage2D`, `glTexSubImage2D`, `glGenerateMipmap` |
| **压缩格式** | GLES3 必须支持 ETC2/EAC，不支持 BC1-BC7（DXT）。需在 `GLES3Mapping` 中映射引擎格式到 ETC2/ASTC |

### 7.8 createPixelBuffer3D

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1493`）— 用 `glTexImage3D`（非计划里的 `glTexStorage3D`） |
| **签名** | `RHIPixelBuffer3DPtr createPixelBuffer3D(PixelBuffer3D *buffer)` |
| **GL4 实现要点** | `glGenTextures` + `glTexImage3D` |
| **GLES3 实现方案** | 与 GL4 逻辑一致，推荐使用 `glTexStorage3D` + `glTexSubImage3D`（GLES 3.0+ 支持 3D 纹理） |
| **GLES3 API** | `glGenTextures`, `glTexStorage3D`, `glTexSubImage3D` |

### 7.9 createPixelBufferCubemap

> 本接口在写作本计划时尚未进入 `RHIContext`，属后补条目。

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1530`） |
| **签名** | `RHIPixelBufferCubemapPtr createPixelBufferCubemap(PixelBufferCubemap *buffer)` |
| **实现要点** | `glGenTextures` + `GL_TEXTURE_CUBE_MAP`，逐面 `glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, ...)`，面偏移按 `Image::getBPP` 计算；含 BGRA→RGBA 转换；结尾 `glGenerateMipmap` + 三个方向 `GL_CLAMP_TO_EDGE` |
| **备注** | `Skybox-Support-Design-todo.md` §2.2 说的「`setPixelBuffers` 的 cubemap 分支是死代码」已不成立，`bindPixelBuffers`（`T3DGLES3Context.cpp:2890`）会正确绑 `GL_TEXTURE_CUBE_MAP` |

---

## 8. 顶点着色器 (Vertex Shader)

### 8.1 createVertexShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1598`） |
| **签名** | `RHIShaderPtr createVertexShader(ShaderVariant *shader)` |
| **GL4 实现要点** | `glCreateShader(GL_VERTEX_SHADER)` + `glShaderSource` + `glCompileShader`，含编译错误日志 |
| **GLES3 实现方案** | 与 GL4 逻辑一致。着色器源码应为 ESSL 300 es / 310 es / 320 es（由 scc.exe 交叉编译生成）。需确保传入的着色器有 `precision highp float;` 声明 |
| **GLES3 API** | `glCreateShader(GL_VERTEX_SHADER)`, `glShaderSource`, `glCompileShader`, `glGetShaderiv`, `glGetShaderInfoLog` |
| **关键差异** | GLSL 400 → ESSL 300 es；需要 precision 限定符 |

### 8.2 setVertexShader

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:1650`）— **计划里的 nullptr 检查没做**，传 nullptr 会崩；GL4 已经把这条收口到 `attachGraphicsShader`（`T3DGL4Context.cpp:4455`），见附录 B.3#2 |
| **签名** | `TResult setVertexShader(ShaderVariant *shader)` |
| **GL4 实现要点** | 删除旧 Program，创建新 Program 并 attach shader，标记 `mProgramDirty = true` |
| **GLES3 实现方案** | 与 GL4 一致。额外增加 nullptr 检查（修复 GL4 的已知缺陷） |
| **GLES3 API** | `glCreateProgram`, `glAttachShader`, `glDeleteProgram` |

### 8.3 setVSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1681` → `stageConstantBuffers:2822`） |
| **签名** | `TResult setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **GL4 实现要点** | 委托 `stageConstantBuffers()`，将 cbuffer 名→GL buffer handle 存入 `mPendingUBOs` |
| **GLES3 实现方案** | 与 GL4 完全一致（UBO 绑定机制相同） |
| **GLES3 API** | 无直接 GL 调用（仅缓存到 `mPendingUBOs`，在 `render()` 时通过 `glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo)` 绑定） |

### 8.4 setVSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1686` → `bindPixelBuffers:2854`）— ⚠ 共用的 `bindPixelBuffers` 有两个缺陷：句柄为 0 时不解绑、MSAA 渲染纹理不切 `GLResolveTex`，见附录 B.3#6 |
| **签名** | `TResult setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **GL4 实现要点** | 委托 `bindPixelBuffers()` → `glActiveTexture` + `glBindTexture` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glActiveTexture(GL_TEXTURE0 + n)`, `glBindTexture(GL_TEXTURE_2D, tex)` |

### 8.5 setVSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1691` → `bindSamplers:2922`）— 含 HLSL `s#` 寄存器到 GL 纹理单元的 remap；⚠ 句柄为 0 时不解绑，见附录 B.3#7 |
| **签名** | `TResult setVSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **GL4 实现要点** | 委托 `bindSamplers()` → `glBindSampler` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glBindSampler(unit, sampler)` |

---

## 9. 像素着色器 (Pixel Shader / Fragment Shader)

### 9.1 createPixelShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1700`） |
| **签名** | `RHIShaderPtr createPixelShader(ShaderVariant *shader)` |
| **GL4 实现要点** | `glCreateShader(GL_FRAGMENT_SHADER)` + 编译 |
| **GLES3 实现方案** | 与 GL4 一致。需确保 ESSL 源码有 `precision mediump float;`（或 highp）声明 |
| **GLES3 API** | `glCreateShader(GL_FRAGMENT_SHADER)`, `glShaderSource`, `glCompileShader` |
| **关键差异** | Fragment Shader 在 GLES 中必须声明 default precision |

### 9.2 setPixelShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1752`）— 含 nullptr 解绑分支 |
| **签名** | `TResult setPixelShader(ShaderVariant *shader)` |
| **GL4 实现要点** | 支持 nullptr 解绑；attach shader 到 Program |
| **GLES3 实现方案** | 与 GL4 一致 |
| **GLES3 API** | `glAttachShader`, `glDetachShader` |

### 9.3 setPSConstantBuffers / setPSPixelBuffers / setPSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:1792` / `1797` / `1802`）— 与 VS 同路径，因此也带同样的 `bindPixelBuffers` / `bindSamplers` 缺陷 |
| **签名** | 同 VS 对应接口 |
| **GLES3 实现方案** | 与 VS 阶段完全一致（GL 中 UBO/Texture/Sampler 绑定不区分 shader stage） |

---

## 10. Hull 着色器 (Tessellation Control)

### 10.1 createHullShader / setHullShader / setHSConstantBuffers / setHSPixelBuffers / setHSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现（`T3DGLES3Context.cpp:1811`-`1819`）— `createHullShader` 返回 `nullptr`、四个 `set*` 静默 `return T3D_OK`，**都没有日志**，上层无法感知。GL4 已全部实现（`T3DGL4Context.cpp:2349`），且 ES 3.2 core 就含 tessellation，可按 GS 的版本门控方式补齐，见附录 B.2.3 |
| **签名** | 各自标准签名 |
| **GLES3 实现方案** | GLES 3.2 支持 `GL_TESS_CONTROL_SHADER`。当前阶段实现为空返回 `T3D_OK`（与 GL4 后端一致），后续可在检测到 GLES 3.2 时启用 |
| **GLES3 API** | `glCreateShader(GL_TESS_CONTROL_SHADER)`（仅 GLES 3.2） |

---

## 11. Domain 着色器 (Tessellation Evaluation)

### 11.1 createDomainShader / setDomainShader / setDSConstantBuffers / setDSPixelBuffers / setDSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现（`T3DGLES3Context.cpp:1825`-`1833`）— 同 Hull Shader，静默返回成功。GL4 已实现（`T3DGL4Context.cpp:2391`） |
| **签名** | 各自标准签名 |
| **GLES3 实现方案** | 同 Hull Shader，当前阶段实现为空返回 |
| **GLES3 API** | `glCreateShader(GL_TESS_EVALUATION_SHADER)`（仅 GLES 3.2） |

---

## 12. 几何着色器 (Geometry Shader)

### 12.1 createGeometryShader

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:1839`）— 已按计划做 `mGLESMinor >= 2` 门控，低版本返回 nullptr + 警告；`compileShader` 侧也支持 Geometry 阶段 |
| **签名** | `RHIShaderPtr createGeometryShader(ShaderVariant *shader)` |
| **GL4 实现要点** | `glCreateShader(GL_GEOMETRY_SHADER)` + 编译 |
| **GLES3 实现方案** | 仅 GLES 3.2（或 `GL_EXT_geometry_shader`）支持。运行时检查 `mGLESMinor >= 2`，不支持时返回 nullptr 并记录警告日志 |
| **GLES3 API** | `glCreateShader(GL_GEOMETRY_SHADER)`（GLES 3.2） |

### 12.2 setGeometryShader / setGSConstantBuffers / setGSPixelBuffers / setGSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | `setGeometryShader` ⚠️ 部分完成（`T3DGLES3Context.cpp:1897`，需 3.2）；三个资源绑定接口 ✅ 已完成（`1931`-`1944`，与 VS/PS 共用路径） |
| **签名** | 各自标准签名 |
| **GLES3 实现方案** | 与 GL4 逻辑一致（attach shader 到 Program）。需运行时检查 GS 支持 |

---

## 13. 计算着色器 (Compute Shader)

### 13.1 createComputeShader

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:1950`）— `glCreateShader(GL_COMPUTE_SHADER)` 与 3.1 门控都做了，但**上游 `compileShader` 不支持 `kCompute` 阶段**（`T3DGLES3ContextBase.cpp:191`-`201` 只分派 V/P/G），实际拿不到编译后的 ESSL 源码，链路是断的，见附录 B.2.1 |
| **签名** | `RHIShaderPtr createComputeShader(ShaderVariant *shader)` |
| **GL4 实现要点** | ~~GL4 后端当前未实现（按设计为空）~~ → **已过时**：GL4 已完整实现（`T3DGL4Context.cpp:2511`），并配了独立 compute program 与 `ensureComputeProgramLinked`（`2955`） |
| **GLES3 实现方案** | GLES 3.1+ 支持 `GL_COMPUTE_SHADER`。运行时检查 `mGLESMinor >= 1`，支持时通过 `glCreateShader(GL_COMPUTE_SHADER)` 创建并编译。GLES3 后端优先于 GL4 实现 CS |
| **GLES3 API** | `glCreateShader(GL_COMPUTE_SHADER)`, `glShaderSource`, `glCompileShader`（GLES 3.1+） |

### 13.2 setComputeShader

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:2008`）— **没有按计划用独立 Program**：CS 被 attach 进图形 `mCurrentProgram`（`2025`-`2031`），GL/ES 规范都不允许 compute 与图形阶段混链，链接必然失败；另外 `shader == nullptr` 与 3.0 设备都静默 `return T3D_OK`。见附录 B.3#4 |
| **签名** | `TResult setComputeShader(ShaderVariant *shader)` |
| **GLES3 实现方案** | 计算着色器使用独立 Program（不与图形 Program 混合）。`glUseProgram(computeProgram)` → `glDispatchCompute` |
| **GLES3 API** | `glUseProgram`, `glDispatchCompute`（GLES 3.1+） |

### 13.3 setCSConstantBuffers / setCSPixelBuffers / setCSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:2042`-`2055`）— 但只是复用了图形阶段的 UBO / 纹理 / 采样器绑定路径；**计划里的 SSBO 与 `glBindImageTexture` 并未实现**，对应的 `setCSStructuredBuffers` / `setCSUnorderedAccessBuffers` 是 🚧 契约式不支持（`2295` / `2296`） |
| **GLES3 实现方案** | SSBO 绑定使用 `glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo)`；Image 绑定使用 `glBindImageTexture`（GLES 3.1+） |
| **GLES3 API** | `glBindBufferBase(GL_SHADER_STORAGE_BUFFER)`, `glBindImageTexture` |

### 13.4 结构化缓冲 / UAV / Dispatch（后补接口，共 9 个）

> 这批接口在写作本计划时尚未进入 `RHIContext`，属后补条目。

| 接口 | 状态 | 位置 |
|------|------|------|
| `createStructuredBuffer` | 🚧 | `T3DGLES3Context.cpp:2292` |
| `setVSStructuredBuffers` / `setPSStructuredBuffers` / `setCSStructuredBuffers` | 🚧 | `2293`-`2295` |
| `setCSUnorderedAccessBuffers` | 🚧 | `2296` |
| `dispatch` / `dispatchIndirect` | 🚧 | `2297` / `2298` |
| `uavBarrier` | 🚧 | `2299` |
| `copyStructureCount` | 🚧 | `2300` |

- 全部走 `T3D_RHI_UNSUPPORTED*` 宏，返回明确错误码并打日志（比静默返回成功好，上层可降级）。
- 能力位在 `fillCapabilities`（`T3DGLES3Context.cpp:101`-`113`）硬编码为 false：`supportsCompute`、`supportsUnorderedAccess`、`supportsStructuredBuffer`、`supportsIndirectDraw`、`supportsIndirectDispatch`、`supportsAppendConsumeBuffer`。只有 compute 相关的 limit（work group count / size、shared memory、SSBO 数）在 3.1+ 时真查了 GL。
- GLES 3.1 core 原生支持 SSBO / `glBindImageTexture` / `glDispatchCompute` / `glMemoryBarrier`，可按 `mGLESMinor >= 1` 放开；D3D11 与 GL4 都已全套实现（GL4 见 `T3DGL4Context.cpp:3151` 起）。补齐方案见附录 B.5。

---

## 14. Shader 编译与反射 (Shader Compilation & Reflection)

### 14.1 compileShader

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:2059` → `T3DGLES3ContextBase.cpp:174` / `185`）— ESSL profile 与版本选择都按计划做了（`EEsProfile` + 300/310/320），但 stage 分派只有 Vertex / Pixel / Geometry，`kCompute` / `kHull` / `kDomain` 直接报 "Unsupported shader stage"，这是 CS 链路断掉的根因 |
| **签名** | `TResult compileShader(ShaderVariant *shader)` |
| **GL4 实现要点** | 委托 `glslangCompileAndReflect()`，使用 glslang 库解析 GLSL 400，提取 Uniform Block 和 Sampler 信息 |
| **GLES3 实现方案** | 复用 glslang 反射流程，但需将着色器 profile 设为 `EEsProfile`，版本设为 300/310/320（对应 ESSL）。glslang 已支持 ESSL 着色器解析 |
| **关键差异** | GL4 使用 `ENoProfile` + 400；GLES3 使用 `EEsProfile` + 300/310/320 |

### 14.2 reflectShaderAllBindings

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3ContextBase.cpp:275`）— 含 SPIRV-Cross 合并结构体名的还原处理 |
| **签名** | `TResult reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)` |
| **GL4 实现要点** | 从 glslang 缓存提取 Uniform Block 成员和 Sampler 绑定信息 |
| **GLES3 实现方案** | 与 GL4 逻辑一致。反射数据结构相同（GlslangBlockInfo/GlslangUniformInfo）。ESSL 的 `layout(std140)` uniform block 内存布局与桌面 GL 一致 |

### 14.3 reflectSamplerBindings

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现（`T3DGLES3ContextBase.cpp:396`）— 空 `return T3D_OK`，反射数据都在但没填回 `samplerParams`。GL4（基类 `T3DGL4ContextBase.cpp:506`，Window 版 `T3DGL4Context.cpp:2835`）与 D3D11 均已实现，见附录 B.4.2 |
| **签名** | `TResult reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams)` |
| **GL4 实现要点** | 更新已有 samplerParams 的 binding 索引 |
| **GLES3 实现方案** | 与 GL4 完全一致 |

---

## 15. 图元与绘制 (Primitive & Draw)

### 15.1 setPrimitiveType

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:2080`）— 引擎 `PrimitiveType` 只有 5 项（PointList / LineList / LineStrip / TriangleList / TriangleStrip），全部有对应 GL 枚举，不存在计划里担心的 QUADS / 邻接图元问题 |
| **签名** | `TResult setPrimitiveType(PrimitiveType primitive)` |
| **GL4 实现要点** | 映射引擎枚举为 GL 枚举存储到 `mPrimitiveType` |
| **GLES3 实现方案** | 映射规则：`TRIANGLES`→`GL_TRIANGLES`, `LINES`→`GL_LINES`, `POINTS`→`GL_POINTS` 等。移除 GLES 不支持的枚举：`GL_QUADS`, `GL_POLYGON`。邻接图元（`GL_TRIANGLES_ADJACENCY`）仅 GLES 3.2 支持，`GL_PATCHES` 仅 GLES 3.2 支持 |
| **GLES3 API** | 无 GL 调用（仅存储枚举值） |
| **关键差异** | 无 `GL_QUADS`/`GL_POLYGON`；邻接/Patch 需 3.2 |

### 15.2 render (indexed)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:2133`）— 延迟 Link + UBO 绑定 + 采样器绑定齐全；`baseVertex != 0` 时按 3.2 走 `glDrawElementsBaseVertex`，低版本回落到 `glDrawElements` 并打警告 |
| **签名** | `TResult render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)` |
| **GL4 实现要点** | 延迟 Program Link + `glUseProgram` + `bindPendingUniformBlocks` + `setupSamplerBindings` → `glDrawElementsBaseVertex` |
| **GLES3 实现方案** | 1) 延迟 Link 逻辑与 GL4 一致；2) GLES 3.0 不支持 `glDrawElementsBaseVertex`（需 GLES 3.2 或 `GL_EXT_draw_elements_base_vertex`）。Fallback：当 `baseVertex == 0` 时使用 `glDrawElements`；非零时检测扩展或调整顶点数据 |
| **GLES3 API** | `glLinkProgram`, `glUseProgram`, `glBindBufferBase(GL_UNIFORM_BUFFER)`, `glDrawElements`（3.0）/ `glDrawElementsBaseVertex`（3.2） |
| **关键差异** | `glDrawElementsBaseVertex` 需 GLES 3.2 或扩展支持 |

### 15.3 render (non-indexed)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:2176`） |
| **签名** | `TResult render(uint32_t vertexCount, uint32_t startVertex)` |
| **GL4 实现要点** | 延迟 Program Link → `glDrawArrays` |
| **GLES3 实现方案** | 与 GL4 完全一致：`glDrawArrays(mPrimitiveType, startVertex, vertexCount)` |
| **GLES3 API** | `glDrawArrays` |

### 15.4 renderIndexedInstanced / renderInstanced

> 本组接口在写作本计划时尚未进入 `RHIContext`，属后补条目。

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:2201` / `2259`） |
| **实现要点** | `glDrawElementsInstanced` / `glDrawArraysInstanced`（GLES 3.0 core），实例步进由 `setVertexBuffers` 里的 `glVertexAttribDivisor` 配好 |
| **限制** | `startInstance != 0` 时打错误日志并返回 `T3D_ERR_NOT_IMPLEMENT`。GLES 无 base instance（等价扩展 `GL_EXT_base_instance` 也非 core），**这是无法对齐 D3D11 的硬限制，不是缺陷**；GL4 走 `glDrawElementsInstancedBaseInstance` 可以支持 |

### 15.5 renderIndexedIndirect / renderIndirect

| 项目 | 内容 |
|------|------|
| **状态** | 🚧 契约式不支持（`T3DGLES3Context.cpp:2301` / `2302`）— `T3D_RHI_UNSUPPORTED(supportsIndirectDraw)` |
| **备注** | GLES 3.1 core 有 `glDrawElementsIndirect` / `glDrawArraysIndirect`，可在 3.1 设备上放开，见附录 B.5 |

---

## 16. 状态重置 (Reset)

### 16.1 reset

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:2306`）— 解绑 Program / VAO / VBO / EBO / FBO、删 Program、清 `mPendingUBOs` 都做了；但不复位 `mRenderingToFBO`（与 `resetRenderTarget` 同一个问题），也不清 `mCurrentGeometryShader` / compute 相关状态 |
| **签名** | `TResult reset()` |
| **GL4 实现要点** | 解绑 Program/VAO/VBO/EBO/FBO，删除 Program，清空 `mPendingUBOs` |
| **GLES3 实现方案** | 与 GL4 完全一致 |
| **GLES3 API** | `glUseProgram(0)`, `glBindVertexArray(0)`, `glBindBuffer(... 0)`, `glBindFramebuffer(... 0)`, `glDeleteProgram` |

---

## 17. 数据传输 (Blit & Copy)

### 17.1 blit (RenderTarget → RenderTarget)

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现（`T3DGLES3Context.cpp:2343`）— 空 `return T3D_OK`，静默 |
| **签名** | `TResult blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **GL4 实现要点** | ~~❌ GL4 未实现（返回 T3D_OK）~~ → **已过时**：GL4 现在四个 blit 重载全部实现（`T3DGL4Context.cpp:3577` / `3607` / `3637` / `3667`），统一走 `resolveBlitEndpoint`（`4561` / `4603`）+ `doBlit`（`4656`），D3D11 也四个全实现 |
| **GLES3 实现方案** | 使用 `glBlitFramebuffer`（GLES 3.0+）：1) 绑定 src FBO 到 `GL_READ_FRAMEBUFFER`；2) 绑定 dst FBO 到 `GL_DRAW_FRAMEBUFFER`；3) 调用 `glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter)` |
| **GLES3 API** | `glBindFramebuffer(GL_READ_FRAMEBUFFER)`, `glBindFramebuffer(GL_DRAW_FRAMEBUFFER)`, `glBlitFramebuffer` |
| **注意** | GLES3 `glBlitFramebuffer` 仅支持 `GL_NEAREST` 过滤（当涉及深度/模板时） |

### 17.2 blit (Texture → RenderTarget)

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:2350`）— 四个重载里唯一有实现的一支：`glBlitFramebuffer` + MSAA resolve + Y 翻转 + TBR `glInvalidateFramebuffer` 都有。缺口：不支持 `size == Vector3::ZERO`「整块拷贝」语义、源必须是带 FBO 的渲染纹理（普通纹理不建临时 FBO）、只处理颜色不处理深度 |
| **签名** | `TResult blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **GL4 实现要点** | 支持 MSAA Resolve + Y 翻转 |
| **GLES3 实现方案** | 逻辑与 GL4 一致，通过临时 FBO + `glBlitFramebuffer` 实现。MSAA Resolve 路径使用 Renderbuffer |
| **GLES3 API** | `glBlitFramebuffer` |

### 17.3 blit (RenderTarget → Texture)

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现（`T3DGLES3Context.cpp:2457`）— 空 `return T3D_OK`，静默 |
| **签名** | `TResult blit(RenderTarget *src, Texture *dst, ...)` |
| **GL4 实现要点** | ~~❌ GL4 未实现~~ → **已过时**：GL4 已实现（`T3DGL4Context.cpp:3637`） |
| **GLES3 实现方案** | 通过 `glBlitFramebuffer` 将 src FBO blit 到挂载了 dst texture 的临时 FBO |
| **GLES3 API** | `glBlitFramebuffer` |

### 17.4 blit (Texture → Texture)

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现（`T3DGLES3Context.cpp:2464`）— 空 `return T3D_OK`。**影响面最大的一支**：后处理链路的 `resolveIfMultisampled` 走的就是这个重载，静默成功会让上层拿到未 resolve 的纹理 |
| **签名** | `TResult blit(Texture *src, Texture *dst, ...)` |
| **GL4 实现要点** | ~~❌ GL4 未实现~~ → **已过时**：GL4 已实现（`T3DGL4Context.cpp:3667`） |
| **GLES3 实现方案** | 创建两个临时 FBO 分别挂载 src/dst 纹理，通过 `glBlitFramebuffer` 传输。或使用 `GL_EXT_copy_image` 扩展的 `glCopyImageSubDataEXT`（如可用） |
| **GLES3 API** | `glBlitFramebuffer`，或 `glCopyImageSubDataEXT`（扩展） |

### 17.5 copyBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:2471`）— `glCopyBufferSubData` 主路径可用，但**零参数校验**：`src == dst`、`size == 0`、越界、目标 immutable、资源类型不匹配全部不检查，类型不匹配时直接静默 `return T3D_OK`。GL4 的同名实现（`T3DGL4Context.cpp:3697`）有完整校验，可直接照搬 |
| **签名** | `TResult copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)` |
| **GL4 实现要点** | ~~❌ GL4 未实现~~ → **已过时**：GL4 已实现且校验完备 |
| **GLES3 实现方案** | 使用 `glCopyBufferSubData`（GLES 3.0+ 核心功能）：`glBindBuffer(GL_COPY_READ_BUFFER, src)` + `glBindBuffer(GL_COPY_WRITE_BUFFER, dst)` + `glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOffset, dstOffset, size)` |
| **GLES3 API** | `glBindBuffer(GL_COPY_READ_BUFFER)`, `glBindBuffer(GL_COPY_WRITE_BUFFER)`, `glCopyBufferSubData` |

### 17.6 writeBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成（`T3DGLES3Context.cpp:2560`）— VBO / IBO / UBO 分支正确（按计划用 `glBufferSubData` / `glBufferData`）；**`PixelBuffer2D` 分支是明确 bug**：宽高传 0、格式硬编码 `GL_RGBA` + `GL_UNSIGNED_BYTE`（`2596`-`2600`），等于计划里想修的 GL4 老 bug 被抄了过来。GL4 现已修正为按 `desc` 取宽高格式（`T3DGL4Context.cpp:3931`，纹理分支在 `3962` 起），见附录 B.3#1 |
| **签名** | `TResult writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer)` |
| **GL4 实现要点** | 使用 DSA（`glNamedBufferData`/`glNamedBufferSubData`）更新 VBO/IBO/UBO；纹理用 `glTexSubImage2D` |
| **GLES3 实现方案** | GLES 不支持 DSA（`glNamedBuffer*`）。替代方案：1) Buffer 类型：`glBindBuffer(target, handle)` + `glBufferSubData` 或 `glMapBufferRange` + `glUnmapBuffer`；2) 纹理类型：`glBindTexture` + `glTexSubImage2D`（修复 GL4 的 width/height=0 bug）；3) 注意绑定/解绑不能污染 VAO 的 EBO，需使用 `GL_COPY_WRITE_BUFFER` 作为临时绑定点 |
| **GLES3 API** | `glBindBuffer`, `glBufferSubData` / `glMapBufferRange`, `glTexSubImage2D` |
| **关键差异** | 无 DSA 支持，需显式 bind/unbind |

### 17.7 map / unmap（GPU Readback）

> 本组接口在写作本计划时尚未进入 `RHIContext`，属后补条目。

| 项目 | 内容 |
|------|------|
| **状态** | 🚧 契约式不支持（`T3DGLES3Context.cpp:2539` / `2546` / `2553`）— `T3D_RHI_UNSUPPORTED_VALUE(supportsReadback)` / `T3D_RHI_UNSUPPORTED(supportsReadback)` |
| **签名** | `ReadbackHandle map(RenderBuffer *src, size_t offset, size_t size)` / `ReadbackHandle map(RenderBuffer *src, const ReadbackRegion &region)` / `TResult unmap(ReadbackHandle handle, Buffer &dst)` |
| **基线** | D3D11 用 staging 资源 + `Map/Unmap` 实现；GL4 用 PBO + `glMapBufferRange` + `glFenceSync` 异步实现（`T3DGL4Context.cpp:3770` / `3824` / `3924`，配套 `allocReadbackRequest:4771`、`finishReadback:4831`） |
| **可行性** | `glMapBufferRange`、PBO（`GL_PIXEL_PACK_BUFFER`）、`glFenceSync` **都是 GLES 3.0 core**，纹理 readback 还需 `glReadPixels` 配合。技术上没有阻塞，只是没做，见附录 B.4.4 |

---

## 18. 帧命令 (Frame Commands)

### 18.1 beginRender

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空（`T3DGLES3Context.h:155`，内联 `return T3D_OK`）— 与 GL4 一致 |
| **签名** | `TResult beginRender()` |
| **GLES3 实现方案** | 与 GL4 一致，GLES 即时模式不需要此操作。内联返回 `T3D_OK` |

### 18.2 endRender

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空（`T3DGLES3Context.h:156`） |
| **签名** | `TResult endRender()` |
| **GLES3 实现方案** | 同 beginRender |

### 18.3 beginPass

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空（`T3DGLES3Context.h:158`） |
| **签名** | `TResult beginPass()` |
| **GLES3 实现方案** | 同 beginRender |

### 18.4 endPass

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空（`T3DGLES3Context.h:159`）— 计划里提的 TBR `glInvalidateFramebuffer` 优化尚未在此落地（目前只在 `blit(Tex→RT)` 内部用过） |
| **签名** | `TResult endPass()` |
| **GLES3 实现方案** | 同 beginRender。但可考虑在此处调用 `glInvalidateFramebuffer` 优化 TBR |

---

## 19. 原生上下文接口 (Native Context)

### 19.1 getNativeContext

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:2669`）— 返回 `mEGLContext` |
| **签名** | `void* getNativeContext() const` |
| **GL4 实现要点** | 返回 `HGLRC`（Windows）/ `GLXContext`（Linux） |
| **GLES3 实现方案** | 返回 `mEGLContext`（EGLContext 句柄） |

### 19.2 restoreNativeContext

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:2676`）— `eglMakeCurrent` 回主 surface |
| **签名** | `void restoreNativeContext()` |
| **GL4 实现要点** | `wglMakeCurrent` / `glXMakeCurrent` |
| **GLES3 实现方案** | `eglMakeCurrent(mEGLDisplay, mMainSurface, mMainSurface, mEGLContext)` |
| **GLES3 API** | `eglMakeCurrent` |

---

## 20. GLES3Context 专有接口（非 RHIContext 纯虚接口）

### 20.1 initEGLContext

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3ContextBase.cpp:39`，版本降级在 `createEGLContext:109`）— 按 3.2 → 3.1 → 3.0 依次尝试 `eglCreateContext` |
| **签名** | `TResult initEGLContext()` |
| **功能** | EGL 上下文初始化 + 版本降级策略 |
| **实现方案** | 1) `eglGetDisplay(EGL_DEFAULT_DISPLAY)` + `eglInitialize`；2) `eglChooseConfig`（EGL_RENDERABLE_TYPE=EGL_OPENGL_ES3_BIT, RGBA8, D24S8）；3) 循环尝试 {3,2} → {3,1} → {3,0} 调用 `eglCreateContext`；4) 成功后 `glGetIntegerv(GL_MAJOR_VERSION/GL_MINOR_VERSION)` 确认实际版本；5) 查询扩展字符串 |
| **GLES3 API** | `eglGetDisplay`, `eglInitialize`, `eglChooseConfig`, `eglCreateContext`, `eglMakeCurrent` |

### 20.2 destroyEGLContext

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3ContextBase.cpp:155`） |
| **签名** | `void destroyEGLContext()` |
| **功能** | 销毁 EGL 上下文和 Display |
| **实现方案** | `eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)` → `eglDestroyContext` → `eglTerminate` |
| **GLES3 API** | `eglMakeCurrent`, `eglDestroyContext`, `eglTerminate` |

### 20.3 swapBackBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:144`） |
| **签名** | `TResult swapBackBuffer(GLES3RenderWindow *renderWindow)` |
| **功能** | 交换前后缓冲区 |
| **实现方案** | `eglSwapBuffers(mEGLDisplay, renderWindow->getEGLSurface())` |
| **GLES3 API** | `eglSwapBuffers` |

### 20.4 resizeRenderWindow

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`T3DGLES3Context.cpp:176`） |
| **签名** | `TResult resizeRenderWindow(GLES3RenderWindow *rw, uint32_t w, uint32_t h)` |
| **功能** | 窗口大小变更处理 |
| **实现方案** | 更新 RenderWindow 的 mWidth/mHeight + `glViewport(0, 0, w, h)`。EGL surface 自动跟随 ANativeWindow 大小变化，无需重建 |
| **GLES3 API** | `glViewport` |

### 20.5 resizeRenderTexture / resizeRenderTarget（RHIContext 接口，未 override）

| 项目 | 内容 |
|------|------|
| **状态** | ⛔ 未 override — `T3DGLES3Context.h` 里没有这两个声明，走 `RHIContext` 默认实现返回 `T3D_ERR_NOT_IMPLEMENT` |
| **签名** | `TResult resizeRenderTexture(RenderTexture *rt, uint32_t width, uint32_t height)` / `TResult resizeRenderTarget(RenderTarget *rt, uint32_t width, uint32_t height)`（基类默认实现见 `T3DRHIContext.h:125` / `134`） |
| **基线** | GL4 已实现（`T3DGL4Context.cpp:943` / `978`），做法是 `releaseRenderTextureResources`（`750`）+ `buildRenderTextureResources`（`767`）重建 |
| **影响** | 窗口 / 分辨率变化时离屏 RT 无法跟着重建，后处理链路会一直用旧尺寸。补齐方案见附录 B.4.3 |

---

## 21. 插件系统 (Plugin)

### 21.1 GLES3Plugin

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`Runtime/Source/T3DGLES3Plugin.cpp`） |
| **功能** | 继承 `Plugin`，在 `install()` 中创建 GLES3Renderer 并通过 `T3D_AGENT.addRHIRenderer()` 注册；`uninstall()` 中移除并销毁 |
| **参考** | GL4Plugin 实现 |

### 21.2 GLES3Renderer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`Runtime/Source/T3DGLES3Renderer.cpp`） |
| **功能** | 继承 `RHIRenderer + Singleton`。`init()` 创建 GLES3Context；`destroy()` 销毁 Context。`mName = RHIRenderer::OPENGLES3` |
| **参考** | GL4Renderer 实现 |

### 21.3 PluginDLL 入口

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`Runtime/Source/T3DGLES3PluginDLL.cpp`） |
| **功能** | `extern "C"` 导出 `dllStartPlugin()` / `dllStopPlugin()`，创建/销毁 GLES3Plugin 实例 |
| **产物** | `libGLES3Renderer.so`（Android 共享库） |

---

## 22. CMake 构建系统

### 22.1 OpenGLES3/CMakeLists.txt

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成，但**目录结构与计划不同** |
| **实际结构** | `OpenGLES3/CMakeLists.txt` 只剩一行 `add_subdirectory(Runtime)`；真正的构建脚本在 `OpenGLES3/Runtime/CMakeLists.txt` |
| **实际内容** | `set_project_name(GLES3Renderer)`、`-DGLES3RENDERER_EXPORT`、`set_project_files` 分别收集 `../Base` 与 `Runtime` 的头/源、`add_library(SHARED)`、链接 `GLESv3 EGL android log T3DMath T3DLog T3DUtils T3DPlatform T3DSystem T3DCore rttr_core ${GLSLANG_LIBRARIES}` |

### 22.2 Renderer/CMakeLists.txt 修改

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成（`source/Plugins/Renderer/CMakeLists.txt:140`-`160`） |
| **实际方案** | Android 分支直接 `add_subdirectory(OpenGLES3)` 不做 `find_package`；非 Android 分支仍保留 `find_package(OpenGLES3)`，找不到模拟器 SDK 就跳过构建（对应第 24 章的跨平台长期方案） |

---

## 汇总统计

> **已废弃**：这里原来的计划期统计（📋 35 / ⚡ 20 / 🔇 9）是「要写多少个接口」的工作量估算，不是实现状态。
> 当前实现状态请看 **§0.18 汇总**（✅ 43 / ⚠️ 19 / ❌ 14 / 🚧 14 / 🔇 4 / ⛔ 2，共 96），差距清单看**附录 B**。

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
| 计算着色器 | ~~未实现~~ → **已完整实现**（含独立 program、dispatch、SSBO/UAV） | 仅 `createComputeShader` / `setComputeShader` 有壳子，编译链路断、派发接口全部 🚧 | **结论反转**：现在是 GL4 更完整 |
| 压缩纹理 | BC1-BC7 | ETC2/EAC + ASTC | **暂不适用**：RHI 的 `PixelFormat` 枚举里根本没有压缩格式，两边都走不到这条路径（压缩只存在于 `Image::FileFormat` 层） |
| TBR 优化 | 不需要 | `glInvalidateFramebuffer` | 移动端性能关键 |
| Sampler Border | 核心功能 | GLES 3.2 / 扩展 | 需运行时检测 |
| glslang Profile | `ENoProfile` + 400 | `EEsProfile` + 300/310/320 | 编译参数差异 |

### 实现优先级

> **历史内容**：下表是 2026-07 从零起步时的开发顺序，其中 P0 / P1 / P2 的绝大部分已经落地。
> 现在要做的补齐顺序请看**附录 B.8 建议的执行顺序**。

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
> ~~**当前状态**：现有 `OpenGLES3/CMakeLists.txt` 是占位文件，错误链接了桌面端 OpenGL 库；源码文件均为空壳。~~
>
> **2026-09 实际状态：本章 23.1～23.9 已全部落地**，Android 侧构建链路是通的；各小节状态列已逐条改为实际状态。已验证：
>
> - `Base + Runtime` 两级目录结构（23.1）✅ — 见 §22.1
> - `OpenGLES3/CMakeLists.txt` 转发 + `Runtime/CMakeLists.txt` 承载实际构建（23.2 / 23.3）✅
> - `Renderer/CMakeLists.txt` 的 Android 分支免 `find_package`（23.4）✅ — `CMakeLists.txt:140`-`160`
> - `source/CMake/Packages/FindOpenGLES3.cmake` 存在，供非 Android 平台查模拟器 SDK（23.5）✅
> - `dependencies/glslang` 下已有 `include` + `prebuilt`（23.6）✅
> - `assets/config/Android/Tiny3D.cfg` 已把 renderer 配成 `GLES3Renderer`（23.8）✅
> - `GLES3RENDERER_EXPORT` 宏在 `Runtime/CMakeLists.txt` 中定义（23.9）✅

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
| **状态** | ✅ 已完成 — 现文件就是下面这 5 行 |
| **当前内容** | ~~错误使用 `find_package(OpenGL)` 并链接桌面 `${OPENGL_LIBRARIES}`~~（已重写） |
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
| **状态** | ✅ 已完成（57 行，与下面的方案基本一致；实际另加了 `-DGLES3RENDERER_EXPORT` 与 `android` / `log` 系统库） |
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
| **状态** | ✅ 已完成（现为 `CMakeLists.txt:140`-`160`） |
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
| **状态** | ✅ 已完成（`FindOpenGLES3.cmake:26`-`35` 就是下面这段） |
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
| **状态** | ✅ 已完成 — `dependencies/glslang/prebuilt/Android/` 下 `arm64-v8a` 与 `x86_64` 均已就位 |
| **当前状态** | ~~仅有 `prebuilt/Windows/x64/` 目录~~ |
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
| **状态** | ✅ 已完成（Android 构建产物可正常打进 APK；如后续新增 Sample 需照此配置） |
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
| **状态** | ✅ 已完成（`assets/config/Android/Tiny3D.cfg:56` 插件项与 `89` 的 renderer 项都是 `GLES3Renderer`） |
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
| **状态** | ✅ 已完成，但**落点与计划不同**：宏定义在 `Runtime/Include/T3DGLES3Prerequisites.h:15`-`19`（不是 `Base` 的 `T3DGLES3PrerequisitesBase.h`），`-DGLES3RENDERER_EXPORT` 由 `Runtime/CMakeLists.txt:8` 提供，使用处是 `T3DGLES3PluginDLL.cpp:15` / `21` |

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

> **2026-09 后续修订**：正文的图例与第 1～20 章状态列已统一改用下面这套状态态图例，正文不再有 📋 / ⚡ 计划态标记，本节保留作为口径定义。

本附录使用与 GL4、D3D11 文档一致的状态态图例：

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
| `blit(Tex→RT)` 不把 `size==ZERO` 当成整张拷贝 | T3DGLES3Context.cpp:2385 | 与 GL4 同一写法。`CameraEffectBehaviour::blitCopy` 默认 `ZERO`，Copy 效果 blit 一块 0×0。带明确 size 的上屏 blit 不受影响 |
| `bindPixelBuffers` 不切 MSAA 的 `GLResolveTex` | T3DGLES3Context.cpp:2881 | 2D 永远绑 `GLTexture` + `GL_TEXTURE_2D`。MSAA 路径用了 `glTexStorage2DMultisample`（GLES 3.1+），可采样对象是 `GLResolveTex` |

---

## A.7 全量口径汇总统计（93 个虚接口）

> **已被取代**：本节是 93 接口旧口径，且分类基于「GL4 也没实现」的旧假设。
> 最终口径请看正文 **§0.18** 与本文档 **B.7**（96 接口：✅ 43 / ⚠️ 19 / ❌ 14 / 🚧 14 / 🔇 4 / ⛔ 2）。下表仅作演进记录保留。

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
| Hull/Domain create 与 set 系列 | 静默返回成功 | Tessellation 走 `T3D_RHI_UNSUPPORTED` / 明确错误码。**四个 blit 与 `reflectSamplerBindings` 已收口，不再列在本行** |
| `setComputeShader` | CS attach 到图形 program，链接必然失败 | 改为独立 compute program（补齐 dispatch 时一并处理） |

### P1 — 核心功能

| 项 | 原因 | 实现要点 |
|----|------|---------|
| ~~`blit(RT→RT)` / `blit(RT→Tex)` / `blit(Tex→Tex)`~~ | ✅ **已完成**（2026-09）：四个重载收口到 `resolveBlitEndpoint` + `doBlit`；`size==ZERO` 为整张；MSAA 先 resolve。真机核对见 `PostProcessingApp-Android-Design-todo.md` §5.4 | — |
| ~~`blit(Tex→RT)` 的 `ZERO` 语义~~ | ✅ **已完成**：`doBlit` 按源宽高展开 `ZERO` | — |
| 压缩纹理（ETC2 / EAC / ASTC） | 移动端纹理内存与带宽的前置条件 | `GLES3Mapping` 补格式映射 + `glCompressedTexImage2D` 上传路径 |
| ~~`reflectSamplerBindings`~~ | ✅ **已完成**：走 glslang 反射缓存，不再是空 `T3D_OK` | — |
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
| §0 全接口实现状态速查表 | ✅ **状态的唯一权威来源**（2026-09 逐函数核对代码，带行号） |
| 第 1～19 章（逐接口实现方案） | ✅ 状态列已于 2026-09 全部改写为实际状态并补上实现位置（原计划期标记 📋 / ⚡ 已清除）；另补入 `setScissorRect`、`createPixelBufferCubemap`、结构化缓冲组、Instanced / Indirect、map / unmap 等后补接口小节。「GLES3 适配方案」「关键差异」「GLES3 API」几列仍是规划内容，作参考 |
| 第 20 章（专有接口） | ✅ 四个专有接口全部落地，另补 20.5 记录未 override 的 RT resize |
| 第 21 章（插件系统） | ✅ 全部落地 |
| 第 22、23 章（CMake 集成） | ✅ 清单全部落地；状态列已逐条核对代码并改写，两处与计划不同已标注：构建脚本落在 `Runtime/CMakeLists.txt`、导出宏落在 `Runtime/Include/T3DGLES3Prerequisites.h` |
| 第 24 章（跨平台方案） | 🚩 **仍然完全未开始**，是本后端最大的一块待办，内容依然适用 |
| 汇总统计与实现优先级表 | ⛔ 计划期工作量估算，已作废并加了指向 §0.18 / B.8 的说明 |

---

## A.10 跨文档一致性与回填清单

GLES3 后端的需求分散在多份设计文档中，本节记录它们与本文档的对应关系，以及哪些说法已经与代码脱节。

### A.10.1 GPU Readback：GLES3 已真实现（仅 2D 彩色）

`doc/todo/GPU-Readback-onRender-Design-todo.md` 第一期已落地。RHI 接口是三个纯虚 `map` / `map(region)` / `unmap`。`T3DGLES3Context` 现在是真实现：`supportsReadback = true`（ES 3.0 起 PBO 就有），`map` / `unmap` 走 PBO + `glReadPixels`。限制：**只支持 2D 彩色纹理，深度 / 模板明确不支持**。MSAA 源会先 resolve 再读；`ReadbackRegion.size` 为 0 按整图算。

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

### A.10.5 相机后处理：blit 已收口，剩真机核对

`doc/todo/Camera-PostProcess-Design-todo.md` B1–B5 已合。效果链是后端无关的管线代码。**四个 blit 重载、`size == ZERO`、MSAA resolve / 采样、`reflectSamplerBindings` 已在 GLES3 收口**，与 D3D11 / GL4 对齐。对照总表见该文档 §12。实现分歧与真机核对见 `PostProcessingApp-Android-Design-todo.md` §5.4。

效果链实际打到的 RHI：

| 调用点 | 重载 | GLES3 Runtime |
|--------|------|----------------|
| `resolveIfMultisampled` | `blit(Texture*, Texture*)`，`size` 默认 `ZERO` | ✅ `doBlit`；MSAA 先 resolve |
| `CameraEffectBehaviour::blitCopy` | `blit(Texture*, RenderTarget*)`，`size` 默认 `ZERO` | ✅ `ZERO` 按整张源矩形展开 |
| `drawFullscreen`（灰度 / 反相 / 染色） | `setRenderTarget` + VS/PS + `_MainTex` + `render(3,0)` | ✅ 绘制接口齐。ESSL 由 `PostProcessShaderSources` 在 Android 上选取（`POSTPROCESS_*_GLES`） |
| 无效果 / 链结束后上屏 | `blit(Texture*, RenderTarget*)`，带明确 size | ✅ |

GLES3 相对 GL4 **仍在的约束**（不是缺口，是平台差异）：

- 嵌入 shader 是 `#version 310 es`。MSAA 路径用 `glTexStorage2DMultisample`，也是 GLES 3.1+。**整个 sample 的 GLES shader 都是 3.1 地板**，测试机确认 ES 3.1+ 即可。
- 没有 `glClipControl`。深度停在 [-1,1]。ESSL 全屏 VS 的 UV 与 GLSL 一样是 `y*0.5+0.5`。真机对预设 0 / 1 的上下方向，不要改 shader。
- 真机只有 Android。Desktop EGL / ANGLE（第 24 章）还没做。
- `map` / `unmap` 已真实现（A.10.1），但只支持 2D 彩色，深度 / 模板不行。本轮 PostProcessingApp 仍不建像素断言。

Android 工程与触摸切预设见 `PostProcessingApp-Android-Design-todo.md`。

### A.10.6 需要修正的上游文档

| 文档 | 问题 |
|------|------|
| `doc/Tiny3D-Architecture.md` §3.6 | RHI 后端表写的是 "OpenGL ES **2/3**"、平台仅标「Android / 可选」，未反映 GLES3Renderer 已是 Android 上注册的主渲染器（`assets/config/Android/Tiny3D.cfg:56`）；同表也完全没有 OpenGL 4 |
| `doc/refs/D3D11-vs-OpenGLES3-API-Mapping.md` | 内容质量较好（§16 完整 compute 章节 + 附录 B 版本能力矩阵 + 附录 C 的 TBR/`glInvalidateFramebuffer` 说明），缺的是与 Tiny3D RHI 接口的一一对应，以及 EGL 生命周期与引擎 `RenderWindow` 的集成说明。实现 E4 与第 24 章跨平台方案时，这两块需要自行补齐。四个 blit 的 `ZERO`=整张契约也没有写进映射表 |

---

# 附录 B：对齐 D3D11 / GL4 的差距清单（2026-09）

> 本附录只做一件事：**以 D3D11（功能基线）和 GL4（同族基线）为参照，列出 GLES3 还缺什么、照谁改、改在哪一行**。
> 附录 A（2026-08）盘的是「GLES3 自身实现到什么程度」，本附录盘的是「与两个基线的差」，两者互补。凡与附录 A 冲突之处，以本附录为准。

## B.1 复核前提：基线已经变了

附录 A 写作时 GL4 与 GLES3 缺口高度同构，所以 A.6.5 / A.10.5 里多处用「与 GL4 同一写法」「GL4 同样缺失」来说明某个缺口不急。**这个前提在 2026-09 已经不成立**：`GL4-Renderer-Backend-todo.md` 附录 A 声明 GL4 Window 变体的 96 个接口（93 RHI + 3 readback）已全部落地。

下列「免责说法」全部失效，对应缺口现在是 GLES3 独有：

| 附录 A 的说法 | 现状 |
|--------------|------|
| `writeBuffer` 纹理分支「与 GL4 一并修」 | GL4 已修（`T3DGL4Context.cpp:3964`-`3979` 从描述符取 width/height/格式），GLES3 仍传 0 |
| `blit(Tex→RT)` 的 `ZERO` 语义「与 GL4 同一写法」 | GL4 已在 `doBlit` 统一处理（`T3DGL4Context.cpp:4695`-`4707`） |
| `resizeRenderTexture` / `resizeRenderTarget`「GL4 同样缺失」 | GL4 已 override（`T3DGL4Context.cpp:943` / `978`） |
| `setVertexShader` 缺 nullptr 检查 | GL4 已收口到 `attachGraphicsShader`（`T3DGL4Context.cpp:4455`） |
| 三个 blit 重载 + `reflectSamplerBindings` 静默返回 | GL4 四个 blit 全实现，`reflectSamplerBindings` 已 override（`T3DGL4Context.cpp:2835`） |
| Geometry Shader「GL4 绑定被注释」 | GL4 已启用（`T3DGL4Context.cpp:2433` 起） |
| `copyBuffer`「GL4 未实现」 | GL4 已实现且带完整参数校验（`T3DGL4Context.cpp:3697`） |

**结论：GLES3 现在是三个后端里唯一大面积落后的一个，且大部分缺口在 GL4 有可直接照抄的实现。**

## B.2 对附录 A 的五处修正

复核代码时发现附录 A 有几处判断需要更正：

### B.2.1 「CS 编译链路已通」是错的

附录 A.6.2 把 `createComputeShader` 列为「GLES3 领先于 GL4 的地方」，并在 A.8 P2 写「CS 编译链路已通，是本后端相对 GL4 的先发优势」。

**实际情况**：`GLES3ContextBase::glslangCompileAndReflect` 的 stage 分派只有三支（`T3DGLES3ContextBase.cpp:191`-`201`）：

```cpp
case SHADER_STAGE::kVertex:   glslangStage = EShLangVertex; break;
case SHADER_STAGE::kPixel:    glslangStage = EShLangFragment; break;
case SHADER_STAGE::kGeometry: glslangStage = EShLangGeometry; break;
default: /* unsupported shader stage → T3D_ERR_GLES3_SHADER_REFLECTION */
```

`SHADER_STAGE::kCompute` 走 default 直接报错，所以 compute shader **在 `compileShader` 阶段就断了**，`createComputeShader` 里那段 `glCreateShader(GL_COMPUTE_SHADER)` 拿不到编译好的 ESSL 源码。GL4 的同名函数六个 stage 全覆盖（`T3DGL4Context.cpp:2617`-`2622`），并对 compute 单独用 GLSL 430 版本号（`2640`）。

补 compute 前必须先补这个 switch，且 ESSL 版本号要按 `mGLESMinor` 给到 310/320（现有 `T3DGLES3ContextBase.cpp:213`-`215` 的逻辑本身是对的）。

### B.2.2 压缩纹理不是 GLES3 后端的缺陷

附录 A.6.5 把「压缩纹理格式映射缺失」列为 GLES3 已知功能缺陷，A.8 P1 要求在 `GLES3Mapping` 补 ETC2/ASTC 映射。

**但 `PixelFormat` 枚举里根本没有压缩格式**（`source/Core/Include/Kernel/T3DConstant.h:38`-`60`，10 个颜色格式 + 4 个深度格式，全是非压缩的）。`Image::FileFormat` 有 `kPVRTC` / `kASTC` / `kETC1` / `kETC2`（`T3DImage.h:67`-`74`），但那是**文件格式**，没有对应的 `PixelFormat` 出口。

所以这不是「GLES3Mapping 少写了几个 case」，而是一条跨层立项：`PixelFormat` 扩枚举 → `Image` 解码保留压缩块 → `PixelBuffer2DDesc` 传块尺寸 → 各后端走 `glCompressedTexImage2D` / `CreateTexture2D(BC*)`。**D3D11 和 GL4 也一样不支持**，三个后端齐平。建议从 GLES3 的 P1 里摘出去，单独立项。

同理，`PixelFormat` 没有任何浮点/半浮点格式，HDR 后处理链在三个后端都受同一限制，不是 GLES3 特有问题。

### B.2.3 Hull / Domain 不必永久判死

附录 A.7 的横向对比表把 GLES3 的 Tessellation 标为「❌（GLES 3.2 才有 API）」，A.8 P0 的建议是「补警告日志并返回 `T3D_ERR_NOT_IMPLEMENT`」。

**OpenGL ES 3.2 core 已包含 tessellation control / evaluation shader**（`GL_EXT_tessellation_shader` 提级进核心），与 geometry shader 同一批。既然 `createGeometryShader` 已经用 `mGLESMinor >= 2` 门控实现了（`T3DGLES3Context.cpp:1839`），Hull / Domain 完全可以照同一模式做，实现骨架直接抄 GL4 的 `createHullShader` / `createDomainShader`（`T3DGL4Context.cpp:2349` / `2391`），只需把 `GL_TESS_CONTROL_SHADER` / `GL_TESS_EVALUATION_SHADER` 换成 ES 版本常量，并在 glslang 里补 `EShLangTessControl` / `EShLangTessEvaluation`。

定位建议：从「❌ 永久不支持」改为「⚠️ 需 3.2，与 GS 同级」。

### B.2.4 Readback 不必永久 stub

附录 A.10.1 把 GLES3 的 `map` / `unmap` 定位为「第一期 stub」。技术上：**`glMapBufferRange`、PBO、`glReadPixels`、`glFenceSync` 都是 GLES 3.0 core**，缓冲与纹理两条 readback 路径都能做，GL4 的 `allocReadbackRequest`（`T3DGL4Context.cpp:4771`）+ `map`（`3770`）+ `unmap`（`3924`）就是不依赖 DSA 之外特性的参考实现。移动端唯一要注意的是 `glReadPixels` 的格式限制（只保证 `GL_RGBA`/`GL_UNSIGNED_BYTE` 与 `GL_IMPLEMENTATION_COLOR_READ_FORMAT`）。

所以 `supportsReadback` 保持 false 是「还没做」，不是「做不了」。

### B.2.5 接口口径应为 96

附录 A.7 用 93 个虚接口统计，A.10.1 已注明「需加上 3 个 readback stub」。本附录统一按 **96 = 93 RHI + 3 readback** 计，与 GL4 文档口径一致。

## B.3 P0：与两个基线都不一致的缺陷

这一组都是「基线已有正确写法、GLES3 写错或没写」，改动量小、风险低，且都能直接照抄。

| # | 缺陷 | GLES3 现状 | 基线参考 | 修法 |
|---|------|-----------|---------|------|
| 1 | `writeBuffer` 纹理分支尺寸与格式全错 | `T3DGLES3Context.cpp:2596`-`2600`：`glTexSubImage2D(..., 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, ...)`，宽高传 0，格式硬编码 | `T3DGL4Context.cpp:3962`-`3988`；D3D11 §17.6 | 主线程从 `PixelBuffer2D::getDescriptor()` 取 width/height，经 `GLES3Mapping::get` / `getPixelType` 转格式后按值捕获进 lambda |
| 2 | `setVertexShader` 缺 nullptr 检查 | `T3DGLES3Context.cpp:1650`：直接 `shader->getRHIShader()`，传 nullptr 崩 | `T3DGL4Context.cpp:4455` `attachGraphicsShader` | 抽一个 `attachGraphicsShader(shader, mCurrentXXVariant)` 把 VS/PS/GS（以后加 HS/DS）统一收口，nullptr 走解绑分支 |
| 3 | `resetRenderTarget` 不复位 `mRenderingToFBO` | `T3DGLES3Context.cpp:571`-`588` 只 `glBindFramebuffer(0)` | `T3DGL4Context.cpp:728`-`731` | 加一行 `mRenderingToFBO = false;`。不改则 reset 后 `setViewProjectionTransform` 继续翻 Y、blit 继续按 FBO 朝向算 |
| 4 | `setComputeShader` 把 CS attach 到图形 program | `T3DGLES3Context.cpp:2025`-`2031` 复用 `mCurrentProgram`；且 `shader == nullptr` 与 3.0 设备都静默 `return T3D_OK` | `T3DGL4Context.cpp:2535`（独立 `mCurrentComputeProgram`）+ `2955` `ensureComputeProgramLinked` | 独立 compute program + `mComputeProgramDirty`；GL/ES 规范都不允许 compute 与图形 stage 混链 |
| 5 | `copyBuffer` 零参数校验 | `T3DGLES3Context.cpp:2471`-`2534`：不判 `src == dst`、不判 `size == 0`、不判越界、不判 `kImmutable`、类型不匹配时静默 `break` 返回 `T3D_OK` | `T3DGL4Context.cpp:3697`-`3756`；D3D11 §17.5（用真实 `ByteWidth` 校验） | 照抄 GL4 的六道校验；`size == 0` 语义是「从 srcOffset 到结尾」，当前实现会拷 0 字节 |
| 6 | `bindPixelBuffers` 不解绑、不复位活动单元、不切 MSAA resolve 纹理 | `T3DGLES3Context.cpp:2905`-`2910`：`handle == 0` 直接 `continue`（旧纹理留在单元上），循环结束不 `glActiveTexture(GL_TEXTURE0)`；2D 分支永远绑 `GLTexture` | `T3DGL4Context.cpp:4278`-`4296`（MSAA 走 `GLResolveTex`）、`4321`-`4326`（绑 0 + 复位单元） | 三处一起改。MSAA 那条尤其重要：MSAA 渲染纹理的可采样对象是 `GLResolveTex`，现在采的是多重采样纹理 |
| 7 | `bindSamplers` 不解绑 | `T3DGLES3Context.cpp:2986`-`2992`：`handle == 0` 跳过，不调 `glBindSampler(unit, 0)` | GL4 `unbindTextureUnits`（`T3DGL4Context.cpp:4639`）会清 sampler | 补解绑分支 |
| 8 | 14 个接口静默返回成功 | Hull 5 个（`1811`-`1819`）、Domain 5 个（`1825`-`1833`）、`reflectSamplerBindings`（`T3DGLES3ContextBase.cpp:396`）、`blit(RT→RT)`（`2343`）、`blit(RT→Tex)`（`2457`）、`blit(Tex→Tex)`（`2464`） | D3D11 / GL4 都不静默：能做的做了，不能做的走 `T3D_RHI_UNSUPPORTED` 或明确错误码 | 短期至少 `T3D_LOG_WARNING` + `T3D_ERR_NOT_IMPLEMENT`；长期按 B.4 / B.5 真实现 |

> 第 8 项是整个后端最危险的一类问题：上层拿到 `T3D_OK` 会认为数据已经拷过去了，后处理链、shadow、tessellation 材质都会静默出错，而不是走降级路径。

## B.4 P1：功能缺口（基线有、GLES3 完全没有）

### B.4.1 四个 blit 重载统一收口

现状：只有 `blit(Tex→RT)` 有实现（`T3DGLES3Context.cpp:2350`-`2453`），且

- 不把 `size == Vector3::ZERO` 当整张拷贝（`2387`-`2388` 直接用 `size.x/y`，全零就是 0×0）；
- 源必须是带 FBO 的渲染纹理（`2358`），普通 2D 纹理直接报错；
- 只处理颜色（`GL_COLOR_BUFFER_BIT`），深度 blit 无路径；
- 目标是渲染纹理时只取 attachment 0，无 scratch FBO。

对齐目标（D3D11 §17 已经是这个形态，GL4 照它做过一遍）：

| 组件 | GL4 参考 | GLES3 适配要点 |
|------|---------|--------------|
| `resolveBlitEndpoint(Texture*)` | `T3DGL4Context.cpp:4561` | 同构，`TT_2D` / `TT_RENDER_TEXTURE` 之外明确报错 |
| `resolveBlitEndpoint(RenderTarget*)` | `4603` | 同构（窗口 → FBO 0，否则递归到颜色/深度附件） |
| `doBlit` | `4656`-`4767` | `glBlitFramebuffer` 在 GLES 3.0 就有；scratch FBO 用于「纹理没有自带 FBO」的情况；`ZERO` → 源宽高减 offset；深度走 `GL_DEPTH_BUFFER_BIT \| GL_STENCIL_BUFFER_BIT`；MSAA 源直接以 `src.fbo` 为读源一次 resolve 到目标（**不要**先写 `GLResolveTex` 再拷，会与 texture unit 上的绑定形成 feedback loop） |
| `unbindTextureUnits` | `4639` | GLES3 无 `GL_TEXTURE_1D` / `GL_TEXTURE_2D_MULTISAMPLE`（3.0）需按版本裁剪 target 列表 |
| TBR `glInvalidateFramebuffer` | GL4 无 | GLES3 现有的这一处优化（`2437`-`2439`）要保留，移到 `doBlit` 尾部 |

优先级内部排序（与附录 A.8 一致，理由见 A.10.5）：**先 `Tex→Tex` 与 `Tex→RT` 的 `ZERO` 语义**，再 `RT→Tex` / `RT→RT`。

### B.4.2 `reflectSamplerBindings`

GLES3 转发到基类空实现（`T3DGLES3Context.cpp:2073` → `T3DGLES3ContextBase.cpp:396` `return T3D_OK`）。GL4 在 Window 变体里 override 了真实现（`T3DGL4Context.cpp:2835`-`2900`+）：从 `mReflectionCache` 取 glslang uniform 列表，按 `glDefineType` 筛 sampler，还原 SPIRV-Cross 合并名，填 `ShaderSamplerParams`。

GLES3 的 `reflectShaderAllBindings` 已经有同样的名字还原逻辑，把 sampler 那一段抽出来复用即可。注意 GLES3 的 sampler 类型集合不同：没有 `GL_SAMPLER_1D`（1D 用 height=1 的 2D 模拟），需要额外处理 `GL_SAMPLER_2D_ARRAY` / `GL_SAMPLER_CUBE_SHADOW`。

这一项还直接影响 `bindSamplers` 的 `remapUnit`（`T3DGLES3Context.cpp:2927`-`2958`）——它靠 `getSamplerBinding()` / `getTexBinding()` 反查纹理单元，元数据不全就会绑错单元。

### B.4.3 `resizeRenderTexture` / `resizeRenderTarget`

两个接口在 `RHIContext` 有默认实现（返回 `T3D_ERR_NOT_IMPLEMENT`），GLES3 未 override（`T3DGLES3Context.h` 全文无这两个声明）。D3D11 与 GL4 都实现了，引擎侧统一入口是 `RenderTexture::resize`。

GL4 的做法值得照搬：把 `createRenderTexture` 里的资源构建逻辑抽成 `releaseRenderTextureResources` + `buildRenderTextureResources` 两个 helper（`T3DGL4Context.cpp:750` / `767`），`resizeRenderTexture` 就是「校验 → release → build」（`943`），`resizeRenderTarget` 遍历所有颜色附件与 depthStencil 逐个 `resize`（`978`）。

GLES3 的 `createRenderTexture` 目前是一整个 200 行的 lambda（`T3DGLES3Context.cpp:273`-`450`），MSAA 纹理 + resolve FBO + depth RBO 全在里面，先做这一步重构再加 resize。

### B.4.4 GPU Readback（`map` / `map(region)` / `unmap`）

现状三个 stub（`T3DGLES3Context.cpp:2539` / `2546` / `2553`），`supportsReadback = false`。可做性见 B.2.4。

实现要点（对照 GL4 `4771` / `3770` / `3924`）：

- 入口校验：源必须声明 `kCPURead`，否则拒绝（GL4 在 `allocReadbackRequest` 里做）；
- 线性缓冲：`glBindBuffer(GL_COPY_READ_BUFFER)` + `glMapBufferRange(GL_MAP_READ_BIT)`，或先 `glCopyBufferSubData` 到 staging（`Usage::kCopy` → `GL_STREAM_READ`，`GPU-Readback-onRender-Design-todo.md` §3.1 已指定）；
- 纹理：绑 FBO + `glReadPixels` 到 PBO，格式受 `GL_IMPLEMENTATION_COLOR_READ_FORMAT` 限制；
- 异步：`glFenceSync` + `glClientWaitSync`，GLES 3.0 core。

### B.4.5 BlendState 的独立混合与 AlphaToCoverage（**附录 A 未记录的新发现**）

`GLES3Context::createBlendState` 只读 `desc.RenderTargetStates[0]`（`T3DGLES3Context.cpp:805`），完全忽略 `desc.IndependentBlendEnable` 和 `desc.AlphaToCoverageEnable`；`setBlendState`（`999`-`1030`）相应地只用 `glBlendFuncSeparate` + `glColorMask` 全局版本。

D3D11 原生支持两者；GL4 都做了（`T3DGL4Context.cpp:1297`-`1323` 存 8 个 RT 的状态，`1532`-`1586` 用 `glEnablei` / `glBlendFuncSeparateiARB` / `glColorMaski` / `GL_SAMPLE_ALPHA_TO_COVERAGE` 应用）。

GLES3 侧的可行性：

- `GL_SAMPLE_ALPHA_TO_COVERAGE` 是 **GLES 3.0 core**，可以无条件对齐；
- per-RT 独立混合（`glBlendFuncSeparatei` / `glColorMaski`）是 **GLES 3.2 core**（`GL_OES_draw_buffers_indexed`），按 `mGLESMinor >= 2` 门控，低版本回落到 RT0 并打一次警告。

`setRenderTarget` 已经支持多颜色附件 + `glDrawBuffers`（`T3DGLES3Context.cpp:504`-`520`），所以 MRT 路径上这个缺口是会被踩到的。

### B.4.6 `fillCapabilities` 静态化

GLES3 把六个高级能力位硬编码 false（`T3DGLES3Context.cpp:108`-`113`），注释写的理由是「后端未实现对应 RHI 接口」——作为当下的诚实取值没问题，但缺两件事：

1. **能力位应按版本判定**，像 GL4 那样（`T3DGL4Context.cpp:115`，按 GL major/minor 给 compute / baseInstance / indirect），否则补完 B.5 还得回来重写；
2. **版本号来源不可靠**：`mGLESMajor` / `mGLESMinor` 来自 `createEGLContext` 的降级尝试（3.2 → 3.1 → 3.0），拿到的是「EGL 愿意给的上下文版本」。应在 `initEGLContext` 成功后补 `glGetIntegerv(GL_MAJOR_VERSION / GL_MINOR_VERSION)` 二次确认，避免低估驱动能力而误禁 GS / CS（附录 A.8 P2 已提，此处重申，因为它是 B.5 的前置）。

## B.5 P2：GLES 3.1 / 3.2 门控的能力补齐

| 项 | 前置 | 参考实现 | 备注 |
|----|------|---------|------|
| Compute + SSBO + UAV + `dispatch` / `dispatchIndirect` / `uavBarrier` / `copyStructureCount` | 先修 B.2.1 的 glslang stage switch、B.3#4 的独立 compute program | `T3DGL4Context.cpp:3151`（SSBO 创建）/ `3225`（UAV + atomic counter）/ `3306`（dispatch 带 group count 校验）/ `3351` / `3390` / `3412` | GLES 3.1 core；`glMemoryBarrier` 语义与 GL4 一致。设备黑名单要求见 `RHI-Compute-UAV-Indirect-Draw-Design-todo.md` §12.5 |
| 间接绘制 `renderIndirect` / `renderIndexedIndirect` | 同上 | `T3DGL4Context.cpp:3464` / `3497` + `validateIndirectArgs`（`4523`） | GLES 3.1 core（`glDrawArraysIndirect` / `glDrawElementsIndirect`） |
| Hull / Domain（见 B.2.3） | glslang 补 TessControl / TessEvaluation | `T3DGL4Context.cpp:2349` / `2391` | GLES 3.2 core，按 `mGLESMinor >= 2` 门控 |
| 扩展检测补全 | — | — | `detectExtensions` 只查 BGRA8888（`T3DGLES3Mapping.cpp:24`-`35`）。计划中的 `GL_EXT_texture_border_clamp`、`GL_EXT_texture_filter_anisotropic`、`GL_EXT_draw_elements_base_vertex`、`GL_EXT_copy_image` 全未检测；`createSamplerState` 已经在无条件调 `GL_TEXTURE_MAX_ANISOTROPY_EXT`（`T3DGLES3Context.cpp:963`），无扩展的设备上是一次静默 GL 错误 |

## B.6 已确认「无法对齐」或「不必对齐」的差异

这些不进待办，但要求上层承担或文档标注：

| 差异 | 说明 |
|------|------|
| 非零 `startInstance` | GLES 各版本都没有 `*BaseInstance`。现在返回 `T3D_ERR_NOT_IMPLEMENT`（`T3DGLES3Context.cpp:2209` / `2266`）是正确处理，`supportsBaseInstance = false` 是正确取值 |
| 深度范围 [-1,1] | 无 `glClipControl`。已由 `getDepthRemapMatrix`（`T3DGLES3Context.cpp:229`）+ FBO 翻 Y（`212`-`221`）处理 |
| 1D 纹理 | 无 `GL_TEXTURE_1D`，用 height=1 的 2D 模拟（`1407`-`1420`）。副作用：`reflectSamplerBindings` 不会有 `GL_SAMPLER_1D`，`bindPixelBuffers` 的 1D 分支 target 是 `GL_TEXTURE_2D` |
| 无 DSA | `writeBuffer` / `copyBuffer` 只能 bind/unbind。现有实现借 `GL_COPY_WRITE_BUFFER` 避免污染 VAO 的 EBO 绑定（`2611`-`2655`），这个思路是对的，保留 |
| `desc.mipmaps` / `desc.arraySize` 被忽略 | `createPixelBuffer2D`（`1465`-`1476`）只上传 mip 0 再 `glGenerateMipmap`，纹理数组完全没走。**GL4 一模一样**（`T3DGL4Context.cpp:2003`-`2013`），是 GL 家族共有缺口，不是 GLES3 落后于 GL4；对齐 D3D11（`buildSubresourceData` 逐 slice / 逐 mip）需要两个后端一起改 |
| 压缩格式 / 浮点格式 | 见 B.2.2，`PixelFormat` 枚举层面的缺口，三后端齐平 |
| `DepthStencilDesc::StencilRef` 被忽略 | `setDepthStencilState` 里 `glStencilFuncSeparate` 的 ref 硬编码为 1（`T3DGLES3Context.cpp:1062`-`1063`）。**GL4 一模一样**（`T3DGL4Context.cpp:1627`-`1628`），而 D3D11（`OMSetDepthStencilState` 传 ref）、Vulkan（`T3DVKContext.cpp:2820`）、Metal（`T3DMetalContext.mm:542`）都用了描述符里的真实 ref。这是 GL 家族共有缺口：任何依赖 `StencilOp::kReplace` 写入非 1 值、或 ref 不等于 1 的模板比较都会不对。修法是把 `StencilRef` 存进 `GLES3DepthStencilStateData` 并传给 `glStencilFuncSeparate`，**建议与 GL4 一并修** |
| `PrimitiveType` 映射 | 引擎枚举只有 5 项（`T3DConstant.h:82`-`89`），`GLES3Mapping` 全覆盖，无缺口 |
| 平台只有 Android | `GLES3RenderWindow::init`（`T3DGLES3RenderWindow.cpp:104`）非 Android 分支直接报错（`156`）。跨平台方案见正文第 24 章，仍未开始 |

## B.7 更新后的统计（96 接口口径）

> **2026-09 二次修订**：正文新增的 **§0 全接口实现状态速查表**是逐函数核对代码后的结果，比本节初版更严。相对初版（50 ✅ / 12 ⚠️）又有 7 项从 ✅ 降为 ⚠️：`createSamplerState`（扩展未检测）、`setDepthStencilState`（StencilRef 硬编码）、`createPixelBuffer2D`（忽略 mip / arraySize）、`compileShader`（stage 不全）、`reset`（不复位 FBO 标志）、`copyBuffer`（无校验）、`blit(Tex→RT)`（无 `ZERO` 语义 / 源受限）。下表为最终口径，与 §0.18 一致。

| 状态 | 数量 | 说明 |
|------|------|------|
| ✅ 已完成 | 43 | 变换 / 深度 remap / 渲染纹理 / 渲染目标 / 视口裁剪 / 清除 / 深度模板与光栅状态 / 顶点输入 / VBO·IBO·UBO / 1D·3D·Cubemap 纹理 / VS·PS 全套（除 `setVertexShader`）/ GS·CS 资源绑定 / 全量反射 / 图元与基础绘制 / 原生上下文 |
| ⚠️ 部分完成 | 19 | `createRenderWindow`（仅 Android）、`resetRenderTarget`、`createBlendState`、`setBlendState`、`createSamplerState`、`setDepthStencilState`、`createPixelBuffer2D`、`setVertexShader`、GS 2 个（需 3.2）、CS 2 个（需 3.1 且编译链路断 + program 混用）、`compileShader`、Instanced 2 个（无 base instance）、`reset`、`blit(Tex→RT)`、`copyBuffer`、`writeBuffer` |
| ❌ 未实现（静默成功） | 14 | Hull 5 + Domain 5 + `reflectSamplerBindings` + `blit(RT→RT)` + `blit(RT→Tex)` + `blit(Tex→Tex)` |
| 🚧 契约式不支持 | 14 | StructuredBuffer 4 + UAV 2 + `dispatch` + `dispatchIndirect` + `copyStructureCount` + 间接绘制 2 + readback 3 |
| 🔇 按设计为空 | 4 | `beginRender`、`endRender`、`beginPass`、`endPass` |
| ⛔ 未 override | 2 | `resizeRenderTexture`、`resizeRenderTarget` |
| **合计** | **96** | |

- **有效可用**（✅ + 🚧 + 🔇）：**61 / 96 ≈ 64%**
- **纯 ✅ 完成度**：**43 / 96 ≈ 45%**
- 19 个 ⚠️ 里有 6 项属于「能用，只是与基线有行为差异或受设备版本门控」（`createRenderWindow`、`createPixelBuffer2D`、GS 2 个、Instanced 2 个），剩下 13 项是需要修的缺陷。

### 三后端对比（96 接口口径）

| 维度 | D3D11 | GL4 Window | GLES3 Runtime |
|------|-------|-----------|--------------|
| ✅ 已完成 | 89 | 92 | 43 |
| ⚠️ 部分完成 | 0 | 0 | 19 |
| ❌ 未实现（静默） | 0 | 0 | 14 |
| 🚧 契约式不支持 | 0 | 4（GPU-driven 4 项） | 14 |
| 🔇 按设计为空 | 7 | 4 | 4 |
| ⛔ 未 override | 0 | 0 | 2 |
| 四个 blit | ✅ 统一收口 | ✅ 统一收口 | ⚠️ 仅 `Tex→RT` 且无 `ZERO` 语义 |
| `copyBuffer` | ✅ 带校验 | ✅ 带校验 | ⚠️ 无校验 |
| Readback | ✅ | ✅ | 🚧 |
| Compute / UAV / Indirect | ✅ | ✅ | 🚧（编译链路亦断） |
| Tessellation | ✅ | ✅ | ❌（3.2 可做） |
| Geometry Shader | ✅ | ✅ | ⚠️ 需 3.2 |
| 独立 RT 混合 / A2C | ✅ / ✅ | ✅ / ✅ | ❌ / ❌ |
| `reflectSamplerBindings` | ✅ | ✅ | ❌ |
| RT resize | ✅ | ✅ | ⛔ |
| 平台覆盖 | Windows | Windows / Linux | 仅 Android |

> D3D11 的 89 / 7 是在其文档「78 接口、71 ✅ / 7 🔇」旧口径上，补入 compute（`T3DD3D11Context.cpp:2109` / `3490` / `3549` / `3578` / `3599` / `3648`）、instanced 与 indirect（`3786` / `3805` / `3823` / `3843`）、readback（`4477` / `4552` / `4743`）实际已落地的接口后推算的；D3D11 文档的汇总统计尚未更新到 96 口径，建议同步。
> GL4 的 4 项 🚧 指其文档 A.4 记录的 GPU-driven 接口 stub；其余按 GL4 文档 A.7「96 接口 100% 完成」计。

## B.8 建议的执行顺序

> 本节只给顺序与理由。每一步的具体实现方案（代码骨架、版本门控、验证点、提交拆分）见 `doc/todo/GLES3-Backend-Alignment-Implementation-todo.md`，两份文档的阶段划分一一对应。

| 阶段 | 内容 | 状态（2026-09-09） |
|------|------|------|
| 第 1 步 | B.3 全部 8 项（缺陷修复） | ✅ 已落地 |
| 第 2 步 | B.4.1 四个 blit + B.4.2 `reflectSamplerBindings` | ✅ 已落地 |
| 第 3 步 | B.4.5 混合状态 + B.4.3 RT resize | ✅ 已落地 |
| 第 4 步 | B.4.6 能力位动态化 + B.2.1 glslang stage 补全 | ✅ 已落地 |
| 第 5 步 | B.5 Compute / Indirect / Tessellation | ✅ Compute / SSBO / UAV / Indirect 已落地；Tessellation 完成 shader 侧，绘制侧待引擎 `PrimitiveType` 扩 patch |
| 第 6 步 | B.4.4 Readback | ✅ 已落地（深度 / Cubemap / 压缩明确不支持） |
| 并行 | 正文第 24 章 Desktop EGL 变体 | ⏸ 未做 |

> 压缩纹理（B.2.2）与 mip / 纹理数组上传（B.6）建议从本后端待办摘出，作为跨后端的 RHI 层立项。
