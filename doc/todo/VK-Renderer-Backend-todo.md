# Vulkan 渲染后端接口实现状态

> 本文档基于 `RHIContext` 纯虚接口定义，逐一分析 `VKContext`（Window 版本）的实现完成度。
>
> - **接口定义**：`source/Core/Include/RHI/T3DRHIContext.h`
> - **VK 基类实现**：`source/Plugins/Renderer/Vulkan/Base/Source/T3DVKContextBase.cpp`
> - **VK Window 实现**：`source/Plugins/Renderer/Vulkan/Window/Source/T3DVKContext.cpp`
> - **VK Window 头文件**：`source/Plugins/Renderer/Vulkan/Window/Include/T3DVKContext.h`

---

## 实现状态图例

| 标记 | 含义 |
|------|------|
| ✅ 已完成 | 功能完整实现 |
| ⚠️ 部分完成 | 有条件分支但仅实现了部分分支，或缺少边界处理 |
| ❌ 未实现 | 函数体为空或直接返回空值/T3D_OK，无实际逻辑 |
| 🔇 按设计为空 | Vulkan 后端按设计意图不需要在此接口实现（逻辑移至其他位置） |
| 🦴 空壳实现 | 只创建了空的 RHI 对象但无任何 Vulkan 资源创建逻辑 |

---

## 1. 变换 (Transform)

### 1.1 setViewProjectionTransform

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat)` |
| **功能** | 设置视图变换矩阵和投影变换矩阵。Vulkan 深度范围为 [0,1]，内部会通过转换矩阵将 [-1,1] 映射到 [0,1]（与 D3D11 相同的深度映射策略） |
| **参数** | `viewMat` — 视图变换矩阵；`projMat` — 投影变换矩阵 |
| **返回值** | `T3D_OK` |
| **实现位置** | `VKContextBase::setViewProjectionTransform` (T3DVKContextBase.cpp:125) |
| **内部 Vulkan API** | 无直接 Vulkan 调用（纯 CPU 矩阵运算：深度范围 [-1,1]→[0,1] 转换矩阵乘法） |
| **备注** | 在 VKContextBase 层实现，VKContext 继承使用 |

---

## 2. 渲染目标 (Render Target)

### 2.1 createRenderWindow

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIRenderTargetPtr createRenderWindow(RenderWindow *renderWindow)` |
| **功能** | 创建 RHI 渲染窗口。内部创建 Win32 Surface、验证 Present 支持、创建 SwapChain（选择最佳格式/PresentMode/Extent）、创建 ImageView、创建深度缓冲（VkImage + VkImageView）、创建 Framebuffer |
| **参数** | `renderWindow` — 引擎渲染窗口对象，包含窗口句柄、宽高、MSAA 等描述信息 |
| **返回值** | 成功返回 `VKRenderWindowPtr`，失败返回 `nullptr` |
| **实现位置** | T3DVKContext.cpp:1014 |
| **内部 Vulkan API** | `vkCreateWin32SurfaceKHR` — 创建 Win32 平台窗口 Surface |
| | `vkGetPhysicalDeviceSurfaceSupportKHR` — 验证 Present 队列支持 |
| | `vkGetPhysicalDeviceSurfaceCapabilitiesKHR` — 查询 Surface 能力 |
| | `vkGetPhysicalDeviceSurfaceFormatsKHR` — 查询支持的表面格式 |
| | `vkCreateSwapchainKHR` — 创建 Swapchain |
| | `vkGetSwapchainImagesKHR` — 获取 Swapchain 图像 |
| | `vkCreateImageView` — 为每个 Swapchain Image 创建 ImageView（通过 `createVkImageView`） |
| | `vkCreateRenderPass` — 创建默认 RenderPass（color + depth） |
| | `vkCreateImage` / `vkAllocateMemory` / `vkBindImageMemory` — 创建深度缓冲 VkImage（通过 `createVkImage`） |
| | `vkCreateFramebuffer` — 为每个 Swapchain Image 创建 Framebuffer |

### 2.2 createRenderTexture

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2D *buffer)` |
| **功能** | 创建 RHI 渲染纹理。根据像素格式判断创建颜色 RT 或深度模板 RT，创建 VkImage + VkImageView，设置合适的 usage flags（COLOR_ATTACHMENT / DEPTH_STENCIL_ATTACHMENT + SAMPLED） |
| **参数** | `buffer` — 引擎像素缓冲区对象，包含宽高、像素格式等信息 |
| **返回值** | 成功返回 `VKPixelBuffer2DPtr`（内含 VkImage/VkImageView/VkDeviceMemory），失败返回 `nullptr` |
| **实现位置** | T3DVKContext.cpp:1307 |
| **内部 Vulkan API** | `vkCreateImage` / `vkAllocateMemory` / `vkBindImageMemory` — 创建 RT 专用 VkImage（通过 `createVkImage`） |
| | `vkCreateImageView` — 创建 ImageView（通过 `createVkImageView`；深度格式时创建两个 view：attachment 用全 aspect、采样用 depth-only aspect） |

### 2.3 setRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setRenderTarget(RenderTarget *renderTarget)` |
| **功能** | 设置当前渲染目标。存储 renderTarget 指针供后续 beginPass/render 使用 |
| **参数** | `renderTarget` — 渲染目标对象，可以是窗口类型或纹理类型 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:1373 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（仅缓存 renderTarget 指针供后续 beginPass/render 使用） |

### 2.4 setRenderTarget (MRT 重载 - Window)

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setRenderTarget(RenderWindow *renderWindow, RenderTexture *depthStencil)` |
| **功能** | 设置窗口类型的渲染目标及独立的深度模板缓冲 |
| **参数** | `renderWindow` — 渲染窗口对象；`depthStencil` — 深度模板纹理 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:1381 |
| **问题** | 函数体仅 `return T3D_OK`，未存储任何状态 |

### 2.5 setRenderTarget (MRT 重载 - Texture Array)

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setRenderTarget(const RenderTexturePtr *renderTexture, uint32_t numOfTextures, RenderTexture *depthStencil)` |
| **功能** | 设置多个渲染纹理作为 MRT 渲染目标 |
| **参数** | `renderTexture` — 渲染纹理数组指针；`numOfTextures` — 纹理数量；`depthStencil` — 深度模板纹理 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:1388 |
| **问题** | 函数体仅 `return T3D_OK`，不支持 MRT 渲染 |

### 2.6 resetRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult resetRenderTarget()` |
| **功能** | 清除当前渲染目标绑定，将 mCurrentRenderTarget 设为 nullptr |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:1395 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（仅将 mCurrentRenderTarget 置为 nullptr） |

---

## 3. 视口 (Viewport)

### 3.1 setViewport

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult setViewport(const Viewport &viewport)` |
| **功能** | 设置视口区域。Vulkan 后端按设计不在此接口实现，viewport 在 beginPass 和 render 过程中通过 vkCmdSetViewport/vkCmdSetScissor 动态设置 |
| **参数** | `viewport` — 视口对象，包含归一化偏移值 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:1403 |
| **备注** | 注释说明 "Viewport is set during command buffer recording in render()"。与 D3D11 不同，Vulkan 使用动态状态在 command buffer 录制时设置 viewport |

---

## 4. 清除操作 (Clear)

### 4.1 clearColor

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult clearColor(const ColorRGB &color)` |
| **功能** | 用指定颜色填充当前渲染目标的 framebuffer。根据 RenderTarget 类型（Window/Texture）分别处理：Window 类型使用 vkCmdClearColorImage 清除 swapchain image，Texture 类型执行 image layout transition 后清除 |
| **参数** | `color` — RGB 颜色值（alpha 固定为 1.0） |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:1411 |
| **内部 Vulkan API** | **Window 路径**：`vkCmdClearColorImage` — 清除 swapchain image 颜色 |
| | **Texture 路径**：`vkCmdPipelineBarrier`（通过 `insertImageBarrier`）— layout transition → TRANSFER_DST |
| | 　　　　　　`vkCmdClearColorImage` — 清除 render texture 颜色 |

### 4.2 clearDepth

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult clearDepth(Real depth)` |
| **功能** | 用指定深度值填充当前渲染目标的 depth buffer。执行 layout transition 到 TRANSFER_DST 后调用 vkCmdClearDepthStencilImage（仅清除深度，不清除模板） |
| **参数** | `depth` — 深度值（通常为 1.0） |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:1469 |
| **内部 Vulkan API** | `vkCmdPipelineBarrier`（通过 `insertImageBarrier`）— depth image layout transition → TRANSFER_DST |
| | `vkCmdClearDepthStencilImage` — 清除深度缓冲 |

### 4.3 clearDepthStencil

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult clearDepthStencil(Real depth, uint32_t stencil)` |
| **功能** | 用指定深度值和模板值同时填充当前渲染目标的 depth buffer 和 stencil buffer。执行 layout transition 后调用 vkCmdClearDepthStencilImage |
| **参数** | `depth` — 深度值；`stencil` — 模板值 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:1507 |
| **内部 Vulkan API** | `vkCmdPipelineBarrier`（通过 `insertImageBarrier`）— depth image layout transition → TRANSFER_DST |
| | `vkCmdClearDepthStencilImage` — 同时清除深度和模板缓冲 |

---

## 5. 渲染状态 (Render State)

### 5.1 createBlendState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIBlendStatePtr createBlendState(BlendState *state)` |
| **功能** | 创建 RHI 颜色混合状态对象。Vulkan 中混合状态是 pipeline 创建的一部分，此处仅创建 VKBlendState 包装对象，实际的 VkPipelineColorBlendStateCreateInfo 在 pipeline 创建时构建 |
| **参数** | `state` — 引擎颜色混合状态对象，包含 SrcBlend/DestBlend/BlendOp/ColorMask 等描述 |
| **返回值** | 成功返回 `VKBlendStatePtr` |
| **实现位置** | T3DVKContext.cpp:1542 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（仅创建引擎包装对象，实际 `VkPipelineColorBlendStateCreateInfo` 在 `getOrCreatePipeline` 时构建） |
| **备注** | Vulkan 不像 D3D11 有独立的 blend state 对象，混合状态嵌入到 Graphics Pipeline 中 |

### 5.2 createDepthStencilState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIDepthStencilStatePtr createDepthStencilState(DepthStencilState *state)` |
| **功能** | 创建 RHI 深度模板状态对象。与 blend state 类似，仅创建包装对象，实际在 pipeline 创建时使用 |
| **参数** | `state` — 引擎深度模板状态对象 |
| **返回值** | 成功返回 `VKDepthStencilStatePtr` |
| **实现位置** | T3DVKContext.cpp:1551 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（仅创建引擎包装对象，实际 `VkPipelineDepthStencilStateCreateInfo` 在 `getOrCreatePipeline` 时构建） |

### 5.3 createRasterizerState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIRasterizerStatePtr createRasterizerState(RasterizerState *state)` |
| **功能** | 创建 RHI 光栅化状态对象。仅创建包装对象 |
| **参数** | `state` — 引擎光栅化状态对象 |
| **返回值** | 成功返回 `VKRasterizerStatePtr` |
| **实现位置** | T3DVKContext.cpp:1559 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（仅创建引擎包装对象，实际 `VkPipelineRasterizationStateCreateInfo` 在 `getOrCreatePipeline` 时构建） |

### 5.4 createSamplerState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHISamplerStatePtr createSamplerState(SamplerState *state)` |
| **功能** | 创建 RHI 纹理采样状态对象。完整创建 VkSampler，支持 Filter/AddressMode/LOD/Anisotropy/CompareOp/BorderColor 等配置 |
| **参数** | `state` — 引擎纹理采样状态对象 |
| **返回值** | 成功返回 `VKSamplerStatePtr`（内含 VkSampler），失败返回 `nullptr` |
| **实现位置** | T3DVKContext.cpp:1567 |
| **内部 Vulkan API** | `vkCreateSampler` — 创建 VkSampler（配置 Filter/AddressMode/LOD/Anisotropy/CompareOp/BorderColor） |

### 5.5 setBlendState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setBlendState(BlendState *state)` |
| **功能** | 设置颜色混合状态。缓存 BlendDesc 供后续 pipeline 创建使用 |
| **参数** | `state` — 混合状态对象（内含已创建的 RHI 资源） |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:1607 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（仅缓存 BlendDesc，在 `getOrCreatePipeline` 时作为 pipeline state 的一部分使用） |
| **备注** | Vulkan 中不直接绑定 blend state，而是缓存描述符，在 getOrCreatePipeline 时作为 pipeline state 的一部分 |

### 5.6 setDepthStencilState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setDepthStencilState(DepthStencilState *state)` |
| **功能** | 设置深度模板状态。缓存 DepthStencilDesc 供后续 pipeline 创建使用 |
| **参数** | `state` — 深度模板状态对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:1618 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（仅缓存 DepthStencilDesc，在 `getOrCreatePipeline` 时作为 pipeline state 的一部分使用） |

### 5.7 setRasterizerState

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setRasterizerState(RasterizerState *state)` |
| **功能** | 设置光栅化状态。缓存 RasterizerDesc 供后续 pipeline 创建使用 |
| **参数** | `state` — 光栅化状态对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:1629 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（仅缓存 RasterizerDesc，在 `getOrCreatePipeline` 时作为 pipeline state 的一部分使用） |

---

## 6. 顶点输入 (Vertex Input)

### 6.1 createVertexDeclaration

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIVertexDeclarationPtr createVertexDeclaration(VertexDeclaration *decl)` |
| **功能** | 创建 RHI 顶点格式对象。Vulkan 中顶点输入描述是 pipeline 的一部分，此处仅创建包装对象 |
| **参数** | `decl` — 顶点格式对象，包含顶点属性列表（Semantic/Format/Slot/Offset） |
| **返回值** | 成功返回 `VKVertexDeclarationPtr` |
| **实现位置** | T3DVKContext.cpp:1640 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（仅创建引擎包装对象，顶点输入描述在 `getOrCreatePipeline` 时通过 `VkPipelineVertexInputStateCreateInfo` 使用） |

### 6.2 setVertexDeclaration

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVertexDeclaration(VertexDeclaration *decl)` |
| **功能** | 设置顶点格式。遍历所有 VertexAttribute，收集唯一的 binding slot 并创建 VkVertexInputBindingDescription 和 VkVertexInputAttributeDescription 数组，供后续 pipeline 创建使用 |
| **参数** | `decl` — 顶点格式对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:1648 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（构建 `VkVertexInputBindingDescription` 和 `VkVertexInputAttributeDescription` 数组缓存，供 `getOrCreatePipeline` 中 `VkPipelineVertexInputStateCreateInfo` 使用） |
| **备注** | 与 D3D11 不同，Vulkan 不调用 IASetInputLayout，而是将顶点输入信息缓存后在 pipeline 创建时使用 |

---

## 7. 缓冲区 (Buffer)

### 7.1 createVertexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIVertexBufferPtr createVertexBuffer(VertexBuffer *buffer)` |
| **功能** | 创建 RHI 顶点缓冲区对象。使用 createVkBuffer 创建 VkBuffer（VERTEX_BUFFER_BIT + HOST_VISIBLE + HOST_COHERENT），支持初始数据通过 vkMapMemory/memcpy/vkUnmapMemory 上传 |
| **参数** | `buffer` — 引擎的顶点缓冲区对象，包含缓冲大小和可选的初始数据 |
| **返回值** | 成功返回 `VKVertexBufferPtr`，失败返回 `nullptr` |
| **实现位置** | T3DVKContext.cpp:1691 |
| **内部 Vulkan API** | `vkCreateBuffer` — 创建 VkBuffer（VERTEX_BUFFER_BIT）（通过 `createVkBuffer`） |
| | `vkGetBufferMemoryRequirements` — 查询内存需求（通过 `createVkBuffer`） |
| | `vkAllocateMemory` — 分配 GPU 内存（HOST_VISIBLE + HOST_COHERENT）（通过 `createVkBuffer`） |
| | `vkBindBufferMemory` — 绑定缓冲区内存（通过 `createVkBuffer`） |
| | `vkMapMemory` / `vkUnmapMemory` — 映射内存并上传初始数据（如有） |

### 7.2 setVertexBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets)` |
| **功能** | 设置渲染用的顶点缓冲对象，支持多个缓冲区绑定到不同 slot，调用 vkCmdBindVertexBuffers |
| **参数** | `startSlot` — 缓冲区起始插槽；`buffers` — 顶点缓冲对象数组；`strides` — 每个缓冲区的顶点跨度；`offsets` — 每个缓冲区的起始偏移 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:1738 |
| **内部 Vulkan API** | `vkCmdBindVertexBuffers` — 绑定一个或多个顶点缓冲区到指定 slot |

### 7.3 createIndexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIIndexBufferPtr createIndexBuffer(IndexBuffer *buffer)` |
| **功能** | 创建 RHI 索引缓冲对象。使用 createVkBuffer 创建 VkBuffer（INDEX_BUFFER_BIT + HOST_VISIBLE + HOST_COHERENT），支持初始数据上传 |
| **参数** | `buffer` — 引擎的索引缓冲对象，包含缓冲大小、索引类型和可选的初始数据 |
| **返回值** | 成功返回 `VKIndexBufferPtr`，失败返回 `nullptr` |
| **实现位置** | T3DVKContext.cpp:1762 |
| **内部 Vulkan API** | `vkCreateBuffer` — 创建 VkBuffer（INDEX_BUFFER_BIT）（通过 `createVkBuffer`） |
| | `vkGetBufferMemoryRequirements` / `vkAllocateMemory` / `vkBindBufferMemory`（通过 `createVkBuffer`） |
| | `vkMapMemory` / `vkUnmapMemory` — 映射内存并上传初始数据（如有） |

### 7.4 setIndexBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setIndexBuffer(IndexBuffer *buffer)` |
| **功能** | 设置渲染用的索引缓冲对象，自动映射索引类型为 VkIndexType，调用 vkCmdBindIndexBuffer |
| **参数** | `buffer` — 索引缓冲对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:1807 |
| **内部 Vulkan API** | `vkCmdBindIndexBuffer` — 绑定索引缓冲区（自动映射引擎 IndexType 到 `VkIndexType`） |

### 7.5 createConstantBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIConstantBufferPtr createConstantBuffer(ConstantBuffer *buffer)` |
| **功能** | 创建 RHI 常量缓冲对象（Vulkan 中对应 Uniform Buffer）。使用 createVkBuffer 创建 VkBuffer（UNIFORM_BUFFER_BIT + HOST_VISIBLE + HOST_COHERENT），支持初始数据上传 |
| **参数** | `buffer` — 引擎的常量缓冲区对象，包含缓冲大小和初始数据 |
| **返回值** | 成功返回 `VKConstantBufferPtr`，失败返回 `nullptr` |
| **实现位置** | T3DVKContext.cpp:1823 |
| **内部 Vulkan API** | `vkCreateBuffer` — 创建 VkBuffer（UNIFORM_BUFFER_BIT）（通过 `createVkBuffer`） |
| | `vkGetBufferMemoryRequirements` / `vkAllocateMemory` / `vkBindBufferMemory`（通过 `createVkBuffer`） |
| | `vkMapMemory` / `vkUnmapMemory` — 映射内存并上传初始数据（如有） |

### 7.6 createPixelBuffer1D

| 项目 | 内容 |
|------|------|
| **状态** | 🦴 空壳实现 |
| **签名** | `RHIPixelBuffer1DPtr createPixelBuffer1D(PixelBuffer1D *buffer)` |
| **功能** | 创建 RHI 1D 像素缓冲区对象（1D 纹理） |
| **参数** | `buffer` — 引擎 1D 像素缓冲区对象 |
| **返回值** | 返回空的 `VKPixelBuffer1D` 对象（未创建任何 VkImage 或 VkImageView） |
| **实现位置** | T3DVKContext.cpp:1868 |
| **问题** | 只调用了 `VKPixelBuffer1D::create()` 就直接返回，没有创建 VkImage 和相应的 ImageView。返回的对象内部资源指针全部为 null |

### 7.7 createPixelBuffer2D

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIPixelBuffer2DPtr createPixelBuffer2D(PixelBuffer2D *buffer)` |
| **功能** | 创建 RHI 2D 像素缓冲区对象（2D 纹理）。创建 VkImage + VkImageView，通过 staging buffer 上传纹理数据，执行 layout transition（UNDEFINED → TRANSFER_DST → SHADER_READ_ONLY） |
| **参数** | `buffer` — 引擎 2D 像素缓冲区对象，包含宽高、格式、mipmap 等信息 |
| **返回值** | 成功返回 `VKPixelBuffer2DPtr`（内含 VkImage/VkImageView/VkDeviceMemory），失败返回 `nullptr` |
| **实现位置** | T3DVKContext.cpp:1876 |
| **内部 Vulkan API** | `vkCreateImage` / `vkGetImageMemoryRequirements` / `vkAllocateMemory` / `vkBindImageMemory` — 创建纹理 VkImage（通过 `createVkImage`） |
| | `vkCreateImageView` — 创建纹理 ImageView（通过 `createVkImageView`） |
| | **Staging upload（如有初始数据）**： |
| | 　`vkCreateBuffer` — 创建 staging buffer（TRANSFER_SRC）（通过 `createVkBuffer`） |
| | 　`vkMapMemory` / `vkUnmapMemory` — 上传数据到 staging buffer |
| | 　`vkCmdPipelineBarrier` — layout transition UNDEFINED → TRANSFER_DST（通过 `transitionImageLayout`） |
| | 　`vkAllocateCommandBuffers` / `vkBeginCommandBuffer` — 创建一次性 command buffer（通过 `beginSingleTimeCommands`） |
| | 　`vkCmdCopyBufferToImage` — 从 staging buffer 拷贝到 VkImage |
| | 　`vkEndCommandBuffer` / `vkQueueSubmit` / `vkQueueWaitIdle` / `vkFreeCommandBuffers`（通过 `endSingleTimeCommands`） |
| | 　`vkCmdPipelineBarrier` — layout transition TRANSFER_DST → SHADER_READ_ONLY（通过 `transitionImageLayout`） |
| | 　`vkDestroyBuffer` / `vkFreeMemory` — 销毁 staging buffer |

### 7.8 createPixelBuffer3D

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `RHIPixelBuffer3DPtr createPixelBuffer3D(PixelBuffer3D *buffer)` |
| **功能** | 创建 RHI 3D 像素缓冲区对象（3D 纹理/体积纹理） |
| **参数** | `buffer` — 引擎 3D 像素缓冲区对象 |
| **返回值** | 当前直接返回 `nullptr` |
| **实现位置** | T3DVKContext.cpp:1947 |
| **问题** | 完全未实现，直接 `return nullptr`，无法创建 3D 纹理资源 |

---

## 8. 顶点着色器 (Vertex Shader)

### 8.1 createVertexShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIShaderPtr createVertexShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 顶点着色器对象。使用已编译的 SPIR-V 字节码创建 VkShaderModule，并通过 SPIRV-Reflect 提取 binding 信息（Uniform Buffer / Combined Image Sampler 等） |
| **参数** | `shader` — 引擎着色器变体对象，包含已编译的 SPIR-V 字节码 |
| **返回值** | 成功返回 `VKVertexShaderPtr`，失败返回 `nullptr` |
| **实现位置** | T3DVKContext.cpp:1954 |
| **内部 Vulkan API** | `vkCreateShaderModule` — 从 SPIR-V 字节码创建 VkShaderModule |
| | **spirv_reflect 库调用**：`spvReflectCreateShaderModule` → `spvReflectEnumerateDescriptorBindings` → `spvReflectDestroyShaderModule` — 提取 descriptor binding 信息（UBO/Sampler 的 binding/set/type） |

### 8.2 setVertexShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVertexShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的顶点着色器。缓存 VkShaderModule 和 stage info 供后续 pipeline 创建使用 |
| **参数** | `shader` — 引擎使用的顶点着色器对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:2026 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（缓存 `VkShaderModule`、entry point 和 `VkDescriptorSetLayoutBinding` 列表，供 `getOrCreatePipeline` / `getOrCreateDescriptorSetLayout` 使用） |

### 8.3 setVSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置顶点着色器的常量缓冲区（Uniform Buffer）。收集 VkBuffer 和 size 信息供后续 descriptor set 更新使用 |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:2060 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（收集 `VkBuffer` 和 size 信息到缓存数组，供 `bindDescriptorSet` 中 `vkUpdateDescriptorSets` 使用） |

### 8.4 setVSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置顶点着色器的纹理资源 |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2086 |
| **问题** | 注释说明 "VS texture bindings not commonly used, but store for completeness"，但实际未存储任何数据 |

### 8.5 setVSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setVSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置顶点着色器的纹理采样器 |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2094 |

---

## 9. 像素着色器 (Pixel Shader / Fragment Shader)

### 9.1 createPixelShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `RHIShaderPtr createPixelShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 像素着色器对象。使用已编译的 SPIR-V 字节码创建 VkShaderModule，通过 SPIRV-Reflect 提取 binding 信息，并执行 binding 重映射以避免与顶点着色器冲突 |
| **参数** | `shader` — 引擎着色器变体对象，包含已编译的 SPIR-V 字节码 |
| **返回值** | 成功返回 `VKPixelShaderPtr`，失败返回 `nullptr` |
| **实现位置** | T3DVKContext.cpp:2101 |
| **内部 Vulkan API** | **spirv_reflect 库调用**：`spvReflectCreateShaderModule` → `spvReflectEnumerateDescriptorBindings` → `spvReflectChangeDescriptorBindingNumbers`（+PS_BINDING_OFFSET 重映射）→ `spvReflectGetCodeSize` / `spvReflectGetCode` → `spvReflectDestroyShaderModule` |
| | `vkCreateShaderModule` — 从重映射后的 SPIR-V 字节码创建 VkShaderModule |

### 9.2 setPixelShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPixelShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的像素着色器（Vulkan 中为 Fragment Shader）。缓存 VkShaderModule 和 stage info 供后续 pipeline 创建使用 |
| **参数** | `shader` — 引擎使用的像素着色器对象 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:2192 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（缓存 `VkShaderModule`、entry point 和 `VkDescriptorSetLayoutBinding` 列表，供 `getOrCreatePipeline` / `getOrCreateDescriptorSetLayout` 使用） |

### 9.3 setPSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置像素着色器的常量缓冲区。收集 VkBuffer 和 size 信息 |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:2223 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（收集 `VkBuffer` 和 size 信息到缓存数组，供 `bindDescriptorSet` 中 `vkUpdateDescriptorSets` 使用） |

### 9.4 setPSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置像素着色器的纹理资源。收集 VkImageView 供后续 descriptor set 更新使用 |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:2247 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（收集 `VkImageView` 到缓存数组，供 `bindDescriptorSet` 中 `vkUpdateDescriptorSets` 使用） |

### 9.5 setPSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置像素着色器的纹理采样器。收集 VkSampler 供后续 descriptor set 更新使用 |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:2268 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（收集 `VkSampler` 到缓存数组，供 `bindDescriptorSet` 中 `vkUpdateDescriptorSets` 使用） |

---

## 10. Hull 着色器 (Tessellation Control Shader)

### 10.1 createHullShader

| 项目 | 内容 |
|------|------|
| **状态** | 🦴 空壳实现 |
| **签名** | `RHIShaderPtr createHullShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 曲面细分控制着色器对象 |
| **参数** | `shader` — 引擎着色器变体对象 |
| **返回值** | 返回空的 `VKHullShaderPtr` 对象（未创建 VkShaderModule） |
| **实现位置** | T3DVKContext.cpp:2291 |
| **问题** | 只调用 `VKHullShader::create()` 返回空壳对象，无 SPIR-V 编译和 VkShaderModule 创建逻辑 |

### 10.2 setHullShader

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setHullShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的曲面细分控制着色器 |
| **参数** | `shader` — 引擎使用的曲面细分着色器对象 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2300 |

### 10.3 setHSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置 Hull 着色器的常量缓冲区 |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2301 |

### 10.4 setHSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置 Hull 着色器的纹理资源 |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2302 |

### 10.5 setHSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setHSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置 Hull 着色器的纹理采样器 |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2303 |

---

## 11. Domain 着色器 (Tessellation Evaluation Shader)

### 11.1 createDomainShader

| 项目 | 内容 |
|------|------|
| **状态** | 🦴 空壳实现 |
| **签名** | `RHIShaderPtr createDomainShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 域着色器对象（Vulkan 中对应 Tessellation Evaluation Shader） |
| **参数** | `shader` — 引擎着色器变体对象 |
| **返回值** | 返回空的 `VKDomainShaderPtr` 对象（未创建 VkShaderModule） |
| **实现位置** | T3DVKContext.cpp:2307 |
| **问题** | 只调用 `VKDomainShader::create()` 返回空壳对象 |

### 11.2 setDomainShader

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setDomainShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的域着色器 |
| **参数** | `shader` — 引擎使用的域着色器，可为 nullptr |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2315 |

### 11.3 setDSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置 Domain 着色器的常量缓冲区 |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2316 |

### 11.4 setDSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置 Domain 着色器的纹理资源 |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2317 |

### 11.5 setDSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setDSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置 Domain 着色器的纹理采样器 |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2318 |

---

## 12. 几何着色器 (Geometry Shader)

### 12.1 createGeometryShader

| 项目 | 内容 |
|------|------|
| **状态** | 🦴 空壳实现 |
| **签名** | `RHIShaderPtr createGeometryShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 几何着色器对象 |
| **参数** | `shader` — 引擎着色器变体对象 |
| **返回值** | 返回空的 `VKGeometryShaderPtr` 对象（未创建 VkShaderModule） |
| **实现位置** | T3DVKContext.cpp:2322 |
| **问题** | 只调用 `VKGeometryShader::create()` 返回空壳对象 |

### 12.2 setGeometryShader

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setGeometryShader(ShaderVariant *shader)` |
| **功能** | 设置渲染使用的几何着色器 |
| **参数** | `shader` — 引擎使用的几何着色器，可为 nullptr |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2330 |

### 12.3 setGSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置几何着色器的常量缓冲区 |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2331 |

### 12.4 setGSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置几何着色器的纹理资源 |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2332 |

### 12.5 setGSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setGSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置几何着色器的纹理采样器 |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2333 |

---

## 13. 计算着色器 (Compute Shader)

### 13.1 createComputeShader

| 项目 | 内容 |
|------|------|
| **状态** | 🦴 空壳实现 |
| **签名** | `RHIShaderPtr createComputeShader(ShaderVariant *shader)` |
| **功能** | 创建 RHI 计算着色器对象 |
| **参数** | `shader` — 引擎着色器变体对象 |
| **返回值** | 返回空的 `VKComputeShaderPtr` 对象（未创建 VkShaderModule） |
| **实现位置** | T3DVKContext.cpp:2337 |
| **问题** | 只调用 `VKComputeShader::create()` 返回空壳对象，无 compute pipeline 创建逻辑 |

### 13.2 setComputeShader

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setComputeShader(ShaderVariant *shader)` |
| **功能** | 设置计算着色器 |
| **参数** | `shader` — 引擎使用的计算着色器 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2345 |

### 13.3 setCSConstantBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)` |
| **功能** | 设置计算着色器的常量缓冲区 |
| **参数** | `startSlot` — 常量缓冲区起始插槽；`buffers` — 常量缓冲区对象数组 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2346 |

### 13.4 setCSPixelBuffers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)` |
| **功能** | 设置计算着色器的纹理资源 |
| **参数** | `startSlot` — 纹理资源起始插槽；`buffers` — 像素缓冲区对象数组 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2347 |

### 13.5 setCSSamplers

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult setCSSamplers(uint32_t startSlot, const Samplers &samplers)` |
| **功能** | 设置计算着色器的纹理采样器 |
| **参数** | `startSlot` — 采样器起始插槽；`samplers` — 纹理采样器对象数组 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:2348 |

---

## 14. Shader 编译与反射 (Shader Compilation & Reflection)

### 14.1 compileShader

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult compileShader(ShaderVariant *shader)` |
| **功能** | 编译着色器。根据 ShaderStage 自动选择对应的 VkShaderStageFlagBits（VERTEX/FRAGMENT/COMPUTE/GEOMETRY/TESSELLATION_CONTROL/TESSELLATION_EVALUATION），使用 glslang 将 GLSL 源码编译为 SPIR-V 字节码，编译后的字节码写回 shader 对象 |
| **参数** | `shader` — 着色器变体对象，包含 GLSL 源码和 ShaderStage 信息 |
| **返回值** | `T3D_OK`，失败返回对应错误码 |
| **实现位置** | `VKContextBase::compileShader` (T3DVKContextBase.cpp:143) |
| **内部 Vulkan API** | 无直接 Vulkan 调用 |
| | **spirv_reflect 库调用**：`spvReflectCreateShaderModule` / `spvReflectDestroyShaderModule` — 验证 SPIR-V 字节码有效性（magic number 检查 + spirv-reflect 解析验证） |
| **备注** | 在 VKContextBase 层实现。与 D3D11 使用 D3DCompile 不同，Vulkan 使用 glslang 编译 GLSL→SPIR-V |

### 14.2 reflectShaderAllBindings

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)` |
| **功能** | 反射着色器的所有绑定信息。使用 SPIRV-Reflect 库解析 SPIR-V 字节码，提取 Uniform Buffer 常量（名称/binding/大小/偏移/类型）和 Combined Image Sampler / Separate Sampler 绑定信息。支持 Bool/Int/Float/Vector4/Matrix4/Struct 等类型识别 |
| **参数** | `shader` — 要反射的着色器（需已编译为 SPIR-V）；`constantParams` — [out] 常量缓冲区绑定信息；`samplerParams` — [out] 纹理采样器绑定信息 |
| **返回值** | `T3D_OK` |
| **实现位置** | `VKContextBase::reflectShaderAllBindings` (T3DVKContextBase.cpp:227) |
| **内部 Vulkan API** | 无直接 Vulkan 调用 |
| | **spirv_reflect 库调用**：`spvReflectCreateShaderModule` → `spvReflectEnumerateDescriptorBindings` — 枚举所有 descriptor binding（UNIFORM_BUFFER / COMBINED_IMAGE_SAMPLER / SAMPLED_IMAGE / SAMPLER），提取常量参数和采样器参数 → `spvReflectDestroyShaderModule` |
| **备注** | 与 D3D11 使用 D3DReflect 不同，Vulkan 使用 spirv_reflect 库 |

### 14.3 reflectSamplerBindings

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `TResult reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams)` |
| **功能** | 运行时反射着色器的纹理采样器绑定信息 |
| **参数** | `shader` — 要反射的着色器；`samplerParams` — [in,out] 纹理采样参数 |
| **返回值** | `T3D_OK` |
| **实现位置** | `VKContextBase::reflectSamplerBindings` (T3DVKContextBase.cpp:463) |
| **备注** | 注释说明 "Sampler bindings are already handled in reflectShaderAllBindings (via COMBINED_IMAGE_SAMPLER and separate SAMPLER types)"。Vulkan 中采样器绑定在 reflectShaderAllBindings 中已完整处理，此接口按设计无需额外操作 |

---

## 15. 图元与绘制 (Primitive & Draw)

### 15.1 setPrimitiveType

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult setPrimitiveType(PrimitiveType primitive)` |
| **功能** | 设置渲染图元类型。将引擎图元类型映射为 VkPrimitiveTopology（三角形列表、三角形带、线条列表等），缓存供后续 pipeline 创建使用 |
| **参数** | `primitive` — 图元类型枚举值 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:2352 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（通过 `VKMapping::get` 将引擎枚举映射为 `VkPrimitiveTopology` 并缓存，在 `getOrCreatePipeline` 中 `VkPipelineInputAssemblyStateCreateInfo` 使用） |
| **备注** | 与 D3D11 直接调用 IASetPrimitiveTopology 不同，Vulkan 将图元拓扑作为 pipeline state 的一部分 |

### 15.2 render (indexed)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)` |
| **功能** | 根据上下文设置好的资源和状态进行带索引的绘制。内部自动执行：getOrCreatePipeline（hash 缓存）→ vkCmdBindPipeline → bindDescriptorSet（分配 descriptor set + 更新 UBO/纹理/采样器绑定）→ vkCmdDrawIndexed |
| **参数** | `indexCount` — 索引数量；`startIndex` — 索引缓冲区中的起始位置；`baseVertex` — 基础顶点偏移 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:2823 |
| **内部 Vulkan API** | `vkCreateGraphicsPipelines` — 创建图形 pipeline（缓存未命中时，通过 `getOrCreatePipeline`） |
| | `vkCmdBindPipeline` — 绑定 pipeline 到 command buffer（`VK_PIPELINE_BIND_POINT_GRAPHICS`） |
| | `vkCmdSetDepthBias` — 设置动态深度偏移 |
| | `vkCreateDescriptorSetLayout`（通过 `getOrCreateDescriptorSetLayout`，缓存未命中时） |
| | `vkCreatePipelineLayout`（通过 `getOrCreatePipelineLayout`，缓存未命中时） |
| | `vkAllocateDescriptorSets` — 从 descriptor pool 分配 descriptor set（通过 `bindDescriptorSet`） |
| | `vkUpdateDescriptorSets` — 更新 UBO/纹理/采样器 descriptor 绑定（通过 `bindDescriptorSet`） |
| | `vkCmdBindDescriptorSets` — 绑定 descriptor set 到 command buffer（通过 `bindDescriptorSet`） |
| | `vkCmdDrawIndexed` — 执行带索引的绘制调用 |

### 15.3 render (non-indexed)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult render(uint32_t vertexCount, uint32_t startVertex)` |
| **功能** | 根据上下文设置好的资源和状态进行不带索引的绘制。流程同 indexed render，最后调用 vkCmdDraw |
| **参数** | `vertexCount` — 顶点数量；`startVertex` — 顶点缓冲区中的起始位置 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:2865 |
| **内部 Vulkan API** | `vkCreateGraphicsPipelines`（通过 `getOrCreatePipeline`，缓存未命中时） |
| | `vkCmdBindPipeline` — 绑定 pipeline |
| | `vkCmdSetDepthBias` — 设置动态深度偏移 |
| | `vkCreateDescriptorSetLayout` / `vkCreatePipelineLayout`（通过内部缓存方法，缓存未命中时） |
| | `vkAllocateDescriptorSets` / `vkUpdateDescriptorSets` / `vkCmdBindDescriptorSets`（通过 `bindDescriptorSet`） |
| | `vkCmdDraw` — 执行不带索引的绘制调用 |

---

## 16. 状态重置 (Reset)

### 16.1 reset

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult reset()` |
| **功能** | 清除所有缓存的渲染状态。包括：当前 VS/PS shader module、VS/PS 常量缓冲区、PS 纹理/采样器、顶点 binding/attribute 信息、图元拓扑、当前 pipeline 引用，以及将渲染状态描述（Rasterizer/Blend/DepthStencil）重置为默认值 |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:2907 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（纯 CPU 端状态重置：清空缓存的 shader module / 常量缓冲区 / 纹理 / 采样器 / 顶点输入信息，重置渲染状态描述为默认值） |

---

## 17. 数据传输 (Blit & Copy)

### 17.1 blit (RenderTarget → RenderTarget)

| 项目 | 内容 |
|------|------|
| **状态** | ⚠️ 部分完成 |
| **签名** | `TResult blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源渲染目标传输图像数据到目标渲染目标 |
| **参数** | `src` — 源渲染目标；`dst` — 目标渲染目标；`srcOffset` — 源偏移；`size` — 传输大小（Vector3::ZERO 表示全部复制）；`dstOffset` — 目标偏移 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:2922 |
| **内部 Vulkan API** | 无直接 Vulkan 调用（当 src 为 E_RT_TEXTURE 时委托到 `blit(Texture*, RenderTarget*)`，该函数内部调用完整的 Vulkan blit 流程） |
| **问题** | 仅处理 src 类型为 E_RT_TEXTURE 的情况，通过委托到 blit(Texture*, RenderTarget*) 实现。当 src 为 E_RT_WINDOW 类型时直接返回 T3D_OK，不执行任何操作 |

### 17.2 blit (Texture → RenderTarget)

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源纹理传输图像数据到目标渲染目标。完整实现 image blit 流程：获取 src/dst VkImage → image barrier（src→TRANSFER_SRC）→ 计算 blit region → vkCmdBlitImage（LINEAR filter）→ image barrier（src→SHADER_READ_ONLY）。支持 dst 为 Window（swapchain image）或 Texture 类型 |
| **参数** | `src` — 源纹理；`dst` — 目标渲染目标；`srcOffset` — 源偏移；`size` — 传输大小（Vector3::ZERO 表示全部复制）；`dstOffset` — 目标偏移 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:2938 |
| **内部 Vulkan API** | `vkCmdPipelineBarrier`（通过 `insertImageBarrier`）— src image layout transition → TRANSFER_SRC |
| | `vkCmdBlitImage` — 执行图像 blit（`VK_FILTER_LINEAR`），支持缩放 |
| | `vkCmdPipelineBarrier`（通过 `insertImageBarrier`）— src image layout transition → SHADER_READ_ONLY（恢复可采样状态） |

### 17.3 blit (RenderTarget → Texture)

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源渲染目标传输图像数据到目标纹理 |
| **参数** | `src` — 源渲染目标；`dst` — 目标纹理；`srcOffset` — 源偏移；`size` — 传输大小；`dstOffset` — 目标偏移 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:3035 |
| **问题** | 标记 `// TODO: implement when needed`，函数体仅 `return T3D_OK` |

### 17.4 blit (Texture → Texture)

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)` |
| **功能** | 从源纹理传输图像数据到目标纹理 |
| **参数** | `src` — 源纹理；`dst` — 目标纹理；`srcOffset` — 源偏移；`size` — 传输大小；`dstOffset` — 目标偏移 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:3043 |
| **问题** | 标记 `// TODO: implement when needed`，函数体仅 `return T3D_OK` |

### 17.5 copyBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)` |
| **功能** | 从源缓冲区复制数据到目标缓冲区。使用 single-time command buffer 执行 vkCmdCopyBuffer |
| **参数** | `src` — 源渲染缓冲区；`dst` — 目标渲染缓冲区；`srcOffset` — 源缓冲区起始偏移（默认 0）；`size` — 复制数据的大小（默认 0 表示全部）；`dstOffset` — 目标缓冲区起始偏移（默认 0） |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:3051 |
| **内部 Vulkan API** | `vkAllocateCommandBuffers` / `vkBeginCommandBuffer`（通过 `beginSingleTimeCommands`） |
| | `vkCmdCopyBuffer` — 执行 GPU 端缓冲区拷贝 |
| | `vkEndCommandBuffer` / `vkQueueSubmit` / `vkQueueWaitIdle` / `vkFreeCommandBuffers`（通过 `endSingleTimeCommands`） |

### 17.6 writeBuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer)` |
| **功能** | 写 GPU 缓冲区。通过 vkMapMemory/memcpy/vkUnmapMemory 将 CPU 端数据写入 GPU 端缓冲区。支持 VertexBuffer/IndexBuffer/ConstantBuffer 类型，通过 dynamic_cast 获取对应的 VkDeviceMemory |
| **参数** | `renderBuffer` — GPU 缓冲区；`buffer` — CPU 端数据缓冲区；`discardWholeBuffer` — 是否丢弃旧数据（Vulkan 实现中目前未区分 discard 和 no-overwrite，始终执行全量映射写入） |
| **返回值** | `T3D_OK`，失败返回 `T3D_ERR_VK_MAP_MEMORY` |
| **实现位置** | T3DVKContext.cpp:3069 |
| **内部 Vulkan API** | `vkMapMemory` — 映射 GPU 端 VkDeviceMemory 到 CPU 地址空间 |
| | `vkUnmapMemory` — 解除映射 |

---

## 18. 帧命令 (Frame Commands)

### 18.1 beginRender

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult beginRender()` |
| **功能** | 开始一帧的渲染命令录制。执行：vkWaitForFences（等待当前帧 fence）→ vkResetFences → vkResetDescriptorPool（回收上一帧 descriptor set）→ vkAcquireNextImageKHR → vkResetCommandBuffer → vkBeginCommandBuffer → image barrier（swapchain image → TRANSFER_DST） |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:516 |
| **内部 Vulkan API** | `vkWaitForFences` — 等待当前帧 fence 完成 |
| | `vkResetFences` — 重置 fence |
| | `vkResetDescriptorPool` — 回收上一帧分配的所有 descriptor set |
| | `vkAcquireNextImageKHR` — 获取下一个可用 swapchain image |
| | `vkResetCommandBuffer` — 重置 command buffer |
| | `vkBeginCommandBuffer` — 开始录制 command buffer |
| | `vkCmdPipelineBarrier`（通过 `insertImageBarrier`）— swapchain image layout transition UNDEFINED → TRANSFER_DST |
| **备注** | 与 D3D11 不同（按设计为空），Vulkan 必须显式管理帧同步和 command buffer 生命周期 |

### 18.2 endRender

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult endRender()` |
| **功能** | 结束一帧的渲染命令录制并提交。执行：image barrier（swapchain image → PRESENT_SRC）→ vkEndCommandBuffer → vkQueueSubmit（带 semaphore 同步） |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:554 |
| **内部 Vulkan API** | `vkCmdPipelineBarrier`（通过 `insertImageBarrier`）— swapchain image layout transition TRANSFER_DST → PRESENT_SRC |
| | `vkEndCommandBuffer` — 结束 command buffer 录制 |
| | `vkQueueSubmit` — 提交 command buffer（带 wait/signal semaphore + fence 同步） |

### 18.3 beginPass

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult beginPass()` |
| **功能** | 开始一个渲染 Pass。根据 RenderTarget 类型（Window/Texture）自动处理 image layout transition，调用 getOrCreateRenderPass/getOrCreateFramebuffer，执行 vkCmdBeginRenderPass，设置 viewport 和 scissor。支持 depth-only pass（如 shadow map） |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:763 |
| **内部 Vulkan API** | **Window 路径**： |
| | 　`vkCmdPipelineBarrier`（通过 `insertImageBarrier`）— swapchain image → COLOR_ATTACHMENT_OPTIMAL |
| | 　`vkCmdPipelineBarrier`（通过 `insertImageBarrier`）— depth image → DEPTH_STENCIL_ATTACHMENT_OPTIMAL |
| | 　`vkCmdBeginRenderPass` — 开始 render pass |
| | 　`vkCmdSetViewport` — 设置动态 viewport（负高度翻转 Y） |
| | 　`vkCmdSetScissor` — 设置动态 scissor rect |
| | **Texture 路径**： |
| | 　`vkCmdPipelineBarrier`（通过 `insertImageBarrier`）— color RT → COLOR_ATTACHMENT_OPTIMAL |
| | 　`vkCmdPipelineBarrier`（通过 `insertImageBarrier`）— depth RT → DEPTH_STENCIL_ATTACHMENT_OPTIMAL |
| | 　`vkCreateRenderPass`（通过 `getOrCreateRenderPass`，缓存未命中时） |
| | 　`vkCreateFramebuffer`（通过 `getOrCreateFramebuffer`，缓存未命中时） |
| | 　`vkCmdBeginRenderPass` — 开始 render pass |
| | 　`vkCmdSetViewport` / `vkCmdSetScissor` |

### 18.4 endPass

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult endPass()` |
| **功能** | 结束一个渲染 Pass。执行 vkCmdEndRenderPass，根据 RenderTarget 类型执行 layout transition：Window → TRANSFER_DST（用于后续 blit）；Texture → SHADER_READ_ONLY（用于后续采样，包括 color 和 depth attachment） |
| **参数** | 无 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:930 |
| **内部 Vulkan API** | `vkCmdEndRenderPass` — 结束 render pass |
| | **Window 路径**：`vkCmdPipelineBarrier`（通过 `insertImageBarrier`）— swapchain image PRESENT_SRC → TRANSFER_DST（为后续 blit 准备） |
| | **Texture 路径**：`vkCmdPipelineBarrier`（通过 `insertImageBarrier`）— color attachment → SHADER_READ_ONLY |
| | 　　　　　　`vkCmdPipelineBarrier`（通过 `insertImageBarrier`）— depth attachment → SHADER_READ_ONLY（用于 shadow map 采样） |

---

## 19. 非 RHIContext 虚接口（有默认实现）

### 19.1 getNativeContext

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `void* getNativeContext() const` |
| **功能** | 获取主渲染 context 的原生句柄。Vulkan 按设计返回 nullptr（可考虑返回 VkDevice 或 VkInstance） |
| **参数** | 无 |
| **返回值** | `nullptr` |
| **实现位置** | 基类默认实现 (T3DRHIContext.h) |

### 19.2 restoreNativeContext

| 项目 | 内容 |
|------|------|
| **状态** | 🔇 按设计为空 |
| **签名** | `void restoreNativeContext()` |
| **功能** | 恢复主窗口的渲染 context。Vulkan 不需要 context 切换 |
| **参数** | 无 |
| **返回值** | 无 |
| **实现位置** | 基类默认实现 (T3DRHIContext.h) |

---

## 20. VKContext 专有接口（非 RHIContext 纯虚接口）

### 20.1 resizeRenderWindow

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult resizeRenderWindow(VKRenderWindow *rw, uint32_t w, uint32_t h)` |
| **功能** | 改变渲染窗口的大小。执行 vkDeviceWaitIdle → 销毁旧资源（Framebuffer/ImageView/DepthImage/旧 SwapChain）→ 重新创建 SwapChain/ImageView/DepthBuffer/Framebuffer |
| **参数** | `rw` — VK 渲染窗口对象；`w` — 新宽度；`h` — 新高度 |
| **返回值** | `T3D_OK` |
| **实现位置** | T3DVKContext.cpp:1253 |
| **内部 Vulkan API** | `vkDeviceWaitIdle` — 等待设备空闲 |
| | `vkDestroyFramebuffer` — 销毁旧 Framebuffer（逐个） |
| | `vkDestroyImageView` — 销毁旧 Swapchain ImageView（逐个） |
| | `vkDestroyImageView` / `vkDestroyImage` / `vkFreeMemory` — 销毁旧深度缓冲 |
| | `vkDestroySwapchainKHR` — 销毁旧 Swapchain |
| | 之后调用 `createRenderWindow` 重建（包含 `vkCreateSwapchainKHR` / `vkGetSwapchainImagesKHR` / `vkCreateImageView` / `vkCreateImage` / `vkCreateFramebuffer` 等） |

### 20.2 resizeRenderTexture

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult resizeRenderTexture(RenderTexture *rt, uint32_t w, uint32_t h)` |
| **功能** | 改变渲染纹理的大小 |
| **参数** | `rt` — 要改变大小的渲染纹理对象；`w` — 新宽度；`h` — 新高度 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:1291 |
| **问题** | 标记 `// TODO: implement when render texture resize is needed`，没有重建 VkImage/VkImageView 的逻辑 |

### 20.3 resizeRenderTarget

| 项目 | 内容 |
|------|------|
| **状态** | ❌ 未实现 |
| **签名** | `TResult resizeRenderTarget(RenderTarget *rt, uint32_t w, uint32_t h)` |
| **功能** | 改变渲染目标的大小 |
| **参数** | `rt` — 要改变大小的渲染目标；`w` — 新宽度；`h` — 新高度 |
| **返回值** | 当前直接返回 `T3D_OK`，无实际操作 |
| **实现位置** | T3DVKContext.cpp:1299 |
| **问题** | 标记 `// TODO: dispatch to resizeRenderWindow or resizeRenderTexture`，没有分发逻辑 |

---

## 21. VKContext 内部方法（Vulkan 特有）

> 以下方法为 VKContext 实现的内部辅助方法，非 RHIContext 接口，但对 Vulkan 渲染管线至关重要。

### 21.1 getOrCreateRenderPass

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `VkRenderPass getOrCreateRenderPass(VkFormat colorFormat, VkFormat depthFormat, bool hasColor, bool depthOnly)` |
| **功能** | 根据 color/depth 格式和配置参数，从缓存中获取或创建新的 VkRenderPass |
| **实现位置** | T3DVKContext.cpp:594 |
| **内部 Vulkan API** | `vkCreateRenderPass` — 创建新的 RenderPass（配置 color/depth attachment、subpass、dependency）（缓存未命中时） |

### 21.2 getOrCreateFramebuffer

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `VkFramebuffer getOrCreateFramebuffer(VkRenderPass renderPass, ...)` |
| **功能** | 根据 render pass 和 attachment 信息，从缓存中获取或创建新的 VkFramebuffer |
| **实现位置** | T3DVKContext.cpp:688 |
| **内部 Vulkan API** | `vkCreateFramebuffer` — 创建新的 Framebuffer（关联 render pass + attachment ImageView）（缓存未命中时） |

### 21.3 getOrCreatePipeline

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `VkPipeline getOrCreatePipeline(...)` |
| **功能** | 基于 hash 的 Graphics Pipeline 缓存系统。根据当前所有渲染状态（shader module/vertex input/rasterizer/blend/depth stencil/topology/render pass）计算 hash，缓存命中则复用，否则创建新的 VkPipeline |
| **实现位置** | T3DVKContext.cpp:2447 |
| **内部 Vulkan API** | `vkCreateDescriptorSetLayout`（通过 `getOrCreateDescriptorSetLayout`） |
| | `vkCreatePipelineLayout`（通过 `getOrCreatePipelineLayout`） |
| | `vkCreateGraphicsPipelines` — 创建图形 pipeline（缓存未命中时） |

### 21.4 getOrCreateDescriptorSetLayout

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `VkDescriptorSetLayout getOrCreateDescriptorSetLayout(...)` |
| **功能** | 根据 VS/PS binding 信息创建或复用 VkDescriptorSetLayout |
| **实现位置** | T3DVKContext.cpp:2360 |
| **内部 Vulkan API** | `vkCreateDescriptorSetLayout` — 创建 descriptor set layout（合并 VS/PS 的 binding，处理 stageFlags 合并）（缓存未命中时） |

### 21.5 getOrCreatePipelineLayout

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `VkPipelineLayout getOrCreatePipelineLayout(VkDescriptorSetLayout descriptorSetLayout)` |
| **功能** | 根据 descriptor set layout 创建或复用 VkPipelineLayout |
| **实现位置** | T3DVKContext.cpp:2422 |
| **内部 Vulkan API** | `vkCreatePipelineLayout` — 创建 pipeline layout（缓存未命中时） |

### 21.6 bindDescriptorSet

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `TResult bindDescriptorSet(VkCommandBuffer cmdBuf, VkPipelineLayout pipelineLayout, VkDescriptorSetLayout descriptorSetLayout)` |
| **功能** | 从当前帧的 descriptor pool 分配 descriptor set，更新 UBO/纹理/采样器绑定，调用 vkCmdBindDescriptorSets |
| **实现位置** | T3DVKContext.cpp:2649 |
| **内部 Vulkan API** | `vkAllocateDescriptorSets` — 从帧 descriptor pool 分配 descriptor set |
| | `vkUpdateDescriptorSets` — 批量更新 descriptor（UBO → `VkDescriptorBufferInfo` / 纹理 → `VkDescriptorImageInfo` / 采样器 → `VkDescriptorImageInfo`） |
| | `vkCmdBindDescriptorSets` — 绑定 descriptor set 到 command buffer |

### 21.7 insertImageBarrier

| 项目 | 内容 |
|------|------|
| **状态** | ✅ 已完成 |
| **签名** | `void insertImageBarrier(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, ...)` |
| **功能** | 插入 VkImageMemoryBarrier 实现 image layout transition |
| **实现位置** | T3DVKContext.cpp:992 |
| **内部 Vulkan API** | `vkCmdPipelineBarrier` — 插入 pipeline barrier（`VkImageMemoryBarrier`），控制 image layout transition 和内存同步 |

### 21.8 辅助函数

| 方法 | 状态 | 功能 | 内部 Vulkan API |
|------|------|------|------|
| `findMemoryType()` | ✅ 已完成 | 查找满足条件的 GPU 内存类型索引 | `vkGetPhysicalDeviceMemoryProperties` |
| `createVkBuffer()` | ✅ 已完成 | 创建 VkBuffer + 分配 VkDeviceMemory | `vkCreateBuffer` → `vkGetBufferMemoryRequirements` → `vkAllocateMemory` → `vkBindBufferMemory` |
| `createVkImage()` | ✅ 已完成 | 创建 VkImage + 分配 VkDeviceMemory | `vkCreateImage` → `vkGetImageMemoryRequirements` → `vkAllocateMemory` → `vkBindImageMemory` |
| `createVkImageView()` | ✅ 已完成 | 创建 VkImageView | `vkCreateImageView` |
| `beginSingleTimeCommands()` | ✅ 已完成 | 创建一次性 command buffer 并 begin | `vkAllocateCommandBuffers` → `vkBeginCommandBuffer` |
| `endSingleTimeCommands()` | ✅ 已完成 | 结束并提交一次性 command buffer，等待完成 | `vkEndCommandBuffer` → `vkQueueSubmit` → `vkQueueWaitIdle` → `vkFreeCommandBuffers` |
| `transitionImageLayout()` | ✅ 已完成 | 使用一次性 command buffer 执行 image layout transition | `beginSingleTimeCommands` + `vkCmdPipelineBarrier` + `endSingleTimeCommands` |
| `swapBackBuffer()` | ✅ 已完成 | vkQueuePresentKHR + 帧索引递进 | `vkQueuePresentKHR` |

---

## 汇总统计

| 状态 | 数量 | 接口 |
|------|------|------|
| ✅ 已完成 | 34 | 核心渲染管线（VS/PS 创建/设置、缓冲区创建/绑定、pipeline/descriptor/render pass 管理、draw call、blit(Tex→RT)、copyBuffer、writeBuffer、帧命令） |
| ⚠️ 部分完成 | 1 | `blit(RT→RT)`：仅处理 src 为 Texture 类型的情况 |
| ❌ 未实现 | 24 | `setRenderTarget` MRT 重载 ×2、`createPixelBuffer3D`、`setVSPixelBuffers`、`setVSSamplers`、Hull Shader 全套 ×4、Domain Shader 全套 ×4、Geometry Shader 全套 ×4、Compute Shader 全套 ×4、`blit(RT→Tex)`、`blit(Tex→Tex)`、`resizeRenderTexture`、`resizeRenderTarget` |
| 🔇 按设计为空 | 4 | `setViewport`（viewport 在录制 command buffer 时动态设置）、`reflectSamplerBindings`（已在 reflectShaderAllBindings 中处理）、`getNativeContext`、`restoreNativeContext` |
| 🦴 空壳实现 | 5 | `createPixelBuffer1D`、`createHullShader`、`createDomainShader`、`createGeometryShader`、`createComputeShader` |

---

## 与 D3D11 实现状态对比

| 功能类别 | D3D11 | Vulkan | 说明 |
|----------|-------|--------|------|
| 核心管线 (VS/PS) | ✅ | ✅ | 两者均完整实现 |
| 帧命令 (begin/endRender/Pass) | 🔇 按设计为空 | ✅ 已完成 | Vulkan 必须显式管理，D3D11 不需要 |
| Hull/Domain Shader | ✅ 已完成 | ❌/🦴 未实现 | D3D11 全部实现，Vulkan 仅空壳 |
| Geometry Shader | ✅ 已完成 | ❌/🦴 未实现 | D3D11 全部实现，Vulkan 仅空壳 |
| Compute Shader | ⚠️ 部分完成 | ❌/🦴 未实现 | D3D11 部分实现，Vulkan 仅空壳 |
| setViewport | ✅ 已完成 | 🔇 按设计为空 | Vulkan 在 render 过程中动态设置 |
| blit (各重载) | ⚠️ 部分完成 | ⚠️ 部分完成 | 两者都有未实现的 blit 重载 |
| copyBuffer | ❌ 未实现 | ✅ 已完成 | Vulkan 已实现，D3D11 未实现 |
| 1D/3D 纹理 | ❌ 未实现 | ❌/🦴 未实现 | 两者均未实现 |
| MRT setRenderTarget 重载 | N/A | ❌ 未实现 | Vulkan 的两个 MRT 重载为空 |

---

## 待办优先级

| 优先级 | 接口 | 原因 |
|--------|------|------|
| **P1 - 核心功能** | `blit(RT→Tex)` / `blit(Tex→Tex)` | 数据传输是渲染管线核心功能，shadow map 等功能可能需要 |
| **P1 - 核心功能** | `blit(RT→RT)` 补全 Window src 分支 | 当前仅支持 Texture 类型 src |
| **P1 - 核心功能** | `setRenderTarget` MRT 重载 | 多渲染目标（延迟渲染等）需要 |
| **P2 - 着色器阶段** | Hull/Domain Shader 全套 | 曲面细分功能需要完整的 TCS/TES pipeline 支持 |
| **P2 - 着色器阶段** | Geometry Shader 全套 | 几何处理功能需要 |
| **P2 - 着色器阶段** | Compute Shader 全套 | GPU 计算需要完整的 compute pipeline 支持 |
| **P2 - 资源创建** | `createPixelBuffer1D` / `createPixelBuffer3D` | 1D/3D 纹理资源创建缺失 |
| **P2 - 资源绑定** | `setVSPixelBuffers` / `setVSSamplers` | 顶点着色器纹理采样（如 vertex texture fetch）需要 |
| **P3 - 窗口管理** | `resizeRenderTexture` / `resizeRenderTarget` | 渲染纹理大小变化时需要 |
