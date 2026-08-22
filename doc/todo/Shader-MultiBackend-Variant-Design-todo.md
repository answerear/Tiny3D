# Shader 多后端变体容器（.tshader）设计与改造计划

> 目标：让同一个 ShaderLab 在切换渲染后端（D3D11 / GL4 / GLES3 / Vulkan）时**无需重新编译 C++、也无需运行时调用 scc**，并使 `BundleFSArchive`（只读资源包）能够正确加载渲染。核心思路对齐 Unity 的 Shader 资产模型：**逻辑 shader 的 UUID 恒定不变，多后端（Graphics API）变体在 shader 资源内部并存，运行时按当前 renderer 选择对应变体。**

---

## 1. 背景与问题

### 1.1 风险点
当前若要「切换渲染后端时不重编译代码 + shader 调用 scc 运行时转换」，会遇到两个无法调和的矛盾：

1. `BundleFSArchive` 是**只读**虚拟文件系统，运行时无法把 scc 转换结果写回。
2. 即使能写，一个 ShaderLab 编译产物 `.tshader` 只有**一个 UUID、一个散列文件**，不同后端会**互相覆盖**。

### 1.2 结论
「运行时切后端」与「运行时用 scc 现编译 shader 并写回资源包」是**两个不同生命周期阶段的事，不应绑定**。参考 Unity：

- 玩家端**不带 shader 源码编译器**，所有 GPU 字节码在**构建期**烘焙完成；运行时只做「挑选变体 + 上传 GPU」。
- 材质（Material）永远只引用**逻辑 shader 的 GUID**，与平台 / API / keyword 变体**完全无关**。
- 变体（API × keyword）**不分配新的 ID**，是 Shader 对象内部按 (API, keyword) 索引的数据块。
- 跨平台差异通过「按平台分别构建 AssetBundle」解决；同平台多 API 则在同一 shader 内并存多份变体。

因此 `BundleFSArchive` 的只读特性是**正确的设计**，不应破坏。正确做法是把多后端编译**前置到打包期**，运行时只读只选。

---

## 2. 现状分析

### 2.1 `.tshader` 数据模型（序列化的 `Shader` 对象）

```
Shader { UUID, ConstantValues, Samplers, Techniques[] }
└─ Technique { LOD, Name, Passes[] }
   └─ Pass { RenderState, Tags, Keywords[], CurrentKeyword,
             VertexShaders / PixelShaders / GeometryShaders / HullShaders / DomainShaders }
      └─ ShaderVariants = std::map<ShaderKeyword, ShaderVariant>      ← keyword 维度已容器化
         └─ ShaderVariant { Code(Buffer), ShaderConstantParams, ShaderSamplerParams }
```

### 2.2 关键事实

1. **keyword 维度已经是 `map<ShaderKeyword, ShaderVariant>` 容器**（已完成一半）。
2. `ShaderVariant` 持有**单一** `mBytesCode`（一种语言的源码 / 字节码）+ 一组**语言相关的反射信息**（`ShaderConstantParams.BindingPoint`、sampler binding）。HLSL 用 `register(b5/t1/s2)`，GLSL/SPIR-V 的 binding 不同，**不能跨语言共享**。
3. 运行时 `ShaderVariant::compile()` → `RHIContext::compileShader(this)` / `createXxxShader(this)`，直接把 `mBytesCode` 交给**当前 RHI**。语言不匹配即失败。
4. scc（`ShaderCompiler`）一次只编**一种** `mArgs.target`，产出「单语言 tshader」。
5. ~~`MetaShader` 只记录一个 `mLanguage`。~~（改造后已移除该字段，meta 不再记录语言，见 §6.2）
6. `BundleBuilder` 把 `.tshader` 按**自身 UUID** 命名导出散列文件；`Material` 引用 `shaderUUID`。

### 2.3 涉及的关键文件

| 模块 | 文件 |
|---|---|
| Shader 变体 | `source/Core/Include/Material/T3DShaderVariant.h`、`source/Core/Source/Material/T3DShaderVariant.cpp` |
| 变体实例 | `source/Core/Include/Material/T3DShaderVariantInstance.h`、`T3DPassInstance.*` |
| Pass / Technique | `source/Core/Include/Material/T3DPass.h`、`source/Core/Source/Material/T3DPass.cpp`、`T3DTechnique.*` |
| Shader 资源 | `source/Core/Include/Resource/T3DShader.h`、`source/Core/Source/Resource/T3DShader.cpp`、`T3DShaderManager.*` |
| Meta | `source/Core/Include/Meta/T3DMetaShader.h` |
| 离线编译器 (scc) | `source/Tools/ShaderCrossCompiler/Source/T3DShaderCompiler.cpp`、`T3DShaderCross.cpp` |
| 打包器 | `source/Tools/BundleBuilder/Source/T3DBundleBuilderApp.cpp` |
| 只读包 | `source/Plugins/Archive/BundleFileSystem/Source/T3DBundleFSArchive.cpp` |
| 开发期自动编译 | `source/Samples/ResourceApp/ResourceApp.cpp`（`autoCompileShaders`）、编辑器对应处 |
| 材质 | `source/Core/Source/Resource/T3DMaterial.cpp` |

---

## 3. 核心设计决策

**在 `ShaderVariant` 之上、`ShaderKeyword` 之下，引入显式的 Language（Graphics API）维度。**

Pass 每个 stage 的容器：

```
改造前：  map<ShaderKeyword, ShaderVariant>
改造后：  map<ShaderKeyword, ShaderVariantSet>
          其中 ShaderVariantSet = map<SHADER_LANGUAGE, ShaderVariant>
```

设计理由：

- **反射信息（binding / register）天生语言相关**，需要按语言区分的不只是 code，而是**整个 `ShaderVariant`**，所以 language 放在 variant 这一层最干净。
- keyword 维度保持不变，与 Unity「keyword variant × graphics API」两个正交维度对齐。
- **UUID / manifest / 材质引用全部不动**——材质永远只认逻辑 shader 的 UUID。
- 引入轻量包装类 `ShaderVariantSet` 承载中间这层 map，使序列化与访问清晰（避免裸嵌套 map）。

一个 `.tshader` 文件 = 一个 UUID = 「(keyword) × (language)」全变体矩阵。

---

## 4. `.tshader` 容器格式扩展

### 4.1 新增语言枚举

```cpp
// T3DShaderVariant.h 附近
TENUM()
enum class SHADER_LANGUAGE : uint32_t
{
    kUnknown = 0,
    kHLSL,      // Direct3D11/12
    kGLSL,      // OpenGL 4
    kESSL,      // OpenGL ES 3
    kSPIRV,     // Vulkan
    kMSL,       // Metal（预留）
    kMax
};
```

### 4.2 `ShaderVariant` 增加 `Language` 字段

```cpp
TPROPERTY(RTTRFuncName="Language", RTTRFuncType="getter")
SHADER_LANGUAGE getLanguage() const { return mLanguage; }

TPROPERTY(RTTRFuncName="Language", RTTRFuncType="setter")
void setLanguage(SHADER_LANGUAGE lang) { mLanguage = lang; }

// 新增成员：
SHADER_LANGUAGE mLanguage {SHADER_LANGUAGE::kUnknown};
```

#### 4.2.1 源码与编译产物必须分离（运行时切后端的前提）

> **现状缺陷**：`ShaderVariant` 只有一块 `mBytesCode`，`compile()` 时被 `ctx->compileShader()` **原地从源码覆盖成字节码**（见 `T3DD3D11ContextBase.cpp` 第 135 行 `setBytesCode`），且 `compile()` 开头 `if (mHasCompiled) break;`。这意味着一旦编译，**源码丢失且不可重编译**。在「运行时切换渲染后端」场景下这是致命的。

为支持运行时切后端 / 设备重置（device lost），`ShaderVariant` 字段需拆为两类：

```cpp
// 后端无关：语言源码（反序列化得到，只读，永不被覆盖）
char            *mSourceCode {nullptr};
size_t           mSourceCodeSize {0};

// 后端相关：编译产物（可释放、可重建）
char            *mByteCode {nullptr};       // 编译后的字节码（如 DXBC/SPIR-V）
size_t           mByteCodeSize {0};
RHIShaderPtr     mRHIShader {nullptr};
bool             mHasCompiled {false};      // 语义改为：当前 active 后端的 RHIShader 是否就绪
```

- `compile()` 始终以 `mSourceCode` 为输入，产物写入 `mByteCode`/`mRHIShader`，**不破坏 `mSourceCode`**。
- `mHasCompiled` 从「是否编译过」改为「**当前后端的 RHIShader 是否就绪**」，可在切后端时被安全 reset。
- `if (mHasCompiled) break;` 保留——在多语言模型下它是正确的缓存（每个 language variant 只为其对应后端编译一次）。

#### 4.2.2 为何 `if (mHasCompiled) break;` 在新模型下是对的

- 旧模型（单 variant 跨后端复用）：切后端后 `mHasCompiled=true` 导致不重编、`mRHIShader` 仍是旧后端对象 → 崩，且源码已被覆盖无法补救。
- 新模型（多语言容器）：**不同后端 = 不同 `ShaderVariant` 对象**（按 `SHADER_LANGUAGE` 分），各自独立持有 `mHasCompiled/mByteCode/mRHIShader`。切后端是「从 `ShaderVariantSet` 重新按 language 选 variant」，不是「重编旧 variant」。kHLSL variant 的编译状态与 kSPIRV variant 互不影响；切回时直接复用已缓存的 `mRHIShader`。`if(mHasCompiled) break;` 因此成为**期望的缓存行为**。

### 4.3 中间层 `ShaderVariantSet`

```cpp
TCLASS()
class T3D_ENGINE_API ShaderVariantSet : public Object
{
    // 同一 keyword 下，按语言索引的多份编译产物
    // std::map<SHADER_LANGUAGE, ShaderVariantPtr> mVariants;
public:
    ShaderVariantPtr get(SHADER_LANGUAGE lang) const;
    void put(SHADER_LANGUAGE lang, ShaderVariantPtr v);

    TPROPERTY(RTTRFuncName="Variants", RTTRFuncType="getter/setter")
    // 序列化 mVariants
};
```

### 4.4 扩展后的 JSON 结构对比

改造前（现状）：

```json
"PixelShaders": {
  "RTTI_Type": "std::map<Tiny3D::ShaderKeyword, Tiny3D::SmartPtr<Tiny3D::ShaderVariant>>",
  "RTTI_Value": [
    { "RTTI_Map_Key": { "...ShaderKeyword..." },
      "RTTI_Map_Value": {
        "RTTI_Type": "Tiny3D::ShaderVariant",
        "RTTI_Value": { "Code": { "...": "" }, "ShaderConstantParams": { "...": "" } }
      } }
  ]
}
```

改造后（多语言容器）：

```json
"PixelShaders": {
  "RTTI_Type": "std::map<Tiny3D::ShaderKeyword, Tiny3D::SmartPtr<Tiny3D::ShaderVariantSet>>",
  "RTTI_Value": [
    { "RTTI_Map_Key": { "...ShaderKeyword..." },
      "RTTI_Map_Value": {
        "RTTI_Type": "Tiny3D::ShaderVariantSet",
        "RTTI_Value": {
          "Variants": {
            "RTTI_Type": "std::map<Tiny3D::SHADER_LANGUAGE, Tiny3D::SmartPtr<Tiny3D::ShaderVariant>>",
            "RTTI_Value": [
              { "RTTI_Map_Key": { "RTTI_Value": "kHLSL" },
                "RTTI_Map_Value": { "RTTI_Type": "Tiny3D::ShaderVariant",
                  "RTTI_Value": { "Language": "kHLSL",
                                  "Code": { "...DXBC/HLSL..." },
                                  "ShaderConstantParams": { "...register(bN)..." } } } },
              { "RTTI_Map_Key": { "RTTI_Value": "kSPIRV" },
                "RTTI_Map_Value": { "RTTI_Type": "Tiny3D::ShaderVariant",
                  "RTTI_Value": { "Language": "kSPIRV",
                                  "Code": { "...SPIR-V..." },
                                  "ShaderConstantParams": { "...binding=N..." } } } }
            ]
          }
        }
      } }
  ]
}
```

---

## 5. 运行时管线改动

### 5.1 当前后端 → 语言推导（集中化）

将散落在 `ResourceApp::autoCompileShaders` 的 if/else 上提到引擎层：

```cpp
// 建议放在 RHIRenderer 或 Agent
SHADER_LANGUAGE RHIRenderer::getShadingLanguage() const
{
    const String &n = getName();
    if (n == OPENGL4)                       return SHADER_LANGUAGE::kGLSL;
    if (n == OPENGLES2 || n == OPENGLES3)   return SHADER_LANGUAGE::kESSL;  // 或 kGLSL
    if (n == VULKAN)                        return SHADER_LANGUAGE::kSPIRV;
    return SHADER_LANGUAGE::kHLSL;          // Direct3D11
}
```

### 5.2 `Pass` 容器与选择逻辑

- 成员类型：6 个 stage（`mVertexShaders` 等）由 `map<ShaderKeyword, ShaderVariantPtr>` → `map<ShaderKeyword, ShaderVariantSetPtr>`（调整 `ShaderVariants` typedef 定义，访问器签名基本不变）。
- `addShaderVariant(keyword, variant)`：内部按 `variant->getLanguage()` 塞进对应 keyword 的 `ShaderVariantSet`。
- `Pass::compile()`：取当前 `SHADER_LANGUAGE lang = ctx->getRenderer()->getShadingLanguage()`，对每个选中的 keyword，从 set 里 `get(lang)` 取出 `ShaderVariant`，**只编译选中的那一个**，其余语言变体不参与运行时编译，也不上传 GPU。
- 找不到当前语言变体 → 明确报错并日志（提示 bundle 未烘焙该后端）。

### 5.3 `PassInstance` / `ShaderVariantInstance`

- 基于「选中的那一个 `ShaderVariant`」工作，**逻辑不变**；只是 variant 来源从「map 直接取」变为「map → set → 按 language 取」。

### 5.4 材质层

- `Material` / `Material::onLoad` / `loadShader(archive, shaderUUID)` **零改动**。

### 5.5 运行时切换渲染后端：资源失效与重建

> **优先级：延后，暂不实行。** 当前阶段仅需支持「不同发行版 / 启动时选择不同后端」（启动时按 `Tiny3D.cfg` 的 `renderer` 确定），**进程内动态切后端**暂不实现。本节作为后续扩展的设计预留。
>
> 注意：即便暂不实现本节，§4.2.1 的「源码 / 编译产物字段分离」仍建议**先做**——它是 device-lost 恢复与未来切后端的共同基础，且不增加当前实现复杂度。

> 引擎当前**没有**运行时切后端机制（启动时按 `Tiny3D.cfg` 的 `renderer` 确定）。要实现「不重编译 C++ 即可切后端」，需新增本节的失效-重建协议。**切后端不重新 load shader 资源**，只重建后端相关的 RHI 对象层。

#### 5.5.1 两层资源划分

| 层 | 内容 | 切后端时 |
|---|---|---|
| 后端**无关** CPU 数据 | `Shader`/`Technique`/`Pass`/`ShaderVariantSet` 容器、各 language 源码（`mSourceCode`）、反射常量/采样器元信息、`Material` 参数 | **保留**，不重建 |
| 后端**相关** RHI 对象 | `ShaderVariant.mByteCode`/`mRHIShader`、`ShaderVariantInstance`/`PassInstance` 的 ConstantBuffer/Sampler RHI、Mesh 的 VB/IB、Texture GPU 资源、PSO 等 | **失效并按新后端重建** |

#### 5.5.2 失效-重建协议

```cpp
// ShaderVariant：释放后端产物，保留源码
void ShaderVariant::invalidateRHI()
{
    mRHIShader = nullptr;       // 释放 RHI 对象
    // 释放 mByteCode（保留 mSourceCode 不动）
    mByteCode = nullptr; mByteCodeSize = 0;
    mHasCompiled = false;       // 下次 compile() 会以 mSourceCode 重新编译
}
```

- `PassInstance` / `ShaderVariantInstance` / `RenderResource` 等各自提供 `invalidateRHI()` / `recreateRHI()`。
- 新增全局流程 `onRendererChanged()`：切换 `RHIContext` 后，遍历所有存活的后端相关资源调用 `invalidateRHI()`，下一帧渲染时**懒重建**（用到哪个变体才 `compile()` 哪个）。

#### 5.5.3 切后端时序

```
用户/系统请求切后端
  └─ 切换 active RHIContext / Renderer 插件
  └─ onRendererChanged():
       遍历 ShaderVariant / *Instance / RenderResource → invalidateRHI()
  └─ 下一帧渲染：
       Pass::compile() 按新 renderer 的 getShadingLanguage() 从 ShaderVariantSet 选 variant
       选中 variant 若 !mHasCompiled → compile()（以 mSourceCode 为输入）→ 建 mRHIShader
       PassInstance / RenderResource 懒重建 GPU 资源
```

#### 5.5.4 缓存与显存权衡（可选）

- 默认可**保留各 language variant 已编译的 `mRHIShader`** 作为缓存：切回曾用过的后端时直接复用（`mHasCompiled=true`，零重编）。
- 若需省显存，可对「非当前后端」的 variant 主动 `invalidateRHI()`，再次切回时以 `mSourceCode` 重新编译——这正是 §4.2.1 必须保留源码的原因。

---

## 6. 离线编译 / 打包改动

### 6.1 scc（`ShaderCompiler`）支持多 target —— 采用路线 A（已选定）

> **实现路线决策：采用路线 A** ——`scc` 一次接受多个目标语言（`-t hlsl,glsl,spirv`），在**同一次运行**内对每个目标循环 cross-compile，把各目标产出的 variant 标注 `Language` 后**合并进同一个 `Pass`**，最终序列化成单个多语言 `.tshader`。不采用「先各编单语言再 merge」的路线 B。

#### 6.1.1 现状回顾（编译流程）

```
compile(code, pass, inputPath, outputDir, args)
  └─ parsePragmaArgs            // 解析 #pragma multi_compile 等
  └─ generateShaderSnippets     // 按 keyword 组合 × stage 枚举出 snippets（与目标语言无关）
  └─ for each snippet:
       compileShaderSnippet(snippet, pass)
         └─ postProcessor: ShaderVariant::create(keyword, content)
                           → setShaderStage(...)
                           → pass->addShaderVariant(keyword, variant)   // 当前：单语言
```

关键观察：**keyword × stage 的枚举（`generateShaderSnippets`）与目标语言完全无关**，因此多目标只需在「cross-compile 输出」这一步按语言各跑一遍，snippet 枚举可复用。

#### 6.1.2 路线 A 改动点

1. **`Args` 支持多目标**（`T3DShaderCompiler.h`）
   ```cpp
   // 现状：String target;
   // 改为：
   TArray<String> targets;   // 例如 { "hlsl", "glsl", "spirv" }
   ```
   保留单 `target` 的兼容解析（单值时等价于单元素列表）。

2. **命令行解析**（`T3DShaderCrossApp` / `T3DShaderCross.cpp`）
   - `-t hlsl,glsl,spirv` 按逗号拆分填入 `Args.targets`；
   - 兼容旧的 `-t hlsl` 单值写法。

3. **`compile(code, pass, ...)` 增加目标语言外层循环**（`T3DShaderCompiler.cpp`）
   ```cpp
   // snippet 枚举只做一次
   ShaderSnippets snippets;
   generateShaderSnippets(source, programParams, snippets);

   for (const String &target : mArgs.targets)        // ← 新增外层循环
   {
       mCurrentTarget = target;                       // 当前目标语言
       for (const auto &s : snippets)
           ret = ret && compileShaderSnippet(s.second, pass);
   }
   ```
   其中 cross-compile 的目标语言映射 `targetDesc.language = getShadingLanguage(mCurrentTarget)`（原先取 `mArgs.target`，改取当前循环目标）。

4. **`compileShaderSnippet(snippet, pass)` 的 postProcessor 标注语言并合并**（`T3DShaderCompiler.cpp` 约 541–549 行）
   ```cpp
   ShaderVariantPtr v = ShaderVariant::create(std::move(keyword), content);
   v->setShaderStage(shaderType);
   v->setLanguage(toShaderLanguage(mCurrentTarget));   // ← 新增：标注语言
   pass->addShaderVariant(v->getShaderKeyword(), v);   // addShaderVariant 内部按 language 合并进 ShaderVariantSet
   ```
   `Pass::addShaderVariant`（阶段 1/2 改动）内部：以 keyword 找到 / 新建 `ShaderVariantSet`，再以 `v->getLanguage()` 为键 `put` 进 set，实现「同 keyword、多语言」合并。

5. **`toShaderLanguage(target)` 映射**：scc 的 target 字符串 → `SHADER_LANGUAGE` 枚举（`hlsl→kHLSL`、`glsl→kGLSL`、`essl→kESSL`、`spirv→kSPIRV`、`msl→kMSL`）。

> 说明：scc 另一个文件输出重载 `compileShaderSnippet(snippet)`（写 `_stage.<target>` 散点文件，约 553–578 行）属于调试/中间产物输出，路线 A 下其文件名后缀改用 `mCurrentTarget` 即可，不影响主链路。

### 6.2 `MetaShader` 记录语言集合 —— ⛔ 已废弃（被设计决策取代）

> **决策（阶段 5 实施时确定）**：不在 `MetaShader` 中记录语言信息。
>
> 原计划是把单 `mLanguage` 升级为 `std::set<SHADER_LANGUAGE> mLanguages`，供工具判断「缺哪个后端、要不要补编」。但 `.tshader` 多语言化后**本身就是自描述的**——「含哪些语言」可直接从其内部 `ShaderVariantSet` 的键集合推导，无需在 meta 里冗余一份并承担同步风险。
>
> 因此：
> - `MetaShader` 的 `mLanguage` / `getLanguage` / `setLanguage` 字段**已整体移除**（`T3DMetaShader.h`），`.tshader.meta` 只保留 UUID。
> - 需要「某 tshader 含哪些语言」的场景（如 `BundleBuilder` 裁剪自检、开发期补编判断），统一**反序列化 `.tshader` 后遍历 `ShaderVariantSet` 推导**。
> - 受影响的写入点：`BuiltinGenerator`（去掉 `setLanguage("hlsl")`）、`MetaFSMonitor`（创建 MetaShader 时本就不设语言）、`ResourceApp::autoCompileShaders`（整体移除，见阶段 6）。

~~原计划：~~

```cpp
// 单 language → 语言集合（已废弃，不再采用）
std::set<SHADER_LANGUAGE> mLanguages;   // 该 tshader 内含哪些后端
```

### 6.3 `BundleBuilder` —— ✅ 已实现（语言裁剪 + 覆盖自检）

- **寻址零改动**（UUID 不变，散列文件仍为 `<uuid>`）。
- 打包前要求 `.tshader` 已含目标语言（由 scc 多目标预编译，见阶段 4）。本阶段**不**让 BundleBuilder 驱动 scc（`.tshader` 已是预编译多语言产物）。

实现要点（`T3DBundleBuilderApp.h/.cpp`）：

- 新增命令行参数 `--keep-languages <a,b,c>`（合法值 `hlsl,glsl,essl,spirv,msl`；**缺省 = 不裁剪、保留全部**，此时 `.tshader` 仍走原样字节拷贝）。
- 指定白名单后，对 `Meta::kShader` 的 `.tshader`：
  - 用 `T3D_SERIALIZER_MGR.deserialize<Shader>()` 反序列化（**不走 `ShaderManager::load`，避免触发运行时编译**；BundleBuilder 以 NullRenderer 起完整引擎，反序列化安全）。
  - 遍历 `Technique → Pass → 6 个 stage → 每个 ShaderVariantSet`，`removeVariant(lang)` 剔除不在白名单内的语言变体。
  - 重新 `serialize()` 到以 UUID 命名的散列文件（取代原样拷贝）。
- **覆盖自检**（裁剪后告警，不中断）：
  - 请求保留但 `.tshader` 内根本不存在的语言 → `missing requested language`。
  - 裁剪后存在「某 (stage, keyword) 集合被清空」→ `variant set left empty`。
- 决策依据：`--keep-languages` 的取值 = 该发行版运行时可能切换到的所有渲染后端对应语言的并集（见 §6.3.1，按渲染后端而非 OS 平台）。

#### 6.3.1 语言裁剪 `--keep-languages`（按「支持的渲染后端」决定，不按 OS 平台）

> **重要修正**：语言裁剪**不能**按操作系统武断决定（例如「Windows 就用 HLSL」）。同一个 OS 可同时支持多种后端——Windows 既能跑 Direct3D11，也能跑 OpenGL4 / Vulkan；Android 能跑 GLES3 / Vulkan。**语言维度与渲染后端一一对应，裁剪依据必须是「该发行版计划支持的渲染后端集合」，由构建者显式指定。**

- 参数形式（保留白名单，默认不裁剪 = 保留全部）：
  ```
  bundlebuilder ... --keep-languages hlsl,glsl,spirv     # 保留这三种，剔除其余
  ```
- 语言与后端的对应关系（与 `RHIRenderer::getShadingLanguage()` 完全一致，是同一张映射表）：

  | 渲染后端 | 语言 (`SHADER_LANGUAGE`) | scc target |
  |---|---|---|
  | Direct3D11 | `kHLSL` | `hlsl` |
  | OpenGL4 | `kGLSL` | `glsl` |
  | OpenGL ES3 | `kESSL` | `essl` |
  | Vulkan | `kSPIRV` | `spirv` |
  | Metal（预留） | `kMSL` | `msl` |

- 决策规则：**`--keep-languages` 的取值 = 该发行版运行时可能切换到的所有渲染后端所对应语言的并集。**
  - 例：某 Windows 发行版只发布 D3D11 → `--keep-languages hlsl`。
  - 例：某 Windows 发行版允许用户在 D3D11 / GL4 / Vulkan 间切换 → `--keep-languages hlsl,glsl,spirv`（全保留）。
  - 例：某 Android 发行版支持 GLES3 + Vulkan → `--keep-languages essl,spirv`。
- **约束**：保留的语言集合必须覆盖运行时所有可能启用的后端；若运行时切到某个后端而 bundle 内缺少对应语言变体，加载该 shader 时会按 §5.2 报错。建议 `BundleBuilder` 在裁剪后做一次自检：对每个 `.tshader`，校验保留语言集合非空且覆盖 `--keep-languages` 全集，缺失则告警。
- UUID / manifest / 材质引用依旧零改动。

> 与构建配置打通（可选增强）：渲染后端集合通常已存在于工程的构建/发布配置中（类似 `Tiny3D.cfg` 的 `plugins` 里启用了哪些 `*Renderer`）。可让 `BundleBuilder` 直接读取该配置推导 `--keep-languages`，避免构建者手填导致与实际启用后端不一致。

---

## 7. 开发期（编辑器 / ResourceApp）改动

`autoCompileShaders` 的语义从「**language 不匹配就覆盖**」改为「**当前 language 不在 tshader 语言集合里就追加编译并 merge 进去**」：

```
读 tshader 的 MetaShader.mLanguages
若 当前 renderer 的 lang ∉ mLanguages:
    scc 编出该 lang 变体 → 合并进 tshader（保留已有语言）→ mLanguages 加入该 lang
否则:
    跳过
```

开发期 MetaFS（可写）里的 tshader 会**逐步累积**多语言且永不互相覆盖；bundle 则是该累积结果的只读快照。

---

## 8. 兼容与迁移

> **决策：不兼容旧格式 `.tshader`** —— 不实现反序列化兼容分支，改为**一次性全量重生成 + 重打包**。

1. ~~**旧 tshader 兼容**：在 `Pass` 反序列化时检测旧结构并包成单元素 set。~~ **不做**。
   - 原因：新代码加载旧 `map<Keyword, ShaderVariant>` 不会优雅降级——JSON 的 RTTI 类型与新 setter 期望的 `map<Keyword, ShaderVariantSet>` 不匹配，各 stage 容器为空 → shader 无变体 → 编译/渲染失败。因此必须保证**没有旧格式文件残留**。
   - 取而代之：用新 scc 重生成全部源 `.tshader`，并用 `BundleBuilder` 重打所有含 shader 的 bundle（现有量很小）。
2. `Version` 字段升版（`0.0.1`→`0.0.2`）**变为可选**：它原本只为兼容分支区分新旧用，无兼容分支则不强制。可保留作格式标记。
3. **本工程迁移完成情况**：
   - 3 个源 `.tshader`（meshes）— ✅ 重生成多语言；bin 副本 — ✅ 同步。
   - `assets/samples/bundle` 与其 bin 副本 — ✅ 用 `BundleBuilder` 重打，旧 `9ba3…`（402KB 单语言）已替换为多语言（1.56MB，含 kHLSL/kGLSL/kESSL/kSPIRV）。
   - 其它含 shader 的 bundle — 若新增需同样重打。

---

## 9. 分阶段实施清单

| 阶段 | 改动 | 主要文件 |
|---|---|---|
| 1. 数据模型 | 新增 `SHADER_LANGUAGE` 枚举、`ShaderVariant.mLanguage`、**源码/编译产物字段分离（`mSourceCode` 永不覆盖 + `mByteCode`/`mRHIShader` 可重建）**、`ShaderVariantSet` 类、`ShaderVariants` typedef 改定义 | `T3DShaderVariant.h/.cpp`、新增 `T3DShaderVariantSet.*`、`T3DTypedef.h` |
| 2. 运行时选择 | `RHIRenderer::getShadingLanguage()`；`Pass` 容器 / `addShaderVariant` / `compile` 按 language 选 | `T3DRHIRenderer.*`、`T3DPass.h/.cpp` |
| 3. 实例层适配 | `PassInstance` / `ShaderVariantInstance` 取 variant 路径 | `T3DPassInstance.cpp` |
| 3b. 运行时切后端（**延后，暂不实行**） | 失效-重建协议：`ShaderVariant::invalidateRHI()`、`*Instance`/`RenderResource` 的 invalidate/recreate、全局 `onRendererChanged()` + 懒重建（见 §5.5）。当前仅支持启动时选后端，进程内动态切换后续再做 | `T3DShaderVariant.*`、`T3DPassInstance.*`、`T3DRenderResourceManager.*`、`T3DAgent.*` |
| 4. 离线编译（路线 A） | `Args.targets` 多目标 + 命令行 `-t a,b,c` 解析 + 目标语言外层循环 + variant 标 `Language` + `addShaderVariant` 合并进 set | `T3DShaderCompiler.h/.cpp`、`T3DShaderCrossApp.*` / `T3DShaderCross.cpp` |
| 5. Meta / 打包 ✅ | ~~`MetaShader` 多语言集合~~（**废弃**：meta 不再记录语言，从 `.tshader` 推导，见 §6.2）；`BundleBuilder` `--keep-languages` 语言裁剪（按发行版支持的渲染后端集合，非按 OS 平台）+ 裁剪后覆盖自检 | `T3DBundleBuilderApp.h/.cpp`（`T3DMetaShader.h` 已移除语言字段） |
| 6. 开发期 | ~~`autoCompileShaders` 改为增量补语言~~ → **已整体移除**（`.tshader` 自带全部语言，开发期无需运行时补编；meta 也不再记录语言） | `ResourceApp.cpp`（已移除 `autoCompileShaders`） |
| 7. 兼容 ✅ | **决策：不兼容旧格式** → ~~`T3DPass.cpp` 反序列化兼容分支~~ 不做；改为一次性全量重生成 `.tshader` + 重打 bundle（见 §8，已完成） | 无代码改动（数据迁移） |
| 8. 验证 ✅ | ResourceApp 从 bundle 加载（`LOAD_FROM_BUNDLE=1`），切 D3D11 / GL4 / VK 各跑一遍，三后端均成功加载 bundle 并编译 `Tiny3DStandard`（分别选 HLSL/GLSL/SPIR-V 变体），无错误。**关键依赖**：材质持久化的是 ShaderLab 逻辑 UUID（`7d32…`），bundle 内实为编译后 `.tshader`（ShaderUUID `9ba3…`），靠 BundleBuilder 写入的 `ALIAS 7d32 9ba3` + 运行时 `BundleFSArchive::read(uuid)` 重定向对齐，必须用带 ALIAS 的 builder 重打 bundle 才能跑通 | `ResourceApp.cpp`、`bundle.manifest`(ALIAS) |

---

## 10. 设计本质（一句话总结）

把引擎的 shader 模型补齐到 Unity 模型：

> **(UUID = 逻辑 shader，恒定不变) × (keyword 维度，已有) × (language / Graphics API 维度，新增到 ShaderVariant)**

材质引用零改动，`BundleFSArchive` 保持只读，彻底消除「同一 UUID 不同后端互相覆盖」的问题；切换渲染后端时既不重编译 C++，也不需要运行时 scc。

---

## 11. 验证点清单（BundleFS + 多后端）

| 功能 | 验证方式 | 结果 |
|---|---|---|
| manifest 解析 | 启动日志 `Bundle manifest loaded, N entries` | ✅ 三后端均 `21 entries` |
| `read(name)` → UUID | `loadMesh(archive, "tortoise.tmesh")` 成功 | ✅ |
| `read(uuid)` 直读 | 子资源（material / shader / texture / skeleton / anim）加载成功 | ✅ |
| ShaderLab UUID 重定向 | manifest `ALIAS 7d32… 9ba3…`；`read(7d32)` 经 `getRedirect` 读到 `9ba3.tshader` | ✅ |
| 多语言变体选择 | 切 D3D11 → 取 kHLSL；切 GL4 → 取 kGLSL；切 VK → 取 kSPIRV | ✅ `Completed compiling shader` 无错误 |
| 端到端渲染 | tortoise 模型在各后端均正常显示并播放骨骼动画 | ✅ 进程稳定运行不崩溃（窗口渲染） |
| 只读保护 | `BundleFSArchive::write` 返回 `T3D_ERR_NOT_IMPLEMENT` | ✅ 代码保证 |
| 缺失后端报错 | 加载缺少当前语言变体的 shader 时给出明确日志 | ✅ BundleBuilder 覆盖自检 + 运行时报错 |
