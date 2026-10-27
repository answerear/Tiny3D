# T4-2 实现计划：TinyEditor Console 日志面板（`UIConsoleWindow`）

> **所属批次**：T4（编辑器 `TinyEditor` 面板与菜单断点接通）  
> **预估工作量**：1.5 人日  
> **涉及文件**：
> - `source/Log/Include/T3DLogger.h` / `Source/T3DLogger.cpp`（或编辑器侧日志监听桥接）
> - `source/Editor/TinyEditor/UIConsoleWindow.h`
> - `source/Editor/TinyEditor/UIConsoleWindow.cpp`

---

## 1. 现状分析（代码依据）

1. **窗口壳子已建好，但 `onGUI()` 为空**：
   - `source/Editor/TinyEditor/UIMainWindow.cpp:128-134` 已创建 `mConsoleWnd = new UIConsoleWindow();` 并注册到 Docking 布局与 `Window -> Panels -> Console` 菜单；
   - 然而 `source/Editor/TinyEditor/UIConsoleWindow.h` 与 `UIConsoleWindow.cpp:34-37` 中：
     ```cpp
     void UIConsoleWindow::onGUI()
     {
     }
     ```
     没有任何成员和绘制逻辑，打开 Console 面板只是一片空白。
2. **日志系统现状（`source/Log/`）**：
   - `Logger`（`T3D_LOG_INFO` / `T3D_LOG_WARNING` / `T3D_LOG_ERROR` 等宏）将日志格式化后写入文件并输出到平台控制台（`T3D_CONSOLE.print`），且内置了异步工作线程 `QueuedJobPool`；
   - 编辑器自身也定义了 `EDITOR_LOG_INFO` / `EDITOR_LOG_WARNING` / `EDITOR_LOG_ERROR`（`source/Editor/TinyEditor/EditorPrerequisites.h`），底层同样走 `T3D_LOG_*`。

---

## 2. 实现方案（建议实现）

### 2.1 在 `Logger` 中增加轻量级日志回调Sink（Listener）

为了不让 `T3DLog` 依赖编辑器，在 `source/Log/Include/T3DLogger.h` 中增加一个可选的日志监听接口或回调注册：

```cpp
// 建议实现（source/Log/Include/T3DLogger.h）
struct LogEntryView
{
    Level       level;
    const char *tag;
    const char *message;
    const char *filename;
    int32_t     line;
    int64_t     timestampMS;
};

using LogSinkCallback = void (*)(const LogEntryView &entry, void *userData);
```

- 在 `Logger` 中提供 `addSink(LogSinkCallback cb, void *userData)` / `removeSink(LogSinkCallback cb, void *userData)`；
- 注意线程安全：`Logger` 可能在后台线程（如 `CppBuildSystem` 编译线程或 `Logger` 自身异步队列）写日志，因此 Sink 回调内部（在 `UIConsoleWindow` 侧）用互斥锁（`Mutex` / `std::mutex`）保护环形缓冲区写入。

### 2.2 `UIConsoleWindow` 数据结构与功能设计（对齐 Unity Console）

在 `UIConsoleWindow.h` 中维护：

```cpp
struct ConsoleLogItem
{
    Logger::Level level;
    String        tag;
    String        message;
    String        file;
    int32_t       line {0};
    String        timeStr;
    uint32_t      repeatCount {1};   ///< Collapse 模式下相邻或相同日志的合并计数
};
```

核心功能：
1. **顶部工具栏（Toolbar）**：
   - `Clear` 按钮：清空当前缓存的日志列表；
   - `Collapse` 开关按钮：合并相同 `(level, tag, message, file, line)` 的日志，在行尾显示计数徽标；
   - `Clear on Play` 开关按钮：监听或响应进入 Play 模式时自动清空旧日志；
   - 搜索过滤框（`ImGuiTextFilter`）：按关键字实时过滤 `message` 或 `tag`；
   - 右侧三个带图标与实时计数的开关按钮：`Info (N)`、`Warning (M)`、`Error (K)`，点击可按等级显隐。
2. **中部滚动日志列表（Log List）**：
   - 使用 `ImGuiListClipper` 虚拟化列表绘制（即使累积上万条日志也只绘制可见的 20～30 行，零卡顿）；
   - 按等级着色：`Error / Fatal` 用红色、`Warning` 用黄色、`Info / Debug` 用默认主文字色；
   - 底部自动跟随滚动（当滚动条原本在最底部时，新日志进来自动 `ImGui::SetScrollHereY(1.0f)`；用户手动往上滚看历史日志时不强行拉回）。
3. **底部分栏详情区（Detail Pane）**：
   - 选中某一行日志后，在底部固定高度子区域展示完整的多行日志正文、Tag、时间戳与 `filename:line`，并提供 `Copy` 按钮一键复制到剪贴板。

---

## 3. 验收标准

1. 启动 `TinyEditor`，打开工程或执行 `Compile C++`、创建/删除物体时，`Console` 面板实时显示引擎与编辑器的 Info / Warning / Error 日志；
2. 切换顶部 Info / Warning / Error 开关、输入搜索词、勾选 Collapse、点击 Clear 均正常工作；
3. 列表超过 2000 条时滚动依然保持满帧。
