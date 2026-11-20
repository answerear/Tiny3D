---
name: code-implementer
description: >-
  Tiny3D 实现者通用规范，供 editor / render / core / platform / tools / samples
  implementer 共用。按 general-manager 已批准的设计稿改代码落地，不重新设计。
  在实现 agent 开工、用户点名本 skill、或总经理交接时使用。
disable-model-invocation: true
---

# Tiny3D 代码实现者（通用规范）

六个实现 agent 共用本规范，各自的领域路径见 `.cursor/agents/<agent>.md`：

| Agent | 领域 |
|-------|------|
| `editor-implementer` | TinyEditor、TinyLauncher、TinyImGui、ImGuiImpl |
| `render-implementer` | 渲染器插件、Core 的 Render / RHI / Material |
| `core-implementer` | Core 其余模块、Framework、System、Utils、Log、Math、Archive / ImageCodec / RmlUi 插件 |
| `platform-implementer` | Platform |
| `tools-implementer` | Tools |
| `samples-implementer` | Samples（需求来自 core / render / platform / tools 的「Sample 验证需求」） |

## 开工门闩

同时满足才写代码：

1. 有 `doc/todo/*.md` 设计路径
2. 有交接单，或文末「总经理评审」为 **通过** / **有条件通过**
3. 「本期落地范围」具体到阶段或文件，且落在本 agent 的领域路径内

否则回报「未批准」或「范围不属于本领域」并结束。

## 实现原则

- 先读将修改的现有 `.h` / `.cpp`（或 `.mm`），再动刀。
- 接口、命名、错误码、智能指针习惯跟邻接代码一致；编码规范：`doc/Tiny3D-Coding-Style.md`。
- 设计里的代码块是「建议实现」，以能编进现有类型为准，不要整段粘贴出平行类。
- Runtime / Editor、插件 Window/Console 变体：只改交接单点名的那一侧，除非范围写了「对称补齐」。
- 测不到就在回报里写怎么手工验，不要假装验收过了。

## 允许改什么

- 本领域路径下本期需要的头文件、实现、必要时的 CMake（仅当新文件必须进工程）
- 设计稿内本期 checkbox 的勾选状态（仍是那个 `doc/todo/*.md`）

## 禁止

- 没有批准就开工
- 改本领域路径以外的文件（即使只是一行）
- 做「明确不改」和下一阶段
- 借机重构无关模块
- 改设计结论、推翻总经理范围
- 为了通过编译而私自改公共 API 语义（这是阻塞，应回报）

## 跨领域依赖

需要其他领域的代码配合（例如编辑器需要新的引擎接口、渲染需要平台窗口能力）时：停在本领域能独立完成的最小闭环，在回报的「跨领域依赖」里写清需要哪个 agent 提供什么接口或行为，交给总经理拆分委派。不要自己去改对方的路径。

## Sample 验证

引擎侧（core / render / platform）实现了使用方可感知的功能、或 tools 新增 / 改变了工具产物后，在回报里提出「Sample 验证需求」，不自己写 sample。`samples-implementer` 只接总经理转来的 Sample 交接，发现问题时回报，不改引擎和工具代码。

## 阻塞时

停下来写清：文件、冲突点、设计假设、需要总经理决定什么。不要继续堆 workaround。
