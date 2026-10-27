# T4-4 实现计划：GameObject 菜单补齐 Camera 与四类 Light 创建

> **所属批次**：T4（编辑器 `TinyEditor` 面板与菜单断点接通）  
> **预估工作量**：1.0 人日  
> **涉及文件**：
> - `source/Editor/TinyEditor/UIEditorWidgetID.h`
> - `source/Editor/TinyEditor/UIMainWindow.cpp`
> - `source/Editor/TinyEditor/UIHierarchyWindow.h`
> - `source/Editor/TinyEditor/UIHierarchyWindow.cpp`

---

## 1. 现状分析（代码依据）

1. **引擎侧组件早已完备**：
   - `Camera`（`source/Core/Include/Component/T3DCamera.h`）
   - `DirectionalLight`（`source/Core/Include/Light/T3DDirectionalLight.h`）
   - `PointLight`（`source/Core/Include/Light/T3DPointLight.h`）
   - `SpotLight`（`source/Core/Include/Light/T3DSpotLight.h`）
   - `AmbientLight`（`source/Core/Include/Light/T3DAmbientLight.h`）
   以上 5 个组件均已标注 `TCLASS()` 并注册进 RTTR，Inspector 也能完整绘制它们的属性。
2. **菜单栏中 `Light` 与 `Camera` 被置灰且无子项**：
   - 在 `source/Editor/TinyEditor/UIMainWindow.cpp:658-670` 与 `1214-1225` 中：
     ```cpp
     // Light
     IM_MENU_ITEM_DATA(ImMenuItemType::kPopup, ID_MENU_ITEM_LIGHT, STR(TXT_LIGHT), "", "", queryDisableDefault, nullptr, nullptr)
     ...
     // Camera
     IM_MENU_ITEM_DATA(ImMenuItemType::kNormal, ID_MENU_ITEM_CAMERA, STR(TXT_CAMERA), "", "", queryDisableDefault, nullptr, nullptr)
     ```
   - `ID_MENU_ITEM_LIGHT` 弹窗为空，`ID_MENU_ITEM_CAMERA` 绑定在 `queryDisableDefault` 上，导致用户在编辑器里新建灯光或相机只能先建 Empty GameObject 再去 Inspector 手动 Add Component。

---

## 2. 实现方案（建议实现）

### 2.1 新增菜单项 Widget ID（`UIEditorWidgetID.h`）

在 `ID_MENU_ITEM_LIGHT` 附近补充 4 个子菜单项 ID：
- `ID_MENU_ITEM_CREATE_DIRECTIONAL_LIGHT`（"Directional Light"）
- `ID_MENU_ITEM_CREATE_POINT_LIGHT`（"Point Light"）
- `ID_MENU_ITEM_CREATE_SPOT_LIGHT`（"Spot Light"）
- `ID_MENU_ITEM_CREATE_AMBIENT_LIGHT`（"Ambient Light"）
（`ID_MENU_ITEM_CAMERA` 已存在，直接使用）

### 2.2 在 `UIMainWindow.cpp` 注册菜单数据与构建层级

1. `createGameObjectMenuItemData`（第 658–670 行）：
   - 将 `ID_MENU_ITEM_LIGHT` 的 query 回调设为 `nullptr`（作为 Popup 父菜单）；
   - 使用 `IM_MENU_ITEM_DATA_DEFAULT` 注册上述 4 个光源子项与 `ID_MENU_ITEM_CAMERA`。
2. `buildGameObjectMenu`（第 1214–1225 行）：
   ```cpp
   IM_MENU_POPUP_BEGIN(ID_MENU_ITEM_LIGHT) // Light
       IM_MENU_ITEM(ID_MENU_ITEM_CREATE_DIRECTIONAL_LIGHT)
       IM_MENU_ITEM(ID_MENU_ITEM_CREATE_POINT_LIGHT)
       IM_MENU_ITEM(ID_MENU_ITEM_CREATE_SPOT_LIGHT)
       IM_MENU_ITEM(ID_MENU_ITEM_CREATE_AMBIENT_LIGHT)
   IM_MENU_POPUP_END()
   ```
   （若已实施 T4-3 的 Hierarchy 右键菜单，同步将 `Light` 子菜单与 `Camera` 挂入 Hierarchy 右键菜单）。

### 2.3 在 `UIHierarchyView` 实现创建回调并收敛重复代码

核查 `UIHierarchyWindow.cpp` 发现，现有的 `onMenuItemCreateEmpty`、`onMenuItemCreateCube`、`onMenuItemCreateSphere`、`onMenuItemCreateCapsule`、`onMenuItemCreateCylinder`、`onMenuItemCreateQuad`、`onMenuItemCreatePlane` 有 **7 份高度重复的 60 行样板代码**（取选中父节点 → `GameObject::create` → `addComponent<Transform3D>` → 挂父节点 → 建 `ImTreeNode` → 展开父节点 → 通知 Inspector → 发 `kEvtModifyScene`）。

借本次改动，在 `UIHierarchyView` 抽出一个通用私有辅助方法：

```cpp
// 建议实现
GameObjectPtr createChildGameObjectUnderSelection(
    const String &defaultName,
    const std::function<TResult(GameObject *)> &setupComponents);
```

然后用它极简实现 5 个新回调：
1. **`onMenuItemCreateCamera`**：
   - 创建 `"Camera"`，挂载 `Transform3D` + `Camera`（默认设置 `perspective(45, aspect, 0.1f, 1000.0f)`，若当前场景有游戏窗口 RT 则自动绑定 `setRenderTarget(EDITOR_SCENE.getGameRenderTarget())`）。
2. **`onMenuItemCreateDirectionalLight`**：
   - 创建 `"Directional Light"`，挂载 `Transform3D`（默认俯仰旋转如 X=50°, Y=-30°）+ `DirectionalLight`。
3. **`onMenuItemCreatePointLight`**：
   - 创建 `"Point Light"`，挂载 `Transform3D` + `PointLight`（默认衰减参数 `1.0f, 0.09f, 0.032f`）。
4. **`onMenuItemCreateSpotLight`**：
   - 创建 `"Spot Light"`，挂载 `Transform3D` + `SpotLight`（默认内外切角 `25° / 35°`）。
5. **`onMenuItemCreateAmbientLight`**：
   - 创建 `"Ambient Light"`，挂载 `Transform3D` + `AmbientLight`。

---

## 3. 验收标准

1. 在 `GameObject -> Light` 下可点击创建 4 种光源，在 `GameObject -> Camera` 可点击创建相机；
2. 新创建的节点立即出现在 Hierarchy 选中节点下并自动选中，Inspector 正确展示对应光源/相机属性，Scene 视图光照实时变化。
