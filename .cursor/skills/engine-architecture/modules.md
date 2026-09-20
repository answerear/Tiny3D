# Tiny3D 模块入口索引

本表是**出发地图**，不是保证最新的架构说明书。与 `CMakeLists.txt` 或源码冲突时，以构建脚本和实现为准。

总览文档：`doc/Tiny3D-Architecture.md`（分层图多，细节须复核）。

## 构建分层（`source/CMakeLists.txt`）

自底向上，常见依赖方向：

| 目录 | 库 | 先看这些 |
|------|-----|----------|
| `Platform/` | T3DPlatform | `Include/Adapter/` 接口，Wrapper + 各 OS Adapter；窗口 / IO / 线程 / 内存 |
| `Log/` | T3DLog | 分级日志 |
| `Utils/` | T3DUtils | 哈希、字符串、Variant、容器工具 |
| `Reflect/` | T3DReflect | `TCLASS` / `TPROPERTY` / `TFUNCTION` 宏 |
| `System/` | T3DSystem | `Object`、`SmartPtr`、UUID、`ObjectTracer` |
| `Math/` | T3DMath | 全模板数学；左手系，矩阵 × 列向量 |
| `Framework/` | T3DFramework | `sendEvent` / `postEvent`，广播/多播/单播 |
| `Core/Runtime/` | T3DCore | 运行时核心 |
| `Core/Editor/` | T3DCoreEditor | 桌面编辑器核心变体 |
| `Plugins/` | 各插件 | Renderer / Archive / ImageCodec |
| `Editor/` | TinyEditor 等 | 编辑器 UI |
| `Player/` | 播放器 | 运行时宿主 |
| `Tools/` | 工具链 | 转换、打包、反射预处理、着色器交叉编译 |
| `Samples/` | 示例 | 验证某子系统时对照这里 |

## Core 子系统（`source/Core/Include/`）

头文件 ↔ 实现：`Include/<Sub>/T3DXxx.h` → `Source/<Sub>/T3DXxx.cpp`。

| 子目录 | 典型入口 | 职责线索 |
|--------|----------|----------|
| `Kernel/` | `T3DAgent.h` | 引擎单例、管理器持有、帧循环 |
| `Component/` | `T3DComponent.h`、`T3DGameObject.h`（GameObject 在 Kernel） | GameObject + Component；Transform / Camera / Light / Geometry / Behaviour / Prefab |
| `Resource/` | `T3DResource.h`、`T3DAssetManager.h` | 资源与 Manager；UUID + 文件名双索引 |
| `Render/` | `T3DForwardRenderPipeline.h`、各类 Buffer | 渲染管线、窗口、缓冲、状态的引擎侧对象 |
| `RHI/` | `T3DRHIContext.h`、`T3DRHIRenderer.h` | 图形 API 抽象；后端插件实现它 |
| `Material/` | Shader / Variant / ResourceParam | 类 ShaderLab 的材质与变体 |
| `Serializer/` | `T3DBinSerializer`、`T3DJsonSerializer` | RTTR 驱动的二进制 / JSON |
| `Animation/` | Clip / Player / Manager | 骨骼动画 |
| `Input/` | `T3DInput.h` | 输入 |
| `Meta/` | `T3DMeta.h` | 编辑器 meta（桌面） |

`Agent` 持有哪些 Manager，以 `T3DAgent.h` 为准，不要背文档列表。

## 插件（`source/Plugins/`）

`Plugins/CMakeLists.txt` 当前三类：`Renderer`、`Archive`、`ImageCodec`。

渲染后端常见变体（以目录为准，不要假设每个后端都有全套）：

- `Window` / `Runtime` — 游戏或窗口宿主
- `Console` — 编辑器控制台预览
- `Base` — 映射与共享逻辑

分析某个后端时：先读 Core `RHI/` 接口，再读该插件的 Context / Renderer / Buffer / Shader，最后对 `doc/todo/*-Renderer-Backend-todo.md` 或 `doc/*-Architecture.md`。

Archive：文件系统、Bundle、Meta 等，走 `Archive` 插件接口，不要直接假设磁盘路径。

## 编辑器（`source/Editor/`）

| 目录 | 职责线索 |
|------|----------|
| `TinyEditor/` | 场景编辑器主程序、各 UI 窗口 |
| `TinyLauncher/` | 项目启动器 |
| `TinyImGui/` | 引擎侧 ImGui 控件封装 |
| `ImGuiImpl/` | 平台 / 渲染后端的 ImGui 绑定 |

编辑器事件：先搜 `EditorEventDefine` 与 `ON_MEMBER`。属性编辑优先查 RTTR 遍历（序列化里已有范式）。

## 工具（`source/Tools/`）

名称以 `Tools/CMakeLists.txt` 为准。常见：`MeshConverter`、`BundleBuilder`、`BuiltinGenerator`、`ShaderCrossCompiler`、`ReflectionPreprocessor`。

## 读代码时的快捷入口

1. **引擎怎么转起来**：`T3DAgent` 的初始化与每帧函数。
2. **对象模型**：`GameObject` + `Component` + RTTR 宏。
3. **一帧怎么画**：Agent → Scene / Camera → RenderPipeline → RHI Context → 插件 Renderer。
4. **资源怎么来**：`AssetManager` / 各 `*Manager` → Archive 插件 → DataStream。
5. **跨平台怎么藏**：Platform `I*` 接口 + `IFactory::createPlatform*()`，上层只走 Wrapper。

## 设计文档习惯位置

| 位置 | 用途 |
|------|------|
| `doc/todo/<Topic>-Design-todo.md` | 施工蓝图；文末「总经理评审」由 `general-manager` 写入 |
| `doc/todo/<Topic>-Status-todo.md` | 现状分析落盘（仅当需要留下摸底结论时） |
| `doc/todo/<Topic>-todo.md` | 实现核对清单 |
| `doc/<Backend>-Architecture.md` | 某后端的架构叙述（仍须对照代码） |
| `doc/refs/` | API 对照，不是引擎分层说明 |
| `.cursor/rules/*.mdc` | 已采纳的局部设计，改相关代码时应遵守 |
