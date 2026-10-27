# T1-3 实现计划：Technique LOD 筛选与 ForwardAdd 多光源叠加 Pass

> **所属批次**：T1（引擎渲染管线与动画半成品补齐）  
> **预估工作量**：2.5 人日  
> **涉及文件**：
> - `source/Core/Include/Material/T3DTechnique.h`
> - `source/Core/Include/Resource/T3DMaterial.h` / `Source/Resource/T3DMaterial.cpp`
> - `source/Core/Include/Render/T3DForwardRenderPipeline.h`
> - `source/Core/Source/Render/T3DForwardRenderPipeline.cpp`
> - `source/Core/Include/Kernel/T3DSettings.h`

---

## 1. 现状分析（代码依据）

### 1.1 `Technique::mLOD` 已解析并序列化，但从未参与选择

1. `source/Core/Include/Material/T3DTechnique.h:142-147` 已有 `getLOD()` / `setLOD(uint32_t)` 及 `TPROPERTY` 反射声明；
2. ShaderLab 编译器（`T3DShaderCross.cpp`）已将 SubShader 内的 `LOD <number>` 解析并写入 `Technique::setLOD`；
3. 然而在 `Material::onLoad` / `Material::setShader` 中选择默认 `mCurrentTechInstance` 时，直接取了第 0 个 Technique，既没有提供 `Shader::setMaximumLOD` / 全局 `maximumLOD` 上限过滤，也没有在渲染管线中按预算切换。

### 1.2 `LightMode = ForwardAdd` 已解析，但管线只画 `ForwardBase`

1. `source/Core/Source/Material/T3DPass.cpp:314-317` 已将 Pass Tag `"LightMode" = "ForwardAdd"` 解析为 `mLightMode = ShaderLab::kBuiltinLightModeForwardAdd`；
2. 然而 `ForwardRenderPipeline::drawCameraQueue`（`T3DForwardRenderPipeline.cpp:663`）只查找 `ShaderLab::kBuiltinLightModeForwardBase`；
3. 同时，`setupLights`（第 910、948 行）将点光源和聚光灯上限硬编码为 `kMaxPointLights = 4`、`kMaxSpotLights = 4`，超出 4 盏时直接打 `Too many point lights in scene !` 并丢弃。

---

## 2. 实现方案（建议实现）

### 2.1 第一部分：Technique LOD 选择机制（对齐 Unity Shader.maximumLOD）

1. **全局与单 Shader 最大 LOD 上限**：
   - 在 `RenderSettings`（`source/Core/Include/Kernel/T3DSettings.h`）增加 `uint32_t maxShaderLOD {UINT32_MAX};`（默认不限制）；
   - 在 `Shader`（`source/Core/Include/Resource/T3DShader.h`）增加 `mMaxLOD {UINT32_MAX}` 与 `setMaxLOD(uint32_t)` / `getMaxLOD()`；
   - 在 `Material` 增加 `selectTechniqueByLOD(uint32_t maxLOD)`：
     - 遍历 `mTechInstances`，选取满足 `tech->getLOD() <= effectiveMaxLOD`（若 `tech->getLOD() == 0` 视为无限制兜底）的首个 Technique（或 LOD 值最大但不超过上限的 Technique）作为 `mCurrentTechInstance`；
     - 若所有 Technique 的 LOD 均超出上限，则回退到 LOD 值最小的那个 Technique。
2. **编辑器 Graphics Tier 菜单联动（可选）**：
   - `UIMainWindow.cpp:458-465` 已有 `Graphics Tier 1 / Tier 2 / Tier 3` 菜单项，可分别对应全局 `maxShaderLOD = 200 / 400 / UINT32_MAX`。

### 2.2 第二部分：`ForwardAdd` 多光源逐光叠加 Pass

1. **光源划分策略**：
   - 在 `ForwardRenderPipeline::setupLights` 中：
     - 前 4 盏点光源与前 4 盏聚光灯仍走 `ForwardBase` Pass 的一次绘制（保持现有 4 光源快路，零额外 DrawCall）；
     - 第 5 盏及之后的点光源/聚光灯归入 `mAdditionalLights` 列表；
     - 或者当材质显式包含 `LightMode = ForwardAdd` Pass 时，支持将额外光源逐批（每批最多 4 盏，或逐盏）通过 `ForwardAdd` Pass 叠加绘制。
2. **`ForwardAdd` Pass 渲染状态与绑定**：
   - 在 `drawCameraQueue` 完成某个 `RenderGroup` 的 `ForwardBase` 绘制后，检查 `mAdditionalLights` 是否非空且 `tech->getPassInstances()` 是否包含 `kBuiltinLightModeForwardAdd`：
   - 若存在 `ForwardAdd` Pass：
     - 渲染状态：若 Pass 自身未指定混合状态，默认强制使用叠加混合（`SrcBlend = kOne`、`DestBlend = kOne`、`DepthWriteEnable = false`、`DepthFunc = kLessEqual`）；
     - 清除环境光与主方向光贡献（避免重复累加），将额外光源填入 `tiny3d_PointLight*` / `tiny3d_SpotLight*` 常量数组，绑定 `ForwardAdd` Pass 并对 `renderables` 提交叠加 DrawCall。

---

## 3. 验收标准

1. **Technique LOD**：编写一个包含两个 SubShader（`LOD 400` 与 `LOD 100`）的测试 Shader，修改 `Shader::setMaxLOD(200)` 时，`Material::getCurrentTechnique()` 自动切到 `LOD 100` 的 SubShader；
2. **ForwardAdd**：在 `LightApp` 中放置 6 盏点光源并为材质添加 `ForwardAdd` Pass，确认第 5、6 盏点光源不再被丢弃，且高光与漫反射正确叠加在物体表面。
