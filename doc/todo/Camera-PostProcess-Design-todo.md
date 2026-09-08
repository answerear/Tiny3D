# 相机后处理与渲染后业务回调：设计与实现

> 本文回答两件事：没有 `Behaviour::onRender` 时，业务层如何在「渲染之后」处理自己的逻辑；以及图像后处理应该挂在哪、怎么加回调。
>
> 结论先行：**帧末的纯 CPU 逻辑用已有的 `onLateUpdate`；消费 GPU 读回结果用下一帧 `onUpdate`；发额外 GPU 命令用订阅式回调或 `Application::onRender`；图像后处理做成挂在相机 GameObject 上的组件，插进管线现有那一行 blit。** 不要在 `Behaviour` 上加遍历式 `onRender`。
>
> **第一期（B1–B5）已落地。** `CameraBehaviour` / `CameraEffectBehaviour` / 临时 RT 池 / MSAA resolve 已进 Core。`CopyEffectBehaviour` / `GrayscaleEffectBehaviour` 以及反相、染色目前在 `PostProcessingApp`（不是 Core 内置组件）。订阅式 `RenderCallback`（B6）仍后做。下文保留设计 Rational；接口以代码为准。GL4 / GLES3 对效果链的 RHI 缺口见 §12。
>
> 相关文档：
>
> - GPU 读回与应用级钩子：`doc/todo/GPU-Readback-onRender-Design-todo.md`（§2.5 拒绝 `Behaviour::onRender`；`Application::onRender` 是应用级最后一个录制窗口）
> - Behaviour 生命周期：`doc/todo/Behaviour-Script-Component-Design-todo.md`
> - 验证计划：`doc/todo/D3D11-Renderer-Backend-Validation-Sample-Plan.md`
> - 后处理 Sample：`doc/todo/PostProcessingApp-Design-todo.md`（热键预设已落地；人眼验收仍以 D3D11 为准，GL4 / GLES3 被 blit 缺口挡住，见 §12）
> - GL4 / GLES3 后端：`doc/todo/GL4-Renderer-Backend-todo.md` A.10.5、`doc/todo/GLES3-Renderer-Backend-todo.md` A.10.5

涉及的主要文件：

- `source/Core/Include/Component/T3DBehaviour.h` / `source/Core/Source/Component/T3DBehaviour.cpp`（基类仍在 Component；派生类目录约定见 §0.3）
- `source/Core/Include/Component/T3DCamera.h` / `source/Core/Source/Component/T3DCamera.cpp`
- `source/Core/Include/Kernel/T3DGameObject.h` / `source/Core/Source/Kernel/T3DGameObject.cpp`
- `source/Core/Include/Render/T3DRenderPipeline.h`
- `source/Core/Include/Render/T3DForwardRenderPipeline.h` / `source/Core/Source/Render/T3DForwardRenderPipeline.cpp`
- `source/Core/Include/Kernel/T3DAgent.h` / `source/Core/Source/Kernel/T3DAgent.cpp`
- `source/Platform/Include/Application/T3DApplication.h`

---

## 0. 目标与非目标

### 0.1 本期目标

| # | 目标 | 说明 |
|---|------|------|
| 1 | **把「渲染后」拆成四类诉求** | 纯 CPU 收尾、读回消费、额外 GPU 绘制、图像后处理，各走各的钩子 |
| 2 | **相机级前后回调** | 只遍历相机自己的 GameObject，不扫整棵场景树 |
| 3 | **相机效果链** | 把「源 RT 直接 blit 上屏」换成「过一遍效果链再 blit」；无效果时零开销、零行为变化 |
| 4 | **临时 RT 池** | 按 `(width, height, format)` 分桶，帧末归还，避免逐帧 create/release |
| 5 | **订阅式渲染回调（可选）** | 非相机对象要发 GPU 命令时用登记，不用全场景虚函数 |

### 0.2 本期边界（明确不做）

- **不加 `Behaviour::onRender`。** 理由见 §2。替代路径见本文 §3 / §4 / §5；readback 文档 §2.5 同步交叉引用本文。
- **不改 `Application::onRender` 的语义。** 它仍是应用级、所有相机画完之后、`endRender` 之前的最后一个录制窗口（readback 文档 A1）。BlitApp 继续用它，游戏里的 bloom 不用它。
- **第一期不做 HDR 中间格式。** 现在 `Camera::setupRenderTexture` 建的是 `E_PF_B8G8R8A8`。灰度、模糊可以在 LDR 上跑通；bloom / tonemapping 等需要 `R16G16B16A16_FLOAT` 的效果放到二期，和相机「HDR 输出」开关一起做。
- **第一期不做完整后处理资源栈。** 不引入 CommandBuffer、不引入 Scriptable Render Pipeline、不引入 Volume / Profile 资源。效果就是挂在相机上的组件。
- **第一期不改编辑器 Scene 相机的绘制模式。** 线框 / ShadedWireframe 仍按现有 `rasterizerOverride` 走；效果链默认不对编辑器相机生效，除非该相机 GameObject 上显式挂了 `CameraEffectBehaviour`。
- **第一期可以先不做订阅式 `RenderCallback`。** 若当期只有相机上的需求，§5 可以后做，避免多一套生命周期。

### 0.3 目录约定：`Behaviour` 派生类单独成目录

`Behaviour` 基类继续留在 `Component/`（它本身是 `Component` 的派生，和 Camera / Light 同级）。**所有引擎侧的 `Behaviour` 派生类**不再塞进 `Component/`，统一放到：

| 种类 | 路径 |
|------|------|
| 头文件 | `source/Core/Include/Behaviour/` |
| 源文件 | `source/Core/Source/Behaviour/` |

本文的 `CameraBehaviour`、`CameraEffectBehaviour`，以及以后引擎里再加的脚本组件，都按这张表落盘。头文件名仍用 `T3D` 前缀，例如 `T3DCameraBehaviour.h`。

**内置后处理效果**继承 `CameraEffectBehaviour`（因此也是 `Behaviour` 派生类），类名统一为 `XXEffectBehaviour`，文件名 `T3DXXEffectBehaviour.h` / `.cpp`，同样放在上面两个目录，不要放到 `builtin` 材质目录或 sample 里冒充引擎组件：

| 效果 | 类名 | 文件 |
|------|------|------|
| 灰度（第一期验证） | `GrayscaleEffectBehaviour` | `Include/Behaviour/T3DGrayscaleEffectBehaviour.h`、`Source/Behaviour/T3DGrayscaleEffectBehaviour.cpp` |
| 拷贝 / 后续模糊、bloom 等 | `CopyEffectBehaviour` / `BlurEffectBehaviour` / … | 同一目录，同一套 `XXEffectBehaviour` 命名 |

Sample 自己的脚本（`RotateBehaviour`、`CubeControllerBehaviour`）继续放在各自 sample 目录，不进 Core。若 sample 只是**挂**引擎内置效果，不要再在 sample 里复制一份同名类。

CMake / 工程文件要把 `Include/Behaviour` 加进 Core 的 include 路径，并把该目录下的 `.cpp` 编进 Core 目标。落地时若目录还不存在，先建空目录再放文件。

---

## 1. 先拆开「渲染后要做什么」

混在一起谈会得出「必须加 `Behaviour::onRender`」的错误结论。实际是四种不同诉求：

| 我要做的事 | 正确位置 | 要不要新东西 |
|---|---|---|
| 纯 CPU 逻辑，只是想「最后再跑」（相机跟随、UI 数值刷新、统计） | `Behaviour::onLateUpdate` | 已有，不用改 |
| 消费 GPU 数据（拾取、遮挡查询结果、截屏） | 本帧 `map`，**下一帧 `onUpdate` 拿结果** | 用 readback 文档已落地的 `map` / `unmap` |
| 发额外 GPU 绘制命令（debug 线、自定义 overlay） | 订阅式渲染回调，或 `Application::onRender` | 需要新增，见 §5；应用级已有 A1 |
| 相机画面的图像处理（bloom、模糊、调色） | 相机上的后处理组件 | 需要新增，见 §4 |

第一类最容易被误判。渲染**不改变场景状态**——管线只读 Transform、材质、剔除结果，不写回。所以「等渲染完再算」这个诉求在 gameplay 层几乎不成立。`onLateUpdate` 已经在所有 `onUpdate` 之后（`T3DScene.cpp`：先 `mRootGameObject->update()`，再 `lateUpdate()`），语义上就是「本帧逻辑的收尾」。Unity 也是这么划的。

第二类有个反直觉的地方：读回结果**天然应该在下一帧的 `onUpdate` 里消费**。数据是 GPU 算完、CPU 拿到之后才有意义，那时候已经是新的一帧。硬要在本帧渲染后同步拿，就得阻塞等 GPU，这正是 readback 文档禁止当常规路径的原因。这类需求也不需要 `onRender`，它需要的是「本帧登记、下帧回调」。

---

## 2. 为什么不加 `Behaviour::onRender`

三条理由，按重要性排。

**1. 分发成本是实打实的。** `GameObject::onLateUpdate`（`T3DGameObject.cpp:494-555`）每次都要 DFS 遍历整棵激活子树，再对每个对象走两个容器（有序 `mUpdateComponents` + 无序 `mUpdateComponents2`），逐个 `asBehaviour()` + `behaviourExecutable()`。现在每帧已经走三趟（update / lateUpdate / fixedUpdate）。加 `onRender` 就是第四趟，而实际需要它的对象通常是个位数。

**2. 它会诱导在错误的地方发 RHI 命令。** 一旦每个 Behaviour 都有 `onRender`，就会有人在里面调 `setRenderTarget`。管线在 `renderForward` 里对每台相机有完整的 RT / viewport / pass 状态编排，末尾还有 `ctx->reset()`（`T3DForwardRenderPipeline.cpp:572`）。任意对象插进去改状态，出来的 bug 极难定位。

**3. Unity 本身也没有这个东西。** `MonoBehaviour` 没有 `OnRender()`。它有的是 `OnPreRender` / `OnPostRender` / `OnRenderImage`——这三个**只对挂在 Camera 所在 GameObject 上的组件生效**。另有 `OnRenderObject` / `OnWillRenderObject` 是逐渲染对象的，性能声名狼藉，在 SRP 里基本被弃用。这个先例值得抄：把回调限制在相机自己的 GameObject 上，成本从「遍历全场景」降到「遍历一个对象的组件」。

readback 文档 §0.2 / §2.5 已经否过这条路。本文是它的正面替代方案，不是翻案。

---

## 3. 相机级前后回调：只扫相机自己的 GameObject

给「这台相机渲染前临时改 culling mask」「画个只属于这台相机的 gizmo」用。既然 §4 已经要遍历相机 GameObject 的组件，这两个钩子成本几乎为零。

### 3.1 接口

```cpp
// 建议新文件：source/Core/Include/Behaviour/T3DCameraBehaviour.h
//             source/Core/Source/Behaviour/T3DCameraBehaviour.cpp
/// 挂在 Camera 所在 GameObject 上的渲染回调基类
class CameraBehaviour : public Behaviour
{
protected:
    /// 本相机开始渲染前（清屏之前）。进入时管线尚未 setRenderTarget
    virtual void onPreRender(RHIContext *ctx) {}

    /// 本相机场景绘制完成后、效果链之前。进入时本相机 pass 已 endPass
    virtual void onPostRender(RHIContext *ctx) {}
};
```

不做成 `Behaviour` 上的虚函数，也不做成 `Camera` 自己的虚函数：

- 挂在 `Behaviour` 上 = 全场景分发，正是 §2 否掉的。
- 挂在 `Camera` 上 = 一台相机只能有一份逻辑，多个效果 / 多个调试组件会互相抢。
- 独立基类 + `getComponents<CameraBehaviour>()`：一台相机可以挂多个，按组件自身顺序或显式 order 调用。

`CameraEffectBehaviour`（§4）可以同时继承 `CameraBehaviour`，也可以做成兄弟类。建议做成**兄弟**：效果链要额外的 src/dst 约定，和「随便画几笔」不是同一套契约。需要前后钩子的效果自己再继承一次，或组合一个 `CameraBehaviour`。

### 3.2 分发点

插在 `ForwardRenderPipeline::renderForward`（`T3DForwardRenderPipeline.cpp:484`）：

```
resolve rt / viewport / VP 矩阵
    ★ invokeCameraBehaviours(camera, onPreRender)     // 清屏之前
clearColor / clearDepthStencil
beginPass
drawCameraQueue（含天空盒）
endPass
    ★ invokeCameraBehaviours(camera, onPostRender)    // 效果链之前
    ★ result = runCameraPostprocessing(...)                  // §4
blit result → 最终目标
ctx->reset()
```

`invokeCameraBehaviours` 只做：

```cpp
auto behaviours = camera->getGameObject()->getComponents<CameraBehaviour>();
for (auto &b : behaviours)
{
    if (b != nullptr && b->isActiveAndEnabled())
        (b.get()->*fn)(ctx);
}
```

`getComponents<T>()` 已含派生类（`T3DGameObject.h:334-344`）。不要 DFS 场景树。

### 3.3 约定

1. `onPreRender` 里可以改本相机的 viewport / clear 参数，但**不要**自己 `setRenderTarget`——下一行管线就会绑 `srcRT`。要临时换 RT，用 `Camera::setRenderTarget` 在 `onLateUpdate` 里改，下一帧生效。
2. `onPostRender` 发生在 `endPass` 之后。可以 `beginPass` / 画 / `endPass`，用完自己 `reset` 到不依赖残留状态。效果链随后会重新绑 RT。
3. 不要在这两个回调里改场景图、销毁 GameObject。销毁走 `Agent::postFrameEndTask`（`T3DAgent.cpp` 的 `mFrameEndTasks`）。
4. 编辑器相机若没挂 `CameraBehaviour`，这两个循环是空的，行为与现在完全一致。

---

## 4. 后处理：插在现有那一行 blit 的位置

### 4.1 现状：中间 RT 已经在了

窗口相机在 `Camera::setRenderTarget` 时会建一张中间 `RenderTexture`（`T3DCamera.cpp:313-318`，`setupRenderTexture` 在 `:389-423`），管线先画到它上面，再 blit 上屏：

```554:569:source/Core/Source/Render/T3DForwardRenderPipeline.cpp
        // 把相机渲染纹理渲染到相机对应的渲染目标上
        if (camera->getRenderTarget()->getRenderTexture() != camera->getRenderTexture())
        {
            T3D_ASSERT(rt->getType() == RenderTarget::Type::E_RT_TEXTURE);
            const Viewport &vp = camera->getViewport();
            // ... viewport 换算 ...
            ctx->blit(rt->getRenderTexture(), camera->getRenderTarget(), offset, box, offset);
        }
```

后处理要做的事，就是把「源 RT 直接 blit 到窗口」换成「源 RT 过一遍效果链，最后一张 blit 到窗口」。基础设施已经在，这是这个方案成本低的关键。

目标本身就是纹理、没有中间 RT 的相机（`setRenderTarget` 的 `E_RT_TEXTURE` 分支，`T3DCamera.cpp:321-325`）：`mSrcRenderTarget == mRenderTarget`，现有 blit 条件为假。效果链若要跑，必须先有一张可采样的源。第一期约定：**只对「源 RT ≠ 最终目标」的相机跑效果链**；直接画到纹理的相机若也要后处理，调用方自己在最终目标后再挂一台相机或自己 ping-pong。不要在第一期偷偷给所有相机都建中间 RT。

### 4.2 `CameraEffectBehaviour` 接口

```cpp
// 建议新文件：source/Core/Include/Behaviour/T3DCameraEffectBehaviour.h
//             source/Core/Source/Behaviour/T3DCameraEffectBehaviour.cpp
/// 相机图像后处理组件，挂在 Camera 所在的 GameObject 上
class CameraEffectBehaviour : public Behaviour
{
public:
    /// 链内执行顺序，小的先执行。不要依赖组件容器的插入顺序
    virtual int32_t getEffectOrder() const { return 0; }

    /**
     * \brief 把 src 处理后写入 dst
     * \param [in] ctx : RHI 上下文，进入时未绑定任何 RT
     * \param [in] src : 上一环节的结果（只读，已 resolve 成非 MSAA）
     * \param [in] dst : 本环节输出目标，尺寸格式与 src 一致
     * \param [in] depth : 相机深度纹理，可为 nullptr
     * \remarks 实现必须写满整个 dst。不写等于留下上一次用完还回池子的脏数据
     */
    virtual void onRenderImage(RHIContext *ctx,
                               RenderTexture *src,
                               RenderTexture *dst,
                               RenderTexture *depth) = 0;
};
```

管线侧改成：

```cpp
        ctx->endPass();

        invokeCameraBehaviours(camera, &CameraBehaviour::onPostRender);

        // 没有任何 enabled 的 CameraEffectBehaviour 时原样返回 rt->getRenderTexture()，
        // 走的还是改造前那条 blit，零开销、零行为变化
        RenderTexture *result = runCameraPostprocessing(ctx, camera, rt->getRenderTexture());

        if (camera->getRenderTarget()->getRenderTexture() != result)
        {
            // ... 原有 viewport 换算 ...
            ctx->blit(result, camera->getRenderTarget(), offset, box, offset);
        }

        ctx->reset();
```

### 4.3 `runCameraPostprocessing` 骨架

```cpp
RenderTexture *ForwardRenderPipeline::runCameraPostprocessing(
    RHIContext *ctx, Camera *camera, RenderTexture *src)
{
    if (src == nullptr)
        return src;

    // 只遍历相机自己的 GameObject，不碰场景树
    auto effects = camera->getGameObject()->getComponents<CameraEffectBehaviour>();

    TArray<CameraEffectBehaviour *> enabled;
    for (auto &e : effects)
    {
        if (e != nullptr && e->isActiveAndEnabled())
            enabled.push_back(e.get());
    }

    if (enabled.empty())
        return src;

    // TUnorderedMultimap 返回顺序不保证稳定，必须显式排序
    std::stable_sort(enabled.begin(), enabled.end(),
        [](const CameraEffectBehaviour *a, const CameraEffectBehaviour *b)
        {
            return a->getEffectOrder() < b->getEffectOrder();
        });

    // MSAA 源不能直接采样，先 resolve（复用 blit 的 Resolve 分支）
    RenderTexture *cur = resolveIfMultisampled(ctx, src);

    for (CameraEffectBehaviour *effect : enabled)
    {
        RenderTexture *dst = acquireTempRT(cur->getWidth(), cur->getHeight(), cur->getFormat());
        effect->onRenderImage(ctx, cur, dst,
            camera->getSrcRenderTarget() != nullptr
                ? camera->getSrcRenderTarget()->getDepthStencil().get()
                : nullptr);
        if (cur != src)
            releaseTempRT(cur);
        cur = dst;
    }

    return cur;
}
```

效果链用完的最后一张临时 RT，在 blit 上屏之后还回池。不要在 `runCameraPostprocessing` 里提前释放它。

### 4.4 四个必须提前想清楚的点

**MSAA。** `Camera::setupRenderTexture` 建中间 RT 时透传了 `desc.MSAA.Count`（`T3DCamera.cpp:414-418`）。MSAA 纹理不能当 SRV 采样，效果链第一步必须先 resolve 成非 MSAA。D3D11 的 `blit` 里已有 `ResolveSubresource` 分支可以复用。**GL4 / GLES3 的 `blit(Texture*, Texture*)` 仍是空实现（静默 `T3D_OK`），`resolveIfMultisampled` 在这两个后端会把未写入的临时 RT 交给效果链。** 忘了这一步，第一个效果就会拿到不能绑的纹理。详见 §12。

**格式没有 HDR 余量。** 现在中间 RT 是 `E_PF_B8G8R8A8`，8 位定点。bloom、tonemapping 需要 `R16G16B16A16_FLOAT` 才有意义，否则高光在进后处理前就被 clamp 掉了。第一期用 LDR 跑通灰度、模糊即可。二期在 `Camera` 上加「HDR 输出」开关决定中间 RT 格式。

**临时 RT 必须池化。** 每个效果一张全屏 RT，逐帧 create/release 会直接拖垮帧率，还会让 `ReportLiveDeviceObjects` 一路涨。做法和 readback 文档 §4.1 的 staging 池一致：按 `(width, height, format)` 分桶，帧末统一归还。两个效果就够用乒乓两张，但接口要按池设计，不要写死两张。

**顺序必须显式。** `getComponents<T>()` 底层是 `TUnorderedMultimap`（`T3DGameObject.h:346`），返回顺序不保证稳定，反序列化后可能和编辑时不一样。所以要有 `getEffectOrder()` 并做稳定排序，不能靠容器顺序。

### 4.5 效果实现约定

第一期验证用一个内置灰度（或拷贝）效果即可，不必先做完整效果库。它是 `CameraEffectBehaviour` 的派生类，命名和落盘见 §0.3：`GrayscaleEffectBehaviour` → `T3DGrayscaleEffectBehaviour.h` / `.cpp`，放在 `Include/Behaviour` 与 `Source/Behaviour`。

```cpp
void GrayscaleEffectBehaviour::onRenderImage(RHIContext *ctx,
                                    RenderTexture *src,
                                    RenderTexture *dst,
                                    RenderTexture * /*depth*/)
{
    // 自己绑 dst 为 RT，src 当 SRV，画一张全屏三角形
    // 顶点缓冲可以复用管线的 mSkyboxVB（T3DForwardRenderPipeline.cpp:741-762）
    // 或效果链自己持有一份同样的大三角形
    ctx->setRenderTarget(/* dst 对应的 RenderTarget */);
    // ... 设材质、画、endPass ...
    ctx->reset();
}
```

约束：

1. **必须写满整个 `dst`。** 池里的 RT 可能带着上一帧另一个效果的残留。
2. 进入时不要假设当前 RT / viewport / shader 还在。自己绑，用完 `reset`。
3. `src` 只读。不要对 `src` 做 UAV，也不要把它当 RT 绑回去。
4. GPU 资源（材质、VB）在 `onEnable` / `onAwake` 创建，`onDisable` 里不要立刻释放——管线可能还握着。销毁走 `postFrameEndTask`。
5. 不要在 `onRenderImage` 里 `map`。读回仍按 readback 文档：`Application::onRender` 里发起，`onPostRender` 里 `unmap`。效果链是管线内部、更早的位置，在这里发起读回会和「所有相机画完」的假设打架。

### 4.6 临时 RT 池

建议放在 `ForwardRenderPipeline` 私有，不要做成全局单例（编辑器可能同时有 Scene / Game 两套管线视角，但当前是一份 pipeline；先绑在 pipeline 上最简单）。

```cpp
struct TempRTKey
{
    uint32_t    width {0};
    uint32_t    height {0};
    PixelFormat format {PixelFormat::E_PF_UNKNOWN};
};

TMap<TempRTKey, TArray<RenderTexturePtr>> mTempRTPool;
TArray<RenderTexturePtr>                  mTempRTInUse;
```

- `acquireTempRT`：同 key 的空闲列表弹一张；没有就 `TextureManager::createRenderTexture`，`Usage::kStatic`，`kCPUNone`（效果链不读回临时 RT）。
- `releaseTempRT`：从 in-use 挪回对应桶。
- `render()` 结束或 `destroy()` 时：in-use 必须为空；退出时把池里的纹理全部 `releaseAllResources`。
- 窗口 resize：`Camera::setupRenderTexture` 会重建中间 RT。池里旧尺寸的纹理下一次 `acquire` 对不上 key，会一直留着。管线 `destroy` 或检测到主窗口尺寸变化时清一次池。

和 readback staging 池可以共用「按描述分桶、帧末归还」的骨架，但**不要合成一个池**：staging 是 `Usage::kCopy`，临时 RT 是 DEFAULT + RTV + SRV，映射完全不同（readback 文档 §3.0.4）。

---

## 5. 非相机对象要发 GPU 命令：订阅式回调

给 debug 线、自定义 overlay 用。核心是**订阅，不是遍历**——只有主动登记的对象才被调用。

第一期若只有相机上的需求，本节可以后做。接口先记账，避免以后再发明第三套钩子。

```cpp
// RenderPipeline 上
enum class RenderStage : uint32_t
{
    kBeforeCameras,   // 所有相机之前，仍在 beginRender/endRender 内
    kAfterCameras,    // 所有相机之后、pipeline->render 返回之前
};

using RenderCallback = TFunction<void(RHIContext *)>;

struct RenderCallbackHandle
{
    uint32_t generation {0};
    uint32_t index {0xFFFFFFFFu};
    bool isValid() const { return index != 0xFFFFFFFFu; }
    static RenderCallbackHandle invalid() { return {}; }
};

RenderCallbackHandle addRenderCallback(RenderStage stage, int32_t order, RenderCallback cb);
void                 removeRenderCallback(RenderCallbackHandle handle);
```

分发点在 `ForwardRenderPipeline::render`（`T3DForwardRenderPipeline.cpp:298-326`）：

```
invokeRenderCallbacks(kBeforeCameras)
for each camera:
    renderShadowMap / renderForward（含 §3 / §4）
invokeRenderCallbacks(kAfterCameras)
```

`kAfterCameras` 仍在 `pipeline->render` 内部，因此也在 `Application::onRender` **之前**。顺序固定为：管线内订阅回调 → 返回 Agent → `Application::onRender`。应用级收尾（readback 的 `map`）不会被 overlay 挡住。

Behaviour 侧在 `onEnable` 订阅、`onDisable` 取消：

```cpp
void DebugLineBehaviour::onEnable()
{
    mHandle = T3D_RENDER_PIPELINE.addRenderCallback(
        RenderStage::kAfterCameras, 0,
        [self = SmartPtr<DebugLineBehaviour>(this)](RHIContext *ctx)
        {
            if (self != nullptr && self->isActiveAndEnabled())
                self->drawLines(ctx);
        });
}

void DebugLineBehaviour::onDisable()
{
    T3D_RENDER_PIPELINE.removeRenderCallback(mHandle);
    mHandle = RenderCallbackHandle::invalid();
}
```

`onDisable` 在组件禁用和销毁前都会被调用（`Behaviour::invokeDisable`），生命周期自带配对。

**两个必须处理的安全问题：**

- **迭代中修改列表。** 回调里再 `addRenderCallback` 会让容器失效。用和 `Agent::mFrameEndTasks` 一样的做法（`T3DAgent.cpp:814-826`）：遍历前 `swap` 出一份快照，增删先进 pending 队列，遍历结束再合并。
- **对象销毁。** `GameObject` 的组件销毁延迟到 `endFrame`，而回调在 `render` 阶段触发。lambda 里只捕获裸 `this` 是危险的，应捕获 `SmartPtr` 并在触发前校验 `isActiveAndEnabled()`。

`order` 参数别省。多个 overlay 的绘制顺序如果靠容器插入顺序决定，行为会随反序列化顺序漂移。

---

## 6. 和 `Application::onRender` 的关系

这几个钩子粒度不同，不冲突，各管一段：

```
beginFrame
  update
    Behaviour::onUpdate
    Behaviour::onLateUpdate          ← 纯 CPU 收尾，渲染还没开始
  beginRender
    pipeline->render(ctx)
      RenderCallback(kBeforeCameras)   ← 可选，§5
      for each camera:
        CameraBehaviour::onPreRender   ← 每相机，清屏前
        renderShadowMap / 场景绘制
        CameraBehaviour::onPostRender  ← 每相机，绘制后、效果链前
        CameraEffectBehaviour::onRenderImage    ← 每相机，效果链
        blit → 最终目标
      RenderCallback(kAfterCameras)    ← 可选，所有相机之后
    Application::onRender()            ← 每帧，app 级收尾，可发 GPU 命令 / map
  endRender
    Application::onPostRender()        ← 提交之后，unmap / 断言
  swapBuffers
endFrame
```

| 钩子 | 粒度 | 谁实现 | 典型用途 |
|------|------|--------|----------|
| `Behaviour::onLateUpdate` | 每对象，渲染前 | 任意脚本 | 相机跟随、UI 数值 |
| `CameraBehaviour::onPre/PostRender` | 每相机 | 挂在相机 GO 上 | 本相机 gizmo、改 clear |
| `CameraEffectBehaviour::onRenderImage` | 每相机每效果 | 挂在相机 GO 上 | bloom、模糊、调色 |
| `RenderCallback` | 每帧订阅 | 任意对象主动登记 | debug 线、全局 overlay |
| `Application::onRender` | 每帧应用级 | Application 子类 | sample 验证、`map` |

`Application::onRender` 是应用级的最后一个录制窗口，`CameraEffectBehaviour` 是管线内的图像处理，两者不重叠。BlitApp 那种验证场景用前者，游戏里的 bloom 用后者。

---

## 7. 文件改动清单

### 7.1 必须改（第一期：相机回调 + 效果链）

| 文件 | 改动 |
|------|------|
| 新建 `source/Core/Include/Behaviour/T3DCameraBehaviour.h` / `source/Core/Source/Behaviour/T3DCameraBehaviour.cpp` | `CameraBehaviour` 基类，两个默认空虚函数 |
| 新建 `source/Core/Include/Behaviour/T3DCameraEffectBehaviour.h` / `source/Core/Source/Behaviour/T3DCameraEffectBehaviour.cpp` | `CameraEffectBehaviour` 基类，`getEffectOrder` + 纯虚 `onRenderImage` |
| `source/Core/Include/Render/T3DForwardRenderPipeline.h` | `runCameraPostprocessing` / `invokeCameraBehaviours` / `acquireTempRT` / `releaseTempRT` / 池成员 |
| `source/Core/Source/Render/T3DForwardRenderPipeline.cpp` | `renderForward` 在 blit 前插入效果链；`destroy` 清池 |
| 引擎 CMake / 工程文件 | 把 `Include/Behaviour` 加进 Core include 路径，并把 `Source/Behaviour` 下的 `.cpp` 编进 Core；RTTR 反射注册（与现有 Behaviour 派生类同一套） |

### 7.2 第一期建议但可后做

| 文件 | 改动 |
|------|------|
| 新建 `source/Core/Include/Behaviour/T3DGrayscaleEffectBehaviour.h` / `source/Core/Source/Behaviour/T3DGrayscaleEffectBehaviour.cpp` | 内置灰度效果，继承 `CameraEffectBehaviour`；验证链路：全屏三角形 + 采样 src、写 dst |
| `source/Core/Include/Render/T3DRenderPipeline.h` | `RenderStage` / `addRenderCallback` / `removeRenderCallback`（§5） |
| `source/Core/Source/Render/T3DForwardRenderPipeline.cpp` 的 `render()` | 相机循环前后分发订阅回调 |

### 7.3 第一期不改

- `T3DBehaviour.h`：不加 `onRender`
- `T3DApplication.h` / `T3DAgent.cpp`：钩子时序按 readback 文档 A1，本文不重复改
- `T3DCamera::setupRenderTexture`：第一期不改格式、不加 HDR 开关
- 编辑器 ImGui 时序（与 readback 文档 §2.6 同一条：另立项）
- sample / BlitApp：继续用 `Application::onRender`，不改成 `CameraEffectBehaviour`

### 7.4 文档回填（实现完成后）

| 文档 | 回填内容 |
|------|---------|
| `GPU-Readback-onRender-Design-todo.md` §12 | ✅ 标明「相机后处理已由本文承接」；并记下 GL4 / GLES3 读回仍是 stub |
| `D3D11-Renderer-Backend-todo.md` | ✅ 登记效果链 blit 替换、临时 RT 池；并交叉引用 GL4 / GLES3 blit 未对齐 |
| `Behaviour-Script-Component-Design-todo.md` | 生命周期表补 `CameraBehaviour` / `CameraEffectBehaviour`，并写明没有 `onRender` |
| `GL4-Renderer-Backend-todo.md` A.10.5 / `GLES3-Renderer-Backend-todo.md` A.10.5 | ✅ 按代码登记后处理所需 blit / 绘制 / shader 变体完成度 |
| `PostProcessingApp-Design-todo.md` | ✅ 去掉「只编 HLSL / 其它后端回退拷贝」；Sample 已落地，GL 验收等 blit |

---

## 8. 分步实现顺序

| 步 | 内容 | 验收 | 预估 |
|----|------|------|------|
| **B0** | 依赖：readback 文档 **A1** 已合（`Application::onRender` 时序正确；接口是 `map` / `unmap`） | ✅ 钩子与 D3D11 读回已落地 | — |
| **B1** | `CameraBehaviour` + `renderForward` 里 `onPreRender` / `onPostRender` 分发 | ✅ 已落地 | 0.5d |
| **B2** | 临时 RT 池（acquire / release / destroy 清池） | ✅ 已落地 | 0.5d |
| **B3** | `CameraEffectBehaviour` + `runCameraPostprocessing`；无效果时走原 blit | ✅ 已落地（含 `CopyEffectBehaviour`） | 1d |
| **B4** | MSAA resolve 接入效果链第一步 | ✅ 已落地 | 0.5d |
| **B5** | `GrayscaleEffectBehaviour` + `getEffectOrder` 稳定排序 | ✅ 已落地。灰度 / 反相 / 染色已有 HLSL + GLSL + ESSL + SPIR-V 嵌入变体（`PostProcessShaderSources`）；GL4 / GLES3 效果链被 blit 缺口挡住，见 §12 | 0.5d |
| **B6** | （可选）§5 订阅式 `RenderCallback` | 一个 debug 线 Behaviour 在 `onEnable`/`onDisable` 配对订阅，销毁后不再被调用 | 0.5d |
| 合计 | | | 约 3.5d（不含 B6 为 3.0d） |

B0 不能省：效果链和 `Application::onRender` 的先后关系要靠 A1 的 `renderOneFrame` 才能说清楚。B1 可以单独合。B3 起必须已经有池，否则验证用的临时 RT 会泄漏。

二期（本文不排期，只记账）：

- `Camera` 上 HDR 输出开关，中间 RT 改 `R16G16B16A16_FLOAT`
- tonemapping / bloom
- 窗口 resize 时主动清池
- 直接画到纹理的相机若也要后处理，再考虑强制建中间 RT

---

## 9. 测试要点

### 9.1 无效果回归

- 场景里一台窗口相机、不挂任何 `CameraEffectBehaviour` / `CameraBehaviour`：画面、RT 数量、draw call 与改造前一致。
- 编辑器 Scene 相机、线框 / ShadedWireframe：现有 `rasterizerOverride` 行为不变。

### 9.2 回调时序

在 D3D11 上对一台挂了 `CameraBehaviour` + 灰度 `CameraEffectBehaviour` 的相机打日志，确认：

```
setRenderTarget(srcRT)
CameraBehaviour::onPreRender
clear / beginPass / draw / endPass
CameraBehaviour::onPostRender
CameraEffectBehaviour::onRenderImage
blit → window
Application::onRender
endRender
Application::onPostRender
```

多台相机：每台自己的 pre → draw → post → effects → blit，然后才是 `Application::onRender`。

### 9.3 效果链正确性

| 用例 | 期望 |
|------|------|
| 只有拷贝效果 | 与无效果像素级一致（允许 MSAA resolve 的精度差） |
| 灰度效果 | 输出是灰度，窗口上能看出 |
| 两个效果 order 0 然后 1 | 先 A 后 B；对调 order 后结果跟着换 |
| 效果 `enabled = false` | 链里跳过，不占临时 RT |
| 相机 GO 禁用 / 销毁 | 不再调用，无悬空 |
| MSAA 开 | 不崩，debug layer 无「MSAA 当 SRV」 |
| 连续 1000 帧 | 临时 RT 池数量不随帧数涨 |

### 9.4 明确不测

- HDR / bloom 观感
- Vulkan 上的效果质量（四个 `blit` 同样未齐，不在第一期验收）
- GL4 / GLES3 的效果观感（**不是「接口后端无关所以不用测」**，是这两个后端的 blit 契约还没对齐 D3D11，见 §12；补齐后再用 PostProcessingApp 人眼验收）
- 订阅式回调与效果链抢 RT（B6 单独测生命周期，不测和效果的交互）
- 在 `onRenderImage` 里做 `map` / `unmap`（明确禁止，见 §4.5）

---

## 10. 风险

| 风险 | 缓解 |
|------|------|
| 效果没写满 `dst`，下一帧看到池子脏数据 | 文档写死；第一期拷贝 / 灰度效果都是全屏覆盖。Debug 下可清成品红再交给效果，漏写一眼能看出来 |
| `getComponents` 顺序漂移 | 强制 `getEffectOrder` + `stable_sort` |
| MSAA 源直接采样 | `runCameraPostprocessing` 第一步 resolve；无效果时仍走原 blit，不强制 resolve。**GL4 / GLES3 上这条 resolve 目前是空操作**，见 §12 |
| 临时 RT 当当前 RT 还绑着就还池 | blit 完成、`ctx->reset()` 之后再 `releaseTempRT(result)` |
| `CameraEffectBehaviour` 里 `setRenderTarget` 把后续 blit 搞乱 | `runCameraPostprocessing` 返回后 blit 自己重新绑最终目标；要求效果用完 `reset` |
| 有人把「读回」写进 `onRenderImage` | §4.5 禁止；读回只认 `Application::onRender` / `onPostRender` |
| 订阅回调捕获裸 `this` | 接口示例强制 `SmartPtr`；`onDisable` 必须 `remove` |

---

## 11. 与既有文档的关系

| 文档 | 关系 |
|------|------|
| `GPU-Readback-onRender-Design-todo.md` | **本文是它 §2.5「拒绝 `Behaviour::onRender`」的正面替代。** `Application::onRender` 与 `map` / `unmap` 已落地；本文不改 Agent 帧循环。读回不要塞进效果链 |
| `Behaviour-Script-Component-Design-todo.md` | 生命周期继续以 `onLateUpdate` 为脚本收尾。本文新增的两个基类是 Behaviour 的派生，不扩展 Behaviour 自身的虚函数表 |
| `D3D11-Renderer-Backend-Validation-Sample-Plan.md` | BlitApp / TextureApp 继续用应用级 `onRender` 做断言，不改成 `CameraEffectBehaviour`。后处理验证见 `PostProcessingApp-Design-todo.md` |
| `Skybox-Support-Design-todo.md` | 天空盒仍在不透明与透明队列之间画（现有 `drawCameraQueue`）。效果链在整台相机 `endPass` 之后，天空盒已经被画进源 RT |
| `GL4-Renderer-Backend-todo.md` A.10.5 | GL4 Window 对后处理的接口完成度：绘制 / 采样 / GLSL 变体已齐，`blit(Tex→Tex)` 与 `blit(Tex→RT)` 的 `ZERO` 语义未对齐 |
| `GLES3-Renderer-Backend-todo.md` A.10.5 | GLES3 与 GL4 同一套 blit 缺口；另有 ESSL `#version 310 es` 与 MSAA `glTexStorage2DMultisample` 的 3.1 门槛 |

---

## 12. 后端对后处理的支持（D3D11 / GL4 / GLES3）

> 管线与效果组件是后端无关的；**blit 契约不是**。第一期人眼验收仍以 D3D11 Window 为准。本节记录效果链实际打到哪些 RHI，以及 GL4 / GLES3 差在哪。接口状态以 2026-09 代码为准。

### 12.1 效果链实际调用的 RHI

| 调用点 | 实际重载 | 用途 |
|--------|----------|------|
| `ForwardRenderPipeline::resolveIfMultisampled` | `blit(Texture*, Texture*)`，`size` 默认 `ZERO` | MSAA 源、或没有 `kGPUShaderResource` 的源，先 resolve 到临时 RT |
| `CameraEffectBehaviour::blitCopy`（Copy 效果） | `blit(Texture*, RenderTarget*)`，`size` 默认 `ZERO` | 整张拷贝 |
| `ForwardRenderPipeline::drawFullscreen`（灰度 / 反相 / 染色） | `setRenderTarget` + 绑 VS/PS / `_MainTex` + `render(3, 0)` + `reset` | 全屏三角采样上一环节 |
| 效果链结束后上屏 | `blit(Texture*, RenderTarget*)`，带明确 `offset/size` | 结果送到窗口 |

`blit(RT→RT)`、`blit(RT→Tex)`、`copyBuffer` 当前后处理**不会调**。`map` / `unmap` 禁止写进 `onRenderImage`。Compute / UAV 是二期 bloom 一类，不是第一期缺口。

D3D11 四个 `blit` 收口到 `resolveBlitEndpoint` + `doBlit`，**`size == Vector3::ZERO` 表示整资源传输**。这是 GL 后端必须对齐的契约，不是「传了 ZERO 就 blit 一块 0×0」。

### 12.2 各后端对照

| 能力 | D3D11 Window | GL4 Window | GLES3 Runtime |
|------|--------------|------------|---------------|
| `createRenderTexture` / `setRenderTarget` / `setViewport` | ✅ | ✅ | ✅ |
| VS/PS 编译、反射、`setPSPixelBuffers` / `setPSSamplers` | ✅ | ✅（含 `SPIRV_Cross_Combined` → `_MainTex`） | ✅ `reflectShaderAllBindings` 同样还原 `_MainTex`；`reflectSamplerBindings` 仍是空 `T3D_OK`（`T3DGLES3ContextBase.cpp:396`），createRHI 路径若只靠后者会丢 binding |
| `drawFullscreen` 所需的 VAO / VB / `render(vertexCount)` | ✅ | ✅ | ✅ |
| 嵌入 shader | HLSL | GLSL（`POSTPROCESS_*_GL`，`#version 400`） | ESSL（`POSTPROCESS_*_GLES`，**`#version 310 es`**） |
| 无效果时上屏 `blit(Tex→RT)`（带明确 size） | ✅ | ✅ | ✅（另有 `glInvalidateFramebuffer`） |
| `blit(Tex→RT)`，`size == ZERO` 当整张拷贝 | ✅ | ❌ 算出 0×0 区域 | ❌ 与 GL4 同一写法 |
| `blit(Texture*, Texture*)` | ✅ | ❌ 空实现，`return T3D_OK`（`T3DGL4Context.cpp:2934`） | ❌ 空实现，`return T3D_OK`（`T3DGLES3Context.cpp:2464`） |
| `bindPixelBuffers` 绑 MSAA RT 的可采样对象 | SRV 走 resolve 后的资源 | 永远绑 `GLTexture` + `GL_TEXTURE_2D`，不切 `GLResolveTex` | 同 GL4 |
| `map` / `unmap`（像素断言，不在效果链内） | ✅ | 🚧 `supportsReadback=false` | 🚧 同上 |
| Console / 离线变体 | null backend | `GL4ConsoleContext` 四个 blit 全空 | 无独立 Console；只在 Android 跑 |

非 MSAA、且相机颜色 RT `shaderReadable=true`（`T3DCamera.cpp:413-415` 已如此）时，`resolveIfMultisampled` 会直接返回 `src`，**不踩** `blit(Tex→Tex)`。这条路径上，shader 效果只依赖绘制 / 采样，GL4 / GLES3 接口是齐的。Copy 效果和 MSAA 相机仍会踩到上表两处 blit 缺口。

### 12.3 缺口怎样把效果链卡死

**1. `blit(Texture*, Texture*)` 未实现 —— 主缺口。**

`resolveIfMultisampled`（`T3DForwardRenderPipeline.cpp:1374`）在需要 resolve 时 `acquireTempRT` 后调用 `ctx->blit(src, resolved)`，然后**返回这张临时 RT**，不是失败回退到 `src`。GL4 / GLES3 空实现返回 `T3D_OK`，临时 RT 从未写入。后续灰度 / 反相再怎么画，采样的也不是相机画面。

窗口 MSAA > 1，或源 RT 没有 `kGPUShaderResource`，一开始就会进这条路径。

**2. 已实现的 `blit(Texture*, RenderTarget*)` 不认 `ZERO`。**

`blitCopy`（`T3DCameraEffectBehaviour.cpp:62`）用默认参数，D3D11 会整张拷贝，GL 算出 `srcX1 = srcX0 + 0`。Copy 预设在 GL4 / GLES3 上静默得到空 / 脏 `dst`。最终上屏那次带了明确宽高，所以**无效果时的窗口 blit 可以正常**。

**3. MSAA 采样侧没接 `GLResolveTex`。**

即便以后补了 Tex→Tex，`bindPixelBuffers` 对 2D 永远绑 `GLTexture`、target 写死 `GL_TEXTURE_2D`。MSAA 颜色附件是 `GL_TEXTURE_2D_MULTISAMPLE`，可采样对象是 `GLResolveTex`。resolve 没做时，效果 shader 里的 `sampler2D` 绑不上合法 2D 纹理。

### 12.4 GLES3 多出来的约束

- 嵌入片元 / 顶点是 `#version 310 es`，`glTexStorage2DMultisample` 也是 GLES 3.1+。**3.0 设备编不过后处理 shader，MSAA 中间 RT 也建不出来。** 失败时应回退 blit 拷贝，但 Copy 自己也被 `ZERO` 语义卡住。
- 没有 `glClipControl`。GL4 用 `glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE)` + FBO 投影 Y 翻转；GLES3 深度停在 [-1,1]。全屏 VS 的 UV：HLSL 用 `0.5 - y*0.5`，GLSL / ESSL 用 `y*0.5 + 0.5`。blit 补齐后要单独对一下是否上下颠倒，这不是缺 API。
- 真机目前只有 Android。Desktop EGL / ANGLE 变体还没做（`GLES3-Renderer-Backend-todo.md` 第 24 章）。

### 12.5 补齐顺序（不在本文排期，记到后端 todo）

1. **`blit(Texture*, Texture*)`**：`glBlitFramebuffer`；`ZERO` 当整张；MSAA 先 resolve 再拷。与 D3D11 一样建议四个重载统一收口，避免再抄一份。
2. **已有 `blit(Texture*, RenderTarget*)` 补 `ZERO` 语义**，Copy 效果才能用。
3. `bindPixelBuffers`：MSAA 源绑 `GLResolveTex` / `GL_TEXTURE_2D`。
4. 空 blit 不要再静默 `T3D_OK`，至少 `T3D_ERR_NOT_IMPLEMENT` + 日志。
5. GLES3：`reflectSamplerBindings` 按 GL4 补上；3.0 设备对 `#version 310 es` 要有明确失败路径。

`blit(RT→RT)` / `blit(RT→Tex)` 可以后做，当前效果链用不到。落地条目见 `GL4-Renderer-Backend-todo.md` A.9 / A.10.5 与 `GLES3-Renderer-Backend-todo.md` A.8 / A.10.5。

---

## 13. 一句话

**脚本收尾用 `onLateUpdate`，读回用下一帧，图像处理挂在相机上插进那一行 blit，应用级收尾继续用 `Application::onRender`。** 这四件事不要合成一个 `Behaviour::onRender`。
