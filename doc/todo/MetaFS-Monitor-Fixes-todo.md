# MetaFS 监控与外部写入协同修改计划

> 目标：让 MetaFS（`source/Plugins/Archive/MetaFileSystem`）在**其他进程往被监控目录里写资源和 meta** 时仍能保持"UUID → 路径"映射表正确，不读半写文件、不崩溃、不误删 meta。
>
> 触发背景：`imgconv`（见 `ImageConverter-Texture-Import-Design-todo.md`）、`meshconv`、`scc` 都会在 TinyEditor 运行期间往磁盘写资源和 `.meta`，而 TinyEditor 用 MetaFS 监控工程目录。本文只覆盖 MetaFS 自身的修改；TinyEditor 如何组织导入流程（源文件与产物的存放、何时调用工具）另见后续的 TinyEditor / meshconv 集成文档。
>
> 本文档为施工蓝图，接口与代码片段均为「建议实现」，不代表已落地。

---

## 1. 现状

### 1.1 结构

- `MetaFSMonitorManager`：按根目录管理多个 `MetaFSMonitor`；`getPathAndMonitor(uuid, ...)` 先查主 monitor，再全局查；遇到 `MetaShaderLab` 时跳转到编译后 shader 的 UUID。
- `MetaFSMonitor`：
  - `init()` 递归扫描根目录：meta 存在则读取，不存在则生成，然后登记映射表。
  - `start()` 启动 Platform 的 `FileSystemMonitor`，排除扩展名 `meta`。
  - 映射表：`mPathMappings`（相对路径 → `MetaPtr`）、`mUUIDMappings`（UUID → 相对路径）。
- `MetaFSArchive::write`：写资源成功后调用 `MetaFSMonitor::generateMetaFile` 生成 meta。
- 文件事件由 Win32 `ReadDirectoryChangesW` 在后台线程收集进队列，**主线程 `FileSystemMonitor::poll()` 时派发**，因此回调不存在多线程竞争，但有一帧左右的延迟。

### 1.2 事件处理

| 事件 | 当前处理 |
| --- | --- |
| `kAdded` | 目录：没有 meta 时生成 `MetaFolder`。文件：**meta 不存在时**加载资源取 UUID（引擎资源类型）或随机 UUID（其他类型），写 meta 并登记映射表；**meta 已存在时什么都不做** |
| `kRemoved` | 删除 `<path>.meta`，从映射表移除 |
| `kModifed` | 忽略 |
| `kRenamedOld` | 同 `kRemoved` |
| `kRenamedNew` | 同 `kAdded` |
| `.meta` 自身的任何变化 | 被排除，不产生事件 |

引擎资源类型（`.ttex`、`.tmesh`、`.tmat` 等）的 meta UUID 来自资源文件内部，所以无论谁写 meta，UUID 都一致。问题集中在**时序、映射表登记和内容完整度**上。

---

## 2. 问题清单

### P1. meta 已存在时不登记映射表（严重）

`T3DMetaFSMonitor.cpp`：

```cpp
    void MetaFSMonitor::onFileAdded(const String &path)
    {
        // ...
            String metaPath = path + ".meta";
            if (!Dir::exists(metaPath))
            {
                meta = generateFileMeta(metaPath, dir, title, ext);
                if (meta != nullptr)
                {
                    addPathLUT(path, meta);
                }
                // ...
            }
        // ...
    }
```

外部工具（`imgconv -a`、`meshconv -a`）写完资源后紧接着写 meta。编辑器下一帧处理 `kAdded` 时 meta 已经存在，于是直接跳过，**新资源的 UUID 不在映射表里**，按 UUID 加载失败，直到重启编辑器重新扫描。`generateMetaFile` 也有同样的逻辑。

### P2. 资源被覆盖或 UUID 改变后映射表不更新

- 覆盖已有资源只产生 `kModifed`，被忽略。
- 外部工具改变 UUID（`-u new`、显式 `--uuid`）时会删除或重写 `.meta`，但 `.meta` 不在监控范围内，没有事件。
- 结果：映射表里仍是旧 UUID，新 UUID 查不到。

### P3. 读到半写文件，且加载失败时崩溃

文件一创建就产生 `kAdded`，写入可能尚未完成。`generateFileMeta` 对引擎资源调用 `loadTexture` / `loadMesh` 等，读到不完整文件会失败，而后续直接 `texture->getUUID()`，没有判空，编辑器崩溃。大文件（带完整 mip 链的纹理、大网格）更容易出现。

### P4. 临时文件会被当作资源

外部工具若采用"写临时文件再 rename"的方式保证原子性，临时文件本身会触发 `kAdded`，MetaFS 为它生成一个 `MetaFile` meta；rename 后再触发 `kRenamedOld` 删除。不致命，但产生多余的磁盘写入和映射表抖动。

### P5. 覆盖式 rename 可能误删 meta

`MoveFileEx(..., MOVEFILE_REPLACE_EXISTING)` 覆盖已有文件时，目标文件可能先报 `FILE_ACTION_REMOVED`（具体事件序列需实测）。`onFileRemoved` 会**无条件删除 `<path>.meta`**，可能把外部工具刚写好的 meta 删掉。

### P6. 映射表增删逻辑有 bug

- `removePathLUT` 用 `path.find_first_not_of(mRootPath)` 求相对路径。`find_first_not_of` 把参数当作**字符集合**，得到的位置是 path 中第一个不在根路径字符集合里的字符，并不是前缀长度。而 `addPathLUT` 用的是 `mRootPath.length() + 1`。两者算出的相对路径不一致，删除时大概率找不到条目，映射表残留已删除的文件。
- `addPathLUT` 用 `emplace`，key 已存在时**不会覆盖**。同一路径重新登记（UUID 变了）时新值写不进去，旧 UUID 也不会被移除。

### P7. 排除目录参数传错

`source/Platform/Source/IO/T3DFileSystemMonitor.cpp`：

```cpp
    TResult FileSystemMonitor::startMonitor(const String &path, const FSMonitorExts &excludeExts, const FSMonitorExcludes &excludeFolders, const FSMonitorOnChanged &onChanged)
    {
        // ...
            ret = mMonitor->startWatching(path, excludeExts, excludeExts, onChanged);
```

`excludeFolders` 没有传下去，第二个参数重复传了 `excludeExts`。目前 MetaFS 没有排除目录，所以暂无实际影响，但以后需要排除目录时会出问题。

### P8. 复制资源文件导致 UUID 重复（记录，本期不处理）

引擎资源的 UUID 存在文件内部，复制一份 `.ttex` 会得到两个 UUID 相同的资源。`addPathLUT` 用 `emplace` 静默忽略后来者。Unity 的做法是检测到重复时给新文件重新分配 GUID；我们需要改写资源文件内部的 UUID，代价较大，本期只在登记时检测并输出警告。

---

## 3. 修改方案

### F1. `onFileAdded` 在 meta 已存在时读取并登记（解决 P1）

已有的单参数 `generateFileMeta(const String &path)` 正是"meta 存在则读、不存在则生成，然后登记"的逻辑（`init()` 扫描用的就是它）。`onFileAdded` 的文件分支改为直接调用它即可。`generateMetaFile` 同理：meta 已存在时读取并登记，而不是跳过。

### F2. 新增 `refresh(path)` 并处理 `kModifed`（解决 P2）

```cpp
class MetaFSMonitor
{
public:
    /// 重新读取 path 对应的 .meta，更新映射表：
    /// 移除该路径的旧 UUID 映射，登记新 UUID；meta 不存在时按 kAdded 处理
    TResult refresh(const String &path);
};

class MetaFSMonitorManager
{
public:
    /// 按绝对路径找到所属 monitor 后调用其 refresh
    TResult refresh(const String &absPath);
};
```

- `kModifed`：对文件调用 `refresh`。只读 meta，不加载资源，开销很小；一次写入可能产生多次 `kModifed`，重复调用无副作用。
- 外部工具改了 UUID 却只动了 `.meta`（没有资源文件事件）时，由调用方在拿到工具结果后**显式**调用 `T3D_MFS_MONITOR_MGR.refresh(path)`。这是 MetaFS 对外承诺的同步接口，不依赖事件。

### F3. 加载失败时判空并延迟重试（解决 P3）

- `generateFileMeta` 中所有 `loadXxx` 之后判空；失败时**不写 meta、不登记**，把路径放进 `mPendingPaths`。
- 文件写完后系统会继续产生 `kModifed`，此时对处在 `mPendingPaths` 中的路径重新走 F1 的流程；成功后移出。
- `mPendingPaths` 中的条目在 `refresh` 时同样会重试。长时间失败（例如文件本身就是坏的）只输出一次错误日志，不反复刷屏。

### F4. 约定临时文件扩展名并排除（解决 P4）

- 约定：外部工具写资源和 meta 时，先写 `<目标文件名>.tmp`，写完后 rename 为目标名。
- MetaFS 的 `excludeExts` 增加 `tmp`；`init()` 扫描时跳过 `.tmp` 文件。
- 写入方需要遵守的顺序见 §4。

### F5. `onFileRemoved` 处理时先确认文件确实不存在（解决 P5）

事件是延迟到主线程派发的，派发时覆盖式 rename 早已完成。`onFileRemoved` 先 `Dir::exists(path)`：

- 文件已不存在：按原逻辑删除 meta、移除映射。
- 文件仍然存在：说明是被替换而非删除，改为调用 `refresh(path)`，**不删除 meta**。

`kRenamedOld` 也走同样的检查。

### F6. 修正映射表增删（解决 P6，兼做 P8 检测）

- 抽出 `toRelativePath(path)`，`addPathLUT` / `removePathLUT` / `refresh` 统一使用：以 `mRootPath + 分隔符` 作前缀匹配，匹配则去掉前缀，否则原样返回。
- `addPathLUT`：
  1. 该路径已有条目且 UUID 不同：先从 `mUUIDMappings` 删除旧 UUID。
  2. 新 UUID 已被**另一路径**占用：输出重复 UUID 警告（P8），保留先登记的那个。
  3. 用 `insert_or_assign` / `operator[]` 写入，保证覆盖。

### F7. 修正 `startMonitor` 参数（解决 P7）

`startWatching(path, excludeExts, excludeFolders, onChanged)`。

---

## 4. 外部写入方约定

所有可能往 MetaFS 监控目录写资源的外部工具（`imgconv`、`meshconv`、`scc`）都应遵守：

1. **原子写入**：资源文件和 meta 都先写 `<name>.tmp`，完整写完、关闭后再 rename 为目标名（同目录同卷，rename 是原子的）。
2. **先 meta 后资源**：两个临时文件都写好后，先 rename meta，再 rename 资源文件。这样监控方看到资源文件出现时，meta 一定已经就位，F1 直接读取登记，不会走"加载资源生成 meta"的路径。
3. **结果回报**：通过工具自己的结果通道（`imgconv` 的 IPC `Result`）回报目标路径和最终 UUID；UUID 发生变化时一并回报旧 UUID，便于调用方调用 `refresh` 并修正引用。
4. 不带 meta 输出（例如 `imgconv` 不带 `-a`）时只做第 1 条。

MetaFS 自身（`MetaFSArchive::write`）是进程内写入，不受此约定约束，但 F1 / F6 的修正同样适用于它。

---

## 5. 验收

- 编辑器运行中，用命令行 `imgconv -a` 往工程目录写一张纹理：不重启编辑器即可按 UUID 加载。
- 同一张纹理用 `-u new` 重新导入：编辑器中按新 UUID 能加载，按旧 UUID 查不到。
- 往工程目录拷入一个几十 MB 的 `.ttex`（拷贝过程中不完整）：编辑器不崩溃，拷贝完成后映射表正确。
- 外部工具 rename 覆盖已有 `.ttex`：`.ttex.meta` 不被删除。
- 删除一个资源文件后，`getPath(uuid)` 返回空（验证 P6 修复）。
- 拷贝一份 `.ttex`（UUID 重复）：日志出现重复 UUID 警告，先登记的文件仍可正常加载。

---

## 6. 风险

- **Windows rename 事件序列**：覆盖式 rename、跨目录移动时 `ReadDirectoryChangesW` 报出的事件顺序需要实测，F5 的"派发时再确认文件是否存在"是为了不依赖具体序列。
- **其他平台监控实现**：`T3DLinuxFSMonitor`、`T3DOSXFSMonitor`、`T3DAndroidFSMonitor` 的事件语义（尤其是 rename 和修改）需逐个确认；编辑器目前只在 Windows 上运行，可后置。
- **编辑器内已加载资源不刷新**：F2 只更新映射表，不会让已加载到内存的 `Texture` 重新加载。资源热重载属于 TinyEditor 集成范畴，另行设计。
- **`refresh` 与事件重复**：显式 `refresh` 与随后到来的 `kModifed` 会重复处理同一路径，要求 `refresh` 幂等。

---

## 7. 后续（依赖 TinyEditor / meshconv 集成文档）

TinyEditor 计划参照 ShaderLab 的方式管理图片：源图片放在 `Assets`，其 meta 记录资源 UUID 与导入设置，`imgconv` 产物输出到 `Temp` 目录，按 UUID 查找时跳转到产物。届时 MetaFS 还需要：

- 识别图片扩展名，生成对应的源图片 meta 类型（而不是通用的 `MetaFile`）。
- `getPathAndMonitor` 对该 meta 类型做跳转，类似现有的 `MetaShaderLab` 处理。

具体设计待集成文档确定后补充到本文。

---

## 8. 进度跟踪

- [x] F6：`toRelativePath`，修正 `removePathLUT`，`addPathLUT` 覆盖与重复 UUID 检测
- [x] F1：`onFileAdded` / `generateMetaFile` 在 meta 已存在时读取并登记
- [x] F3：加载失败判空、`mPendingPaths` 延迟重试
- [x] F2：`MetaFSMonitor::refresh` / `MetaFSMonitorManager::refresh`，处理 `kModifed`
- [x] F4：排除 `.tmp`
- [x] F5：`onFileRemoved` 派发时确认文件不存在
- [x] F7：`FileSystemMonitor::startMonitor` 参数修正
- [ ] 验收（§5）
- [ ] 第 7 节源图片 meta 与跳转（等待集成文档）
