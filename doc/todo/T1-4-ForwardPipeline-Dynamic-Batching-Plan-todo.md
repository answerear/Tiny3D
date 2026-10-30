# T1-4 实现计划：前向渲染管线动态合批（`ForwardRenderPipeline::setupBatch`）

> **所属批次**：T1（引擎渲染管线与动画半成品补齐）  
> **预估工作量**：3.0 人日  
> **涉及文件**：
> - `source/Core/Include/Kernel/T3DSettings.h`
> - `source/Core/Include/Render/T3DForwardRenderPipeline.h`
> - `source/Core/Source/Render/T3DForwardRenderPipeline.cpp`
> - `source/Samples/CMakeLists.txt`
> - `source/Samples/DynamicBatchApp/CMakeLists.txt`（新增）
> - `source/Samples/DynamicBatchApp/DynamicBatchApp.h`（新增）
> - `source/Samples/DynamicBatchApp/DynamicBatchApp.cpp`（新增）

---

## 1. 现状分析（代码依据）

1. **配置项与函数声明早已存在，但无实现**：
   - `source/Core/Include/Kernel/T3DSettings.h` 的 `RenderSettings` 中已定义 `bool enableDynamicBatch {false};`；
   - `source/Core/Include/Render/T3DForwardRenderPipeline.h:129` 已声明 `TResult setupBatch();`，注释为「动态合批预处理（头文件已声明，当前无 .cpp 实现）」；
   - `source/Core/Source/Render/T3DForwardRenderPipeline.cpp:166` 在 `addRenderable` 中虽然写了注释 `// 已有材质，合并 draw call`，但实际只是 `it->second.emplace_back(renderable);`，在 `drawCameraQueue`（第 688–718 行）中仍然是 `for (const auto &renderable : renderables)` 逐个设置 `tiny3d_ObjectToWorld`、绑定各自的 VB/IB 并提交独立 DrawCall。
2. **RHI 侧硬件实例化（Instancing）已就绪**：
   - `RHI-Compute` 一期（A1–A9）已在全部 5 个窗口渲染后端（D3D11 / GL4 / GLES3 / Vulkan / Metal）实现了 `renderIndexedInstanced` / `renderInstanced` 与 `VertexAttribute::InputRate::kPerInstance`。

---

## 2. 方案选型：CPU 动态顶点合并 vs 自动硬件 Instancing

针对 `mRenderQueue` 中同一 `(Camera, Queue, Material)` 分组下的 `Renderables` 列表，提供两级合批能力：

| 方式 | 适用条件 | 原理 | 优点 |
|------|----------|------|------|
| **路径 A：CPU 动态网格合批（Dynamic Mesh Batching）** | 相同 `Material*`、不同 `Mesh`（或同 Mesh）、单物体顶点数 $\le 300$、单一 VB Stream、无骨骼蒙皮、缩放符号一致（无负缩放翻转绕序） | CPU 侧将各物体的顶点 Position/Normal/Tangent 乘上各自的 `LocalToWorld` 变换到世界空间，写入一块共享的 `Usage::kDynamic` 顶点/索引缓冲，将 `tiny3d_ObjectToWorld` 设为单位矩阵，1 次 `ctx->render(...)` 画出整批 | **无需修改现有 Shader**（直接兼容 `Tiny3DStandard.shader`） |
| **路径 B：同网格硬件实例化（Auto GPU Instancing）** | 相同 `Material*`、**相同 `VertexBuffer*` 与 `IndexBuffer*`**、材质开启 Instancing 关键字 | 将各实例的 `LocalToWorld` 矩阵写入 slot 1 的 Per-Instance VB，调用 `ctx->renderIndexedInstanced` | CPU 零顶点变换开销，适合成百上千个相同 Cube/Sphere |

**本期（T1-4）以「路径 A（CPU 动态合批）」为主实现**，因为它对现有 `.shader` 零侵入，开启 `enableDynamicBatch = true` 后即可直接对同材质小网格进行合批降 DrawCall。为避免污染现有各司其职的样例（`GeometryApp` 专用于验证 `Geometry` 组件基础渲染、`SceneApp` 专用于验证视锥场景剔除），本期新建独立样例 **`DynamicBatchApp`** 专门验证动态合批。

---

## 3. 详细实现设计（路径 A：CPU 动态合批）

### 3.1 合批准入条件判定 `canDynamicBatch(const Renderable *r)`

满足以下全部条件的 `Renderable` 才进入动态合批池，否则回退走原有的单物体独立绘制：
1. `T3D_AGENT.getSettings().renderSettings.enableDynamicBatch == true`；
2. 图元类型为 `PrimitiveType::kTriangleList`；
3. 非骨骼蒙皮网格（`r->getVertexBuffers().size() == 1` 且不含 `E_VAS_BLENDWEIGHT` / `E_VAS_BLENDINDICES`）；
4. 顶点数量 `vertexCount <= kMaxDynamicBatchVerticesPerObject`（取 `300`），且合并后总顶点数不超过单批上限 `kMaxBatchTotalVertices`（取 `16384`，保证 `uint16_t` 索引不溢出）；
5. 所属 `Transform3D` 的世界矩阵行列式符号一致（不存在一个正缩放、一个镜像负缩放混在同一批导致背面剔除反转）。

### 3.2 共享动态缓冲池（`ForwardRenderPipeline` 成员）

在 `ForwardRenderPipeline` 中维护按顶点格式（`VertexDeclaration` 哈希）缓存的动态 VB 和共享 16 位动态 IB：

```cpp
/// 动态合批单批最大顶点数（控制在 65535 以内以使用 16 位索引）
static constexpr uint32_t kMaxBatchVertices = 16384;
/// 动态合批单批最大索引数
static constexpr uint32_t kMaxBatchIndices  = 49152;
/// 单个物体允许参与动态合批的最大顶点数
static constexpr uint32_t kMaxBatchObjectVertices = 300;
```

### 3.3 `setupBatch()` 与绘制流程改造

1. 在 `drawCameraQueue`（及 `renderShadowMap`）遍历 `RenderGroup`（同一 `Material*` 下的 `const Renderables &renderables`）时：
   - 若 `!enableDynamicBatch || renderables.size() <= 1`：直接走原有逐物体循环；
   - 若 `enableDynamicBatch && renderables.size() > 1`：
     - 将满足 `canDynamicBatch` 且 `VertexDeclaration` 相同的物体归入合批桶；
     - 在 CPU 侧遍历桶内每个 `Renderable`：
       - 从 `Mesh` / `SubMesh` 的 CPU 侧 `Buffer`（`mesh->getVertices()[0]` 与 `submesh->getIndices()`）读取原始顶点与索引；
       - 将 `POSITION` 乘以 `worldMatrix`，将 `NORMAL` / `TANGENT` 乘以 `worldInvTranspose` 并归一化，其余属性（`TEXCOORD` / `COLOR`）原样拷贝到批处理 CPU 缓冲；
       - 将索引加上当前批的 `baseVertexOffset` 写入批处理索引缓冲；
     - 调用 `mBatchVB->writeData(0, batchVBData, true)` 与 `mBatchIB->writeData(0, batchIBData, true)`；
     - 将材质的 `tiny3d_ObjectToWorld` 与 `tiny3d_WorldToObject` 设为 `Matrix4::IDENTITY`，更新一次 VS 常量缓冲；
     - 绑定 `mBatchVB` 与 `mBatchIB`，执行**单次** `ctx->render(totalIndices, 0, 0)`！
     - 不满足合批条件的剩余物体仍走原有逐物体循环。

---

### 3.4 新增独立验证样例 `DynamicBatchApp`

为保持职责单一，不修改 `SceneApp`（用于验证场景视锥剔除）和 `GeometryApp`（用于验证 `Geometry` 组件基础渲染），在 `source/Samples/DynamicBatchApp/` 下新建独立样例工程：
1. 在 `source/Samples/CMakeLists.txt` 中注册 `add_subdirectory(DynamicBatchApp)`；
2. 参照 `ShadowApp` 搭建带方向光、地面 `Plane`、`ForwardBase` + `ShadowCaster` 双 Pass 的场景；
3. 在场景中放置 20 个共享同一材质（`mCubeMaterial`）且具有不同位移/旋转/正缩放的 Cube；
4. 开启 `enableDynamicBatch`（并支持按键或配置切换开关对比），验证合批前后画面一致性与 DrawCall 合并效果。

---

## 4. 验收标准

1. 新建独立样例 `DynamicBatchApp`（不污染 `SceneApp` 与 `GeometryApp`），在其中开启 `enableDynamicBatch`，放置 20 个共享同一材质的 Cube；
2. 用 RenderDoc 抓帧确认：`DynamicBatchApp` 中 20 个 Cube 在 `ForwardBase` Pass 和 `ShadowCaster` Pass 中均被合并为 **1 次 `DrawIndexed` 调用**，且画面光照、法线、阴影与未开启合批时像素级一致。
