# D3D11 渲染后端未实现接口 —— 实现方案

> 配套文档：`doc/todo/D3D11-Renderer-Backend-todo.md`（实现状态清单）。
>
> 本文只覆盖状态为 **❌ 未实现**、**⚠️ 部分完成**、**💬 被注释禁用** 的接口，给出可直接落地的改造方案。
>
> 涉及文件：
>
> - `source/Plugins/Renderer/Direct3D11/Window/Source/T3DD3D11Context.cpp`（下称 `T3DD3D11Context.cpp`）
> - `source/Plugins/Renderer/Direct3D11/Window/Include/T3DD3D11Context.h`
> - `source/Plugins/Renderer/Direct3D11/Window/Include/T3DD3D11RenderBuffer.h`
> - `source/Plugins/Renderer/Direct3D11/Window/Source/T3DD3D11RenderBuffer.cpp`
> - `source/Plugins/Renderer/Direct3D11/Base/Include/T3DD3D11Error.h`
> - `source/Plugins/Renderer/Direct3D11/Base/Source/T3DD3D11Mapping.cpp`
> - `source/Core/Include/RHI/T3DRHIContext.h`

---

## 0. 阅读前须知：三条影响所有方案的既有约束

在动手前必须先理解这三点，否则写出来的代码在多线程模式下会出错。

### 0.1 `ENQUEUE_UNIQUE_COMMAND` 的返回值不是操作结果

```157:177:source/Core/Include/RHI/T3DRHIThread.h
        template<typename Action, typename... Args>
        TResult enqueue_unique_command(Action action, Args... args)
        {
            TResult ret = T3D_OK;
            // ... 省略 trait 推导 ...
            if (isRunning())
            {
                command_type *cmd = T3D_NEW command_type(std::move(args)..., action);
                ret = addCommand(cmd);
            }
            else
            {
                ret = action(args...);
            }
            return ret;
        }
```

RHI 线程开启时，返回的是**入队结果**（恒 `T3D_OK`）；只有 RHI 线程未运行（单线程回退路径）时才返回 lambda 的真实返回值。

**推论**：`createXXX` 系列里 `if (T3D_FAILED(ret)) { d3dBuffer = nullptr; }` 这种写法在多线程下永远不会触发。**新代码不要依赖这个返回值判断资源是否创建成功**，而是：

- 让 lambda 内部失败时把 D3D 对象保持为 `nullptr`，并打 `T3D_LOG_ERROR`；
- 所有后续使用点（`setRenderTarget` / `setPixelBuffers` / `blit`）对 `D3DTexture == nullptr` 做防御。

现有代码已经是这个隐含契约，本文所有方案都遵循它，不额外引入同步等待。

### 0.2 lambda 参数必须是「自持有生命周期」的类型

`RHICommandT` 把参数**按值拷贝**进 `std::tuple`，命令在 RHI 线程执行时原始对象可能已销毁。因此：

| 传递内容 | 正确做法 | 反例 |
|---------|---------|------|
| 引擎/RHI 对象 | 传 `SmartPtr`（`TexturePtr` / `D3D11PixelBuffer2DPtr`），构造时自动加引用 | 传裸 `Texture*` |
| CPU 数据块 | 深拷贝一份 `Buffer` 交给命令，lambda 末尾 `buffer.release()`（参考 `writeBuffer`，T3DD3D11Context.cpp:3394） | 直接传调用方的 `Buffer` |
| POD 描述结构 | 直接按值传 `D3D11_TEXTURE2D_DESC` 等 | — |
| 裸 COM 指针 | 只在「同一条命令内取出并使用」时可用；跨命令必须由 SmartPtr 包装对象持有 | 把 `ID3D11Texture2D*` 存进命令 |

### 0.3 D3D11 拷贝 API 的硬性限制

后面 blit / copyBuffer 的方案全部围绕这张表设计：

| API | 要求 | 备注 |
|-----|------|------|
| `CopyResource` | 同资源类型、同尺寸、同 mip/array 层数、格式相同或同 typeless family、SampleDesc 相同 | dst 不能是 `IMMUTABLE` |
| `CopySubresourceRegion` | 同上，但允许区域偏移；**不允许 MSAA 资源参与** | dst 不能是 `IMMUTABLE`；同一资源内 src/dst subresource 不能相同 |
| `ResolveSubresource` | src 必须 MSAA、dst 必须非 MSAA、尺寸相同、格式兼容 | 只能整个 subresource |
| 全屏四边形绘制（`blitRegion`） | 需要 src 的 **SRV** 和 dst 的 **RTV** | 可跨格式、可缩放 |

**关键坑**：SwapChain BackBuffer 创建时 `BufferUsage` 只带 `DXGI_USAGE_RENDER_TARGET_OUTPUT`，**没有 `DXGI_USAGE_SHADER_INPUT`，所以拿不到 SRV**。这意味着「窗口作为 blit 源」的区域拷贝路径天然不可用，必须专门处理（见 §4.3）。

---

## 1. 任务分组与优先级

| 优先级 | 分组 | 任务 | 预估 | 依赖 | 状态 |
|-------|------|------|------|------|------|
| **P0** | A 低风险缺陷修复 | A1 `setVertexShader`/`setComputeShader` 空指针 | 0.2d | 无 | ✅ |
| **P0** | A | A2 `createPixelBuffer2D` 除零 + mip/array 子资源 | 0.5d | §2.1 helper | ✅ |
| **P0** | A | A3 `createPixelBufferCubemap` 硬编码 mip/array | 0.5d | §2.1 helper | ✅ |
| **P0** | A | A4 恢复窗口自带 DepthStencil | 0.5d | 无 | ✅ |
| **P1** | B 资源创建补全 | B1 `D3D11Mapping::get(PixelBuffer3DDesc)` | 0.2d | 无 | ✅ |
| **P1** | B | B2 `D3D11PixelBuffer3D` 类补全 | 0.2d | 无 | ✅ |
| **P1** | B | B3 `createPixelBuffer3D` | 0.5d | B1 + B2 | ✅ |
| **P1** | B | B4 `createPixelBuffer1D` | 0.5d | §2.1 helper | ✅ |
| **P1** | C 拷贝/Blit | C0 抽取源 / 目标解析 helper | 0.5d | 无 | ✅ 落地为 `BlitEndpoint` + `resolveBlitEndpoint` |
| **P1** | C | C1 `blit(Texture→RenderTarget)` 补全全部纹理类型 | 0.5d | C0 | ✅ Cubemap 仅直接拷贝 |
| **P1** | C | C2 `blit(RenderTarget→RenderTarget)` | 0.5d | C0 | ✅ |
| **P1** | C | C3 `blit(RenderTarget→Texture)` | 0.3d | C0 | ✅ |
| **P1** | C | C4 `blit(Texture→Texture)` | 0.3d | C0 | ✅ |
| **P1** | C | C5 `copyBuffer` | 0.5d | 无 | ✅ |
| **P2** | D Resize | D0 决定 resize 接口归属（提升到 `RHIContext` / 保持后端私有） | 0.2d | 需决策 | ✅ 选择提升到 `RHIContext` |
| **P2** | D | D1 `resizeRenderTexture` | 1d | D0 | ✅ |
| **P2** | D | D2 `resizeRenderTarget` | 0.3d | D1 | ✅ |
| ~~P2~~ | ~~E 内部 CBuffer~~ | ~~E1 `setupInternalCBuffers`~~ | — | — | ✅ 已删除，见 §7 |
| ➡ 已转出 | F RHI 层扩展 | F1 Compute `dispatch` + UAV | — | 已独立立项，见 §8 | ➡ `RHI-Compute-UAV-Indirect-Draw-Design-todo.md` |
| ➡ 已转出 | F | F2 Instanced / Indirect Draw | — | 同上 | ➡ 同上 |
| **P3** | G Console 后端 | G1 明确 null backend 定位并加断言日志 | 0.3d | 无 | ✅ |
| **P3** | H 清理 | H1 `reflectSamplerBindings` 下沉到基类 | 0.3d | 无 | ✅ |
| **P3** | H | H2 删除 `reset()` 中的历史注释代码 | 0.1d | 无 | ✅ |
| **P3** | H | H3 合并两份重复的 `reflectShaderAllBindings` | 0.2d | 无 | ✅ 见 §9.4 |
| **P3** | H | H4 删除 `D3D11RenderWindow::init` / `setConstantBuffer` 死代码 | 0.2d | 无 | ✅ 见 §9.5 |
| **P3** | H | H5 `swapBackBuffer` 的 VSync 可配置 | 0.2d | 无 | ✅ 见 §9.6 |

建议按 **A → B → C → D** 顺序推进：A 组是纯缺陷修复且无依赖，B 组补齐资源创建后 C 组的 blit 才有完整的源/目标类型可用，D 组依赖前面稳定的资源创建路径。

> 除 F 组（暂缓）与 Cubemap 的带缩放 blit（缺一个 cube 采样 shader）外，本文列出的任务均已实现并通过整解决方案编译。各接口的最终状态与实现位置见 `D3D11-Renderer-Backend-todo.md`。

---

## 2. 前置基础设施

这一节的东西被后面多个任务复用，先做。

### 2.1 子资源数据构建 helper

`createPixelBuffer1D/2D/3D/Cubemap` 都需要把一块扁平的 CPU 数据切成 `D3D11_SUBRESOURCE_DATA` 数组。当前 2D 路径只填了 1 个元素、Cubemap 只填了 6 个（不含 mip），mip > 1 或 array > 1 时 D3D11 会读越界或直接失败。

统一到一个 helper，加到 `D3D11Context` 的 protected 区（或独立的 `D3D11SubresourceBuilder` 工具类）：

```cpp
// T3DD3D11Context.h
protected:
    /**
     * \brief 把扁平像素数据按 [arraySlice][mipLevel] 顺序切分为 D3D11 子资源数组
     * \param [in] data : CPU 数据首地址，可为 nullptr（表示不上传初始数据）
     * \param [in] dataSize : CPU 数据字节数，用于越界校验
     * \param [in] format : 像素格式，用于计算每像素字节数
     * \param [in] width : 顶层宽度
     * \param [in] height : 顶层高度，1D 纹理传 1
     * \param [in] depth : 顶层深度，非 3D 纹理传 1
     * \param [in] mipLevels : mip 层数，至少 1
     * \param [in] arraySize : 数组层数（Cubemap 传 6 * 立方体个数），至少 1
     * \param [out] outSubresources : 输出的子资源描述数组
     * \return 数据不足以覆盖全部子资源时返回 T3D_ERR_INVALID_PARAM
     */
    TResult buildSubresourceData(const uint8_t *data, size_t dataSize,
        PixelFormat format, uint32_t width, uint32_t height, uint32_t depth,
        uint32_t mipLevels, uint32_t arraySize,
        TArray<D3D11_SUBRESOURCE_DATA> &outSubresources);
```

实现要点：

1. **D3D11 子资源索引顺序**是 `arraySlice * mipLevels + mipLevel`，循环必须外层 array、内层 mip，与 D3D11 约定一致。
2. **每级尺寸**：`mipW = max(1u, width >> level)`，height / depth 同理。
3. **行距 / 面距**：
   - `bytesPerPixel = Image::getBPP(format) / 8`
   - `SysMemPitch = mipW * bytesPerPixel`
   - `SysMemSlicePitch = SysMemPitch * mipH`（3D 纹理必填；1D/2D 填 0 即可，但填正确值无害）
4. **除零防御**：`bytesPerPixel == 0`（`E_PF_UNKNOWN` 或未登记格式）直接返回错误并打日志，不要继续走 D3D 调用。这是 A2 的根因修复点。
5. **`data == nullptr` 或 `dataSize == 0`**：返回空数组，调用方给 `CreateTextureXD` 传 `nullptr`（表示不初始化内容）。
6. **越界校验**：累加所有 subresource 的字节数，超过 `dataSize` 时返回 `T3D_ERR_INVALID_PARAM` 并打出期望/实际大小，避免 D3D11 读野内存。
7. **块压缩格式**：当前 `PixelFormat` 枚举（`T3DConstant.h:40-59`）没有 BC/ETC/ASTC，暂不处理。将来加入时行距公式改为 `max(1, ceil(mipW/4)) * blockBytes`，helper 内加一个 `isBlockCompressed(format)` 分支即可，调用方无需改动。

> 注意：helper 返回的 `TArray<D3D11_SUBRESOURCE_DATA>` 里的 `pSysMem` 指向调用方的数据。若在 RHI 线程模式下使用，必须像 `writeBuffer` 那样**先深拷贝 CPU 数据**再入队，或者在主线程侧就完成 `CreateTexture`（见各任务的具体说明）。

### 2.2 新增错误码

`T3DD3D11Error.h` 目前缺 1D / 3D 纹理和资源拷贝相关错误码。在枚举**末尾**追加（不要插在中间，避免既有日志里的数值含义漂移）：

```cpp
        /// 创建 1D 纹理失败
        T3D_ERR_D3D11_CREATE_TEXTURE1D,
        /// 创建 3D 纹理失败
        T3D_ERR_D3D11_CREATE_TEXTURE3D,
        /// 资源拷贝的源与目标不兼容（类型/尺寸/格式/采样数不匹配）
        T3D_ERR_D3D11_INCOMPATIBLE_COPY,
        /// 该后端不支持请求的操作
        T3D_ERR_D3D11_UNSUPPORTED_OPERATION,
```

### 2.3 `D3D11Mapping::get(const PixelBuffer3DDesc &)` 是空壳（任务 B1）

```180:185:source/Plugins/Renderer/Direct3D11/Base/Source/T3DD3D11Mapping.cpp
    D3D11_TEXTURE3D_DESC D3D11Mapping::get(const PixelBuffer3DDesc &src)
    {
        D3D11_TEXTURE3D_DESC dst;
        memset(&dst, 0, sizeof(dst));
        return dst;
    }
```

按 1D / 2D 的同名重载补齐即可：

```cpp
    D3D11_TEXTURE3D_DESC D3D11Mapping::get(const PixelBuffer3DDesc &src)
    {
        D3D11_TEXTURE3D_DESC dst;
        memset(&dst, 0, sizeof(dst));

        dst.Width = src.width;
        dst.Height = src.height;
        dst.Depth = src.depth;
        dst.MipLevels = src.mipmaps;
        dst.Format = get(src.format);

        return dst;
    }
```

> `D3D11_TEXTURE3D_DESC` 没有 `ArraySize` 和 `SampleDesc`（3D 纹理不支持数组和 MSAA），这与 `PixelBuffer3DDesc` 的字段集合正好吻合，无需额外映射。
>
> 顺带修一个隐患：`PixelBuffer3DDesc`（`T3DRenderBufferDesc.h:136-158`）的 `width`/`height`/`depth`/`mipmaps` **没有默认初始化值**，而 1D/2D 的对应字段都有。补上 `{0}` / `{1}`，避免栈上未初始化描述被直接用于建纹理。

---

## 3. A 组：低风险缺陷修复（P0）

### 3.1 A1 —— `setVertexShader` / `setComputeShader` 空指针解引用

**现状**：`setPixelShader` / `setHullShader` / `setDomainShader` / `setGeometryShader` 四个都有 `shader == nullptr` 的解绑分支，只有 VS 和 CS 没有：

```2164:2175:source/Plugins/Renderer/Direct3D11/Window/Source/T3DD3D11Context.cpp
    TResult D3D11Context::setVertexShader(ShaderVariant *shader)
    {
        D3D11VertexShaderPtr d3dShader = static_cast<D3D11VertexShader*>(shader->getRHIShader());
        
        auto lambda = [this](const D3D11VertexShaderPtr &d3dShader)
        {
            mD3DDeviceContext->VSSetShader(d3dShader->D3DShader, nullptr, 0);
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, d3dShader);
    }
```

`shader->getRHIShader()` 在 `shader == nullptr` 时直接崩。

**方案**：完全照抄 `setPixelShader`（T3DD3D11Context.cpp:2242）的结构，两个函数各加一个前置分支：

```cpp
    TResult D3D11Context::setVertexShader(ShaderVariant *shader)
    {
        if (shader == nullptr)
        {
            auto lambda = [this]()
            {
                mD3DDeviceContext->VSSetShader(nullptr, nullptr, 0);
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda);
        }

        // ... 原有逻辑不变 ...
    }
```

`setComputeShader`（T3DD3D11Context.cpp:2593）同理，换成 `CSSetShader`。

**验证**：对每个 stage 调用 `setXXXShader(nullptr)`，确认无崩溃且 RenderDoc 里对应 stage 显示为未绑定。

### 3.2 A2 —— `createPixelBuffer2D` 除零与子资源不完整

**现状**（T3DD3D11Context.cpp:1966-2002）：

```cpp
                    D3D11_SUBRESOURCE_DATA initDataDesc = {};
                    initDataDesc.pSysMem = buffer->getBuffer().Data;
                    initDataDesc.SysMemPitch = static_cast<uint32_t>(buffer->getBuffer().DataSize) / buffer->getDescriptor().height;
                    initDataDesc.SysMemSlicePitch = 0;
```

三个问题：

1. `height == 0` 时整数除零 → 崩溃。
2. 行距用 `DataSize / height` 推算，只在「单 mip 单 array 层」时才等于真实行距；`mipmaps > 1` 或 `arraySize > 1` 时 `DataSize` 是所有层的总和，算出来的行距完全错误。
3. `d3dDesc.MipLevels = desc.mipmaps` / `ArraySize = desc.arraySize` 已经告诉 D3D11 要建 N 个子资源，但只传了 1 个 `D3D11_SUBRESOURCE_DATA` → D3D11 会按 N 个读取，越界。

同时 `TResult ret = D3D11Mapping::get(...)` 的返回值被下一行 `ret = ENQUEUE_UNIQUE_COMMAND(...)` 覆盖，Usage/AccessMode 不匹配的错误被吞掉了。

**方案**：

1. 在**主线程侧**（进 lambda 之前）先校验并规范化描述：
   - `desc.width == 0 || desc.height == 0` → 打错误日志、返回 `nullptr`。
   - `mipLevels = max(1u, desc.mipmaps)`、`arraySize = max(1u, desc.arraySize)`，写回 `d3dDesc`（不要让 0 传进 D3D11，`MipLevels = 0` 在 D3D11 里是「自动生成完整 mip 链」的特殊语义，与引擎语义不一致）。
   - 检查 `D3D11Mapping::get(usage, accMode, ...)` 的返回值，失败直接返回 `nullptr`。
2. 把 `SysMemPitch` 的计算换成 §2.1 的 `buildSubresourceData`，用 `Image::getBPP(desc.format)` 而不是 `DataSize / height`。
3. **深拷贝 CPU 数据**：现在的 lambda 捕获 `PixelBuffer2DPtr buffer` 并在 RHI 线程读 `buffer->getBuffer().Data`。`PixelBuffer2D` 的生命周期由 SmartPtr 保住，但 `Buffer::Data` 是否仍有效取决于上层是否已把数据交给 PixelBuffer 持有（`Texture2D` 析构里有「数据已交给 PixelBuffer 则从 mDesc 摘除指针」的逻辑）。为稳妥起见，与 `writeBuffer` 一致：主线程侧深拷贝一份数据 + 构建好的 subresource 数组一起入队，lambda 末尾释放。
4. `d3dSRVDesc.Texture2D.MipLevels` 同步用规范化后的 `mipLevels`。
5. `arraySize > 1` 时 SRV 维度应该是 `D3D11_SRV_DIMENSION_TEXTURE2DARRAY` 并填 `Texture2DArray.{MostDetailedMip, MipLevels, FirstArraySlice, ArraySize}`，否则 `Texture2DArray` 类型的纹理采样会拿不到除第 0 层以外的数据。当前硬编码 `TEXTURE2D`，一并修掉。

**代码骨架**：

```cpp
    RHIPixelBuffer2DPtr D3D11Context::createPixelBuffer2D(PixelBuffer2D *buffer)
    {
        const auto &desc = buffer->getDescriptor();

        if (desc.width == 0 || desc.height == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
                "createPixelBuffer2D: invalid size [%u x %u]", desc.width, desc.height);
            return nullptr;
        }

        D3D11_USAGE d3dUsage;
        uint32_t d3dAccess = 0;
        if (T3D_FAILED(D3D11Mapping::get(buffer->getUsage(), buffer->getCPUAccessMode(), d3dUsage, d3dAccess)))
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
                "createPixelBuffer2D: invalid usage [%d] / access [%u]",
                buffer->getUsage(), buffer->getCPUAccessMode());
            return nullptr;
        }

        const uint32_t mipLevels = std::max(1u, desc.mipmaps);
        const uint32_t arraySize = std::max(1u, desc.arraySize);

        TArray<D3D11_SUBRESOURCE_DATA> subresources;
        if (T3D_FAILED(buildSubresourceData(
                static_cast<const uint8_t *>(desc.buffer.Data), desc.buffer.DataSize,
                desc.format, desc.width, desc.height, 1, mipLevels, arraySize, subresources)))
        {
            return nullptr;
        }

        // 深拷贝 CPU 数据，并把 subresources 里的 pSysMem 重定向到副本
        // （细节略，参考 writeBuffer 的 ownedBuffer 处理）

        D3D11_TEXTURE2D_DESC d3dDesc = D3D11Mapping::get(desc);
        d3dDesc.MipLevels = mipLevels;
        d3dDesc.ArraySize = arraySize;
        d3dDesc.Usage = d3dUsage;
        d3dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        d3dDesc.CPUAccessFlags = d3dAccess;
        d3dDesc.MiscFlags = 0;

        D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc = {};
        d3dSRVDesc.Format = d3dDesc.Format;
        if (arraySize > 1)
        {
            d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            d3dSRVDesc.Texture2DArray.MostDetailedMip = 0;
            d3dSRVDesc.Texture2DArray.MipLevels = mipLevels;
            d3dSRVDesc.Texture2DArray.FirstArraySlice = 0;
            d3dSRVDesc.Texture2DArray.ArraySize = arraySize;
        }
        else
        {
            d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            d3dSRVDesc.Texture2D.MostDetailedMip = 0;
            d3dSRVDesc.Texture2D.MipLevels = mipLevels;
        }

        D3D11PixelBuffer2DPtr d3dBuffer = D3D11PixelBuffer2D::create();
        // lambda 内 CreateTexture2D(&d3dDesc, subresources.empty() ? nullptr : subresources.data(), ...)
        // 失败时保持 d3dBuffer->D3DTexture == nullptr 并打日志，末尾释放深拷贝数据
        // ...
        return d3dBuffer;
    }
```

**验证**：
- `width=4, height=4, mipmaps=3, arraySize=1` 的纹理，RenderDoc 里检查 3 个 mip 的内容都正确。
- `arraySize=4, mipmaps=1`，采样第 2/3 层能取到正确内容。
- `height=0` 时返回 `nullptr` 且不崩。

### 3.3 A3 —— `createPixelBufferCubemap` 硬编码 mip / array / 采样数

**现状**（T3DD3D11Context.cpp:2038-2057）：

```cpp
            d3dDesc.MipLevels = 1;
            d3dDesc.ArraySize = PixelBufferCubemap::FACE_COUNT;
            d3dDesc.SampleDesc.Count = 1;
            d3dDesc.SampleDesc.Quality = 0;
            // ...
            d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            d3dSRVDesc.TextureCube.MipLevels = 1;
```

后果：

- 传入 `mipmaps > 1` 的天空盒 / IBL 预过滤环境贴图时，mip 链被静默丢弃，采样 `SampleLevel` 拿到的永远是 mip 0，粗糙度分级失效。
- `CubemapArray`（`Cubemap` 的 `getArraySize()` 返回 `mDesc.arraySize / 6`，说明引擎侧已支持多立方体）无法创建，`ArraySize` 被写死成 6。

**方案**：

1. 计算 `mipLevels = max(1u, desc.mipmaps)`、`cubeCount = max(1u, desc.arraySize / PixelBufferCubemap::FACE_COUNT)`。
2. `d3dDesc.MipLevels = mipLevels`、`d3dDesc.ArraySize = cubeCount * FACE_COUNT`。
3. 子资源改用 §2.1 的 helper，传 `arraySize = cubeCount * 6`、`mipLevels`，helper 内部产出的顺序（外层 array slice、内层 mip）正好匹配 D3D11 对 cubemap 的 `face * mipLevels + mip` 布局，也匹配 `PixelBufferCubemap` 注释里声明的 `+X,-X,+Y,-Y,+Z,-Z` 面序（前提是上层按面连续存放）。
4. SRV：`cubeCount == 1` 用 `TEXTURECUBE` + `TextureCube.MipLevels = mipLevels`；`cubeCount > 1` 用 `D3D11_SRV_DIMENSION_TEXTURECUBEARRAY` + `TextureCubeArray.{MostDetailedMip=0, MipLevels, First2DArrayFace=0, NumCubes=cubeCount}`。
5. `SampleDesc.Count/Quality` 保持 1/0 —— **这一项不是缺陷**。D3D11 的 `D3D11_RESOURCE_MISC_TEXTURECUBE` 不能与 MSAA 组合，硬编码是正确的。但应把注释从「mipmaps 只支持顶层数据上传」改成说明 MSAA 不适用的原因，避免后人误改。
6. 现有 lambda 里手写的 `bpp / rowPitch / faceSize` 计算整段删除，由 helper 统一负责（顺带获得 §2.1 的 bpp 为 0 防御和越界校验）。

**验证**：
- 6 面 + 4 级 mip 的 cubemap，shader 里 `SampleLevel(cube, dir, 2)` 取到正确的 mip 2 内容。
- `arraySize = 12`（2 个立方体）时 SRV 维度为 `TEXTURECUBEARRAY`，采样第 2 个立方体正常。

### 3.4 A4 —— 恢复窗口自带 DepthStencil（💬 被注释禁用）

**现状**：私有重载 `createRenderWindow(D3D11RenderWindow*, w, h, MSAACount, MSAAQuality)` 里创建深度模板缓冲和 DSV 的整段代码被注释（T3DD3D11Context.cpp:874-905），导致 `D3D11RenderWindow::D3DDSBuffer` / `D3DDSView` 恒为 `nullptr`。窗口型 RenderTarget 的 `clearDepth` / `clearDepthStencil` 被静默跳过，深度测试也无附件可用。

`resizeRenderWindow`（T3DD3D11Context.cpp:913）已经在释放这两个对象并重新调用 `createRenderWindow`，说明设计上本就期望窗口自带 DS —— 只是创建端被注释了。

**方案**：取消注释并做三处加固。

1. **格式与 MSAA 参数化，不要硬编码 `DXGI_FORMAT_D24_UNORM_S8_UINT`**。在 `D3D11RenderWindow` 上加一个 `PixelFormat DepthStencilFormat {PixelFormat::E_PF_D24_UNORM_S8_UINT}`，由 `RenderWindow` 的创建描述透传，默认值保持 D24S8 以兼容现状。
2. **MSAA 必须与 BackBuffer 一致**，否则 `OMSetRenderTargets` 会因为 RTV / DSV 采样数不匹配被 D3D11 拒绝。直接从 BackBuffer 反查而不是信参数：

```cpp
            D3D11_TEXTURE2D_DESC backBufferDesc;
            pD3DRenderWindow->D3DBackBuffer->GetDesc(&backBufferDesc);

            D3D11_TEXTURE2D_DESC dsDesc;
            memset(&dsDesc, 0, sizeof(dsDesc));
            dsDesc.Width = backBufferDesc.Width;
            dsDesc.Height = backBufferDesc.Height;
            dsDesc.MipLevels = 1;
            dsDesc.ArraySize = 1;
            dsDesc.Format = D3D11Mapping::get(pD3DRenderWindow->DepthStencilFormat);
            dsDesc.SampleDesc = backBufferDesc.SampleDesc;
            dsDesc.Usage = D3D11_USAGE_DEFAULT;
            dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            dsDesc.CPUAccessFlags = 0;
            dsDesc.MiscFlags = 0;
```

   注意用 `backBufferDesc.Width/Height` 而非传入的 `w/h`：`resizeRenderWindow` 调 `ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0)` 是让 DXGI 自己取窗口客户区尺寸，此时实际尺寸可能与传入的 `w/h` 不一致，用 BackBuffer 的实际尺寸才不会错位。
3. **DSV 显式指定 desc**，不要传 `nullptr`：`ViewDimension` 按 `SampleDesc.Count > 1 ? TEXTURE2DMS : TEXTURE2D`，`Texture2D.MipSlice = 0`。
4. 失败时对已创建的 `D3DDSBuffer` 做 `D3D_SAFE_RELEASE` 再 break，避免半初始化状态泄漏。

**验证**：
- 窗口渲染一帧后在 RenderDoc 里确认 OM 阶段有 DSV 绑定，深度缓冲内容合理。
- 调 `clearDepth` 后深度值确实被清。
- 拖拽改变窗口大小多次，无 D3D11 debug layer 警告、无泄漏（`ID3D11Debug::ReportLiveDeviceObjects`）。

---

## 4. C 组：拷贝 / Blit 家族

四个 `blit` 重载 + `copyBuffer` 里，只有 `blit(Texture*, RenderTarget*, ...)` 有实现且不完整。这一组代码高度同构，先抽公共部分，再逐个补。

### 4.1 C0 —— 抽取源 / 目标解析 helper

**现状**：`blit(Texture→RenderTarget)` 的两个分支（T3DD3D11Context.cpp:3229-3266 与 3295-3332）是**完全重复**的 38 行 switch，且 6 个 case 里有 5 个是空的：

```3229:3249:source/Plugins/Renderer/Direct3D11/Window/Source/T3DD3D11Context.cpp
                    switch (pSrc->getTextureType())
                    {
                    case TEXTURE_TYPE::TT_1D:
                        break;
                    case TEXTURE_TYPE::TT_2D:
                        {
                            Texture2D *pTex2D = static_cast<Texture2D *>(pSrc.get());
                            D3D11PixelBuffer2D *pD3DPixelBuffer = static_cast<D3D11PixelBuffer2D*>(pTex2D->getPixelBuffer()->getRHIResource().get());
                            pD3DSrc = pD3DPixelBuffer->D3DTexture;
                            pD3DSRV = pD3DPixelBuffer->D3DSRView;
                        }
                        break;
                    case TEXTURE_TYPE::TT_2D_ARRAY:
                        break;
                    case TEXTURE_TYPE::TT_3D:
                        break;
                    case TEXTURE_TYPE::TT_CUBE:
                        break;
                    case TEXTURE_TYPE::TT_CUBE_ARRAY:
                        break;
```

空 case 落下来后 `pD3DSrc` / `pD3DSRV` 都是 `nullptr`，直接进 `CopyResource(dst, nullptr)` —— D3D11 debug layer 报错，release 下行为未定义。

**方案**：定义一个统一的「blit 端点」结构和两个解析函数，放在 `D3D11Context` 的 protected 区：

```cpp
    /**
     * \brief blit 操作的一个端点（源或目标）解析结果
     */
    struct BlitEndpoint
    {
        /// 用于 CopyResource / CopySubresourceRegion 的资源
        ID3D11Resource              *Resource {nullptr};
        /// 用于全屏四边形路径的源视图；不可采样时为 nullptr
        ID3D11ShaderResourceView    *SRView {nullptr};
        /// 用于全屏四边形路径的目标视图；不可作为 RT 时为 nullptr
        ID3D11RenderTargetView      *RTView {nullptr};
        /// 目标深度模板视图，可为 nullptr
        ID3D11DepthStencilView      *DSView {nullptr};
        /// 尺寸与格式，用于兼容性校验
        uint32_t                     Width {0};
        uint32_t                     Height {0};
        uint32_t                     Depth {1};
        DXGI_FORMAT                  Format {DXGI_FORMAT_UNKNOWN};
        uint32_t                     SampleCount {1};
    };

    /// 从 Texture 解析出 blit 端点；needResolve 为 true 时对 MSAA 源先做 ResolveSubresource
    TResult resolveBlitEndpoint(Texture *tex, bool asSource, BlitEndpoint &out);
    /// 从 RenderTarget 解析出 blit 端点（窗口取 BackBuffer/RTView，纹理取首个颜色附件）
    TResult resolveBlitEndpoint(RenderTarget *rt, bool asSource, BlitEndpoint &out);
    /// 校验两个端点能否用 CopyResource / CopySubresourceRegion 直接拷贝
    bool isDirectCopyCompatible(const BlitEndpoint &src, const BlitEndpoint &dst, bool regionCopy);
```

`resolveBlitEndpoint(Texture*)` 内的类型分派表：

| `TEXTURE_TYPE` | RHI 资源类型 | `Resource` | `SRView` | 说明 |
|---------------|-------------|-----------|---------|------|
| `TT_1D` | `D3D11PixelBuffer1D` | `D3DTexture` | `D3DSRView` | 依赖 B4 完成 |
| `TT_2D` | `D3D11PixelBuffer2D` | `D3DTexture` | `D3DSRView` | 已有 |
| `TT_2D_ARRAY` | `D3D11PixelBuffer2D` | `D3DTexture` | `D3DSRView` | 与 2D 同一个 RHI 类，SRV 维度不同（A2 修复后正确） |
| `TT_3D` | `D3D11PixelBuffer3D` | `D3DTexture` | `D3DSRView` | 依赖 B2 / B3 完成 |
| `TT_CUBE` / `TT_CUBE_ARRAY` | `D3D11PixelBufferCubemap` | `D3DTexture` | `D3DSRView` | 整资源拷贝可用；区域拷贝的全屏四边形路径需要专门的 cube 采样 shader，先返回 `T3D_ERR_D3D11_UNSUPPORTED_OPERATION` |
| `TT_RENDER_TEXTURE` | `D3D11PixelBuffer2D` | MSAA 时 `D3DResolveTex`，否则 `D3DTexture` | `D3DSRView` | 保留现有的 `ResolveSubresource` 逻辑 |

统一的兜底：`default` 分支和「取到的 RHI 资源为 nullptr」都必须打 `T3D_LOG_ERROR` 并返回 `T3D_ERR_D3D11_UNSUPPORTED_OPERATION` / `T3D_ERR_INVALID_POINTER`，**不能再静默落穿**。

`resolveBlitEndpoint(RenderTarget*)`：

- `E_RT_WINDOW`：`Resource = D3DBackBuffer`、`RTView = D3DRTView`、`DSView = D3DDSView`、`SRView = nullptr`（见 §0.3 的 SwapChain 限制）。尺寸/格式从 `D3DBackBuffer->GetDesc()` 取。
- `E_RT_TEXTURE`：走 `getRenderTexture(0)->getPixelBuffer()->getRHIResource()` 转 `D3D11PixelBuffer2D`；`RTView = D3DRTView`、`DSView` 从 `getDepthStencil()` 的 pixel buffer 取。MRT 场景下 blit 语义上只处理 attachment 0，需在文档和日志里说明。

`isDirectCopyCompatible` 的判定（对应 §0.3 的表）：

```
regionCopy == false（CopyResource）:
    src.Width == dst.Width && src.Height == dst.Height && src.Depth == dst.Depth
    && src.Format == dst.Format
    && src.SampleCount == dst.SampleCount
regionCopy == true（CopySubresourceRegion）:
    src.Format == dst.Format
    && src.SampleCount == 1 && dst.SampleCount == 1
```

### 4.2 C1 —— 补全 `blit(Texture → RenderTarget)`

用 C0 的 helper 重写，两个 `RenderTarget::Type` 分支合并成一条路径：

```cpp
    TResult D3D11Context::blit(Texture *src, RenderTarget *dst,
        const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        auto lambda = [this](const TexturePtr &pSrc, const RenderTargetPtr &pDst,
            const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
        {
            BlitEndpoint srcEp, dstEp;
            TResult ret = resolveBlitEndpoint(pSrc.get(), true, srcEp);
            if (T3D_FAILED(ret)) { return ret; }
            ret = resolveBlitEndpoint(pDst.get(), false, dstEp);
            if (T3D_FAILED(ret)) { return ret; }

            return doBlit(srcEp, dstEp, srcOffset, size, dstOffset);
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, TexturePtr(src), RenderTargetPtr(dst),
            srcOffset, size, dstOffset);
    }
```

> 生命周期上传 `RenderTargetPtr` 比现在传 `D3D11RenderWindowPtr` / `D3D11PixelBuffer2DPtr` 更安全：`RenderTarget` 持有 `RenderWindowPtr` / `RenderTexturePtr`，能保住整条引用链；同时 helper 在 RHI 线程内解析，避免主线程侧解析出的裸 COM 指针在执行时已失效。

统一的 `doBlit`（新增 protected 方法）承载决策：

```
if size == Vector3::ZERO:                         // 整资源
    if isDirectCopyCompatible(src, dst, false):
        CopyResource(dst.Resource, src.Resource)
    elif src.SampleCount > 1 && dst.SampleCount == 1 && 尺寸相同:
        ResolveSubresource(...)
    elif src.SRView != nullptr && dst.RTView != nullptr:
        blitRegion(src.SRView, dst.RTView, dst.DSView, 0, {dst.Width, dst.Height, 1}, 0)  // 缩放/跨格式
    else:
        T3D_LOG_ERROR + T3D_ERR_D3D11_INCOMPATIBLE_COPY
else:                                              // 区域
    if isDirectCopyCompatible(src, dst, true):
        D3D11_BOX box = {srcOffset.x, srcOffset.y, srcOffset.z,
                         srcOffset.x + size.x, srcOffset.y + size.y, max(1, srcOffset.z + size.z)}
        CopySubresourceRegion(dst.Resource, 0, dstOffset.x, dstOffset.y, dstOffset.z,
                              src.Resource, 0, &box)
    elif src.SRView != nullptr && dst.RTView != nullptr:
        blitRegion(src.SRView, dst.RTView, dst.DSView, srcOffset, size, dstOffset)
    else:
        T3D_LOG_ERROR + T3D_ERR_D3D11_INCOMPATIBLE_COPY
```

两点与现有实现的差异需要留意：

1. 现在的实现是「`size == ZERO` 就 `CopyResource`」，不做任何兼容性检查。改成先校验，不兼容时优先走 `blitRegion`（能跨格式、能缩放），只有连 SRV/RTV 都拿不到时才报错。
2. 现有的 `blitRegion` **只按 dst 的 viewport 定位，完全忽略 `srcOffset`** —— 顶点缓冲是固定的全屏四边形 UV（`setupBlitQuad`，T3DD3D11Context.cpp:246），所以源始终是整张图被拉伸到 dst 的 `size` 区域。要真正支持 `srcOffset`，需要在 `blitRegion` 里增加一个「源 UV 矩形」的常量缓冲或改成动态顶点缓冲（`D3D11_USAGE_DYNAMIC` + `Map(WRITE_DISCARD)` 每次写 4 个顶点的 UV）。**建议作为 C1 的一部分修掉**，否则 `srcOffset` 参数在非直接拷贝路径下是个谎言。

### 4.3 C2 —— 实现 `blit(RenderTarget → RenderTarget)`

`resolveBlitEndpoint(RenderTarget*, true, ...)` + `doBlit` 直接复用，结构与 C1 完全一致。

**必须单独说明的限制**：源是 `E_RT_WINDOW` 时 `SRView == nullptr`（§0.3），因此：

- 尺寸/格式/采样数完全一致 → `CopyResource` 可用（窗口 → 窗口、窗口 → 同规格 RenderTexture 都 OK）。
- 需要缩放或跨格式 → 无路可走，返回 `T3D_ERR_D3D11_INCOMPATIBLE_COPY` 并在日志里明确写出原因（"swapchain backbuffer has no SRV, cannot scale-blit from window"）。

如果后续确实需要「把窗口内容缩放到别处」（截图缩略图、编辑器画中画等），有两个选项，建议在需求出现时再选：

| 选项 | 做法 | 代价 |
|-----|------|------|
| 改 SwapChain | `DXGI_SWAP_CHAIN_DESC::BufferUsage` 加 `DXGI_USAGE_SHADER_INPUT`，BackBuffer 就能建 SRV | 部分驱动/全屏独占模式下有兼容性风险，需实测 |
| 中转纹理 | 先 `CopyResource` 到一张同规格的临时 RenderTexture，再从它 `blitRegion` | 多一次全屏拷贝，但零兼容性风险 |

### 4.4 C3 / C4 —— `blit(RenderTarget → Texture)` 与 `blit(Texture → Texture)`

C0 的 helper 做好后这两个是纯组合，各约 15 行：

```cpp
    TResult D3D11Context::blit(RenderTarget *src, Texture *dst, ...)
    {
        auto lambda = [this](const RenderTargetPtr &pSrc, const TexturePtr &pDst, ...)
        {
            BlitEndpoint srcEp, dstEp;
            TResult ret = resolveBlitEndpoint(pSrc.get(), true, srcEp);
            if (T3D_FAILED(ret)) { return ret; }
            ret = resolveBlitEndpoint(pDst.get(), false, dstEp);
            if (T3D_FAILED(ret)) { return ret; }
            return doBlit(srcEp, dstEp, srcOffset, size, dstOffset);
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, RenderTargetPtr(src), TexturePtr(dst), ...);
    }
```

`asSource = false` 时 `resolveBlitEndpoint(Texture*)` 需要额外校验：

- 目标纹理的 `Usage` 不能是 `Usage::kImmutable`（D3D11 拒绝写入 IMMUTABLE 资源），否则返回 `T3D_ERR_D3D11_INVALID_USAGE`。
- 目标若要走 `blitRegion` 路径，必须有 `D3DRTView` —— 普通 `createPixelBuffer2D` 建出来的纹理 `BindFlags` 只有 `D3D11_BIND_SHADER_RESOURCE`，**没有 RTV**。所以「Texture 作为 blit 目标」只支持直接拷贝路径；需要缩放时报 `T3D_ERR_D3D11_INCOMPATIBLE_COPY`。这一条要写进 `RHIContext::blit` 的接口注释里，避免上层误用。

### 4.5 C5 —— 实现 `copyBuffer`

**签名**：`TResult copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset = 0, size_t size = 0, size_t dstOffset = 0)`

**方案**：结构直接参考 `writeBuffer`（T3DD3D11Context.cpp:3378）里那段成熟的「按 `getRHIResource()->getResourceType()` switch 出 `ID3D11Resource*`」逻辑 —— 把它抽成一个 helper 复用，避免第三份拷贝：

```cpp
    /// 从 RenderBuffer 取出底层 D3D11 资源；类型不支持时返回 nullptr
    ID3D11Resource *getD3DResource(RenderBuffer *buffer);
```

`copyBuffer` 实现：

```cpp
    TResult D3D11Context::copyBuffer(RenderBuffer *src, RenderBuffer *dst,
        size_t srcOffset, size_t size, size_t dstOffset)
    {
        if (src == nullptr || dst == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "copyBuffer: null buffer");
            return T3D_ERR_INVALID_POINTER;
        }

        if (dst->getUsage() == Usage::kImmutable)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "copyBuffer: destination is immutable");
            return T3D_ERR_D3D11_INVALID_USAGE;
        }

        const size_t copySize = (size == 0) ? src->getBufferSize() - srcOffset : size;

        if (srcOffset + copySize > src->getBufferSize()
            || dstOffset + copySize > dst->getBufferSize())
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
                "copyBuffer: range out of bounds. src[%zu+%zu/%zu] dst[%zu+%zu/%zu]",
                srcOffset, copySize, src->getBufferSize(),
                dstOffset, copySize, dst->getBufferSize());
            return T3D_ERR_INVALID_PARAM;
        }

        auto lambda = [this](const RenderBufferPtr &pSrc, const RenderBufferPtr &pDst,
            size_t srcOffset, size_t copySize, size_t dstOffset)
        {
            ID3D11Resource *pD3DSrc = getD3DResource(pSrc.get());
            ID3D11Resource *pD3DDst = getD3DResource(pDst.get());
            if (pD3DSrc == nullptr || pD3DDst == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "copyBuffer: unsupported buffer type");
                return T3D_ERR_D3D11_UNSUPPORTED_OPERATION;
            }

            if (srcOffset == 0 && dstOffset == 0 && copySize == pSrc->getBufferSize()
                && pSrc->getBufferSize() == pDst->getBufferSize())
            {
                mD3DDeviceContext->CopyResource(pD3DDst, pD3DSrc);
            }
            else
            {
                // 线性 buffer 的 D3D11_BOX：left/right 是字节偏移，其余维度必须是 0/1
                D3D11_BOX box = {};
                box.left = static_cast<UINT>(srcOffset);
                box.right = static_cast<UINT>(srcOffset + copySize);
                box.top = 0;
                box.bottom = 1;
                box.front = 0;
                box.back = 1;

                mD3DDeviceContext->CopySubresourceRegion(pD3DDst, 0,
                    static_cast<UINT>(dstOffset), 0, 0, pD3DSrc, 0, &box);
            }

            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, RenderBufferPtr(src), RenderBufferPtr(dst),
            srcOffset, copySize, dstOffset);
    }
```

要点：

1. **`D3D11_BOX` 对线性 buffer 的约定**是 `top=0, bottom=1, front=0, back=1`，`left/right` 为字节偏移。填错（比如 `bottom=0`）会导致 0 字节拷贝且不报错。
2. **`ConstantBuffer` 有 16 字节对齐要求**：`CopySubresourceRegion` 到常量缓冲时 `dstOffset` 和 `copySize` 都应是 16 的倍数。加一条校验并在不满足时报错。
3. **PixelBuffer 不该走这个接口**。`copyBuffer` 语义是线性缓冲拷贝，纹理请用 `blit`。`getD3DResource` 遇到 `kPixelBufferXD` 时应打警告并返回错误，而不是让 `D3D11_BOX` 的线性解释套到 2D 纹理上（那会拷出错乱的结果）。
4. **同资源自拷贝**：`pD3DSrc == pD3DDst` 且区间重叠时 D3D11 行为未定义，加校验直接拒绝。

**验证**：VB → VB 全量拷贝、带偏移的区间拷贝，`Map(READ)` 回读比对内容；CB 非 16 对齐时返回错误。

---

## 5. B 组：资源创建补全

### 5.1 B2 —— `D3D11PixelBuffer3D` 类补全

**现状**（`T3DD3D11RenderBuffer.h:132-139`）：

```132:139:source/Plugins/Renderer/Direct3D11/Window/Include/T3DD3D11RenderBuffer.h
    class D3D11PixelBuffer3D : public RHIPixelBuffer3D
    {
    public:
        void *getNativeObject() const override;
        
        ID3D11Texture2D             *D3DTexture {nullptr};
        ID3D11ShaderResourceView    *D3DSRView {nullptr};
    };
```

三个问题：与同族类相比缺 `create()` 静态工厂、缺析构（COM 对象永久泄漏）、`D3DTexture` 类型是 `ID3D11Texture2D*` 而非 `ID3D11Texture3D*`。

**方案**：对齐 `D3D11PixelBuffer2D` 的形式：

```cpp
    class D3D11PixelBuffer3D : public RHIPixelBuffer3D
    {
    public:
        static D3D11PixelBuffer3DPtr create();

        ~D3D11PixelBuffer3D() override;

        void *getNativeObject() const override;

        ID3D11Texture3D             *D3DTexture {nullptr};
        ID3D11ShaderResourceView    *D3DSRView {nullptr};

    protected:
        D3D11PixelBuffer3D() = default;
    };
```

`.cpp` 里补 `create()`（`return T3D_NEW D3D11PixelBuffer3D()`）和析构（`D3D_SAFE_RELEASE(D3DSRView); D3D_SAFE_RELEASE(D3DTexture);`，与 2D 版本一致的释放顺序：先视图后资源）。

**连带影响**：`writeBuffer`（T3DD3D11Context.cpp:3422）和 `setPixelBuffers`（T3DD3D11Context.cpp:3623）都已经引用了 `D3D11PixelBuffer3D::D3DTexture` / `D3DSRView`。改类型后 `D3DTexture` 赋给 `ID3D11Resource*` 仍然合法（`ID3D11Texture3D` 也派生自 `ID3D11Resource`），编译不会断。确认一遍即可。

### 5.2 B3 —— 实现 `createPixelBuffer3D`

**现状**：`return nullptr;`（T3DD3D11Context.cpp:2018）。上层 `PixelBuffer3D::onLoad()` 会把 `mRHIResource` 设成 `nullptr`，任何使用 3D 纹理的地方都会空指针。

**方案**：结构与 A2 修好后的 `createPixelBuffer2D` 同构，差异只在维度和不支持的特性：

```cpp
    RHIPixelBuffer3DPtr D3D11Context::createPixelBuffer3D(PixelBuffer3D *buffer)
    {
        const auto &desc = buffer->getDescriptor();

        if (desc.width == 0 || desc.height == 0 || desc.depth == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
                "createPixelBuffer3D: invalid size [%u x %u x %u]",
                desc.width, desc.height, desc.depth);
            return nullptr;
        }

        D3D11_USAGE d3dUsage;
        uint32_t d3dAccess = 0;
        if (T3D_FAILED(D3D11Mapping::get(buffer->getUsage(), buffer->getCPUAccessMode(),
                d3dUsage, d3dAccess)))
        {
            return nullptr;
        }

        const uint32_t mipLevels = std::max(1u, desc.mipmaps);

        TArray<D3D11_SUBRESOURCE_DATA> subresources;
        if (T3D_FAILED(buildSubresourceData(
                static_cast<const uint8_t *>(desc.buffer.Data), desc.buffer.DataSize,
                desc.format, desc.width, desc.height, desc.depth,
                mipLevels, 1, subresources)))
        {
            return nullptr;
        }

        D3D11_TEXTURE3D_DESC d3dDesc = D3D11Mapping::get(desc);   // 依赖 B1
        d3dDesc.MipLevels = mipLevels;
        d3dDesc.Usage = d3dUsage;
        d3dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        d3dDesc.CPUAccessFlags = d3dAccess;
        d3dDesc.MiscFlags = 0;

        D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc = {};
        d3dSRVDesc.Format = d3dDesc.Format;
        d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
        d3dSRVDesc.Texture3D.MostDetailedMip = 0;
        d3dSRVDesc.Texture3D.MipLevels = mipLevels;

        // 深拷贝数据 + 入队 CreateTexture3D / CreateShaderResourceView（与 2D 同构）
        // ...
    }
```

3D 纹理特有约束，要在实现里体现：

1. **不支持 array**：`PixelBuffer3DDesc` 本身没有 `arraySize`，helper 传 `arraySize = 1`。
2. **不支持 MSAA**：`D3D11_TEXTURE3D_DESC` 无 `SampleDesc` 字段，天然满足。
3. **`SysMemSlicePitch` 必填**：3D 纹理的 subresource 数据是按 slice 连续排布的，helper 必须给出 `SysMemPitch * mipH`，否则 D3D11 会按 0 处理导致所有 slice 读同一份数据。
4. **每级 depth 也要减半**：`mipD = max(1u, depth >> level)`。helper 里已统一处理。

**验证**：建一个 8×8×8、3 级 mip 的 3D 纹理，shader 里 `Sample(tex3d, uvw)` 采样；RenderDoc 里逐 slice 检查内容与 mip 尺寸。

### 5.3 B4 —— 实现 `createPixelBuffer1D`

**现状**（T3DD3D11Context.cpp:1923）：

```cpp
    RHIPixelBuffer1DPtr D3D11Context::createPixelBuffer1D(PixelBuffer1D *buffer)
    {
        D3D11PixelBuffer1DPtr d3dBuffer = D3D11PixelBuffer1D::create();
        return d3dBuffer;
    }
```

返回的对象里 `D3DTexture` / `D3DSRView` 全是 `nullptr` —— 比返回 `nullptr` 更危险，因为上层 `getRHIResource()` 非空、能通过所有空检查，直到真正 `PSSetShaderResources(nullptr)` 才出问题。

**方案**：与 B3 同构，差异：

- 用 `D3D11Mapping::get(const PixelBuffer1DDesc &)`（已实现，`T3DD3D11Mapping.cpp:147`）
- `CreateTexture1D`，失败码用新增的 `T3D_ERR_D3D11_CREATE_TEXTURE1D`
- helper 传 `height = 1, depth = 1`
- `arraySize = max(1u, desc.arraySize)`；`arraySize > 1` 时 SRV 用 `D3D11_SRV_DIMENSION_TEXTURE1DARRAY` + `Texture1DArray.{MostDetailedMip, MipLevels, FirstArraySlice, ArraySize}`，否则 `TEXTURE1D` + `Texture1D.{MostDetailedMip, MipLevels}`
- 1D 纹理的 `SysMemPitch` 会被 D3D11 忽略，但 helper 统一填 `mipW * bpp` 无害

**验证**：256×1 的渐变查找表（LUT）纹理，shader 采样值与 CPU 侧数据一致。

---

## 6. D 组：Resize（P2）

### 6.1 D1 —— `resizeRenderTexture`

**现状**（T3DD3D11Context.cpp:949）：`return T3D_OK;` —— 谎报成功，RenderTexture 尺寸永远不变。Vulkan 后端也是同样的 stub（`T3DVKContext.cpp:1298`），说明这是整个 RHI 层的共性缺口。

**先看清一件事**：`resizeRenderTexture` / `resizeRenderTarget` **不是 `RHIContext` 的虚接口**，只声明在 `T3DD3D11Context.h:77/86` 和 `T3DVKContext.h:58/63` 这两个具体后端类上；全仓搜索也找不到任何调用点。也就是说它们目前是**纯死代码**。所以 D1/D2 的第一步不是写实现，而是先决定接口归属：

| 选项 | 做法 |
|-----|------|
| **提升到 RHI 层（推荐）** | 在 `RHIContext` 加 `virtual TResult resizeRenderTexture(RenderTexture*, uint32_t, uint32_t) = 0` 和 `resizeRenderTarget(...)`，各后端 override。这样 `RenderTexture::resize()` 才有统一的调用路径 |
| 保持后端私有 | 由 `D3D11RenderWindow::resize` 那样的后端专属路径触发（参考 `T3DD3D11RenderWindow.cpp:137` 调 `D3D11_CONTEXT->resizeRenderWindow`）。缺点是引擎层无法统一驱动 |

下面的实现按「提升到 RHI 层」写。

**核心难点**：D3D11 的纹理尺寸不可变，resize 必然是「销毁 + 重建」。但 `RenderResource::mRHIResource` 是 **protected**：

```88:109:source/Core/Include/Render/T3DRenderResource.h
        virtual Type getType() const = 0;

        /// 返回已创建的底层 RHI 资源；未加载时为 nullptr
        RHIResourcePtr getRHIResource() const { return mRHIResource; }

    protected:
        RenderResource() = default;
        // ...
        /// 底层 RHI 资源指针，onLoad 创建、onUnload 置空
        RHIResourcePtr mRHIResource {nullptr};
```

RHI 后端拿不到写权限，无法把新建的 `D3D11PixelBuffer2D` 替换进去。

**方案：原地重建（推荐）**

不替换 wrapper 对象，而是让**同一个 `D3D11PixelBuffer2D` 实例**换掉内部的 COM 对象。这样所有持有 `RHIResourcePtr` 的上层（`RenderTarget`、材质的纹理槽）都不需要感知变化，指针稳定。

1. 在 `D3D11Context` 加一个私有方法，把 `createRenderTexture` 里建 texture / RTV / SRV / DSV 的那段逻辑抽出来，参数化为「往一个已存在的 `D3D11PixelBuffer2D` 上填充资源」：

```cpp
    /// 按 desc 为 d3dBuffer 创建（或重建）纹理与视图；调用前需先 releaseRenderTextureResources
    TResult buildRenderTextureResources(const PixelBuffer2DDesc &desc, Usage usage,
        uint32_t accessMode, D3D11PixelBuffer2D *d3dBuffer);
    /// 释放 d3dBuffer 上的全部 COM 对象
    void releaseRenderTextureResources(D3D11PixelBuffer2D *d3dBuffer);
```

   `createRenderTexture`（T3DD3D11Context.cpp:582）改为「create 空 wrapper + 调 `buildRenderTextureResources`」，与 resize 共用同一份代码，避免两处逻辑漂移。

2. `resizeRenderTexture` 的实现：

```cpp
    TResult D3D11Context::resizeRenderTexture(RenderTexture *rt, uint32_t w, uint32_t h)
    {
        if (rt == nullptr || w == 0 || h == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
                "resizeRenderTexture: invalid args [%u x %u]", w, h);
            return T3D_ERR_INVALID_PARAM;
        }

        // 描述由 RenderTexture::resize 在调用本函数前更新，这里只做一致性校验
        T3D_ASSERT(rt->getWidth() == w && rt->getHeight() == h);

        PixelBuffer2D *pixelBuffer = static_cast<PixelBuffer2D *>(rt->getPixelBuffer());
        if (pixelBuffer == nullptr || pixelBuffer->getRHIResource() == nullptr)
        {
            // 还没 load，改完描述即可，后续 onLoad 会按新尺寸创建
            return T3D_OK;
        }

        auto lambda = [this](const PixelBuffer2DPtr &pixelBuffer,
            const D3D11PixelBuffer2DPtr &d3dBuffer)
        {
            releaseRenderTextureResources(d3dBuffer.get());
            return buildRenderTextureResources(pixelBuffer->getDescriptor(),
                pixelBuffer->getUsage(), pixelBuffer->getCPUAccessMode(), d3dBuffer.get());
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, PixelBuffer2DPtr(pixelBuffer),
            smart_pointer_cast<D3D11PixelBuffer2D>(pixelBuffer->getRHIResource()));
    }
```

3. **`RenderTexture` 需要暴露 resize 入口**。`Texture2D::setWidth` / `setHeight`（`T3DTexture.h:327/331`）在 `protected:` 区（该区从 `T3DTexture.h:284` 开始），RHI 后端**改不了引擎侧的描述**。所以上面 `resizeRenderTexture` 里 `rt->setWidth(w)` 那两行实际写不出来，正确的分层是：

   给 `RenderTexture` 加一个 public 方法：

```cpp
        /**
         * \brief 改变渲染纹理尺寸，同步更新描述与 GPU 资源
         * \param [in] width : 新宽度
         * \param [in] height : 新高度
         * \return 调用成功返回 T3D_OK
         */
        TResult resize(uint32_t width, uint32_t height);
```

   它先改自己的 `mDesc`，再转调 `T3D_AGENT.getActiveRHIContext()->resizeRenderTexture(this, w, h)`。后端只负责「按当前描述重建 RHI 资源」这一步，`resizeRenderTexture` 里不再有 `setWidth` / `setHeight`，改为直接读 `rt->getWidth()` / `getHeight()` 做一致性断言。这个方向也让 GL / Vulkan / Metal 后端能共用同一套引擎侧逻辑（见 §11）。

**关键风险**：重建期间旧的 RTV/SRV 可能还绑在 device context 上。必须在释放前解绑：

```cpp
    // releaseRenderTextureResources 开头
    ID3D11ShaderResourceView *nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
    mD3DDeviceContext->PSSetShaderResources(0, ARRAYSIZE(nullSRVs), nullSRVs);
    mD3DDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
```

   粗暴但可靠。更精细的做法是记录绑定槽位，但 resize 是低频操作，不值得为此增加状态跟踪。

**替代方案（不推荐，供对比）**：给 `RenderResource` 加 `protected: void setRHIResource(RHIResourcePtr)` 并让 `PixelBuffer2D` 暴露 `recreate()`。这样后端可以整体换 wrapper，但会让所有持有旧 `RHIResourcePtr` 的地方拿到失效对象，需要全局审计引用点，风险远大于原地重建。

### 6.2 D2 —— `resizeRenderTarget`

**现状**（T3DD3D11Context.cpp:956）：`return T3D_OK;`。Vulkan 后端的同名函数留了 `// TODO: dispatch to resizeRenderWindow or resizeRenderTexture` 注释，意图很明确。

**方案**：纯分派 + 遍历所有附件：

```cpp
    TResult D3D11Context::resizeRenderTarget(RenderTarget *rt, uint32_t w, uint32_t h)
    {
        if (rt == nullptr || w == 0 || h == 0)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        TResult ret = T3D_OK;

        if (rt->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            D3D11RenderWindow *rw = static_cast<D3D11RenderWindow *>(
                rt->getRenderWindow()->getRHIRenderWindow());
            ret = resizeRenderWindow(rw, w, h);
        }
        else
        {
            // 所有颜色附件必须同尺寸，逐个 resize
            for (uint32_t i = 0; i < rt->getNumOfRenderTextures(); ++i)
            {
                ret = resizeRenderTexture(rt->getRenderTexture(i), w, h);
                if (T3D_FAILED(ret)) { return ret; }
            }
        }

        // 深度模板附件（窗口型也可能挂了外部 RenderTexture 作为 DS）
        if (rt->getDepthStencil() != nullptr)
        {
            ret = resizeRenderTexture(rt->getDepthStencil(), w, h);
        }

        return ret;
    }
```

注意窗口型分支也要处理 `getDepthStencil()` —— A4 完成前，窗口的深度模板完全靠外挂 RenderTexture 提供，漏了它会导致 resize 后 RTV/DSV 尺寸不匹配，`OMSetRenderTargets` 直接失败。

---

## 7. E 组：`setupInternalCBuffers` —— ✅ 已删除

内建 `CBufferPerFrame` / `CBufferPerDraw` 是早期设计残留。引擎的常量缓冲现在统一走 `createConstantBuffer` + `setVSConstantBuffers` 等通用路径由材质系统管理，这套绕过 RHI 抽象的裸 D3D11 内建 CBuffer 已无用途，故直接删除而非启用。

已删除的内容：

| 位置 | 内容 |
|-----|------|
| `T3DD3D11Context.h` | `void setupInternalCBuffers();` 声明 |
| `T3DD3D11Context.cpp` `init()` | `setupInternalCBuffers();` 调用 |
| `T3DD3D11Context.cpp` | `setupInternalCBuffers()` 定义（37 行全注释的函数体） |
| `T3DD3D11Context.cpp` | 注释版 `setWorldTransform()`，其中引用的 `mCBufferPerDraw` / `mPerDrawCBuffer` 早已从头文件移除 |

`mPerFrameCBuffer` / `mPerDrawCBuffer` 成员和 `CBufferPerFrame` / `CBufferPerDraw` 结构体在本次改动前就已不在头文件中，无需处理。

**遗留的相邻死代码**：`D3D11Context::setConstantBuffer(uint32_t startSlot, const Buffer &, ID3D11Buffer *)`（单缓冲版本，`T3DD3D11Context.cpp:409`）唯一的调用点就是上面那段注释版 `setWorldTransform`，删除后已无任何调用者。它本身是一段可用的 `Map(WRITE_DISCARD)` + `VSSetConstantBuffers` 实现，与复数版 `setConstantBuffers` 职责重叠但语义不同（前者写内容后绑定，后者只绑定）。**建议一并删除**，若将来需要「写入并绑定单个 CBuffer」的能力，走 `writeBuffer` + `setVSConstantBuffers` 组合即可，不需要这个后端私有的旁路。

---

## 8. F 组：RHI 层缺失接口 —— ➡ 已独立立项

> **本节已转出**，完整设计见 [`doc/todo/RHI-Compute-UAV-Indirect-Draw-Design-todo.md`](RHI-Compute-UAV-Indirect-Draw-Design-todo.md)。
>
> 下面保留缺口现状与范围边界的记录，作为该立项的背景说明；**不要按本节内容做零散改动**，具体方案以立项文档为准。

### 8.1 缺口现状

这两项不是 D3D11 后端的实现缺口，而是 `RHIContext` 抽象本身没有对应接口：

| 缺口 | 现状 |
|-----|------|
| Compute `dispatch` | D3D11 后端已有 `setComputeShader` / `setCSConstantBuffers` / `setCSPixelBuffers` / `setCSSamplers`，但 RHI 层**没有任何办法真正发起一次 compute 派发**，整条 CS 接口链是断头路 |
| UAV（`ID3D11UnorderedAccessView`） | RHI 层完全没有 UAV 概念，compute 无处写输出 |
| Instanced Draw | 大批量重复几何体只能逐个 draw call |
| Indirect Draw | 依赖 UAV / 结构化缓冲，当前无基础设施 |

### 8.2 为什么不能只加 `dispatch`

`ID3D11DeviceContext::Dispatch` 本身的封装是平凡的（一个 `ENQUEUE_UNIQUE_COMMAND` 包一行调用），但**加了也用不了** —— compute shader 的输出必须写进 UAV，没有 UAV 支持时 `dispatch` 只是个空转的接口。而 UAV 支持牵动的面很宽：

1. `PixelBuffer2DDesc` / `VertexBufferDesc` 需要增加 `unorderedAccess` 标记，创建时给 `BindFlags` 加 `D3D11_BIND_UNORDERED_ACCESS`。
2. `D3D11PixelBufferXD` / `D3D11VertexBuffer` 需要增加 `ID3D11UnorderedAccessView *D3DUAView` 成员及创建逻辑。
3. RHI 层需要新增 `setCSUnorderedAccessViews(uint32_t startSlot, const RenderBuffers &buffers)`，D3D11 侧调 `CSSetUnorderedAccessViews`。
4. `dispatch` 后必须解绑 UAV（`CSSetUnorderedAccessViews` 传 nullptr），否则同一资源无法作为 SRV 被后续 draw 读取 —— D3D11 会打 HAZARD 警告并把 SRV 绑定静默置空。
5. `RHIContext` 的新纯虚函数要求 **GL4 / GLES3 / Vulkan / Metal / Reference3D 全部补 override**（不支持的返回 `T3D_ERR_NOT_IMPLEMENT`，`T3DCommonErrorDef.h:72`），改动面覆盖所有后端。
6. 结构化缓冲（`StructuredBuffer` / `ByteAddressBuffer`）在 RHI 层也不存在，而这是 compute 最常用的输入输出载体 —— 只有纹理 UAV 的 compute 能力很有限。

Instanced Draw 的配套需求同理：实例数据通常放在第二个顶点缓冲（`D3D11_INPUT_PER_INSTANCE_DATA`），需要 `VertexAttribute` 支持 per-instance 语义和 `InstanceDataStepRate`，改动落在 `createVertexDeclaration` 和顶点声明的描述结构上。Indirect Draw 则直接依赖 UAV / 结构化缓冲。

### 8.3 结论

**「Compute + UAV + 结构化缓冲 + Instanced/Indirect Draw」应作为一个完整特性单独立项**，一次性设计好 RHI 抽象再落地到各后端。先合一个孤立的 `dispatch` 或 `drawIndexedInstanced` 只会在抽象层留下无人调用的接口，并在真正设计 UAV 时被迫返工。

该立项已落地为 [`doc/todo/RHI-Compute-UAV-Indirect-Draw-Design-todo.md`](RHI-Compute-UAV-Indirect-Draw-Design-todo.md)，覆盖：`GPUAccess` 权限抽象、结构化缓冲、UAV 资源描述与视图管理、CS 资源绑定与 `uavBarrier` 解绑时机、`copyStructureCount` 的 GPU 侧计数搬运、per-instance 顶点属性、三个跨 API 二进制一致的 indirect 参数结构、跨后端能力查询（`RHICapabilities`），以及各后端的降级策略。

---

## 9. G / H 组：Console 后端与清理（P3）

### 9.1 G1 —— Console 后端定位

`D3D11ConsoleContext`（`Console/Source/T3DD3D11ConsoleContext.cpp`）是一个 null backend：除了从 `D3D11ContextBase` 继承的 `compileShader` / `reflectShaderAllBindings` 之外，几乎所有 `RHIContext` 方法都直接 `return T3D_OK` 或 `return nullptr`。

**这是符合设计意图的** —— 它的用途是无显示环境下的离线 shader 编译与反射（`BundleBuilder` / `BuiltinGenerator` 等命令行工具），不需要真实渲染。**不应该按 Window 后端的标准去"补全"它**。

但当前的静默返回有隐患：工具链里若误调了绘制/资源接口，会静默拿到 `nullptr` 然后在别处崩，排查成本高。建议：

1. 在 `D3D11ConsoleContext` 的类注释里明确写出「本类为离线编译用 null backend，仅 compileShader / reflect* 有效」。
2. 给所有返回 `nullptr` 的资源创建接口加一行 `T3D_LOG_WARNING(LOG_TAG_D3D11RENDERER, "%s is not available in console context", __FUNCTION__)`，让误用在日志里立刻可见。
3. 若离线工具确实需要真实设备（例如要验证纹理能否创建），正确做法是用 `D3D_DRIVER_TYPE_NULL` 或 WARP 软件设备初始化真实的 `D3D11Context`，而不是往 null backend 里塞实现。

### 9.2 H1 —— `reflectSamplerBindings` 下沉

`D3D11ContextBase::reflectSamplerBindings`（`T3DD3D11ContextBase.cpp:318-321`）是空实现，而 `D3D11Context` 里有完整实现。这意味着 Console 后端的 shader 反射会丢掉 sampler 绑定信息 —— 对离线编译产物来说这可能是实际 bug（反射数据不完整会写进 shader 变体的元数据里）。

**方案**：sampler 反射只依赖 `ID3D11ShaderReflection`，不需要 `ID3D11Device`，把 `D3D11Context` 里的实现整体移到 `D3D11ContextBase`，删掉 Window 侧的 override。这样 Console 后端自动获得正确的 sampler 反射。

改完后需要验证一次 `BundleBuilder` 产出的 shader 变体元数据里 sampler 绑定信息完整。

### 9.3 H2 —— 清理 `reset()` 中的历史注释

`reset()` 和 `backupRenderState()` 都已正确实现，但函数开头留有大段被注释的旧实现。直接删除 —— git 历史里有，留在文件里只是噪音。同时删掉 `doBlit` 上线后已无调用者的 `blitAll`。

### 9.4 H3 —— 合并两份重复的 `reflectShaderAllBindings`

`D3D11ContextBase` 与 `D3D11Context` 各有一份功能等价的实现，Window 侧那份只是多了行内注释，以及一大段早期 `ShaderConstantBinding` / `ShaderTexSamplerBinding` 数据结构的注释残留。

**方案**：删除 `D3D11Context` 的 override 与头文件声明，统一走基类。基类版本的日志比 Window 版多打了 `Rows` / `Columns` / `Elements`，反射信息更全，保留它。

合并时发现基类实现有 COM 泄漏：正常返回路径和 `GetDesc` 失败路径都没有释放 `pReflection` / `pShaderBlob`。一并补上 `D3D_SAFE_RELEASE`，并删掉一个从未使用的 `uint32_t size` 局部变量。

### 9.5 H4 —— 删除 `D3D11RenderWindow::init` 与 `setConstantBuffer` 死代码

两处都是早期设计的残留：

- `D3D11RenderWindow::init` 的函数体整个被注释后只剩 `return true`，SwapChain / RTView / DSView 的创建职责早已迁到 `D3D11Context::createRenderWindow`。函数与声明一并删除，`create()` 只保留 new 出空壳对象并注明资源由谁创建。
- `setConstantBuffer` 的单缓冲版本在 §7 删掉注释版 `setWorldTransform` 之后已经没有调用者。删除函数与声明。

顺带把 `D3D11Renderer::init()` 里的 `// ret = mContext->init();` 换成对 `create()` 返回值的实际检查：为空时返回 `T3D_ERR_D3D11_CREATE_FAILED` 并打日志，而不是揣着 nullptr context 继续往下走。

### 9.6 H5 —— `swapBackBuffer` 的 VSync 可配置

`Present` 的 SyncInterval 原先硬编码为 0，引擎侧 `Settings::renderSettings::vsync` 一路传到 `RenderWindowDesc::IsVsync` 之后就断了。

**方案**：给 `D3D11RenderWindow` 加一个 `UINT PresentSyncInterval {0}` 成员，`createRenderWindow` 建 SwapChain 时按 `desc.IsVsync` 置为 1 或 0，`swapBackBuffer` 用该值调 `Present`。放在 RenderWindow 上而不是 Context 上，是因为多窗口时每个窗口的 VSync 设置可以不同。

---

## 10. 验证方案

### 10.1 单项验证

各任务的验证要点已写在对应小节。统一原则：

1. **必开 D3D11 debug layer**（`T3D_DEBUG` 下 `init()` 已加 `D3D11_CREATE_DEVICE_DEBUG`），任何新增代码路径跑完后 output 窗口不应有 D3D11 ERROR/WARNING。
2. **RenderDoc 抓帧核对**。项目已配置 RenderDoc MCP，资源创建类任务（A2/A3/B3/B4）用它逐 mip / 逐 slice 检查内容，比写单测更直接。
3. **退出时检查泄漏**：`ID3D11Debug::ReportLiveDeviceObjects(D3D11_RLDO_DETAIL)`，A4 / D1 这类涉及重建的任务必须确认没有多余的 live object。

### 10.2 两种线程模式都要过

所有改动必须在 **RHI 线程开启** 和 **关闭** 两种模式下各验证一遍。§0.1 / §0.2 描述的坑只在多线程模式下暴露：

- 单线程模式：`enqueue_unique_command` 同步执行，返回真实错误码，CPU 数据指针必然有效 → 错误路径和生命周期问题都被掩盖。
- 多线程模式：返回值恒 `T3D_OK`，lambda 执行时机延后一帧 → 深拷贝缺失、SmartPtr 漏持有会在这里崩。

### 10.3 回归范围

| 改动组 | 需要回归的场景 |
|-------|--------------|
| A2 / A3 / B3 / B4 | 所有采样纹理的场景；天空盒（cubemap mip）；`assets/samples` 全量跑一遍 |
| A4 | 窗口渲染的深度测试与深度清除；窗口 resize；全屏切换 |
| C 组 | 后处理链（RenderTexture 间 blit）；编辑器视口预览；截图功能 |
| D 组 | 编辑器面板拖拽改变视口大小；窗口 resize |
| H1 | `BundleBuilder` / `BuiltinGenerator` 的 shader 编译产物比对 |

---

## 11. 与其他后端的对齐

D3D11 的这批缺口在其他后端也大量存在，实现时顺手对齐能省下重复设计：

| 接口 | D3D11 | Vulkan | GL4 | GLES3 |
|-----|-------|--------|-----|-------|
| `createPixelBuffer1D` | ✅ 本文 B4 | 待查 | 待查 | 待查 |
| `createPixelBuffer3D` | ✅ 本文 B3 | 待查 | 待查 | 待查 |
| `blit` 四重载 | ✅ 本文 C 组 | 待查 | 待查 | 待查 |
| `copyBuffer` | ✅ 本文 C5 | 待查 | 待查 | 待查 |
| `resizeRenderTexture` | ✅ 本文 D1 | ❌ 未 override 新增的 `RHIContext` 虚接口 | 同左 | 同左 |
| `resizeRenderTarget` | ✅ 本文 D2 | ❌ 未 override 新增的 `RHIContext` 虚接口 | 同左 | 同左 |
| `dispatch` / UAV / instanced / indirect | ➡ RHI 层缺失，已独立立项（§8） | 同左 | 同左 | 同左 |

> D0 的决策落地为：`resizeRenderTexture` / `resizeRenderTarget` 提升为 `RHIContext` 虚接口，基类默认返回 `T3D_ERR_NOT_IMPLEMENT`。其它后端在 override 之前调用 `RenderTexture::resize` 会拿到这个错误码，而不是静默无操作。

其中 §2.1 的 `buildSubresourceData` 逻辑（mip 尺寸递推、行距/面距计算、越界校验）是**后端无关的纯计算**，建议直接放到 Core 层（例如 `PixelUtil` / `Image` 的静态方法），让 Vulkan / GL / Metal 后端复用，只在最后一步转成各自的原生结构。这能避免同一套 mip 计算在五个后端里写五遍、错五种不同的方式。

同理，§6.1 提到的「RenderTexture resize 需要引擎层驱动而非后端反改引擎对象」也是跨后端的一致性问题，最好在 `RenderTexture` 上加统一的 `resize()` 入口，各后端只实现「重建 RHI 资源」这一步。
