# T4-3 实现计划：Hierarchy 与 Project 窗口工具栏按钮、右键菜单及资产拖拽接线

> **所属批次**：T4（编辑器 `TinyEditor` 面板与菜单断点接通）  
> **预估工作量**：2.5 人日  
> **涉及文件**：
> - `source/Editor/TinyEditor/UIHierarchyWindow.h` / `UIHierarchyWindow.cpp`
> - `source/Editor/TinyEditor/UIProjectWindow.h` / `UIProjectWindow.cpp`
> - `source/Editor/TinyEditor/GUIExtension/ImPropertyDrawer.cpp`

---

## 1. 现状分析（代码依据）

### 1.1 `UIHierarchyWindow` 的三处空回调

核查 `source/Editor/TinyEditor/UIHierarchyWindow.cpp`：
1. **左上角 `+` 创建按钮（`ID_HIERARCHY_ADD_BUTTON`）回调为空**（第 48–52 行）：
   ```cpp
   auto clickedAdd = [](ImWidget *button)
   {
   };
   ```
   而实际上 `UIHierarchyView::onCreate()`（第 217–234 行）早已写好了完整的 `onMenuItemCreateEmpty`、`onMenuItemCreateCube`、`onMenuItemCreateSphere`、`onMenuItemCreateCapsule`、`onMenuItemCreateCylinder`、`onMenuItemCreateQuad`、`onMenuItemCreatePlane`、`onMenuItemDelete`！
2. **搜索输入框（`ID_HIERARCHY_SEARCH_INPUT`）只打日志不筛树**（第 64–68 行）：
   ```cpp
   auto inputTextCallback = [](ImInputText *inputText, const String &text)
   {
       EDITOR_LOG_DEBUG("Input text : %s", text.c_str());
       return 0;
   };
   ```
3. **树节点右键点击 `treeNodeRClicked` 为空**（第 450–453 行）：
   ```cpp
   void UIHierarchyView::treeNodeRClicked(ImTreeNode *node)
   {
   }
   ```
   无法在 Hierarchy 里右键创建子物体或删除选中物体。

### 1.2 `UIProjectWindow` 的三处断点（含拖拽源缺失）

核查 `source/Editor/TinyEditor/UIProjectWindow.cpp` 与 `GUIExtension/ImPropertyDrawer.cpp`：
1. **Inspector 已有拖拽接收端，但 Project 面板没有拖拽发射端**：
   - `ImPropertyDrawer.cpp` 已经实现了 `ImGui::BeginDragDropTarget()` / `AcceptDragDropPayload(...)` 来接收材质、纹理、网格等资产 UUID；
   - 但 `UIProjectWindow.cpp:1170-1264` 的 `UIAssetDetailView::createListItem` 在创建资产图标项 `ImListItem` 时，**完全没有设置 `ImGui::BeginDragDropSource()`**，导致用户无法从 Project 面板把材质/网格/贴图拖到 Inspector 属性槽里！
2. **左上角 `+` 按钮（`ID_PROJECT_TOOLBAR_ADD_BUTTON`）回调为空**（第 118–120 行）：
   - 实际上 `UIAssetHierarchyView`（第 251–264 行）已经建好了包含 `Folder` 和 `C++ Class` 的 `mContextMenu`，只是没接到 `+` 按钮上。
3. **右侧资产项双击/点击无动作**（第 1175–1178 行）：
   - 点击或双击 `.tscene` 场景文件、`.cpp/.h` 源码文件或文件夹时，只打印了一句 `List item [...] clicked`。

---

## 2. 实现方案（建议实现）

### 2.1 `UIHierarchyWindow` 接线

1. **创建 Hierarchy 上下文菜单 `mContextMenu`**：
   - 在 `UIHierarchyView::onCreate()` 中创建 `ImContextMenu`（ID 如 `ID_HIERARCHY_CONTEXT_MENU`），挂载已有菜单项：
     - `Create Empty` (`ID_MENU_ITEM_CREATE_EMPTY`)
     - `3D Object` 子菜单：`Cube` / `Sphere` / `Capsule` / `Cylinder` / `Quad` / `Plane`
     - 分隔线
     - `Delete` (`ID_MENU_ITEM_DELETE`)
   - 在 `UIHierarchyView::treeNodeRClicked(ImTreeNode *node)` 中：先将 `node` 设为当前选中项（`mTreeWidget->setSelection(node); postSelectionChanged(node);`），再调用 `mContextMenu->show(this);`。
   - 在 `UIHierarchyToolBar` 的 `+` 按钮回调中，同样弹出该上下文菜单（若当前无选中节点，默认以场景根节点 `mRoot` 作为父节点）。
2. **Hierarchy 搜索框实时过滤**：
   - 在 `UIHierarchyView` 增加 `String mSearchFilter;`；
   - `inputTextCallback` 更新 `mSearchFilter`（不区分大小写子串匹配）：
     - 遍历树节点：若节点自身名称匹配或任一后代节点匹配，则置 `setVisible(true)` 并自动展开父节点；否则置 `setVisible(false)`。

### 2.2 `UIProjectWindow` 接线与 Drag & Drop 发射源

1. **补齐 `ImListItem` 的 `BeginDragDropSource`（打通资产拖拽）**：
   - 先核对 `ImPropertyDrawer.cpp` 中 `AcceptDragDropPayload` 约定的 Payload Type 字符串与数据结构（例如携带 `AssetNode*` 或资源的 `UUID` / 相对路径）；
   - 在 `ImListItem` 绘制后（或通过 `ImListItem` 的拖拽回调扩展），当 `node->getMeta()` 为有效资产（`kMesh` / `kMaterial` / `kTexture` / `kShader` / `kShaderLab` / `kPrefab` / `kScene`）时：
     ```cpp
     if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
     {
         const UUID &uuid = node->getMeta()->getUUID();
         ImGui::SetDragDropPayload(payloadType, &uuid, sizeof(UUID));
         ImGui::TextUnformatted(node->getTitle().c_str());
         ImGui::EndDragDropSource();
     }
     ```
   - 同步支持将 `.tprefab` 或 `.tmesh` 从 Project 面板拖入 `UIHierarchyView` 直接实例化到场景中！
2. **接通 Project 工具栏 `+` 按钮与搜索框**：
   - `+` 按钮点击时调用 `mHierarchyView` 的创建菜单（在当前选中的目录节点下新建文件夹或 C++ 类）；
   - 搜索框输入时过滤 `UIAssetDetailView::populateItems` 展示的 `ImListItem` 列表。
3. **接通右侧 `ImListItem` 双击交互**：
   - 双击 `Meta::Type::kFolder`：进入该子文件夹并同步左侧目录树与顶部 `UIAssetPathBar`；
   - 双击 `Meta::Type::kScene`：提示保存当前脏场景后调用 `PROJECT_MGR.openScene(...)` 切换场景；
   - 双击 `.h` / `.cpp` 文件：调用 `ExternalIDELauncher` 打开对应源码文件。

---

## 3. 验收标准

1. 在 Hierarchy 中点击左上角 `+` 或右键节点，均可弹出菜单创建几何体与删除节点；输入搜索词可实时过滤层级树；
2. 从 Project 面板右侧将 `.tmat` / `.ttex` / `.tmesh` 拖拽到 Inspector 对应槽位，属性立即更新并生效；
3. 在 Project 右侧双击文件夹可进入下一级目录。
