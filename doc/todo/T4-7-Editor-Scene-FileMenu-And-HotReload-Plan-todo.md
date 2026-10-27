# T4-7 实现计划：File 菜单场景新建/打开/另存为及资产热重载

> **所属批次**：T4（编辑器 `TinyEditor` 面板与菜单断点接通）  
> **关联文档**：`doc/todo/MetaFS-Monitor-Fixes-todo.md`  
> **预估工作量**：2.5 人日  
> **涉及文件**：
> - `source/Editor/TinyEditor/UIMainWindow.h` / `UIMainWindow.cpp`
> - `source/Editor/TinyEditor/ProjectManager.h` / `ProjectManager.cpp`
> - `source/Plugins/Archive/MetaFileSystem/Source/T3DMetaFSMonitor.cpp`

---

## 1. 现状分析（代码依据）

### 1.1 `File` 菜单中场景操作仅 `Save` 可用，其余全部置灰

核查 `source/Editor/TinyEditor/UIMainWindow.cpp:311-336`：
- `ID_MENU_ITEM_SAVE (Ctrl+S)` 已接通 `onMenuItemSave` → `PROJECT_MGR.saveProject()`；
- 但同组的以下菜单项全部绑定在 `queryDisableDefault` 上，无任何回调：
  - `ID_MENU_ITEM_NEW_SCENE (Ctrl+N)`（新建场景）
  - `ID_MENU_ITEM_OPEN_SCENE (Ctrl+O)`（打开场景）
  - `ID_MENU_ITEM_SAVE_AS (Ctrl+Shift+S)`（场景另存为）
- 同时，`Assets -> Create -> Scene`（`ID_MENU_ITEM_SCENE`，第 510 行）也处于 `queryDisableDefault` 状态。
- 而实际上，`TinyImGui` 已经提供了文件对话框控件 `ImOpenFileDialog`（`source/Editor/TinyImGui/Include/ImOpenFileDialog.h`），`ProjectManager` 也已经实现了默认场景创建逻辑（`createDefaultScene`）和场景加载/卸载逻辑（`PlayModeController::reloadScene` 已在复用）。

### 1.2 外部修改资产文件后内存中的 `Resource` 不自动热重载

- `doc/todo/MetaFS-Monitor-Fixes-todo.md` 已修好了 `MetaFSMonitor` 在文件新增/修改/重命名时的 `UUID ↔ Path` 映射表同步；
- 但正如该文档 §6 风险第 3 条所记：「只更新映射表，不会让已加载到内存的 `Texture` / `Material` 重新加载」。当用户在外部修改了贴图、材质或重跑 `imgconv` / `scc` 覆盖了产物后，编辑器内存里已处于 `kLoaded` 状态的 `Resource` 不会自动刷新。

---

## 2. 分步实现方案（建议实现）

### 2.1 第一步：在 `ProjectManager` 补齐场景生命周期 API（1.0 d）

在 `source/Editor/TinyEditor/ProjectManager.h/.cpp` 中新增/公开以下方法：

1. **`TResult newScene()`**：
   - 在帧末安全点（`T3D_AGENT.postFrameEndTask`）执行：
     - 清空编辑器选中态与 Hierarchy 树引用（复用 `PlayModeController::clearEditorReferences` 与 `flushPendingDestroys` 流程）；
     - 卸载当前运行时场景；
     - 创建默认新场景（含根节点、`Main Camera`、`Directional Light`，复用 `createProject` 时的默认场景模板逻辑），设为当前场景并广播 `kEvtOpenScene`。
2. **`TResult openScene(const String &relativeScenePath)`**：
   - 同样投递到帧末安全点：卸载旧场景 → `T3D_ASSET_MGR.loadScene(relativeScenePath)` → 设置为当前场景 → 广播 `kEvtOpenScene`。
3. **`TResult saveSceneAs(const String &newRelativeScenePath)`**：
   - 将当前场景改名并保存到工程 `Assets/` 下的目标路径（`.tscene`），生成对应 `.meta`，清除场景脏标记（`kEvtModifyScene(false)`）并刷新 Project 视图。

### 2.2 第二步：接通 `UIMainWindow` 的 `File` 菜单与未保存提示弹窗（0.8 d）

1. **脏场景守卫弹窗**：
   - 当 `PROJECT_MGR.isProjectModified()` 为 `true` 时，执行 `New Scene` 或 `Open Scene` 前弹出 `ImMessageBox` 询问用户是否先保存当前场景修改。
2. **菜单项接线（`UIMainWindow.cpp`）**：
   - 启用条件：`PROJECT_MGR.isProjectOpened() && !PLAY_MODE_CTRL.isPlaying() && !PLAY_MODE_CTRL.isBuildInFlight()`；
   - `ID_MENU_ITEM_NEW_SCENE (Ctrl+N)`：调用 `PROJECT_MGR.newScene()`；
   - `ID_MENU_ITEM_OPEN_SCENE (Ctrl+O)`：弹出 `ImOpenFileDialog`（限定在工程 `Assets/` 目录下过滤 `.tscene`），选中后调用 `PROJECT_MGR.openScene(...)`；
   - `ID_MENU_ITEM_SAVE_AS (Ctrl+Shift+S)`：弹出保存路径对话框，输入文件名后调用 `PROJECT_MGR.saveSceneAs(...)`；
   - `ID_MENU_ITEM_SCENE`（`Assets -> Create -> Scene`）：在当前选中文件夹下创建默认 `.tscene` 资产文件并刷新 Project 面板。

### 2.3 第三步：基于 파일事件驱动已加载 `Resource` 热重载（0.7 d）

1. 当 `MetaFSMonitor` 处理 `kModifed` 事件并完成 `refresh(path)` 拿到该资源的 `UUID` 与 `Meta::Type` 后，发出一个引擎/编辑器事件（或在 `ProjectManager` 轮询到的变更队列中处理）；
2. 在**帧末安全点（`postFrameEndTask`）**按 `Meta::Type` 查询对应 `ResourceManager`（如 `T3D_TEXTURE_MGR`、`T3D_MATERIAL_MGR`、`T3D_MESH_MGR`、`T3D_SHADER_MGR`）中该 `UUID` 是否已处于 `kLoaded` 状态：
   - 若已加载：调用 `resource->reload()`（内部先 `unload()` 释放旧 GPU 资源，再 `load()` 从 `T3D_ASSET_MGR.getArchive()` 重新读取最新内容）。
   - 注意必须在 `postFrameEndTask`（RHI 线程空闲且命令队列排空后）执行 `reload()`，绝不能在帧中间直接销毁旧纹理/缓冲，否则会触发跨线程 Use-After-Free。

---

## 3. 验收标准

1. 在 `TinyEditor` 中点击 `File -> New Scene`、`File -> Open Scene`、`File -> Save As...`，场景可正常新建、切换与另存，Hierarchy、Scene、Game、Inspector 四面板同步更新且无悬垂指针崩溃；
2. 场景有未保存修改时切换场景会弹出保存确认提示；
3. 编辑器运行期间在外部更新一张已绑定在场景物体上的 `.ttex` 或 `.tmat`，Scene 视口画面自动刷新为新贴图/材质。
