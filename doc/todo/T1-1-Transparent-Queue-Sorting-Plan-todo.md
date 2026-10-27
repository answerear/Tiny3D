# T1-1 实现计划：前向渲染管线透明队列从后往前排序（Back-to-Front）

> **所属批次**：T1（引擎渲染管线与动画半成品补齐）  
> **预估工作量**：1.5 人日  
> **关联文档**：`doc/todo/Skybox-Support-Design-todo.md` §10（已登记为已知问题）  
> **涉及文件**：
> - `source/Core/Include/Render/T3DForwardRenderPipeline.h`
> - `source/Core/Source/Render/T3DForwardRenderPipeline.cpp`

---

## 1. 现状分析（代码依据）

在 `source/Core/Source/Render/T3DForwardRenderPipeline.cpp` 中：

1. **入队结构按材质分组，抹平了空间顺序**：
   - `addRenderable(Camera *camera, Renderable *renderable)`（第 124–172 行）将所有物体一律装入 `CameraRenderQueue`：`TMap<Camera*, TMap<uint32_t /*queue*/, TMap<Material*, TList<Renderable*>>>>`。
2. **绘制时透明队列与不透明队列走同一套循环**：
   - `drawCameraQueue`（第 619–729 行）按 `queue` 从小到大遍历：
     - `queue < kBuiltinQueueTransparent (3000)`（Background / Geometry / AlphaTest）按材质分组绘制是正确的，能减少状态与 Shader 切换，前后遮挡由 Z-Buffer 硬件解决；
     - 在 `itemQueue.first >= ShaderLab::kBuiltinQueueTransparent` 处插入了 `renderSkybox`（第 630–636 行）；
     - 但随后的透明队列（`Transparent = 3000`、`Overlay = 4000`）依然按 `Material*` 指针顺序遍历绘制！由于半透明材质通常关闭深度写入（`ZWrite Off`）并开启 Alpha Blend，**先画近处半透明物体、后画远处半透明物体会导致混合颜色错误**。

---

## 2. 实现方案（建议实现）

### 2.1 区分不透明队列与透明队列的绘制策略

- **不透明队列（`queue < ShaderLab::kBuiltinQueueTransparent`）**：
  - 保持现有按 `Material*` 聚合绘制的快路不变（最小化材质/PSO 切换）。
- **透明队列（`queue >= ShaderLab::kBuiltinQueueTransparent`）**：
  - 在每个透明子队列（如 3000、3001...、4000）内部，将该队列下所有 `RenderGroup` 中的 `Renderable*` 展开到临时数组 `TArray<TransparentItem>`，按**到相机的距离从远到近（Back-to-Front）**排序后逐个绘制。

### 2.2 距离度量算法

为兼顾相机正交/透视投影与旋转稳定性，采用**沿相机观察方向的深度投影距离**（或世界坐标到相机位置的距离平方 `distanceSq`）：

```cpp
struct TransparentSortItem
{
    Renderable *renderable {nullptr};
    Material   *material   {nullptr};
    Real        distanceSq {REAL_ZERO};
};
```

计算方法：
1. 优先取 `renderable->getRenderBound()` 的世界空间中心点（若无 Bound，则取 `renderable->getGameObject()->getTransformNode()->getLocalToWorldTransform().getTranslation()`）；
2. 计算 `(worldCenter - cameraWorldPos.xyz()).lengthSquare()`；
3. 使用 `std::stable_sort` 按 `a.distanceSq > b.distanceSq`（从远到近）排序；距离相等时保留入队顺序以防帧间闪烁。

### 2.3 相邻同材质状态复用优化

排序后逐个绘制 `TransparentSortItem` 时，记录上一个元素的 `Material *lastMaterial = nullptr`：
- 若当前 `item.material != lastMaterial`，才执行 `setupForwardPassMatrices`、`setupLights`、`setupRenderState`、`setupShaders`；
- 若连续两个远近相邻的透明物体恰好使用同一个 `Material*`，只更新 `setupWorldMatrix` 并提交 `ctx->render(...)`，兼顾排序正确性与状态切换开销。

### 2.4 ShadowMap Pass 过滤半透明物体

顺手检查 `ForwardRenderPipeline::renderShadowMap`（第 402–486 行）：
- 透明队列（`itemQueue.first >= ShaderLab::kBuiltinQueueTransparent`）通常不应作为不透明阴影投射体参与 `renderShadowMap`（除非材质显式定义了 `ShadowCaster` Pass）。目前第 428 行已有 `tech->getPassInstances().find(ShaderLab::kBuiltinLightModeShadowCaster)` 检查，保持该守卫即可。

---

## 3. 验收标准

1. 在场景中沿相机视线前后放置 3 个不同颜色的半透明平面（`Queue = Transparent`、`ZWrite Off`、`Blend SrcAlpha OneMinusSrcAlpha`），故意按「从近到远」顺序创建并挂上不同材质实例；
2. 正面与背面观察时，半透明叠加颜色均符合正确的画家算法（远处背景透出正确，转动相机 180 度后前后叠加关系自动翻转且无闪烁）。
