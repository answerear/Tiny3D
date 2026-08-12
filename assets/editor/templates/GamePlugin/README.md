# {ProjectName} 业务代码

这个目录是 TinyEditor 新建工程时自动生成的，用来放游戏的 C++ 业务逻辑。编辑器点
Play 的时候会先把它编成动态库再加载，脚本就跑起来了。

## 怎么加一个自己的脚本

1. 在 `Include/` 加头文件，从 `Tiny3D::Behaviour` 派生，照 `ExampleBehaviour.h` 的
   结构写；
2. 在 `Source/` 加同名 `.cpp`，写上 `RTTR_REGISTRATION` 块注册类型和属性；
3. 回到编辑器点 Play。新文件会被自动收进构建，不用改 CMakeLists。

注册进 `RTTR_REGISTRATION` 的属性会显示在 Inspector 里，也会被存进场景文件；没注册
的字段只是普通的运行期成员。

## 目录结构

```
Assets/Source/
  Include/            业务头文件
  Source/             业务源文件
  GamePluginCommon.cmake   两个变体共享的构建逻辑，一般不用动
  Editor/             Editor 变体的 CMakeLists，编辑器 Play 时编这个
  Runtime/            Runtime 变体的 CMakeLists，发布游戏时编这个
  CMakeLists.txt      想用 IDE 直接打开整个 Source 时用的顶层工程
```

`Include/` 和 `Source/` 是两个变体共用的，你只在这两个目录里写代码，Editor 和
Runtime 两份产物会自动都有。

## 两个变体是怎么回事

编辑器和发布出去的游戏链的是两个不同的引擎核心库（`T3DCoreEditor` 与 `T3DCore`），
它们的 ABI 不兼容，所以同一份业务代码需要各编一次：

| | Editor 变体 | Runtime 变体 |
|--|------------|-------------|
| 谁加载 | TinyEditor | 发布后的游戏 |
| 什么时候编 | 每次点 Play | 导出发布版时 |
| 产物 | `Library/ScriptAssemblies/Editor/` | `Library/ScriptAssemblies/Runtime/` |

对你写代码没有影响，只有一条约束：**不要用编辑器专有的 API**，比如
`Scene::getEditorCamera`、`EditorScene`、`PrefabUtility`。用了的话 Editor 变体照样
能编过，但发布时 Runtime 变体会编译失败。菜单里的 **Validate Runtime Build** 可以
随时单独编一次 Runtime 变体来确认，不必等到导出才发现。

`GameObject`、`Component`、`Behaviour`、`Transform3D` 和 `Scene` 的常规接口在两种
构建下完全一致，正常写 gameplay 碰不到这条边界。

## 手动构建

平时不需要，编辑器会自动调。想在命令行构建的话（在工程根目录执行）：

```
cmake -S Assets/Source/Editor -B Temp/ScriptBuild/Editor \
  -DTINY3D_SDK_ROOT=<TinyEditor 所在目录> \
  -DGAME_PROJECT_ROOT=<工程根目录>
cmake --build Temp/ScriptBuild/Editor --config Debug
```

`TINY3D_SDK_ROOT` 要指向 TinyEditor 可执行文件所在的目录，那里有编辑器构建时导出的
`Tiny3DSDK.cmake`，记录了工具链信息，业务库必须用同样的配置编译才能被正确加载。
`GAME_PROJECT_ROOT` 指向工程根，保证 DLL 落到 `{工程根}/Library/ScriptAssemblies/`。
