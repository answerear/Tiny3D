# T1-3 实现计划：Technique LOD 筛选与 ForwardAdd 多光源叠加 Pass

> **所属批次**：T1（引擎渲染管线与动画半成品补齐）  
> **状态**：✅ **已完成**（`T3DShader` / `T3DMaterial` / `T3DForwardRenderPipeline` / `TinyEditor` + `LightApp` 验收通过）  
> **预估工作量**：2.5 人日  
> **涉及文件**：
> - `source/Core/Include/Material/T3DTechnique.h` / `Source/Material/T3DTechnique.cpp`
> - `source/Core/Source/Material/T3DPass.cpp`
> - `source/Core/Include/Resource/T3DShader.h` / `Source/Resource/T3DShader.cpp`
> - `source/Core/Include/Resource/T3DMaterial.h` / `Source/Resource/T3DMaterial.cpp`
> - `source/Core/Include/Render/T3DForwardRenderPipeline.h` / `Source/Render/T3DForwardRenderPipeline.cpp`
> - `source/Core/Include/Kernel/T3DSettings.h`
> - `source/Editor/TinyEditor/UIMainWindow.cpp`
> - `source/Samples/LightApp/LightApp.cpp`
> - `assets/editor/builtin/shaders/Tiny3DStandardForwardFragment.cginc`（及 `assets/samples/meshes/` 同步副本）
> - `assets/editor/builtin/shaders/Tiny3DStandardForward.cginc`（及 `assets/samples/meshes/` 同步副本）
> - `assets/editor/builtin/shaders/Tiny3DStandard.shader`（及 `assets/samples/meshes/` 同步副本）

---

## 1. 现状分析（代码依据）

### 1.1 `Technique::mLOD` 已解析并序列化，但从未参与选择

1. `source/Core/Include/Material/T3DTechnique.h:142-147` 已有 `getLOD()` / `setLOD(uint32_t)` 及 `TPROPERTY` 反射声明；
2. ShaderLab 编译器（`T3DShaderCross.cpp`）已将 SubShader 内的 `LOD <number>` 解析并写入 `Technique::setLOD`；
3. 然而在 `Material::onLoad` / `Material::setShader` 中选择默认 `mCurrentTechInstance` 时，直接取了第 0 个 Technique，既没有提供 `Shader::setMaximumLOD` / 全局 `maximumLOD` 上限过滤，也没有在渲染管线中按预算切换。

### 1.2 `LightMode = ForwardAdd` 已解析，但管线只画 `ForwardBase`

1. `source/Core/Source/Material/T3DPass.cpp:314-317` 已将 Pass Tag `"LightMode" = "ForwardAdd"` 解析为 `mLightMode = ShaderLab::kBuiltinLightModeForwardAdd`；
2. 然而 `ForwardRenderPipeline::drawCameraQueue`（`T3DForwardRenderPipeline.cpp:684, 737, 781`）只查找 `ShaderLab::kBuiltinLightModeForwardBase`；
3. 同时，`setupLights`（第 969-1105 行）仅支持 1 盏环境光、1 盏方向光（多盏时后者覆盖前者），且将点光源和聚光灯上限硬编码为 `kMaxPointLights = 4`、`kMaxSpotLights = 4`，超出 4 盏时直接打 `Too many point/spot lights in scene !` 并丢弃。

---

## 2. 实现方案

### 2.1 第一部分：Technique LOD 选择机制（对齐 Unity `Shader.maximumLOD`）

1. **全局与单 Shader 最大 LOD 上限**：
   - 在 `RenderSettings`（`source/Core/Include/Kernel/T3DSettings.h`）增加 `uint32_t maxShaderLOD {UINT32_MAX};`（默认不限制）；
   - 在 `Shader`（`source/Core/Include/Resource/T3DShader.h`）增加 `mMaxLOD {UINT32_MAX}` 与 `setMaxLOD(uint32_t)` / `getMaxLOD()`；
   - 在 `Material` 增加 `selectTechniqueByLOD(uint32_t maxLOD)`：
     - 遍历 `mTechInstances`，选取满足 `tech->getLOD() <= effectiveMaxLOD`（若 `tech->getLOD() == 0` 视为无限制兜底）的首个 Technique（或 LOD 值最大但不超过上限的 Technique）作为 `mCurrentTechInstance`；
     - 若所有 Technique 的 LOD 均超出上限，则回退到 LOD 值最小的那个 Technique。
2. **编辑器 Graphics Tier 菜单联动（可选）**：
   - `UIMainWindow.cpp:458-465` 已有 `Graphics Tier 1 / Tier 2 / Tier 3` 菜单项，可分别对应全局 `maxShaderLOD = 200 / 400 / UINT32_MAX`。

---

### 2.2 第二部分：`ForwardAdd` 多光源分批叠加 Pass（方案 B：混合分批模式）

#### 2.2.1 设计选型背景：为什么采用“混合分批模式”而非 Unity Built-in 的“逐盏灯模式”？

| 维度 | 方案 A：Unity Built-in 逐盏灯模式 | 方案 B：混合分批模式（推荐采用） |
| :--- | :--- | :--- |
| **`ForwardBase` 容量** | 仅 1 盏平行光（点光/聚光全部移出 `ForwardBase`） | 保持现有 **1 平行光 + 4 点光 + 4 聚光** 快路 |
| **`ForwardAdd` 粒度** | 每 1 盏额外光源触发 1 次 DrawCall | 每 **1 平行光 + 4 点光 + 4 聚光** 为 1 个 `LightBatch` 触发 1 次 DrawCall |
| **现有资产兼容性** | **破坏兼容**：十余个 Sample 与 `.tmat`/`.tshader` 仅有 `ForwardBase`，移出点光/聚光后全部变黑 | **100% 向后兼容**：现有 Sample 与内置材质无需修改即可保持原样工作 |
| **DrawCall 开销** | 1 平行光 + 8 点光 = **9 次 DrawCall** | 1 平行光 + 8 点光 = **2 次 DrawCall**（1 Base + 1 Add） |
| **CBuffer 结构** | 需拆分或引入 `#pragma multi_compile_fwdadd` 多变体 | **完全复用现有 `tiny3d_LightParams`**，零 CBuffer 布局改动 |

#### 2.2.2 管线侧：光源分批数据结构（`LightBatch`）

在 `ForwardRenderPipeline`（`T3DForwardRenderPipeline.h`）中引入 `LightBatch` 结构，将场景中的全部光源按 `tiny3d_LightParams` 的槽位容量（`1 Dir + 4 Point + 4 Spot`）切分为若干批次：

```cpp
/**
 * \brief 单次 Pass（ForwardBase 或单次 ForwardAdd）可承载的光源批次
 */
struct LightBatch
{
    ColorRGBA        ambientColor {0.0f, 0.0f, 0.0f, 0.0f};
    ColorRGBA        dirLightColor {0.0f, 0.0f, 0.0f, 0.0f};
    Vector4          dirLightDir {0.0f, 0.0f, 0.0f, 0.0f};
    uint32_t         pointLightCount {0};
    ColorArray       pointLightColor {kMaxPointLights, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f)};
    Vector4fArray    pointLightPos {kMaxPointLights, Vector4f(0.0f, 0.0f, 0.0f, 0.0f)};
    Vector4fArray    pointLightAttenuation {kMaxPointLights, Vector4f(1.0f, 0.0f, 0.0f, 0.0f)};
    uint32_t         spotLightCount {0};
    ColorArray       spotLightColor {kMaxSpotLights, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f)};
    Vector4fArray    spotLightPos {kMaxSpotLights, Vector4f(0.0f, 0.0f, 0.0f, 0.0f)};
    Vector4fArray    spotLightDir {kMaxSpotLights, Vector4f(0.0f, 0.0f, 0.0f, 0.0f)};
    Vector4fArray    spotLightAttenuation {kMaxSpotLights, Vector4f(1.0f, 0.0f, 0.0f, 0.0f)};
};
```

1. **分批构建函数 `buildLightBatches()`**（在每帧 `renderForward` 前或 `cull` 结束时构建一次，存入 `TArray<LightBatch> mLightBatches`）：
   - **环境光（`kAmbient`）**：仅写入 `mLightBatches[0].ambientColor`；`mLightBatches[1..N]` 的 `ambientColor` 固定为 `(0, 0, 0, 0)`，防止叠加 Pass 重复累加环境光。
   - **平行光（`kDirectional`）**：
     - 优先将投射阴影的 `mImportantDirLight` 放入 `mLightBatches[0]`（使 `ForwardBase` 的 `shadowMap` 阴影采样与之对应）；
     - 第 2 盏及后续平行光依次分配到 `mLightBatches[1]`, `mLightBatches[2]` ... 的 `dirLightColor / dirLightDir` 槽位。
   - **点光源（`kPoint`）**：
     - 按每批 `kMaxPointLights = 4` 盏切分：第 $i$ 盏点光源（$0\text{-based}$）归入批次 `batchIdx = i / kMaxPointLights`，槽位 `slot = i % kMaxPointLights`；
     - 未填满的槽位保持颜色与强度为 `0`（注意：`pointLightAttenuation` 默认设为 `(1, 0, 0, 0)` 避免 `0/0`，尽管 Shader 中 `denom > 0.0` 已有保护）。
   - **聚光灯（`kSpot`）**：
     - 按每批 `kMaxSpotLights = 4` 盏切分：第 $j$ 盏聚光灯（$0\text{-based}$）归入批次 `batchIdx = j / kMaxSpotLights`，槽位 `slot = j % kMaxSpotLights`。
   - **批次总数**：`numBatches = max({1, dirCount, ceil(pointCount / 4.0), ceil(spotCount / 4.0)})`。
     - 当场景仅有 $\le 1$ 盏平行光、$\le 4$ 盏点光源、$\le 4$ 盏聚光灯时，`mLightBatches.size() == 1`，不触发任何 `ForwardAdd` Pass，保持零额外 DrawCall。

2. **材质光照常量写入函数 `applyLightBatch(Material *material, size_t batchIndex)`**：
   - 将 `mLightBatches[batchIndex]` 中的 `tiny3d_AmbientLight`、`tiny3d_DirLightColor`、`tiny3d_DirLightDir`、`tiny3d_PointLight*`、`tiny3d_SpotLight*` 写入 `material`。
   - 原 `setupLights(ctx, material)` 等价于 `applyLightBatch(material, 0)`。

#### 2.2.3 管线侧：`drawCameraQueue` 中的 `ForwardBase` + `ForwardAdd` 提交流程

在 `ForwardRenderPipeline::drawCameraQueue`（`T3DForwardRenderPipeline.cpp:660-816`）中：

1. **不透明队列（`queue < RenderQueue::kTransparent`）**：
   - 对每个 `itemGroup (material, renderables)`：
     - **Step 1（`ForwardBase` Pass）**：
       - 查找 `ShaderLab::kBuiltinLightModeForwardBase`；
       - 调用 `applyLightBatch(material, 0)` 写入第 0 批光源；
       - 绑定 `ForwardBase` 的 `RenderState` 与 Shader，遍历 `renderables` 提交绘制。
     - **Step 2（`ForwardAdd` Pass，当 `mLightBatches.size() > 1` 且相机非纯线框 `kWireframe` 模式时）**：
       - 在 `tech->getPassInstances()` 中查找 `ShaderLab::kBuiltinLightModeForwardAdd`；
       - 若该材质未定义 `ForwardAdd` Pass，则跳过（保证无 `ForwardAdd` 的旧材质或自发光/UI 材质安全跳过）；
       - 若存在 `ForwardAdd` Pass：
         - 绑定 `ForwardAdd` 的 `RenderState`（见 2.2.4 节叠加状态保障）；
         - 循环 `for (size_t batchIdx = 1; batchIdx < mLightBatches.size(); ++batchIdx)`：
           - 调用 `applyLightBatch(material, batchIdx)` 更新该批次光源到材质；
           - 调用 `setupShaders(ctx, material, addPass)` 刷新 PS 光照常量缓冲；
           - 遍历 `renderables` 调用 `submitRenderable(ctx, renderable, material, addPass)` 叠加绘制。

2. **透明队列（`queue >= RenderQueue::kTransparent`）**：
   - 透明队列按距离从后往前逐个 `TransparentSortItem` 绘制；
   - 对单个透明物体完成 `ForwardBase` 绘制后，若 `mLightBatches.size() > 1` 且其材质包含 `ForwardAdd` Pass，立即对该物体循环 `batchIdx = 1..N-1` 执行 `ForwardAdd` 叠加（确保在下一个更近的半透明物体盖上来之前，把当前半透明物体的附加光照累加完毕）。

#### 2.2.4 渲染状态保障：Additive 混合与 Early-Z 深度测试

`ForwardAdd` Pass 的核心硬件机制是 **在已写入深度的可见表面上做加法混合**：
- **`BlendState`**：`BlendEnable = true`、`SrcBlend = BlendFactor::kOne`、`DestBlend = BlendFactor::kOne`、`BlendOp = BlendOperation::kAdd`（颜色直接相加：`Src * 1 + Dst * 1`）；
- **`DepthStencilState`**：`DepthEnable = true`、`DepthWriteEnable = false`（不破坏 `ForwardBase` 写入的深度）、`DepthFunc = CompareFunction::kLessEqual`（必须包含 `Equal`，否则同一物体第 2 遍绘制时深度值与 Z-Buffer 相等，若用默认的 `kLess` 会被全部剔除！同时 `LessEqual` 可利用 GPU Early-Z 自动跳过被前景遮挡的像素）。
- **管线兜底策略**：
  - 在 `.shader` 的 `ForwardAdd` Pass 中显式声明 `Blend One One`、`ZWrite Off`、`ZTest LEqual`；
  - 同时在管线侧提供 `setupForwardAddRenderState(ctx, renderState)` 辅助函数：若代码动态创建的 `ForwardAdd` Pass 未显式开启 `BlendEnable`，管线自动覆盖为 `One + One` 混合、`DepthWriteEnable = false`、`DepthFunc = kLessEqual`，避免用户漏写状态导致第 2 遍绘制直接覆盖掉 `ForwardBase` 的底色。

#### 2.2.5 Shader 侧改造：复用 `tiny3d_LightParams` 新增 `fragForwardAdd`

1. **`Tiny3DStandardForwardFragment.cginc`**（`assets/editor/builtin/shaders/` 与 `assets/samples/meshes/`）：
   - 将方向光 + 点光源数组 + 聚光灯数组的逐像素光照求和提取为公共内联辅助函数（或直接在 `fragForwardAdd` 中调用）：
   - **`fragForwardBase(VertexOutputForwardBase input)`**：
     - 保持现有实现不变：`ambient * albedo.rgb + dirColor * shadow + sum(pointColor[0..3]) + sum(spotColor[0..3])`，输出 `alpha = 1.0f`。
   - **新增 `fragForwardAdd(VertexOutputForwardBase input)`**：
     - 与 `fragForwardBase` 共用完全相同的输入结构 `VertexOutputForwardBase` 和常量缓冲 `tiny3d_LightParams`；
     - **区别 1**：不计算、不累加 `ambient`（环境光已在 `ForwardBase` 加过）；
     - **区别 2**：平行光贡献 `dirColor` **不乘 `calcShadow(input.lightSpacePos)`**（主方向光阴影贴图仅对应 `ForwardBase` 的 `mImportantDirLight`，附加平行光不复用主光的阴影贴图）；
     - 累加当前批次的 `dirColor + sum(pointColor[0..3]) + sum(spotColor[0..3])`，返回 `float4(color, 1.0f)`。

2. **`Tiny3DStandardForward.cginc`**：
   - 在现有 `fragForward` 旁增加 `ForwardAdd` 片元入口包装：
     ```hlsl
     float4 fragForward(VertexOutputForwardBase i) : SV_Target { return fragForwardBase(i); }
     float4 fragForwardAdditive(VertexOutputForwardBase i) : SV_Target { return fragForwardAdd(i); }
     ```

3. **`Tiny3DStandard.shader`**：
   - 在 `SubShader` 的 `FORWARD`（`ForwardBase`）Pass 之后、`ShadowCaster` Pass 之前，新增 `FORWARD_ADD` Pass：
     ```hlsl
     Pass
     {
         Name "FORWARD_ADD"
         Tags { "LightMode" = "ForwardAdd" }

         Blend One One
         ZWrite Off
         ZTest LEqual

         CGPROGRAM
         #pragma vertex vertForward
         #pragma fragment fragForwardAdditive
         #pragma target 4.0

         #pragma multi_compile _ T3D_SKINNED_MESH
         #pragma multi_compile _ T3D_GPU_SKINNING

         #include "Tiny3DStandardForward.cginc"
         ENDCG
     }
     ```

---

## 3. 验收标准

1. **Technique LOD**：编写一个包含两个 SubShader（`LOD 400` 与 `LOD 100`）的测试 Shader，修改 `Shader::setMaxLOD(200)` 时，`Material::getCurrentTechnique()` 自动切到 `LOD 100` 的 SubShader；
2. **ForwardAdd 向后兼容**：在少于等于 1 盏平行光、4 盏点光源、4 盏聚光灯的现有 Sample（如 `PointLightApp`、`SpotLightApp`、`ShadowApp`）中，`mLightBatches.size() == 1`，不触发 `ForwardAdd` Pass，画面与 DrawCall 数量与改动前完全一致；
3. **ForwardAdd 多光源叠加**：在 `LightApp` 中放置 6 盏点光源（或 2 盏平行光 + 6 盏点光源）并为材质启用 `ForwardAdd` Pass，确认：
   - 不再打印 `Too many point lights in scene !` 警告；
   - 第 5、6 盏点光源（及第 2 盏平行光）的高光与漫反射通过第 2 个 `LightBatch` 的 `ForwardAdd` Pass 正确叠加在物体表面，且环境光与主光阴影未发生重复累加或过曝。
