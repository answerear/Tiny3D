# ImageConverter 纹理导入工具设计与分步计划

> 目标：新增命令行工具 **ImageConverter**（`imgconv`），调用 FreeImageCodec 等图像编解码器把 png / jpg 等源图片统一转换成引擎纹理资源 `*.ttex`。引擎内（包括 RmlUi）所有图片都只引用 `.ttex`，便于统一管理。
>
> **转换逻辑只在 ImageConverter 里实现**，不进 Core。BuiltinGenerator、MeshConverter、TinyEditor 等需要导入纹理的程序都通过启动 `imgconv` 子进程完成，双方用 **socket 进程间通信** 交换请求和结果（复用 TinyEditor / TinyLauncher 已在使用的 `TCPListener` / `TCPConnection` + protobuf）。Core 只增加运行时和 meta 必须用到的数据字段。
>
> 核心诉求之一：**预乘 alpha（premultiplied alpha）要按纹理区分**。UI 纹理需要预乘，3D 材质纹理通常不需要，因此预乘必须是每张纹理各自的导入设置，并且结果要写进 `.ttex` 供运行时判断。
>
> 本文档为施工蓝图，接口与代码片段均为「建议实现」，不代表已落地。

---

## 1. 背景与目标

### 1.1 背景

- RmlUi 本身不解码图片。所有图片来源都经过 `Rml::RenderInterface::LoadTexture(dimensions, source)`，`source` 字符串由引擎自行解释；`GenerateTexture` 只接受已解码、已预乘的 RGBA8 像素，不接受编码后的文件 buffer。
- `source/Plugins/RmlUi/Source/T3DRmlRenderInterface.cpp` 中，`.ttex` / `.ttexture` 已经走 `T3D_ASSET_MGR.loadTexture` 复用引擎纹理；其他后缀走回退分支（Archive 读字节、`T3D_IMAGE_CODEC` 解码、CPU 预乘、上传）。
- RmlUi 混合状态为 `SrcBlend = One` / `DestBlend = OneMinusSrcAlpha`，即假定纹理已预乘。`.ttex` 分支目前不做预乘，RmlUi shader 里也没有预乘，所以半透明 `.ttex` 图片会出现边缘偏亮、白边。
- 引擎目前没有独立的"图片转 `.ttex`"工具。`.ttex` 只由 MeshConverter（导出模型附带纹理）和 BuiltinGenerator 生成。

### 1.2 本期目标

1. Core 新增 `TextureImportSettings`，扩展 `MetaTexture` 和 `Texture`，支持按纹理记录导入参数和预乘结果（只有数据，没有转换逻辑）。
2. 新增命令行工具 ImageConverter（`imgconv`），完整实现图片到 `.ttex` 的转换流程。
3. `imgconv` 支持两种运行方式：给人用的命令行模式，给程序用的 socket IPC 模式。
4. BuiltinGenerator 中"扫描目录、把图片转成 `.ttex`"的部分改为通过 IPC 调用 `imgconv`。
5. MeshConverter 导出 FBX 引用的纹理时改为通过 IPC 调用 `imgconv`。
6. RmlUi 运行时根据 `Texture` 的预乘标记兜底处理，迁移 UI 资源后删除非 `.ttex` 回退分支。

### 1.3 已确定的决策

| 事项 | 决策 |
| --- | --- |
| 可执行文件名 | `imgconv` |
| mip 生成 | 参考 Unity：`Default`、`NormalMap` 类型默认**生成** mip，`UI` 类型默认**不生成**；可用 `--mips` / `--no-mips`（或 IPC 请求中的设置）覆盖 |
| MeshConverter | 本期改为调用 `imgconv` 导出纹理，不再在进程内转换 |
| 转换逻辑位置 | 只在 `imgconv` 中实现，不进 Core |
| 程序间调用方式 | socket IPC，复用 System 库的 `TCPListener` / `TCPConnection`，消息体用 protobuf |
| 报告文件 | 只给人看（可选的文本汇总），**不作为程序接口** |
| IPC 请求参数 | `Import` 请求携带**与命令行完全相同的参数数组**（`repeated string args`），与命令行共用同一个解析器；只有结果（`Result` / `Done`）是结构化 protobuf 字段 |
| UUID | 参考 MeshConverter：**由调用方或参数指定**。用 `--uuid` 显式指定，未指定时按 `-u reuse\|new` 策略处理；IPC 调用方在请求参数里带 `--uuid`（见 §5.4） |
| meta 生成 | 参考 MeshConverter：带 `-a` 时才写 `.ttex.meta`，由 `imgconv` 自己写完整的 `MetaTexture`（见 §5.5）。`imgconv` **只写输出侧的 `.ttex.meta`，从不写源图片侧的 meta** |
| 写盘方式 | 先写 `.tmp` 再 rename；带 `-a` 时先 rename meta、后 rename `.ttex`，保证文件监控方看到 `.ttex` 时 meta 已就位（见 §5.6，与 `MetaFS-Monitor-Fixes-todo.md` §4 的约定一致） |

### 1.4 本期边界（暂不实现）

- GPU 压缩格式（BC / ASTC / ETC）及按平台选择格式。
- sRGB 像素格式（`sRGB` 设置本期只记录，不生效）。
- 法线贴图专用处理（重新归一化、通道重排）。
- cubemap / 纹理数组的导入（BuiltinGenerator 现有 cubemap 逻辑保持不变）。
- FBX 内嵌纹理（MeshConverter 当前也只支持外部图片文件）。
- **TinyEditor 集成**：另行设计，与 meshconv 在编辑器中的使用一起重新分析现状后写入单独文档。目前倾向参照 ShaderLab：源图片放在 `Assets`，其 meta 由编辑器独占（记录资源 UUID 与导入设置），`imgconv` 产物输出到 `Temp` 目录。本文只保证 `imgconv` 能满足这种用法：不带 `-a` 运行、输出到任意目录、用 `--uuid` 指定 UUID、全部导入设置可由参数给出。
- **MetaFS 的修改**（映射表登记、外部写入协同）：见 `MetaFS-Monitor-Fixes-todo.md`。

---

## 2. 现状分析

| 项目 | 现状 | 对本方案的影响 |
| --- | --- | --- |
| mip 数据布局 | D3D11 `buildSubresourceData` 把 `Buffer` 当作"所有 mip 首尾相连、行不对齐"读取 | `imgconv` 要自己生成整条 mip 链，紧密拼进一个 `Buffer` |
| `createTexture2D(name, Image*, mipmaps)` | 只拷贝第 0 级数据 | 带 mip 时必须用传 `Buffer` 的重载 |
| `PixelFormat` | 没有 sRGB、没有 BC / ASTC / ETC | `sRGB`、`Compression` 本期只预留 |
| `Image` | 已有 `convert`、`copy`（带过滤）、`flip` | `imgconv` 直接复用；预乘在工具内遍历像素实现，不给 `Image` 加方法 |
| `Texture` | 没有预乘标记 | 新增 `PremultipliedAlpha` 属性 |
| `MetaTexture` | 只有 UUID | 新增 `SourceHash` / `ImportSettings` |
| 工具初始化 | BuiltinGenerator 加载 `FileSystemArchiveEditor`、`FreeImageCodecEditor`、`NullRendererEditor`，Null 渲染器，链接 `T3DCoreEditor` | `imgconv` 照抄 |
| 子进程 | Platform 已有 `Process`（`start` / `wait` / `getExitCode`，Win32 与 Posix 实现），不捕获 stdout | 调用方用 `Process` 启动 `imgconv`；结果通过 socket 回传 |
| socket 通信 | System 库 `TCPListener` / `TCPConnection`：非阻塞、`poll()` 驱动、回调式；8 字节包头（magic / `uint16_t` 长度 / seq）完成分包；TinyEditor 与 TinyLauncher 用它 + protobuf（`tools/protobuf/proto/editor/NetPackage.proto`）通信 | 直接复用；需补"读取实际监听端口"接口（见 §7.2） |
| MeshConverter 的 UUID 处理 | 命令行 `-u reuse\|new`（默认 `reuse`）。`reuse` 时 `FBXImporterNew::lookupReusableUUID` 调 `ConverterGuidUtil::readExistingMetaUUID` 从输出目录的旧 `.meta` 读 UUID，读不到则新生成；`new` 时 `EngineExporter::removeStaleMeta` 写资源前删除旧 `.meta`。UUID 在创建资源时传入 | `imgconv` 照搬这套策略，并增加 `--uuid` 显式指定；IPC 模式下 UUID 由调用方决定 |
| MeshConverter 的 meta 生成 | `-a` 时用 `ARCHIVE_TYPE_METAFS` 写资源，由 MetaFS 插件 `MetaFSMonitor::generateMetaFile` 生成 meta：**只在 `.meta` 不存在时**按资源对象的 UUID 生成，且只含 UUID；不带 `-a` 时不写 meta | `imgconv` 保留 `-a` 语义，但不走 MetaFS，自己写完整 `MetaTexture`（需要写入导入设置，且要能覆盖 UUID 不同的旧 meta，见 §5.5） |
| MetaFS 对外部写入的处理 | 编辑器的 MetaFS 监控在 meta 已存在时不登记映射表，忽略修改事件，加载半写文件时会崩溃，覆盖式 rename 可能误删 meta | `imgconv` 按 §5.6 的顺序写盘；MetaFS 侧的修复见 `MetaFS-Monitor-Fixes-todo.md` |
| BuiltinGenerator 的 UUID 处理 | `BuiltinGuidUtil::readExistingMetaUUID` 读旧 meta 的 UUID，创建纹理时传入，自己写 `MetaTexture` | 保留由它计算 UUID，以 `--uuid` 放进请求参数 |
| MeshConverter 纹理 | `FBXImporterNew::createTexture` 读取 FBX 引用的外部图片，OpenGL 制式时 `flip()`，按 FBX 的 WrapMode 设置寻址，创建纹理后立即把 UUID 写进材质；最后由 `EngineExporter::exportTexture` 写 `.ttex` | 改用 `imgconv` 后，MeshConverter 先按自己的 `-u` 策略确定纹理 UUID 并写进材质，再把 `--uuid`、`--flip-y`、`--address-u/v` 等放进请求参数 |

---

## 3. 整体设计

```text
 ┌──────────────────┐ ┌──────────────────┐ ┌──────────────────┐ ┌──────────────────┐
 │ 命令行/打包脚本  │ │ BuiltinGenerator │ │ MeshConverter    │ │ TinyEditor(后续) │
 └────────┬─────────┘ └────────┬─────────┘ └────────┬─────────┘ └────────┬─────────┘
          │ 命令行参数         │ TCPListener 监听 127.0.0.1:<随机端口>          │
          │                    │ Process 启动 imgconv --ipc-port --ipc-token     │
          │                    └──────────────────┬──────────────────────────────┘
          │                                       │ TCPConnection + protobuf
          ▼                                       ▼   Import / Result / Progress ...
        ┌──────────────────────────────────────────────────────┐
        │ imgconv（ImageConverter）                            │
        │  命令行模式：argv → 解析 → 转换 → 控制台输出         │
        │  IPC 模式  ：连回调用方 → 循环处理 Import(args)      │
        │  共用：ImageConverterCommand（解析）                 │
        │        ImageConverterApp（编排：UUID / 设置 / 增量） │
        └──────────────┬──────────────────────┬────────────────┘
                       ▼                      ▼
        ImageImporter                   TextureExporter
        png/jpg → Texture 对象          Texture 对象 → 文件
        T3D_IMAGE_MGR (FreeImageCodec)  saveTexture + MetaTexture
        预乘 / mip / createTexture2D          ▼
                                 xxx.ttex  +  xxx.ttex.meta
```

- **命名与 MeshConverter 一致**：`XXXImporter` 是"外部格式 → 引擎内存对象"（对应 `FBXImporter`），`XXXExporter` 是"内存对象 → 文件"（对应 `EngineExporter`）。Importer 不写盘，Exporter 不解码。

- **转换逻辑只在 `imgconv` 里**。Core 不新增任何转换代码。
- **两种运行方式共用同一个参数解析器和同一条转换流程**，只是参数的载体和结果去向不同：
  - 命令行模式：参数来自进程的 `argv`，结果打印到控制台（可选输出给人看的汇总文件）。
  - IPC 模式：参数来自 `Import` 请求里的 `args` 数组（内容与命令行完全相同），结果通过 `Result` / `Done` 消息回给调用方。
  - 好处：只维护一个解析器；命令行新增的参数 IPC 立即可用，不用改 `.proto`；`imgconv` 把收到的参数打进日志，复制到命令行即可复现问题。
- 导入设置存放在 **`.ttex.meta`（`MetaTexture`）** 中：它本来就保存 UUID，重新导入时读回即可同时拿到 UUID 和上次的设置；编辑器 Inspector 编辑的也是这个 meta。
- 预乘结果写进 **`.ttex`（`Texture::PremultipliedAlpha`）**，运行时据此判断。
- 走子进程的好处：转换崩溃不影响调用方；工具可单独升级；命令行、打包脚本、CI、编辑器用的是同一个程序。
- 一个 `imgconv` 进程可连续处理多个 `Import` 请求，调用方（如 MeshConverter 处理整个模型）只需启动一次。

---

## 4. 第 1 阶段：Core 数据字段

Core 只改数据，不加转换逻辑。

### 4.1 新增 `TextureImportSettings`

位置：`source/Core/Include/Meta/T3DMetaTexture.h`，`TSTRUCT` 反射，`T3D_OS_DESKTOP` 宏包裹。它是 `MetaTexture` 的字段类型，需要反射和序列化，所以直接定义在 `T3DMetaTexture.h` 中。

```cpp
TENUM()
enum class TextureImportType : uint32_t
{
    kDefault = 0,
    kUI,
    kNormalMap,
};

TSTRUCT()
struct T3D_ENGINE_API TextureImportSettings
{
    TextureImportType   TextureType {TextureImportType::kDefault};
    bool                PremultipliedAlpha {false};
    bool                GenerateMipmaps {true};   // 与 Default 类型一致；UI 类型由 imgconv 填为 false
    bool                FlipY {false};
    uint32_t            MaxSize {4096};
    bool                sRGB {true};          // 预留，本期不生效
    String              Compression {};       // 预留，本期不生效
    SamplerDesc         Sampler {};
};
```

各类型默认值由 `imgconv` 负责填充（Core 不放这段逻辑）：

| 类型 | 预乘 | 生成 mip | 寻址 | sRGB |
| --- | --- | --- | --- | --- |
| `Default` | 否 | 是 | Wrap | 是 |
| `UI` | 是 | 否 | Clamp | 是 |
| `NormalMap` | 否 | 是 | Wrap | 否 |

mip 默认值参考 Unity：3D 纹理（`Default`、`NormalMap`）不带 mip 时远处会闪烁、出现摩尔纹，采样缓存命中率也低，所以默认生成；UI 纹理基本按 1:1 显示，mip 只会多占约 1/3 内存，所以默认不生成（对应 Unity `Sprite (2D and UI)` 类型的默认值）。需要时用 `--mips` / `--no-mips` 覆盖。

### 4.2 扩展 `MetaTexture`

新增 `TPROPERTY` 字段：

- `SourceHash`：源文件字节 + 序列化后设置的 hash，用于增量导入。
- `ImportSettings`：`TextureImportSettings`。

> 不记录源文件路径（如 `SourcePath`），避免在外部资源管理器中移动文件后产生脏路径；需要定位关联资源时统一通过 UUID 与 `MetaFSMonitor` 的内存映射表解析。

旧 meta 缺少这些字段时使用默认值。

MetaFS 自动生成的 `MetaTexture` 只含 UUID，不会填这两个字段；它们由 `imgconv` 在 `-a` 模式下写入（见 §5.5）。

### 4.3 扩展 `Texture`

```cpp
TPROPERTY(RTTRFuncName="PremultipliedAlpha", RTTRFuncType="getter")
bool isPremultipliedAlpha() const { return mPremultipliedAlpha; }

TPROPERTY(RTTRFuncName="PremultipliedAlpha", RTTRFuncType="setter")
void setPremultipliedAlpha(bool value) { mPremultipliedAlpha = value; }
```

由 `imgconv` 在创建纹理后设置；运行时（RmlUi）读取。

### 4.4 反射与兼容

- 运行 ReflectionPreprocessor 重新生成 RTTR 注册代码，否则序列化会悄悄丢掉新字段。
- 实测二进制 T3DB 对"多一个字段"是否兼容；保险起见用 BuiltinGenerator / MeshConverter 重新生成现有 `.ttex`。

### 4.5 验收

- 旧 `.ttex` / `.meta` 能正常加载。
- 新字段在文本模式（JSON）下能写入、能读回。

---

## 5. 第 2 阶段：ImageConverter 工程与转换流程

### 5.1 工程结构

```text
source/Tools/ImageConverter/
├── CMakeLists.txt
├── Include/
│   ├── T3DImageConverterPrerequisites.h
│   ├── T3DImageConverterApp.h
│   ├── T3DImageConverterCommand.h      // 参数解析（命令行与 IPC 共用）
│   ├── T3DImageConverterOptions.h      // 解析结果
│   ├── T3DImageImporter.h              // 图片 → Texture 对象（解码、预乘、mip）
│   ├── T3DTextureExporter.h            // Texture 对象 → .ttex / .ttex.meta
│   ├── T3DImgconvIPCServer.h           // IPC 模式：连回调用方、处理请求
│   └── T3DImportSummary.h              // 命令行模式：给人看的汇总输出
└── Source/
    ├── main.cpp
    ├── T3DImageConverterApp.cpp
    ├── T3DImageConverterCommand.cpp
    ├── T3DImageConverterOptions.cpp
    ├── T3DImageImporter.cpp
    ├── T3DTextureExporter.cpp
    ├── T3DImgconvIPCServer.cpp
    └── T3DImportSummary.cpp
```

- 结构参照 MeshConverter：`ImageImporter` 对应 `FBXImporter`，`TextureExporter` 对应 `EngineExporter`，`ImageConverterApp` 负责编排（对应 `ConverterCommand`）。暂不引入 `AssetImporter` / `AssetExporter` 那样的基类，等出现第二对实现（如 `ImageExporter`）时再抽象。
- `Tools/CMakeLists.txt` 增加 `add_subdirectory(ImageConverter)`，工程归入 `Tools` 文件夹。
- 链接 `T3DPlatform`、`T3DSystem`、`T3DLog`、`T3DUtils`、`T3DMath`、`T3DFramework`、`T3DCoreEditor`、`rttr_core`，以及 IPC 公共库（见 §7.4）。
- 可执行文件名 `imgconv`（避免与 GNU `iconv` 重名）。
- 插件加载照抄 BuiltinGenerator：`FileSystemArchiveEditor`、`FreeImageCodecEditor`、`NullRendererEditor`。

### 5.2 参数解析（命令行与 IPC 共用）

```cpp
class ImageConverterCommand
{
public:
    /// 命令行：main(argc, argv)；IPC：把 Import.args 转成 argv 形式后调用同一个函数
    bool parse(const TArray<String> &args, ImageConverterOptions &opts, String &error) const;
};
```

- 命令行模式：`main` 把 `argv[1..]` 转成 `TArray<String>` 调 `parse`。
- IPC 模式：`Import.args` 原样转成 `TArray<String>` 调 `parse`。参数按数组逐个传递，不拼成一整条字符串，因此不涉及引号转义，路径含空格或中文也没问题。
- `parse` 失败时返回明确的错误信息（哪个参数不认识、哪个值不合法）。命令行打印到控制台并返回非 0；IPC 回一条 `status = IS_FAILED` 的 `Result` 和 `Done`。
- 解析得到的 `ImageConverterOptions` 交给 `ImageConverterApp` 编排，`ImageImporter` / `TextureExporter` 不区分调用来源。
- 只在某种模式下有效的参数：

  | 参数 | 命令行模式 | IPC 请求中 |
  | --- | --- | --- |
  | `--ipc-port` / `--ipc-token` | 进入 IPC 模式 | **报错**（只能出现在进程启动参数里） |
  | `--summary` | 输出汇总文件 | 忽略并回一条警告 `Log` |
  | `-i <dir> -r` | 批量处理 | 允许，一个请求对应多条 `Result`，最后一条 `Done` |

### 5.3 单张图片的转换流程

输入：解析后的 `ImageConverterOptions` 中与单个文件相关的部分（源文件、输出目录、输出名、UUID、UUID 策略、是否生成 meta、设置覆盖项、是否强制、是否文本输出）。输出：结果状态、目标路径、UUID、尺寸、格式、mip 数、是否预乘、错误码与错误信息。

**编排（`ImageConverterApp`）**

0. **确定 UUID**。按 §5.4 的规则得到最终 UUID。
1. **合并导入设置**。优先级从高到低：参数中的覆盖项 > 已有 `.ttex.meta` 中的设置 > `TextureType` 默认值 > 目录约定（如 `ui/**` 默认 `UI`）。UUID 策略为 `new` 时不读旧 meta 的设置。
2. **增量判断**。计算"源文件字节 + 序列化后设置"的 hash，与 meta 中 `SourceHash` 相同、且旧 meta 的 UUID 与最终 UUID 相同时跳过（强制模式时不跳过）。没有 meta（未开 `-a`）时总是转换。
3. 把源文件、UUID、合并后的设置交给 `ImageImporter`，得到 `Texture` 对象后交给 `TextureExporter`。

**`ImageImporter`：图片 → `Texture` 对象（不写盘）**

4. **解码**。`T3D_IMAGE_MGR.loadImage`，底层 FreeImageCodec。
5. **统一格式**。`Image::convert` 到 BGRA8（4 字节/像素），保证每行紧密排列，避开 FreeImage 行对齐问题。
6. **翻转与缩放**。按需 `flip()`；超过 `MaxSize` 时用 `Image::copy` + 双线性过滤缩小。
7. **预乘 alpha**。工具内遍历像素实现 `rgb = rgb * a / 255`。
8. **生成 mip（`GenerateMipmaps` 为 true 时）**。逐级用 `Image::copy` 缩小一半直到 1×1，所有级别紧密拼进一个 `Buffer`。**必须在预乘之后**，否则透明像素残留的 RGB 会在过滤时混进边缘。未开启时 `mipmaps = 1`。
9. **创建纹理**。用第 0 步确定的 UUID 调用传 `Buffer` 的 `createTexture2D`，设置 `SamplerDesc` 与 `PremultipliedAlpha`。

**`TextureExporter`：`Texture` 对象 → 文件（不解码）**

10. **清理旧 meta**。UUID 策略为 `new` 且不带 `-a` 时删除旧 `.ttex.meta`（对应 MeshConverter 的 `EngineExporter::removeStaleMeta`）；带 `-a` 时旧 meta 会在第 12 步被新 meta 原子替换，不需要先删。被替换或删除的旧 meta 中的 UUID 记为"旧 UUID"，通过 `Result.previous_uuid` 回报。
11. **写临时文件**。用普通文件系统归档（`ARCHIVE_TYPE_FS`）`saveTexture` 写 `<name>.ttex.tmp`；带 `-a` 时再按 §5.5 写 `<name>.ttex.meta.tmp`（`SourceHash`、`ImportSettings` 由编排层传入）。
12. **rename 就位**。按 §5.6 的顺序 rename 为正式文件名。

### 5.4 UUID 规则（参考 MeshConverter）

| 优先级 | 参数 | 行为 |
| --- | --- | --- |
| 1 | `--uuid <hex>` | 直接使用 |
| 2 | `-u reuse`（默认） | 读输出目录中 `<name>.ttex.meta` 的 UUID；没有 meta 或读取失败则新生成 |
| 3 | `-u new` | 忽略并删除旧 `.ttex.meta`，新生成 UUID |

- 命令行和 IPC 用的是同一组参数，规则完全一致。
- `--uuid` 只对单文件输入有效；目录输入时报错。
- 显式指定的 UUID 与旧 meta 中的不同：以显式指定为准，覆盖旧 meta，并输出警告（这张纹理的身份变了，引用旧 UUID 的资源会失效）。
- **IPC 调用方的约定**：程序调用时应**总是带 `--uuid`**，由调用方按自己的策略决定（MeshConverter 按其 `-u`，BuiltinGenerator 按 `BuiltinGuidUtil`，编辑器按资源数据库）。`-u` 只作为兜底，供不关心 UUID 的调用方使用。
- `Result.uuid` 始终回传最终使用的 UUID。

### 5.5 meta 生成（参考 MeshConverter 的 `-a`）

- 未开启 `-a`：只写 `.ttex`，不写也不修改 `.ttex.meta`。此时没有导入设置记忆和增量判断。
- 开启 `-a`：写完 `.ttex` 后，`TextureExporter` 自己序列化一份完整的 `MetaTexture`（UUID、`SourceHash`、`ImportSettings`）覆盖写入 `.ttex.meta`，写法与 BuiltinGenerator 现有的 `archive->write` + `T3D_SERIALIZER_MGR.serialize` 一致。
- 不走 MetaFS（`ARCHIVE_TYPE_METAFS`）的原因：
  - MetaFS 只在 `.meta` 不存在时生成，已存在就不动。显式指定的 UUID 与旧 meta 不同时，会出现 `.ttex` 与 `.meta` 的 UUID 不一致。
  - MetaFS 生成的 `MetaTexture` 只含 UUID，导入设置还得再读改写一遍。
  - 少依赖一个插件。
- **meta 归属**：`imgconv` 只读写输出目录中的 `<name>.ttex.meta`，**从不读写源图片旁边的 meta**（例如 `xxx.png.meta`）。源图片侧的 meta 若存在，由拥有它的调用方（如将来的 TinyEditor）维护，调用方从中取出 UUID 与导入设置，转成 `--uuid`、`--type` 等参数传给 `imgconv`，并且不带 `-a`。此时 `imgconv` 完全不碰 meta：带 `--uuid` 不会去读旧 meta，不带 `-a` 不写 meta（`-u reuse` 会读、`-u new` 会删输出侧旧 meta，这种用法不应使用 `-u`）。
- 不带 `-a` 时，`imgconv` 不做增量判断（没有 `SourceHash` 可比），每次都转换；需要增量的调用方自己判断是否发请求。

### 5.6 写盘顺序与原子性

输出目录可能正被其他进程的文件监控（TinyEditor 的 MetaFS）观察，因此 `TextureExporter` 写盘必须保证观察方不会看到半写文件，也不会看到"有 `.ttex` 没有 meta"的中间状态：

1. `.ttex` 与 `.ttex.meta` 都先完整写到同目录下的 `.tmp` 文件并关闭。
2. 带 `-a` 时先把 `<name>.ttex.meta.tmp` rename 为 `<name>.ttex.meta`（覆盖旧文件）。
3. 再把 `<name>.ttex.tmp` rename 为 `<name>.ttex`（覆盖旧文件）。
4. 任何一步失败：删除残留的 `.tmp` 文件，`Result` 报失败。若第 2 步已成功而第 3 步失败，新 meta 与旧 `.ttex` 的 UUID 可能不一致，`Result.error_message` 中写明，由调用方决定重试。

- Platform 的 `Dir` 目前只有 `copy` / `remove`，没有 rename。需新增 `Dir::rename(const String &src, const String &dst, bool overwriteExisting)`：Windows 用 `MoveFileExW(..., MOVEFILE_REPLACE_EXISTING)`，Posix 用 `rename()`（本身即覆盖语义）。
- 临时文件扩展名固定为 `.tmp`，MetaFS 会将其排除（`MetaFS-Monitor-Fixes-todo.md` F4）。
- `imgconv` 启动时不清理目录中遗留的 `.tmp`（可能属于其他进程）；只清理自己本次产生的。

### 5.7 验收

- 分别转换一张半透明 png 和一张不透明 jpg。
- 检查 `.ttex` 的尺寸、mip 数量、`Buffer` 大小（等于各级 mip 字节数之和）、预乘后像素值。
- 转换完成后输出目录中没有残留 `.tmp` 文件；人为让 rename 失败（目标文件被占用），`.tmp` 被清理且结果为失败。
- 用文件监控工具观察输出目录：带 `-a` 时 `.ttex.meta` 的出现早于 `.ttex`。

---

## 6. 第 3 阶段：命令行模式（给人用）

### 6.1 命令行参数

以下参数同时也是 IPC `Import.args` 的格式（`--ipc-*` 除外）。

```text
imgconv -i <file|dir> -o <dstDir> [options]
  -r                 递归处理目录
  --type <default|ui|normal>
  --premultiply / --no-premultiply
  --mips             强制生成完整 mip 链（default / normal 类型默认已生成）
  --no-mips          强制不生成（ui 类型默认不生成）
  --max-size <N>
  --flip-y
  --address-u <wrap|clamp|mirror|border>
  --address-v <wrap|clamp|mirror|border>
  --name <title>     输出文件名（不含扩展名），仅单文件输入有效
  --uuid <hex>       显式指定纹理 UUID，仅单文件输入有效
  -u <reuse|new>     未指定 --uuid 时的 UUID 策略（同 MeshConverter，默认 reuse）
                       reuse - 复用已有 .ttex.meta 中的 UUID，没有则新生成
                       new   - 删除已有 .ttex.meta，新生成 UUID
  -a                 生成 .ttex.meta（同 MeshConverter）
  --exclude <name>   跳过指定子目录（可多次指定）
  --force            忽略 hash 强制重转
  --summary <file>   把处理汇总写成文本文件（给人看；IPC 请求中忽略）
  -t                 文本(JSON)输出，默认二进制
  -?                 帮助

imgconv --ipc-port <port> --ipc-token <token>     IPC 模式（给程序用，见 §7）
```

示例：

```text
imgconv -i ui/icons/close.png -o assets/ui/icons --type ui -a
imgconv -i textures/ -o assets/textures -r --type default -a
imgconv -i textures/lut.png -o assets/textures --no-mips -a -u new
imgconv -i ui/bg.png -o assets/ui --premultiply -a -t --uuid cd4df3aa9d1ea1133d2142e36d8c64a8
```

### 6.2 行为约定

- 批量处理时单个文件失败只记录错误并继续。
- 退出码：全部成功（含跳过）返回 0；有任意失败返回非 0。
- 控制台每个文件输出一行（源路径、结果、目标路径），结束时打印成功 / 跳过 / 失败数量。
- `--summary` 输出的文件只供人查看，格式不保证稳定，**程序不应解析它**。
- 参数解析失败时打印具体错误和帮助，返回非 0。

### 6.3 验收

- 单文件、目录、递归三种模式可用。
- 带 `-a` 第二次运行时未改动的文件显示为跳过。
- 带 `-a` 修改 `--type` 后会重新转换，且 UUID 不变（`reuse`）。
- `-u new` 生成新 UUID，`.ttex` 与 `.ttex.meta` 中的 UUID 一致。
- `--uuid` 指定的值与旧 meta 不同时，`.ttex` 与 `.ttex.meta` 都更新为指定值，并有警告。
- 不带 `-a` 时不产生、不修改 `.ttex.meta`。

---

## 7. 第 4 阶段：IPC 模式（给程序用）

### 7.1 角色与流程

- **调用方是服务端**：用 `TCPListener` 监听；**`imgconv` 是客户端**：用 `TCPConnection` 连回调用方。
- 选择这个方向的原因：调用方先监听、拿到端口后再启动子进程，不存在"子进程还没准备好"的竞争；子进程只需要知道端口号。

```text
调用方                                              imgconv
  │ 1. TCPListener.listen(0, "127.0.0.1")
  │    getListenPort() → port；生成随机 token
  │ 2. Process.start("imgconv --ipc-port <port> --ipc-token <token>")
  │                                                   │ 3. 初始化引擎、加载插件
  │ ◄──────────────── TCP 连接 ─────────────────────── │    TCPConnection.connect(127.0.0.1, port)
  │ ◄──────────── Hello(token, version) ───────────── │
  │ 4. 校验 token 与协议版本，不通过则断开
  │ ───────────── Import(seq=1, args) ──────────────► │    ImageConverterCommand::parse(args)
  │ ◄──────────── Progress / Log(seq=1) ───────────── │    ImageImporter → TextureExporter
  │ ◄──────────── Result(seq=1, uuid, ...) ────────── │
  │ ◄──────────── Done(seq=1, 1/0/0) ──────────────── │
  │ ───────────── Import(seq=2, args) ──────────────► │
  │ ◄──────────── Result(seq=2, ...) ──────────────── │
  │ ◄──────────── Done(seq=2, ...) ────────────────── │
  │ ───────────── Shutdown ─────────────────────────► │
  │                                                   │ 5. 发完缓冲区后断开、退出
  │ 6. Process.wait()，检查退出码
```

### 7.2 复用 `TCPListener` / `TCPConnection` 需要补的地方

| 项目 | Launcher 现状 | `imgconv` 场景的做法 |
| --- | --- | --- |
| 端口 | 固定 5327（全局只有一个 Launcher） | 调用方可能同时有多个（编辑器 + 命令行跑 MeshConverter），**用端口 0 让系统分配**。给 `TCPListener` 新增 `getListenPort()`，内部用已有的 `Socket::getLocalName` 读取实际端口 |
| 监听地址 | 传 `""`，监听所有网卡 | **只监听 `127.0.0.1`**：不触发 Windows 防火墙弹窗，外部机器也连不进来 |
| 身份校验 | 无 | 调用方生成随机 token（如 128 位十六进制），通过命令行传给 `imgconv`；`imgconv` 在 `Hello` 中带回，不匹配则断开，防止本机其他程序误连 |
| 驱动方式 | Launcher / Editor 在主循环里 `poll()` | TinyEditor 照旧在主循环 `poll()`；MeshConverter、BuiltinGenerator 是同步工具，由客户端辅助类内部循环 `poll()` 等待（见 §7.4） |
| 退出 | 长驻 | `imgconv` 收到 `Shutdown` 后继续 `poll()` 直到发送缓冲区清空，再断开、退出，避免最后的 `Result` 丢失 |
| 单包大小 | 包头长度字段为 `uint16_t`，单包约 64KB 上限 | 协议只传参数和元数据（路径、UUID、错误信息），不传像素数据，不会超限 |

### 7.3 消息协议（protobuf）

新增 `tools/protobuf/proto/tools/ImageConverter.proto`，生成方式参照 `generate_editor_proto.bat`（新增对应的生成脚本）。TCP 包头中的 `seq` 用作请求编号：`Progress` / `Log` / `Result` / `Done` 使用与对应 `Import` 相同的 `seq`，与 TinyEditor ↔ TinyLauncher 的约定一致。

**请求只传参数数组，不定义导入设置的结构和枚举**；结果仍是结构化字段。

```protobuf
syntax = "proto3";
package Tiny3D.ImageConverter;

enum MessageID
{
    MSGID_NONE     = 0;
    MSGID_HELLO    = 1;   // imgconv → 调用方
    MSGID_IMPORT   = 2;   // 调用方 → imgconv
    MSGID_PROGRESS = 3;   // imgconv → 调用方
    MSGID_LOG      = 4;   // imgconv → 调用方
    MSGID_RESULT   = 5;   // imgconv → 调用方，每个文件一条
    MSGID_DONE     = 6;   // imgconv → 调用方，一个 Import 处理结束
    MSGID_SHUTDOWN = 7;   // 调用方 → imgconv
}

enum ImportStatus  { IS_SUCCEEDED = 0; IS_SKIPPED = 1; IS_FAILED = 2; }
enum LogLevel      { LL_INFO = 0; LL_WARNING = 1; LL_ERROR = 2; }

message Hello      { string token = 1; uint32 version = 2; uint64 pid = 3; }

// 与命令行完全相同的参数（不含 --ipc-*），逐个传递，不拼接成一条字符串
message Import     { repeated string args = 1; }

message Progress   { string stage = 1; uint32 percent = 2; }
message Log        { LogLevel level = 1; string message = 2; }
message Result
{
    ImportStatus status              = 1;
    string       source              = 2;
    string       target              = 3;
    string       uuid                = 4;
    uint32       width               = 5;
    uint32       height              = 6;
    string       format              = 7;
    uint32       mipmaps             = 8;
    bool         premultiplied_alpha = 9;
    int32        error_code          = 10;
    string       error_message       = 11;
    string       previous_uuid       = 12;   // 被替换或删除的旧 .ttex.meta 中的 UUID；UUID 未变或原先无 meta 时为空
}
message Done       { uint32 succeeded = 1; uint32 skipped = 2; uint32 failed = 3; }
message Shutdown   {}

// 调用方 → imgconv
message RequestBody
{
    MessageID message_id = 1;
    oneof body { Import import = 2; Shutdown shutdown = 3; }
}

// imgconv → 调用方
message ResponseBody
{
    MessageID message_id = 1;
    oneof body { Hello hello = 2; Progress progress = 3; Log log = 4; Result result = 5; Done done = 6; }
}
```

- 每个 `Import` 都以一条 `Done` 结束。调用方以 `Done` 判断请求完成，而不是以 `Result` 数量判断。
- 单文件请求：一条 `Result` + 一条 `Done`。目录请求（`-i <dir> -r`）：每个文件一条 `Result`，最后一条 `Done`。参数解析失败：一条 `IS_FAILED` 的 `Result`（`error_message` 写明哪个参数有问题）+ 一条 `Done`。
- MeshConverter、BuiltinGenerator 需要给每个文件单独指定 `--uuid`，实际仍逐个文件发请求；目录请求主要给不关心 UUID 的调用方使用。
- UUID 与 meta 的处理规则与命令行完全一致（§5.4、§5.5），因为用的就是同一组参数。
- `imgconv` 在日志中打印每个请求的完整参数，便于复制到命令行复现。
- `Hello.version` 为协议版本号，调用方发现不匹配直接报错并终止子进程。参数格式的变化由命令行兼容性保证，不需要改协议版本。

### 7.4 IPC 公共代码

放在 `source/Tools/Common/ImgconvIPC/`，编成一个静态库，`imgconv`、MeshConverter、BuiltinGenerator、TinyEditor 都链接它，避免像现在 `NetPackage.pb.*` 那样在多个工程里各拷贝一份。

```text
source/Tools/Common/ImgconvIPC/
├── CMakeLists.txt
├── proto/
│   ├── ImageConverter.pb.h            // 生成代码，只放这一份
│   └── ImageConverter.pb.cc
├── Include/
│   ├── T3DImgconvArgs.h               // 参数构造辅助类
│   └── T3DImgconvClient.h             // 调用方辅助类
└── Source/
    ├── T3DImgconvArgs.cpp
    └── T3DImgconvClient.cpp
```

参数构造辅助类草案（减少手拼参数出错；生成的就是命令行参数数组）：

```cpp
class ImgconvArgs
{
public:
    ImgconvArgs &input(const String &path);          // -i
    ImgconvArgs &output(const String &dir);          // -o
    ImgconvArgs &name(const String &title);          // --name
    ImgconvArgs &uuid(const UUID &uuid);             // --uuid
    ImgconvArgs &uuidPolicyNew();                    // -u new
    ImgconvArgs &generateMeta();                     // -a
    ImgconvArgs &type(const String &type);           // --type default|ui|normal
    ImgconvArgs &mips(bool enable);                  // --mips / --no-mips
    ImgconvArgs &flipY();                            // --flip-y
    ImgconvArgs &addressU(const String &mode);       // --address-u
    ImgconvArgs &addressV(const String &mode);       // --address-v
    ImgconvArgs &textMode();                         // -t
    ImgconvArgs &force();                            // --force
    ImgconvArgs &raw(const String &arg);             // 追加辅助类尚未覆盖的参数

    const TArray<String> &args() const;
    String toCommandLine() const;                    // 用于日志与复现
};
```

调用方辅助类草案：

```cpp
class ImgconvClient
{
public:
    /// 监听 127.0.0.1:0，生成 token，启动 imgconv，等待 Hello 并校验
    TResult start(const String &imgconvPath, uint32_t connectTimeoutMS = 10 * 1000);

    /// 同步：发送 Import(args)，内部循环 poll() 直到收到同 seq 的 Done、超时或断开
    TResult importSync(const ImgconvArgs &args, TArray<ImageConverter::Result> &results,
                       ImageConverter::Done &done, uint32_t timeoutMS = 60 * 1000);

    /// 异步：供 TinyEditor 在主循环中使用，结果通过回调返回
    TResult importAsync(const ImgconvArgs &args, const OnImportResult &onResult,
                        const OnImportDone &onDone, const OnImportProgress &onProgress = nullptr);

    /// 由有主循环的调用方每帧调用
    void poll();

    /// 发送 Shutdown，等待断开与进程退出，返回退出码
    TResult stop(uint32_t &exitCode, uint32_t timeoutMS = 10 * 1000);
};
```

### 7.5 异常处理

| 情况 | 处理 |
| --- | --- |
| 超时未连上 | 检查子进程是否已退出：已退出则按启动失败处理并取退出码；未退出则 `terminate` 后报错 |
| `Hello` 的 token 或版本不匹配 | 断开连接、`terminate` 子进程、报错 |
| 请求参数解析失败 | `imgconv` 回 `IS_FAILED` 的 `Result`（写明错误参数）和 `Done`，连接保持，可继续后续请求 |
| 等待 `Done` 时连接断开 | 判定 `imgconv` 崩溃，当前请求按失败处理；调用方可选择重新 `start` 后继续剩余请求 |
| 单个请求超时 | 按失败处理，`terminate` 子进程；超时时间可配置，大图需放宽 |
| 调用方先断开（崩溃或退出） | `imgconv` 收到断开回调后立即退出，避免残留孤儿进程 |
| `imgconv` 在 IPC 模式下的退出码 | 正常 `Shutdown` 返回 0；token 校验失败、连接失败、协议错误返回非 0 |

### 7.6 验收

- 同时运行两个调用方（例如两个 MeshConverter），各自分配到不同端口，互不干扰。
- 一个 `imgconv` 进程连续处理多个 `Import`，`Result` / `Done` 的 `seq` 与请求对应。
- 同一组参数分别用命令行和 IPC 执行，产出的 `.ttex` / `.ttex.meta` 完全一致。
- 请求中带拼错的参数时，收到写明错误参数的失败 `Result`，`imgconv` 不退出。
- 目录请求返回多条 `Result` 和一条 `Done`，计数正确。
- 人为让 `imgconv` 崩溃（例如传入损坏图片并在调试版本中断言），调用方能检测到并报告失败。
- 杀掉调用方，`imgconv` 自动退出。
- 监听只绑定 `127.0.0.1`，Windows 上不出现防火墙弹窗。

---

## 8. 第 5 阶段：BuiltinGenerator 改为通过 IPC 调用 `imgconv`

- `BuiltinTextures::generateTexture` 保留现有的目录扫描（跳过 `cubemaps` 子目录、跳过 `.ttex` / `.meta`），但每个图片文件不再自己 `loadImage` + `createTexture2D`，而是通过 `ImgconvClient::importSync` 发送 `Import`。
- 整个生成过程只 `start` 一次 `imgconv`，全部发完后 `stop`。
- UUID 仍由 BuiltinGenerator 决定：继续用 `BuiltinGuidUtil::readExistingMetaUUID` 读旧 meta，读不到则 `UUID::generate()`，作为 `--uuid` 传入。
- 请求参数带 `-a`，`.ttex.meta` 由 `imgconv` 写；BuiltinGenerator 不再为这些纹理自己写 `MetaTexture`。
- 每个文件的请求参数形如：`-i <file> -o <textures> --name <title> --uuid <uuid> -a`。
- `imgconv` 路径：与 BuiltinGenerator 位于同一输出目录，按可执行文件所在目录拼接。
- CMake：链接 `ImgconvIPC`；`add_dependencies(BuiltinGenerator ImageConverter)`，保证先构建 `imgconv`。
- 以下部分仍在 BuiltinGenerator 进程内用 Core 生成，**不变**：
  - 程序生成纹理：`white`、`Test`、`skybox_default`。
  - cubemap（6 面图拼装）。
  - 内置材质、网格、shader。

  因此 BuiltinGenerator 仍然链接 `T3DCoreEditor`；只是图片转换这一块不再自己实现。

**验收**：重新运行 BuiltinGenerator，产出的 `.ttex` 与 UUID 与之前一致，samples 正常运行。

---

## 9. 第 6 阶段：MeshConverter 改为通过 IPC 调用 `imgconv`

### 9.1 改动

- 导入开始前 `ImgconvClient::start` 一次，导出结束后 `stop`。
- `FBXImporterNew::createTexture` 不再 `loadImage` + `createTexture2D`，改为对每个 FBX 外部图片：
  1. 按现有逻辑解析图片路径（FBX 记录路径不存在时回退到输入目录）。
  2. 按 MeshConverter 自己的 `-u` 策略确定纹理 UUID：`lookupReusableUUID(title, EXT_TEXTURE)`，返回 `UUID::INVALID`（`new` 策略或没有旧 meta）时用 `UUID::generate()` 新生成。先把这个 UUID 写进材质（`material->setTexture("_MainTex", uuid)`）。
  3. 用 `ImgconvArgs` 组装请求参数，对应关系：

     | 现有逻辑 | `imgconv` 参数 |
     | --- | --- |
     | 图片路径 | `-i <path>` |
     | 输出目录 `opts.dstDir` | `-o <dstDir>` |
     | 资源名 `title` | `--name <title>` |
     | 第 2 步确定的纹理 UUID | `--uuid <uuid>` |
     | `opts.uuidPolicy == kNew`（`-u new`） | `-u new`（`imgconv` 会删除旧 `.ttex.meta`） |
     | `opts.isGeneratingMeta`（`-a`） | `-a` |
     | `mIsOpenGLMetric` 时 `image->flip()` | `--flip-y` |
     | `GetWrapModeU/V()`（eRepeat / eClamp） | `--address-u wrap\|clamp` / `--address-v wrap\|clamp` |
     | `opts.isTxt`（`-t`） | `-t` |
     | MeshConverter 新增 `--tex-no-mips` | `--no-mips`（不传则按 `Default` 类型默认生成 mip） |

     生成的参数示例：

     ```text
     -i D:/models/tortoise.png -o assets/meshes --name tortoise --uuid cd4df3aa9d1ea1133d2142e36d8c64a8 -a --flip-y --address-u wrap --address-v clamp
     ```

  4. `importSync` 等到 `Done` 后检查 `Result` 状态；`Result.uuid` 应与第 2 步的 UUID 相同，不同则报错。
- 由于 UUID 在发请求前就已确定，材质不需要等 `Result` 回来才能填纹理引用；以后改为多张纹理并行转换也更容易。
- 同一张图片被多个材质引用时只请求一次：按源文件绝对路径缓存"路径 → UUID"。
- 纹理不再放进 `mResources`，`EngineExporter::exportTexture` 不再参与纹理导出（保留代码或删除，实施时决定）。纹理的 `.ttex.meta` 由 `imgconv` 写，`EngineExporter::removeStaleMeta` 不再处理纹理。
- CMake：链接 `ImgconvIPC`；`add_dependencies(MeshConverter ImageConverter)`。

### 9.2 需要注意

- MeshConverter 在 `-a` 模式下用 `ARCHIVE_TYPE_METAFS` 写自己的资源，它进程内的 MetaFS 也会监控输出目录。`imgconv` 按 §5.6 先 rename meta 再 rename `.ttex`，MeshConverter 的 MetaFS 看到 `.ttex` 时 meta 已存在，不会再生成一份；实施时验证。MeshConverter 自己写 `.tmesh` / `.tmat` 是否也要改成"临时文件再 rename"，放到 TinyEditor / meshconv 集成文档中一起分析。
- 单张纹理失败时 MeshConverter 记录警告并跳过这张纹理（与现在 `createTexture` 失败时 `continue` 的行为一致），不中断整个模型导出；`imgconv` 崩溃时可重新 `start` 后继续。

**验收**：

- 重新转换 samples 里的模型（如 tortoise），`.ttex` 与 UUID 与之前一致，材质正确引用纹理，画面与之前一致。
- `-u new` 重新转换时纹理拿到新 UUID，材质引用的是新 UUID，`.ttex` 与 `.ttex.meta` 一致。
- OpenGL 制式模型的纹理方向正确。
- 整个模型转换过程中只启动一次 `imgconv`。

---

## 10. 第 7 阶段：RmlUi 运行时适配

- `RmlRenderInterface::loadEngineTexture` 读取 `Texture::isPremultipliedAlpha()`，记录到纹理 handle 对应的信息中。
- 未预乘纹理：绘制时通过 constant buffer 传标记，像素 shader 中执行 `rgb *= a`，并输出警告，提示应按 `UI` 类型导入。
- 修正 `LoadTexture` 中"带压缩和 mip"的注释，使其与实际一致。
- 非 `.ttex` 回退分支：迁移完成前保留并输出警告；迁移完成后删除。

**验收**：

- 按 `UI` 类型导入的半透明图标边缘无白边。
- 故意按 `Default` 类型导入的图片画面依然正确，日志中有警告。

---

## 11. 第 8 阶段：资源迁移

1. 用 `imgconv` 命令行模式批量转换 UI 目录下的 png / jpg。
2. RML / RCSS 中的图片引用全部改为 `.ttex`。
3. 验证 Android 打包（bundle）与加载。
4. 删除 RmlUi 非 `.ttex` 回退分支。

---

## 12. 第 9 阶段：TinyEditor 集成（另行设计）

TinyEditor 如何使用 `imgconv`（源图片与产物的存放位置、源图片 meta 的格式、何时触发导入、热重载、进度显示等）将与 meshconv 在编辑器中的使用一起重新分析现状，写入单独的集成文档，本文不展开。

本文对集成文档的承诺只限于 `imgconv` 本身已提供的能力：

- IPC 模式下一个常驻进程可连续处理请求，`importAsync` + `poll()` 适合放进编辑器主循环；`Progress` / `Log` 可用于进度条和日志面板。
- 带 `--uuid` 且不带 `-a` 时 `imgconv` 不读写任何 meta，导入设置完全由参数给出，适合"meta 由编辑器独占"的用法（§5.5）。
- 写盘遵守 §5.6 的原子顺序；`Result.target`、`Result.uuid`、`Result.previous_uuid` 足以让调用方调用 MetaFS 的 `refresh` 并修正引用（`MetaFS-Monitor-Fixes-todo.md` F2）。

---

## 13. 第 10 阶段：后续扩展

- **压缩格式**：`PixelFormat` 增加 BC / ASTC / ETC，`imgconv` 接入编码库（如 bc7enc、astc-encoder），各渲染后端补映射，支持按平台选择格式。
- **sRGB 格式**：增加 sRGB 像素格式，使 `sRGB` 设置生效。
- **法线贴图**：`NormalMap` 类型的专用处理。
- **cubemap / 纹理数组导入**：`imgconv` 支持 6 面图输入（协议增加对应请求），BuiltinGenerator 的 cubemap 逻辑也改为调用它。
- **FBX 内嵌纹理**：MeshConverter 先把内嵌图片导出到临时文件，再发 `Import`。
- **反向导出 `ImageExporter`**：`.ttex` → png / jpg，用于调试查看、外部编辑后回导。需要配套的 `EngineImporter`（`.ttex` → `Texture` 对象，对应 MeshConverter 的 `EngineImporter`）。届时再考虑为 Importer / Exporter 抽基类。

---

## 14. 风险

- **其他渲染后端的 mip 布局**：除 D3D11 外，GL / Vulkan 等后端上传纹理时也必须按"mip 紧密拼接"读取 `Buffer`，需逐个确认。`Default` 类型默认生成 mip 后，几乎所有 3D 纹理都会走多级 mip 上传路径，这一项需在第 2 阶段验收前完成。
- **二进制格式兼容**：`Texture` 新增字段后旧二进制 `.ttex` 能否读取，需在第 1 阶段测清楚。
- **反射预处理**：新增结构体与属性后必须重跑 ReflectionPreprocessor。
- **子进程路径与依赖**：调用方必须能找到 `imgconv` 以及它依赖的插件 dll；部署时 `imgconv` 要和插件放在同一目录。
- **路径编码**：`Import` 中的路径统一用 UTF-8；`imgconv` 内部打开文件时按平台转换编码。命令行模式下含空格或中文的路径需正确加引号（`Process::start` 在 Windows 上走 `CreateProcess`）。
- **端口分配**：`TCPListener` 需新增 `getListenPort()`；确认 `bind(0)` 在 Windows / Posix 实现下都能正常分配端口。
- **连接与请求超时**：`imgconv` 启动需要初始化引擎和加载插件，连接超时不能设得太短；大图转换耗时长，请求超时需可配置。
- **退出前数据未发完**：`imgconv` 必须在发送缓冲区清空后再退出，否则调用方收不到最后的 `Result`。
- **同步工具的轮询**：`ImgconvClient::importSync` 内部循环 `poll()` 时需要适当 sleep，避免空转占满 CPU。
- **UUID 不一致**：显式指定的 UUID 与旧 meta 不同、或调用方与 `imgconv` 对 UUID 理解不一致时，会导致引用失效。`imgconv` 以显式 UUID 为准覆盖 meta 并警告；调用方用 `Result.uuid` 校验。
- **参数拼错只能运行时发现**：IPC 请求改为参数数组后，失去了 protobuf 字段的编译期检查。缓解：调用方统一用 `ImgconvArgs` 构造参数；`imgconv` 解析失败时回写明错误参数的 `Result`；调用方把 `IS_FAILED` 当作错误处理而不是静默跳过。
- **输出目录被文件监控**：TinyEditor 的 MetaFS 会观察 `imgconv` 写入的目录。`imgconv` 侧靠 §5.6 的原子写入顺序规避半写文件和"有资源无 meta"；MetaFS 侧的配套修复（meta 已存在时登记、`refresh`、排除 `.tmp`、不误删 meta）见 `MetaFS-Monitor-Fixes-todo.md`，两边都落地后编辑器运行期间调用 `imgconv` 才安全。
- **rename 覆盖语义**：Platform `Dir` 需新增覆盖式 `rename`（§5.6）；目标文件被其他进程占用（如编辑器正在读）时 rename 会失败，需要明确的失败结果和可重试。
- **不带 `-a` 的产物**：没有 `.ttex.meta` 时，下次 `reuse` 读不到 UUID 会新生成，引用会断。需要稳定 UUID 的调用方要么带 `-a`，要么显式传 `--uuid`。

---

## 15. 进度跟踪

- [x] 第 1 阶段：`TextureImportSettings` / `MetaTexture` / `Texture` 数据字段，反射与兼容验证
- [x] 第 2 阶段：ImageConverter 工程、共用参数解析 `ImageConverterCommand::parse`、`ImageConverterApp` 编排（UUID 规则 `--uuid` / `-u`、设置合并、增量）、`ImageImporter`（解码、预乘、mip 生成）、`TextureExporter`（新增 `Dir::rename`、`.tmp` + rename 原子写入、先 meta 后 `.ttex`、`-a` 写 meta、`-u new` 处理旧 meta 并回报旧 UUID）
- [x] 第 3 阶段：命令行模式（参数、退出码、控制台输出、`--summary`）
- [x] 第 4 阶段：IPC 模式（`TCPListener::getListenPort`、`ImageConverter.proto`（`Import.args` + `Result` / `Done`）、`ImgconvIPC` 公共库（`ImgconvArgs` / `ImgconvClient`）、`imgconv` IPC 服务端复用参数解析、异常处理）
- [x] 第 5 阶段：BuiltinGenerator 通过 IPC 调用 `imgconv`
- [x] 第 6 阶段：MeshConverter 通过 IPC 调用 `imgconv`
- [x] 第 7 阶段：RmlUi 预乘兜底与注释修正
- [x] 第 8 阶段：UI 资源迁移，删除回退分支
- [ ] 第 9 阶段：TinyEditor 集成（另见 TinyEditor / meshconv 集成文档，待写；依赖 `MetaFS-Monitor-Fixes-todo.md`）
- [ ] 第 10 阶段：压缩 / sRGB / 法线贴图 / cubemap / FBX 内嵌纹理
