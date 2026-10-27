# T4-6 实现计划：Edit 菜单常用操作接线与全局 Undo / Redo 命令栈

> **所属批次**：T4（编辑器 `TinyEditor` 面板与菜单断点接通）  
> **预估工作量**：4.0 人日  
> **涉及文件**：
> - `source/Editor/TinyEditor/UIMainWindow.cpp`
> - `source/Editor/TinyEditor/UIHierarchyWindow.cpp`
> - `source/Editor/TinyEditor/UIInspectorWindow.cpp`
> - `source/Editor/TinyEditor/GUIExtension/ImPropertyDrawer.cpp`
> - `source/Editor/TinyEditor/UndoManager.h` / `UndoManager.cpp`（建议新增）

---

## 1. 现状分析（代码依据）

在 `source/Editor/TinyEditor/UIMainWindow.cpp:360-405` 中，`Edit` 菜单下的核心编辑命令全部已定义好菜单 ID、多语言文本和快捷键提示，但**全部被绑在 `queryDisableDefault` 上，回调全为 `nullptr`**：

| 菜单项 | ID | 快捷键 | 当前状态 |
|--------|----|--------|----------|
| Undo | `ID_MENU_ITEM_UNDO` | `Ctrl+Z` | 置灰，无实现 |
| Redo | `ID_MENU_ITEM_REDO` | `Ctrl+Y` | 置灰，无实现 |
| Cut / Copy / Paste | `ID_MENU_ITEM_CUT / COPY / PASTE` | `Ctrl+X/C/V` | 置灰，无实现 |
| Duplicate | `ID_MENU_ITEM_DUPLICATE` | `Ctrl+D` | 置灰，无实现 |
| Rename | `ID_MENU_ITEM_RENAME` | — | 置灰，无实现 |
| Frame Selected | `ID_MEHU_ITEM_FRAME_SELECTED` | `F` | 置灰，无实现 |
| Toggle Active State | `ID_MENU_ITEM_TOGGLE_ACTIVE_STATE` | `Alt+Shift+A` | 置灰，无实现 |

同时，引擎底层其实已经具备支撑这些操作的全部核心能力：
- `GameObject::clone()`（`T3DGameObject.h:115`）已支持整棵子树深拷贝；
- `RttrArchive` + `JsonSerializer` 已支持将任意 `GameObject` 或 `Component` 序列化为内存快照；
- `ImPropertyDrawer` 对属性的所有读写都经过统一的 RTTR `property::get_value` / `set_value`。

---

## 2. 分步实现方案（建议实现）

### 2.1 第一步：快速接通不依赖 Undo 的 4 个高频菜单项（1.0 d）

1. **`Duplicate (Ctrl+D)`**：
   - 启用条件：`mInspectorWnd->getSelectedGameObject() != nullptr` 且非场景根节点；
   - 实现：调用 `GameObjectPtr copy = selected->clone();`，将 `copy->getTransformNode()` 挂到 `selected->getTransformNode()->getParent()` 下，通知 `UIHierarchyWindow` 刷新并选中新克隆节点，发送 `kEvtModifyScene`。
2. **`Copy (Ctrl+C)` / `Paste (Ctrl+V)`**：
   - `Copy`：记录当前选中 `GameObject` 的 UUID（或将其序列化为内存 JSON 字符串存入编辑器剪贴板）；
   - `Paste`：反序列化或 `clone` 出新子树，挂到当前选中节点同级或子级。
3. **`Toggle Active State (Alt+Shift+A)`**：
   - 调用 `selected->setActive(!selected->isActive());`，发送 `kEvtModifyScene`。
4. **`Frame Selected (F)`**：
   - 发送聚焦事件（与 T4-5 的 Scene 相机聚焦联动），将编辑器相机对准选中物体的世界坐标/包围盒。

### 2.2 第二步：设计轻量级 `UndoManager`（命令模式 + RTTR 属性快照）（1.5 d）

新建 `source/Editor/TinyEditor/UndoManager.h/.cpp`（单例 `UNDO_MGR`）：

```cpp
// 建议实现
class IEditorCommand : public Allocator
{
public:
    virtual ~IEditorCommand() = default;
    virtual const char *getName() const = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;
    /// 连续拖拽滑条时合并同类命令
    virtual bool mergeWith(const IEditorCommand *other) { return false; }
};
```

- `UndoManager` 维护 `mUndoStack` 与 `mRedoStack`（默认容量上限如 100 步）；
- **关键约束**：命令对象中**绝不直接持有 `GameObject*` 或 `Component*` 裸指针/智能指针**，而是持有 **`UUID`**！原因：
  1. 若某个对象在后续步骤中被删除再通过 Undo 恢复，或者经历了 Play Mode / Assembly Reload 重建，旧指针已失效，而按 `UUID` 在当前 `Scene` 中查找永远安全；
  2. 若持有 `GameObjectPtr` 强引用，被删除的对象会因为躺在 Undo 栈里而无法真正释放或干扰引用计数。

### 2.3 第三步：四类核心操作的 Command 封装（1.5 d）

1. **`PropertyChangeCommand`（Inspector 属性修改 & Scene Gizmo 变换）**：
   - 记录：`targetUUID`（GameObject 或 Component 的 UUID）、`propertyName`（RTTR 属性名）、`oldValue`（`RTTRVariant`）、`newValue`（`RTTRVariant`）；
   - 在 `ImPropertyDrawer.cpp` 中：
     - 当控件 `ImGui::IsItemActivated()` 时快照 `oldValue`；
     - 当控件值变化或 `ImGui::IsItemDeactivatedAfterEdit()` 时，推入一条 `PropertyChangeCommand`；
   - `undo()` / `redo()`：按 `targetUUID` 找到对象，通过 RTTR `prop.set_value(instance, oldValue / newValue)` 写回，并触发必要的资源/变换刷新。
2. **`CreateGameObjectCommand` / `DeleteGameObjectCommand`**：
   - 将被创建或被删除的 `GameObject` 子树通过 `JsonSerializer` 序列化为内存字节流（连同其父节点 UUID）；
   - `DeleteGameObjectCommand::undo()`：从内存字节流反序列化恢复整棵子树（保留原 UUID！），重新挂回原父节点并刷新 Hierarchy；
   - `DeleteGameObjectCommand::redo()`：按 UUID 找到该节点并调用 `GameObject::destroy`。
3. **`AddComponentCommand` / `RemoveComponentCommand`**：
   - 同样记录所属 `GameObject` 的 UUID、组件类名及组件属性的序列化快照，支持撤销添加与撤销删除组件。
4. **生命周期清理**：
   - 切换工程、切换场景或触发 C++ 插件重编译导致类型结构变化时，调用 `UNDO_MGR.clear()`。

---

## 3. 验收标准

1. 在 Inspector 中拖动 `Transform3D` 的 Position/Rotation 或修改材质/灯光颜色后，按 `Ctrl+Z`（或点击 `Edit -> Undo`）数值与画面立即恢复原状，按 `Ctrl+Y`（`Edit -> Redo`）重做生效；
2. 新建或删除一个 Cube 后按 `Ctrl+Z`，Cube 在 Hierarchy 与 Scene 中正确消失或复原；
3. 选中物体按 `Ctrl+D` 可直接复制出新物体。
