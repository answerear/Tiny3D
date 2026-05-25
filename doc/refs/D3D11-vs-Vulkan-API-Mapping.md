# D3D11 vs Vulkan API 映射参考

> 以 Direct3D 11 API 为基准，列出 Vulkan 中对应能力的接口，并提供典型使用代码示例。
>
> 命名模式：`D3D11-vs-{API}-API-Mapping.md`

---

## 1. 初始化与设备

### 概念对比

| 维度 | D3D11 | Vulkan |
|------|-------|--------|
| 设计理念 | 驱动隐式管理，API 精简 | 应用显式管理，API 底层 |
| 设备创建 | 单步完成（Device + Context） | 多步（Instance → PhysicalDevice → Device） |
| 队列管理 | 隐式（Context 内部） | 显式（Queue Family + Queue） |

### 接口映射

| D3D11 | Vulkan | 说明 |
|-------|--------|------|
| `D3D11CreateDevice` | `vkCreateInstance` + `vkCreateDevice` | D3D11 一步，Vulkan 分两步 |
| `ID3D11Device` | `VkDevice` | 逻辑设备 |
| `ID3D11DeviceContext` | `VkQueue` + `VkCommandBuffer` | 命令录制与提交 |
| `IDXGIAdapter` | `VkPhysicalDevice` | 物理显卡 |
| `IDXGIFactory` | `VkInstance` | 顶层管理对象 |
| `D3D_FEATURE_LEVEL` | `VkPhysicalDeviceFeatures` / `Properties` | 能力查询 |

### 使用示例

**D3D11**

```cpp
ID3D11Device* device = nullptr;
ID3D11DeviceContext* context = nullptr;
D3D_FEATURE_LEVEL featureLevel;
D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };

HRESULT hr = D3D11CreateDevice(
    nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
    D3D11_CREATE_DEVICE_DEBUG, levels, 1, D3D11_SDK_VERSION,
    &device, &featureLevel, &context);
```

**Vulkan**

```cpp
// 1. 创建 Instance
VkApplicationInfo appInfo{};
appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
appInfo.apiVersion = VK_API_VERSION_1_0;

VkInstanceCreateInfo instanceCI{};
instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
instanceCI.pApplicationInfo = &appInfo;
const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
instanceCI.enabledLayerCount = 1;
instanceCI.ppEnabledLayerNames = layers;

VkInstance instance;
vkCreateInstance(&instanceCI, nullptr, &instance);

// 2. 选择物理设备
uint32_t count = 0;
vkEnumeratePhysicalDevices(instance, &count, nullptr);
std::vector<VkPhysicalDevice> gpus(count);
vkEnumeratePhysicalDevices(instance, &count, gpus.data());
VkPhysicalDevice physicalDevice = gpus[0];

// 3. 查找图形队列族
uint32_t qfCount = 0;
vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &qfCount, nullptr);
std::vector<VkQueueFamilyProperties> qfs(qfCount);
vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &qfCount, qfs.data());
uint32_t graphicsFamily = 0;
for (uint32_t i = 0; i < qfCount; i++)
    if (qfs[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) { graphicsFamily = i; break; }

// 4. 创建逻辑设备
float priority = 1.0f;
VkDeviceQueueCreateInfo queueCI{};
queueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
queueCI.queueFamilyIndex = graphicsFamily;
queueCI.queueCount = 1;
queueCI.pQueuePriorities = &priority;

VkDeviceCreateInfo deviceCI{};
deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
deviceCI.queueCreateInfoCount = 1;
deviceCI.pQueueCreateInfos = &queueCI;

VkDevice device;
vkCreateDevice(physicalDevice, &deviceCI, nullptr, &device);

VkQueue graphicsQueue;
vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
```

---

## 2. 交换链与呈现

### 概念对比

| 维度 | D3D11 | Vulkan |
|------|-------|--------|
| 交换链归属 | DXGI 层 | KHR 扩展 |
| Surface | 隐含在 HWND | 显式 VkSurfaceKHR |
| 图像获取 | 自动 | 手动 vkAcquireNextImageKHR |

### 接口映射

| D3D11 / DXGI | Vulkan | 说明 |
|-------|--------|------|
| `IDXGIFactory::CreateSwapChain` | `vkCreateSwapchainKHR` | 创建交换链 |
| `IDXGISwapChain` | `VkSwapchainKHR` | 交换链对象 |
| `IDXGISwapChain::Present` | `vkQueuePresentKHR` | 呈现 |
| `IDXGISwapChain::GetBuffer` | `vkGetSwapchainImagesKHR` | 获取后缓冲 |
| `IDXGISwapChain::ResizeBuffers` | 销毁+重建 Swapchain | 窗口 resize |
| — | `VkSurfaceKHR` / `vkCreateWin32SurfaceKHR` | Vulkan 独有 |
| — | `vkAcquireNextImageKHR` | 获取下一帧图像 |

### 使用示例

**D3D11**

```cpp
DXGI_SWAP_CHAIN_DESC desc{};
desc.BufferCount = 2;
desc.BufferDesc.Width = 1280;
desc.BufferDesc.Height = 720;
desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
desc.OutputWindow = hwnd;
desc.SampleDesc.Count = 1;
desc.Windowed = TRUE;
desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

IDXGISwapChain* swapChain = nullptr;
dxgiFactory->CreateSwapChain(device, &desc, &swapChain);
swapChain->Present(1, 0); // VSync
```

**Vulkan**

```cpp
// 创建 Surface
VkWin32SurfaceCreateInfoKHR surfaceCI{};
surfaceCI.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
surfaceCI.hwnd = hwnd;
surfaceCI.hinstance = hInstance;
VkSurfaceKHR surface;
vkCreateWin32SurfaceKHR(instance, &surfaceCI, nullptr, &surface);

// 创建 Swapchain
VkSurfaceCapabilitiesKHR caps;
vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);

VkSwapchainCreateInfoKHR swapCI{};
swapCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
swapCI.surface = surface;
swapCI.minImageCount = 2;
swapCI.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
swapCI.imageExtent = { 1280, 720 };
swapCI.imageArrayLayers = 1;
swapCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
swapCI.presentMode = VK_PRESENT_MODE_FIFO_KHR;

VkSwapchainKHR swapchain;
vkCreateSwapchainKHR(device, &swapCI, nullptr, &swapchain);

// 获取图像
uint32_t imgCount;
vkGetSwapchainImagesKHR(device, swapchain, &imgCount, nullptr);
std::vector<VkImage> images(imgCount);
vkGetSwapchainImagesKHR(device, swapchain, &imgCount, images.data());

// 呈现
VkPresentInfoKHR presentInfo{};
presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
presentInfo.swapchainCount = 1;
presentInfo.pSwapchains = &swapchain;
presentInfo.pImageIndices = &imageIndex;
presentInfo.waitSemaphoreCount = 1;
presentInfo.pWaitSemaphores = &renderFinished;
vkQueuePresentKHR(queue, &presentInfo);
```

---

## 3. 命令录制与提交

### 概念对比

| 维度 | D3D11 | Vulkan |
|------|-------|--------|
| 模式 | 即时模式（调用即执行） | 显式录制 → 提交 |
| 多线程 | Deferred Context（可选） | 天然多 CommandBuffer 并行录制 |
| 命令池 | 不存在 | VkCommandPool 管理分配 |

### 接口映射

| D3D11 | Vulkan | 说明 |
|-------|--------|------|
| `ID3D11DeviceContext` (Immediate) | `VkCommandBuffer` | 命令录制 |
| `ID3D11DeviceContext` (Deferred) | `VkCommandBuffer` (Secondary) | 多线程录制 |
| — | `VkCommandPool` / `vkAllocateCommandBuffers` | 分配 |
| — | `vkBeginCommandBuffer` / `vkEndCommandBuffer` | 开始/结束录制 |
| — | `vkQueueSubmit` | 提交到 GPU |
| `ExecuteCommandList` | `vkCmdExecuteCommands` | 执行子命令 |

### 使用示例

**D3D11**

```cpp
// 即时模式 — 调用即执行
context->ClearRenderTargetView(rtv, clearColor);
context->DrawIndexed(indexCount, 0, 0);
```

**Vulkan**

```cpp
// 创建 Command Pool
VkCommandPoolCreateInfo poolCI{};
poolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
poolCI.queueFamilyIndex = graphicsFamily;
poolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
VkCommandPool cmdPool;
vkCreateCommandPool(device, &poolCI, nullptr, &cmdPool);

// 分配 Command Buffer
VkCommandBufferAllocateInfo allocInfo{};
allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
allocInfo.commandPool = cmdPool;
allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
allocInfo.commandBufferCount = 1;
VkCommandBuffer cmdBuf;
vkAllocateCommandBuffers(device, &allocInfo, &cmdBuf);

// 录制
VkCommandBufferBeginInfo beginInfo{};
beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
vkBeginCommandBuffer(cmdBuf, &beginInfo);
// ... 渲染命令 ...
vkEndCommandBuffer(cmdBuf);

// 提交
VkSubmitInfo submitInfo{};
submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
submitInfo.commandBufferCount = 1;
submitInfo.pCommandBuffers = &cmdBuf;
vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence);
```

---

## 4. 渲染通道

### 概念对比

| 维度 | D3D11 | Vulkan |
|------|-------|--------|
| 渲染通道 | 隐式（绑定 RTV/DSV 即开始） | 显式 VkRenderPass |
| Attachment | 无需提前声明 | 需描述 load/store 操作 |
| Subpass | 不存在 | 支持多 subpass（tile-based GPU 优化） |
| Framebuffer | 隐含在 RTV/DSV | 显式 VkFramebuffer |

### 接口映射

| D3D11 | Vulkan | 说明 |
|-------|--------|------|
| `OMSetRenderTargets` | `vkCmdBeginRenderPass` | 开始渲染 |
| — (隐式结束) | `vkCmdEndRenderPass` | 结束渲染 |
| — | `VkRenderPass` / `vkCreateRenderPass` | 定义 attachment 布局 |
| — | `VkFramebuffer` / `vkCreateFramebuffer` | 绑定 ImageView |

### 使用示例

**D3D11**

```cpp
ID3D11RenderTargetView* rtvs[] = { backBufferRTV };
context->OMSetRenderTargets(1, rtvs, dsv);
float clear[] = { 0, 0, 0, 1 };
context->ClearRenderTargetView(backBufferRTV, clear);
context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
context->DrawIndexed(count, 0, 0);
```

**Vulkan**

```cpp
// 创建 RenderPass（一次性）
VkAttachmentDescription attachments[2]{};
attachments[0].format = VK_FORMAT_B8G8R8A8_UNORM;
attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
attachments[1].format = VK_FORMAT_D24_UNORM_S8_UINT;
attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

VkAttachmentReference colorRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
VkAttachmentReference depthRef = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
VkSubpassDescription subpass{};
subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
subpass.colorAttachmentCount = 1;
subpass.pColorAttachments = &colorRef;
subpass.pDepthStencilAttachment = &depthRef;

VkRenderPassCreateInfo rpCI{};
rpCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
rpCI.attachmentCount = 2;
rpCI.pAttachments = attachments;
rpCI.subpassCount = 1;
rpCI.pSubpasses = &subpass;
VkRenderPass renderPass;
vkCreateRenderPass(device, &rpCI, nullptr, &renderPass);

// 每帧：开始 RenderPass
VkClearValue clears[2];
clears[0].color = {{0, 0, 0, 1}};
clears[1].depthStencil = {1.0f, 0};

VkRenderPassBeginInfo rpBegin{};
rpBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
rpBegin.renderPass = renderPass;
rpBegin.framebuffer = framebuffer;
rpBegin.renderArea = {{0, 0}, {1280, 720}};
rpBegin.clearValueCount = 2;
rpBegin.pClearValues = clears;
vkCmdBeginRenderPass(cmdBuf, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);
// ... 绘制 ...
vkCmdEndRenderPass(cmdBuf);
```

---

## 5. 缓冲区

### 概念对比

| 维度 | D3D11 | Vulkan |
|------|-------|--------|
| 内存管理 | 驱动自动 | 应用显式分配 VkDeviceMemory |
| 创建步骤 | 一步 CreateBuffer | 三步：Create + Allocate + Bind |
| Staging | 通过 Usage/CPUAccess 区分 | 显式 staging buffer + copy |

### 接口映射

| D3D11 | Vulkan | 说明 |
|-------|--------|------|
| `ID3D11Device::CreateBuffer` | `vkCreateBuffer` + `vkAllocateMemory` + `vkBindBufferMemory` | 创建缓冲 |
| `ID3D11Buffer` | `VkBuffer` + `VkDeviceMemory` | 缓冲+内存分离 |
| `IASetVertexBuffers` | `vkCmdBindVertexBuffers` | 绑定 VB |
| `IASetIndexBuffer` | `vkCmdBindIndexBuffer` | 绑定 IB |
| `VSSetConstantBuffers` / `PSSetConstantBuffers` | `vkUpdateDescriptorSets` | 通过 Descriptor 绑定 UBO |

### 使用示例

**D3D11**

```cpp
D3D11_BUFFER_DESC vbDesc{};
vbDesc.ByteWidth = sizeof(vertices);
vbDesc.Usage = D3D11_USAGE_DEFAULT;
vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
D3D11_SUBRESOURCE_DATA initData{ vertices };
ID3D11Buffer* vb;
device->CreateBuffer(&vbDesc, &initData, &vb);

UINT stride = sizeof(Vertex), offset = 0;
context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
```

**Vulkan**

```cpp
// 创建 Buffer
VkBufferCreateInfo bufCI{};
bufCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
bufCI.size = sizeof(vertices);
bufCI.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
VkBuffer vb;
vkCreateBuffer(device, &bufCI, nullptr, &vb);

// 分配内存
VkMemoryRequirements memReqs;
vkGetBufferMemoryRequirements(device, vb, &memReqs);
VkMemoryAllocateInfo allocInfo{};
allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
allocInfo.allocationSize = memReqs.size;
allocInfo.memoryTypeIndex = findMemoryType(memReqs.memoryTypeBits,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
VkDeviceMemory mem;
vkAllocateMemory(device, &allocInfo, nullptr, &mem);
vkBindBufferMemory(device, vb, mem, 0);

// 上传数据
void* data;
vkMapMemory(device, mem, 0, sizeof(vertices), 0, &data);
memcpy(data, vertices, sizeof(vertices));
vkUnmapMemory(device, mem);

// 绑定
VkDeviceSize offsets[] = { 0 };
vkCmdBindVertexBuffers(cmdBuf, 0, 1, &vb, offsets);
```

---

## 6. 纹理与图像

### 概念对比

| 维度 | D3D11 | Vulkan |
|------|-------|--------|
| 纹理对象 | `ID3D11Texture2D` 统一 | `VkImage` + `VkImageView` 分离 |
| 布局转换 | 驱动自动 | 显式 Image Layout Transition |
| 数据上传 | `UpdateSubresource` / Map | staging buffer + `vkCmdCopyBufferToImage` |

### 接口映射

| D3D11 | Vulkan | 说明 |
|-------|--------|------|
| `CreateTexture2D` | `vkCreateImage` + `vkAllocateMemory` + `vkBindImageMemory` | 创建纹理 |
| `ID3D11Texture2D` | `VkImage` | 图像对象 |
| `ID3D11ShaderResourceView` | `VkImageView` | 着色器资源视图 |
| `CreateShaderResourceView` | `vkCreateImageView` | 创建视图 |
| `UpdateSubresource` | staging + `vkCmdCopyBufferToImage` | 上传数据 |
| — | `vkCmdPipelineBarrier` (layout transition) | Vulkan 独有 |

### 使用示例

**D3D11**

```cpp
D3D11_TEXTURE2D_DESC texDesc{};
texDesc.Width = 256; texDesc.Height = 256;
texDesc.MipLevels = 1; texDesc.ArraySize = 1;
texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
texDesc.SampleDesc.Count = 1;
texDesc.Usage = D3D11_USAGE_DEFAULT;
texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
D3D11_SUBRESOURCE_DATA init{ pixelData, 256 * 4, 0 };
ID3D11Texture2D* tex;
device->CreateTexture2D(&texDesc, &init, &tex);

D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
srvDesc.Format = texDesc.Format;
srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
srvDesc.Texture2D.MipLevels = 1;
ID3D11ShaderResourceView* srv;
device->CreateShaderResourceView(tex, &srvDesc, &srv);
context->PSSetShaderResources(0, 1, &srv);
```

**Vulkan**

```cpp
// 创建 Image
VkImageCreateInfo imgCI{};
imgCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
imgCI.imageType = VK_IMAGE_TYPE_2D;
imgCI.format = VK_FORMAT_R8G8B8A8_UNORM;
imgCI.extent = { 256, 256, 1 };
imgCI.mipLevels = 1; imgCI.arrayLayers = 1;
imgCI.samples = VK_SAMPLE_COUNT_1_BIT;
imgCI.tiling = VK_IMAGE_TILING_OPTIMAL;
imgCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
VkImage image;
vkCreateImage(device, &imgCI, nullptr, &image);
// ... allocate + bind memory (同 buffer 模式) ...

// Layout transition: UNDEFINED → TRANSFER_DST
VkImageMemoryBarrier barrier{};
barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
barrier.image = image;
barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

// 从 staging buffer 拷贝
VkBufferImageCopy region{};
region.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
region.imageExtent = { 256, 256, 1 };
vkCmdCopyBufferToImage(cmdBuf, stagingBuf, image,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

// Layout transition: TRANSFER_DST → SHADER_READ_ONLY
barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_TRANSFER_BIT,
    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

// 创建 ImageView
VkImageViewCreateInfo viewCI{};
viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
viewCI.image = image;
viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
viewCI.format = VK_FORMAT_R8G8B8A8_UNORM;
viewCI.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
VkImageView imageView;
vkCreateImageView(device, &viewCI, nullptr, &imageView);
```

---

## 7. 采样器

### 接口映射

| D3D11 | Vulkan | 说明 |
|-------|--------|------|
| `CreateSamplerState` | `vkCreateSampler` | 创建采样器 |
| `ID3D11SamplerState` | `VkSampler` | 采样器对象 |
| `PSSetSamplers` | `vkUpdateDescriptorSets` | 通过 Descriptor 绑定 |
| `D3D11_FILTER` | `VkFilter` + `VkSamplerMipmapMode` | 过滤模式拆分 |
| `D3D11_TEXTURE_ADDRESS_MODE` | `VkSamplerAddressMode` | 寻址模式 |

### 使用示例

**D3D11**

```cpp
D3D11_SAMPLER_DESC sd{};
sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
sd.MaxAnisotropy = 16;
sd.MinLOD = 0; sd.MaxLOD = D3D11_FLOAT32_MAX;
ID3D11SamplerState* sampler;
device->CreateSamplerState(&sd, &sampler);
context->PSSetSamplers(0, 1, &sampler);
```

**Vulkan**

```cpp
VkSamplerCreateInfo sampCI{};
sampCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
sampCI.magFilter = VK_FILTER_LINEAR;
sampCI.minFilter = VK_FILTER_LINEAR;
sampCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
sampCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
sampCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
sampCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
sampCI.maxAnisotropy = 16.0f;
sampCI.anisotropyEnable = VK_TRUE;
sampCI.maxLod = VK_LOD_CLAMP_NONE;
VkSampler sampler;
vkCreateSampler(device, &sampCI, nullptr, &sampler);
// 通过 Descriptor Set 绑定（见第 10 节）
```

---

## 8. 着色器

### 概念对比

| 维度 | D3D11 | Vulkan |
|------|-------|--------|
| 语言 | HLSL | GLSL/HLSL → SPIR-V |
| 编译 | 运行时 D3DCompile 或离线 fxc | 离线编译为 SPIR-V |
| 对象 | 按阶段分类（ID3D11VertexShader等） | 统一 VkShaderModule |
| 反射 | D3DReflect | SPIRV-Reflect |

### 接口映射

| D3D11 | Vulkan | 说明 |
|-------|--------|------|
| `D3DCompile` | glslang / dxc → SPIR-V | 编译 |
| `CreateVertexShader` / `CreatePixelShader` 等 | `vkCreateShaderModule` | Vulkan 不区分 stage |
| `VSSetShader` / `PSSetShader` | `VkPipelineShaderStageCreateInfo` | 在 Pipeline 创建时指定 |
| `D3DReflect` | `spvReflectCreateShaderModule` | 反射 |

### 使用示例

**D3D11**

```cpp
ID3DBlob* vsBlob = nullptr;
D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr,
    "VSMain", "vs_5_0", 0, 0, &vsBlob, nullptr);
ID3D11VertexShader* vs;
device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vs);

ID3DBlob* psBlob = nullptr;
D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr,
    "PSMain", "ps_5_0", 0, 0, &psBlob, nullptr);
ID3D11PixelShader* ps;
device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &ps);

context->VSSetShader(vs, nullptr, 0);
context->PSSetShader(ps, nullptr, 0);
```

**Vulkan**

```cpp
// 离线编译: glslangValidator -V shader.vert -o vert.spv
std::vector<char> vertCode = readFile("vert.spv");
std::vector<char> fragCode = readFile("frag.spv");

VkShaderModuleCreateInfo vsCI{};
vsCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
vsCI.codeSize = vertCode.size();
vsCI.pCode = reinterpret_cast<const uint32_t*>(vertCode.data());
VkShaderModule vertModule;
vkCreateShaderModule(device, &vsCI, nullptr, &vertModule);

VkShaderModuleCreateInfo fsCI{};
fsCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
fsCI.codeSize = fragCode.size();
fsCI.pCode = reinterpret_cast<const uint32_t*>(fragCode.data());
VkShaderModule fragModule;
vkCreateShaderModule(device, &fsCI, nullptr, &fragModule);

// 在 Pipeline 创建时指定 stage
VkPipelineShaderStageCreateInfo stages[2]{};
stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
stages[0].module = vertModule;
stages[0].pName = "main";
stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
stages[1].module = fragModule;
stages[1].pName = "main";
```

---

## 9. 管线状态

### 概念对比

| 维度 | D3D11 | Vulkan |
|------|-------|--------|
| 管理方式 | 独立 State Object 分别创建/绑定 | 整合为单个 VkPipeline |
| 混合 | `ID3D11BlendState`（独立） | Pipeline 内 ColorBlendState |
| 光栅化 | `ID3D11RasterizerState`（独立） | Pipeline 内 RasterizationState |
| 深度模板 | `ID3D11DepthStencilState`（独立） | Pipeline 内 DepthStencilState |
| 动态状态 | 部分可随时改 | 通过 VkDynamicState 声明 |

### 接口映射

| D3D11 | Vulkan | 说明 |
|-------|--------|------|
| `CreateBlendState` | `VkPipelineColorBlendStateCreateInfo` | 嵌入 Pipeline |
| `CreateRasterizerState` | `VkPipelineRasterizationStateCreateInfo` | 嵌入 Pipeline |
| `CreateDepthStencilState` | `VkPipelineDepthStencilStateCreateInfo` | 嵌入 Pipeline |
| `CreateInputLayout` | `VkPipelineVertexInputStateCreateInfo` | 嵌入 Pipeline |
| `OMSetBlendState` / `RSSetState` / `OMSetDepthStencilState` | `vkCmdBindPipeline` | 绑定整个 Pipeline |
| `IASetPrimitiveTopology` | `VkPipelineInputAssemblyStateCreateInfo` | 图元拓扑 |
| — | `vkCreateGraphicsPipelines` | 创建整合 Pipeline |

### 使用示例

**D3D11**

```cpp
// 分别创建各状态对象
D3D11_RASTERIZER_DESC rsDesc{};
rsDesc.FillMode = D3D11_FILL_SOLID;
rsDesc.CullMode = D3D11_CULL_BACK;
rsDesc.DepthClipEnable = TRUE;
ID3D11RasterizerState* rsState;
device->CreateRasterizerState(&rsDesc, &rsState);

D3D11_DEPTH_STENCIL_DESC dsDesc{};
dsDesc.DepthEnable = TRUE;
dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
ID3D11DepthStencilState* dsState;
device->CreateDepthStencilState(&dsDesc, &dsState);

// 分别绑定
context->RSSetState(rsState);
context->OMSetDepthStencilState(dsState, 0);
context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
```

**Vulkan**

```cpp
// 所有状态整合在一个 Pipeline 中
VkPipelineRasterizationStateCreateInfo rs{};
rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
rs.polygonMode = VK_POLYGON_MODE_FILL;
rs.cullMode = VK_CULL_MODE_BACK_BIT;
rs.frontFace = VK_FRONT_FACE_CLOCKWISE;
rs.lineWidth = 1.0f;

VkPipelineDepthStencilStateCreateInfo ds{};
ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
ds.depthTestEnable = VK_TRUE;
ds.depthWriteEnable = VK_TRUE;
ds.depthCompareOp = VK_COMPARE_OP_LESS;

VkPipelineInputAssemblyStateCreateInfo ia{};
ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

VkPipelineColorBlendAttachmentState blendAtt{};
blendAtt.colorWriteMask = 0xF;
VkPipelineColorBlendStateCreateInfo blend{};
blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
blend.attachmentCount = 1;
blend.pAttachments = &blendAtt;

// 动态 Viewport/Scissor
VkDynamicState dynStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
VkPipelineDynamicStateCreateInfo dyn{};
dyn.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
dyn.dynamicStateCount = 2;
dyn.pDynamicStates = dynStates;

VkGraphicsPipelineCreateInfo pipeCI{};
pipeCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
pipeCI.stageCount = 2;
pipeCI.pStages = stages;
pipeCI.pVertexInputState = &vertexInput;
pipeCI.pInputAssemblyState = &ia;
pipeCI.pRasterizationState = &rs;
pipeCI.pDepthStencilState = &ds;
pipeCI.pColorBlendState = &blend;
pipeCI.pDynamicState = &dyn;
pipeCI.pViewportState = &viewportState;
pipeCI.pMultisampleState = &ms;
pipeCI.layout = pipelineLayout;
pipeCI.renderPass = renderPass;

VkPipeline pipeline;
vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeCI, nullptr, &pipeline);
vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
```

---

## 10. 资源绑定

### 概念对比

| 维度 | D3D11 | Vulkan |
|------|-------|--------|
| 模型 | Slot-based（按阶段分 slot） | Descriptor Set + Binding |
| 布局 | 隐式（shader 反射） | 显式 DescriptorSetLayout + PipelineLayout |
| 更新 | 直接 Set | `vkUpdateDescriptorSets` 批量更新 |

### 接口映射

| D3D11 | Vulkan | 说明 |
|-------|--------|------|
| `VSSetConstantBuffers` / `PSSetConstantBuffers` | `vkUpdateDescriptorSets` + `vkCmdBindDescriptorSets` | UBO 绑定 |
| `PSSetShaderResources` | `vkUpdateDescriptorSets` (SAMPLED_IMAGE) | 纹理绑定 |
| `PSSetSamplers` | `vkUpdateDescriptorSets` (COMBINED_IMAGE_SAMPLER) | 采样器绑定 |
| Register (b0, t0, s0) | Binding number | 槽位 |
| — | `VkDescriptorSetLayout` / `VkPipelineLayout` | 布局描述 |
| — | `VkDescriptorPool` / `vkAllocateDescriptorSets` | 分配 |

### 使用示例

**D3D11**

```cpp
context->VSSetConstantBuffers(0, 1, &mvpBuffer);
context->PSSetConstantBuffers(0, 1, &materialBuffer);
context->PSSetShaderResources(0, 1, &diffuseSRV);
context->PSSetSamplers(0, 1, &linearSampler);
```

**Vulkan**

```cpp
// 创建 Layout
VkDescriptorSetLayoutBinding bindings[3]{};
bindings[0] = { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT };
bindings[1] = { 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT };
bindings[2] = { 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT };

VkDescriptorSetLayoutCreateInfo layoutCI{};
layoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
layoutCI.bindingCount = 3;
layoutCI.pBindings = bindings;
VkDescriptorSetLayout dsLayout;
vkCreateDescriptorSetLayout(device, &layoutCI, nullptr, &dsLayout);

// 分配 Descriptor Set
VkDescriptorSetAllocateInfo dsAlloc{};
dsAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
dsAlloc.descriptorPool = pool;
dsAlloc.descriptorSetCount = 1;
dsAlloc.pSetLayouts = &dsLayout;
VkDescriptorSet ds;
vkAllocateDescriptorSets(device, &dsAlloc, &ds);

// 更新
VkDescriptorBufferInfo mvpInfo = { mvpBuf, 0, sizeof(MVP) };
VkDescriptorBufferInfo matInfo = { matBuf, 0, sizeof(Material) };
VkDescriptorImageInfo texInfo = { sampler, imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

VkWriteDescriptorSet writes[3]{};
writes[0] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, ds, 0, 0, 1,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &mvpInfo };
writes[1] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, ds, 1, 0, 1,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &matInfo };
writes[2] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, ds, 2, 0, 1,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &texInfo };
vkUpdateDescriptorSets(device, 3, writes, 0, nullptr);

// 绑定
vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
    pipelineLayout, 0, 1, &ds, 0, nullptr);
```

---

## 11. 渲染目标

### 接口映射

| D3D11 | Vulkan | 说明 |
|-------|--------|------|
| `CreateRenderTargetView` | `vkCreateImageView` + RenderPass attachment | RTV |
| `CreateDepthStencilView` | `vkCreateImageView` (DEPTH) + attachment | DSV |
| `OMSetRenderTargets` | `vkCmdBeginRenderPass` | 设置 RT |
| `ClearRenderTargetView` | loadOp=CLEAR 或 `vkCmdClearColorImage` | 清除颜色 |
| `ClearDepthStencilView` | loadOp=CLEAR 或 `vkCmdClearDepthStencilImage` | 清除深度 |

### 使用示例

**D3D11**

```cpp
ID3D11Texture2D* backBuf;
swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuf));
ID3D11RenderTargetView* rtv;
device->CreateRenderTargetView(backBuf, nullptr, &rtv);

D3D11_TEXTURE2D_DESC dd{ 1280, 720, 1, 1, DXGI_FORMAT_D24_UNORM_S8_UINT,
    {1,0}, D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL };
ID3D11Texture2D* depthTex;
device->CreateTexture2D(&dd, nullptr, &depthTex);
ID3D11DepthStencilView* dsv;
device->CreateDepthStencilView(depthTex, nullptr, &dsv);

context->OMSetRenderTargets(1, &rtv, dsv);
context->ClearRenderTargetView(rtv, clearColor);
context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
```

**Vulkan**

```cpp
// RT 由 RenderPass + Framebuffer 管理（见第 4 节）
// 清除在 RenderPass loadOp 中指定，或手动:
VkClearColorValue clr = {{0, 0, 0, 1}};
VkImageSubresourceRange range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
vkCmdClearColorImage(cmdBuf, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clr, 1, &range);

VkClearDepthStencilValue depthClr = { 1.0f, 0 };
VkImageSubresourceRange dRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };
vkCmdClearDepthStencilImage(cmdBuf, depthImg,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &depthClr, 1, &dRange);
```

---

## 12. 绘制命令

### 接口映射

| D3D11 | Vulkan | 说明 |
|-------|--------|------|
| `Draw` | `vkCmdDraw` | 非索引 |
| `DrawIndexed` | `vkCmdDrawIndexed` | 索引 |
| `DrawInstanced` | `vkCmdDraw` (instanceCount>1) | 实例化 |
| `DrawIndexedInstanced` | `vkCmdDrawIndexed` (instanceCount>1) | 索引实例化 |
| `DrawInstancedIndirect` | `vkCmdDrawIndirect` | 间接 |
| `DrawIndexedInstancedIndirect` | `vkCmdDrawIndexedIndirect` | 间接索引 |
| `Dispatch` | `vkCmdDispatch` | 计算 |

### 使用示例

**D3D11**

```cpp
context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
context->DrawIndexed(36, 0, 0);              // 索引绘制
context->Draw(3, 0);                          // 非索引
context->DrawIndexedInstanced(36, 100, 0, 0, 0); // 实例化
```

**Vulkan**

```cpp
// 必须在 RenderPass 内，Pipeline 已绑定
vkCmdSetViewport(cmdBuf, 0, 1, &viewport);
vkCmdSetScissor(cmdBuf, 0, 1, &scissor);
vkCmdBindVertexBuffers(cmdBuf, 0, 1, &vb, offsets);
vkCmdBindIndexBuffer(cmdBuf, ib, 0, VK_INDEX_TYPE_UINT16);
vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
    pipelineLayout, 0, 1, &ds, 0, nullptr);

vkCmdDrawIndexed(cmdBuf, 36, 1, 0, 0, 0);   // 索引绘制
vkCmdDraw(cmdBuf, 3, 1, 0, 0);               // 非索引
vkCmdDrawIndexed(cmdBuf, 36, 100, 0, 0, 0);  // 实例化
```

---

## 13. 资源数据传输

### 接口映射

| D3D11 | Vulkan | 说明 |
|-------|--------|------|
| `Map` / `Unmap` | `vkMapMemory` / `vkUnmapMemory` | CPU 映射 |
| `UpdateSubresource` | staging + `vkCmdCopyBufferToImage` | 更新纹理 |
| `CopyResource` | `vkCmdCopyBuffer` / `vkCmdCopyImage` | GPU 复制 |
| `CopySubresourceRegion` | `vkCmdCopyBufferToImage` / `vkCmdCopyImageToBuffer` | 子资源 |
| — | `vkCmdBlitImage` | 带缩放的图像复制 |

### 使用示例

**D3D11**

```cpp
// Map/Unmap 更新动态缓冲
D3D11_MAPPED_SUBRESOURCE mapped;
context->Map(cbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
memcpy(mapped.pData, &data, sizeof(data));
context->Unmap(cbuffer, 0);

// GPU 复制
context->CopyResource(dst, src);
```

**Vulkan**

```cpp
// 直接映射 (HOST_VISIBLE)
void* ptr;
vkMapMemory(device, uboMemory, 0, sizeof(data), 0, &ptr);
memcpy(ptr, &data, sizeof(data));
vkUnmapMemory(device, uboMemory);

// GPU buffer copy
VkBufferCopy region = { 0, 0, size };
vkCmdCopyBuffer(cmdBuf, src, dst, 1, &region);

// Image blit (带缩放)
VkImageBlit blit{};
blit.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
blit.srcOffsets[1] = { (int)srcW, (int)srcH, 1 };
blit.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
blit.dstOffsets[1] = { (int)dstW, (int)dstH, 1 };
vkCmdBlitImage(cmdBuf, srcImg, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    dstImg, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
```

---

## 14. 同步

### 概念对比

| 维度 | D3D11 | Vulkan |
|------|-------|--------|
| 模型 | 驱动隐式同步 | 应用显式同步 |
| CPU-GPU | Map 时自动等待 | `VkFence` |
| GPU-GPU | 隐式 | `VkSemaphore` + `VkPipelineBarrier` |
| 内存屏障 | 不需要 | `VkImageMemoryBarrier` 等 |

### 接口映射

| D3D11 | Vulkan | 说明 |
|-------|--------|------|
| 隐式 | `VkFence` / `vkWaitForFences` | CPU 等 GPU |
| 隐式 | `VkSemaphore` | GPU 队列间同步 |
| 隐式 | `vkCmdPipelineBarrier` | 命令间同步+布局转换 |
| `Flush` | `vkQueueSubmit` | 强制提交 |
| — | `vkDeviceWaitIdle` | 等待空闲 |

### 使用示例

**D3D11**

```cpp
// 大部分同步隐式完成
D3D11_MAPPED_SUBRESOURCE mapped;
context->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped); // 驱动自动同步
context->Unmap(buf, 0);
```

**Vulkan**

```cpp
// Fence (CPU-GPU)
VkFenceCreateInfo fCI{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, VK_FENCE_CREATE_SIGNALED_BIT };
VkFence fence;
vkCreateFence(device, &fCI, nullptr, &fence);
vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
vkResetFences(device, 1, &fence);

// Semaphore (GPU-GPU)
VkSemaphore imgReady, renderDone;
// acquire → wait imgReady → render → signal renderDone → present waits renderDone
vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imgReady, VK_NULL_HANDLE, &idx);

// Pipeline Barrier (命令间)
VkImageMemoryBarrier barrier{};
barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
barrier.image = rtImage;
barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
vkCmdPipelineBarrier(cmdBuf,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
    0, 0, nullptr, 0, nullptr, 1, &barrier);
```

---

## 15. 枚举映射

### 像素格式

| DXGI_FORMAT | VkFormat |
|-------------|----------|
| `R8G8B8A8_UNORM` | `VK_FORMAT_R8G8B8A8_UNORM` |
| `B8G8R8A8_UNORM` | `VK_FORMAT_B8G8R8A8_UNORM` |
| `R32G32B32A32_FLOAT` | `VK_FORMAT_R32G32B32A32_SFLOAT` |
| `R32G32B32_FLOAT` | `VK_FORMAT_R32G32B32_SFLOAT` |
| `R32G32_FLOAT` | `VK_FORMAT_R32G32_SFLOAT` |
| `R32_FLOAT` | `VK_FORMAT_R32_SFLOAT` |
| `R16G16B16A16_FLOAT` | `VK_FORMAT_R16G16B16A16_SFLOAT` |
| `D24_UNORM_S8_UINT` | `VK_FORMAT_D24_UNORM_S8_UINT` |
| `D32_FLOAT` | `VK_FORMAT_D32_SFLOAT` |
| `D32_FLOAT_S8X24_UINT` | `VK_FORMAT_D32_SFLOAT_S8_UINT` |
| `R16_UINT` | `VK_FORMAT_R16_UINT` |
| `R32_UINT` | `VK_FORMAT_R32_UINT` |
| `BC1_UNORM` | `VK_FORMAT_BC1_RGBA_UNORM_BLOCK` |
| `BC3_UNORM` | `VK_FORMAT_BC3_UNORM_BLOCK` |
| `BC5_UNORM` | `VK_FORMAT_BC5_UNORM_BLOCK` |
| `BC7_UNORM` | `VK_FORMAT_BC7_UNORM_BLOCK` |

### 图元拓扑

| D3D11 | VkPrimitiveTopology |
|-------|---------------------|
| `TRIANGLELIST` | `VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST` |
| `TRIANGLESTRIP` | `VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP` |
| `LINELIST` | `VK_PRIMITIVE_TOPOLOGY_LINE_LIST` |
| `LINESTRIP` | `VK_PRIMITIVE_TOPOLOGY_LINE_STRIP` |
| `POINTLIST` | `VK_PRIMITIVE_TOPOLOGY_POINT_LIST` |

### 填充模式

| D3D11_FILL_MODE | VkPolygonMode |
|-----------------|---------------|
| `D3D11_FILL_SOLID` | `VK_POLYGON_MODE_FILL` |
| `D3D11_FILL_WIREFRAME` | `VK_POLYGON_MODE_LINE` |

### 剔除模式

| D3D11_CULL_MODE | VkCullModeFlags |
|-----------------|-----------------|
| `D3D11_CULL_NONE` | `VK_CULL_MODE_NONE` |
| `D3D11_CULL_FRONT` | `VK_CULL_MODE_FRONT_BIT` |
| `D3D11_CULL_BACK` | `VK_CULL_MODE_BACK_BIT` |

### 比较函数

| D3D11_COMPARISON_FUNC | VkCompareOp |
|-----------------------|-------------|
| `NEVER` | `VK_COMPARE_OP_NEVER` |
| `LESS` | `VK_COMPARE_OP_LESS` |
| `EQUAL` | `VK_COMPARE_OP_EQUAL` |
| `LESS_EQUAL` | `VK_COMPARE_OP_LESS_OR_EQUAL` |
| `GREATER` | `VK_COMPARE_OP_GREATER` |
| `NOT_EQUAL` | `VK_COMPARE_OP_NOT_EQUAL` |
| `GREATER_EQUAL` | `VK_COMPARE_OP_GREATER_OR_EQUAL` |
| `ALWAYS` | `VK_COMPARE_OP_ALWAYS` |

### 混合因子

| D3D11_BLEND | VkBlendFactor |
|-------------|---------------|
| `ZERO` | `VK_BLEND_FACTOR_ZERO` |
| `ONE` | `VK_BLEND_FACTOR_ONE` |
| `SRC_COLOR` | `VK_BLEND_FACTOR_SRC_COLOR` |
| `INV_SRC_COLOR` | `VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR` |
| `SRC_ALPHA` | `VK_BLEND_FACTOR_SRC_ALPHA` |
| `INV_SRC_ALPHA` | `VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA` |
| `DEST_ALPHA` | `VK_BLEND_FACTOR_DST_ALPHA` |
| `INV_DEST_ALPHA` | `VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA` |
| `DEST_COLOR` | `VK_BLEND_FACTOR_DST_COLOR` |
| `INV_DEST_COLOR` | `VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR` |

### 纹理寻址模式

| D3D11_TEXTURE_ADDRESS_MODE | VkSamplerAddressMode |
|----------------------------|----------------------|
| `WRAP` | `VK_SAMPLER_ADDRESS_MODE_REPEAT` |
| `MIRROR` | `VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT` |
| `CLAMP` | `VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE` |
| `BORDER` | `VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER` |

### 过滤模式

| D3D11_FILTER | VkFilter (mag/min) + VkSamplerMipmapMode |
|-------------|------------------------------------------|
| `MIN_MAG_MIP_POINT` | `NEAREST` + `NEAREST` + `NEAREST` |
| `MIN_MAG_MIP_LINEAR` | `LINEAR` + `LINEAR` + `LINEAR` |
| `MIN_MAG_POINT_MIP_LINEAR` | `NEAREST` + `NEAREST` + `LINEAR` |
| `MIN_MAG_LINEAR_MIP_POINT` | `LINEAR` + `LINEAR` + `NEAREST` |
| `ANISOTROPIC` | `LINEAR` + `LINEAR` + `LINEAR` + anisotropyEnable=TRUE |

---

## 附录：核心差异总结

| 领域 | D3D11 特点 | Vulkan 特点 |
|------|-----------|-------------|
| 初始化 | 简单，1 函数创建 Device+Context | 复杂，需逐步创建 Instance→PhysicalDevice→Device→Queue |
| 内存 | 驱动管理 | 应用管理（选择内存类型、手动分配绑定） |
| 命令 | 即时执行 | 录制 → 提交，天然支持多线程 |
| 渲染通道 | 隐式 | 显式 RenderPass（对 tile-based GPU 有优化意义） |
| 管线状态 | 独立对象随时切换 | 整合为不可变 Pipeline（需提前创建，建议缓存） |
| 资源绑定 | Slot-based 直观 | Descriptor Set 灵活但复杂 |
| 同步 | 驱动隐式 | 应用显式（Fence/Semaphore/Barrier） |
| 错误处理 | HRESULT 返回 | VkResult + Validation Layer |
