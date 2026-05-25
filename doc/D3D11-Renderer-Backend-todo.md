# D3D11 Context API 实现状态清单

本文档列出 `D3D11Context`（Window 版本）所有 RHI 接口方法的实现状态，**精确到方法内部每个条件分支**，标注已完整实现、部分实现（有分支缺失）、桩函数及注释掉的功能。

> 基于源文件：
> - `source/Plugins/Renderer/Direct3D11/Base/Source/T3DD3D11ContextBase.cpp`
> - `source/Plugins/Renderer/Direct3D11/Window/Source/T3DD3D11Context.cpp`（3547 行）

---

## 统计总览

| 状态 | 数量 | 说明 |
|------|------|------|
| ✅ 完整实现 | 65 | 所有分支均含真实 D3D11 调用 |
| 🟡 部分实现（有空分支） | 8 | 方法整体可用，但某些 switch/if 分支为空 |
| ⬜ 空操作（设计如此） | 4 | D3D11 无需帧/Pass 标记 |
| ❌ 整体桩函数 | 7 | 仅 `return T3D_OK` 或 `return nullptr` |
| ⚠️ 已注释的功能块 | 5 | 完整代码被注释掉 |

---

## 一、已完整实现的 API（所有分支均完整）

### 1.1 初始化与生命周期

| 方法 | D3D11 调用 |
|------|-----------|
| ✅ `create()` | 工厂方法 |
| ✅ `init()` | `D3D11CreateDevice` |
| ✅ `~D3D11Context()` | `ClearState` + `Flush` + 释放全部 COM |

### 1.2 渲染窗口 / 渲染目标

| 方法 | D3D11 调用 |
|------|-----------|
| ✅ `createRenderWindow()` | `CreateSwapChain` + `CreateRenderTargetView` |
| ✅ `createRenderTexture()` | `CreateTexture2D` + SRV/RTV/DSV（Color RT 和 DS RT 全路径） |
| ✅ `resizeRenderWindow()` | `ResizeBuffers` + 重建 RTV |
| ✅ `setRenderTarget(RenderTarget*)` | 按 Window/Texture 分发 |
| ✅ `setRenderTarget(Window, DS)` | `OMSetRenderTargets` |
| ✅ `setRenderTarget(Texture[], n, DS)` | `OMSetRenderTargets`（多 MRT） |
| ✅ `resetRenderTarget()` | `OMSetRenderTargets(0, nullptr, nullptr)` |
| ✅ `swapBackBuffer()` | `SwapChain::Present` |

### 1.3 视口与清除

| 方法 | D3D11 调用 |
|------|-----------|
| ✅ `setViewport()` | `RSSetViewports` |
| ✅ `clearColor()` | `ClearRenderTargetView`（Window + 多 RT 纹理） |
| ✅ `clearDepth()` | `ClearDepthStencilView(D3D11_CLEAR_DEPTH)` |
| ✅ `clearDepthStencil()` | `ClearDepthStencilView(DEPTH\|STENCIL)` |

### 1.4 渲染状态创建与设置

| 方法 | D3D11 调用 |
|------|-----------|
| ✅ `createBlendState()` | `CreateBlendState` |
| ✅ `createDepthStencilState()` | `CreateDepthStencilState` |
| ✅ `createRasterizerState()` | `CreateRasterizerState` |
| ✅ `createSamplerState()` | `CreateSamplerState` |
| ✅ `setBlendState()` | `OMSetBlendState` |
| ✅ `setDepthStencilState()` | `OMSetDepthStencilState` |
| ✅ `setRasterizerState()` | `RSSetState` |

### 1.5 顶点格式与缓冲区

| 方法 | D3D11 调用 |
|------|-----------|
| ✅ `createVertexDeclaration()` | `CreateInputLayout` |
| ✅ `setVertexDeclaration()` | `IASetInputLayout` |
| ✅ `createVertexBuffer()` | `CreateBuffer(BIND_VERTEX_BUFFER)` |
| ✅ `setVertexBuffers()` | `IASetVertexBuffers` |
| ✅ `createIndexBuffer()` | `CreateBuffer(BIND_INDEX_BUFFER)` |
| ✅ `setIndexBuffer()` | `IASetIndexBuffer` |
| ✅ `createConstantBuffer()` | `CreateBuffer(BIND_CONSTANT_BUFFER)` |
| ✅ `createPixelBuffer2D()` | `CreateTexture2D` + `CreateShaderResourceView` |

### 1.6 着色器创建与设置

| 方法 | D3D11 调用 |
|------|-----------|
| ✅ `createVertexShader()` | `CreateVertexShader` |
| ✅ `createPixelShader()` | `CreatePixelShader` |
| ✅ `createHullShader()` | `CreateHullShader` |
| ✅ `createDomainShader()` | `CreateDomainShader` |
| ✅ `createGeometryShader()` | `CreateGeometryShader` |
| ✅ `createComputeShader()` | `CreateComputeShader` |
| ✅ `setVertexShader()` | `VSSetShader` |
| ✅ `setPixelShader()` | `PSSetShader`（支持 nullptr） |
| ✅ `setHullShader()` | `HSSetShader`（支持 nullptr） |
| ✅ `setDomainShader()` | `DSSetShader`（支持 nullptr） |
| ✅ `setGeometryShader()` | `GSSetShader`（支持 nullptr） |
| ✅ `setComputeShader()` | `CSSetShader` |

### 1.7 着色器资源绑定（18 个方法，全部完整）

| 类别 | 方法 |
|------|------|
| ConstantBuffers | ✅ `setVS/PS/HS/DS/GS/CSConstantBuffers()` |
| ShaderResources | ✅ `setVS/PS/HS/DS/GS/CSPixelBuffers()` |
| Samplers | ✅ `setVS/PS/HS/DS/GS/CSSamplers()` |

### 1.8 Shader 编译与反射

| 方法 | D3D11 调用 |
|------|-----------|
| ✅ `setViewProjectionTransform()` | 矩阵转换 |
| ✅ `compileShader()` | `D3DCompile` |
| ✅ `reflectShaderAllBindings()` | `D3DReflect`（cbuffer + texture + sampler） |
| ✅ `reflectSamplerBindings()` | `D3DReflect`（仅 texture + sampler 更新） |

### 1.9 绘制与状态管理

| 方法 | D3D11 调用 |
|------|-----------|
| ✅ `setPrimitiveType()` | `IASetPrimitiveTopology` |
| ✅ `render(indexCount, startIndex, baseVertex)` | `DrawIndexed` |
| ✅ `render(vertexCount, startVertex)` | `Draw` |
| ✅ `backupRenderState()` | 全量 Get 备份 |
| ✅ `reset()` | 全量 Set 还原 |
| ✅ `writeBuffer()` | `Map` / `Unmap` |
| ✅ `blitAll()` | `CopyResource` |
| ✅ `blitRegion()` | 全屏 quad 渲染 |
| ✅ `setupBlitQuad()` | 编译 blit shader + 创建资源 |

---

## 二、部分实现的 API（有空分支）

### 2.1 `blit(Texture *src, RenderTarget *dst, ...)` — 第 3098-3238 行

方法整体可用，但 `switch (src->getTextureType())` 内有多个空分支：

| 纹理类型分支 | 状态 | 说明 |
|-------------|------|------|
| `TT_2D` | ✅ 已实现 | 获取 D3DTexture + D3DSRView |
| `TT_RENDER_TEXTURE` | ✅ 已实现 | 含 MSAA `ResolveSubresource` 处理 |
| `TT_1D` | ❌ **空 break** | 无任何逻辑，pD3DSrc/pD3DSRV 保持 nullptr |
| `TT_2D_ARRAY` | ❌ **空 break** | 同上 |
| `TT_3D` | ❌ **空 break** | 同上 |
| `TT_CUBE` | ❌ **空 break** | 同上 |
| `TT_CUBE_ARRAY` | ❌ **空 break** | 同上 |

> **风险**: 传入 TT_1D/TT_3D/TT_CUBE 等类型时，`pD3DSrc` 为 nullptr，后续 `blitAll()` / `blitRegion()` 会崩溃或产生未定义行为。
>
> 此 switch 出现**两次**（dst 为 Window 时一次在第 3114-3151 行，dst 为 Texture 时一次在第 3180-3217 行），两处的空分支完全相同。

### 2.2 `setPixelBuffers()` 通用方法 — 第 3487-3522 行

`switch (buffer->getRHIResource()->getResourceType())` 内的分支：

| 资源类型分支 | 状态 | 说明 |
|-------------|------|------|
| `kPixelBuffer1D` | 🟡 **可编译但运行时空** | 访问 `D3D11PixelBuffer1D::D3DSRView`，但 `createPixelBuffer1D` 从未创建 SRV |
| `kPixelBuffer2D` | ✅ 已实现 | — |
| `kPixelBuffer3D` | 🟡 **可编译但运行时空** | `createPixelBuffer3D` 返回 nullptr，不会走到这里 |
| `kPixelBufferCubemap` | ✅ 已实现 | 委托到 `D3D11PixelBuffer2D` |

### 2.3 `writeBuffer()` — 第 3263-3355 行

`switch (renderBuffer->getRHIResource()->getResourceType())` 的分支覆盖情况：

| 资源类型分支 | 状态 | 说明 |
|-------------|------|------|
| `kVertexBuffer` | ✅ 已实现 | → `D3D11VertexBuffer::D3DBuffer` |
| `kIndexBuffer` | ✅ 已实现 | → `D3D11IndexBuffer::D3DBuffer` |
| `kConstantBuffer` | ✅ 已实现 | → `D3D11ConstantBuffer::D3DBuffer` |
| `kPixelBuffer2D` | ✅ 已实现 | → `D3D11PixelBuffer2D::D3DTexture` |
| `kPixelBufferCubemap` | ✅ 已实现 | → `D3D11PixelBuffer2D::D3DTexture` |
| `kPixelBuffer1D` | 🟡 **可编译但运行时空** | 访问 `D3D11PixelBuffer1D::D3DTexture`，但 create 未初始化真实纹理 |
| `kPixelBuffer3D` | 🟡 **可编译但运行时空** | `createPixelBuffer3D` 返回 nullptr |

### 2.4 `reflectShaderAllBindings()` — 第 2514-2770 行

`switch (bindDesc.Type)` 的分支：

| 资源类型 | 状态 | 说明 |
|---------|------|------|
| `D3D_SIT_CBUFFER` | ✅ 已实现 | 遍历变量，创建 `ShaderConstantParam` |
| `D3D_SIT_TEXTURE` | ✅ 已实现 | 创建/更新 `ShaderSamplerParam`，设置 texBinding |
| `D3D_SIT_SAMPLER` | ✅ 已实现 | 设置 samplerBinding |
| `D3D_SIT_TBUFFER` | ❌ **无 case** | Texture Buffer 未处理 |
| `D3D_SIT_UAV_*` | ❌ **无 case** | UAV（Compute Shader 读写）未处理 |
| `D3D_SIT_STRUCTURED` | ❌ **无 case** | StructuredBuffer 未处理 |
| `D3D_SIT_BYTEADDRESS` | ❌ **无 case** | ByteAddressBuffer 未处理 |

> **影响**: Compute Shader 使用 UAV / StructuredBuffer / ByteAddressBuffer 时，反射不会返回这些绑定信息。

### 2.5 `setConstantBuffer()` 单缓冲区版本 — 第 409-437 行

```cpp
mD3DDeviceContext->VSSetConstantBuffers(startSlot, 1, &pD3DBuffer);
```

> **问题**: 固定调用 `VSSetConstantBuffers`，无法用于 PS/GS/HS/DS/CS 阶段。只在内部特定场景使用，但如果误用会绑定到错误的着色器阶段。

---

## 三、空操作 API（设计如此，非缺陷）

| 方法 | 说明 |
|------|------|
| ⬜ `beginRender()` | D3D11 无需帧起始标记 |
| ⬜ `endRender()` | D3D11 无需帧结束标记 |
| ⬜ `beginPass()` | D3D11 无需 Pass 标记 |
| ⬜ `endPass()` | D3D11 无需 Pass 标记 |

---

## 四、整体桩函数 API（完全未实现）

| 方法 | 行号 | 当前行为 | 建议实现 |
|------|------|---------|---------|
| ❌ `createPixelBuffer1D()` | 1905-1909 | 创建空壳 `D3D11PixelBuffer1D` 但无 D3D 资源 | `CreateTexture1D` + `CreateShaderResourceView` |
| ❌ `createPixelBuffer3D()` | 2000-2003 | `return nullptr` | `CreateTexture3D` + `CreateShaderResourceView` |
| ❌ `resizeRenderTexture()` | 949-952 | `return T3D_OK` | 释放旧资源，重新 `CreateTexture2D` + SRV/RTV/DSV |
| ❌ `resizeRenderTarget()` | 956-959 | `return T3D_OK` | 按 RT 类型分发到 Window / Texture resize |
| ❌ `blit(RT→RT)` | 3091-3094 | `return T3D_OK` | `CopyResource` 或 `ResolveSubresource` |
| ❌ `blit(RT→Texture)` | 3242-3245 | `return T3D_OK` | Staging Texture 读回 |
| ❌ `blit(Texture→Texture)` | 3249-3252 | `return T3D_OK` | `CopyResource` / `CopySubresourceRegion` |
| ❌ `copyBuffer()` | 3256-3259 | `return T3D_OK` | `CopySubresourceRegion` 或 Map+memcpy+Unmap |

---

## 五、已注释 / 残留代码

| 位置 | 行号 | 说明 |
|------|------|------|
| ⚠️ `setupInternalCBuffers()` | 163-199 | 整个方法体为空 — 内部 per-frame (`Tiny3DPerFrame`) 和 per-draw (`Tiny3DPerDraw`) cbuffer 的 `CreateBuffer` 逻辑全部注释掉 |
| ⚠️ `createRenderWindow()` 内部 | 874-905 | DepthStencilView 创建逻辑（`CreateTexture2D` + `CreateDepthStencilView`）已注释 — 深度由外部 RenderTexture 管理 |
| ⚠️ `setWorldTransform()` | 441-449 | 整个方法被注释掉，依赖已注释的 `mPerDrawCBuffer` |
| ⚠️ `reflectShaderAllBindings()` 内部 | 2596-2602 | `Tiny3DPerDraw` / `Tiny3DPerFrame` 内建 cbuffer 的跳过逻辑被注释 |
| ⚠️ `backupRenderState()` 旧版 | 965-982 | 旧的简易备份代码被注释，已替换为新的全量备份（无需处理） |

---

## 六、完整的未实现 / 空分支汇总表

按函数名 + 具体空分支列出，作为开发 TODO 清单：

### 整体未实现

| # | 函数 | 行号 | 状态 |
|---|------|------|------|
| 1 | `createPixelBuffer1D()` | 1905 | 无 `CreateTexture1D` |
| 2 | `createPixelBuffer3D()` | 2000 | `return nullptr` |
| 3 | `resizeRenderTexture()` | 949 | `return T3D_OK` |
| 4 | `resizeRenderTarget()` | 956 | `return T3D_OK` |
| 5 | `blit(RenderTarget→RenderTarget)` | 3091 | `return T3D_OK` |
| 6 | `blit(RenderTarget→Texture)` | 3242 | `return T3D_OK` |
| 7 | `blit(Texture→Texture)` | 3249 | `return T3D_OK` |
| 8 | `copyBuffer()` | 3256 | `return T3D_OK` |

### `blit(Texture→RenderTarget)` 内部空分支

| # | 函数 | 空分支 | 行号 |
|---|------|--------|------|
| 9 | `blit(Tex→RT)` dst=Window | `case TT_1D: break;` | 3116-3117 |
| 10 | `blit(Tex→RT)` dst=Window | `case TT_2D_ARRAY: break;` | 3126-3127 |
| 11 | `blit(Tex→RT)` dst=Window | `case TT_3D: break;` | 3128-3129 |
| 12 | `blit(Tex→RT)` dst=Window | `case TT_CUBE: break;` | 3130-3131 |
| 13 | `blit(Tex→RT)` dst=Window | `case TT_CUBE_ARRAY: break;` | 3132-3133 |
| 14 | `blit(Tex→RT)` dst=Texture | `case TT_1D: break;` | 3182-3183 |
| 15 | `blit(Tex→RT)` dst=Texture | `case TT_2D_ARRAY: break;` | 3192-3193 |
| 16 | `blit(Tex→RT)` dst=Texture | `case TT_3D: break;` | 3194-3195 |
| 17 | `blit(Tex→RT)` dst=Texture | `case TT_CUBE: break;` | 3196-3197 |
| 18 | `blit(Tex→RT)` dst=Texture | `case TT_CUBE_ARRAY: break;` | 3198-3199 |

### Shader 反射缺失的资源类型

| # | 函数 | 缺失的 case | 说明 |
|---|------|------------|------|
| 19 | `reflectShaderAllBindings()` | `D3D_SIT_TBUFFER` | Texture Buffer |
| 20 | `reflectShaderAllBindings()` | `D3D_SIT_UAV_RWTYPED` | RW Texture（CS 读写） |
| 21 | `reflectShaderAllBindings()` | `D3D_SIT_STRUCTURED` | StructuredBuffer |
| 22 | `reflectShaderAllBindings()` | `D3D_SIT_UAV_RWSTRUCTURED` | RWStructuredBuffer |
| 23 | `reflectShaderAllBindings()` | `D3D_SIT_BYTEADDRESS` | ByteAddressBuffer |
| 24 | `reflectShaderAllBindings()` | `D3D_SIT_UAV_RWBYTEADDRESS` | RWByteAddressBuffer |
| 25 | `reflectShaderAllBindings()` | `D3D_SIT_UAV_APPEND_STRUCTURED` | AppendStructuredBuffer |
| 26 | `reflectShaderAllBindings()` | `D3D_SIT_UAV_CONSUME_STRUCTURED` | ConsumeStructuredBuffer |

### 因上游未实现导致的间接空分支

| # | 函数 | 分支 | 说明 |
|---|------|------|------|
| 27 | `setPixelBuffers()` | `kPixelBuffer1D` | 编译通过，但运行时 SRV 为 nullptr（上游 create 未初始化） |
| 28 | `setPixelBuffers()` | `kPixelBuffer3D` | create 返回 nullptr，不会走到，但逻辑同样为空 |
| 29 | `writeBuffer()` | `kPixelBuffer1D` | 编译通过，但 D3DTexture 为 nullptr |
| 30 | `writeBuffer()` | `kPixelBuffer3D` | 同上 |

### 已注释的功能块

| # | 位置 | 行号 | 说明 |
|---|------|------|------|
| 31 | `setupInternalCBuffers()` | 163-199 | 内部 per-frame/per-draw cbuffer 创建 |
| 32 | `createRenderWindow()` 内 DSV | 874-905 | DepthStencil 创建逻辑 |
| 33 | `setWorldTransform()` | 441-449 | 整个方法 |
| 34 | `reflectShaderAllBindings()` 内部 | 2596-2602 | 内建 cbuffer 跳过逻辑 |
| 35 | `reset()` 旧版全解绑逻辑 | 2898-2987 | 已被新的全量还原替代（无需处理） |

---

## 七、优先级建议

### P0 — 影响当前渲染流程

| # | TODO | 影响 |
|---|------|------|
| 3 | `resizeRenderTexture()` | 编辑器视口 resize / 动态分辨率 |
| 4 | `resizeRenderTarget()` | 同上 |
| 5 | `blit(RT→RT)` | 后处理管线 |
| 9-18 | `blit(Tex→RT)` 空分支 | 传入非 TT_2D/TT_RENDER_TEXTURE 纹理时崩溃 |

### P1 — Compute Shader / 高级渲染

| # | TODO | 影响 |
|---|------|------|
| 19-26 | `reflectShaderAllBindings()` UAV/Structured | CS 反射 |
| 8 | `copyBuffer()` | GPU Buffer 间拷贝 |
| 6 | `blit(RT→Texture)` | 截屏、RT 读回 |
| 7 | `blit(Texture→Texture)` | 纹理拷贝 |

### P2 — 扩展纹理类型

| # | TODO | 影响 |
|---|------|------|
| 1 | `createPixelBuffer1D()` | 1D LUT / 渐变贴图 |
| 2 | `createPixelBuffer3D()` | 体积雾 / 体素 |
| 27-30 | 间接空分支 | 随上游 create 实现后自动生效 |

### P3 — 清理

| # | TODO | 影响 |
|---|------|------|
| 31-34 | 注释代码清理或恢复 | 代码整洁度 |

---

## 八、D3D11ConsoleContext 参考

Console 版本为编辑器后台/离屏处理的**纯桩实现**：

| 状态 | 数量 |
|------|------|
| 有真实逻辑 | 2（`create()` + `createRenderWindow()`） |
| 返回 nullptr 的桩 | 18（所有 createXxx） |
| 返回 T3D_OK 的桩 | ~45（所有 set/render/blit/copy） |

Console 版本设计上不需要真实 GPU 操作，当前状态符合预期。
