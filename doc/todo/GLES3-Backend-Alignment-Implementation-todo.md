# GLES3 后端对齐实现方案

> 配套文档：`doc/todo/GLES3-Renderer-Backend-todo.md`（现状复核，含 §0 全接口速查表与附录 B 差距清单）
>
> 分工：**那份文档回答「差什么」，本文档回答「怎么补」**。本文只覆盖需要改动的项，已完成（✅）且无行为差异的接口不再重复。
>
> 基线：D3D11（`T3DD3D11Context.cpp`）与 GL4（`source/Plugins/Renderer/OpenGL4/Window/Source/T3DGL4Context.cpp`），后者是同族参考，绝大多数项可以「照抄 + 按 GLES 版本裁剪」。
>
> 编写日期：2026-09。文中所有行号对应写作时的代码状态，改动落地后需回填。
>
> **落地状态（2026-09-09）**：P0～P3 与可选 P2-5（Hull / Domain shader 侧）已按本文实现。能力位在 `confirmDeviceVersion()`（`eglMakeCurrent` 后）按 GLES 3.1 / 3.2 动态打开。GLES 间接绘制第 4 字段必须为 0；深度 / 模板 / Cubemap / 压缩纹理回读仍明确报 `T3D_ERR_GLES3_UNSUPPORTED_OPERATION`。

---

## 目录

- [1. 工作项总览](#1-工作项总览)
- [2. 实现前置约定](#2-实现前置约定)
- [3. 阶段一：P0 缺陷修复](#3-阶段一p0-缺陷修复)
- [4. 阶段二：blit 统一收口与采样器反射](#4-阶段二blit-统一收口与采样器反射)
- [5. 阶段三：混合状态与 RT resize](#5-阶段三混合状态与-rt-resize)
- [6. 阶段四：能力位动态化与 glslang stage 补全](#6-阶段四能力位动态化与-glslang-stage-补全)
- [7. 阶段五：Compute / SSBO / UAV / Indirect](#7-阶段五compute--ssbo--uav--indirect)
- [8. 阶段六：GPU Readback](#8-阶段六gpu-readback)
- [9. 阶段七：扩展检测与采样器修正](#9-阶段七扩展检测与采样器修正)
- [10. 文件改动清单](#10-文件改动清单)
- [11. 验证方案](#11-验证方案)
- [12. 风险与回退](#12-风险与回退)
- [13. 提交拆分建议](#13-提交拆分建议)

---

## 1. 工作项总览

| ID | 工作项 | 类别 | 主要落点 | 最低 GLES 版本 | 依赖 | 阶段 |
|----|--------|------|---------|--------------|------|------|
| P0-1 | `writeBuffer` 纹理分支尺寸/格式 | 缺陷 | `T3DGLES3Context.cpp:2586`-`2609` | 3.0 | — | 一 |
| P0-2 | `attachGraphicsShader` 收口（nullptr 保护） | 缺陷 | `1650` / `1752` / `1897` | 3.0 | — | 一 |
| P0-3 | `resetRenderTarget` 复位 `mRenderingToFBO` | 缺陷 | `571`-`588` | 3.0 | — | 一 |
| P0-4 | 独立 compute program | 缺陷 | `2008`-`2038` | 3.1 | — | 一 |
| P0-5 | `copyBuffer` 六道校验 | 缺陷 | `2471`-`2535` | 3.0 | — | 一 |
| P0-6 | `bindPixelBuffers` 解绑 / 复位单元 / MSAA resolve | 缺陷 | `2854`-`2918` | 3.0 | — | 一 |
| P0-7 | `bindSamplers` 解绑 | 缺陷 | `2922`-`3004` | 3.0 | — | 一 |
| P0-8 | 静默成功接口收口 | 缺陷 | Hull `1811`-`1819`、Domain `1825`-`1833`、blit `2343`/`2457`/`2464` | 3.0 | — | 一 |
| P0-9 | `StencilRef` 硬编码（GL 家族共有） | 缺陷 | `1062`-`1063` + GL4 `1627`-`1628` | 3.0 | — | 一 |
| P1-1 | 四个 `blit` 重载统一收口 | 功能 | 新增 `resolveBlitEndpoint` / `doBlit` | 3.0（MSAA 路径 3.1） | P0-6 | 二 |
| P1-2 | `reflectSamplerBindings` | 功能 | `T3DGLES3ContextBase.cpp:396` | 3.0 | — | 二 |
| P1-3 | BlendState 独立混合 + A2C | 功能 | `800`-`839` / `999`-`1030` | A2C 3.0；per-RT 3.2 | — | 三 |
| P1-4 | `resizeRenderTexture` / `resizeRenderTarget` | 功能 | `createRenderTexture` 拆 helper | 3.0 | — | 三 |
| P1-5 | `fillCapabilities` 动态化 + 版本二次确认 | 功能 | `101`-`140` / `createEGLContext` | 3.0 | — | 四 |
| P1-6 | glslang stage 补全（Compute / Tess） | 功能 | `T3DGLES3ContextBase.cpp:191`-`201` | 3.1 / 3.2 | P1-5 | 四 |
| P2-1 | StructuredBuffer（SSBO） | 能力 | 新增 `GLES3StructuredBuffer` | 3.1 | P1-5、P1-6 | 五 |
| P2-2 | UAV + atomic counter | 能力 | `setCSUnorderedAccessBuffers` | 3.1 | P2-1 | 五 |
| P2-3 | `dispatch` / `dispatchIndirect` / `uavBarrier` / `copyStructureCount` | 能力 | `2297`-`2300` | 3.1 | P0-4、P2-1 | 五 |
| P2-4 | `renderIndirect` / `renderIndexedIndirect` | 能力 | `2301`-`2302` | 3.1 | P1-5 | 五 |
| P2-5 | Hull / Domain（Tessellation） | 能力 | `1811`-`1833` | 3.2 | P1-6 | 五（可选） |
| P3-1 | GPU Readback（`map` ×2 / `unmap`） | 能力 | `2539`-`2556` | 3.0 | P1-5 | 六 |
| P3-2 | 扩展检测补全 | 缺陷 | `T3DGLES3Mapping.cpp:24`-`35` | 3.0 | — | 七（可并行） |
| P3-3 | `createSamplerState` 各向异性/边框色 | 缺陷 | `926`-`995` | 3.0 + 扩展 | P3-2 | 七 |

阶段顺序与 `GLES3-Renderer-Backend-todo.md` B.8 一致。阶段七无依赖，可以任何时候插入。

---

## 2. 实现前置约定

### 2.1 GLES 版本与 API 可用性

这张表是后面所有门控判断的依据。**GLES 不是 GL 的子集裁剪那么简单，几个关键 API 的版本落点与 GL4 差异很大**：

| 能力 | GL4 用法 | GLES3 对应 | 最低版本 | 说明 |
|------|---------|-----------|---------|------|
| Blit | `glBlitFramebuffer` | 同名 | **3.0** | 但有额外限制，见 §2.2 |
| FBO 失效提示 | 无 | `glInvalidateFramebuffer` | 3.0 | TBR 上是实打实的带宽收益，保留现有用法 |
| 缓冲区拷贝 | `glCopyNamedBufferSubData` | `glCopyBufferSubData` + `GL_COPY_READ/WRITE_BUFFER` | 3.0 | 无 DSA，只能 bind |
| 缓冲区映射 | `glMapNamedBuffer` | `glMapBufferRange` + `glUnmapBuffer` | 3.0 | 无 DSA |
| PBO 回读 | `glGetTextureSubImage` | `glReadPixels` + `GL_PIXEL_PACK_BUFFER` | 3.0 | **GLES 无 `glGetTextureSubImage`**，只能经 FBO |
| 同步对象 | `glFenceSync` | 同名 | 3.0 | `glClientWaitSync` 同 |
| A2C | `GL_SAMPLE_ALPHA_TO_COVERAGE` | 同名 | 3.0 | 可无条件对齐 |
| MSAA 纹理 | `glTexImage2DMultisample` | `glTexStorage2DMultisample` | **3.1** | 现有 `createRenderTexture:310` 已经在用，即 MSAA RT 事实上要求 3.1 |
| Compute | `glDispatchCompute` | 同名 | 3.1 | |
| SSBO | `GL_SHADER_STORAGE_BUFFER` | 同名 | 3.1 | |
| Atomic counter | `GL_ATOMIC_COUNTER_BUFFER` | 同名 | 3.1 | |
| 内存屏障 | `glMemoryBarrier` | 同名 | 3.1 | 位掩码语义一致 |
| 间接绘制 | `glDrawArraysIndirect` / `glDrawElementsIndirect` | 同名 | 3.1 | **参数结构第 4 个字段必须为 0**，见 §7.5 |
| 间接派发 | `glDispatchComputeIndirect` | 同名 | 3.1 | |
| per-RT 混合 | `glEnablei` / `glBlendFuncSeparatei` / `glColorMaski` | 同名（无 ARB 后缀） | **3.2** | 3.1 上可查 `GL_OES_draw_buffers_indexed` |
| base vertex | `glDrawElementsBaseVertex` | 同名 | 3.2 | 现有代码已按 `mGLESMinor >= 2` 门控 |
| base instance | `glDrawElementsInstancedBaseInstance` | **不存在** | — | 永久缺口，返回 `T3D_ERR_NOT_IMPLEMENT` 是正确处理 |
| Tessellation | `glPatchParameteri` | 同名 | 3.2 | 3.1 上可查 `GL_EXT_tessellation_shader` |
| 1D 纹理 | `GL_TEXTURE_1D` | **不存在** | — | 用 height=1 的 2D 模拟 |
| 深度回读 | `glReadPixels(GL_DEPTH_COMPONENT)` | **不允许** | — | GLES 的 `glReadPixels` 只接受颜色格式，见 §8.5 |

### 2.2 `glBlitFramebuffer` 在 GLES 上的额外限制（影响 P1-1 的设计）

这是 GLES 与 GL4 在 blit 上最本质的差异，直接决定了 `doBlit` **不能照抄 GL4 的实现**：

| 限制 | GL4 | GLES 3.0+ | 后果 |
|------|-----|-----------|------|
| MSAA 源 + 缩放 | 允许 | **不允许**（`GL_INVALID_OPERATION`） | MSAA 源必须先等尺寸 resolve |
| MSAA 源 + 翻转（`srcY0 > srcY1`） | 允许 | **不允许** | 翻 Y 只能发生在 resolve 之后 |
| MSAA 源与目标格式 | 可不同 | **必须相同** | resolve FBO 必须与源同 internal format |
| 读写 FBO 同时为 MSAA | 允许（采样数需匹配） | 采样数必须匹配 | 一般不会命中 |
| 深度/模板 + `GL_LINEAR` | 报错 | 报错 | 深度一律 `GL_NEAREST` |
| `glDrawBuffer`（单数） | 有 | **无** | 只能用 `glDrawBuffers(1, &buf)` |

**结论**：GL4 的 `doBlit`（`T3DGL4Context.cpp:4749`-`4754`）刻意「MSAA 源直接 resolve 到目标」以规避 feedback loop，这条策略在 GLES 上不成立。GLES3 必须保留现有 `blit(Tex→RT)`（`2420`-`2428`）的「先 resolve 到 `GLResolveFBO`，再从 resolve FBO blit 到目标」两段式，**并额外在 blit 前解绑纹理单元来规避 feedback loop**（GL4 用 `unbindTextureUnits` 达到同一目的，两边的手段要合起来用）。

### 2.3 线程模型约束

所有 RHI 调用遵循现有约定，新代码必须照做，否则在多线程 RHI 下必崩：

1. **主线程**解引擎对象（`Texture` / `RenderBuffer` / `PixelBuffer2D::getDescriptor()` / `ShaderVariant`），把结果抽成 POD 或智能指针；
2. POD **按值**捕获进 lambda，禁止在 lambda 里访问引擎对象裸指针（`ReadbackRequest*` 这类由 context 自己持有生命周期的除外，参照 GL4 `3782`）；
3. 通过 `ENQUEUE_UNIQUE_COMMAND(lambda, args...)` 入队；
4. 需要 CPU 拿到 GPU 结果时（只有 readback）用 `T3D_AGENT.syncRHIThread()`，参照 GL4 `finishReadback`（`4841` / `4876`）。

反面教材就是当前 `writeBuffer` 纹理分支：描述符没在主线程取，lambda 里只好把宽高硬编码成 0。

### 2.4 错误码扩展

`T3DGLES3Error.h` 现有 24 个码，缺两个本次要用的。在枚举**末尾**追加（前面插入会改变已有码值）：

```cpp
    T3D_ERR_GLES3_UNSUPPORTED_OPERATION,    // 后端能做但当前参数组合做不了（对齐 T3D_ERR_GL4_UNSUPPORTED_OPERATION）
    T3D_ERR_GLES3_READBACK,                 // readback 专用失败码
```

`T3D_ERR_GLES3_MAP_BUFFER`（`:37`）已存在，readback 映射失败复用它。

### 2.5 静默失败收口原则

三种收口方式，按「后端有没有可能支持」区分，不要混用：

| 情形 | 处理 | 示例 |
|------|------|------|
| 能力位为 false 的契约式不支持 | `T3D_RHI_UNSUPPORTED(capField)` / `_PTR` / `_VALUE`（断言 + 警告 + `T3D_ERR_NOT_IMPLEMENT`） | 3.0 设备上的 `dispatch` |
| 本后端永久做不到 | 明确错误码 + 一次性 `T3D_LOG_ERROR` | 非零 `startInstance` |
| 参数非法 | `T3D_ERR_INVALID_PARAM` / `T3D_ERR_OUT_OF_BOUND` | `copyBuffer` 越界 |

**任何情况下都不再返回 `T3D_OK` 表示「什么都没干」**。这是本次改造的第一原则：上层拿到 `T3D_OK` 会认为数据已经在 GPU 上了。

---

## 3. 阶段一：P0 缺陷修复

这一阶段全部是「基线有正确写法、GLES3 写错或没写」，改动集中在 `T3DGLES3Context.cpp`，可以一次提交。

### 3.1 P0-1 `writeBuffer` 纹理分支

**现状**（`2586`-`2609`）：宽高传 `0, 0`，格式硬编码 `GL_RGBA` / `GL_UNSIGNED_BYTE`。等于任何一次纹理 `writeBuffer` 都是空操作（写 0×0 区域），非 RGBA8 格式还会额外报 `GL_INVALID_OPERATION`。

**修法**：照 GL4 `3960`-`3988`，主线程取描述符：

```cpp
if (isTexture)
{
    PixelBuffer2D *pb = static_cast<PixelBuffer2D*>(renderBuffer);
    const auto &desc = pb->getDescriptor();
    const GLenum pixelFmt  = GLES3Mapping::get(desc.format);
    const GLenum pixelType = GLES3Mapping::getPixelType(desc.format);
    const GLsizei texW = static_cast<GLsizei>(desc.width);
    const GLsizei texH = static_cast<GLsizei>(desc.height);
    GLES3PixelBuffer2DPtr glTex = static_cast<GLES3PixelBuffer2D*>(rhiRes.get());

    auto lambda = [this](const GLES3PixelBuffer2DPtr &glTex, Buffer ownedBuffer,
        GLsizei texW, GLsizei texH, GLenum pixelFmt, GLenum pixelType)
    {
        glBindTexture(GL_TEXTURE_2D, glTex->GLTexture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texW, texH, pixelFmt, pixelType, ownedBuffer.Data);
        glBindTexture(GL_TEXTURE_2D, 0);
        GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::writeBuffer(texture)");
        T3D_POD_SAFE_DELETE_ARRAY(ownedBuffer.Data);
        return T3D_OK;
    };

    return ENQUEUE_UNIQUE_COMMAND(lambda, glTex, ownedBuffer, texW, texH, pixelFmt, pixelType);
}
```

**GLES 额外注意**：`glTexStorage2D` 创建的不可变纹理只能 `glTexSubImage2D`，不能 `glTexImage2D`；现有 `createPixelBuffer2D` 就是 `glTexStorage2D`，所以上面的写法是对的。另外要校验 `buffer.DataSize` 与 `texW * texH * bpp` 是否匹配，不匹配直接返回 `T3D_ERR_INVALID_PARAM`（GL4 也没做，属于顺手加固）。

### 3.2 P0-2 `attachGraphicsShader` 收口

**现状**：`setVertexShader`（`1650`）、`setPixelShader`（`1752`）、`setGeometryShader`（`1897`）各写一遍 attach 逻辑，`setVertexShader` 缺 `nullptr` 检查，传 `nullptr` 直接崩。

**修法**：照 GL4 `4455`-`4493` 抽公共 helper。头文件 `T3DGLES3Context.h` protected 区新增：

```cpp
        TResult attachGraphicsShader(ShaderVariant *shader, ShaderVariant *&currentVariant);
```

实现：

```cpp
TResult GLES3Context::attachGraphicsShader(ShaderVariant *shader, ShaderVariant *&currentVariant)
{
    ShaderVariant **slot = &currentVariant;

    if (shader == nullptr)
    {
        auto lambda = [this, slot]() { *slot = nullptr; return T3D_OK; };
        return ENQUEUE_UNIQUE_COMMAND(lambda);
    }

    GLES3Shader *glShader = static_cast<GLES3Shader*>(shader->getRHIShader());
    if (glShader == nullptr || glShader->GLShaderHandle == 0)
    {
        T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "attachGraphicsShader : shader has no RHI object !");
        return T3D_ERR_INVALID_POINTER;
    }

    GLuint shaderHandle = glShader->GLShaderHandle;

    auto lambda = [this](GLuint shaderHandle, ShaderVariant *variant, ShaderVariant **slot)
    {
        *slot = variant;
        if (mCurrentProgram == 0) { mCurrentProgram = glCreateProgram(); }
        glAttachShader(mCurrentProgram, shaderHandle);
        mProgramDirty = true;
        GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::attachGraphicsShader");
        return T3D_OK;
    };

    return ENQUEUE_UNIQUE_COMMAND(lambda, shaderHandle, shader, slot);
}
```

`setVertexShader` / `setPixelShader` / `setGeometryShader` 改为一行转发。GS 保留 `mGLESMinor >= 2` 的门控（不满足时打警告返回 `T3D_ERR_NOT_IMPLEMENT`，不要静默 `T3D_OK`）。

**顺带**：GS 目前没有 `mCurrentGSVariant` 成员，`bindSamplers` 的 `remapUnit` 只查 VS / PS。补 `mCurrentGSVariant` 成员并纳入 `remapUnit` 的查找链，与 GL4（`4455` 的 `currentVariant` 参数化设计）一致。

### 3.3 P0-3 `resetRenderTarget` 复位 FBO 标志

**现状**（`571`-`588`）：只 `glBindFramebuffer(GL_FRAMEBUFFER, 0)`，不复位 `mRenderingToFBO`。后果是 reset 之后 `setViewProjectionTransform`（`199`-`228`）继续按 FBO 朝向翻 Y，`blit` 的 `flipY` 判断也跟着错。

**修法**：lambda 内加一行 `mRenderingToFBO = false;`。同时 `reset()`（`2306`-`2337`）里也补上（它 `glBindFramebuffer(0)` 了却同样没复位），并顺手复位新增的 compute 相关成员：

```cpp
                mRenderingToFBO = false;
                mCurrentVSVariant = nullptr;
                mCurrentPSVariant = nullptr;
                mCurrentGSVariant = nullptr;
                mCurrentCSVariant = nullptr;
                GL_SAFE_DELETE_PROGRAM(mCurrentComputeProgram);
                mComputeProgramDirty = false;
```

### 3.4 P0-4 独立 compute program

**现状**（`2008`-`2038`）：CS 被 attach 到 `mCurrentProgram`（图形 program）。GL/ES 规范都禁止 compute shader 与图形 stage 混链，链接必失败；而且 `shader == nullptr` 和 3.0 设备两条路径都静默 `return T3D_OK`。

**修法**：照 GL4 `2535`-`2578` + `2955`-`2991`。头文件新增成员与 helper：

```cpp
        GLuint  mCurrentComputeProgram {0};
        bool    mComputeProgramDirty {false};
        ShaderVariant *mCurrentCSVariant {nullptr};
        ShaderVariant *mCurrentGSVariant {nullptr};   // P0-2 顺带

        TResult ensureComputeProgramLinked();
```

`setComputeShader`：

```cpp
TResult GLES3Context::setComputeShader(ShaderVariant *shader)
{
    if (!mCapabilities.supportsCompute)
    {
        T3D_RHI_UNSUPPORTED(supportsCompute);
    }

    if (shader == nullptr)
    {
        auto lambda = [this]()
        {
            mCurrentCSVariant = nullptr;
            if (mCurrentComputeProgram != 0)
            {
                glUseProgram(0);
                GL_SAFE_DELETE_PROGRAM(mCurrentComputeProgram);
            }
            mComputeProgramDirty = false;
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda);
    }
    // ... 与 GL4 2556-2577 同构：新建独立 program、attach、置 dirty
}
```

`ensureComputeProgramLinked()` 与 GL4 `2955` 完全同构：无 program 报错、dirty 才 link、link 后 `glUseProgram` + `bindPendingUniformBlocks` + `setupSamplerBindings`。

**注意**：`dispatch` 之后要把 `glUseProgram(mCurrentProgram)` 恢复回图形 program（GL4 `3339`-`3342`），否则后续 draw call 会用着 compute program。

**前置**：真正能跑还需要 P1-6（glslang 认识 `kCompute`）与 P1-5（`supportsCompute` 动态取值）。本项先把结构改对，能力位仍为 false 时走 `T3D_RHI_UNSUPPORTED`。

### 3.5 P0-5 `copyBuffer` 校验

**现状**（`2471`-`2535`）：只判两个空指针。不判 `src == dst`、不判 `size == 0` 的语义、不判越界、不判 `kImmutable`；类型不匹配时 lambda 内 `break` 掉，外层照样返回 `T3D_OK`。

**修法**：照抄 GL4 `3697`-`3765` 的六道校验，只把 DSA 换成 bind：

```cpp
    // 1. 空指针  2. src == dst  3. 仅线性缓冲（含 kStructuredBuffer）
    // 4. dst 非 kImmutable      5. size == 0 → 从 srcOffset 到结尾
    // 6. srcOffset + copySize / dstOffset + copySize 越界
```

`isLinearBuffer` 的类型集合要包含 `kStructuredBuffer`（P2-1 之后就会有），提前写上不影响当前行为。GL 句柄提取抽一个 `getGLBufferHandle(RenderBuffer*)`（对应 GL4 `4497`），后面 readback / indirect 都要用：

```cpp
GLuint GLES3Context::getGLBufferHandle(RenderBuffer *buffer) const
{
    if (buffer == nullptr || buffer->getRHIResource() == nullptr) { return 0; }
    switch (buffer->getRHIResource()->getResourceType())
    {
    case RHIResource::ResourceType::kVertexBuffer:     return static_cast<GLES3VertexBuffer*>(...)->GLBuffer;
    case RHIResource::ResourceType::kIndexBuffer:      return static_cast<GLES3IndexBuffer*>(...)->GLBuffer;
    case RHIResource::ResourceType::kConstantBuffer:   return static_cast<GLES3ConstantBuffer*>(...)->GLBuffer;
    case RHIResource::ResourceType::kStructuredBuffer: return static_cast<GLES3StructuredBuffer*>(...)->GLBuffer;  // P2-1 后启用
    default: return 0;
    }
}
```

拷贝本体保留现有 `GL_COPY_READ_BUFFER` / `GL_COPY_WRITE_BUFFER` 写法（避免污染 VAO 的 EBO 绑定，这一点比 GL4 的 DSA 更需要小心，现有实现是对的）。

### 3.6 P0-6 `bindPixelBuffers`

**现状**（`2905`-`2910`）三个问题一起改：

| 问题 | 现状 | 修法 | GL4 参考 |
|------|------|------|---------|
| 不解绑 | `handle == 0` 直接 `continue`，上一次的纹理留在单元上 | 去掉 `continue`，`glBindTexture(target, 0)` 照常执行 | `4321`-`4325` |
| 不复位活动单元 | 循环结束停在最后一个单元 | 循环后 `glActiveTexture(GL_TEXTURE0)` | `4326` |
| MSAA 采错纹理 | 2D 分支永远绑 `GLTexture` | `GLMSAACount > 1 && GLResolveTex != 0` 时绑 `GLResolveTex`（target 仍是 `GL_TEXTURE_2D`） | `4278`-`4296` |

MSAA 那条最关键：MSAA 渲染纹理的可采样对象是 resolve 纹理，绑多重采样纹理到 `GL_TEXTURE_2D` 上是错误的 target 组合，采样结果未定义。

GL4 的第三分支（`GLMSAACount > 1` 但无 resolve → 绑 `GL_TEXTURE_2D_MULTISAMPLE`）在 GLES3 上要按版本裁剪：`GL_TEXTURE_2D_MULTISAMPLE` 是 3.1 才有的 target，3.0 设备上该分支应报错而不是绑错 target。实际上现有 `createRenderTexture` 的 MSAA 路径必带 resolve（`334`-`344`），所以第三分支只是防御。

### 3.7 P0-7 `bindSamplers`

**现状**（`2986`-`2992`）：`handle == 0` 跳过。修法同上，改成 `glBindSampler(unit, 0)`。注意这里的 unit 要走已有的 `remapUnit`（`2927`-`2958`）换算，不能直接用 `startSlot + i`。

### 3.8 P0-8 静默成功接口收口

在真实现落地之前（阶段二 / 五），先让这些接口说实话：

| 接口 | 现状 | 过渡处理 | 最终归宿 |
|------|------|---------|---------|
| `createHullShader` / `createDomainShader`（`1811` / `1825`） | 返回 `nullptr` 无日志 | `T3D_LOG_WARNING` + `nullptr` | P2-5（3.2 实现） |
| Hull / Domain 的 8 个 set 系列（`1816`-`1819` / `1830`-`1833`） | `return T3D_OK` | `T3D_LOG_WARNING` + `T3D_ERR_NOT_IMPLEMENT` | P2-5 |
| `blit(RT→RT)`（`2343`）、`blit(RT→Tex)`（`2457`）、`blit(Tex→Tex)`（`2464`） | `return T3D_OK` | `T3D_ERR_NOT_IMPLEMENT` | P1-1 |
| `reflectSamplerBindings`（基类 `396`） | `return T3D_OK` | 不动（阶段二直接实现，中间态返回错误会打断现有渲染） | P1-2 |

日志一律用 `T3D_LOG_WARNING` 且只在首次调用打（可以用函数内 `static bool` 或计数），避免逐帧刷屏。

### 3.9 P0-9 `StencilRef`（建议与 GL4 一并修）

**现状**：`setDepthStencilState`（`1062`-`1063`）把 `glStencilFuncSeparate` 的 ref 硬编码为 1，`GL4Context` 同样（`1627`-`1628`）。D3D11 / Vulkan / Metal 都用描述符里的真实 ref。任何 ref ≠ 1 的模板比较、或 `StencilOp::kReplace` 想写非 1 值都会错。

**修法**：`GLES3DepthStencilStateData`（`T3DGLES3RenderState.h:40`-`56`）新增 `uint8_t stencilRef {0};`，`createDepthStencilState` 从 `desc.StencilRef` 取值，`setDepthStencilState` 用 `d.stencilRef` 替换字面量 1。GL4 侧同名改动（`GL4DepthStencilStateData` + `1352` + `1627`）。

这是 GL 家族共有缺口，两个后端一起改才有意义，单改一边会让两边行为分叉。

---

## 4. 阶段二：blit 统一收口与采样器反射

### 4.1 P1-1 设计概述

对齐 D3D11 / GL4 的三段式结构：**端点解析 → 统一执行 → 四个重载薄封装**。

```
blit(RT,  RT)  ─┐
blit(Tex, RT)  ─┤   resolveBlitEndpoint(src, asSource=true)  ─┐
blit(RT,  Tex) ─┤ → resolveBlitEndpoint(dst, asSource=false) ─┤ → doBlit(srcEp, dstEp, offsets, size)
blit(Tex, Tex) ─┘                                             ┘
```

### 4.2 数据结构

`T3DGLES3Context.h` protected 区新增（对照 GL4 `T3DGL4Context.h:257`-`274`）：

```cpp
        struct BlitEndpoint
        {
            GLuint      fbo {0};
            GLuint      texture {0};
            GLuint      resolveFbo {0};
            GLuint      resolveTex {0};
            uint32_t    width {0};
            uint32_t    height {0};
            uint32_t    sampleCount {1};
            bool        isWindow {false};
            bool        isDepth {false};
            bool        needsScratchFbo {false};
        };

        TResult resolveBlitEndpoint(Texture *tex, bool asSource, BlitEndpoint &out);
        TResult resolveBlitEndpoint(RenderTarget *rt, bool asSource, BlitEndpoint &out);
        TResult doBlit(const BlitEndpoint &src, const BlitEndpoint &dst,
            const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset);
        void unbindTextureUnits();

        GLuint  mScratchReadFBO {0};
        GLuint  mScratchDrawFBO {0};
```

两个 scratch FBO 在析构与 `reset()` 里用 `GL_SAFE_DELETE_FBO` 清理。

### 4.3 `resolveBlitEndpoint`

与 GL4 `4561`-`4635` 同构，两处 GLES 差异：

```cpp
TResult GLES3Context::resolveBlitEndpoint(Texture *tex, bool asSource, BlitEndpoint &out)
{
    // 1. 空指针 / 无 RHI 资源 → T3D_ERR_INVALID_POINTER
    // 2. 仅 TT_2D / TT_RENDER_TEXTURE，其余 → T3D_ERR_GLES3_UNSUPPORTED_OPERATION
    //    （GLES3 无 1D 纹理，Cubemap / 3D 的 face / slice 选择本次不做）
    GLES3PixelBuffer2D *glPB = ...;
    out.texture         = glPB->GLTexture;
    out.fbo             = glPB->GLFBO;
    out.resolveFbo      = glPB->GLResolveFBO;
    out.resolveTex      = glPB->GLResolveTex;
    out.width           = tex2D->getWidth();
    out.height          = tex2D->getHeight();
    out.sampleCount     = glPB->GLMSAACount;
    out.isDepth         = (format 落在 E_PF_D24_UNORM_S8_UINT .. E_PF_D16_UNORM 区间);
    out.needsScratchFbo = (glPB->GLFBO == 0);   // 普通采样纹理没有自带 FBO
    return T3D_OK;
}
```

`RenderTarget` 版：窗口 → `fbo = 0` / `isWindow = true`；否则递归到颜色附件 0，无颜色附件时递归到 depthStencil；两者都没有报错。与 GL4 `4603`-`4635` 一致。

**这一步解决了现有实现「源必须是带 FBO 的渲染纹理」的限制**（`2358`）：普通 2D 纹理走 scratch FBO 路径。

### 4.4 `doBlit`

GLES 版与 GL4 `4656`-`4767` 的差异集中在 MSAA 与 `glDrawBuffers`：

```cpp
TResult GLES3Context::doBlit(const BlitEndpoint &src, const BlitEndpoint &dst,
    const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
{
    // ① 先摘干净所有 texture unit，避免同一张纹理既在 unit 上又当 FBO 附件（feedback loop）
    unbindTextureUnits();

    GLint prevFBO = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

    GLuint readFBO = src.fbo;
    GLuint drawFBO = dst.fbo;

    // ② scratch FBO：纹理没有自带 FBO 时临时挂上去
    if (src.needsScratchFbo) { /* mScratchReadFBO + glFramebufferTexture2D(src.texture) */ }
    if (dst.needsScratchFbo) { /* mScratchDrawFBO + glFramebufferTexture2D(dst.texture) */ }

    // ③ size == ZERO 语义：整张（源宽高减去 offset）
    const uint32_t copyW = (size == Vector3::ZERO)
        ? (src.width  - std::min((uint32_t)srcOffset.x(), src.width))
        : (uint32_t)size.x();
    const uint32_t copyH = (size == Vector3::ZERO)
        ? (src.height - std::min((uint32_t)srcOffset.y(), src.height))
        : (uint32_t)size.y();
    if (copyW == 0 || copyH == 0) { return T3D_ERR_INVALID_PARAM; }

    // ④ GLES 专有：MSAA 源必须先等尺寸、不翻转地 resolve（见 §2.2）
    if (src.sampleCount > 1)
    {
        if (src.resolveFbo == 0 || src.resolveTex == 0)
        {
            T3D_LOG_ERROR(..., "blit : MSAA source has no resolve FBO !");
            return T3D_ERR_GLES3_UNSUPPORTED_OPERATION;
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER, src.fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, src.resolveFbo);
        glBlitFramebuffer(0, 0, src.width, src.height, 0, 0, src.width, src.height,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
        readFBO = src.resolveFbo;
    }

    // ⑤ 矩形与翻转：翻转只能作用在非 MSAA 的读源上，此时源已 resolve 完毕
    //    dst.isWindow && mProjectionFlipped → 交换 srcY0 / srcY1

    // ⑥ mask：深度端点走 GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT，否则 GL_COLOR_BUFFER_BIT
    //    深度 blit 要求两端 format 完全一致，不一致直接报错（GLES 比 GL4 严）

    // ⑦ 绑定：GLES 无 glDrawBuffer(单数)
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
    if (!src.isDepth) { glReadBuffer(readFBO == 0 ? GL_BACK : GL_COLOR_ATTACHMENT0); }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFBO);
    if (!dst.isDepth)
    {
        const GLenum drawBuf = (drawFBO == 0) ? GL_BACK : GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, &drawBuf);
    }

    glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, GL_NEAREST);

    // ⑧ TBR：保留现有优化，读源用完即失效
    GLenum discards[] = { src.isDepth ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0 };
    glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, 1, discards);

    glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    GL_CHECK_ERROR(LOG_TAG_GLES3RENDERER, "GLES3Context::doBlit");
    return T3D_OK;
}
```

⑧ 有个坑：`glInvalidateFramebuffer` 对**默认帧缓冲**的 attachment 名字不同（`GL_COLOR` / `GL_DEPTH` / `GL_STENCIL`，不是 `GL_COLOR_ATTACHMENT0`）。`readFBO == 0` 时要么用 `GL_COLOR`，要么干脆跳过失效调用。现有代码（`2438`）没区分，属于顺手修掉的小 bug。

### 4.5 `unbindTextureUnits`

GL4 版（`4639`-`4652`）固定循环 16 个单元、遍历 5 种 target。GLES3 版需要：

- 去掉 `GL_TEXTURE_1D`（不存在）；
- `GL_TEXTURE_2D_MULTISAMPLE` 仅在 `mGLESMinor >= 1` 时解绑；
- 单元数用 `glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS)` 查一次缓存起来，别写死 16（GLES 设备下限是 32，但写死不好）。

### 4.6 四个重载

统一成 GL4 `3577`-`3693` 的模板：空指针检查 → 两次 `resolveBlitEndpoint` → `ENQUEUE_UNIQUE_COMMAND(doBlit)`。四个函数体除参数类型外完全一致。现有 `blit(Tex→RT)` 的 200 行实现整体删除，逻辑并入 `doBlit`。

**内部优先级**：先保证 `Tex→Tex` 与 `Tex→RT` 的 `ZERO` 语义（相机后处理链最先踩到），再补 `RT→Tex` / `RT→RT`。

### 4.7 P1-2 `reflectSamplerBindings`

**现状**：`GLES3Context::reflectSamplerBindings`（`2073`）转发到基类空实现（`T3DGLES3ContextBase.cpp:396` `return T3D_OK`）。

**影响面**：`bindSamplers` 的 `remapUnit`（`2927`-`2958`）靠 `getSamplerBinding()` / `getTexBinding()` 反查纹理单元，元数据缺失就会绑错单元。只是当前 `reflectShaderAllBindings` 顺带填过一部分，掩盖了这个洞。

**修法**：在 `GLES3ContextBase.cpp` 实现（不是 Runtime 层，基类持有 `mReflectionCache`），照 GL4 `2835`-`2897`，sampler 类型表按 GLES 调整：

```cpp
    switch (uniform.glDefineType)
    {
    case GL_SAMPLER_2D:              texType = TT_2D;   break;
    case GL_SAMPLER_3D:              texType = TT_3D;   break;
    case GL_SAMPLER_CUBE:            texType = TT_CUBE; break;
    case GL_SAMPLER_2D_SHADOW:       texType = TT_2D;   break;
    case GL_SAMPLER_CUBE_SHADOW:     texType = TT_CUBE; break;   // GLES3 有，GL4 版漏了
    case GL_SAMPLER_2D_ARRAY:        texType = TT_2D;   break;   // 同上
    case GL_SAMPLER_2D_ARRAY_SHADOW: texType = TT_2D;   break;
    case GL_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_2D: texType = TT_2D;   break;
    // 无 GL_SAMPLER_1D：GLES3 的 1D 纹理用 height=1 的 2D 模拟
    default: break;
    }
```

`SPIRV_Cross_Combined` 前缀还原逻辑与 `reflectShaderAllBindings` 里现有的那段完全一样，抽成基类私有 helper `stripCombinedSamplerName(const String&)` 给两处共用，避免两份逻辑漂移。

---

## 5. 阶段三：混合状态与 RT resize

### 5.1 P1-3 独立混合与 AlphaToCoverage

**现状**：`createBlendState`（`805`）只读 `desc.RenderTargetStates[0]`，忽略 `IndependentBlendEnable` 与 `AlphaToCoverageEnable`；`setBlendState`（`999`-`1030`）只有全局版 API。而 `setRenderTarget` 已经支持多颜色附件 + `glDrawBuffers`，MRT 路径上这个缺口一定会被踩到。

**结构改动**：`T3DGLES3RenderState.h` 的 `GLES3BlendStateData`（`16`-`26`）改成 GL4 的分层形态（对照 `GL4BlendStateData`）：

```cpp
    struct GLES3BlendTargetData
    {
        bool        enabled {false};
        GLenum      srcRGB {GL_ONE};
        GLenum      dstRGB {GL_ZERO};
        GLenum      srcAlpha {GL_ONE};
        GLenum      dstAlpha {GL_ZERO};
        GLenum      opRGB {GL_FUNC_ADD};
        GLenum      opAlpha {GL_FUNC_ADD};
        GLboolean   colorMask[4] {GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE};
    };

    struct GLES3BlendStateData
    {
        bool independentBlend {false};
        bool alphaToCoverage {false};
        GLES3BlendTargetData targets[BlendDesc::kMaxRenderTarget];
    };
```

**`createBlendState`**：与 GL4 `1297`-`1323` 同构，`independentBlend` 时填满 `kMaxRenderTarget` 个，否则只填 `targets[0]`。

**`setBlendState`**：两条路径，按版本门控：

```cpp
    // A2C：GLES 3.0 core，无条件对齐
    d.alphaToCoverage ? glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE) : glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    if (d.independentBlend && mSupportsIndexedBlend)   // mGLESMinor >= 2 或 GL_OES_draw_buffers_indexed
    {
        for (i : kMaxRenderTarget)
        {
            rt.enabled ? (glEnablei(GL_BLEND, i), glBlendFuncSeparatei(...), glBlendEquationSeparatei(...))
                       : glDisablei(GL_BLEND, i);
            glColorMaski(i, ...);
        }
    }
    else
    {
        if (d.independentBlend) { /* 首次警告：降级到 RT0 */ }
        // 现有全局版逻辑，用 targets[0]
    }
```

注意 GLES 的函数名**没有 ARB 后缀**（`glBlendFuncSeparatei` 而非 GL4 里的 `glBlendFuncSeparateiARB`）。3.1 设备走 `GL_OES_draw_buffers_indexed` 时函数名带 `OES` 后缀，需要通过 `eglGetProcAddress` 取；如果不想引入扩展函数指针管理，第一版可以只支持 3.2 core，3.1 直接降级 + 警告。

降级警告用 `static bool` 只打一次，MRT 材质每帧都设混合状态。

### 5.2 P1-4 RT resize

**前置重构**：`createRenderTexture`（`273`-`450`）是一整个 178 行的 lambda，MSAA 纹理 + resolve FBO + depth RBO 全在里面。照 GL4（`750` / `767`）拆两个 helper：

```cpp
        void releaseRenderTextureResources(GLES3PixelBuffer2D *pb);
        TResult buildRenderTextureResources(PixelBuffer2D *buffer, GLES3PixelBuffer2D *glPixelBuffer);
```

`releaseRenderTextureResources` 的删除顺序照 GL4 `750`-`763`：resolveFBO → resolveTex → depthRBO → FBO → texture，最后 `GLMSAACount = 1`。

`buildRenderTextureResources` 把现有 lambda 体整体搬进去，签名从「一堆 POD 参数」改为「从 `buffer->getDescriptor()` 现取」——它只在 RHI 线程执行，且 `PixelBuffer2D` 由智能指针保活，与 GL4 `767` 一致。`createRenderTexture` 变成：

```cpp
    auto lambda = [this](const PixelBuffer2DPtr &buffer, const GLES3PixelBuffer2DPtr &glPixelBuffer)
    {
        return buildRenderTextureResources(buffer.get(), glPixelBuffer.get());
    };
```

**`resizeRenderTexture`**（照 GL4 `943`-`974`）：校验非空 / 尺寸非零 → 校验描述符尺寸已被引擎侧改成新值（引擎入口是 `RenderTexture::resize`，后端只重建 RHI 资源）→ 入队 `release + build`。

**`resizeRenderTarget`**（照 GL4 `978`-`1019`）：窗口类型转发到已有的 `resizeRenderWindow(GLES3RenderWindow*, w, h)`（`176`-`198`，已实现）；否则遍历 `getNumOfRenderTextures()` 个颜色附件 + depthStencil 逐个 `resize`。

两个接口都要在 `T3DGLES3Context.h` 补 `override` 声明（当前全文没有，走的是 `RHIContext` 默认的 `T3D_ERR_NOT_IMPLEMENT`）。

**GLES 特有注意**：resize 后原 FBO 句柄失效，如果 `mCurrentRenderTarget` 正指向它，需要在 build 之后重新 `setRenderTarget`。GL4 没处理这一点是因为桌面端 resize 通常发生在帧边界；移动端旋转屏幕时可能在帧中触发，建议在 `resizeRenderTexture` 末尾补一次「若当前 RT 命中则重绑」的保护。

---

## 6. 阶段四：能力位动态化与 glslang stage 补全

### 6.1 P1-5 `fillCapabilities`

**现状**（`101`-`140`）：六个高级能力位硬编码 false，理由是「后端未实现对应 RHI 接口」。这在当下是诚实的取值，但阶段五落地后必须回来重写，不如现在就按 GL4（`115`-`160`）的形态做成版本判定：

```cpp
void GLES3Context::fillCapabilities()
{
    // 版本来源见 6.2，此时 mGLESMajor / mGLESMinor 已由 glGetIntegerv 二次确认
    const bool has31 = (mGLESMajor > 3) || (mGLESMajor == 3 && mGLESMinor >= 1);
    const bool has32 = (mGLESMajor > 3) || (mGLESMajor == 3 && mGLESMinor >= 2);

    mCapabilities.supportsInstancing          = true;   // 3.0 core
    mCapabilities.supportsBaseInstance        = false;  // GLES 永远没有
    mCapabilities.supportsCompute             = has31;
    mCapabilities.supportsUnorderedAccess     = has31;
    mCapabilities.supportsStructuredBuffer    = has31;
    mCapabilities.supportsIndirectDraw        = has31;
    mCapabilities.supportsIndirectDispatch    = has31;
    mCapabilities.supportsAppendConsumeBuffer = has31;
    mCapabilities.supportsReadback            = true;   // 3.0 core，P3-1 落地后置 true

    if (has31) { /* 现有的 group count / shared memory / SSBO 上限查询 */ }
    while (glGetError() != GL_NO_ERROR) {}
}
```

**上线节奏很重要**：能力位一旦置 true，上层就会走非降级路径。所以每个能力位必须**与对应接口的实现同一个提交上线**，不能提前打开。建议做法是在阶段四只落地版本探测与 `has31` / `has32` 的计算，能力位赋值语句在阶段五 / 六里逐条从 `false` 改成 `has31`。

### 6.2 版本号来源

`mGLESMajor` / `mGLESMinor`（`T3DGLES3ContextBase.h:73`-`74`）当前来自 `createEGLContext` 的降级尝试（3.2 → 3.1 → 3.0），拿到的是「EGL 愿意给的上下文版本」，可能低估驱动能力，导致 GS / CS 被误禁。

**修法**：`initEGLContext` 成功、`eglMakeCurrent` 之后补一次二次确认：

```cpp
    GLint major = 0, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    if (major > 0) { mGLESMajor = major; mGLESMinor = minor; }
    while (glGetError() != GL_NO_ERROR) {}
```

注意顺序：`fillCapabilities` 必须在这之后调用。

### 6.3 P1-6 glslang stage 补全

**现状**（`T3DGLES3ContextBase.cpp:191`-`201`）：switch 只认 `kVertex` / `kPixel` / `kGeometry`，`kCompute` 直接落 default 报 `T3D_ERR_GLES3_SHADER_REFLECTION`。所以即便 `createComputeShader` 能编译 GL 侧 shader，**上层的编译链路是断的**。

**修法**：

```cpp
        EShLanguage glslangStage;
        int minVersion = 300;
        switch (shader->getShaderStage())
        {
        case SHADER_STAGE::kVertex:   glslangStage = EShLangVertex;         break;
        case SHADER_STAGE::kPixel:    glslangStage = EShLangFragment;       break;
        case SHADER_STAGE::kGeometry: glslangStage = EShLangGeometry;   minVersion = 320; break;
        case SHADER_STAGE::kCompute:  glslangStage = EShLangCompute;    minVersion = 310; break;
        case SHADER_STAGE::kHull:     glslangStage = EShLangTessControl;    minVersion = 320; break;
        case SHADER_STAGE::kDomain:   glslangStage = EShLangTessEvaluation; minVersion = 320; break;
        default: /* 报错 */ break;
        }

        const int deviceVersion = 300 + mGLESMinor * 10;
        if (deviceVersion < minVersion)
        {
            T3D_LOG_ERROR(LOG_TAG_GLES3CONTEXTBASE,
                "shader stage requires ESSL %d, device provides %d !", minVersion, deviceVersion);
            ret = T3D_ERR_GLES3_NOT_SUPPORTED;
            break;
        }

        const int profileVersion = std::max(deviceVersion, minVersion);
        // parse(resources, profileVersion, EEsProfile, false, false, EShMsgDefault)
```

现有的 `profileVersion` 计算（`213`-`215`）按设备版本取 300/310/320，改成「设备版本与 stage 最低要求取大」，并在设备不满足时给明确错误码而不是让 glslang 报一堆看不懂的语法错。

**上游依赖**：这条链路能不能真正跑通，还取决于 shader 编译管线（SPIRV-Cross → ESSL）是否为 GLES3 后端产出 compute / tess 变体。这部分不在本后端范围内，需要与 `RHI-Compute-UAV-Indirect-Draw-Design-todo.md` 的变体生成一起验证。

---

## 7. 阶段五：Compute / SSBO / UAV / Indirect

整段以 `mCapabilities.supportsXXX` 为总闸，3.0 设备上仍走 `T3D_RHI_UNSUPPORTED`。现有的 11 行单行 stub（`2292`-`2302`）全部展开。

### 7.1 P2-1 `GLES3StructuredBuffer`

GLES3 目前**没有**这个类（`T3DGLES3Prerequisites.h` 的前向声明列表里也没有）。新增到 `T3DGLES3RenderBuffer.h`，对照 `GL4StructuredBuffer`：

```cpp
    class GLES3StructuredBuffer : public RHIStructuredBuffer
    {
    public:
        static GLES3StructuredBufferPtr create();
        ~GLES3StructuredBuffer() override;
        void *getNativeObject() const override;

        GLuint      GLBuffer {0};
        GLuint      GLCounterBuffer {0};
        uint32_t    ElementCount {0};
        uint32_t    ElementSize {0};
        bool        HasCounter {false};
    protected:
        GLES3StructuredBuffer() = default;
    };
```

`T3DGLES3Prerequisites.h` 补前向声明与 `T3D_DECLARE_SMART_PTR`；`T3DGLES3RenderBuffer.cpp` 补 `create` / 析构（`GL_SAFE_DELETE_BUFFER` 两个句柄）/ `getNativeObject`。

`createStructuredBuffer` 与 GL4 `3151`-`3208` 同构，无需改动（`glGenBuffers` + `glBufferData(GL_SHADER_STORAGE_BUFFER)` + 可选 atomic counter，全是 3.1 core）。

`setVSStructuredBuffers` / `setPSStructuredBuffers` / `setCSStructuredBuffers` 三个都转发到 `bindStructuredBuffers`（GL4 `4382`-`4413`，`glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, handle)`）。

> **GLES 限制**：VS 阶段的 SSBO 支持是可选的，`GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS` 在很多移动 GPU 上是 0。`setVSStructuredBuffers` 里应该查一次这个上限，为 0 时报错而不是静默绑定失败。这是 GLES 相对 GL4 需要额外加的一道检查。

### 7.2 P2-2 UAV 与 atomic counter

`setCSUnorderedAccessBuffers` 照 GL4 `3225`-`3303`：主线程把 `(ssbo, counter, initialCount, hasCounter)` 抽成 POD 数组，lambda 里 `glBindBufferBase` 两个 target，`initialCount != kKeepUAVCounter` 时写初值。

GLES 无 `glNamedBufferSubData`，初值写入改成：

```cpp
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, bindings[i].counter);
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(uint32_t), &count);
```

`uavBarrier` 照 GL4 `3390`-`3410`，`glMemoryBarrier` 的六个位在 GLES 3.1 全部存在，可以原样搬。

`copyStructureCount` 照 GL4 `3412`-`3462`，DSA 换成 `GL_COPY_READ_BUFFER` / `GL_COPY_WRITE_BUFFER` 的 `glCopyBufferSubData`。

### 7.3 P2-3 `dispatch` / `dispatchIndirect`

照 GL4 `3306`-`3388`。要点：

- 入口查能力位 + group count 非零 + 不超 `maxDispatchGroupCount`（`fillCapabilities` 已查好）；
- lambda 内先 `ensureComputeProgramLinked()`（P0-4 引入）；
- `glDispatchCompute` / `glDispatchComputeIndirect` 之后恢复 `glUseProgram(mCurrentProgram)`。

### 7.4 P2-4 间接绘制

`renderIndirect` / `renderIndexedIndirect` 照 GL4 `3464`-`3528`，加 `validateIndirectArgs`（GL4 `4523`-`4557`）：非空 → `kGPUIndirectArgs` 访问标志 → 4 字节对齐 → 不越界 → 有 RHI 资源。

**GLES 专有一条**：GLES 3.1 的 `DrawArraysIndirectCommand` / `DrawElementsIndirectCommand` 第四个字段是 `reservedMustBeZero`（GL 那边是 `baseInstance`）。**args buffer 里这个字段不为 0 时行为未定义**。`validateIndirectArgs` 无法在 CPU 侧读 GPU buffer 来校验，所以要在接口文档与 `RHI-Compute-UAV-Indirect-Draw-Design-todo.md` 里明确写清「GLES 后端要求 baseInstance 字段恒为 0」，与 `supportsBaseInstance = false` 一致。

### 7.5 P2-5 Hull / Domain（可选）

GLES 3.2 core（3.1 上是 `GL_EXT_tessellation_shader`）。前置是 P1-6 的 glslang stage 补全。落地内容：

- `createHullShader` / `createDomainShader` 走与 `createComputeShader` 相同的 `compileGLSLShader(GL_TESS_CONTROL_SHADER / GL_TESS_EVALUATION_SHADER, ...)`；
- `setHullShader` / `setDomainShader` 转发 `attachGraphicsShader`（P0-2 的 helper，加 `mCurrentHSVariant` / `mCurrentDSVariant` 成员）；
- 8 个 set 系列转发到已有的 `stageConstantBuffers` / `bindPixelBuffers` / `bindSamplers`；
- `setPrimitiveType` 遇到 patch 图元时需要 `glPatchParameteri(GL_PATCH_VERTICES, n)`——引擎 `PrimitiveType` 枚举当前只有 5 项、没有 patch，所以这一条要等 RHI 层扩枚举，**本阶段可以只做 shader 侧，绘制侧标注为待引擎支持**。

考虑到移动端真正用 tessellation 的场景极少，这项可以推迟，但**不要再留成静默 `T3D_OK`**（P0-8 已收口）。

---

## 8. 阶段六：GPU Readback

### 8.1 整体结构

照 GL4 的三件套：`allocReadbackRequest`（`4771`）+ `map` 两个重载（`3770` / `3824`）+ `finishReadback`（`4831`）。`ReadbackRequest` 结构可以整体照抄 GL4 `T3DGL4Context.h:276`-`293`，字段一个不改。

头文件新增：

```cpp
        struct ReadbackRequest { /* 同 GL4 */ };
        ReadbackHandle allocReadbackRequest(RenderBuffer *src, bool isTexture, ReadbackRequest *&outRequest);
        TResult finishReadback(ReadbackHandle handle, Buffer &dst);

        uint32_t mNextReadbackIndex {0};
        uint32_t mReadbackGeneration {1};
        TMap<uint32_t, ReadbackRequest> mPendingReadbacks;
```

`allocReadbackRequest` 与 GL4 完全一致（`kCPURead` 校验 + 资源类型校验 + handle 分配），无 GLES 差异。

### 8.2 缓冲区路径（`map(RenderBuffer*, offset, size)`）

GL4 用 `glNamedBufferData` + `glCopyNamedBufferSubData`，GLES 换成 bind 版：

```cpp
        glGenBuffers(1, &request->Staging);
        glBindBuffer(GL_COPY_WRITE_BUFFER, request->Staging);
        glBufferData(GL_COPY_WRITE_BUFFER, (GLsizeiptr)copySize, nullptr, GL_STREAM_READ);
        glBindBuffer(GL_COPY_READ_BUFFER, srcBuf);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, (GLintptr)offset, 0, (GLsizeiptr)copySize);
        glBindBuffer(GL_COPY_READ_BUFFER, 0);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
```

`GL_STREAM_READ` 是 GLES 3.0 core 的 usage hint。越界校验与 `size == 0` 语义照抄 GL4 `3794`-`3803`。

### 8.3 纹理路径（`map(RenderBuffer*, ReadbackRegion)`）

**这里与 GL4 分歧最大**：GLES 没有 `glGetTextureSubImage`，只能「把纹理挂到 FBO 上 + `glReadPixels` 到 PBO」。

```cpp
        // ① MSAA 源先 resolve（与 doBlit 同一套逻辑）
        GLuint tex = glPB->GLTexture;
        if (glPB->GLMSAACount > 1 && glPB->GLResolveTex != 0) { /* blit 到 GLResolveFBO */ tex = glPB->GLResolveTex; }

        // ② 挂 scratch FBO（复用 mScratchReadFBO，阶段二已引入）
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mScratchReadFBO);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, region.mipLevel);
        if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { /* 报错 */ }
        glReadBuffer(GL_COLOR_ATTACHMENT0);

        // ③ 读到 PBO
        glGenBuffers(1, &request->Staging);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, request->Staging);
        glBufferData(GL_PIXEL_PACK_BUFFER, (GLsizeiptr)request->TotalBytes, nullptr, GL_STREAM_READ);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);              // 默认 4，行对齐会破坏 TightRowPitch 约定
        glReadPixels(offsetX, offsetY, copyWidth, copyHeight, readFmt, readType, nullptr);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
```

`GL_PACK_ALIGNMENT` 那一行不能漏：默认值 4 会让非 4 字节对齐宽度的行尾补 padding，与 `TightRowPitch = copyWidth * bpp` 的约定不符。

### 8.4 读回格式选择

`glReadPixels` 的合法 (format, type) 组合在 GLES 上比 GL 严：**只有 `GL_RGBA` + `GL_UNSIGNED_BYTE` 是所有实现必须支持的**，另外每个 FBO 还提供一组实现相关的组合，通过

```cpp
    GLint implFormat = 0, implType = 0;
    glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &implFormat);
    glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE,  &implType);
```

查询（必须在目标 FBO 已绑定后查）。策略：

1. 先按 `GLES3Mapping::get(desc.format)` / `getPixelType(desc.format)` 取理想组合；
2. 与查询到的实现组合比对，不一致且理想组合不是 `(GL_RGBA, GL_UNSIGNED_BYTE)` 时，改用查询结果并按其重算 `bpp` / `TightRowPitch`；
3. 整数格式（`GL_RGBA_INTEGER`）与浮点格式各自只有一组合法组合，不匹配直接返回 `T3D_ERR_GLES3_UNSUPPORTED_OPERATION`。

**这需要 `GLES3Mapping` 新增 `getBytesPerPixel(PixelFormat)`**（GL4 有，GLES3 没有，见 `T3DGLES3Mapping.h` 现有方法列表）。

### 8.5 不支持的情形

明确报错，不要试图绕：

| 情形 | 原因 | 返回 |
|------|------|------|
| 深度 / 模板纹理回读 | GLES 的 `glReadPixels` 不接受 `GL_DEPTH_COMPONENT` / `GL_STENCIL_INDEX` | `T3D_ERR_GLES3_UNSUPPORTED_OPERATION` |
| 3D / Cubemap / 数组纹理 | 需要逐 slice 挂 FBO，第一版不做（GL4 也只支持 2D） | 同上 |
| 压缩纹理 | 无回读路径 | 同上 |

深度回读如果确实需要，唯一的可行路径是先用一个 depth-sampling shader 把深度写进 R32F 颜色 RT，再回读——那属于上层渲染逻辑，不是 RHI 该做的。

### 8.6 `unmap` 与同步

`finishReadback` 与 GL4 `4831`-`4889` 同构，两处替换：

```cpp
    // GL4: glMapNamedBuffer(request->Staging, GL_READ_ONLY)
    glBindBuffer(GL_COPY_READ_BUFFER, request->Staging);
    void *mapped = glMapBufferRange(GL_COPY_READ_BUFFER, 0, (GLsizeiptr)request->TotalBytes, GL_MAP_READ_BIT);
    // ... memcpy ...
    glUnmapBuffer(GL_COPY_READ_BUFFER);
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
```

`T3D_AGENT.syncRHIThread()` 的两次调用位置不变（入口一次确保拷贝命令已执行，memcpy 入队后一次等结果）。

**可选的异步化**：`map` 尾部 `glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0)` 记录 fence，`unmap` 里先 `glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0)` 探测是否就绪，未就绪再走阻塞等待。移动端 GPU 与 CPU 的延迟差比桌面大，这个优化的收益比 GL4 上明显。可以放在第二版。

### 8.7 能力位

全部落地后 `fillCapabilities` 里 `supportsReadback = true`。在此之前保持 false（当前连赋值语句都没有，默认 false，符合预期）。

---

## 9. 阶段七：扩展检测与采样器修正

### 9.1 P3-2 `detectExtensions` 补全

**现状**（`T3DGLES3Mapping.cpp:24`-`35`）：只查 `GL_EXT_texture_format_BGRA8888`。

补齐清单：

| 扩展 | 用途 | 影响的接口 |
|------|------|-----------|
| `GL_EXT_texture_filter_anisotropic` | 各向异性过滤 | `createSamplerState` |
| `GL_EXT_texture_border_clamp` | 边框寻址与边框色（3.2 core） | `createSamplerState`、`GLES3Mapping::get(TextureAddressMode)` |
| `GL_OES_draw_buffers_indexed` | 3.1 上的 per-RT 混合 | `setBlendState`（P1-3） |
| `GL_EXT_tessellation_shader` | 3.1 上的 tessellation | P2-5 |
| `GL_EXT_draw_elements_base_vertex` | 3.1 上的 base vertex | `renderIndexedInstanced` 可放宽 3.2 门控 |
| `GL_EXT_color_buffer_float` | 浮点 RT 与浮点回读 | `createRenderTexture`、P3-1 |

实现方式统一：`glGetStringi(GL_EXTENSIONS, i)` 遍历一次，结果存进静态 bool，提供 `isXXXSupported()` 查询。当前的单字符串查法（`strstr`）在扩展名互为前缀时会误判，一并改掉。

### 9.2 P3-3 `createSamplerState`

**现状**（`961`-`964`）：无条件调 `glSamplerParameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT)`，没有扩展的设备上是一次静默 GL 错误（`GL_INVALID_ENUM`，被 `GL_CHECK_ERROR` 吞在 debug 日志里）。另外 `desc.BorderColor` 完全没处理。

修法：

```cpp
    if (maxAniso > 1.0f && GLES3Mapping::isAnisotropicSupported())
    {
        GLfloat limit = GLES3Mapping::getMaxAnisotropy();     // 由 GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 查得
        glSamplerParameterf(glState->GLSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::min(maxAniso, limit));
    }

    if (addressMode 含 kBorder && GLES3Mapping::isBorderClampSupported())
    {
        glSamplerParameterfv(glState->GLSampler, GL_TEXTURE_BORDER_COLOR_EXT, borderColor);
    }
    // 不支持 border clamp 时，GLES3Mapping::get(kBorder) 应回落到 GL_CLAMP_TO_EDGE 并首次警告
```

---

## 10. 文件改动清单

### 10.1 修改

| 文件 | 涉及工作项 | 改动规模 |
|------|-----------|---------|
| `Runtime/Source/T3DGLES3Context.cpp` | P0-1~9、P1-1、P1-3~5、P2-1~5、P3-1、P3-3 | 大（主战场） |
| `Runtime/Include/T3DGLES3Context.h` | 同上（新增成员、helper、`resize*` override 声明） | 中 |
| `Runtime/Include/T3DGLES3RenderState.h` | P0-9（`stencilRef`）、P1-3（blend 分层） | 小 |
| `Runtime/Source/T3DGLES3RenderState.cpp` | 同上 | 小 |
| `Runtime/Include/T3DGLES3RenderBuffer.h` | P2-1（`GLES3StructuredBuffer`） | 小 |
| `Runtime/Source/T3DGLES3RenderBuffer.cpp` | 同上 | 小 |
| `Runtime/Include/T3DGLES3Prerequisites.h` | P2-1（前向声明 + 智能指针） | 小 |
| `Base/Source/T3DGLES3ContextBase.cpp` | P1-2（`reflectSamplerBindings`）、P1-6（stage）、P1-5（版本二次确认） | 中 |
| `Base/Include/T3DGLES3ContextBase.h` | P1-2（名字还原 helper 声明） | 小 |
| `Base/Include/T3DGLES3Error.h` | §2.4 两个新错误码 | 小 |
| `Base/Include/T3DGLES3Mapping.h` | P3-1（`getBytesPerPixel`）、P3-2（扩展查询） | 小 |
| `Base/Source/T3DGLES3Mapping.cpp` | 同上 | 中 |
| `OpenGL4/Window/**` | P0-9（`StencilRef`，与 GLES3 同步修） | 小 |

无需新增源文件——`GLES3StructuredBuffer` 放进已有的 `T3DGLES3RenderBuffer.*`，与 GL4 的组织方式一致。CMake 不需要改。

### 10.2 需要同步更新的文档

| 文档 | 更新内容 |
|------|---------|
| `GLES3-Renderer-Backend-todo.md` | §0 速查表状态与行号、B.7 统计、B.8 执行进度 |
| `GL4-Renderer-Backend-todo.md` | P0-9 的 `StencilRef` 修复记录 |
| `RHI-Compute-UAV-Indirect-Draw-Design-todo.md` | GLES3 后端的 `baseInstance 必须为 0` 约束（§7.4） |
| `GPU-Readback-onRender-Design-todo.md` | GLES3 的深度回读不支持、格式受 `GL_IMPLEMENTATION_COLOR_READ_*` 限制 |

---

## 11. 验证方案

### 11.1 设备矩阵

单一设备验不出版本门控，最少三档：

| 档位 | 代表设备 | 验证重点 |
|------|---------|---------|
| GLES 3.0 | 老 Mali-T7xx / Adreno 3xx，或用 `EGL_CONTEXT_MINOR_VERSION = 0` 强制降级 | 所有 3.1/3.2 能力位为 false，上层走降级路径不崩、不静默出错 |
| GLES 3.1 | 主流中端机 | Compute / SSBO / Indirect 全通；per-RT 混合走降级警告 |
| GLES 3.2 | 近几年旗舰 | per-RT 混合、base vertex、（可选）tessellation |

x86_64 模拟器可以覆盖 3.0 / 3.1 的功能正确性，但 MSAA 与 TBR 相关行为必须真机验证。

### 11.2 分项验证点

| 工作项 | 验证方法 | 通过标准 |
|--------|---------|---------|
| P0-1 | 用非 RGBA8 格式（如 R8）纹理调 `writeBuffer`，回读比对 | 内容正确、无 GL 错误 |
| P0-2 | `setVertexShader(nullptr)` | 不崩、`mCurrentVSVariant` 置空 |
| P0-3 | RT 渲染 → `resetRenderTarget` → 窗口渲染 | 画面不上下颠倒 |
| P0-4 | 3.1 设备上 `setComputeShader` + draw | 图形 program 不被污染，draw 正常 |
| P0-5 | 越界 / 同源 / immutable 三组非法参数 | 各自返回对应错误码，无 GL 错误 |
| P0-6 | MSAA RT 渲染后作为纹理采样 | 采到 resolve 后的正确内容 |
| P1-1 | 四个重载 × {全量 `ZERO`、子区域、MSAA 源、深度、窗口目标} | 内容与朝向正确，无 `GL_INVALID_OPERATION` |
| P1-2 | 多 sampler 的材质 | 每个 sampler 绑到正确的纹理单元 |
| P1-3 | MRT + 每个 RT 不同混合 + A2C | 各 RT 独立生效；3.1 设备降级且只警告一次 |
| P1-4 | 旋转屏幕 / 手动 resize RT | FBO 重建成功，无残影与 FBO incomplete |
| P2-* | Compute 写 SSBO → `copyStructureCount` → indirect draw 全链路 | 与 GL4 同一用例结果一致 |
| P3-1 | 缓冲区回读 + 2D 纹理回读，与 D3D11/GL4 结果逐字节比对 | 完全一致；`GL_PACK_ALIGNMENT` 边界宽度（如 13 像素宽）也一致 |

### 11.3 回归基线

阶段一落地后，`T3D_LOG_WARNING` 会明显变多（原本静默的路径开始说话）。**这些警告本身就是验证产物**：跑一遍现有 Sample，把出现的警告逐条归类，属于「上层确实在调用未实现接口」的要记录进上层的适配待办。

---

## 12. 风险与回退

| 风险 | 影响 | 缓解 |
|------|------|------|
| 能力位提前置 true | 上层走非降级路径，在不支持的设备上黑屏 | 能力位赋值与接口实现同提交；每个能力位单独一次真机验证 |
| P0-8 收口后上层报错 | 原本「静默成功」的调用变成错误返回，上层可能没有降级分支 | 阶段一先只加日志、错误码分两批上；观察一轮 Sample 日志再切错误码 |
| `doBlit` 的 MSAA 两段式与 feedback loop | 画面出现未定义内容或驱动报错 | `unbindTextureUnits` 必须在 blit 最前；MSAA 用例在真机上单独验 |
| `glReadPixels` 格式不匹配 | 回读数据错乱而不报错 | 强制走 `GL_IMPLEMENTATION_COLOR_READ_*` 校验；与 GL4 结果逐字节比对 |
| glslang ES compute 变体缺失 | 阶段五做完也跑不通 | 阶段四结束时先用一个手写 ESSL compute shader 打通链路，再接变体系统 |
| 只有 Android 能验 | 迭代慢，问题定位困难 | 正文第 24 章的 Desktop EGL / ANGLE 变体建议尽早插入，它能把阶段二~六的调试成本降一个量级 |

**回退粒度**：每个阶段独立提交且互不依赖代码（只依赖顺序），任一阶段出问题可单独 revert。唯一的例外是 P0-4 与阶段五的 compute 链路强耦合，建议合并评审。

---

## 13. 提交拆分建议

| # | 提交内容 | 预估改动 | 评审重点 |
|---|---------|---------|---------|
| 1 | P0-1 / P0-3 / P0-5 / P0-6 / P0-7（纯缺陷，无行为面扩展） | ~250 行 | 校验逻辑与 GL4 对齐程度 |
| 2 | P0-2 `attachGraphicsShader` 收口 + P0-8 日志（暂不改返回码） | ~150 行 | 三个 set 函数的行为等价性 |
| 3 | P0-9 `StencilRef`（GLES3 + GL4 同改） | ~30 行 | 两个后端一起改，不要漏 |
| 4 | P1-1 blit 三段式 + P0-8 blit 返回码切换 | ~400 行 | MSAA 两段式、`ZERO` 语义、feedback loop |
| 5 | P1-2 `reflectSamplerBindings` | ~120 行 | sampler 类型表覆盖度 |
| 6 | P1-3 混合状态 | ~180 行 | 3.2 门控与降级路径 |
| 7 | P1-4 `createRenderTexture` 拆 helper + resize | ~250 行 | 重构前后行为等价 |
| 8 | P1-5 能力位动态化 + P1-6 glslang stage（能力位仍全 false） | ~120 行 | 版本探测顺序 |
| 9 | P0-4 + P2-1~4 Compute / SSBO / UAV / Indirect（含能力位开闸） | ~700 行 | 与 GL4 同用例结果比对 |
| 10 | P3-1 Readback（含能力位开闸） | ~450 行 | 格式协商与 `GL_PACK_ALIGNMENT` |
| 11 | P3-2 / P3-3 扩展检测与采样器 | ~200 行 | 无扩展设备上的降级 |
| 12（可选） | P2-5 Tessellation | ~300 行 | 需先确认引擎 `PrimitiveType` 是否扩 patch |

合计约 3100 行改动，其中约六成是从 GL4 照抄后按 GLES 版本裁剪。

---

## 附：与 GL4 的「不能照抄」清单速查

做的时候最容易踩的就是「以为能照抄结果不能」，集中列一遍：

| 位置 | GL4 写法 | GLES3 必须改成 |
|------|---------|--------------|
| 所有 DSA 调用 | `glNamedBufferData` / `glNamedBufferSubData` / `glCopyNamedBufferSubData` / `glMapNamedBuffer` | bind 到 `GL_COPY_READ/WRITE_BUFFER` 后用非 DSA 版 |
| MSAA blit | 直接以 MSAA FBO 为读源 blit 到目标 | **必须先等尺寸 resolve**，再从 resolve FBO blit（缩放/翻转都不允许发生在 MSAA 源上） |
| `glDrawBuffer(GL_BACK)` | 有 | 用 `glDrawBuffers(1, &buf)` |
| 纹理回读 | `glGetTextureSubImage` | 挂 FBO + `glReadPixels` + PBO，且深度不可读 |
| per-RT 混合 | `glBlendFuncSeparateiARB` | `glBlendFuncSeparatei`（3.2 core，无 ARB 后缀） |
| MSAA 纹理创建 | `glTexImage2DMultisample` | `glTexStorage2DMultisample`（3.1） |
| `unbindTextureUnits` 的 target 列表 | 含 `GL_TEXTURE_1D` | 去掉 1D；`GL_TEXTURE_2D_MULTISAMPLE` 按 3.1 门控 |
| 间接绘制参数 | 第 4 字段是 `baseInstance` | 是 `reservedMustBeZero`，必须为 0 |
| `glInvalidateFramebuffer` 附件名 | GL4 未用 | 默认帧缓冲上是 `GL_COLOR` / `GL_DEPTH`，不是 `GL_COLOR_ATTACHMENT0` |
