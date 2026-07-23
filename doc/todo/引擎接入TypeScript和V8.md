# 引擎接入 TypeScript 与 V8 设计（脚本系统路线 B）

> 目标：在 Tiny3D 现有 **GameObject-Component + `Behaviour` 生命周期框架**之上，接入 **TypeScript 脚本 + V8 运行时**，让开发者用 TS 编写 gameplay、挂到 GameObject 上，并支持热重载。本文是《Behaviour 脚本组件系统设计》(`Behaviour-Script-Component-Design-todo.md`) 中「路线 B」的独立展开，对应其里程碑 **M4**。

> **前置条件**：路线 A（`Behaviour` 基类的生命周期 `onAwake/onEnable/onStart/onUpdate/onLateUpdate/onFixedUpdate/onDisable/onDestroy`、组件级 `enabled`、全局 `Time`、调度）已完成。本方案只是把这套 C++ 框架的回调"转发"到 V8 里的 TS 对象，不重新设计生命周期。

---

## 1. 选型定论

> **脚本语言用 TypeScript，运行时用 V8。**

引擎定位为**学习/实验**，明确**不考虑 iOS（因此 JIT 限制无关）与构建复杂度**，优先采用当前最主流、最强的技术栈：

- **TypeScript** 提供编译期类型安全（补 Lua 无类型之短），生态与开发体验好，且脚本天然可复用到 web/微信小游戏。
- **V8** 提供业界最强 JIT 性能与最成熟的调试生态（DevTools / Inspector 协议）。

**语言 vs 运行时**：TS 编译后是 JS，类型仅存在于编译期；运行时执行 V8 上的 JS。gameplay 脚本用 TS 编写，经 `tsc`（或 esbuild/swc）编译为 JS 后加载。

**为什么不是 Lua / C# / QuickJS**：
- Lua 无编译期类型，易出运行时错误；
- C# (mono/.NET) 过重、发布链复杂；
- QuickJS 等轻量运行时更适合移动/嵌入场景，但本引擎不追求那类兼容性与体积约束，故直接选性能与生态天花板的 V8。

**微信小游戏定位（附带收益，非当前目标）**：选 TS 让 gameplay 脚本天然可复用到 web/微信小游戏；但引擎本体是 C++，真正移植需 WebAssembly/WebGL 版运行时，属另立项工程，脚本语言统一只是"必要不充分"的一环。

---

## 2. 整体架构

### 2.1 桥接结构

- `ScriptBehaviour : Behaviour` 持有一个 **V8 中的 JS class 实例句柄**（`v8::Global<v8::Object>`），把 `Behaviour` 的生命周期回调转发到 JS 对象的同名方法。
- 序列化存「脚本资源路径 + 导出字段」，字段值通过 RTTR / Variant ↔ V8 Value 互通。

```
Behaviour (C++, 路线 A)
   └── ScriptBehaviour (C++)  ── 持有 ──▶  JS 实例 (V8, 用户的 TS class)
          onUpdate() ──────── callJS("onUpdate") ────────▶ onUpdate()
```

### 2.2 RTTR 自动绑定 + 类型声明（核心杠杆）

- 引擎已有 RTTR，可据其元信息**自动生成 V8 绑定层**，把 `GameObject / Component / Transform3D / Time` 等暴露给 JS。
- 同一套元信息**自动生成 `.d.ts` 类型声明**，使 TS 侧获得完整类型提示与编译检查——这是 TS+RTTR 组合独有的红利（Lua 绑定无此类型产物）。

### 2.3 运行时抽象（可选但推荐）

把 V8 相关调用（compile / eval / callMethod / createInstance / gc-hook）收敛到一个薄接口层，隔离 V8 版本升级带来的 API 变动，也为将来实验其它引擎留口子（非兼容性需求，纯为可维护）。所有 `v8::` 只出现在 `Script/` 目录内。

---

## 3. 子里程碑总览

| 子里程碑 | 内容 | 产出 |
|---|---|---|
| **M4.1** | V8 集成 + `ScriptEngine` 单例（Isolate/Context 生命周期，随 Agent） | 能在引擎内 eval 一段 JS |
| **M4.2** | RTTR→V8 绑定生成器：把 `GameObject/Component/Transform3D/Time` 暴露给 JS | JS 里能调引擎 API |
| **M4.3** | `.d.ts` 生成器：从 RTTR 导出类型声明 | TS 侧有类型提示/编译检查 |
| **M4.4** | `ScriptBehaviour` 桥接：生命周期回调转发到 TS 对象 | TS 写 gameplay 可挂 GameObject |
| **M4.5** | C++↔JS 生命周期/GC 弱引用回收 | 无泄漏/悬挂 |
| **M4.6** | TS 工具链 + 脚本资源（`.ts`→`.js`，`ScriptAsset`） | 编辑器/运行时加载脚本 |
| **M4.7** | 热重载 | 改 TS 不重启即时生效 |

---

## 4. M4.1 — V8 集成与 `ScriptEngine` 单例

沿用引擎单例约定（`Singleton<T>` + `T3D_SCRIPT` 宏），由 `Agent` 持有；封装 V8 的 `Isolate` / `Context` / `HandleScope` 样板，对外只露高层接口。

```cpp
// source/Core/Include/Script/T3DScriptEngine.h（新增）
class T3D_ENGINE_API ScriptEngine : public Singleton<ScriptEngine>
{
    friend class Agent;
public:
    // 高层 API（隐藏 v8:: 细节）
    ScriptValue eval(const String &code, const String &fileName);
    ScriptObject createInstance(const String &className);        // new 一个 TS class
    ScriptValue callMethod(const ScriptObject &obj, const String &method,
                           const ScriptArgs &args);
    void        collectGarbage();                                // 主动触发 GC（调试用）
    v8::Isolate *isolate() const { return mIsolate; }
private:
    void start();      // Agent::init：创建 Isolate + Context + 平台
    void tick();       // 每帧 pump message loop / microtasks（如需）
    void shutdown();   // Agent 退出：Dispose

    v8::Isolate *mIsolate {nullptr};
    v8::Global<v8::Context> mContext;
    std::unique_ptr<v8::Platform> mPlatform;
    // ArrayBuffer allocator、绑定注册表 等
};
#define T3D_SCRIPT  ScriptEngine::getInstance()
```

- **依赖引入**：V8 预编译库或源码构建（`v8`、`libplatform`）；`Agent::init` 里 `V8::InitializePlatform + V8::Initialize`，退出时反向 Dispose。
- **接入点**：`Agent::init` 创建 `ScriptEngine`；若需要 microtask/promise，在 `Agent::update`（事件派发后、Scene 更新前）`tick()` 一次。
- **抽象收敛**：所有 `v8::` 只出现在 `Script/` 目录内，外部仅见 `ScriptValue/ScriptObject/ScriptArgs` 等薄包装，隔离 V8 版本升级。

---

## 5. M4.2 — RTTR → V8 绑定生成器

复用引擎已有 RTTR 元信息，**自动**把 C++ 类型暴露给 JS，避免手写绑定。

- **生成方式（二选一）**：
  1. **运行时反射绑定**：启动时遍历 `RTTRType`，为每个注册类型创建 `v8::FunctionTemplate`（构造器）+ 为每个 `TPROPERTY` 装 accessor、每个 `TFUNCTION` 装 callback。实现一次、覆盖所有类型，最省事，推荐首选。
  2. **离线代码生成**：扩展现有 `ReflectionPreprocessor`，为每个类生成 `.v8.cpp` 绑定。性能略好但维护成本高。
- **值编组（marshalling）**：建一张 `RTTRVariant ↔ v8::Value` 双向转换表——基础类型（int/float/bool/String）、数学类型（`Vector3/Quaternion` 映射为 JS 对象或 TypedArray）、`UUID`（字符串）、容器（Array/Object）。
- **对象包装**：C++ 对象在 JS 侧表现为一个持有 `内部指针 + RTTRType` 的 wrapper（`v8::Object` 带 internal field 存 `Component*`）；方法调用时取回指针 + 通过 RTTR `invoke`。
- **API 表面**：至少暴露 `GameObject`（`getComponent/addComponent/find`）、`Component`/`Behaviour`、`Transform3D`、`Time`（只读 getter）、数学库。

---

## 6. M4.3 — `.d.ts` 类型声明生成器

同一份 RTTR 元信息再生成 TypeScript 声明，让 TS 侧获得类型安全（这正是选 TS 而非 Lua 的核心收益）。

```ts
// 自动生成 engine.d.ts（示意）
declare class Transform3D extends Component {
  position: Vector3;
  rotate(axis: Vector3, angleDeg: number): void;
}
declare namespace Time {
  function deltaTime(): number;   // 毫秒
}
declare class Behaviour extends Component {
  enabled: boolean;
  readonly gameObject: GameObject;
  onAwake?(): void; onStart?(): void; onUpdate?(): void; /* ... */
}
```

- 遍历 `RTTRType` → 输出 class/method/property 声明；`TPROPERTY` 映射为字段，`TFUNCTION` 映射为方法，枚举映射为 `enum`。
- 作为构建产物随 SDK 分发，用户 TS 工程 `import` 后即有补全与编译检查。

---

## 7. M4.4 — `ScriptBehaviour` 桥接

TS 侧用户脚本继承（`.d.ts` 里的）`Behaviour`；C++ 侧一个 `ScriptBehaviour` 实体组件持有对应 JS 实例，把生命周期回调转发过去。

```ts
// 用户脚本 Rotator.ts
export class Rotator extends Behaviour {
  speed: number = 90;               // 度/秒，导出字段（可序列化 + Inspector）
  onUpdate() {
    this.gameObject.getComponent(Transform3D)
        .rotate(Vector3.UNIT_Y, this.speed * Time.deltaTime() / 1000);
  }
}
```

```cpp
// source/Core/Include/Component/T3DScriptBehaviour.h（新增）
TCLASS()
class T3D_ENGINE_API ScriptBehaviour : public Behaviour
{
    TRTTI_ENABLE(Behaviour)  TRTTI_FRIEND
public:
    ComponentPtr clone() const override;             // 复制脚本路径+导出字段，重建 JS 实例
protected:
    void onAwake() override    { callJS("onAwake"); }
    void onEnable() override    { callJS("onEnable"); }
    void onStart() override      { callJS("onStart"); }
    void onUpdate() override      { callJS("onUpdate"); }
    void onLateUpdate() override  { callJS("onLateUpdate"); }
    void onFixedUpdate() override { callJS("onFixedUpdate"); }
    void onDisable() override     { callJS("onDisable"); }
    void onDestroy() override     { callJS("onDestroy"); releaseJS(); }
private:
    void callJS(const char *method);   // 方法不存在则空转（避免每帧异常）

    TPROPERTY() String  mScriptClass;  // 脚本类名/资源路径（序列化）
    TPROPERTY() /* 导出字段快照，见 §9 */ ;
    ScriptObject mJsInstance;          // v8::Global<v8::Object>
};
```

- **实例化**：Awake 前由 `ScriptEngine::createInstance(mScriptClass)` new 出 JS 对象，并把「对应的 C++ `GameObject` wrapper」注入为 JS 侧 `this.gameObject`。
- **回调转发**：`callJS` 先查方法是否存在（缓存查询结果），避免每帧对未定义的 `onUpdate` 反复抛异常；异常在边界处捕获并转引擎日志，不让 JS 异常穿透 C++。
- **性能**：仅在用户脚本定义了该回调时才注册进对应更新阶段队列（未定义 `onUpdate` 的脚本不进 update 遍历），减少空转跨边界调用。

---

## 8. M4.5 — C++↔JS 生命周期与 GC 弱引用回收（头号难点）

引擎侧侵入式引用计数，V8 侧独立 GC，两套内存管理必须对齐，避免泄漏/悬挂。

- **C++ 持有 JS**：`ScriptBehaviour::mJsInstance` 用 `v8::Global`（强引用），在 `onDestroy` 时显式 `Reset()` 释放，使 JS 实例可被回收。
- **JS 持有 C++**：JS wrapper 的 internal field 存 `Component*` 并 `acquire()` 一次；对 wrapper 设 `SetWeak` + 回调，在 V8 回收 wrapper 时 `release()` 对应 C++ 引用。
- **防悬挂**：C++ 对象若先于 JS 回收销毁（如 `destroy(GameObject)`），需把其 JS wrapper 的 internal field 置空并标记失效；JS 再访问已失效 wrapper 时抛可捕获错误而非崩溃。
- **wrapper 缓存**：同一 C++ 对象多次传入 JS 应复用同一 wrapper（用 `TUnorderedMap<void*, v8::Global<Object>>`），保证 JS 侧 `===` 语义与引用一致性。

---

## 9. M4.6 — TS 工具链与脚本资源

- **编译**：`.ts → .js` 用离线 `tsc`（最简单）或集成 `esbuild/swc`（快）；开发期可在编辑器保存时自动编译。
- **脚本资源**：新增 `ScriptAsset`（`.ts`/编译后 `.js` + meta），纳入现有资源/Meta 体系（UUID 索引），像其它资源一样被 `Scene`/`Prefab` 引用。
- **导出字段序列化**：`ScriptBehaviour` 把 JS 实例上标注为"导出"的字段（如约定 `@serialize` 装饰器或首字母规则）快照成 RTTR `Variant` 存入 `.tscene`；反序列化后写回 JS 实例。字段同样驱动 Inspector 自动生成控件。
- **模块加载**：约定脚本以 ES module 组织，`ScriptEngine` 提供简单的模块解析/缓存（按资源路径）。

---

## 10. M4.7 — 热重载

TS/JS 热重载是本路线相对 C++ 的核心价值。

- **触发**：`FileSystemMonitor`（平台层已有）监听脚本资源变更 → 重新 `tsc` 编译 → 通知 `ScriptEngine` 重载对应模块。
- **实例迁移**：对所有引用该脚本的 `ScriptBehaviour`：① 快照当前导出字段；② `Reset` 旧 JS 实例；③ 用新类 `createInstance` 重建；④ 回填字段快照；⑤ 视策略补发 `onEnable`（一般**不**重跑 `onAwake/onStart`，保持运行态连续）。
- **失败回退**：编译报错时保留旧实例并在编辑器提示，不中断运行。

---

## 11. 帧循环接入小结

```
Agent::update()
  ├─ EventManager::dispatchEvent()
  ├─ ScriptEngine::tick()          // 可选：microtask/promise 泵
  ├─ Scene::flushPendingStart()    // ScriptBehaviour.onStart → callJS
  ├─ while stepFixed(): Scene::fixedUpdate()   // onFixedUpdate → callJS
  └─ Scene::update()               // onUpdate/onLateUpdate → callJS
Agent::endFrame()
  └─ 延迟销毁 → ScriptBehaviour.onDestroy → callJS + releaseJS
```

> 生命周期时机（Awake 同步 / Start 延迟 / OnEnable-OnDisable 配对 / 帧末 OnDestroy）完全沿用路线 A 的语义，详见《Behaviour 脚本组件系统设计》的生命周期章节与时序图。

---

## 12. 风险与注意点

1. **GC/生命周期**（§8）是最大风险，务必先用小样例验证弱引用回收与防悬挂闭环。
2. **跨边界性能**：大量脚本每帧 `onUpdate` 的 C++→JS 调用开销；对策——未定义回调不入队、热逻辑留 C++、必要时批量派发。
3. **精度**：JS `number` 为 double，安全整数 2^53；`Time` 毫秒无碍，`UUID`/大计数用字符串或 `BigInt`。
4. **异常隔离**：JS 异常必须在 C++ 边界捕获并转日志，严禁穿透。
5. **确定性**：JS 浮点更不可控，帧同步逻辑不放 JS（本引擎不做帧同步，风险可接受）。
6. **V8 体积/构建**：产物较大、构建较重——已按引擎"学习/实验、不计构建复杂度"的定位接受。
