# T4-5 实现计划：Scene 视口交互（编辑器相机漫游 + 射线点选 + 3D Gizmo）

> **所属批次**：T4（编辑器 `TinyEditor` 面板与菜单断点接通）  
> **预估工作量**：3.5 人日  
> **涉及文件**：
> - `source/Editor/TinyEditor/UISceneWindow.h` / `UISceneWindow.cpp`
> - `source/Editor/TinyEditor/EditorSceneImpl.h` / `EditorSceneImpl.cpp`
> - `source/Core/Include/Component/T3DCamera.h` / `Source/Component/T3DCamera.cpp`

---

## 1. 现状分析（代码依据）

核查 `source/Editor/TinyEditor/UISceneWindow.cpp`（共 75 行）与 `EditorSceneImpl.h/.cpp`：

1. **`UISceneWindow::onGUI()` 只做纯图片展示**：
   - 第 41–52 行绘制了 `Shading Mode`（`Shaded` / `Wireframe` / `Shaded Wireframe`）下拉框；
   - 第 54–69 行计算了居中裁剪 UV 并调用 `ImGui::Image(EDITOR_SCENE.getSceneRT(), region, uv0, uv1);`；
   - **完全没有任何鼠标/键盘事件处理**：无法在 Scene 视口内漫游或旋转编辑器相机，无法点击选中 3D 物体，也没有平移/旋转/缩放 Gizmo 手柄。
2. **底层数学与相交检测其实已经齐备**：
   - `T3DMath` 已有 `Ray`、`Plane`、`Aabb`、`Sphere`、`Obb`、`Math::intersects(ray, aabb/sphere/obb)`（`IntersectionApp` 已验证）；
   - `Bound` 组件（`AabbBound` / `SphereBound` / `ObbBound`）已挂在每个网格 `GameObject` 上；
   - `EDITOR_SCENE.getEditorCamera()` 已持有独立的编辑器相机 `Camera*` 与其所属 `Transform3D*`。

---

## 2. 分步实现方案（建议实现）

### 2.1 第一步：修复 Scene RT 尺寸自适应与视口矩形记录（0.5 d）

- 当前 `UISceneWindow::onGUI()` 用固定尺寸 RT 做 UV 裁剪，当窗口拉大超过 RT 尺寸时 UV 会越界拉伸；应参照 `UIGameView::onGUI()`（`UIGameWindow.cpp:95`），按 `ImGui::GetContentRegionAvail()` 动态调整或匹配编辑器相机宽高比（`camera->setAspectRatio(region.x / region.y)`）；
- 在 `ImGui::Image` 之后立即记录视口图像的屏幕像素矩形：
  ```cpp
  mViewportMin = ImGui::GetItemRectMin();
  mViewportMax = ImGui::GetItemRectMax();
  mIsViewportHovered = ImGui::IsItemHovered();
  ```

### 2.2 第二步：编辑器相机漫游控制（对齐 Unity Scene View 操控）（1.0 d）

当鼠标悬停在 Scene 图像区域（`mIsViewportHovered`）开启操控，拖拽过程中即使鼠标移出区域也保持捕获，直到松开按键：

1. **右键按住 + 鼠标移动 + WASD/QE（Flythrough 漫游模式）**：
   - 鼠标 `io.MouseDelta.x / y` 控制编辑器相机的 Yaw（绕世界 Y 轴）与 Pitch（绕局部 X 轴，夹取在 $[-89^\circ, +89^\circ]$）；
   - 按住右键期间：
     - `W / S`：沿相机前向（Forward）前后移动；
     - `A / D`：沿相机右向（Right）左右移动；
     - `E / Q`：沿世界或相机上向（Up）上下升降；
     - 按住 `Shift` 加速（如 $3\times$），滚轮滚动调整基础漫游速度 `mFlySpeed`。
2. **鼠标中键按住拖拽（Pan 平移）**：
   - 沿相机局部的 Right 与 Up 轴平移相机位置。
3. **Alt + 左键按住拖拽（Orbit 围绕枢轴旋转）**：
   - 围绕当前聚焦点 `mPivotPoint`（选中物体中心或默认前方距离点）做球面旋转。
4. **鼠标滚轮（Zoom 推拉）**：
   - 沿相机前向推拉相机位置。
5. **快捷键 `F`（Frame Selected 聚焦选中物体）**：
   - 将 `mPivotPoint` 设为当前选中 `GameObject` 的世界坐标（若有 `Bound` 则取包围球半径 $R$），将编辑器相机平滑或立即移动到 `center - forward * (R * 2.5f)`。

### 2.3 第三步：视口鼠标左键射线点选（Ray Picking）（1.0 d）

当在 Scene 视口内**左键单击**（区分于拖拽和 Gizmo 操作，`!ImGuizmo::IsOver()` 且鼠标按下到抬起位移 $< 3$ 像素）时：

1. **屏幕坐标转世界射线 `screenPointToRay`**：
   - 计算归一化视口坐标：
     $$u = \frac{\text{mouseX} - \text{minX}}{\text{maxX} - \text{minX}} \in [0, 1], \quad v = \frac{\text{mouseY} - \text{minY}}{\text{maxY} - \text{minY}} \in [0, 1]$$
   - 转换为 NDC 坐标：$x_{\text{ndc}} = 2u - 1, \quad y_{\text{ndc}} = 1 - 2v$；
   - 利用编辑器相机的 `(Proj * View).inverse()` 将近平面点 $(x_{\text{ndc}}, y_{\text{ndc}}, -1, 1)$ 与远平面点 $(x_{\text{ndc}}, y_{\text{ndc}}, 1, 1)$ 反投影回世界空间，构造 `Ray(origin, direction)`。
2. **场景树包围体求交测试**：
   - 遍历 `T3D_SCENE_MGR.getCurrentScene()->getRootGameObject()` 下的激活节点（`visitActive`）；
   - 对带有 `Bound` 组件（`AabbBound` / `SphereBound` / `ObbBound`）的节点，执行射线求交并计算命中距离 $t \ge 0$；
   - 取距离相机最小正值 $t_{\min}$ 的 `GameObject*`（若均未命中则为 `nullptr`）；
   - 发送 `EventParamGameObjectSelected`（`kEvtGameObjectSelected`），并通知 `UIHierarchyWindow` 同步高亮对应树节点、通知 `UIInspectorWindow` 刷新属性面板。

### 2.4 第四步：3D 变换手柄（Translate / Rotate / Scale Gizmo）（1.0 d）

1. **实现方式**：
   - 引入单头文件/单源文件库 `ImGuizmo`（MIT 协议，仅 `ImGuizmo.h` + `ImGuizmo.cpp` 两个文件，放于 `source/External/imgui/` 或 `Editor/TinyImGui/`），或者用 `ImGui::GetWindowDrawList()` 绘制三轴变换手柄；
2. **工具栏与快捷键（对标 Unity `W / E / R`）**：
   - 在 `UISceneWindow` 顶部 `Shading Mode` 左侧增加工具切换按钮组：
     - `Q`：View（观察模式，不画 Gizmo）
     - `W`：Translate（三轴平移手柄）
     - `E`：Rotate（三轴旋转球手柄）
     - `R`：Scale（三轴缩放手柄）
     - 以及 `Local / World` 坐标系切换按钮。
3. **变换写回与脏标记**：
   - 当选中 `GameObject` 存在 `Transform3D` 时，读取其世界或局部矩阵喂给 Gizmo；
   - 拖拽发生修改时，分解或反算出新的局部 `Position` / `Orientation` / `Scaling` 写回 `Transform3D`，并发送 `kEvtModifyScene` 标记场景已修改（若 T4-6 Undo 栈已就位，在松开鼠标时压入一条 `TransformCommand`）。

---

## 3. 验收标准

1. 在 Scene 窗口中按住右键 + WASD 可自由飞行漫游，滚轮推拉、中键平移流畅；
2. 左键点击场景中的 Cube / Sphere 可直接选中，Hierarchy 与 Inspector 同步切换；点击空白处取消选中；
3. 按 `W / E / R` 切换平移/旋转/缩放手柄，拖拽三轴可实时改变物体变换，Inspector 中 `Transform3D` 数值同步跳动。
