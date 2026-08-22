# 序列化归档统一（JSON + Bin 合二为一）设计与改造计划

> **状态：阶段 0–5 全部完成。** 两种格式共用一份 RTTR 遍历逻辑，读写四个后端均为前向单遍、零物化。
> 每个阶段的实测结论见 §9（含逐字节基线比对、往返自检、零告警检查）。
> 落地过程中偏离原计划的地方都记在对应阶段里，其中阶段 4 需要**改动 `IArchiveReader` 接口**（原计划误判为"接口不变"）。

> 目标：把当前各写一遍的 `JsonSerializer` 与 `BinSerializer` 收敛为**一份共享的 RTTR 遍历逻辑 + 可插拔的格式后端**，消除约 600+ 行重复代码与已有的行为不一致，同时**保留 Bin 的流式读取优势**（不物化整棵中间树）。核心思路对齐业界对象序列化框架（Unreal `FArchive`、cereal、Boost.Serialization）的 **Archive 模式**：一份遍历逻辑，多个前向单遍的读写后端。

---

## 1. 背景与问题

### 1.1 现状

引擎里存在两个序列化器，都实现自 `Serializer` 接口（`source/Core/Include/Serializer/T3DSerializer.h`）：

```cpp
class Serializer : public Object, public Noncopyable {
    virtual TResult serialize(DataStream &stream, const RTTRObject &obj) = 0;
    virtual RTTRObject deserialize(DataStream &stream) = 0;
    virtual TResult deserialize(DataStream &stream, RTTRVariant &obj) = 0;
};
```

- `JsonSerializer`（`T3DJsonSerializer.cpp`）：基于 rapidjson，写用 `PrettyWriter`，读用 `Document` DOM。
- `BinSerializer`（`T3DBinSerializer.cpp`）：自定义 `T3DB` 二进制格式，`kind 标签 + varint`，纯流式读写。
- `SerializerManager` 按 `FileMode` 选择写入器，按文件头 magic（`pickDeserializer`）自动选择读取器，做到运行时透明读取。

### 1.2 问题

两个序列化器**读写的内容（语义）完全相同，只有磁盘格式不同**，却把整套遍历语义各复制了一遍：

| 语义逻辑 | JSON | Bin |
|---|---|---|
| 原子类型分派 | `WriteAtomicType` / `ReadAtomicType` | `WriteAtomic` / `ReadAtomicByKind` |
| 顺序容器 | `WriteSequentialContainer` / seq 读 | `WriteSeq` / `ReadSeqInto` |
| 关联容器 (map/set) | `WriteAssociatvieContainer` / map 读 | `WriteMap` / `ReadMapInto` |
| 对象属性遍历 | `WriteObject` / `ReadValue` 属性循环 | `WriteObject` / `FillObjectMembers` |
| wrapper/SmartPtr 分派 | `WriteVariant` | `WriteValue` |
| SmartPtr 回填 | `convertToSmartPtr` | `ConvertToSmartPtr` |

这些函数承载的规则完全一致：跳过 `NO_SERIALIZE`、空指针/空 `SmartPtr` 不写、多态按派生类型名建对象、`onPreSave/onPostSave/onPostInit/onPostLoad` 生命周期回调、Resource 反序列化时 `onAddComponentForLoadingResource`、`isInvokeLifecycleCallbacks` 开关等。

### 1.3 已存在的行为不一致（统一后应一并修复）

1. **保存时生命周期开关不一致**：`BinSerializer::serialize` 对顶层对象的 `onPreSave/onPostSave` 受 `isInvokeLifecycleCallbacks` 控制，而 `JsonSerializer::serialize` **无条件调用**、未检查该开关。
2. **改名兼容不一致**：Bin 读取有 `SERIALIZE_ALIAS` 曾用名回退（`FindProperty`），JSON 读取**没有**，`get_property` 找不到即报错。
3. **未知属性处理**：Bin 靠自描述 `SkipValue` 前向跳过；JSON 靠 DOM 天然跳过。语义应统一表达。
4. **枚举表示**：JSON 写字符串名（失败退化为 uint64），Bin 恒写字符串名。

---

## 2. 方案选型

### 2.1 为什么不用"全量中间树（DOM）"

把两种格式都先解析成一棵通用 IR 树，再由一份遍历做 `RTTR ↔ IR`，逻辑确实统一了，但**会让 Bin 失去纯流式优势**：Mesh/Texture 的 `Buffer` 可能几十 MB，最不该在一个中间树里再复制一份。

### 2.2 选定方案：前向单遍归档（Archive / pull 模式）

把中间表示从"可随机访问的 **tree**"降级为"只能顺序前进的 **事件流**"。共享遍历不变，但读取端按需从流里拉取（pull），**不物化整棵树**，于是 Bin 保持流式。

这正是业界对象序列化的主流做法：

- **Unreal `FArchive`**：一个重载 `operator<<`，同一份代码既序列化又反序列化，纯顺序流，格式藏在 archive 后端。
- **cereal / Boost.Serialization**：一份 `serialize(Archive&)` 模板 + 多个顺序 archive 后端（binary/JSON/XML）。

> 说明：JSON 编辑器侧本来就需要容错、乱序 key、人手编辑，输入端**允许**继续用 rapidjson DOM——只要它对外实现"按存储顺序前向 pull"的归档接口即可，内存占用与现状一致。底线目标是**让 Bin 保持流式**，而非强制 JSON 也流式。
>
> 事后补记：JSON 最终也做成了真流式（阶段 4）。代价是牺牲了一部分对手改 JSON 的乱序容错——这个取舍当时被低估了，详见阶段 4 的容错权衡表。

---

## 3. 让流式统一成立的两个约束

两种格式当前都已基本满足，只需小改 JSON 读取路径。

### 3.1 磁盘布局 = 遍历顺序，且自定界

- **Bin**：`kind 标签 + count + varint 长度`，本就完全可前向跳过 ✓
- **JSON**：`RTTI_Type` 在前、`RTTI_Value` 在后、对象类型名最前 ✓；
  唯一要改：读取从"**按 key 查找**"改为"**按存储顺序 in-order 迭代**"：
  - `FindMember(RTTI_VALUE)` → 顺序读
  - `FindMember(RTTI_MAP_KEY / RTTI_MAP_VALUE)` → 固定顺序读

  改造后 JSON 的读取顺序与 Bin 的 `FillObjectMembers`（先读属性名，再在类里查 RTTR 属性）完全一致。

### 3.2 只允许 1-token 前瞻（peek），禁止向后 seek

- 多态判空、"容器 vs 原子"分派只需看下一个 token。
- Bin：peek 一个 kind 字节即可；JSON：当前节点类型本就已知。
- 无任意回退 → 流式成立。

---

## 4. 架构设计

### 4.1 分层

```
┌───────────────────────────────────────────────────────────┐
│  SerializerManager（不变：按 FileMode 选写、按 magic 选读）  │
└───────────────────────────────────────────────────────────┘
                     │ 组装 后端 + 共享遍历
                     ▼
┌───────────────────────────────────────────────────────────┐
│  共享遍历层  RttrArchive（唯一一份语义逻辑）                 │
│   - 写：RTTR 对象 → IArchiveWriter 事件                     │
│   - 读：IArchiveReader 事件 → RTTR 对象                     │
│   - 承载：NO_SERIALIZE / 空指针跳过 / 多态 / Buffer·UUID /  │
│           SmartPtr 转换 / alias 回退 / 生命周期回调          │
└───────────────────────────────────────────────────────────┘
        │                                   │
        ▼                                   ▼
┌──────────────────┐              ┌──────────────────┐
│ IArchiveWriter   │              │ IArchiveReader   │
│  ├ JsonWriter    │              │  ├ JsonReader    │  ← SAX 逐 token 拉取，零物化
│  └ BinWriter     │              │  └ BinReader     │  ← 纯流式，零物化
└──────────────────┘              └──────────────────┘
```

### 4.2 归档接口草图

值的自描述类型标签（读写后端共用，语义对齐现有 `BinKind`）：

```cpp
enum class ArchiveKind : uint8_t {
    Null, Bool,
    I8, U8, I16, U16, I32, U32, I64, U64, F32, F64,
    String, Enum, Object, Seq, Map, Buffer, Uuid,
};
```

写入端：

```cpp
class IArchiveWriter {
public:
    virtual ~IArchiveWriter() = default;

    virtual void writeHeader() = 0;

    // 结构
    virtual void beginObject(rttr::string_view typeName) = 0;
    virtual void endObject() = 0;
    virtual void writePropertyName(rttr::string_view name) = 0;
    virtual void beginArray(size_t count) = 0;
    virtual void endArray() = 0;
    virtual void beginMap(bool keyOnly, size_t count) = 0;
    virtual void endMap() = 0;

    // 叶子
    virtual void writeBool(bool v) = 0;
    virtual void writeI64(int64_t v, ArchiveKind kind) = 0;   // kind 用于精确类型
    virtual void writeU64(uint64_t v, ArchiveKind kind) = 0;
    virtual void writeF64(double v, ArchiveKind kind) = 0;    // F32/F64
    virtual void writeString(rttr::string_view v) = 0;
    virtual void writeEnum(rttr::string_view name, uint64_t value) = 0;
    virtual void writeBuffer(const uint8_t *data, size_t size) = 0;
    virtual void writeUuid(const UUID &uuid) = 0;
    virtual void writeNull() = 0;
};
```

读取端（前向 pull，与写入端严格镜像）：

```cpp
class IArchiveReader {
public:
    virtual ~IArchiveReader() = default;

    virtual TResult readHeader() = 0;

    // 值信封：每个值都是 readKind ... endValue 配对
    virtual ArchiveKind readKind() = 0;
    virtual void        endValue() = 0;

    // 前向容错
    virtual void skipValue() = 0;                  // 未知属性：连标签一起跳
    virtual void skipPayload(ArchiveKind kind) = 0; // 标签已读但与声明类型不符

    // 结构
    virtual void     beginObject(String &typeName) = 0;   // 多态需在建对象前拿类型名
    virtual bool     nextProperty(String &name) = 0;      // 顺序吐属性名，无则 false
    virtual void     endObject() = 0;
    virtual uint64_t beginArray() = 0;                    // 元素数，仅预分配提示，未知给 0
    virtual void     endArray() = 0;
    virtual bool     nextElement() = 0;                   // 顺序拉元素，无则 false
    virtual void     beginMap() = 0;
    virtual void     endMap() = 0;
    virtual void     beginMapEntry() = 0;
    virtual void     readMapKeyTag() = 0;
    virtual void     readMapValueTag() = 0;
    virtual void     endMapEntry() = 0;

    // 叶子（schema 驱动：由遍历层用 RTTR hint 决定调哪个）
    virtual bool     readBool() = 0;
    virtual int64_t  readInt(ArchiveKind kind) = 0;   // kind 给出落盘确切宽度
    virtual uint64_t readUint(ArchiveKind kind) = 0;
    virtual double   readReal(ArchiveKind kind) = 0;
    virtual String   readString() = 0;               // 字符串与枚举名共用
    virtual void     readBuffer(Buffer &out) = 0;    // 实现负责释放旧数据并分配
    virtual void     readUuid(UUID &out) = 0;
};
```

落地时相比早期草图的三处调整（阶段 2 实施后回填）：

1. **去掉不消费的 `peekKind()`，改为消费式 `readKind()` + `endValue()` 配对。** 草图里 `peekKind` 在 Bin 后端需要"读 1 字节再 seek 回退"。但遍历层是 schema 驱动的，每个值的标签本来就只在值的开头被读取且恰好读一次，前瞻从未真正必要。去掉它后 Bin 后端**完全不需要 seek**，流式性质更强（只进不退）。`endValue()` 供文本后端弹出 DOM 节点游标，Bin 为空操作。
2. **属性改为 `nextProperty()` 拉取，而非"先读个数再循环"。** 磁盘上 Bin 确实是「个数 + N 个 (名字, 值)」，但把个数收敛成后端内部状态后，阶段 4 的 SAX JSON 后端（无法预知成员数）也能实现同一接口。
3. **叶子读取用 `readInt/readUint/readReal(kind)` 返回最宽类型，由遍历层按 `kind` 收窄回确切类型。** 后端只需按标签解出正确字节数，无需接触 rttr；确切宽度的变体仍在遍历层重建，与写入端对称（例如 U64 极大值不会先被压成 int64 再放大）。

### 4.3 共享遍历层（唯一语义实现）

写入伪代码（对照现有 `WriteVariant` / `WriteObject` / `WriteSeq` / `WriteMap`）：

```cpp
void RttrArchive::writeValue(IArchiveWriter &w, const variant &var) {
    // 1. 计算 effType / eff（解 wrapper）
    // 2. isObject → onPreSave（受 isInvokeLifecycleCallbacks 统一控制）
    // 3. 分派：atomic/enum/string → w.writeXxx
    //         sequential → w.beginArray + 递归
    //         associative → w.beginMap + 递归
    //         Buffer → w.writeBuffer
    //         UUID → w.writeUuid
    //         其它对象 → w.beginObject + writeObjectProps
    // 4. isObject → onPostSave
}
```

读取伪代码（对照现有 `ReadValue` / `FillObjectMembers` / `ReadSeqInto` / `ReadMapInto`）：

```cpp
void RttrArchive::readInto(IArchiveReader &r, const type &hint, variant &obj) {
    // hint 驱动：
    //   arithmetic → r.readI64/U64/F64/readBool，再 convert 到 hint
    //   enumeration → r.readEnumName → name_to_value
    //   string → r.readString
    //   Buffer → r.readBuffer
    //   UUID → r.readUuid
    //   sequential → r.beginArray + 逐元素 readElement
    //   associative → r.beginMap + 逐项 readElement
    //   其它对象（值类型 TSTRUCT）→ r.beginObject(忽略类型名) + fillMembers
}

variant RttrArchive::readNew(IArchiveReader &r, bool topLevel) {
    // 多态入口：r.beginObject(typeName) → type::get_by_name → ctor
    //   onPostInit → fillMembers → 返回
}
```

`fillMembers` 统一承载：`nextProperty` 循环、`FindProperty`（含 `SERIALIZE_ALIAS` 回退）、多态属性走 `readNew`、值类型走 `readInto`、未知属性 `skipValue`、末尾生命周期回调（`onPostLoad` / `onAddComponentForLoadingResource`，受 `isInvokeLifecycleCallbacks` 控制）。

### 4.4 各后端实现要点

| 后端 | 写 | 读 |
|---|---|---|
| **Bin** | 现有 `writeKind/writePod/writeUVarint/writeString` 原样搬进 `BinArchiveWriter` | 现有 `readKind/readPod/readUVarint/readString/SkipValue` 搬进 `BinArchiveReader`，**纯流式、零物化** |
| **JSON** | `PrettyWriter` 封装为 `beginObject/writePropertyName/...`，头部写 `{Magic,Version,Object}` | 先落轻量版：`Document` DOM 但**按 `MemberBegin/MemberEnd` 顺序迭代**实现 pull（阶段 3）；再换彻底版：`rapidjson::Reader` 逐 token 拉取、**零物化**（阶段 4，已完成） |

`peekKind`：Bin 读 1 字节后 seek 回退；JSON 由当前 DOM 节点/token 类型推断。

---

## 5. 磁盘格式（保持不变）

- **Bin**：`T3DB` 头（magic 4B + version u16 + flags u16 + reserved u32）+ 根 Value。格式字节级不变。
- **JSON**：`{ "Magic":"T3D", "Version":"0.0.1", "Object": {...} }`，`RTTI_Type` / `RTTI_Value` / `RTTI_Map_Key` / `RTTI_Map_Value` 键名不变。
- `SerializerManager::pickDeserializer` 按首 4 字节 magic 选后端的逻辑不变（`T3DB` → Bin，`{` → JSON）。

> 统一是**内部重构**，不改变磁盘产物；已烘焙的 bundle 与编辑器 JSON 资产继续可读。

---

## 6. 涉及的关键文件

| 模块 | 文件 |
|---|---|
| 序列化接口 | `source/Core/Include/Serializer/T3DSerializer.h` |
| JSON 序列化器 | `source/Core/Include/Serializer/T3DJsonSerializer.h`、`source/Core/Source/Serializer/T3DJsonSerializer.cpp` |
| Bin 序列化器 | `source/Core/Include/Serializer/T3DBinSerializer.h`、`source/Core/Source/Serializer/T3DBinSerializer.cpp` |
| 管理器 | `source/Core/Include/Serializer/T3DSerializerManager.h`、`source/Core/Source/Serializer/T3DSerializerManager.cpp` |
| 新增（本方案） | `T3DSerializeArchive.h`（`IArchiveReader/Writer` + `ArchiveKind`）、`T3DRttrArchive.h/.cpp`（共享遍历）、`T3DJsonArchive.*`、`T3DBinArchive.*` |

> 基类头文件命名为 `T3DSerializeArchive.h` 而非 `T3DArchive.h`：后者已被 `source/Core/Include/Kernel/T3DArchive.h`（资源存储的 `Archive` 基类）占用，同名头文件即使目录不同也容易在包含路径与 IDE 跳转上混淆。包含卫士同步为 `__T3D_SERIALIZE_ARCHIVE_H__`。

---

## 7. 分阶段改造计划

> 每一步都可独立编译、独立用现有 `bin_verify_*` 往返测试回归。

### 阶段 0：准备 — 已完成
- [x] 梳理并列出两套实现的**全部语义差异点**（§1.3），作为统一后的行为基线。
- [x] 确认 `isInvokeLifecycleCallbacks` 在保存/加载、顶层/嵌套的**目标统一语义**：保存侧顶层与嵌套一律受开关控制。实测该口径对现有资产是字节中性的——只有 `Prefab` 与 `Skeleton` 重写了 `onPreSave`，二者都只作为顶层资源出现（`SkinnedMesh` 仅按 UUID 引用骨架，`mSkeleton` 不是序列化属性），`Object` 基类实现为空。

### 阶段 1：抽象写入端（风险最低，先落地）— 已完成
- [x] 定义 `ArchiveKind` 与 `IArchiveWriter`（`T3DSerializeArchive.h`）。
- [x] 实现 `BinArchiveWriter`（Bin 写原语连同格式常量一并提到 `T3DBinArchive.h` 的 `BinArchiveIO` 命名空间，供后续 Reader 复用）、`JsonArchiveWriter`（`PrettyWriter` 用 pimpl 藏在 `.cpp`，公共头不泄漏 rapidjson）。
- [x] 实现共享 `RttrArchive::write` 及其 `writeValue/writeObjectBody/writeSeq/writeMap/writeElement`（合并两套写遍历）。
- [x] `JsonSerializer::serialize` / `BinSerializer::serialize` 改为委托共享遍历 + 对应后端；两个 `.cpp` 中被取代的写入器代码已删除。
- [x] 对齐保存侧 `onPreSave/onPostSave` 与 `isInvokeLifecycleCallbacks` 行为（顶层与嵌套一致受开关控制）。
- [x] **验证**：见 §9.1 实测结论。

#### 阶段 1 期间发现并修复的缺陷

**1）非空 `SmartPtr<Object>` 属性被静默丢弃（原 Bin 写入器缺陷）**

原 Bin 写入器判空用：

```cpp
variant wv = propValue.extract_wrapped_value();
if (!wv.is_valid() || wv.convert<void *>() == nullptr) continue;   // 转换失败 == 空
```

非空 `SmartPtr<T>` 解包出的裸指针并不总能转成 `void*`，一旦转换失败就被当作空指针跳过。实测 `Pass::RenderState` 整棵子树（`BlendDesc` / `DepthStencilDesc` / `RasterizerDesc`，单个 shader 约 2.2 KB）在二进制产物中**完全丢失**——即打成 bundle 的 shader 丢掉了全部渲染状态。原 JSON 写入器改用 `convert(type::get<SmartPtr<Object>>())` 因而未踩到，但它随后 `get_value<SmartPtr<Object>>()` 的取值在 convert 失败时属于未定义行为。

统一实现改为 `isNullValue()`：wrapper 只在**能转成 `SmartPtr<Object>` 且其为空**、或转换失败（`wrapper_mapper::convert` 仅在源非空且类型匹配时置成功位）时才判空，不再把"转换失败"一律当空。

**2）无序容器迭代顺序对变体拷贝次数敏感**

原两套实现对属性变体的处理次数不同：JSON 直接使用 `prop.get_value()` 的变体，Bin 先拷贝进 `entries` 数组。变体内若持有 `unordered_map`，多一次拷贝就会改变桶布局进而改变迭代顺序。统一成一份遍历后**无法同时**与两种旧产物字节一致，只能择一。

选择保持 JSON 字节稳定（`entries.emplace_back(..., std::move(propValue))`），理由：JSON 是纳入版本管理、需人工 diff 的资产格式，编辑器重存不应产生无意义差异；二进制是可随时重新生成的构建产物，其无序容器顺序变化不影响语义（往返自检已覆盖）。

### 阶段 2：抽象读取端（Bin 先接，保流式）— 已完成
- [x] 定义 `IArchiveReader`（前向 pull；实际接口与草图的三处调整见 §4.2）。
- [x] 实现 `BinArchiveReader`（搬现有 Bin 流式读，**零物化、且不再需要 seek**）。
- [x] 实现共享 `RttrArchive::read` 及其 `readNew/readInto/readIntoKind/readElement/fillObjectMembers/readSeqInto/readMapInto`（含 `SERIALIZE_ALIAS` 回退、装载侧生命周期回调）。
- [x] `BinSerializer::deserialize`（两个重载）改为委托共享遍历；`T3DBinSerializer.cpp` 中被取代的 `RTTRObjectBinReader`（约 610 行）已删除，该文件现仅剩 `create()` + 两端薄封装约 60 行。
- [x] **验证**：见 §9.2 实测结论。

#### 阶段 2 的两个实现要点

**1）属性剩余计数栈 + `endObject()` 兜底排空**

接口按 `nextProperty` 拉取，而磁盘上是「个数 + N 个 (名字, 值)」，故 `BinArchiveReader` 保留唯一一处状态：按嵌套深度记录各层剩余属性数的栈。`endObject()` 会把该层尚未读取的属性连名字带值一起消费掉，从而**统一**了原先分散的两处流对齐处理——原 `ReadNew` 在"类型未注册"分支里手写了一段消费对象体的循环，现在只需 `beginObject` 后直接 `endObject`。遍历层任何提前跳出都不会再让流失去对齐。

**2）修掉悬垂的顶层资源指针**

原 `RTTRObjectBinReader::mObj` 指向 `ReadNew` 的局部变体，函数返回后即悬垂（仅因后续无人解引用而未暴露）。共享实现把它收进 `ReadContext::mTopResource`，并在离开该对象作用域前显式置空，语义上不再依赖"恰好没人再用"。

### 阶段 3：JSON 读取接入（DOM 顺序迭代）— 已完成
- [x] 实现 `JsonArchiveReader`：基于 `Document`，对象属性按 `MemberBegin/MemberEnd` **顺序迭代**（不再按 RTTR 属性名 `FindMember` 查找）。值信封与 map 键值标签仍按名取，理由见下。
- [x] `JsonSerializer::deserialize`（两个重载）改为委托共享遍历；`T3DJsonSerializer.cpp` 中被取代的 `RTTRObjectJsonReader` 与 `JsonStream`（约 590 行）已删除，该文件现仅剩 `create()` + 两端薄封装约 60 行。
- [x] 补齐 JSON 读取的 `SERIALIZE_ALIAS` 回退（原本缺失，随共享遍历自动获得）。
- [x] 顺带统一 `deserialize(stream, obj)` 的错误码：JSON 侧原先按失败点返回 `T3D_ERR_MISSING_OBJECT` / `T3D_ERR_DESERIALIZE_OBJECT`，现与 Bin 侧一致（头部错误原样透传，内容还原失败一律 `T3D_ERR_DESERIALIZE_OBJECT`）。
- [x] **验证**：见 §9.3 实测结论。

#### 阶段 3 期间发现并修复的缺陷

**JSON 不携带数字宽度，按 JSON 形状推断标签会让边界值读不进去**

`IArchiveReader::readKind` 要求给出值的确切宽度标签。二进制有显式的 kind 字节，JSON 却只有"数字"这一种形态，因此第一版实现按 rapidjson 的数字形态推断，把所有小数一律报成 `F64`。这逼出了一次原本不存在的 `double → float` 收窄转换，而 rttr 的收窄带范围校验：

```cpp
// rttr/detail/variant/variant_data_converter.h
static bool to(const double& from, float& to) {
    if (from < float_min || from > float_max) return false;   // 越界即失败
    to = static_cast<float>(from);  return true;
}
```

`SamplerDesc::MaxLOD`（`Real MaxLOD {FLT_MAX}`，落盘为 `3.4028234663852887e38`）恰好卡在边界上，转换失败 → 变体仍是 `double` → `set_value` 失败 → 属性读不进去。二进制路径不受影响：它的标签就是 `F32`，读出的变体类型与声明类型相同，**根本不做转换**。

修法是让 JSON 也用信封里的 `RTTI_Type` 名恢复确切宽度（`arithmeticKind()` 与写入端 `writeAtomic` 的选择一一对应），JSON 形状只作为类型名不可用时的兜底。这样两个后端都变成"标签即声明类型、遍历层零转换"，一次性消除了整类收窄转换的边界问题，而不只是修好 `MaxLOD` 一个值。

> 该缺陷对现有资产是字节中性的——`MaxLOD` 的默认值恰好就是 `FLT_MAX`，读不进去也看不出差异。它只在日志里露头，这也再次说明"零序列化告警"这条判据的价值。

#### 阶段 3 的两个实现要点

**1）DOM 游标帧栈**

前向 pull 的全部状态是一个 `JsonReadFrame` 栈：值信封、对象成员游标、数组元素游标、键值对条目各占一帧，进出严格与接口的 `readKind/endValue`、`beginObject/endObject` 等调用配对。容器元素在接口上没有单独的"取下一个"调用，是由 `readKind` 自身从数组帧的游标推进的；属性值与 map 键/值则由 `nextProperty` / `readMapKeyTag` / `readMapValueTag` 显式指定给下一次 `readKind`。

**2）保留按名取信封键，而非改成按位置取**

§3.1 曾计划把 `FindMember(RTTI_VALUE / MAP_KEY / MAP_VALUE)` 一并改成固定顺序读。实际只把**对象属性**改成了顺序迭代——这才是关键性质（读取顺序与落盘顺序一致，与 Bin 的 `fillObjectMembers` 对齐）。值信封与 map 条目都是**定长两键记录**，按名取是 O(2) 且对手改过键序的资产更宽容，改成按位置取只会白白降低容错。真流式（阶段 4）时这两处自然会变成顺序读，届时再改。

另需注意：JSON 把顺序容器和 `set` 都写成 `[...]`，两者元素形状完全相同。键值对容器可由"首元素是 `{RTTI_Map_Key, ...}` 条目"判出（不依赖类型注册），但 `set` 与顺序容器只能靠 `RTTI_Type` 名判定。资产里确有 `ShaderKeyword::Keys`（`std::set<std::basic_string<char>>`）走这条路径且验证通过，说明容器类型名可靠可解析。

### 阶段 4：JSON 真流式 — 已完成

原判断「接口不变、替换实现即可」不成立：`beginArray/beginMap` 返回元素个数这一点 SAX 下无法满足，接口必须先改。

- [x] `IArchiveReader` 的容器读取从「先取个数再循环」改为 `nextElement()` 拉取（与 `nextProperty` 对称）。`beginArray` 保留返回值但降级为**预分配提示**（未知时返回 0）；`beginMap` 不再返回个数与 `keyOnly`。
- [x] `RttrArchive::readSeqInto` 改为拉取循环 + 按需增长容器，并在落盘个数少于提示时收缩；`readMapInto` 改为拉取循环。
- [x] `BinArchiveReader` 自行维护元素计数栈（`ElemFrame`），`endArray/endMap` 补齐未读元素以保持流对齐（与既有 `endObject` 同构）。
- [x] 用 `rapidjson::Reader` 的 `IterativeParseNext` 重写 `JsonArchiveReader`，不再物化 DOM。
- [x] **验证**：见 §9.4 实测结论。

#### 如何把 push 式的 SAX 反转成 pull

`IterativeParseNext` 每次调用**恰好触发一次** handler 回调（分隔符 `:`/`,` 不算），因此只要用一个 handler 把单个事件抓存成 `JsonToken`，就得到一条 pull 式 token 流。读取器内部只保留 **1 个 token 的前瞻槽**，`IArchiveReader` 的语义即可全部落地：

| 接口调用 | SAX 实现 |
| --- | --- |
| `readKind` | 消费信封 `{`，按序扫键取到 `RTTI_Type`，遇 `RTTI_Value` 停；前瞻载荷 token 定标签 |
| `nextProperty` | 前瞻：是 `Key` 则消费并返回键名，是 `}` 则返回 false |
| `nextElement` | 前瞻：不是 `]` 就返回 true |
| `skipValue` / `skipPayload` | 深度配平地消费一个完整值 |
| `end*` | 消费对应的 `}` / `]`，不匹配即置失败位 |

由此**不再需要任何游标帧栈**——阶段 3 的 DOM 实现需要一个帧栈来记录节点位置，流式实现里"位置"就是流本身。

配套把 DataStream 桥接换成带 4KB 读缓冲的版本：SAX 是逐字节 `Peek/Take` 的，而改造前的桥接每次 `Peek` 都要 `>>` 一字节再 `seek(-1)` 回退，每字节两次虚调用加一次定位。

#### 阶段 4 期间发现并修复的缺陷

**1）把 `kindOf` 作用在过期 token 上（本阶段引入）**

`readKind` 扫描信封键的循环是因「取到 `RTTI_Value` 这个 Key」而退出的，此时载荷 token 尚未读入前瞻槽，槽里留的是那个 Key。第一版直接把 `token` 成员传给 `kindOf`，于是一律落到 `default` 分支返回 `Null`，把载荷当空值吞掉，下一步 `endValue` 就撞在下一个属性的 Key 上——22 个资产全部在同一位置报 `Unbalanced json value envelope`。

根因是"token 成员只有在前瞻槽已装填时才有效"这个前提没有被类型系统或接口保护。修法不只是补一次 `peek()`，而是引入 `peekToken()` 访问器作为取 token 的唯一入口（内部保证先装填、取不到时返回 `None` 空 token），并把所有叶子读取也改为经由它，使同类错误写不出来。

**2）标签为结构类型时未丢弃载荷（既有缺陷）**

`readIntoKind` 的枚举分支与字符串分支在 `readAtomicByKind` 返回无效变体（落盘标签是对象/容器等结构类型）时只告警、不调 `skipPayload`，载荷没有被消费。二进制流会因此**错位**，后续属性全部读坏。这是阶段 2 就存在的缺陷，只是需要 schema 漂移才触发、现有资产碰不到；流式 JSON 同样受影响，故一并补上。

#### 容错权衡：流式换来的严格性

流式无法回退定位，因此以下场景比 DOM 实现更严格（都只影响**手改过**的 JSON，引擎自身写出的永远是规范顺序）：

| 场景 | DOM（阶段 3） | 流式（阶段 4） |
| --- | --- | --- |
| 信封内 `RTTI_Value` 早于 `RTTI_Type` | 按名取，可容 | 类型名取不到，退化为按 token 形态判定（数字丢宽度、容器判不出关联） |
| 键值对条目内值早于键 | 按名取，可容 | 告警并按位置读，键值可能错位 |
| 顺序容器 vs `set` | 首元素形状可判键值对容器，仅 `set` 依赖类型名 | **全部**依赖 `RTTI_Type` 名可解析 |
| 文档尾部有多余内容 | DOM 解析会报错 | 读完根值即停，不再校验尾部 |

其中第三条是风险面的实质扩大：关联容器的识别从"形状可判"退回"必须类型名可解析"。实测 `std::map` / `std::unordered_map` / `unordered_multimap` / `std::set` 四类容器名均可解析且验证通过，且判错时遍历层会告警而非静默丢数据，可接受。

### 阶段 5：收尾 — 已完成
- [x] 删除 `T3DJsonSerializer.cpp` / `T3DBinSerializer.cpp` 中被共享遍历取代的重复代码，仅保留 `create()` + `serialize/deserialize` 薄封装（随阶段 2、3 一并完成，两个文件各约 60 行）。
- [x] 删除 `SerializerManager::serializeObjectWithoutType` / `deserializeObjectWithoutType`：空壳实现、无任何调用方，对应的模板封装也早已被注释掉。
- [x] 标注格式版本语义与兼容策略（`T3DBinArchive.h` / `T3DJsonArchive.h`）。
- [x] 把统一后的**行为基线**写进 `T3DRttrArchive.h`——语义的唯一实现处就是它，基线放在别处必然过期。
- [x] 收敛 `deserialize` 接口：删除返回 `RTTRObject` 的单参数重载。

#### 为什么收敛 deserialize 接口不只是洁癖

原条目把它标为"可选"，理由是"语义易错"。实际查证后发现问题比"易错"更具体——被删掉的实现是这样的：

```cpp
RTTRObject JsonSerializer::deserialize(DataStream &stream)
{
    RTTRVariant obj;
    ...
    return obj;   // 用**函数局部** variant 构造非拥有视图并返回
}
```

`rttr::instance` 的构造函数存的是 `var.get_data_address_container()`，即被持有数据的**地址**。对 `as_raw_ptr` 策略注册的类型，这个地址就是堆对象的裸指针，视图拷走它，局部 variant 析构也不影响——所以这段代码能工作。但只要有一个可序列化类型改用 `as_object` 策略，数据地址就落在 variant 的内部存储里，函数返回即悬垂。

也就是说：**这个函数的正确性隐式依赖"所有可序列化类型都注册为 as_raw_ptr"，而这个前提没有任何地方写明或校验。** 收敛成出参形式后，variant 由调用方持有，指针有效期变得显式可控。

需要说清的是，收敛**并没有**消除 `as_raw_ptr` 的所有权负担：`deserialize<T>(stream)` 仍然返回裸指针、仍需调用方用 SmartPtr 接管（各 `ResourceManager` 正是如此，`BundleBuilder` 则显式用 `ObjectPtr owner` 接管）。那是 RTTR 注册策略固有的，不是这个接口造成的。所以做法是：保留 `deserialize<T>` 的签名不动（约 20 处运行时资源加载调用点零改动），只把它的实现改到出参路径上，并把所有权契约写进注释。

#### 阶段 5 补记：顺着所有权契约查出并修掉一处真实泄漏

上面把"调用方须用 SmartPtr 接管"写成了注释里的契约。事后按这条契约逐点核对，先确认了机制本身是对的：

- `Object::mReferCount` 从 **0** 起（`T3DObject.h`），`SmartPtr(Object*)` 构造时 `acquire()` 到 1，析构 `release()` 归零即 `delete this`。所以"裸指针立刻包进 SmartPtr"不多不少正好一次释放。
- 全部约 20 处 `deserialize<T>` 调用点都确实接管了：各 `ResourceManager::loadResource` 靠返回值隐式转成 `ResourcePtr`，工具侧一律是 `MetaPtr` / `ShaderPtr` 局部量。**正常路径无泄漏。**

但有一条分支不满足契约——`try_convert<T>()` 返回 `nullptr` 时：

```cpp
// 修复前
return RTTRObject(var).try_convert<T>();   // 失败返回 nullptr，堆对象已建好却无人接管
```

反序列化本身成功，对象已经在堆上建出来了，只是落盘类型与请求类型不符（错标/损坏的资产），于是既不会返回给调用方、也没有任何人释放它——连同其全部 SmartPtr 成员构成的**整棵对象图**一起泄漏，且没有任何日志。

修复：新增 `Serializer::discardUnclaimed()`，在这条分支上就地接管并销毁，同时把"类型不匹配"这件事报出来（原先只是静默返回 `nullptr`）。它按 `var.get_type().is_pointer()` 区分两种构造策略——`as_object` 的值类型由 variant 自己持有并释放，无需干预；只有 `as_raw_ptr` 产出的裸指针才需要就地回收。

关于"是否所有类型都是 `as_raw_ptr`"，这次也查清了确切规则（`T3DReflectionGenerator.cpp`）：**`Tiny3D::Object` 派生类一律 `as_raw_ptr`**，非 Object 的 `TSTRUCT` 默认 `as_object`、可用 `RTTRConstructAsPointer` 显式改成裸指针（全仓库仅 `Launcher::ProjectInfo` 一处）。这解释了为什么双参数的 `deserialize<T>(stream, T&)`（用于各类 settings 值类型）没有同类泄漏：那条路径上 variant 自己持有存储。

验证方式是造一个错标类型的资产（把某 `.meta` 的根 `RTTI_Type` 从 `Tiny3D::MetaMaterial` 改成 `Tiny3D::Texture2D`）后跑 BundleBuilder，日志给出：

```
Serializer | Deserialized type [Tiny3D::Texture2D*] mismatches requested type [Tiny3D::Meta] !
```

两个要点：类型显示为 `Texture2D*`（**指针**），直接印证了 `as_raw_ptr` 下 variant 不持有对象、不回收就必然泄漏；而"不是 Object 派生类、无法回收"的警告没有出现，说明 `try_convert<Object>()` 成功、`ObjectPtr` 确实接管并销毁了它。正常资产集上该分支零触发，产物与往返自检均无变化。

---

## 8. 收益与代价

### 收益
- 消除约 600+ 行重复遍历逻辑，语义只维护一份，从源头杜绝"改了 JSON 忘了 Bin"。
- 顺带修复 §1.3 的四处行为不一致（生命周期开关、alias 回退、未知属性、枚举表示）。
- **两个读取端都是纯流式**：Bin 由阶段 2 保证，JSON 由阶段 4 补齐，峰值内存不再随文档大小增长。
- 后续新增格式（如 MessagePack、CBOR）只需实现一个后端，零改遍历逻辑。
- 顺带清掉的隐患：`RenderState` 子树丢失（阶段 1）、无序容器导致输出不确定（阶段 1）、`MaxLOD` 这类 float 属性读不进去（阶段 3）、标签为结构类型时载荷未丢弃导致二进制流错位（阶段 4）、返回非拥有视图的 `deserialize` 重载隐式依赖 `as_raw_ptr` 注册策略（阶段 5）、类型不匹配时反序列化产物无人接管导致整棵对象图泄漏（阶段 5 补记）。

### 代价与注意点
- 共享遍历必须**严格 in-order**：比随机访问树受限，但两边现状本就是顺序遍历，代价接近零。
- JSON 的对象属性、容器元素、值信封与键值对标签**全部按存储顺序读**（阶段 3 改属性，阶段 4 改其余）。手改过的 JSON 若打乱了信封内或键值对内的键序，容错能力弱于 DOM 实现，权衡见阶段 4 记录。
- `skipValue/skipPayload` 必须进接口并两端实现，且都是前向深度配平地跳过。原计划的不消费 `peekKind` 已证明不必要，见 §4.2。
- 落盘格式表达力不足的地方要靠 RTTI 类型名补：JSON 的数字不带宽度、顺序容器与关联容器同形，两处都依赖 `RTTI_Type` 名可解析。流式化后关联容器的识别**完全**依赖类型名（无法前瞻首元素形状），风险面比阶段 3 略大，见阶段 4 记录。
- 容器元素个数只有二进制后端能预知，因此接口按拉取设计、`beginArray` 的返回值只是预分配提示；顺序容器在流式后端下按需增长，比一次 `set_size` 多几次 resize。

---

## 9. 验证策略

### 9.1 阶段 1 实测结论

验证载体为 `bundlebuilder`（`source/Tools/BundleBuilder`），它同时覆盖两个写入端：

| 命令 | 覆盖的写入端 |
| --- | --- |
| `bundlebuilder --assets assets --out <dir> --verify` | Bin 写入器（JSON→对象→T3DB），并逐资源做往返自检 |
| `bundlebuilder --assets assets --out <dir> --keep-languages hlsl,glsl,essl,spirv` | JSON 写入器（`.tshader` 裁剪语言变体后重新序列化） |

做法：`git stash` 掉序列化器改动生成改造前基线，恢复后再生成新产物，逐文件 SHA256 比对。基线与新代码各自连跑两次均为 0 差异，确认两边都是确定性输出，比对结果可信。

结论（共 34 个产物文件）：

- **JSON：0 个差异**，与改造前逐字节完全一致。
- **BIN：9 个差异**，全部已定性：
  - 5 个 `.tshader` 体积增大（如 4296→6521 字节），即上述 `RenderState` 丢失缺陷被修复后补回的子树；
  - 3 个 `.tmat` + 1 个 `.tskel` 体积不变、内容为无序容器元素置换（字符串多重集比对完全相同，属纯排列），语义等价。
- 往返自检：22 个二进制资源 **22 passed / 0 failed**。

### 9.2 阶段 2 实测结论

Bin 读取端在 `bundlebuilder --verify` 中被完整覆盖：`verifyRoundtrip` 用**新读取器**反序列化每个二进制产物，再重新序列化并比对字节长度（长度对无序容器的元素顺序不敏感，但对丢属性、流失去对齐、类型读错都敏感）。

| 检查项 | 结果 |
| --- | --- |
| `T3DCore` 编译 | 0 error、0 warning（含无 C4819） |
| 往返自检（22 个二进制资源） | **22 passed / 0 failed** |
| 序列化层告警/错误日志 | **0 条**（无属性未找到、无不兼容跳过、无 insert/set 失败、无类型未注册） |
| BIN 产物 vs 阶段 1 基线（34 文件） | 逐字节 **0 差异** |
| JSON 产物 vs 阶段 1 基线（34 文件） | 逐字节 **0 差异** |

"0 条序列化告警"是这一阶段最关键的判据：阶段 1 踩过的坑正是**静默丢数据**（`RenderState` 整棵子树消失却不报错），而共享读遍历在属性查不到、标签与声明类型不符、容器插入失败时都会打日志，全程零告警意味着 22 个资源的每一个属性都按声明类型正常读入。

流式性质由构造保证而非实测：`BinArchiveReader` 只有前向 `read` 与 `skipBytes`（`seek` 仅用于跳过，不回退），不构建任何中间树，`readBuffer` 直接把字节读进目标 `Buffer`，因此大 `Buffer` 场景的内存占用与改造前一致。

### 9.3 阶段 3 实测结论

JSON 读取端是 `bundlebuilder` 的**输入**路径，因此整条流水线就是它的端到端测试：22 个资源经新 JSON 读取器还原成对象图后写成二进制，产物只要与基线逐字节一致，就说明还原出的对象图与改造前完全相同。

| 检查项 | 结果 |
| --- | --- |
| `T3DCore` 编译 | 0 error、0 warning（含无 C4819） |
| BIN 产物 vs 基线（34 文件，读端为新 JSON 读取器） | 逐字节 **0 差异** |
| JSON 产物 vs 基线（34 文件，读写两端均为新实现） | 逐字节 **0 差异** |
| 往返自检（22 个二进制资源） | **22 passed / 0 failed** |
| 序列化层告警/错误日志 | **0 条** |

覆盖到的语义路径（均由现有资产实际触发）：多态属性与容器内多态元素（`SmartPtr<Pass>` 列表）、键值对容器（`std::map` / `std::unordered_map` / `unordered_multimap`）、只有键的容器（`std::set`）、原生 C 数组（`RTBlendDesc[8]`）、`Buffer`（base64）、`UUID`、枚举按名落盘、`Real`(float) 边界值。

未被现有资产触发、仅靠与 Bin 共用同一份代码来保证的路径：`SERIALIZE_ALIAS` 改名回退、未知属性跳过、枚举按数值落盘、空指针 `Null` 值。

> 修 `MaxLOD` 前后产物字节均与基线一致（默认值恰为 `FLT_MAX`），该缺陷**只能靠日志发现**。这条经验值得沿用：字节比对确认"没有变化"，日志扫描确认"没有静默降级"，两者缺一不可。

### 9.4 阶段 4 实测结论

| 检查项 | 结果 |
| --- | --- |
| `T3DCore` 编译 | 0 error、0 warning |
| BIN 产物 vs 基线（34 文件） | 逐字节 **0 差异** |
| JSON 产物 vs 基线（34 文件） | 逐字节 **0 差异** |
| 往返自检（22 个二进制资源） | **22 passed / 0 failed** |
| 序列化层告警/错误日志 | **0 条** |

**补测：未知属性跳过。** 这条路径现有资产覆盖不到，而流式实现的 `skipValue` 需要自己做深度配平，风险集中。做法是往 `Test.ttex` 注入一个未注册属性，其值是「数组 → 含键值对条目的嵌套 map → 尾随元素」的深层结构，然后跑转换：

- 日志恰好 1 条 `Property (LegacyJunk) not found in class (Tiny3D::Texture2D), skipping.`；
- 该资产的二进制产物与基线**逐字节一致**，证明垃圾子树被完整跳过、流未错位、其余属性无损。

测完已 `git checkout` 还原资产。

**性能与内存实测。** 全量转换（22 资源，含往返自检）三次取样：4741 / 4651 / 4759 ms，峰值工作集约 330 MB。

需要如实说明：**峰值内存的改善在当前资产规模下测不出来**。最大的 JSON 资产是 461 KB（`Tiny3DStandard.tshader`），其 DOM 只占几 MB，而峰值 330 MB 由 Debug 版引擎与 RTTR 对象图主导。流式的价值是渐进性质——峰值不再随文档大小增长，而非在现有资产上省下可观内存。若将来出现几十 MB 级的文本资产（如带大段内嵌 base64 的网格），差异才会显现。

耗时方面没有做受控 A/B（阶段 3 的 DOM 二进制已被覆盖），因此不宣称具体加速比。可以确定的只是消除了两项固定开销：整棵 DOM 的构建与字符串副本，以及改造前桥接层「每次 `Peek` 都 `>>` 一字节再 `seek(-1)`」的每字节两次虚调用加一次定位。

### 9.5 阶段 5 实测结论

| 检查项 | 结果 |
| --- | --- |
| **全量解决方案**编译（引擎 + 编辑器 + 全部工具与示例） | 0 error、0 warning |
| BIN 产物 vs 基线（34 文件） | 逐字节 **0 差异** |
| JSON 产物 vs 基线（34 文件） | 逐字节 **0 差异** |
| 往返自检（22 个二进制资源） | **22 passed / 0 failed** |
| 序列化层告警/错误日志 | **0 条** |

本阶段改了公共 API，因此编译范围从 `T3DCore` 扩到整个解决方案，以确认约 20 处 `deserialize<T>` 调用点、编辑器与全部工具都不受影响。

**过程中的一个自摆乌龙值得记下**：阶段 4 补测未知属性跳过时改过 `Test.ttex`，测完用 `git checkout` 还原。但仓库 `core.autocrlf=true`，checkout 把工作区文件的行尾从 LF 改写成了 CRLF，而 `.ttex` 在 bundle 里是**字节拷贝**（源文件与产物哈希相同），于是 JSON 产物比对多出 1 个差异文件。表征很容易误判成代码问题——但同一资产的 BIN 产物完全一致，说明对象图没变；进一步看差异是「行数相同、无行级差异、大小差 129 字节 ≈ 129 个 CR」，才定性到行尾。已把该文件转回 LF，重跑后 0 差异。

教训：用 `git checkout` 还原测试期间改动的资产时，若仓库开了 `autocrlf`，还原的是**内容**而非**字节**。做字节级比对的项目要留意这一点。

**补记（泄漏修复）的验证结果**：全量解决方案编译通过（仅 `SLParserLex.cpp` 这个 flex 生成词法器的 stdint 宏重定义 C4005 属既有告警，与本改动无关）；BIN / JSON 产物各 34 文件逐字节 0 差异；往返自检 22 passed / 0 failed；正常资产集上新增的回收分支**零触发**。因"零触发"意味着修复未被跑到，另用错标类型的资产做了一次定向验证，确认分支行为正确（见上文阶段 5 补记）。

### 9.6 后续阶段沿用的验证项

- 复用现有 `Temp/bin_verify_*`、`bin_leak_check*` 等往返/泄漏校验目录的用例。
- 关键回归项：
  1. 同一对象 JSON 与 Bin 分别序列化→反序列化，字段完全一致。
  2. Bin 字节级往返一致（`serialize→deserialize→serialize` 二进制相同）。
  3. 大 `Buffer`（Mesh/Texture）反序列化内存峰值不高于改造前（验证流式）。
  4. 多态属性（`SmartPtr<Object>` 派生类）、容器内多态元素还原正确。
  5. `SERIALIZE_ALIAS` 改名兼容在 JSON / Bin 均生效。
  6. `isInvokeLifecycleCallbacks=false`（离线打包）时无引用环泄漏、无扁平层级重采集破坏往返。
