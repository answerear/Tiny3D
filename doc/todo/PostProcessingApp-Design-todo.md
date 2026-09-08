# PostProcessingApp：相机后处理验证 Sample 设计

> 用一个独立 Sample 验收已落地的相机回调与效果链（`Camera-PostProcess-Design-todo.md` B1–B5），并顺带演示「sample 自己写效果」该怎么挂。
>
> **Sample 已落地**（场景、热键预设、Copy / 灰度 / 反相 / 染色、GLSL / ESSL 嵌入变体）。人眼 + 热键验收仍以 **D3D11 Window** 为准。GL4 / GLES3 的 shader 变体已经能选到，但效果链被 blit 缺口挡住，见 `Camera-PostProcess-Design-todo.md` §12。
>
> 相关文档：
>
> - 后处理施工蓝图：`doc/todo/Camera-PostProcess-Design-todo.md`（B1–B5 已合；§9 测试要点由本文承接；§12 记 GL4 / GLES3 blit 契约）
> - GPU 读回：`doc/todo/GPU-Readback-onRender-Design-todo.md`（像素断言走 `Application::onRender` / `onPostRender` 的 `map` / `unmap`，**不要**写进 `onRenderImage`）
> - GL4 / GLES3 后端：`doc/todo/GL4-Renderer-Backend-todo.md` A.10.5、`doc/todo/GLES3-Renderer-Backend-todo.md` A.10.5
> - 现有 Sample 骨架：`source/Samples/BehaviourApp`（场景）、`source/Samples/InputApp`（`T3D_INPUT.getKeyDown`）、`source/Samples/TextureApp`（读回冒烟）

---

## 0. 怎么验证（结论先行）

分两层，不要混：

| 层 | 谁来做 | 看什么 |
|---|---|---|
| **人眼 + 热键** | 启动 App，按 0–7 切预设 | 彩色立方体变灰 / 反相 / 染色；关效果立刻回到原画面 |
| **日志时序** | 相机上挂 `LogCameraBehaviour` | `pre → draw → post → onRenderImage → blit → Application::onRender` |
| **可选像素断言** | `Application::onRender` 里 `map` 相机中间 RT | 拷贝预设与无效果预设的中心像素一致；灰度预设饱和度为 0 |

第一期人眼验收只要求 **D3D11 Window**。灰度 / 反相 / 染色的嵌入变体已经按后端选好了（`PostProcessShaderSources.cpp`）：Windows 上 D3D11 走 HLSL、GL4 走 GLSL、Vulkan 走 SPIR-V；Android 上 GLES3 走 ESSL、Vulkan 走 SPIR-V。**不是「其它后端回退 blit 拷贝」**——GL4 / GLES3 会真的去编后处理 shader、走 `drawFullscreen`。挡住它们的是 RHI blit：`blit(Texture*, Texture*)` 空实现、`blit(Texture*, RenderTarget*)` 不把 `size==ZERO` 当成整张拷贝。MSAA 相机或 Copy 预设在这两个后端上会静默吃空 RT / 空拷贝，不是「看不出效果但不崩」那么温和。详见后处理文档 §12。

**D3D11 上现在就能做的检查：**

1. 跑 `PostProcessingApp`，预设 0 应与普通前向场景一致。
2. 热键 2 / 3 / 7：灰度 / 反相 / 染色一眼能看出来；热键 1（Copy）应与预设 0 观感一致。
3. 不挂任何效果（或全部 `setEnabled(false)`）时，画面、RT 数量应与改造前一致。

GL4 / GLES3 上只能先做第 1、3 条（无效果走带明确 size 的上屏 blit）。有 Copy 或 MSAA 时不要当验收，见后处理文档 §12。

---

## 1. 目标与非目标

### 1.1 目标

| # | 目标 | 对应后处理文档 |
|---|------|----------------|
| 1 | 无效果回归：启动默认预设 0，画面就是普通前向场景 | §9.1 |
| 2 | 引擎内置效果：`CopyEffectBehaviour`、`GrayscaleEffectBehaviour` | §9.3 拷贝 / 灰度 |
| 3 | Sample 自写效果：反相、染色，证明派生 `CameraEffectBehaviour` 不必改引擎 | §0.3「sample 自己的脚本不进 Core」 |
| 4 | `getEffectOrder` 稳定排序：灰度→反相 与 反相对调，结果可预期 | §9.3 两个效果 |
| 5 | `enabled = false` 跳过、不占临时 RT | §9.3 |
| 6 | `CameraBehaviour` 时序日志 | §9.2 |
| 7 | 热键切预设，方便人眼和 RenderDoc 抓帧 | — |

### 1.2 非目标

- **不加 `Behaviour::onRender`。** 控制逻辑用 `onUpdate`（热键）/ `onLateUpdate`（可选 HUD 文案）；读回用 `Application::onRender`。
- **不实现 B6 订阅式 `RenderCallback`。**
- **不做 HDR / bloom / tonemapping。** 中间 RT 仍是 `E_PF_B8G8R8A8`。
- **不改编辑器 Scene 相机。** 本 Sample 是独立 exe，不挂编辑器。
- **不把 Sample 脚本做成引擎组件。** `LogCameraBehaviour`、`InvertEffectBehaviour`、`TintEffectBehaviour`、`PostProcessControllerBehaviour` **只进 `PostProcessingApp` 工程**，文件落在 `source/Samples/PostProcessingApp/`，由该 target 编译与 `tiny3d_enable_reflection`。禁止放到 `source/Core/Include/Behaviour/`、`source/Core/Source/Behaviour/`，也不要加进 T3DCore / T3DCoreEditor 的 CMake。类名不要加 `T3D` 前缀（与 `RotateBehaviour` 同一套）。
- **第一期不强制像素级 CI。** 读回断言标成 P2；P1 靠人眼 + 日志 + D3D11 debug layer。GL4 / GLES3 的 `map` / `unmap` 仍是 stub，P2 像素断言这两端做不了。
- **第一期不把 GL4 / GLES3 当效果验收平台。** shader 变体已经在，缺的是 blit 契约；补齐后再用本 Sample 人眼过一遍。
- **不在 `onRenderImage` 里 `map` / `unmap`。**

---

## 2. Sample 形态

对齐 BehaviourApp：一个窗口、一台透视相机、几颗彩色立方体 + 地面，让灰度和反相「一眼能看出来」。

```
PostProcessingApp : SampleWindowApp
  applicationDidFinishLaunching
    建场景 / 光 / 彩色立方体 / 地面 / 相机
    相机 GO 上挂：
      LogCameraBehaviour          // 时序
      CopyEffectBehaviour         // 引擎，默认 disabled
      GrayscaleEffectBehaviour    // 引擎，默认 disabled
      InvertEffectBehaviour       // sample，默认 disabled
      TintEffectBehaviour         // sample，默认 disabled
      PostProcessControllerBehaviour  // 热键切预设
  onRender / onPostRender         // 仅 P2 读回；P1 可空实现
```

立方体用饱和色（红 / 绿 / 蓝 / 黄 / 品红），不要全灰材质，否则灰度预设无法验收。

复用 BehaviourApp 的 mesh / 光照搭建即可，不必再发明一套着色器。`RotateBehaviour` 可以挂一颗立方体，证明效果链跟物体运动同时工作。

---

## 3. 预设与热键

`PostProcessControllerBehaviour` 挂在**相机** GameObject 上，`onUpdate` 里读 `T3D_INPUT.getKeyDown`（与 InputApp 的 `CubeControllerBehaviour` 同一套）。

| 键 | 预设 | 启用的效果（order） | 期望画面 |
|----|------|---------------------|----------|
| `0` | None | 全部 disabled | 与改造前一致，彩色 |
| `1` | Copy | Copy @ 0 | 与 0 像素级接近（允许 MSAA resolve 误差） |
| `2` | Grayscale | Grayscale @ 0 | 整屏灰度，立方体仍能看出明暗 |
| `3` | Invert | Invert @ 0 | 红变青、绿变品红、蓝变黄 |
| `4` | GrayThenInvert | Grayscale @ 0，Invert @ 1 | 先灰再反：高光变暗、暗部变亮的灰图 |
| `5` | InvertThenGray | Invert @ 0，Grayscale @ 1 | 先反再灰：也是灰图，但明暗与 4 **不同**（红块反相后更亮） |
| `6` | GrayDisabled | Grayscale 挂着但 `enabled=false` | 与 0 相同；链里跳过 |
| `7` | Tint | Tint @ 0（偏暖橙） | 整屏偏色，仍能认出原色相对关系 |

`O`：对调当前链里两个效果的 `EffectOrder`（4 / 5 之间切换的快捷方式）。  
`L`：开关 `LogCameraBehaviour` 的逐帧日志（默认只打前 3 帧，避免刷屏）。  
`Esc`：不特殊处理，走窗口关闭。

切预设时只改 `setEnabled` / `setEffectOrder`，**不要** `addComponent` / `removeComponent`。这样测的是「链里跳过」，不是销毁重建。

控制台每切一次打一行：`[PostProcess] preset=4 GrayThenInvert enabled={Copy:0 Gray:1 Invert:1 Tint:0} order={Gray:0 Invert:1}`。

---

## 4. 组件职责

### 4.1 引擎组件（只挂，不复制）

| 类 | 头文件 | 用法 |
|----|--------|------|
| `CopyEffectBehaviour` | `Behaviour/T3DCopyEffectBehaviour.h` | 预设 1 |
| `GrayscaleEffectBehaviour` | `Behaviour/T3DGrayscaleEffectBehaviour.h` | 预设 2 / 4 / 5 / 6 |

不要在 Sample 里再写一份同名类。

### 4.2 Sample 脚本（只进 PostProcessingApp，不进 Core）

下面四个类是本 Sample 后出现的验证/演示脚本，**全部编进 `PostProcessingApp` 工程**，反射产物进 `Samples/PostProcessingApp/Generated/`。Core 只提供基类 `CameraBehaviour` / `CameraEffectBehaviour` 和内置 `Copy` / `Grayscale`。

| 类 | 文件（均在 `source/Samples/PostProcessingApp/`） | 作用 | 工程 |
|----|------|------|------|
| `LogCameraBehaviour` | `LogCameraBehaviour.h/.cpp` | 继承 `CameraBehaviour`，`onPreRender` / `onPostRender` 打日志 | PostProcessingApp |
| `InvertEffectBehaviour` | `InvertEffectBehaviour.h/.cpp` | `1 - rgb`，全屏三角形，抄引擎灰度的建材质路径 | PostProcessingApp |
| `TintEffectBehaviour` | `TintEffectBehaviour.h/.cpp` | `rgb * TintColor`，`TintColor` 做成 `TPROPERTY` | PostProcessingApp |
| `PostProcessControllerBehaviour` | `PostProcessControllerBehaviour.h/.cpp` | 热键、切预设、改 order | PostProcessingApp |

`LogCameraBehaviour` 约定：

```
[Camera] onPreRender
[Camera] onPostRender
[Effect] InvertEffectBehaviour::onRenderImage   // 各效果自己打一行，带 this 指针
[App] onRender
```

`onPreRender` 发生在管线 `setRenderTarget` **之前**（见 `T3DForwardRenderPipeline.cpp`）。日志顺序应是：

```
onPreRender
（管线清屏 / beginPass / draw / endPass，Sample 打不到）
onPostRender
onRenderImage × N
（管线 blit，Sample 打不到）
Application::onRender
```

### 4.3 反相 / 染色实现约定

与 `GrayscaleEffectBehaviour` 同一套契约：

1. `onAwake` 里建 HLSL 材质 + 全屏三角形所用 VS/PS；非 HLSL 则 `mMaterial = nullptr`。
2. `onRenderImage`：有材质就 `ForwardRenderPipeline::drawFullscreen`；否则 `blitCopy`（保证其它后端不崩）。
3. 必须写满 `dst`；用完 `reset`（`drawFullscreen` / `blitCopy` 已做）。
4. 材质名带组件 UUID，避免多实例撞 `TextureManager` / `ShaderManager` 名字。
5. 销毁不在 `onDisable` 里立刻卸 GPU 资源，走组件析构或 `postFrameEndTask`。

反相 PS 示意：

```hlsl
float4 color = _MainTex.Sample(sampler__MainTex, input.uv);
return float4(1.0f - color.rgb, color.a);
```

染色 PS 示意（`TintColor` 用 `material->setColor`，或第一期写死 `(1.2, 0.9, 0.6)` 少一条常量缓冲）：

```hlsl
float4 color = _MainTex.Sample(sampler__MainTex, input.uv);
return float4(color.rgb * float3(1.2f, 0.9f, 0.6f), color.a);
```

第一期染色可以写死，少踩 cbuffer 反射；要进 Inspector 再加 `TPROPERTY TintColor`。

---

## 5. 目录与工程

归属先看这张表，落地时不要把右边四份写进 Core：

| 工程 | 放什么 |
|------|--------|
| **T3DCore / T3DCoreEditor**（已有） | `CameraBehaviour`、`CameraEffectBehaviour`、`CopyEffectBehaviour`、`GrayscaleEffectBehaviour` |
| **PostProcessingApp**（新建） | `LogCameraBehaviour`、`InvertEffectBehaviour`、`TintEffectBehaviour`、`PostProcessControllerBehaviour`，以及 `PostProcessingApp.h/.cpp` |

```
source/Samples/PostProcessingApp/          // 全部编进 PostProcessingApp，不进 Core
  CMakeLists.txt                          // 从 BehaviourApp 抄，改项目名
  PostProcessingApp.h / .cpp
  LogCameraBehaviour.h / .cpp
  InvertEffectBehaviour.h / .cpp
  TintEffectBehaviour.h / .cpp
  PostProcessControllerBehaviour.h / .cpp
```

`source/Samples/CMakeLists.txt` 加一行 `add_subdirectory(PostProcessingApp)`。

**编码：本 Sample 新建的源码文件（`.h` / `.cpp`）必须用 UTF-8 BOM。** 与仓库现有 Sample / Core 源文件一致；无 BOM 的 UTF-8 在 MSVC 下中文注释和字符串会按系统代码页误读。落地时写文件后确认文件头是 `EF BB BF`，不要存成 UTF-8 无签名或 GBK。`CMakeLists.txt` 按仓库惯例即可，不强求 BOM。

CMake 要点（与 BehaviourApp 对齐）：

- `tiny3d_enable_reflection`，`SOURCE_DIR` 指向本 Sample，带 `TCLASS` 的脚本才能 `addComponent<T>()`。
- include：`Behaviour/` 已在 `T3D_CORE_INC_DIR` 下，直接 `#include "Behaviour/T3DGrayscaleEffectBehaviour.h"`。
- 不需要 BuiltinGenerator，不往 `assets/editor/builtin` 丢 shader。

Windows 优先。Android / 其它桌面可以后挂，骨架按 BehaviourApp 的平台分支留着即可。

---

## 6. 启动场景（`applicationDidFinishLaunching`）

```
createScene + Ambient + DirectionalLight
5 个饱和色立方体（可挂 RotateBehaviour）
1 块地面
Camera → setRenderTarget(窗口)     // 必须走中间 RT，效果链才跑
  + LogCameraBehaviour
  + Copy / Grayscale / Invert / Tint（全 disabled）
  + PostProcessControllerBehaviour → applyPreset(0)
```

`Camera::setRenderTarget(窗口)` 会建中间 `RenderTexture`。效果链只对「源 RT ≠ 最终目标」的相机生效；不要改成直接画到纹理。

默认预设 0：**所有效果 disabled**。这样「启动即回归」——没按键时和 BehaviourApp 观感一致。

---

## 7. 可选 P2：像素断言

相机中间 RT 创建时是 `kCPUNone`（`Camera::setupRenderTexture`），不能直接 `map`。不要为了 Sample 去改 Camera。

做法（与 TextureApp 同一条两阶段读回）：

1. App 启动时建一张与窗口同尺寸、`kCPURead`、非 MSAA 的 `RenderTexture`（或 `Texture2D`）。
2. `onRender`：`ctx->blit(camera->getRenderTexture(), mReadbackRT)`，再 `mHandle = mReadbackRT->map(...)`。
3. `onPostRender`：`unmap`，抽中心 8×8 的平均色。
4. 预设 0 与预设 1：平均色差小于阈值（例如每通道 2/255）。
5. 预设 2：`max(r,g,b) - min(r,g,b)` 接近 0（灰度）。
6. 只在切入预设后的第 2 帧做一次（等一帧让效果链跑完），不要每帧阻塞。

P1 可以不写这些，先留空的 `onRender` / `onPostRender`。

---

## 8. 分步实现

| 步 | 内容 | 验收 |
|----|------|------|
| **S0** | CMake + 空 `PostProcessingApp`，场景抄 BehaviourApp（彩色立方体） | 能启动，画面正常，无效果 |
| **S1** | 相机挂 `GrayscaleEffectBehaviour` 且默认 enabled | D3D11 下整屏灰度；注释掉则恢复 |
| **S2** | `PostProcessControllerBehaviour` + 预设 0 / 1 / 2 / 6 | 热键切无效果 / 拷贝 / 灰度 / 禁用灰度 |
| **S3** | `InvertEffectBehaviour` + 预设 3 / 4 / 5 | 反相可见；4 与 5 明暗不同 |
| **S4** | `TintEffectBehaviour` + 预设 7 | 整屏偏暖 |
| **S5** | `LogCameraBehaviour` + `L` | 前 3 帧日志顺序符合 §4.2 |
| **S6** | （可选）P2 读回断言 | 预设 0 vs 1 中心色接近；预设 2 饱和度≈0 |

S0 可单独合。S1 是最小可用验证。S3 才真正测 `getEffectOrder`。

---

## 9. 验收清单（对着后处理文档 §9）

| # | 操作 | 期望 |
|---|------|------|
| 1 | 启动，不按键 | 彩色场景，与 BehaviourApp 同类观感 |
| 2 | 按 `1` | 看不出变化（拷贝） |
| 3 | 按 `2` | 灰度 |
| 4 | 按 `3` | 反相，原色可辨 |
| 5 | 按 `4` 再按 `5` | 两幅灰图明暗分布不同 |
| 6 | 按 `6` | 回到彩色 |
| 7 | 按 `7` | 偏暖，不是灰度 |
| 8 | 按 `L`，看控制台 | pre → post → onRenderImage → onRender |
| 9 | D3D11 debug layer | 无「不能把 MSAA 当 SRV」 |
| 10 | 连续跑一会儿再切预设 | 不崩；关 App 时 `ReportLiveDeviceObjects` 不随切预设次数涨 |

明确不测：HDR / bloom、Vulkan 效果质量、B6、在效果里读回。GL4 / GLES3 的效果观感等 blit 补齐后再测（后处理文档 §12）；无效果回归（预设 0）这两端现在就能看，走的是带明确 size 的上屏 `blit(Tex→RT)`。

---

## 10. 风险

| 风险 | 缓解 |
|------|------|
| 立方体材质太灰，灰度看不出来 | 五个立方体用纯红 / 绿 / 蓝 / 黄 / 品红 |
| 反相 / 灰度 shader 在 GL 上选得到但画面空 | 不是变体缺失。`getXxxEffectShader` 已按 `OPENGL4` / Android GLES 选 GLSL / ESSL。空画面优先查 `resolveIfMultisampled` 的 `blit(Tex→Tex)` 和 Copy 的 `ZERO` 语义，见后处理文档 §12 |
| GL 全屏 VS 与 HLSL UV 公式不一致 | HLSL：`0.5 - y*0.5`；GLSL / ESSL：`y*0.5 + 0.5`。blit 补齐后对一下是否上下颠倒，不要先改管线 |
| 切预设时 `add`/`remove` 组件 | 禁止；只 `setEnabled` / `setEffectOrder` |
| 有人把 `map` 写进 `onRenderImage` | 读回只允许 `Application::onRender` |
| 相机没走中间 RT | `setRenderTarget(窗口)`，不要 `setRenderTarget(纹理)` |
| 反射没进 Sample | CMake 必须 `tiny3d_enable_reflection`，每个 `TCLASS` 头文件配同名 `.cpp` |
| Core 新 Behaviour 没进工程 | 先 reconfigure，确认 `T3DGrayscaleEffectBehaviour.cpp` 在 T3DCore 工程里 |
| 把 Log / Invert / Tint / Controller 写进 Core | 禁止；这四个只属 PostProcessingApp，见 §4.2 / §5 |
| 新建 `.h` / `.cpp` 存成 UTF-8 无 BOM | MSVC 误读中文；按 §5 存 UTF-8 BOM（文件头 `EF BB BF`） |

---

## 11. 和既有文档的关系

| 文档 | 关系 |
|------|------|
| `Camera-PostProcess-Design-todo.md` | 本文是它 §9 / §7.3「后处理验证另开 sample」的落地计划；GL4 / GLES3 缺口见该文档 §12 |
| `D3D11-Renderer-Backend-Validation-Sample-Plan.md` | BlitApp / TextureApp 继续测 blit / 读回；本 Sample 只测相机效果链 |
| `GPU-Readback-onRender-Design-todo.md` | P2 读回复用 A1 钩子；不改 Agent 帧循环。GL4 / GLES3 读回仍是 stub |
| `GL4-Renderer-Backend-todo.md` A.10.5 | GL4 后处理接口完成度与补齐顺序 |
| `GLES3-Renderer-Backend-todo.md` A.10.5 | GLES3 同上，外加 ESSL 3.1 门槛 |

---

## 12. 一句话

**启动默认无效果做回归，热键挂上引擎灰度和 Sample 反相/染色，用 order 对调证明链是稳的；时序靠 `CameraBehaviour` 日志，读回留给 `Application::onRender`。**
