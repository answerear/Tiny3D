# 反射配置由 CMake 原生生成（去掉 cct 与 nmake 中转）设计与改造计划

> 目标：把 `ReflectionSettings.json` 的生成从「Windows 专属的 `generate-*.bat` + `cct` + `nmake/` 六次 configure」这条链路，搬进 **CMake 自身**，让每个平台的构建各自生成**反映当次工具链**的配置。
>
> 连带收益：删掉 `cct` 工具、`nmake/` 目录、六个 `TINY3D_*_RTTR` 开关；Android / macOS / Linux 不再依赖 Windows 构建产物；generate 脚本去掉六次 NMake configure。

---

## 1. 背景与问题

### 1.1 现状链路

Windows 下生成一次反射代码要走完整的五段：

```
generate-vs2019-x64-debug.bat
  ├─ setup-msvc-env.bat              # 准备 %INCLUDE%
  ├─ cmake -G "Visual Studio 16"     # 配置 + 编译 rpp.exe / cct.exe
  ├─ cmake -G "NMake Makefiles" ×6   # 每个模块一次，只为导出 compile_commands.json
  │    └─ xcopy compile_commands.json nmake/<Module>/
  ├─ cct.exe nmake/<Module> <生成目录>   # → nmake/<Module>/ReflectionSettings.json
  └─ rpp.exe nmake/<Module> <源码目录>   # 读上面那份 JSON，产出 *.generated.cpp
```

其他平台并不重跑这条链路，而是**复用它的产物**：

- `source/Reflect/CMakeLists.txt` 里 rpp 的配置目录写死成 `${CMAKE_CURRENT_SOURCE_DIR}/../nmake/<Module>`；
- `source/CMakeLists.txt` 中 `TINY3D_GENERATED_BASE_DIR` 在 `CMAKE_CROSSCOMPILING` 时默认指向 `source/vs2019-x64`，各模块从那里 glob `.generated.cpp`。

### 1.2 谁读谁：rpp 并不读 compile_commands.json

这一点容易看串，先明确下来，后面所有讨论都建立在它之上。

**rpp 从头到尾只认 `ReflectionSettings.json`**，文件名在代码里是写死的常量：

```cpp
// source/Tools/ReflectionPreprocessor/Source/T3DReflectionPreprocessor.cpp
const String ReflectionPreprocessor::kReflectionSettingsFile = "ReflectionSettings.json";
```

rpp 的第一个命令行参数是个**目录**，它在该目录下拼上这个固定文件名去读（`parseSettingsFile`）。整个 rpp 工程里没有任何地方引用过 `compile_commands.json`——那是 cct 的输入，不是 rpp 的。

完整数据流：

```
CMake（内部持有结构化的 include 列表 / 宏列表）
  │  拼成命令行字符串
  ├─→ compile_commands.json
  │      │  cct 用 find("-I") / find("-D") 从字符串里扒回结构化数据
  │      ├─→ ReflectionSettings.json
  │             │  rpp 解析 JSON，转成 clang 参数
  │             └─→ libclang → *.generated.cpp
```

所以 `compile_commands.json` 只是个**中间格式**，承担「把 CMake 已有的结构化数据序列化成命令行字符串」这一步，紧接着又被 cct 反向解析回去。本方案要做的是把这一整段往返删掉，让 CMake 直接写最终的 `ReflectionSettings.json`：

```
CMake ──→ ReflectionSettings.json ──→ rpp ──→ libclang ──→ *.generated.cpp
```

**rpp 侧完全不动**：读的文件名、JSON 格式、字段语义全部保持原样。这也是阶段 1 能用「新旧两份 JSON 逐字段比对」来验证的前提。

> 注：`source/Tools/ReflectionPreprocessor/Source/main.cpp` 顶部并排放着两段流程注释，**第一段（「ReflectionPreprocessor 根据 compile_commands.json 生成反射代码」）是早期设想，从未落地**，第二段才是实际实现。清理阶段应删掉第一段，避免继续误导。

### 1.3 cct 实际提供了什么

`CompileCommandTool::execute` 只做三件事：读 `compile_commands.json` 的**第一条**命令、用字符串查找扒出 `-I` 与 `-D`、再从 `%INCLUDE%` 收集系统头。

```cpp
// source/Tools/CompileCommandTool/Source/T3DCompileCommandTool.cpp
const auto &obj = d[0];                       // 只看第一条
auto itr = obj.FindMember("command");
flags = itr->value.GetString();
...
extract(flags, "-I", includePathes);
extract(flags, "-D", macroDefinitions);
```

因为只读第一条，所以每个模块必须拿到一份「只含自己」的 `compile_commands.json`。这就是六个 RTTR 开关的**全部**用途——它们只是把 `CMAKE_EXPORT_COMPILE_COMMANDS` 限定在某一个子目录：

```cmake
# source/System/CMakeLists.txt
if (TINY3D_SYSTEM_RTTR)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif (TINY3D_SYSTEM_RTTR)
```

而 Visual Studio 生成器**不支持**导出 `compile_commands.json`（只有 Makefile / Ninja 支持），所以又不得不额外拉一个 NMake 生成器跑六遍。

**结论：整条 `nmake/` + `cct` 链路的存在理由，只是「把某个 target 的 include 目录和宏定义取出来」——而 CMake 在定义该 target 时就已经知道这些。**

而且这个往返**不是无损的**。cct 扒 `-I` 用的是「找到前缀后一路读到下一个空格」：

```cpp
// source/Tools/CompileCommandTool/Source/T3DCompileCommandTool.cpp
String::size_type start = pos + 2;
String::size_type end = origin.find(' ', start);   // 不认引号
...
String str = origin.substr(start, count);
```

这个 lambda 没有处理引号（同一个函数里后面解析 `-isysroot` 那段倒是处理了）。含空格的 include 路径在 `compile_commands.json` 里是被引号包起来的，走到这里会从空格处截断成 `"C:/Program` 之类的残值。

目前没暴露，只是因为仓库路径与各依赖路径**恰好都不含空格**；换个装在 `C:\Program Files` 下的第三方库就会踩到。CMake 那边本来就持有一份干净的路径列表，绕这一圈纯属自找麻烦。

### 1.4 由此产生的问题

| # | 问题 | 具体表现 |
|---|------|---------|
| 1 | **Android 强依赖 Windows 产物** | `nmake/*/ReflectionSettings.json` 缺失时，`add_reflect_target` 退化成 `echo "Skipping"`，反射**静默不生成**；`TINY3D_GENERATED_BASE_DIR` 又写死 `vs2019-x64`，用 `generate-vs2022` 生成就对不上 |
| 2 | **平台语义错配** | Android 用 **MSVC 头**解析、**NDK** 编译。`#if defined(T3D_OS_WINDOWS)` 等条件编译按 Windows 求值，只能靠手工补丁掩盖（见 `source/Core/Runtime/CMakeLists.txt` 中「非桌面平台排除 Meta 模块生成文件」那段） |
| 3 | **构建耗时** | 六次 NMake configure，每次都重做一遍编译器探测，是 generate 脚本里最慢的部分之一 |
| 4 | **失败静默** | `%INCLUDE%` 为空时 cct 只打印一条 WARNING 就继续，写出缺 `SystemIncludePath` 的 JSON；rpp 退到「扫默认安装位置挑版本号最大的」兜底，用的工具集可能与实际编译的不一致 |
| 5 | **解析有损** | 经由命令行字符串往返，含空格的 include 路径会被 cct 从空格处截断（见 §1.3 末）。当前靠「路径恰好不含空格」侥幸成立 |

---

## 2. 关键洞察：字段全都能从 CMake 拿到

`ReflectionSettings.json` 一共五个字段，逐个对照：

| JSON 字段 | 当前来源 | CMake 来源 |
|-----------|---------|-----------|
| `IncludePath` | `compile_commands.json` 里的 `-I` | `$<TARGET_PROPERTY:tgt,INCLUDE_DIRECTORIES>` |
| `MacroDefinition` | `compile_commands.json` 里的 `-D` | `$<TARGET_PROPERTY:tgt,COMPILE_DEFINITIONS>` |
| `OtherFlags` | 固定四项 + macOS 的 `-isysroot`/`-arch` | 固定四项 + `CMAKE_OSX_SYSROOT` / `CMAKE_OSX_ARCHITECTURES` / `CMAKE_CXX_COMPILER_TARGET` |
| `GeneratedPath` | cct 的第二个命令行参数 | 函数入参（相对 `SOURCE_DIR`，见 §6.1 注意事项） |
| `SystemIncludePath` | `%INCLUDE%` 环境变量 | **需要新写探测逻辑，按工具链分岔（§4）** |

前四项是纯搬运。真正要设计的只有 `SystemIncludePath`，而它恰好正是当前架构最该修的地方——因为它就是「平台绑定」的根源。

---

## 3. 方案选型

### 3.1 方案 A：CMake 直接生成（**选定**）

用 `file(GENERATE)` 配合生成器表达式，为每个需要反射的 target 在其 binary dir 下写出一份 `ReflectionSettings.json`。cct 与 `nmake/` 彻底删除。

- **优点**：每个平台生成自己的配置，语义正确；不再经过命令行字符串的有损往返，含空格路径问题（§1.4 第 5 条）自然消失；一次 configure 搞定，省掉六次 NMake；`-W` 白名单等参数可以统一收进 CMake 函数（现在散落在 `.bat`、`build.yml`、`Reflect/CMakeLists.txt` 三处）；多配置生成器下可按 `$<CONFIG>` 各出一份（现状做不到，见 §7.3）。
- **代价**：要处理 configure / 构建期时序（§5），改动面覆盖引擎所有模块的 CMakeLists。

### 3.2 方案 B：保留 cct，改为由 CMake 传参

让 cct 不再读 `compile_commands.json`，改成接收 CMake 传来的 include / 宏列表。

- 能去掉 `nmake/` 和六次 configure，改动比 A 小。
- 但 cct 退化成一个「把命令行参数拼成 JSON」的程序，没有存在价值，且仍要维护一个 C++ 工具的构建依赖（rpp 必须先编出来才能跑反射，cct 同理）。
- **不采用**，但可作为 A 遇阻时的降级路线。

### 3.3 方案 C：把 JSON 纳入版本控制

- `IncludePath` / `SystemIncludePath` 全是机器相关的**绝对路径**，换台机器即失效。
- **直接否掉。**

---

## 4. `SystemIncludePath` 的平台探测

新增 `source/CMake/Utils/Tiny3DDetectSystemIncludes.cmake`，导出两个变量：`TINY3D_SYSTEM_INCLUDE_DIRS`（进 `SystemIncludePath`）与 `TINY3D_REFLECT_TARGET_FLAGS`（进 `OtherFlags`）。

### 4.1 MSVC

优先级：

1. `$ENV{INCLUDE}` —— 在开发者环境里 configure 时直接可用，与现状完全等价。
2. 从 `CMAKE_CXX_COMPILER` 反推。编译器路径形如
   `.../VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe`，向上四级即工具集根，`include` 子目录即得。
   **这条比 rpp 现有的兜底可靠得多**：rpp 的 `findVCToolsInclude()` 是扫默认安装位置挑版本号最大的，而这里拿到的**一定是实际编译用的那个工具集**。
3. Windows SDK：`CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION` 给版本号，加上 Kits 根目录拼出 `Include/<ver>/{ucrt,um,shared,winrt,cppwinrt}`。

> 注意：`CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES` 在 MSVC 下通常是**空的**，不能指望。

### 4.2 Android NDK

**这是本方案最主要的价值所在。**

- `CMAKE_SYSROOT`（NDK toolchain 文件必定设置）→ `${CMAKE_SYSROOT}/usr/include`；
- `--target=${CMAKE_CXX_COMPILER_TARGET}`（如 `aarch64-none-linux-android26`）进 `OtherFlags`。

rpp 的 JSON 解析已经认 `-target`（见 `JsonHandler` 对 `OtherFlags` 的处理与 `parseFlags` 中的 token 提取），libclang 本身是全 target 的，**host 上的 Windows exe 解析 aarch64 目标没有任何问题**。

### 4.3 Apple（macOS / iOS）

`CMAKE_OSX_SYSROOT` → `-isysroot`，`CMAKE_OSX_ARCHITECTURES` → `-arch`。与 cct 当前从 `compile_commands.json` 里捞出来的等价，只是不再绕一圈。rpp 里现有的 `appendSysroot()` 兜底可保留，但正常路径下不会再触发。

### 4.4 Linux

`CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES` —— GCC / Clang 下 CMake 会正确填充，直接用。

---

## 5. 时序问题与解法（核心难点）

### 5.1 鸡生蛋

- `$<TARGET_PROPERTY:...>` 只有在 **configure 结束后**才展开，`file(GENERATE)` 也是那时才落盘。
- 但现在 `tiny3d_enable_reflection` 是在 **configure 期**就 `execute_process` 跑 rpp 的：

```cmake
# source/CMake/Utils/Tiny3DReflectHelpers.cmake
execute_process(
    COMMAND "${_rpp_cfg}" "${T3DR_SETTINGS_DIR}" "${T3DR_SOURCE_DIR}" -b -j 8
    ...)
```

之所以要在 configure 期跑，是为了让紧接着的 `file(GLOB)` 能收集到 `.generated.cpp` 并 `target_sources` 进去（`if (EXISTS "${_gen}")` 才加入）。引擎模块走的 `SET_PROJECT_FILES` 同理，底层是 `file(GLOB CONFIGURE_DEPENDS)`，**glob 的是磁盘现状**。

于是形成死结：配置文件要等 configure 结束才写得出来，rpp 却要在 configure 中间跑。

### 5.2 解法：rpp 移到构建期 + 产物预声明

1. **rpp 只在构建期跑**（`add_custom_command` + stamp，现有代码已有这套，删掉 configure 期的 `execute_process` 分支即可）。
2. **产物列表改为按源码文件名推导**，不再 glob 磁盘。`tiny3d_enable_reflection` 其实已经在做名字推导（`RotateBehaviour.cpp` → `RotateBehaviour.generated.cpp`），只要去掉那道 `if (EXISTS ...)` 过滤：

```cmake
set(_gen "${T3DR_GENERATED_DIR}/${_title}.generated.cpp")
set_source_files_properties("${_gen}" PROPERTIES GENERATED TRUE)
list(APPEND _generated "${_gen}")
```

`GENERATED TRUE` 让 CMake 接受尚不存在的文件，由 `add_dependencies(${TARGET} ${_reflect_target})` 保证编译前已产出。

3. **顺带修掉一个老问题**：现状首次 configure 时 `.generated.cpp` 还不存在，会**整批漏编**，必须再 configure 一次（`refresh-*.bat` 存在的部分原因）。改成预声明后一次到位。

### 5.3 对引擎模块的影响

`source/{System,Math,Core/Runtime,Core/Editor,Editor/*}/CMakeLists.txt` 里这类调用：

```cmake
set_project_files(Generated ${TINY3D_GENERATED_BASE_DIR}/System/Generated/ .generated.cpp)
```

要换成「按本模块源码推导」的新宏（暂名 `tiny3d_declare_generated_sources`）。但 rpp 除了逐源文件的产物，还会为模板实例另写出**不对应本模块任何源文件**的产物。

**阶段 1 已实测清楚（Core/Runtime，96 个 `.generated.cpp`）：**

| 类别 | 数量 | 能否按源文件名推导 |
|---|---|---|
| 本模块源文件驱动 | 81 | 能 |
| 跨模块模板（`T3DAabb` / `T3DVector3` 等，头在 `source/Math`） | 10 | 不能 |
| STL 模板（`list` / `map` / `set` / `vector` / `unordered_map`，头在 MSVC 安装目录） | 5 | 不能 |

那 15 个产物的文件名 = 模板**定义所在头文件**的 basename。rpp 已经为每个源文件写出同名 `.tpl` 边车文件，其第一列正是该头文件路径，例如：

```
D:\...\source\Math\Include\T3DAabb.h|Tiny3D::TAabb<float>|...
C:\...\MSVC\14.29.30133\include\list|std::list<Tiny3D::UUID>|...
```

所以清单信息 rpp 手上已经有了，只是没有汇总输出。且这 15 个的**集合本身依赖解析结果**——CMake 在 configure 期无从得知，纯推导路线走不通。三个候选解法：

1. **rpp 汇总输出**：跑完写一份 `Generated/.rpp.manifest`。信息最全，但 CMake 首次 configure 时该文件还不存在，仍需一次 glob 兜底，没有真正解决时序。
2. **聚合成固定文件名**：让 rpp 把所有模板实例产物合并进一个 `Templates.generated.cpp`。名字确定，CMake 可以无条件预声明，时序问题彻底消失。需要改 rpp 的产物切分逻辑。
3. **CMake 侧预声明并集**：模板白名单本就是 CMake 传给 rpp 的（`-W Tiny3D::TAabb;...`），STL 那几个也是有限集合，可以预声明这个并集，再要求 rpp 对没有实例的也产出一个空 `.cpp`。改动比 2 小，但把「哪些 STL 容器会被实例化」变成了需要人工维护的清单。

**已选定方案 2**，它是唯一让产物列表在 configure 期完全确定的做法，与 §5.2 的预声明思路一致。细则见 §5.4。

**另一个曾考虑但被否掉的路子**（记录下来免得重复推演）：rpp 完全不改，CMake 预声明一个固定的聚合文件，构建期用脚本扫描目录、生成一个 `#include` 所有模板产物的汇总文件（unity build 风格）。行不通——rpp 产物开头就是 `#include <rttr/registration>`，想靠包进 namespace 来规避函数名冲突的话，标准库和 rttr 的头会一起被卷进 namespace，必然编译失败。除非 rpp 先把 include 段和 registration 段分开输出，那还不如直接做方案 2。

### 5.4 方案 2 实施细则（已确认）

#### 5.4.1 一条决定性约束

RTTR 注册宏展开出来的函数名是写死的，且 rttr 文档明说「It is not possible to place the macro multiple times in one cpp file」：

```cpp
// source/External/rttr/src/rttr/detail/registration/registration_impl.h:290
#define RTTR_REGISTRATION                                       \
static void rttr_auto_register_reflection_function_();          \
...                                                             \
static void rttr_auto_register_reflection_function_()
```

而 rpp 现在每个产物都是一个独立的 `RTTR_REGISTRATION` 块。**所以聚合不能靠拼接文件或 `#include` 汇总，必须把注册语句真正合并进同一个块。**

#### 5.4.2 判据

「路径不在 `mProjectPath`（命令行的 `SourcePath`）之下」⟺「模板实例产物」。rpp 内部已有这个信息：`insertSourceFiles()` 收 `isTemplate` 参数，非模板条目必须匹配 `mProjectPath` 前缀才入表，模板则无条件入表。

已用两种独立口径交叉验证（按 `.cpp + .h` 的 basename、只按 `.cpp` 的 basename），Core/Runtime 两次都是同一批 15 个，System 与 Math 都是 0 个。没有灰色地带，也不存在「本模块头里定义、本模块内实例化」这种会让两种口径打架的情况。

#### 5.4.3 聚合产物的结构

文件名 **`Templates.generated.cpp`**（全树无同名源文件，不会和按源文件名推导出的产物撞车）。

```cpp
// Copyright (C) 2024  Answer Wong
// Generated code exported from ReflectionPreprocessor.
// DO NOT modify this manually! Edit the corresponding .h files instead!
//
// 模板实例的聚合产物。这些模板定义在本模块源码树之外，名字无法在 configure 期
// 从本模块源码推出来，所以统一收进这个固定文件名。

#include <rttr/registration>
#include "Component/T3DTransformNode.h"      // 去重后的全部头文件
#include "T3DAabb.h"

RTTR_REGISTRATION
{
    using namespace rttr;

    // ---- from C:/.../MSVC/14.29.30133/include/list
    {
        using namespace std;
        registration::class_<std::list<Tiny3D::UUID>>("std::list<Tiny3D::UUID>")
            .constructor<>()(policy::ctor::as_object);
    }

    // ---- from D:/.../source/Math/Include/T3DAabb.h
    {
        using namespace Tiny3D;
        registration::class_<Tiny3D::TAabb<float>>("Tiny3D::TAabb<float>")
            .constructor<>()(policy::ctor::as_object);
    }
}
```

三个要点：

1. **`#include` 段去重后提到最顶部**，绝不能留在 `{ }` 内——产物含 `#include <rttr/registration>`，包进块作用域或 namespace 会直接编译失败。
2. **每个原产物的内容包一层 `{ }`** 隔离 `using namespace` 作用域。严格说是冗余的（注册语句用的都是 `Tiny3D::TAabb<float>` 这种全限定名），但零成本零风险。
3. **文件级平台守卫降级为块级**，包在各自的 `{ }` 外面。现有 `useFileGuard` 那套逻辑要按原产物粒度重算。

#### 5.4.4 三个边界

- **空文件必须产出。** System 与 Math 实测 0 个模板实例。CMake 无条件预声明的前提是文件一定存在，所以没有任何模板实例时也要写出一个只有注释头的 `Templates.generated.cpp`，否则这两个模块立刻构建失败。
- **增量粒度变粗。** `commitGeneratedFile()` 的字节比对机制对聚合文件照样有效，内容没变就不碰修改时间，不会无谓触发重编。但 Core/Runtime 从 15 个文件变 1 个，任一模板实例变动就要重编整个聚合单元。这些文件都很小，可接受。
- **模板产物没有 `.deps` / `.tpl` 边车**（实测 96 个 cpp 只有 81 份边车文件），聚合后若需要参与 rpp 自身的增量解析判断，得另行考虑。

#### 5.4.5 连带缺口：无反射内容的源文件不产出产物

**这一项不属于聚合本身，但会卡住整个阶段 2。**

`source/Core` 有 153 个 `.cpp`，产物只有 81 个；System 是 8 个 `.cpp` 对 3 个产物。原因在 `generateSource()` 开头——没有反射节点的条目直接 `continue`，不落盘：

```cpp
for (const auto &val : mSourceFiles)
{
    if (val.second.empty())
    {
        RP_LOG_WARNING("The AST of source file [%s] has not be generated !", ...);
        continue;
    }
```

现状靠 §5.4.6 那道 `if (EXISTS)` 兜住，而 §5.2 要求删掉它。删掉之后 CMake 会预声明 153 个而实际只有 81 个存在，构建直接失败。

**已确认的解法：让 rpp 对所有扫描到的源文件都产出产物，无反射内容的只写注释头。** 这样 CMake 预声明的列表与 rpp 的产出都从同一个源头（target 的源文件列表）推导，天然自洽。代价是多出几十个空翻译单元参与编译（Core 约 72 个，每个几十毫秒且可并行），`Generated` 目录会变杂。

好处是它与聚合改动落在 `generateSource()` 同一个函数里，可以一次做完。

#### 5.4.6 业务工程那边会变简单

现在业务工程排除模板产物，靠的是「按 `.cpp` 名推导恰好漏掉」这个隐式副作用：

```cmake
# source/CMake/Utils/Tiny3DReflectHelpers.cmake
# rpp 扫到引擎头里的 STL 实例会另写出 list/unordered_map.generated.cpp，那些已在
# T3DCore 里注册过，应用层再编会 include 路径不对，还会重复 RTTR 注册。
```

聚合之后，排除动作变成「不要把 `Templates.generated.cpp` 加进 target」，从隐式副作用变成一行显式代码。

#### 5.4.7 顺带修掉：include 路径用反斜杠

产物里的 include 形如 `#include "Component\T3DTransformNode.h"`，`\T` 在 C++ 里是未定义的转义序列，MSVC 容忍但不可移植，阶段 3 上 Android 迟早要改。**已确认一并改成正斜杠。**

另外同一个头会以两种相对路径各 include 一次（`"Component\T3DTransformNode.h"` 和 `"Include\Component\T3DTransformNode.h"`），靠 include guard 兜着，聚合时做去重正好消掉。

**对验收的影响：** 改斜杠会让**全部**产物的字节发生变化，阶段 1 建立的逐字节基线随之失效。因此阶段 2 实施时要分两步走，先只改斜杠、用「归一化后比对」确认差异仅限于 `\`→`/`，重建基线之后再做聚合，这样聚合本身的问题才隔离得出来。

#### 5.4.8 记录：Core/Runtime 与 Core/Editor 产物当前完全相同（正常）

两者的 rpp 扫描根同为 `source/Core`（`rpp .\Core\Runtime ..\Core` 与 `rpp .\Core\Editor ..\Core`），产出 96 个同名产物，实测**内容也逐字节相同**。

这是**设计如此**，不是配置错误：`T3DCore` 与 `T3DCoreEditor` 编译的本就是同一批 Core 源码，只是宏不同（Editor 版多 `T3D_EDITOR` / `T3D_WIN_EDITOR`），产出两个 DLL 分别给 Player 和编辑器用。RTTR 的注册按 DLL 隔离，两份各自注册是必要的。

配置本身也是对的——两份 `ReflectionSettings.json` 的宏确实有差异：

| | MacroDefinition |
|---|---|
| Core/Runtime | 15 项，含 `T3DCore_EXPORTS`，无 `T3D_EDITOR` |
| Core/Editor | 17 项，含 `T3DCoreEditor_EXPORTS`、`T3D_EDITOR`、`T3D_WIN_EDITOR` |

产物之所以仍然相同，是因为**当前源码里所有反射标记都落在 `T3D_EDITOR` 守卫之外**。查过两个典型文件：`T3DPrefabInstance.h` 的 `TRTTI_ENABLE` 在第 42 行，而 `#if defined(T3D_EDITOR)` 段在 105–119 行，两者不重叠；`T3DPrefabUtility.h` 整个类在守卫内，但它没有任何反射标记。

**值得留意的是这个「相同」只是当前源码的巧合，不是保证。** 哪天有人在 `T3D_EDITOR` 守卫内加一个带 `TRTTI_ENABLE` 的类，两份产物就应该分叉——那才是系统的正确行为。阶段 1 的比对只覆盖了 Core/Runtime，这个分叉能力尚未被验证过，建议在阶段 2 补一个用例：临时在守卫内加一个反射类，确认只有 Editor 版产出对应的注册代码。

---

## 6. 接口设计

### 6.1 `tiny3d_write_reflection_settings(<target>)`

```cmake
tiny3d_write_reflection_settings(T3DCore
    SETTINGS_DIR  ${CMAKE_CURRENT_BINARY_DIR}/Reflect
    SOURCE_DIR    ${CMAKE_CURRENT_SOURCE_DIR}
    GENERATED_DIR ${TINY3D_GENERATED_BASE_DIR}/Core/Runtime/Generated)
```

内部用 `file(GENERATE OUTPUT "${SETTINGS_DIR}/ReflectionSettings.json" CONTENT ...)` 写出。

**注意事项：**

- rpp 接收的是**目录**而非文件（`options.SettingsPath = argv[1]`，文件名固定），因此每个 target 需要独立的 `SETTINGS_DIR`。
- `GeneratedPath` 必须写**相对 `SOURCE_DIR`** 的路径——rpp 会把它拼到 `SOURCE_DIR` 后面。现有 `Tiny3DReflectHelpers.cmake` 已有此约束与校验，照搬即可。
- 生成器表达式里的分号分隔列表要用 `$<JOIN:...>` 转成 JSON 数组，且路径需按 §6.3 做转义。

### 6.2 `tiny3d_enable_reflection()` 改造

现有函数保留对外形态，内部三处改动：

1. 删掉「读底板 JSON + 正则插入 `IncludePath`」那套（`_tiny3d_write_reflection_settings`），改为调用 6.1；
2. 删掉 configure 期 `execute_process` 跑 rpp 的分支；
3. 产物列表改预声明（§5.2）。

同时删掉这段 configure 期的特判——它存在的唯一原因就是 `nmake/` 那六次 configure：

```cmake
if (TINY3D_SYSTEM_RTTR OR TINY3D_MATH_RTTR OR TINY3D_CORE_RTTR ...)
    set(_run_rpp FALSE)
```

### 6.3 JSON 转义

Windows 路径含反斜杠，必须转成正斜杠或双写。现有 `_tiny3d_reflect_json_escape` 已做这件事，但它在 configure 期工作；改用 `file(GENERATE)` 后路径来自生成器表达式，需在表达式层面处理（`$<JOIN>` 前先对目录列表做一次 `string(REPLACE)`，或统一约定 CMake 侧路径一律正斜杠）。

---

## 7. 语义变化与新增能力

### 7.1 Android 反射产物会变

换成 NDK 头解析后，`#if defined(T3D_OS_WINDOWS)` 之类条件编译才会被正确求值。**这是修正而非回归**，但会让目前被掩盖的问题浮出来：

- `source/Core/Runtime/CMakeLists.txt` 里「非桌面平台排除 Meta 模块生成文件」的手工补丁，本质上就是在给这个错配打补丁，改完后应可删除；
- 可能出现「Windows 下注册、Android 下不注册」的类型，需确认不影响序列化兼容。

**必须实跑 Android 构建验证，不能纸上推导。**

### 7.2 GamePlugin / SDK 导出的适配

现在业务工程是「读引擎产出的 JSON 当底板，正则往 `IncludePath` 头部插业务 include」：

```cmake
string(REGEX REPLACE
    "(\"IncludePath\"[ \t]*:[ \t]*\\[)[ \t]*(\r?\n)"
    "\\1\\2${_extra_json}"
    _json "${_json}")
```

改造后业务工程走同一个函数生成**自己那份完整配置**，不再需要底板、正则和 `TINY3D_SDK_REFLECTION_BASE`。

但有个约束：业务工程 configure 时**未必在开发者环境里**，`$ENV{INCLUDE}` 可能为空。所以 `GenerateTiny3DSDK.cmake` 仍需把探测结果固化进 `Tiny3DSDK.cmake` 带过去——**固化的思路是对的，只是产物从整份 JSON 缩小成一组路径变量**（`TINY3D_SDK_SYSTEM_INCLUDE_DIRS`）。§4.1 的第 2 条（从编译器路径反推）在这里也能作为二级兜底。

阶段 2 已落地，最终的兜底顺序是：`$ENV{INCLUDE}` → 从 `CMAKE_CXX_COMPILER` 反推 → SDK 固化值 → 都拿不到就 `FATAL_ERROR`。固化值排在反推之后，是因为业务工程真在开发者环境里时，自己探到的那份才对得上它自己的工具链；SDK 里那份只反映引擎当初的构建环境。`TINY3D_SDK_REFLECTION_BASE` 与 `ReflectionSettings.base.json` 在新链路下不再导出（`GenerateTiny3DSDK.cmake` 里按开关跳过），旧链路那段原样保留。

### 7.3 新增：按配置生成

现状 VS 工程是多配置的，但 `ReflectionSettings.json` 只有一份，且是 `generate-*.bat` 里用 `-DCMAKE_BUILD_TYPE=Debug` 跑 NMake 拿到的——**Release 构建用的其实是 Debug 的宏**（`_DEBUG` 等）。

`file(GENERATE)` 支持在输出路径里带 `$<CONFIG>`，可以各出一份。属于顺手修掉的既有缺陷，建议放在阶段 3 单独验证。

---

## 8. 迁移阶段划分

每个阶段都能独立验证、独立回退。

### 阶段 1：Windows 双轨比对（不动现有流程）—— 已完成

- 实现 §4.1 探测 + §6.1 生成函数；
- 输出到**新路径**（如 `<binary>/Reflect/ReflectionSettings.json`），`nmake/` 与 cct 全部保留；
- **验收**：新旧两份 JSON 逐字段比对一致（`IncludePath` 集合相同、`MacroDefinition` 相同、`SystemIncludePath` 相同）；用新 JSON 手工跑一次 rpp，产出的 `.generated.cpp` 与旧的**逐字节一致**。
- **回退**：删掉新增文件即可，主链路未改。

**落地物：**

| 文件 | 作用 |
|---|---|
| `source/CMake/Utils/Tiny3DDetectSystemIncludes.cmake` | §4 的工具链头文件探测，四个平台都已实现 |
| `source/CMake/Utils/Tiny3DReflectHelpers.cmake` | 追加 `tiny3d_write_reflection_settings()`，旧函数原样保留 |
| `source/CMakeLists.txt` | 选项 `TINY3D_REFLECT_SETTINGS_PREVIEW`（默认 ON），六个模块统一接线 |
| `source/Projects/compare-reflection-settings.ps1` | 验收一：逐字段比对 |
| `source/Projects/verify-reflection-output.ps1` | 验收二：两份配置各跑一次 rpp，产物哈希比对 |

**验收结果：**

比对基线不能直接用仓库里现存的 `source/nmake/`——那份是 **VS2022 Hostx86/x86** 配置的产物，而 VS 工程是 VS2019 v142 x64，工具集、位宽、宏三重错配（即 §1.4 第 4 条，实测坐实）。因此另起 `source/nmake-verify/`，在与 VS 工程**相同的环境**下重跑 cct 取得基线。

| 模块 | IncludePath | SystemIncludePath | MacroDefinition | OtherFlags | GeneratedPath | rpp 产物 |
|---|---|---|---|---|---|---|
| System | 8 项一致（含顺序） | 8 项一致 | 14 项一致 | 一致 | 一致 | 11 个文件逐字节一致 |
| Core/Runtime | 16 项一致（含顺序） | 8 项一致 | 15 项一致 | 一致 | 一致 | 258 个文件逐字节一致 |

**实现过程中发现并修掉的一处：** CMake 为共享库自动追加的 `<target>_EXPORTS` 不在 `COMPILE_DEFINITIONS` 属性里，得从 `DEFINE_SYMBOL` / target 类型另行补出。漏掉它会让导出宏被解析成 `dllimport`，类声明的语义和实际编译时对不上。

**顺带暴露的旧链路缺陷（均为新方案正确、cct 有损）：**

1. `find_package(ZLIB)` 命中了 Strawberry Perl 自带的 zlib，`C:/Strawberry/c/include` 确实在 `TinyEditor.vcxproj` 里，但 cct 产出的 JSON 没有它——因为 cct 只读 `compile_commands.json` 的**第一条**命令（§1.4 第 5 条实证）。
2. cct 在 `%INCLUDE%` 为空时会静默把 `SystemIncludePath` 整段省掉；验证过程中从非开发者 shell 跑 cct 立刻复现了（§1.4 第 3 条实证）。

### 阶段 2：切换时序，Windows 走新链路 —— 已完成

细则已在 §5.4 确认。要改 rpp 的 C++ 代码，风险比阶段 1 纯 CMake 改动高一档，**按下面四步走，每步单独验收**，否则出了问题分不清是哪一处引入的。

1. **改 include 斜杠**（§5.4.7）。只动这一处。验收：产物与阶段 1 基线做「归一化后比对」，确认差异**仅限**于 `\`→`/`。通过后重建逐字节基线。
2. **无反射内容的源文件也产出产物**（§5.4.5）。验收：产物数量从 81 变为 rpp 扫描到的源文件总数，且原有 81 个的内容逐字节不变。
3. **模板实例聚合**（§5.4.3）。验收：`Templates.generated.cpp` 编译通过；运行时把 `rttr::type::get_types()` 的结果与改动前做集合比对，**注册的类型集合必须完全一致**——这一步不能再比字节，因为产物结构本来就变了。
4. **落地 §5.2 并切换链路**：rpp 移到构建期、产物改预声明、`Reflect/CMakeLists.txt` 的配置目录从 `nmake/<Module>` 改为各 target 的 `SETTINGS_DIR`。

   顺手补上 `tiny3d_enable_reflection()` 里 glob 的 `CONFIGURE_DEPENDS`：

   ```cmake
   # source/CMake/Utils/Tiny3DReflectHelpers.cmake:264 —— 现状没有 CONFIGURE_DEPENDS
   file(GLOB_RECURSE _src_cpp
       "${T3DR_SOURCE_DIR}/*.cpp"
       "${T3DR_SOURCE_DIR}/*.cxx")
   ```

   引擎侧的 `SET_PROJECT_FILES` 带了（`ProjectCMakeModule.cmake:30`），业务工程这边没带，导致用户新增 `.cpp` 后 CMake 不会自动察觉，得手动重新 configure。代价是每次构建 CMake 要重新 glob 比对一次，对业务工程的文件量级可以忽略。

   **预声明对「用户新增类」这个场景是改善而非退步**，值得在验收时确认一遍：

   | | 现状（`if (EXISTS)` 过滤） | 预声明 |
   |---|---|---|
   | 新增 `Foo.cpp` + 反射标签 | 首次 configure 时产物尚不存在 → 不入 target → 构建后漏编 → **必须再 configure 一次** | 一次 configure + build 到位 |
   | 在已有 `.h` 里新增反射类 | 该文件此前无产物时同样漏编 | **无需 configure**，rpp 构建期重解析并更新产物 |
   | 新增类触发新模板实例 | 产物名对应不上源文件，漏编（见 §11） | 聚合后由固定文件名覆盖（引擎侧）；业务工程侧仍受 §11 限制 |

   预声明不需要预知「有哪些反射类」，只需要「有哪些源文件」——后者在 configure 期通过 glob 完全可知。这个假定成立的前提是 §5.4.5：rpp 对所有扫描到的源文件都产出产物。

- **总验收**：清空构建目录，一次 configure + build 成功，确认不再需要二次 configure；编辑器能正常打开并加载场景（反射缺失在运行期才暴露，静态检查看不出来）。
- **回退**：保留 `nmake/` 生成逻辑，用一个 CMake 选项在新旧链路间切换。rpp 侧的三处改动各自独立，可单独回退。

**落地物：**

| 文件 | 作用 |
|---|---|
| `source/Tools/ReflectionPreprocessor/Source/T3DReflectionGenerator.cpp` | 步骤 1 的 `toIncludeSeparator()`；步骤 3 的 `isTemplateInstanceFile()` / `computeFileGuard()` / `writeRegistrations()` / `generateTemplateAggregate()` |
| `source/Tools/ReflectionPreprocessor/Source/T3DReflectionPreprocessor.cpp` | 步骤 2 的 `writeEmptyGeneratedSources()`，在 `generateSource()` 之后补齐空产物 |
| `source/CMake/Utils/Tiny3DReflectHelpers.cmake` | `tiny3d_add_reflection_target()`（构建期 rpp + stamp 增量）；`_tiny3d_enable_reflection_native()` 接管业务工程 |
| `source/CMake/Utils/ProjectCMakeModule.cmake` | `tiny3d_declare_generated_sources()`，产物清单按源文件推导 |
| `source/CMakeLists.txt` | 选项 `TINY3D_REFLECT_CMAKE_NATIVE`（默认 ON），文件末尾统一接线六个模块 |
| `source/Reflect/CMakeLists.txt` | 新链路下退化成伞形 target，旧的 cct / nmake 那套整段保留在 `else` 分支 |
| `source/CMake/Utils/GenerateTiny3DSDK.cmake` / `Tiny3DSDK.cmake.in` | §7.2：导出 `TINY3D_SDK_SYSTEM_INCLUDE_DIRS`，不再复制整份 JSON 底板 |
| `source/Projects/reflect-audit-products.ps1` | 核对「产物目录」与「vcxproj 实编清单」，专抓有注册内容却漏编的产物 |

**怎么构建：** 新链路不再需要 `generate-*.bat` 那套 `cct` + `nmake` 前置步骤，只要 `%INCLUDE%` 到位（`source/Projects/setup-msvc-env.bat "[16.0,17.0)" 14.29`），普通的两条命令就够了：

```bat
cmake -G "Visual Studio 16 2019" -DTINY3D_BUILD_RTTR_TOOL=OFF -DCMAKE_BUILD_TYPE=Debug ../
cmake --build . --config Debug
```

`generate-*.bat` 暂不删，阶段 4 统一清理。

**验收结果（VS2019 v142 x64 / Debug）：**

产物清单核对 —— 六个模块的预声明清单与 rpp 实际写出的产物**完全重合**，没有漏编，也没有多声明：

| 模块 | 预声明 | 磁盘产物 | 漏编（含注册） | 声明了但不存在 |
|---|---|---|---|---|
| System | 9 | 9 | 0 | 0 |
| Math | 13 | 13 | 0 | 0 |
| Core/Runtime | 154 | 154 | 0 | 0 |
| Core/Editor | 154 | 154 | 0 | 0 |
| Editor/TinyLauncher | 9 | 9 | 0 | 0 |
| Editor/TinyEditor | 25 | 25 | 0 | 0 |

注册内容核对 —— 把构建实际编进去的那批产物与步骤 1 基线（`ReflectSnapshot/s1slash`）逐条比对，注册语句数与注册类型集合都一致：

| 模块 | 注册语句 | 注册类型 |
|---|---|---|
| Core/Runtime | 3263 → 3263 | 241 → 241，集合相同 |
| Math | 287 → 287 | 10 → 10，集合相同 |
| System | 40 → 40 | 3 → 3，集合相同 |

总验收 —— 删掉 `source/vs2019-native/` 后一次 configure + 一次 build 全绿（0 error），rpp 在构建期跑了 9 次（六个模块 + 三个用 `tiny3d_enable_reflection` 的 Sample），**不需要第二次 configure**。紧接着的第二次 build 完全空转：不重新 configure、不启动 rpp、不重编任何产物。

运行期抽查 —— `bundlebuilder` 在 POST_BUILD 里加载编辑器插件、按反射序列化资源，导出 28 个文件 / 32 条 manifest 记录成功。反射注册若有缺失，这一步会先炸。GUI 编辑器打开场景仍需人工过一遍。

回退核对 —— `-DTINY3D_REFLECT_CMAKE_NATIVE=OFF` 能正常 configure，走的还是 cct / nmake 那套（configure 期跑 rpp、glob 产物目录）。

**实施中发现并修掉的两处：**

1. **聚合产物撞上 COFF 段数上限**（`C1128: number of sections exceeded object file format limit`）。几十个模板实例的注册挤进一个翻译单元，rttr 的 `type_data` 又给每个类型摊开一大片 COMDAT。聚合之前这些注册分散在各自产物里，所以以前碰不到这条线。解法是给 `Templates.generated.cpp` 单独加 `/bigobj`（见 `tiny3d_declare_generated_sources`）。这是聚合方案的固有代价，Android / Linux 侧不受影响（ELF 无此限制）。
2. **业务工程目录里的构建残留被当成源文件**。gradle 的 `Android/app/.cxx/**/CMakeCXXCompilerId.cpp` 被 `file(GLOB_RECURSE)` 收进来，跟着产出并编进 Sample。步骤 2 之前 rpp 不给无反射内容的文件出产物，所以撞不上。`_tiny3d_enable_reflection_native()` 里按 `CMakeFiles` / `.cxx` / `.gradle` 过掉。

**顺带确认的既有缺陷（与反射链路无关，未在本阶段修）：** `ResourceApp` 与 `SkyboxApp` 的 POST_BUILD 都会用 bundlebuilder 重打同一份 `assets/samples/bundle` 并拷到同一个运行目录。并行构建（MSBuild `-m`）下两者互相踩，报 `MSB3073`；串行构建正常。

### 阶段 3：接入 Android，补齐其他平台

- 落地 §4.2 / §4.3 / §4.4；
- Android 侧去掉 `TINY3D_GENERATED_BASE_DIR` 指向 `vs2019-x64` 的默认值，改为各自构建目录；
- **验收**：在**没有跑过任何 Windows generate 脚本**的干净树上，Android Studio 能完整构建并运行 Sample；对比 NDK 头解析前后 `.generated.cpp` 的差异并逐条确认（§7.1）；顺带验证 §7.3。
- gradle 的 `buildHostRpp` 任务保留（rpp.exe 仍需 host 编译），但 `onlyIf { !rppExe.exists() }` 应改为带时间戳判断，否则 rpp 更新后不会重建。

### 阶段 4：清理

- 删除 `source/Tools/CompileCommandTool/` 整个目录；
- 删除六个 `TINY3D_*_RTTR` 开关及各 CMakeLists 里对应的 `CMAKE_EXPORT_COMPILE_COMMANDS` 分支；
- `generate-vs2019-x64-debug.bat` / `generate-vs2022-x64-debug.bat` 删掉 `nmake` 段（六次 configure + 六次 cct）；
- `.github/workflows/build.yml` 的「阶段二」整段删除；
- `GenerateTiny3DSDK.cmake` 改为导出路径变量而非复制 JSON 底板；
- 删掉 `ReflectionPreprocessor/Source/main.cpp` 顶部那段从未落地的流程注释（§1.2 注），并把第二段更新成新链路。

---

## 9. 风险与取舍

| 风险 | 评估 | 对策 |
|------|------|------|
| 模板实例产物无法靠名字推导（§5.3） | ~~高，阻塞阶段 2~~ → **已解**（阶段 1 摸清，选定聚合方案） | 见 §5.4：按 `mProjectPath` 前缀判定，聚合进固定名的 `Templates.generated.cpp` |
| 无反射内容的源文件不产出产物，预声明会落空（§5.4.5） | ~~**高**，阻塞阶段 2~~ → **已解** | 让 rpp 对所有扫描到的源文件都产出产物，无内容的只写注释头 |
| 改 rpp 的 C++ 代码引入回归 | ~~中~~ → **已验收**（阶段 2 三张比对表） | 阶段 2 拆成四步，每步单独验收；三处改动可分别回退 |
| 聚合产物撞上目标文件格式上限 | 低，仅 MSVC | 阶段 2 实测触发 `C1128`，给 `Templates.generated.cpp` 单独加 `/bigobj` |
| 工程目录里的构建残留被当成源文件 | 低 | 阶段 2 实测触发（gradle 的 `.cxx/`），glob 时按目录名过掉 |
| 业务工程自有类型的模板实例不注册 | 低，且为既有缺陷 | **不在本方案范围**，见 §11，建议另行立项 |
| Android 反射产物变化引入回归 | 中 | 阶段 3 逐文件 diff，先在一个 Sample 上验证 |
| MSVC 系统头探测在非常规安装下失败 | 中 | 三级兜底：`$ENV{INCLUDE}` → 编译器路径反推 → rpp 现有自动探测；**任一级都拿不到时应直接 `FATAL_ERROR`，不再静默** |
| 生成器表达式在 JSON 里的转义 | 中 | 阶段 1 就要把含空格 / 中文的路径纳入测试 |
| 改动面覆盖所有模块 CMakeLists | 中 | 按阶段推进，每阶段留回退开关 |
| 业务工程 configure 时无开发者环境 | 低 | §7.2 的固化机制 |

**明确不做的事**：不改 rpp 的解析逻辑与增量缓存机制；不改 `.generated.cpp` 的代码生成规则。本方案只改「配置从哪来」。

---

## 10. 影响文件清单

**新增**

- `source/CMake/Utils/Tiny3DDetectSystemIncludes.cmake`

**改动**

- `source/CMake/Utils/Tiny3DReflectHelpers.cmake`（核心）
- `source/CMake/Utils/GenerateTiny3DSDK.cmake`、`Tiny3DSDK.cmake.in`
- `source/CMake/Utils/ProjectCMakeModule.cmake`（新增产物预声明宏）
- `source/CMakeLists.txt`（`TINY3D_GENERATED_BASE_DIR`）
- `source/Reflect/CMakeLists.txt`
- `source/{System,Math,Core/Runtime,Core/Editor,Editor/TinyLauncher,Editor/TinyEditor}/CMakeLists.txt`
- `source/Samples/*/CMakeLists.txt`（`tiny3d_enable_reflection` 调用方）
- `source/Projects/generate-vs20{19,22}-x64-debug.bat`
- `.github/workflows/build.yml`
- `assets/editor/templates/GamePlugin/GamePluginCommon.cmake`

**删除**

- `source/Tools/CompileCommandTool/`
- `source/nmake/`（构建产物，非版本控制）

---

## 11. 已知限制：业务工程自有类型的模板实例不会被注册

**这是既有缺陷，不由本方案引入，也不由本方案解决。单独记录，建议另行立项。**

### 11.1 现象

业务工程定义了新类型再套进 STL 容器（例如成员写 `std::vector<MyNewType>`），这份模板实例的反射注册会缺失。rpp 其实正常产出了对应的 `.generated.cpp`，只是没被编进 target。

实测 InputApp，`Generated` 目录里躺着三个产物，`InputApp.vcxproj` 只编译了一个：

| 产物 | 是否编译 |
|---|---|
| `CubeControllerBehaviour.generated.cpp` | 是 |
| `list.generated.cpp` | 否 |
| `unordered_map.generated.cpp` | 否 |

### 11.2 根因：引擎与业务工程收集产物的方式不同

| | 收集方式 | 模板产物 |
|---|---|---|
| 引擎模块 | `set_project_files(Generated .../Generated/ .generated.cpp)`，**glob 整个目录** | 全部编入。`T3DCore.vcxproj` 里 96 个产物一个不落，`T3DAabb` / `list` / `vector` / `unordered_map` 都在 |
| 业务工程 | `tiny3d_enable_reflection()`，**按 `.cpp` 源文件名推导** | 对应不上任何源文件，被漏掉 |

所以「Math 的类模板在别处使用时也注册进去了」这个观察是成立的——那走的是引擎侧的目录 glob。缺口只存在于业务工程一侧。

### 11.3 触发条件很窄

业务工程使用 `std::vector<Buffer>` 这类**引擎已注册过**的实例没有问题：RTTR 的 type registry 跨 DLL 共享，`T3DCore.dll` 注册过的类型 GamePlugin 查得到。只有业务工程**自己定义新类型再套进容器**时，引擎不可能预先知道该类型，这份实例才会缺失。

### 11.4 两个障碍

当前的排除不纯粹是设计选择，`Tiny3DReflectHelpers.cmake` 里的原注释提了两条：

1. **重复注册**——纯引擎类型的实例（`std::list<UUID>` 等）已在 `T3DCore` 注册过，业务工程再编一遍是重复的；
2. **include 路径不对**——模板产物里的 `#include` 是相对引擎 include 路径写的，业务工程即便想编也编不过去。

第 2 条是更硬的障碍，所以现状更接近「编不了，索性排除」而非主动设计。

### 11.5 为什么不并进阶段 2

§5.4.3 的聚合方案不会自动修好它：`Templates.generated.cpp` 里的 include 仍然是引擎的相对路径。真要修，得让 rpp 区分「引用了业务工程自有类型的模板实例」与「纯引擎类型的模板实例」，前者归业务工程编、后者跳过，并解决 include 路径的改写。这是独立课题，混进阶段 2 会放大风险。

聚合方案唯一的正面影响是：排除动作从「按名字推导恰好漏掉」这种隐式副作用，变成针对一个固定文件名的显式决策，将来要做精细区分时有个明确的着手点。
