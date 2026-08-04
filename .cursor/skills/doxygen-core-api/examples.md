# Doxygen 示例：必须先读 `.cpp`

以下摘自 `GameObject`。正确注释来自实现行为，不是函数名翻译。

## 反例：凭名字臆造（禁止）

```cpp
// BAD — 未读实现，只翻译函数名
/**
 * \brief 从 mesh 播种包围体
 * \param [in] go : game object
 * \param [in] mesh : mesh
 * \param [in] geometry : geometry
 * \return Bound 指针
 */
static BoundPtr seedBoundFromMesh(GameObject *go, Mesh *mesh, Geometry *geometry);
```

问题：未说明空指针、`Bound::Type::NONE`、已有 Bound 时返回 `nullptr`，也未说明会对 geometry 设剔除包围体。

## 正例：对照 `T3DGameObject.cpp`

实现要点（`seedBoundFromMesh`）：
- `go == nullptr || mesh == nullptr` → `nullptr`
- `mesh->getBoundSeedType() == NONE` → `nullptr`
- 已有任意 `Bound` 组件 → `nullptr`（以组件为准）
- 否则按种子类型创建 Bound，并在 `geometry != nullptr` 时设渲染剔除包围体

```cpp
/**
 * \brief 用 mesh 的包围体种子播种 Bound 组件（仅当无 Bound 时），并设为渲染剔除包围体
 * \param [in] go : 需要挂载 Bound 组件的 GameObject
 * \param [in] mesh : 提供包围体种子的 mesh 资源
 * \param [in] geometry : 需要设置渲染剔除包围体的 geometry，可以为 nullptr
 * \return 播种成功返回新建的 Bound 组件；mesh 没有种子或已存在 Bound 组件时返回 nullptr
 */
static BoundPtr seedBoundFromMesh(GameObject *go, Mesh *mesh, Geometry *geometry);
```

## 正例：`collectHierarchy`

实现要点：
- `root == nullptr` 直接返回
- 有 `TransformNode` 时 `visitAll` 收集整棵子树
- 无 `TransformNode` 时只收集自身

```cpp
/**
 * \brief 从根节点出发，收集整棵子树的所有 GameObject 到扁平表
 * \param [in] root : 子树根节点
 * \param [out] out : 收集结果，key 为 UUID，value 为 GameObjectPtr
 * \note 若节点没有 TransformNode 则只收集自身，不继续遍历子节点
 */
static void collectHierarchy(GameObject *root, GameObjects &out);
```

## 正例：`isActiveInHierarchy`（含可选 remarks）

实现：自身 `mIsActive` 为 false 则 false；否则沿父 `TransformNode` 链检查祖先 `isActive()`。

```cpp
/**
 * \brief 自身与所有祖先是否都处于激活状态（对标 Unity 的 activeInHierarchy）
 * \remarks 场景树遍历（visitActive）本身就会在祖先失活时裁掉整棵子树，因此
 *          只有不走遍历、从独立登记表里取到的组件（如相机）才需要它
 */
bool isActiveInHierarchy() const;
```

`\remarks` 中的调用场景须能从代码或邻近注释证实；不要为「显得完整」编造使用场景。

## 检查清单（每个 API）

1. 已打开对应 `.cpp`（或内联体）并读完定义
2. `\brief` / `\param` / `\return` 都能在实现中找到依据
3. 空指针、早退、错误码已写明（若实现有）
4. 没有只把英文标识符翻译成中文的空洞 brief
