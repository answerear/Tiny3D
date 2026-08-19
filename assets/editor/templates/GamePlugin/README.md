# {ProjectName} 业务代码

这个目录是 TinyEditor 新建工程时自动生成的，用来放游戏的 C++ 业务逻辑。编辑器点
Play 的时候会先把它编成动态库再加载，脚本就跑起来了。

## 怎么加一个自己的脚本

1. 在本目录（`Assets/Source/` 根下）加头文件，从 `Tiny3D::Behaviour` 派生，照
   `ExampleBehaviour.h` 的结构写：`TCLASS()`、`TRTTI_ENABLE(Tiny3D::Behaviour)`、
   `TRTTI_FRIEND`；
2. 需要出现在 Inspector、并写入场景文件的成员，用 `TPROPERTY` 标注 getter / setter；
3. 在同一目录加**同名** `.cpp`，只写逻辑，**不要**再手写 `RTTR_REGISTRATION`
   （构建时 rpp 会生成，手写会重复注册）；
4. 回到编辑器点 Play。新文件会被自动收进构建，不用改 CMakeLists。

标了 `TPROPERTY` 的属性会显示在 Inspector 里，也会被存进场景文件；没标的字段只是
普通的运行期成员。

已经用旧模板建过的工程不会自动升级。旧布局把头文件放在 `Include/`、源文件放在
`Source/` 子目录；现在两者都直接放在 `Assets/Source/` 根下。升级时先把这两个子
目录里的 `.h` / `.cpp` 搬到本目录，再打开 C++ 工程或点 Play——编辑器会同步新的
`GamePluginCommon.cmake`，它只 GLOB 根目录，文件还留在旧子目录里会编不过。
`Player/CMakeLists.txt` 和顶层 `add_subdirectory(Player)` 若缺失也一并拷过来。

## 目录结构

```
Assets/Source/
  *.h / *.cpp              业务头文件和源文件，直接放在这里
  GamePluginCommon.cmake   两个变体共享的构建逻辑，一般不用动
  Editor/                  Editor 变体的 CMakeLists，编辑器 Play 时编这个
  Runtime/                 Runtime 变体的 CMakeLists，发布游戏时编这个
  Player/                  TinyPlayer.exe，VS 里 F5 跑 Runtime 用的壳
  CMakeLists.txt           想用 IDE 直接打开整个 Source 时用的顶层工程
```

根目录的 `.h` / `.cpp` 是两个变体共用的，你只在这里写代码，Editor 和 Runtime
两份产物会自动都有。反射生成的 `*.generated.cpp` 在构建目录里，不会进这份源码树。

## 两个变体是怎么回事

编辑器和发布出去的游戏链的是两个不同的引擎核心库（`T3DCoreEditor` 与 `T3DCore`），
它们的 ABI 不兼容，所以同一份业务代码需要各编一次：

| | Editor 变体 | Runtime 变体 |
|--|------------|-------------|
| 谁加载 | TinyEditor | TinyPlayer |
| 什么时候编 | 每次点 Play | 导出发布版时 |
| 产物 | `Library/CppAssemblies/Editor/` | `Library/CppAssemblies/Runtime/` |

对你写代码没有影响，只有一条约束：**不要用编辑器专有的 API**，比如
`Scene::getEditorCamera`、`EditorScene`、`PrefabUtility`。用了的话 Editor 变体照样
能编过，但发布时 Runtime 变体会编译失败。菜单里的 **Validate Runtime Build** 可以
随时单独编一次 Runtime 变体来确认，不必等到导出才发现。

`GameObject`、`Component`、`Behaviour`、`Transform3D` 和 `Scene` 的常规接口在两种
构建下完全一致，正常写 gameplay 碰不到这条边界。

## 手动构建

平时不需要，编辑器会自动调。想在命令行构建的话（在工程根目录执行）：

```
cmake -S Assets/Source -B Temp/CppBuild \
  -DTINY3D_SDK_ROOT=<TinyEditor 所在目录> \
  -DGAME_PROJECT_ROOT=<工程根目录>
cmake --build Temp/CppBuild --target {ProjectName}Editor --config Debug
```

`TINY3D_SDK_ROOT` 要指向 TinyEditor 可执行文件所在的目录，那里有编辑器构建时导出的
`Tiny3DSDK.cmake`，记录了工具链信息，业务库必须用同样的配置编译才能被正确加载。
同目录还要有 `rpp` 和 `ReflectionSettings.base.json`（引擎 generate 脚本会带上）。
`GAME_PROJECT_ROOT` 指向工程根，保证 DLL 落到 `{工程根}/Library/CppAssemblies/`。

`--target` 换成 `{ProjectName}` 编 Runtime 变体，换成 `TinyPlayer` 编独立宿主。

## 在 Visual Studio 里调试

菜单里 **Open C++ Project** 会在 `Temp/CppBuild` 下生成包含三个工程的 sln：
`{Name}Editor`、`{Name}`、`TinyPlayer`。启动项是 `TinyPlayer`。

这个 sln 和编辑器「编译 C++」用的是**同一棵构建树**，两边共享 obj、`*.tlog` 和反射
缓存。所以在 VS 里编一次之后回编辑器点「编译 C++」会秒回，反之亦然。代价是编辑器
触发 reconfigure（增删源文件时）会重写 `.vcxproj`，VS 若开着会弹「项目已在外部修改」。

两边不要同时编：MSBuild 认不了编辑器那把构建锁，撞上了会报 `LNK1104` 或 `MSB3021`。

- **Runtime（游戏逻辑，独立窗口）**：F5 跑 `TinyPlayer`。它会 `--project` 打开当前
  工程，加载 `Library/CppAssemblies/Runtime/{Name}.dll`。断点打在业务源码上即可。
- **Editor（编辑器里的脚本）**：TinyEditor 已经开着工程时，用
  **调试 → 附加到进程 → TinyEditor.exe**。不要在编辑器还开着时再 F5 出第二个
  TinyEditor。如果编辑器没开，可以把 `{Name}Editor` 设为启动项再 F5，它会带
  `-p -n -o` 拉起 TinyEditor。

不要用 Editor 配置跑 TinyPlayer，也不要把 Runtime DLL attach 到 TinyEditor：
`T3DCore` 和 `T3DCoreEditor` 的 ABI 不兼容。
