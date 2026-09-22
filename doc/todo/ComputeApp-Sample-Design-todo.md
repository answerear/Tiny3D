# ComputeApp：Compute Shader / UAV 验证 Sample 设计

> 为已落地的 RHI compute / UAV / indirect 能力补上**第一个真实调用方**。`RHI-Compute-UAV-Indirect-Draw-Design-todo.md` 的 A/B/C/D 四期共 26 项已全部合入，但 §11 的运行期验证一条都没执行过，代码库里也**没有任何一处调用 `dispatch()`**，`assets` 下**没有任何 compute shader 源文件**。本文设计这个缺失的验证 Sample。
>
> **本文是施工蓝图，不代表已落地。** 代码片段均为「建议实现」。
>
> 相关文档：
>
> - RHI 能力蓝图：`doc/todo/RHI-Compute-UAV-Indirect-Draw-Design-todo.md`（下称 **RHI-Compute 文档**；§6.7 典型调用序列、§9.2 跨编译风险、§11 验证方案由本文承接执行）
> - GPU 读回与帧钩子：`doc/todo/GPU-Readback-onRender-Design-todo.md`（`map` / `unmap` 的时序约束，本文全部自检用例依赖它）
> - Sample 骨架参考：`source/Samples/TextureApp`（读回冒烟 + 三个帧钩子）、`source/Samples/PostProcessingApp`（热键切用例、跨后端嵌入 shader、sample 自带 Behaviour）
> - Shader 工具链：`source/Tools/ShaderCrossCompiler`、`source/Tools/SampleShaderEmbedder/embed-sample-shaders.ps1`

---

## 0. 结论先行：这个 Sample 怎么算跑通

分两轨，互不依赖，可以分期交付：

| 轨 | 谁来判定 | 判定依据 |
|---|---|---|
| **自检轨** | 程序自己 | 每个用例 `dispatch` 后回读结果与 CPU 参考值逐元素比对，不一致就 `T3D_ASSERT` + 打日志。启动后跑一遍，控制台出现 `[ComputeApp] K1..K8 all passed` 即通过 |
| **可视轨** | 人眼 | 6.5 万个粒子在屏幕上被 GPU 积分驱动流动。按 `C` 切到 CPU 参考实现，画面应当**看不出区别**；按 `V` 开 GPU 剔除，视锥外的粒子消失且日志打印的可见数随相机转动变化 |

两轨都要在 **D3D11 debug layer 零 ERROR / 零 WARNING** 的前提下成立。这是 RHI-Compute 文档 §11.2 的第一条，UAV 相关代码最容易触发 HAZARD，比写单测更有效。

**能跑的后端只有三个**：D3D11 Window、OpenGL4 Window（GL 4.3+）、OpenGLES3（ES 3.1+）。Vulkan / Metal / 全部 Console / Null 后端的 compute 是 `T3D_RHI_UNSUPPORTED` stub。Sample 在这些后端上必须**打 warning、跳过自检、可视轨退化为 CPU 实现**，不黑屏不崩溃。

---

## 1. 目标与非目标

### 1.1 目标

| # | 目标 | 对应 RHI-Compute 文档 |
|---|------|----------------------|
| 1 | 打通 compute shader 的**资产 → 跨编译 → 嵌入 → 运行时编译 → 反射 → 派发**全链路，产出仓库里第一批 `.cshader` | §9 |
| 2 | 逐条执行 §11.1 验证矩阵中 compute / UAV / indirect 相关的用例，用回读做机器判定 | §11.1 |
| 3 | 验证 **UAV → SRV 切换**（`uavBarrier`）在图形管线里真的可用：CS 写的结构化缓冲被 VS 读到 | §6.3、§6.7 |
| 4 | 验证 **GPU-driven 闭环**：`copyStructureCount` → `renderIndexedIndirect`，计数不经过 CPU | §6.7 |
| 5 | 暴露跨后端差异并记账：哪些用例 GL 侧静默降级、哪些绑定号对不上 | §8.2、§9.2 |
| 6 | 给后续 `ComputeShader` 资产类型（Material 层）提供一个「手写 RHI 版本」的参照实现 | §1.2 边界 |

### 1.2 非目标

- **不做 Material / Pass 层的 compute 支持。** `Pass::addShaderVariant` 对 `SHADER_STAGE::kCompute` 直接返回 `T3D_ERR_NOT_IMPLEMENT`（`T3DPass.cpp:359-362`），本 Sample **绕开 Pass**，直接 `ShaderVariant::create` + `compile()` + `RHIContext` 手工绑定。这条是本 Sample 全部架构决策的源头，见 §3.1。
- **不做 RenderGraph / compute pass 调度。** 所有 GPU 命令在 `Application::onRender` 里线性录制。
- **不改任何 Core / RHI / 后端代码。** 如果实施过程中发现后端有 bug，记账到 RHI-Compute 文档 §11，单独修，不混进本 Sample 的提交。唯一例外是 `embed-sample-shaders.ps1` 的 `$jobs` 扩展（§4.3）。
- **不补 Vulkan / Metal 的 compute 实现。** 那是 RHI-Compute 文档五期的 E1 / E3。
- **第一期不做 Android。** 移植参照 `PostProcessingApp-Android-Design-todo.md`，列为 S5。
- **不追求粒子系统的物理正确性或美观。** 可视轨的唯一职责是「让人一眼看出 GPU 在算」，不是做粒子引擎。
- **不用 compute 蒙皮当可视轨。** 评估过拿 Unity 式的 compute skinning（独立蒙皮 pass 把结果写进 GPU 缓冲，各 pass 复用）替换粒子，结论是**暂缓、另行立项**。三条理由：覆盖面上它给不了 groupshared / 原子 / indirect / `copyStructureCount` / 纹理 UAV，K4 / K5 / K7 / K8 一个都省不掉，它能替换的只有 K9 / K10；算错时嫌疑人太多（矩阵行列主序、骨骼矩阵乘序、权重归一化、顶点 stride 解码），与「验证 Sample 必须隔离被测对象」相冲；`AnimationPlayerMgr::update()` 在 CPU tick 上做蒙皮，而 compute 只能在 `onRender` 里录制，Sample 只能并列一条路而非接进引擎。

  立项时可以直接复用的三条结论，记在这里免得重新讨论：① `AnimationPlayer::CPUSkinning()` 是引擎自带的 oracle，可逐顶点对拍，不必另写参考实现；② `SkeletalAnimationApp` 的手臂网格与骨架全部程序化生成，无资产依赖，隔离性比想象中好；③ 分三步放开变量能把隔离性找回来 —— 先让全部骨骼矩阵为单位阵（输出须与输入**逐字节相等**，此步只测绑定号 / stride 解码 / `uavBarrier` / vertex pulling，完全不碰蒙皮数学），再放开单骨骼纯旋转比对手算值，最后才跟 `CPUSkinning()` 对拍完整骨架。另注意它在当前 RHI 下**不是 Unity 的复刻**：输入端顶点缓冲不能当 CS 的 SRV、输出端 GL 不能把顶点缓冲当 UAV 写（§2.2），所以输入输出都得走 StructuredBuffer，VS 侧做 vertex pulling。这两条都是接口类型闸门而非能力缺失，RHI-Compute 文档 §12.8 已给出 1.5–2 人日的解法；若那笔改动先落地，蒙皮立项时就能直接读写网格自己的顶点缓冲，不必复制副本。

---

## 2. 现状：Sample 要建在什么地基上

### 2.1 可用的 RHI 能力（已落地，D3D11 为准）

```544:650:source\Core\Include\RHI\T3DRHIContext.h
        virtual RHIShaderPtr createComputeShader(ShaderVariant *shader) = 0;
        virtual TResult setComputeShader(ShaderVariant *shader) = 0;
        virtual TResult setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) = 0;
        virtual TResult setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) = 0;
        virtual TResult setCSStructuredBuffers(uint32_t startSlot, const StructuredBuffers &buffers) = 0;
        virtual TResult setCSUnorderedAccessBuffers(uint32_t startSlot,
            const UnorderedAccessBuffers &buffers,
            const UAVInitialCounts &initialCounts = UAVInitialCounts()) = 0;
        virtual TResult dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;
        virtual TResult dispatchIndirect(RenderBuffer *argsBuffer, size_t argsOffset = 0) = 0;
        virtual TResult uavBarrier(const UnorderedAccessBuffers &buffers) = 0;
        virtual TResult copyStructureCount(RenderBuffer *dstBuffer, size_t dstOffset,
            RenderBuffer *srcBuffer) = 0;
```

配套的：`setVSStructuredBuffers` / `setPSStructuredBuffers`（SRV 路径）、`renderInstanced` / `renderIndexedInstanced` / `renderIndirect` / `renderIndexedIndirect`、`map` / `unmap`（读回）、`getCapabilities()`。

### 2.2 三个后端的实际能力对照

这张表决定每个用例能在哪些后端跑，是 §5 用例矩阵的输入。

| 能力 | D3D11 Window | GL4 Window | GLES3 Runtime |
|------|-------------|-----------|--------------|
| `dispatch` / `dispatchIndirect` | ✅ | ✅ (4.3+) | ✅ (3.1+) |
| 结构化缓冲 UAV（`RWStructuredBuffer`） | ✅ | ✅ SSBO | ✅ SSBO |
| ByteAddress / Typed 缓冲 | ✅ | ⚠️ 靠 SPIRV-Cross 折成 SSBO，需实测 | ⚠️ 同左 |
| **纹理 UAV（`RWTexture2D`）** | ✅ | ❌ **静默失效** | ❌ **静默失效** |
| Append / Consume + `copyStructureCount` | ✅ | ✅ atomic counter | ✅ |
| `set{VS,PS,CS}StructuredBuffers` | ✅ | ✅ | ✅ |
| `renderIndirect` / `renderIndexedIndirect` | ✅ | ✅ (4.0+) | ✅ |
| `map` / `unmap` 读回 | ✅ | ✅ | ✅（仅 2D 彩色） |

**「静默失效」这一条是本 Sample 最需要提前知道的坑。** GL4 的 UAV 绑定只认结构化缓冲，其它类型的资源直接落到 `ssbo = 0`，然后 `glBindBufferBase(..., 0)` 解绑，**不报错也不返回失败**：

```3259:3270:source\Plugins\Renderer\OpenGL4\Window\Source\T3DGL4Context.cpp
            RenderBuffer *rb = buffers[i].get();
            if (rb != nullptr && rb->getRHIResource() != nullptr
                && rb->getRHIResource()->getResourceType() == RHIResource::ResourceType::kStructuredBuffer)
            {
                GL4StructuredBuffer *glSB = static_cast<GL4StructuredBuffer*>(rb->getRHIResource().get());
                b.ssbo = glSB->GLBuffer;
                b.counter = glSB->GLCounterBuffer;
                b.hasCounter = glSB->HasCounter;
            }

            bindings.push_back(b);
```

全仓库搜不到 `glBindImageTexture`。所以 **K5（纹理 UAV）在 GL 上不会报错，只会算出错误结果**。Sample 必须在用例元数据里显式标注「仅 D3D11」，靠后端名字跳过，**不能指望返回值**。这一条要同步回填到 RHI-Compute 文档 §8.2。

> 这个静默分支有两半，成因不同。**顶点缓冲那一半是接口类型闸门**，RHI-Compute 文档 §12.8.2 已给出解法（SRV 路径按资源类型分发时，UAV 路径复用同一套逻辑一并修好），修完顶点缓冲当 UAV 写在 GL 上就能工作。**纹理那一半是真缺实现**，要补 `glBindImageTexture`，属于 E2 的范围。本 Sample 两半都不修，只是靠 `backendMask` 绕开。

### 2.3 Shader 工具链现状

- ShaderLab 侧 compute 全通：`kProgramCompute`（`SLParserTypes.h:125-133`）、`#pragma compute <entry>`（`T3DShaderCompiler.cpp:181-184`）、`kComputeShader` 映射到 ShaderConductor 的 `ComputeShader`（同文件 `:234`）。
- D3D11 反射已覆盖全部 UAV 类型与 `numthreads`（`T3DD3D11ContextBase.cpp:334-384`），profile 为 `cs_5_0`。
- Sample **不在运行时读 `.cshader`**，走「scc 离线跨编译 → PowerShell 嵌入 `SampleShaders_*.h` → C++ 常量」这条既有链路（`embed-sample-shaders.ps1` 头部注释）。

---

## 3. 架构决策

### 3.1 为什么整条渲染不走 Scene / Material

三个硬约束叠加，逼出「手工 RHI 录制」这个唯一解：

1. `Pass` 拒绝 compute variant，所以 compute shader 无法挂进 `Material`。
2. `ForwardRenderPipeline` 只有 graphics draw 路径，没有 compute pass 插入点，也不会帮你调 `setVSStructuredBuffers`。
3. UAV → SRV 的切换时机（`uavBarrier`）必须由调用方精确控制，而管线内部的绑定顺序对 Sample 不可见。

因此：

```
Scene / Camera          只负责：清屏、提供 view / proj 矩阵、（可选）画一个参照立方体
Application::onRender   负责：全部 compute 派发 + 粒子的手工绘制录制
Application::onPostRender 负责：回读比对 + 断言
```

`onRender` 是 GPU-Readback 文档确立的**唯一合法命令录制点**（在 `beginRender` / `endRender` 之间）。

> **开放问题 O1**：`onRender` 被调用时，当前绑定的 RenderTarget、viewport、深度状态是相机管线 `render()` 结束后的残留值。手工绘制粒子之前**是否需要重新设一遍 RT / viewport**，要在 S2 动手时用 RenderDoc 确认。若需要，Sample 自己调 `setRenderTarget` / `setViewport`，不要去改管线。

### 3.2 Compute shader 对象怎么创建

绕开 `Pass`，直接用 `ShaderVariant`。它的 `compile()` 已经能分发到 `createComputeShader`：

```113:136:source\Core\Source\Material\T3DShaderVariant.cpp
            switch (mShaderStage)
            {
            case SHADER_STAGE::kVertex:
                mRHIShader = ctx->createVertexShader(this);
                break;
            case SHADER_STAGE::kPixel:
                mRHIShader = ctx->createPixelShader(this);
                break;
            case SHADER_STAGE::kCompute:
                mRHIShader = ctx->createComputeShader(this);
                break;
```

Sample 侧的封装（建议放 `ComputeKernel.h/.cpp`，只进 ComputeApp 工程）：

```cpp
/// 一个 compute 内核：持有 ShaderVariant、反射出的线程组尺寸，负责换算组数
class ComputeKernel
{
public:
    /// 按当前 active 后端挑源码、创建变体、编译并反射线程组尺寸
    static ComputeKernelPtr create(const char *name, const ShaderSourceSet &sources);

    /// 把「线程总数」换算成「线程组数」并派发，不足一组向上取整
    TResult dispatchThreads(uint32_t threadsX, uint32_t threadsY = 1, uint32_t threadsZ = 1);

    ShaderVariant *getVariant() const { return mVariant; }

private:
    ShaderVariantPtr mVariant;
    uint32_t         mGroupSize[3] {1, 1, 1};   ///< 来自 reflect() 的 numthreads
};
```

`dispatchThreads` 用反射出来的 `numthreads` 做换算，而不是让调用点硬编码 `(N + 63) / 64`。这样 shader 改了 `numthreads` 不用同步改 C++，同时顺带验证了 C7（线程组尺寸反射）。

`ShaderSourceSet` 是个按后端选源码的小结构，沿用 `PostProcessingApp` 已有的写法（`rendererName == RHIRenderer::OPENGL4` 之类的分支），不发明新机制。

### 3.3 结构化缓冲怎么建

走 `RenderResourceManager::loadStructuredBuffer`。UAV 资源的硬约束：**必须 `Usage::kStatic` + `kGPUUnorderedAccess`**（`RenderBuffer::validateGPUAccess` 会校验）。需要回读的缓冲额外带 `kCPURead`。

```cpp
StructuredBufferDesc desc;
desc.kind         = StructuredBufferKind::kStructured;
desc.elementSize  = sizeof(uint32_t);
desc.elementCount = kElemCount;
desc.hasCounter   = false;

mOutBuffer = T3D_RENDER_RES_MGR.loadStructuredBuffer(
    desc, MemoryType::kGPU, Usage::kStatic, kCPURead,
    kGPUUnorderedAccess | kGPUShaderResource);
```

`kGPUShaderResource` 一并带上，是因为 K9 之后同一块缓冲要在 `uavBarrier` 之后当 SRV 被 VS 读。

### 3.4 用例调度：一个 `ComputeCase` 表

对齐 `PostProcessingApp` 的预设机制，但用例元数据里多两个字段：

```cpp
struct ComputeCase
{
    const char *name;             ///< "K1 linear write"
    uint32_t    requiredCaps;     ///< 位掩码：kNeedCompute / kNeedUAV / kNeedIndirect / kNeedReadback
    uint32_t    backendMask;      ///< 位掩码：kD3D11 / kGL4 / kGLES3；用于绕开 §2.2 的静默失效
    TResult   (*record)(ComputeApp &);    ///< 在 onRender 里录制
    TResult   (*verify)(ComputeApp &);    ///< 在 onPostRender 里回读比对
};
```

`backendMask` 不是冗余：`requiredCaps` 能挡住 `supportsCompute == false` 的后端，但挡不住「能力位说支持、实现却静默 no-op」的纹理 UAV。两个字段各管一类问题，注释里要写清楚这个区别，否则后人一定会合并掉它。

---

## 4. Shader 资产与工具链

### 4.1 文件清单

新增到 `assets/samples/shaders/`，扩展名 `.cshader`（scc 靠 `#pragma compute` 识别阶段，扩展名只是给人看的约定）：

| 文件 | 入口 | numthreads | 用途 |
|------|------|-----------|------|
| `ComputeLinearWrite.cshader` | `CSMain` | (64,1,1) | K1 |
| `ComputeSaxpy.cshader` | `CSMain` | (64,1,1) | K2：SRV + CB + UAV 三者共存 |
| `ComputeRawTyped.cshader` | `CSMain` | (64,1,1) | K3：ByteAddress + Typed |
| `ComputeReducePass0.cshader` | `CSMain` | (256,1,1) | K4：groupshared 组内归约 |
| `ComputeReducePass1.cshader` | `CSMain` | (256,1,1) | K4：partial 二级归约 |
| `ComputeTextureWrite.cshader` | `CSMain` | (8,8,1) | K5：`RWTexture2D`，D3D11 only |
| `ComputeReflectProbe.cshader` | `CSMain` | (8,8,1) | K6：只反射不派发，含全部资源类型 |
| `ComputeMakeArgs.cshader` | `CSMain` | (1,1,1) | K7：写 `DispatchIndirectArgs` |
| `ComputeParticleUpdate.cshader` | `CSMain` | (256,1,1) | K9 可视轨：积分 |
| `ComputeParticleCull.cshader` | `CSMain` | (256,1,1) | K10：视锥剔除 + `InterlockedAdd` |
| `ParticleDraw.vshader` / `.pshader` | `vert` / `frag` | — | K9 可视轨：VS 从 SRV 取粒子状态 |

### 4.2 Shader 写法约定

```hlsl
#pragma compute CSMain
#pragma target 5.0

struct Particle
{
    float3 position;
    float  life;
    float3 velocity;
    float  size;
};

cbuffer ParticleParams : register(b0)
{
    float4 gTimeParams;      // x=dt, y=time, zw=保留
    float4 gAttractor;       // xyz=吸引子, w=强度
};

RWStructuredBuffer<Particle> gParticles : register(u0);

[numthreads(256, 1, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    // ...
}
```

三条硬约定：

1. **寄存器显式写死**。`t#` 给 SRV、`u#` 给 UAV、`b#` 给 CB，和 `setCS*Buffers` 的 `startSlot` 一一对应。不要依赖编译器自动分配。
2. **不用 `AppendStructuredBuffer` / `ConsumeStructuredBuffer`**。RHI-Compute 文档 §9.2 第 4 条的结论：SPIRV-Cross 会把它折成「SSBO + 原子计数器」的组合，binding 数量与 HLSL 不是 1:1，是跨后端一致性最脆弱的一环。K10 改用 `RWStructuredBuffer` + 显式 `InterlockedAdd` 手动维护计数，配合 `hasCounter = true` 的缓冲走 `copyStructureCount`。
3. **结构体字段按 16 字节对齐手工排布**。上面的 `Particle` 刻意把 `life` / `size` 塞进 `float3` 后面凑满两个 `float4`，避免 HLSL 与 GLSL std430 的 padding 规则差异。C++ 侧的镜像结构体加 `static_assert(sizeof(Particle) == 32)`。

### 4.3 跨编译与嵌入

每个 shader 跑四次 scc（`-t` 只能给一个值，四个后端无法合并）：

```bash
scc ComputeParticleUpdate.cshader -t hlsl  -o <output>\HLSL      -N -O0
scc ComputeParticleUpdate.cshader -t glsl  -o <output>\OpenGL4   -N -O0
scc ComputeParticleUpdate.cshader -t essl  -o <output>\OpenGLES3 -N -O0
scc ComputeParticleUpdate.cshader -t spirv -o <output>\Vulkan    -N -O0
```

产物落在 `assets/samples/shaders/output/<Backend>/<Name>_compute.<ext>`（该目录 gitignored）。SPIR-V 变体照常生成，让 `SampleShaders_vk.h` 保持形状完整，但 Sample 运行时不会用它（Vulkan 是 stub）。

然后扩 `embed-sample-shaders.ps1` 的 `$jobs`：

```powershell
[PSCustomObject]@{ Header = "SampleShaders_hlsl.h";  Const = "COMPUTE_PARTICLE_UPDATE";      Src = "HLSL\ComputeParticleUpdate_compute.hlsl";      Kind = "text"  }
[PSCustomObject]@{ Header = "SampleShaders_gl4.h";   Const = "COMPUTE_PARTICLE_UPDATE_GL";   Src = "OpenGL4\ComputeParticleUpdate_compute.glsl";   Kind = "text"  }
[PSCustomObject]@{ Header = "SampleShaders_gles3.h"; Const = "COMPUTE_PARTICLE_UPDATE_GLES"; Src = "OpenGLES3\ComputeParticleUpdate_compute.essl"; Kind = "text"  }
[PSCustomObject]@{ Header = "SampleShaders_vk.h";    Const = "COMPUTE_PARTICLE_UPDATE_VK";   Src = "Vulkan\ComputeParticleUpdate_compute.spirv";   Kind = "spirv" }
```

脚本的 `-Verify` 模式是这套嵌入格式的回归测试，扩完 `$jobs` 后要能对全部 header 报 MATCH。

### 4.4 头号风险：UAV 绑定号跨编译对不上

**这是整个 Sample 最可能翻车的地方，必须在 S1 第一天验证，其余所有工作都排在它后面。**

D3D11 侧 `register(u0)` 直接就是 `CSSetUnorderedAccessViews` 的 slot 0。GL 侧则是 `glBindBufferBase(GL_SHADER_STORAGE_BUFFER, startSlot + i, ssbo)`（`T3DGL4Context.cpp:3281`），用的是**调用方传的 slot**，而 GLSL 里的 binding 号由 SPIRV-Cross 生成。HLSL 的 `t#` 与 `u#` 是两个独立编号空间，SPIR-V 的 descriptor binding 是一个统一空间 —— 两边对不上是默认结局，对上才是运气。

验证方法很直接：K1 跑通 D3D11 之后，打开 `output/OpenGL4/ComputeLinearWrite_compute.glsl`，肉眼看 SSBO 块上的 `layout(binding = N)` 到底是几。三种可能与对策：

| 观察结果 | 对策 |
|---------|------|
| `binding = 0`，与 `u0` 一致 | 最好的情况，什么都不用做 |
| 编号有固定偏移（如 SRV 占了前几个号） | Sample 侧按后端算 `startSlot`，并把规律写进本文与 RHI-Compute 文档 §9.3 |
| 编号无规律 | 停下来，这是后端 bug 不是 Sample 问题。记账到 RHI-Compute 文档 §11，S1 先只交付 D3D11 |

配套手段：scc 加 `-N -O0` 已经保留了可读的变量名，`RenderDoc` 的 pipeline state 里能直接看到实际绑定；GL 侧也可以临时用 `glGetProgramResourceIndex` 打印一遍。

---

## 5. 用例设计

### 5.1 自检轨用例矩阵

全部在启动后的**第一帧**跑完，`record` 在 `onRender`、`verify` 在 `onPostRender`。跑完置 `mSelfTestDone = true`，之后每帧只跑可视轨。按 `R` 可重跑。

| 用例 | 验证点（对应 RHI-Compute §11.1） | 数据规模 | 期望结果 | 后端 |
|------|--------------------------------|---------|---------|------|
| **K0** | 能力探测 | — | 打印 `getCapabilities()` 全字段；`supportsCompute == false` 时后续全跳过 | 全部 |
| **K1** | C1/C2 `dispatch` + `RWStructuredBuffer` | 1024 × uint | `out[i] == i * 2` | 三个 |
| **K2** | C4 SRV + CB + UAV 共存 | 1024 × float4 | `out[i] == in[i] * gScale + gBias`，逐分量容差 1e-5 | 三个 |
| **K3** | B4~B7 三种缓冲形态 | 1024 × uint | ByteAddress：`raw[i] == i ^ 0xA5A5`；Typed：`typed[i] == float4(i,i,i,i)/1024` | D3D11 必过，GL 实测后定 |
| **K4** | C3 `uavBarrier` + `groupshared` + 多次 dispatch | 65536 × uint | 两级归约求和 `0..65535` == `2147450880` | 三个 |
| **K5** | B8 纹理 UAV | 256×256 RGBA8 | `px(x,y) == (x, y, 128, 255)` | **仅 D3D11** |
| **K6** | C6/C7 反射 | — | 逐条比对 `ShaderResourceParam::Kind` / `binding` / `elementStride`；`numthreads` 反射为 (8,8,1) | 仅 D3D11 |
| **K7** | D4 `dispatchIndirect` | args{4,1,1} | 恰好前 256 个元素被写，第 257 个仍是哨兵 `0xDEADBEEF` | D3D11 + GL4 |
| **K8** | D2 `copyStructureCount` | 1024 输入 | 过滤出的元素数 M 被搬进 args buffer 偏移 4 处，回读该 uint == M | D3D11 + GL4 |

**K4 的设计意图**值得单独说：它是唯一一个需要「dispatch → barrier → dispatch」的自检用例。第一级每组 256 线程用 `groupshared` 归约出 partial，写进 `partialBuf`；`uavBarrier({partialBuf})` 之后第二级把 256 个 partial 再归约成 1 个。如果 barrier 是空操作或者时序错了，结果会是不确定的偏小值 —— 这比 K1 那种「单次 dispatch 写完就读」的用例敏感得多。期望值 `65535 * 65536 / 2 = 2147450880`，在 uint32 范围内，不会溢出。

**K3 的态度**：ByteAddress / Typed 在 GL 上靠 SPIRV-Cross 折成 SSBO，行为未经验证。策略是「D3D11 必须过，GL 跑一遍看结果，不过就记账、标 `backendMask` 跳过，不改后端」。这条用例的真正价值是**把跨编译差异测出来并写进文档**，而不是强求三端一致。

### 5.2 可视轨设计

**K9 — GPU 粒子积分（S2 交付）**

- 65536 个粒子，`Particle` 结构见 §4.2，存在 `kGPUUnorderedAccess | kGPUShaderResource` 的结构化缓冲里，初值由 CPU 在初始化时写入（随机球面分布）。
- 每帧 `onRender`：
  1. 更新 CB（`dt`、`time`、吸引子位置沿圆周运动）
  2. `setComputeShader(particleUpdate)` → `setCSConstantBuffers(0, {cb})` → `setCSUnorderedAccessBuffers(0, {particleBuf})` → `dispatchThreads(65536)`
  3. `uavBarrier({particleBuf})` + `setComputeShader(nullptr)` —— **解绑是必须的**，否则 D3D11 会静默解掉 SRV 绑定（`T3DRHIContext.h:597-598`）
  4. `setVSStructuredBuffers(0, {particleBuf})`，VS 用 `SV_InstanceID` 索引取粒子状态，`renderInstanced` 画 6 顶点的 billboard quad，65536 个实例
- 按 `C` 切 CPU 参考模式：同样的积分在 CPU 算完 `writeBuffer` 上传，画面应当看不出区别。**这是可视轨的判定依据** —— 不是「好不好看」，是「GPU 和 CPU 算出来一样不一样」。

**K10 — GPU 剔除 + 间接绘制（S4 交付）**

RHI-Compute 文档 §6.7 那段序列的实际执行，改用 `InterlockedAdd` 而非 Append（§4.2 约定 2）：

```cpp
// 1. 剔除：可见粒子索引写进 visibleBuf，计数走 InterlockedAdd
ctx->setComputeShader(cullKernel->getVariant());
ctx->setCSConstantBuffers(0, {frustumCB});
ctx->setCSUnorderedAccessBuffers(0, {particleBuf, visibleBuf}, {kKeepUAVCounter, 0});
cullKernel->dispatchThreads(kParticleCount);

// 2. 计数直接落进 DrawIndexedIndirectArgs::instanceCount，不经过 CPU
ctx->copyStructureCount(drawArgsBuf, offsetof(DrawIndexedIndirectArgs, instanceCount), visibleBuf);

// 3. 解绑，让两块缓冲能当 SRV 用
ctx->uavBarrier({particleBuf, visibleBuf, drawArgsBuf});
ctx->setComputeShader(nullptr);

// 4. 间接绘制
ctx->setVSStructuredBuffers(0, {particleBuf, visibleBuf});
ctx->renderIndexedIndirect(drawArgsBuf, 0);
```

判定方式有三层，从弱到强：

1. 人眼：把剔除用的视锥人为缩小到相机视锥的 60%，转动相机时边缘粒子成片消失
2. 日志：同帧额外把 counter 回读一份打印出来，可见数随相机转动变化且不等于总数
3. 机器：同一帧再跑一遍 CPU 剔除，比对可见集合的**数量**（集合本身顺序不定，不比对内容）

### 5.3 交互

Desktop 键位，沿用 `PostProcessingApp` 的输入解耦写法（命令源 poll，不在 `Behaviour::onRender` 里做事）：

| 键 | 行为 |
|---|------|
| `R` | 重跑全部自检用例，日志重新打一遍 |
| `1`–`9` | 单独重跑某个自检用例（调试用） |
| `C` | 可视轨 GPU / CPU 模式切换 |
| `V` | 开关 GPU 剔除 + 间接绘制 |
| `P` | 暂停积分（冻结粒子，方便 RenderDoc 抓帧） |

**热键不响应先查输入法**：中文输入法开着时按键被 IME 截走，SDL 收不到 scancode，`getKeyDown` 自然一直是 false。切英文输入即可。同理，`keybd_event` / `SendKeys` 这类合成按键也进不去，热键没法用脚本自动化验证，只能手测。

---

## 6. 文件与构建改动清单

### 6.1 新增

```
source/Samples/ComputeApp/
├── ComputeApp.h / .cpp              # SampleWindowApp 派生，三个帧钩子 + 用例表
├── ComputeKernel.h / .cpp           # §3.2 的内核封装
├── ComputeCases.h / .cpp            # K0–K8 的 record / verify 实现
├── ParticleSystem.h / .cpp          # K9 / K10 可视轨
├── ComputeShaderSources.h / .cpp    # 按后端选源码（仿 PostProcessShaderSources.cpp）
├── ComputeControllerBehaviour.h/.cpp# 热键，需要 tiny3d_enable_reflection
└── CMakeLists.txt                   # 复制 PostProcessingApp 的，改 set_project_name

assets/samples/shaders/Compute*.cshader   # §4.1 的 10 个
assets/samples/shaders/ParticleDraw.{vshader,pshader}
```

### 6.2 修改

| 文件 | 改动 |
|------|------|
| `source/Samples/CMakeLists.txt` | `add_subdirectory(ComputeApp)` |
| `source/CMakeLists.txt` | `TINY3D_BUILD_SAMPLES` 块内 `add_dependencies(ComputeApp T3DCore)` |
| `source/Samples/Common/SampleShaders.h` | 声明 `COMPUTE_*` / `COMPUTE_*_GL` / `COMPUTE_*_GLES` / `COMPUTE_*_VK` 常量 |
| `source/Samples/Common/SampleShaders_{hlsl,gl4,gles3,vk}.h` | 嵌入产物（脚本生成，不手改） |
| `source/Tools/SampleShaderEmbedder/embed-sample-shaders.ps1` | 扩 `$jobs`（§4.3） |

**不修改**：任何 `source/Core`、`source/Plugins/Renderer` 下的文件。ComputeApp 自带的 Behaviour 与 PostProcessingApp 同一套规矩 —— 只进本 target，类名不加 `T3D` 前缀，不进 T3DCore 的 CMake。

---

## 7. 分期

每期结束都是可编译、可运行、可回滚的状态。

| 期 | 内容 | 预估 | 依赖 | 交付判定 |
|----|------|------|------|---------|
| **S0** | 脚手架：目录、CMake 两处注册、空 `ComputeApp` 能起窗口、K0 打印能力位 | 0.5d | 无 | 窗口起来，日志打出 `supportsCompute=1` |
| **S1a** | **绑定号验证**：只做 K1，只跑 D3D11，跑通后立刻看 GL / ESSL 产物的 binding 号（§4.4） | 1d | S0 | D3D11 下 K1 passed；GL binding 号规律写进本文 |
| **S1b** | K2 / K3 / K4 + 三后端跑通（K3 按实测定 `backendMask`） | 1.5d | S1a | 三后端 `K1..K4 all passed`，GL 若有不过的记账 |
| **S2** | 可视轨 K9：粒子积分 + 手工 `renderInstanced` + CPU 对照模式 | 2d | S1b、O1 已确认 | 粒子在动；按 `C` 切 CPU 画面无差异 |
| **S3** | K5 纹理 UAV + K6 反射，D3D11 only | 1d | S1b | D3D11 下两例 passed，GL 正确跳过 |
| **S4** | K7 / K8 + K10 GPU 剔除间接绘制 | 2d | S2、S3 | `copyStructureCount` 回读值正确；按 `V` 边缘粒子消失 |
| **S5** | Android 工程（GLES3 真机） | 1.5d | S2 | 真机跑通自检轨 + 可视轨，参照 `PostProcessingApp-Android-Design-todo.md` |

**合计约 9.5 人日**，其中 S0–S2 是主线（5 人日），S3–S5 可按优先级插空。

推荐顺序说明：**S1a 必须最先做且不可跳过**。绑定号对不上的话，S1b 之后的所有跨后端工作都要返工；而如果它在第一天就暴露，代价只是「S1 收缩成 D3D11 only」，后面的期不受影响。

---

## 8. 验证与回归

### 8.1 每期都要过的三条

沿用 RHI-Compute 文档 §11.2：

1. **D3D11 debug layer 必开**（`T3D_DEBUG` 下 `init()` 已加 `D3D11_CREATE_DEVICE_DEBUG`）。跑完一轮自检 + 30 秒可视轨，output 窗口不得有任何 D3D11 ERROR / WARNING。UAV 与 SRV 同时绑同一资源、忘记 `uavBarrier` 都会在这里以 HAZARD 的形式暴露。
2. **RenderDoc 抓帧核对**（项目已配好 RenderDoc MCP）。要看的：`Dispatch` 事件的 CS 阶段 u0/u1 槽有正确绑定；`DrawIndexedIndirect` 事件里 RenderDoc 解析出的参数缓冲内容与预期一致；粒子 draw 的 `InstanceCount` 是 65536。
3. **退出时 `ReportLiveDeviceObjects`**，确认没有 UAV 视图残留。判定时别只看 UAV：拿一个不做手工绘制的 Sample（如 TextureApp）做对照，它退出时只剩 `Live ID3D11Device` 一条，多出来的每一条都要有解释。P10 就是这么发现的。

### 8.2 两种 RHI 线程模式都要过

RHI-Compute 文档 §11.3 点名的：单线程模式下 `ENQUEUE_UNIQUE_COMMAND` 同步执行、返回真实错误码、CPU 数据指针必然有效，会**掩盖**一整类问题。多线程模式下返回值恒 `T3D_OK`、lambda 延后执行，Sample 自己持有的 `StructuredBufferPtr` 生命周期、CB 更新与 dispatch 的顺序才会真正受检验。

**自检轨的断言在多线程模式下尤其重要**：`map` 录进命令流、`unmap` 阻塞等待，跨帧时序如果错了，回读到的会是上一帧的数据 —— 而 K1 那种「结果与帧无关」的用例发现不了，K9 的 CPU 对照模式能发现。

### 8.3 不做的验证

- 不做性能测试。粒子数固定 65536，不做 GPU 耗时对比，不调优。
- 不做像素级 CI。可视轨靠人眼 + CPU 对照，自检轨的回读断言已经是机器判定，够了。
- 不验证 Vulkan / Metal。它们的 compute 是 stub，Sample 在这两个后端上只需要「不崩、打 warning、退化到 CPU 粒子」。

---

## 9. 风险与开放问题

| # | 风险 | 影响 | 应对 |
|---|------|------|------|
| **R1** | UAV 绑定号跨编译对不上（§4.4） | GL / GLES3 全线不可用 | S1a 第一天验证；最坏情况 S1 收缩成 D3D11 only，其余期不受影响 |
| **R2** | `onRender` 里手工绘制需要重设 RT / viewport（开放问题 O1） | 粒子画不出来或画错位置 | S2 动手前用 RenderDoc 看一眼 `onRender` 入口时的管线状态 |
| **R3** | GL 侧纹理 UAV 静默失效（§2.2） | K5 在 GL 上算出错误结果却不报错 | `backendMask` 硬编码跳过，不依赖返回值；同步回填 RHI-Compute 文档 §8.2 |
| **R4** | `groupshared` 大小在移动 GPU 上只有 16KB | K4 在 GLES3 真机上可能编译失败 | K4 的 `groupshared` 数组控制在 256 × 4B = 1KB；同时在 K0 打印 `maxComputeSharedMemory` |
| **R5** | 结构体布局 HLSL / std430 不一致 | 粒子数据错乱，表现为乱飞 | §4.2 约定 3 手工对齐 + `static_assert`；K2 的 float4 比对能提前发现 |
| **R6** | `renderInstanced` + 无顶点缓冲的 billboard 画法各后端支持度不一 | 可视轨在 GL 上画不出 | VS 不用顶点属性、全靠 `SV_VertexID` / `SV_InstanceID` 索引；若某后端不支持无 VB 绘制，退化为绑一个 6 顶点的 dummy VB |
| **R7** | Sample 数量膨胀，CMake 样板重复 | 维护成本 | 本 Sample 照抄现有样板，**不顺手引入 `add_sample()` 宏**。那是独立的重构课题，混进来会让本 Sample 的改动难以评审 |

### 9.1 落地实测：D3D11 上真正踩到的坑

按发生顺序记，都是 Sample 侧解决的，没有改 Core / RHI / 后端。

| # | 现象 | 根因 | 处置 |
|---|------|------|------|
| P1 | 反射阶段 `std::out_of_range` 崩溃 | D3D11 反射的 `getOriginalName` 按 SPIRV-Cross 跨编译产物的命名硬剥前缀（`T3DD3D11ContextBase.cpp:150-177`），手写 HLSL 的 `cbuffer Foo { float4 gBar; }` 不合规就 `substr` 越界 | HLSL 的 cbuffer 一律写成 `cbuffer type_Foo { float4 Foo_gBar; }`；GL / GLES 的反射会容错，保持自然命名 |
| P2 | D3D11 编译 compute 时空指针解引用 | 两个用例共用内核名时 `ShaderManager::createShader` 命中缓存，`Shader::addTechnique` 拒绝重名 Technique 且不回填 shader 指针，后端沿 `Pass → Technique → Shader` 取名就炸 | dummy Pass 的 Shader / Technique / Pass 名加全局自增后缀，挂不上直接判失败不往下编 |
| P3 | 结构化缓冲创建返回 `E_INVALIDARG` | `MISC_BUFFER_STRUCTURED` 与 `MISC_DRAWINDIRECT_ARGS` 在 D3D11 互斥 | 间接参数缓冲改 `StructuredBufferKind::kByteAddress`，shader 侧对应 `RWByteAddressBuffer` |
| P4 | `CreateInputLayout` 返回 `E_INVALIDARG` | GPU 拉流路径的 VS 没有顶点属性，给了 0 元素的 VertexDeclaration | 该路径先改成不建也不设 VertexDeclaration —— 这个处置后来被 P9 推翻了 |
| P5 | `DEVICE_VSSETSHADERRESOURCES_HAZARD` 刷屏，粒子不显示 | `uavBarrier` 只是解绑再绑回来刷缓存，UAV 槽位仍占着资源，D3D11 于是把 VS 的 SRV 强制置 NULL | dispatch 之后显式把用到的 CS UAV 槽写 nullptr，再去绑 SRV |
| P6 | **画面全黑但 clear 生效、debug layer 一条错都没有** | 管线渲染的是离屏 RT，最后只把颜色 blit 回后台缓冲，**窗口自己的深度缓冲一帧都没被清过**，内容全 0；深度函数 Less 拿 0 当参考值，整片粒子被判 `depth_test_failed` | 手工绘制前自己 `clearDepthStencil(1.0f, 0)`。这正是 R2 的具体形态：`onRender` 里手工绘制，RT / viewport / **深度缓冲**都得自己负责 |
| P7 | 关窗退出时在 `ScopeLock` 构造里访问违例 | `theApp` 是全局对象，析构在 `main()` 返回之后；那时 Agent 连同 RenderStateManager、资源用的 SyncObject 都已销毁，Sample 还攥着的 `BlendStatePtr` / `StructuredBufferPtr` 这时才 release，等于往已析构的锁上加锁 | `ParticleSystem::teardown()` 在 `applicationWillTerminate()` 里把所有 GPU 资源引用显式断掉（顺带先收掉还挂着的回读）。**Sample 持有的任何引擎资源智能指针都不能活过 `applicationWillTerminate`** |
| P8 | 每帧稳定刷 `CSSETUNORDEREDACCESSVIEWS_HAZARD` + 强制把 VS / CS 的 SRV 置 NULL | P5 只解了「本帧 dispatch 完再绑 SRV」，没管跨帧：上一帧绘制留下的 VS SRV（粒子 / 可见列表）、剔除内核留下的 CS SRV（粒子）都还占着槽位，下一帧把同一块缓冲当 UAV 绑回去就撞 hazard | 谁绑谁收：绘制结束清 VS SRV 槽 0~1，剔除 dispatch 结束清 CS SRV 槽 0。**UAV 和 SRV 的解绑要对称到帧边界，不能只在一帧内自洽** |
| P9 | 剔除 + 间接绘制模式下 `DrawIndexedInstancedIndirect` 报 `DEVICE_SHADER_LINKAGE_REGISTERINDEX`，说 `SV_VertexID` 的硬件寄存器在 IA 与 VS 之间对不上，整个 draw 被丢弃 | P4 的「不设 VertexDeclaration」意味着 IA 上留着**别人**的 input layout（管线 blit 的，或 CPU 对照路径的）。D3D11 的 input layout 是拿建它时那个 VS 的字节码定的，运行时把它的签名当作 IA 的输出签名来校验，换个 VS 就对不上 —— 而 `setVertexDeclaration` 不收 nullptr，没法解绑 | 按 `mDrawVS` 建一个只有一个占位 POSITION 属性的 VertexDeclaration（多余元素 D3D11 允许，少了才报错），绘制前设上，并绑个 dummy VB 免得再挨「顶点缓冲没绑」的警告 |

| P10 | 退出时 `ReportLiveDeviceObjects` 报 13 个残留对象，refcount 约等于跑过的帧数（TextureApp 对照只剩 device 自己） | `setRenderTarget` 内部会 `backupRenderState()`，把当时绑着的整套状态 AddRef 一份，这份引用只有 `reset()` 会还 —— `T3DApplication.h:121` 的 `onRender` 注释写了「自己绑定的状态用完请 `ctx->reset()`」，手工绘制路径漏了这一步 | `record()` 在 `endPass()` 之后补 `ctx->reset()`。残留对象列表和手工绘制碰到的对象一一对应，是很好的定位线索 |

P9 那个「不报错的兄弟」也值得记：同一个 VS、同一个野 input layout，`DrawInstanced` 一声不吭，只有走索引的 `DrawIndexedInstancedIndirect` 才校验并报错。所以**「非索引绘制没警告」不能推出 input layout 是干净的**。真正的解法是 RHI 支持 `setVertexDeclaration(nullptr)` → `IASetInputLayout(nullptr)`，本 Sample 受「不改 RHI」约束才用占位 layout 绕过去，这一条可以作为 RHI 的待办。

P6 的定位方式值得记一笔：颜色 clear 能上屏说明命令流没问题，但 draw 无声无息 —— debug layer 对「深度测试全挂」不报任何消息。最后是 RenderDoc 抓帧 + `pixel_history` 直接给出 `failure_reasons: ["depth_test_failed"]`、`depth: 0.0` 才钉死的。同类「状态全对但画不出来」的问题，优先走 pixel history，别靠读代码猜。

---

## 10. 与既有文档的关系

| 文档 | 关系 |
|------|------|
| `RHI-Compute-UAV-Indirect-Draw-Design-todo.md` | 本文是它 §11「验证方案」的执行载体。每个用例跑完后，§11.1 表格里对应行的状态、§8.2 各后端能力表的实测修正、§9.2 跨编译结论，都要回填过去。它的 §12.8（SRV 输入侧不能绑顶点缓冲）由本文 §1.2 的选题评估触发并回填，核查后确认是纯接口类型问题、1.5–2 人日可解；那笔改动**不在本 Sample 范围内**，但若先于 S1 落地，可给自检轨加一个「顶点缓冲当 `ByteAddressBuffer` 读」的用例 |
| `GPU-Readback-onRender-Design-todo.md` | 本文全部自检用例依赖它的 `map` / `unmap` 与三个帧钩子。§6「Sample 如何升级」里点名的 BlitApp 尚未改造，本 Sample 是第二个真实使用者，用法要与 TextureApp 保持一致 |
| `PostProcessingApp-Design-todo.md` | 提供了「热键切用例 + 跨后端嵌入 shader + sample 自带 Behaviour」的完整先例，本文的交互层与工程结构照抄 |
| `Shader-MultiBackend-Variant-Design-todo.md` | compute 是它未覆盖的第五个阶段，§4.4 的绑定号结论应回填 |

---

## 11. 一句话

引擎有了 compute 的全套 RHI 原语却没有一个调用方，这个 Sample 的作用是把「编译通过」变成「跑出正确结果」—— 先用回读断言把 D3D11 钉死，再用同一套用例把 GL / GLES3 的差异测出来记账，最后用一个 CPU / GPU 可对照的粒子系统证明整条 UAV → barrier → SRV → 间接绘制的链路真的闭合。
