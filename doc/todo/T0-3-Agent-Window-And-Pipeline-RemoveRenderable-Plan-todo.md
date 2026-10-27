# T0-3 实现计划：Agent 窗口查找恢复与渲染管线 `removeRenderable` 补齐

> **所属批次**：T0（引擎核心层小切口收尾）  
> **预估工作量**：0.5 人日  
> **涉及文件**：
> - `source/Core/Include/Kernel/T3DAgent.h`
> - `source/Core/Source/Kernel/T3DAgent.cpp`
> - `source/Core/Include/Render/T3DForwardRenderPipeline.h`
> - `source/Core/Source/Render/T3DForwardRenderPipeline.cpp`

---

## 1. 现状分析（代码依据）

### 1.1 `Agent::getRenderWindow` 恒返回 `nullptr`

在 `source/Core/Source/Kernel/T3DAgent.cpp:617-672` 中：
- `addRenderWindow(RenderWindowPtr window)` 将窗口以 `mRenderWindows.emplace(window->getName(), window);` 存入 `Agent::mRenderWindows`（`TMap<String, RenderWindowPtr>`）；
- `removeRenderWindow(const String &name)` 从 `mRenderWindows.erase(name);` 移除；
- 然而 `getRenderWindow(const String &name) const`（第 658–672 行）中，旧版调用 `mActiveRHIRenderer->getContext()->getRenderTarget(name)` 被注释掉后，**未补上对 `mRenderWindows` 的查找**，导致该函数永远返回 `nullptr`：

```cpp
// source/Core/Source/Kernel/T3DAgent.cpp:658-672（现状）
RenderWindowPtr Agent::getRenderWindow(const String &name) const
{
    RenderWindowPtr window = nullptr;
    if (mActiveRHIRenderer == nullptr)
    {
        T3D_LOG_ERROR(LOG_TAG_ENGINE, "Do not set active renderer !");
    }
    else
    {
        // window = mActiveRHIRenderer->getContext()->getRenderTarget(name);
    }
    return window;
}
```

此外，`addRenderWindow`（第 627 行）未对 `window == nullptr` 判空；`processEvents`（`T3DAgent.cpp` 处理 `SDL_WINDOWEVENT_RESIZED` 处）也未判空 `mDefaultWindow`。

### 1.2 `ForwardRenderPipeline::removeRenderable` 为 TODO 空实现

在 `source/Core/Source/Render/T3DForwardRenderPipeline.cpp:175-180` 中：

```cpp
TResult ForwardRenderPipeline::removeRenderable(Renderable *renderable)
{
    // TODO:
    return T3D_OK;
}
```

虽然正常帧循环中 `cull(Scene *)` 每帧开头都会 `mRenderQueue.clear()`，但在同一帧内（`cull` 之后、`render` 之前，如 `CameraBehaviour::onPreRender` 中）动态禁用或移除 `Renderable` 时，`removeRenderable` 无法将其从 `mRenderQueue`（`Camera* → QueueID → Material* → TList<Renderable*>`）中摘除，存在访问失效组件的风险。

---

## 2. 实现方案（建议实现）

### 2.1 修复 `Agent::addRenderWindow` / `getRenderWindow` / `processEvents`

1. **`Agent::addRenderWindow`**（`T3DAgent.cpp:617`）：
   - 增加 `if (window == nullptr) return T3D_ERR_INVALID_POINTER;` 校验。
2. **`Agent::getRenderWindow`**（`T3DAgent.cpp:658`）：
   ```cpp
   // 建议实现
   RenderWindowPtr Agent::getRenderWindow(const String &name) const
   {
       if (mActiveRHIRenderer == nullptr)
       {
           T3D_LOG_ERROR(LOG_TAG_ENGINE, "Do not set active renderer !");
           return nullptr;
       }

       const auto itr = mRenderWindows.find(name);
       if (itr != mRenderWindows.end())
       {
           return itr->second;
       }

       return nullptr;
   }
   ```
3. **`Agent::processEvents`**：
   - 在响应窗口 Resize 事件调用 `mDefaultWindow->resize(...)` 前补 `if (mDefaultWindow != nullptr)` 保护。
4. 更新 `T3DAgent.h:150, 165, 214` 处的 Doxygen 注释，移除「当前实现查找逻辑已注释，恒返回 nullptr」等过时描述。

### 2.2 实现 `ForwardRenderPipeline::removeRenderable`

在 `source/Core/Source/Render/T3DForwardRenderPipeline.cpp:175` 实现三级遍历移除，并在子容器变空时顺带清理空键，避免产生空材质组跑无效状态绑定：

```cpp
// 建议实现
TResult ForwardRenderPipeline::removeRenderable(Renderable *renderable)
{
    if (renderable == nullptr)
    {
        return T3D_ERR_INVALID_PARAM;
    }

    for (auto itCam = mRenderQueue.begin(); itCam != mRenderQueue.end(); ++itCam)
    {
        RenderQueue &queueMap = itCam->second;
        for (auto itQueue = queueMap.begin(); itQueue != queueMap.end(); )
        {
            RenderGroup &groupMap = itQueue->second;
            for (auto itGroup = groupMap.begin(); itGroup != groupMap.end(); )
            {
                Renderables &list = itGroup->second;
                list.remove(renderable);
                if (list.empty())
                {
                    itGroup = groupMap.erase(itGroup);
                }
                else
                {
                    ++itGroup;
                }
            }

            if (groupMap.empty())
            {
                itQueue = queueMap.erase(itQueue);
            }
            else
            {
                ++itQueue;
            }
        }
    }

    return T3D_OK;
}
```

---

## 3. 验收标准

1. `T3DCore` 与 `T3DCoreEditor` 编译通过；
2. 默认窗口创建后调用 `T3D_AGENT.getRenderWindow(window->getName())` 返回非空且等于 `getDefaultRenderWindow()`；
3. 调用 `addRenderable` 后再调用 `removeRenderable`，确认目标 `Renderable` 从 `mRenderQueue` 中被彻底移除且不残留空材质分组。
