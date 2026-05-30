# D3D11 vs OpenGL ES 3.x API 映射参考

> 以 Direct3D 11 API 为基准，列出 OpenGL ES 3.0/3.1/3.2 中对应能力的接口，并提供典型使用代码示例。
>
> 命名模式：`D3D11-vs-{API}-API-Mapping.md`
>
> **版本覆盖**：GLES 3.0（最低要求）/ 3.1 / 3.2（目标版本），标注各特性所需最低版本。

---

## 1. 初始化与设备

### 概念对比

| 维度 | D3D11 | OpenGL ES 3.x |
|------|-------|---------------|
| 设计理念 | 对象模型，驱动隐式管理 | 状态机模型，驱动隐式管理 |
| 设备创建 | 单步完成（Device + Context） | EGL 创建 Context（平台无关） |
| 队列管理 | 隐式（Context 内部） | 隐式（单线程 Context 绑定） |
| 多线程 | Deferred Context | 需共享 Context 或显式同步（EGL 支持共享上下文） |
| 上下文管理 | DXGI + Device | EGL（统一的跨平台上下文管理） |

### 接口映射

| D3D11 | OpenGL ES 3.x | 说明 |
|-------|---------------|------|
| `D3D11CreateDevice` | `eglCreateContext` | 创建渲染上下文 |
| `ID3D11Device` | EGL Context | 逻辑设备（状态机） |
| `ID3D11DeviceContext` | EGL Context（当前绑定） | 命令发送 |
| `IDXGIAdapter` | `glGetString(GL_RENDERER)` | 物理显卡信息 |
| `IDXGIFactory` | `eglGetDisplay` + `eglInitialize` | 顶层管理 |
| `D3D_FEATURE_LEVEL` | `eglGetConfigAttrib` / `glGetIntegerv(GL_MAJOR_VERSION)` | 能力查询 |

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

**OpenGL ES 3.x (EGL + Android)**

```cpp
// 1. 获取 EGL Display
EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
eglInitialize(display, nullptr, nullptr);

// 2. 选择配置
EGLConfig config;
EGLint numConfigs;
EGLint configAttribs[] = {
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_DEPTH_SIZE, 24,
    EGL_STENCIL_SIZE, 8,
    EGL_NONE
};
eglChooseConfig(display, configAttribs, &config, 1, &numConfigs);

// 3. 创建上下文（版本降级策略：3.2 -> 3.1 -> 3.0）
EGLContext context = EGL_NO_CONTEXT;
int versions[][2] = { {3, 2}, {3, 1}, {3, 0} };
for (auto& ver : versions) {
    EGLint contextAttribs[] = {
        EGL_CONTEXT_MAJOR_VERSION, ver[0],
        EGL_CONTEXT_MINOR_VERSION, ver[1],
        EGL_NONE
    };
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context != EGL_NO_CONTEXT) break;
}

// 4. 查询实际版本
eglMakeCurrent(display, surface, surface, context);
GLint major, minor;
glGetIntegerv(GL_MAJOR_VERSION, &major);
glGetIntegerv(GL_MINOR_VERSION, &minor);
// major.minor 即为实际 GLES 版本

// 5. 查询能力
const char* version = (const char*)glGetString(GL_VERSION);
const char* renderer = (const char*)glGetString(GL_RENDERER);
const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
```

---

## 2. 交换链与呈现

### 概念对比

| 维度 | D3D11 | OpenGL ES 3.x |
|------|-------|---------------|
| 交换链归属 | DXGI 层 | EGL 层 |
| Surface | 隐含在 HWND | EGLSurface（绑定到 ANativeWindow） |
| 缓冲切换 | `Present` 调用 | `eglSwapBuffers` |
| 双缓冲 | 通过 SwapChain 描述 | EGL Config 中隐含（通常默认双缓冲） |

### 接口映射

| D3D11 / DXGI | OpenGL ES 3.x (EGL) | 说明 |
|-------|----------------------|------|
| `IDXGIFactory::CreateSwapChain` | `eglCreateWindowSurface` | 创建渲染表面 |
| `IDXGISwapChain` | `EGLSurface` | 交换链/渲染表面 |
| `IDXGISwapChain::Present` | `eglSwapBuffers(display, surface)` | 呈现 |
| `IDXGISwapChain::GetBuffer` | 默认帧缓冲（FBO 0） | 后缓冲 |
| `IDXGISwapChain::ResizeBuffers` | `eglDestroySurface` + `eglCreateWindowSurface` 或 `glViewport` | 窗口 resize |
| VSync 控制 | `eglSwapInterval(display, 1)` | 垂直同步 |

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

**OpenGL ES 3.x (EGL + Android)**

```cpp
// 创建窗口表面（ANativeWindow 从 Android Activity 获取）
EGLSurface surface = eglCreateWindowSurface(display, config, nativeWindow, nullptr);

// 绑定上下文和表面
eglMakeCurrent(display, surface, surface, context);

// 设置 VSync
eglSwapInterval(display, 1); // 1 = VSync 开启, 0 = 关闭

// 窗口大小变化时更新视口
glViewport(0, 0, width, height);

// 每帧结束：呈现
eglSwapBuffers(display, surface);

// 窗口销毁时
eglDestroySurface(display, surface);
```

---

## 3. 命令录制与提交

### 概念对比

| 维度 | D3D11 | OpenGL ES 3.x |
|------|-------|---------------|
| 模式 | 即时模式（调用即执行） | 即时模式（调用即执行） |
| 多线程 | Deferred Context（可选） | 需共享 Context，多线程支持有限 |
| 命令缓冲 | 通过 Deferred Context 生成 CommandList | 无原生 Command Buffer |
| 批处理 | 驱动内部优化 | 驱动内部优化 |

### 接口映射

| D3D11 | OpenGL ES 3.x | 说明 |
|-------|---------------|------|
| `ID3D11DeviceContext` (Immediate) | 当前 EGL Context | 即时命令 |
| `ID3D11DeviceContext` (Deferred) | — (无等价) | 多线程录制不可用 |
| `ExecuteCommandList` | — | 无等价 |
| `Flush` | `glFlush()` | 强制提交命令到 GPU |
| — | `glFinish()` | 等待所有命令完成 |

### 使用示例

**D3D11**

```cpp
// 即时模式 — 调用即执行
context->ClearRenderTargetView(rtv, clearColor);
context->DrawIndexed(indexCount, 0, 0);
```

**OpenGL ES 3.x**

```cpp
// 即时模式 — 调用即执行（与 D3D11 类似）
glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

// 强制刷新命令队列（通常不需要）
glFlush();
```

---

## 4. 渲染通道

### 概念对比

| 维度 | D3D11 | OpenGL ES 3.x |
|------|-------|---------------|
| 渲染通道 | 隐式（绑定 RTV/DSV 即开始） | 隐式（绑定 FBO 即开始） |
| Attachment | 绑定 View 对象 | FBO Attachment（颜色/深度/模板） |
| Framebuffer | 隐含在 RTV/DSV | 显式 FBO 对象（0 = 默认帧缓冲） |
| 多渲染目标 | 绑定多个 RTV | `glDrawBuffers` 指定多个颜色附件（GLES 3.0+） |
| Invalidate | — | `glInvalidateFramebuffer`（性能优化，GLES 3.0+） |

### 接口映射

| D3D11 | OpenGL ES 3.x | 说明 |
|-------|---------------|------|
| `OMSetRenderTargets` | `glBindFramebuffer` + `glFramebufferTexture2D` | 设置渲染目标 |
| — (隐式结束) | `glBindFramebuffer(GL_FRAMEBUFFER, 0)` | 切换回默认帧缓冲 |
| `ID3D11RenderTargetView` | FBO Color Attachment | 颜色附件 |
| `ID3D11DepthStencilView` | FBO Depth/Stencil Attachment | 深度模板附件 |
| — | `glGenFramebuffers` / `glDeleteFramebuffers` | FBO 生命周期 |
| — | `glCheckFramebufferStatus` | 完整性检查 |
| — | `glInvalidateFramebuffer` | 通知驱动丢弃附件内容（移动端性能关键） |

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

**OpenGL ES 3.x**

```cpp
// 创建 FBO（一次性）
GLuint fbo;
glGenFramebuffers(1, &fbo);
glBindFramebuffer(GL_FRAMEBUFFER, fbo);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

// 检查完整性
if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    // 错误处理...

// 每帧：绑定 FBO 并渲染
glBindFramebuffer(GL_FRAMEBUFFER, fbo);
GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
glDrawBuffers(1, drawBuffers);

glClearColor(0, 0, 0, 1);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

// 渲染结束后 Invalidate 不再使用的附件（移动端性能优化）
GLenum invalidAttachments[] = { GL_DEPTH_STENCIL_ATTACHMENT };
glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, invalidAttachments);

// 切换回默认帧缓冲
glBindFramebuffer(GL_FRAMEBUFFER, 0);
```

---

## 5. 缓冲区

### 概念对比

| 维度 | D3D11 | OpenGL ES 3.x |
|------|-------|---------------|
| 内存管理 | 驱动自动 | 驱动自动 |
| 创建步骤 | 一步 CreateBuffer | `glGenBuffers` + `glBufferData` |
| 类型区分 | BindFlags 区分用途 | Target 绑定点区分（GL_ARRAY_BUFFER 等） |
| 数据更新 | Map/Unmap 或 UpdateSubresource | `glMapBufferRange` / `glBufferSubData` |
| 不可变存储 | — | `glBufferStorage`（GLES 3.1+ via EXT） |

### 接口映射

| D3D11 | OpenGL ES 3.x | 说明 |
|-------|---------------|------|
| `ID3D11Device::CreateBuffer` | `glGenBuffers` + `glBufferData` | 创建缓冲 |
| `ID3D11Buffer` | `GLuint` (buffer name) | 缓冲对象 |
| `IASetVertexBuffers` | `glBindBuffer(GL_ARRAY_BUFFER, ...)` + VAO | 绑定 VB |
| `IASetIndexBuffer` | `glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ...)` (在 VAO 中) | 绑定 IB |
| `VSSetConstantBuffers` / `PSSetConstantBuffers` | `glBindBufferBase(GL_UNIFORM_BUFFER, ...)` | 绑定 UBO |
| `D3D11_USAGE_DYNAMIC` | `GL_DYNAMIC_DRAW` | 动态用途提示 |
| `D3D11_USAGE_DEFAULT` | `GL_STATIC_DRAW` | 静态用途提示 |
| `D3D11_USAGE_STAGING` | PBO (`GL_PIXEL_PACK_BUFFER`) + `GL_STREAM_READ` | 数据回读 |

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

**OpenGL ES 3.x**

```cpp
// 创建顶点缓冲
GLuint vbo;
glGenBuffers(1, &vbo);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// 在 VAO 中配置顶点属性（GLES 3.0 必须使用 VAO）
GLuint vao;
glGenVertexArrays(1, &vao);
glBindVertexArray(vao);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
glEnableVertexAttribArray(0);
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
glEnableVertexAttribArray(1);

// 创建索引缓冲
GLuint ibo;
glGenBuffers(1, &ibo);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

// 创建 UBO（Uniform Buffer Object）
GLuint ubo;
glGenBuffers(1, &ubo);
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformData), nullptr, GL_DYNAMIC_DRAW);
glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo); // 绑定到 binding point 0
```

---

## 6. 纹理与图像

### 概念对比

| 维度 | D3D11 | OpenGL ES 3.x |
|------|-------|---------------|
| 纹理对象 | `ID3D11Texture2D` + View 分离 | `GLuint` 纹理对象统一 |
| 视图 | SRV/RTV/DSV/UAV 分离 | 纹理本身可直接绑定，FBO 附件作为"视图" |
| 数据上传 | `UpdateSubresource` / Map | `glTexImage2D` / `glTexSubImage2D` |
| 布局转换 | 驱动自动 | 驱动自动 |
| 不可变纹理 | 隐含在 Usage | `glTexStorage2D`（推荐，GLES 3.0+） |
| 压缩格式 | BC1-BC7 (DXT/BPTC) | ETC2/EAC（必须支持），ASTC（可选扩展） |

### 接口映射

| D3D11 | OpenGL ES 3.x | 说明 |
|-------|---------------|------|
| `CreateTexture2D` | `glGenTextures` + `glTexStorage2D` / `glTexImage2D` | 创建纹理 |
| `ID3D11Texture2D` | `GLuint` (texture name) | 纹理对象 |
| `ID3D11ShaderResourceView` | 纹理直接绑定到纹理单元 | 着色器资源 |
| `CreateShaderResourceView` | — (不需要，纹理可直接使用) | — |
| `PSSetShaderResources` | `glActiveTexture` + `glBindTexture` | 绑定纹理到着色器 |
| `UpdateSubresource` | `glTexSubImage2D` | 更新纹理数据 |
| `GenerateMips` | `glGenerateMipmap` | 生成 Mipmap |

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

**OpenGL ES 3.x**

```cpp
// 创建纹理（使用 glTexStorage2D — 推荐，不可变分配）
GLuint texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);
glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 256, 256); // 不可变分配
glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256,
    GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

// 设置纹理参数（或使用独立 Sampler Object）
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// 生成 Mipmap
glGenerateMipmap(GL_TEXTURE_2D);

// 绑定到着色器（纹理单元 0）
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texture);
// ESSL 中: uniform sampler2D diffuseMap; layout(binding = 0)
```

---

## 7. 采样器

### 概念对比

| 维度 | D3D11 | OpenGL ES 3.x |
|------|-------|---------------|
| 采样器对象 | 独立 SamplerState 对象 | 独立 Sampler Object（GLES 3.0+） 或纹理参数 |
| 绑定方式 | 按 slot 绑定 | 按纹理单元绑定 |
| 各向异性 | 原生支持 | 扩展 `GL_EXT_texture_filter_anisotropic`（广泛可用） |

### 接口映射

| D3D11 | OpenGL ES 3.x | 说明 |
|-------|---------------|------|
| `CreateSamplerState` | `glGenSamplers` + `glSamplerParameter*` | 创建采样器 |
| `ID3D11SamplerState` | `GLuint` (sampler name) | 采样器对象 |
| `PSSetSamplers` | `glBindSampler(unit, sampler)` | 绑定采样器到纹理单元 |
| `D3D11_FILTER` | `GL_TEXTURE_MIN_FILTER` / `GL_TEXTURE_MAG_FILTER` | 过滤模式 |
| `D3D11_TEXTURE_ADDRESS_MODE` | `GL_TEXTURE_WRAP_S/T/R` | 寻址模式 |

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

**OpenGL ES 3.x**

```cpp
// 使用独立 Sampler Object（推荐）
GLuint sampler;
glGenSamplers(1, &sampler);
glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
glSamplerParameterf(sampler, GL_TEXTURE_MIN_LOD, 0.0f);
glSamplerParameterf(sampler, GL_TEXTURE_MAX_LOD, 1000.0f);

// 各向异性过滤（需要扩展 GL_EXT_texture_filter_anisotropic）
glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

// 绑定到纹理单元 0
glBindSampler(0, sampler);
```

---

## 8. 着色器

### 概念对比

| 维度 | D3D11 | OpenGL ES 3.x |
|------|-------|---------------|
| 语言 | HLSL | ESSL (OpenGL ES Shading Language) |
| 版本 | SM 5.0 | `#version 300 es` (3.0) / `310 es` (3.1) / `320 es` (3.2) |
| 编译 | 运行时 D3DCompile 或离线 fxc | 运行时 `glCompileShader` |
| 对象 | 按阶段分类（ID3D11VertexShader 等） | 统一 Shader Object + Program 链接 |
| 反射 | D3DReflect | `glGetActiveUniform` / `glGetProgramResource`（GLES 3.1+） |
| 链接 | 隐式 | 显式 Program 链接 |
| 几何着色器 | 支持 | GLES 3.2 或 `GL_EXT_geometry_shader` |
| 曲面细分 | 支持 | GLES 3.2 或 `GL_EXT_tessellation_shader` |
| 计算着色器 | 支持 | GLES 3.1+ |

### 接口映射

| D3D11 | OpenGL ES 3.x | 说明 |
|-------|---------------|------|
| `D3DCompile` | `glShaderSource` + `glCompileShader` | 编译着色器 |
| `CreateVertexShader` | `glCreateShader(GL_VERTEX_SHADER)` | 创建 VS |
| `CreatePixelShader` | `glCreateShader(GL_FRAGMENT_SHADER)` | 创建 FS |
| `CreateGeometryShader` | `glCreateShader(GL_GEOMETRY_SHADER)` | 创建 GS（GLES 3.2） |
| `CreateComputeShader` | `glCreateShader(GL_COMPUTE_SHADER)` | 创建 CS（GLES 3.1+） |
| `VSSetShader` / `PSSetShader` | `glUseProgram(program)` | 绑定着色器程序 |
| — | `glCreateProgram` + `glAttachShader` + `glLinkProgram` | 链接为 Program |
| `D3DReflect` | `glGetActiveUniform` / `glGetProgramResourceiv` | 反射 |
| `ID3D11InputLayout` | `glVertexAttribPointer` + VAO | 顶点输入布局 |

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

**OpenGL ES 3.x**

```cpp
// 顶点着色器 (ESSL 300 es)
const char* vsSrc = R"(#version 300 es
precision highp float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(std140) uniform MVP {
    mat4 uMVP;
};
out vec2 vUV;
void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    vUV = aUV;
})";

GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertShader, 1, &vsSrc, nullptr);
glCompileShader(vertShader);
// 检查编译错误
GLint success;
glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
if (!success) {
    char log[512];
    glGetShaderInfoLog(vertShader, sizeof(log), nullptr, log);
    // 错误处理...
}

// 片段着色器
const char* fsSrc = R"(#version 300 es
precision mediump float;
in vec2 vUV;
uniform sampler2D diffuseMap;
out vec4 FragColor;
void main() {
    FragColor = texture(diffuseMap, vUV);
})";

GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fragShader, 1, &fsSrc, nullptr);
glCompileShader(fragShader);

// 链接 Program
GLuint program = glCreateProgram();
glAttachShader(program, vertShader);
glAttachShader(program, fragShader);
glLinkProgram(program);

// 使用
glUseProgram(program);

// 清理 Shader（链接后可删除）
glDeleteShader(vertShader);
glDeleteShader(fragShader);
```

---

## 9. 管线状态

### 概念对比

| 维度 | D3D11 | OpenGL ES 3.x |
|------|-------|---------------|
| 管理方式 | 独立 State Object 分别创建/绑定 | 全局状态机，逐项设置 |
| 混合 | `ID3D11BlendState`（独立对象） | `glEnable(GL_BLEND)` + `glBlendFunc` |
| 光栅化 | `ID3D11RasterizerState`（独立对象） | `glCullFace` / `glFrontFace`（无 glPolygonMode） |
| 深度模板 | `ID3D11DepthStencilState`（独立对象） | `glEnable(GL_DEPTH_TEST)` + `glDepthFunc` |
| 输入布局 | `ID3D11InputLayout` 对象 | VAO 中配置 |
| 填充模式 | Solid / Wireframe | **仅 Solid**（GLES 不支持 Wireframe） |

### 接口映射

| D3D11 | OpenGL ES 3.x | 说明 |
|-------|---------------|------|
| `CreateBlendState` | `glBlendFunc` / `glBlendEquation` | 混合状态 |
| `CreateRasterizerState` | `glCullFace` / `glFrontFace` | 光栅化状态（无 PolygonMode） |
| `CreateDepthStencilState` | `glDepthFunc` / `glDepthMask` / `glStencilOp` | 深度模板状态 |
| `CreateInputLayout` | VAO + `glVertexAttribPointer` | 顶点输入布局 |
| `OMSetBlendState` | `glEnable(GL_BLEND)` + `glBlendFunc` | 设置混合 |
| `RSSetState` | `glEnable(GL_CULL_FACE)` + `glCullFace` | 设置光栅化 |
| `OMSetDepthStencilState` | `glEnable(GL_DEPTH_TEST)` + `glDepthFunc` | 设置深度 |
| `IASetPrimitiveTopology` | `glDrawElements` / `glDrawArrays` 的 mode 参数 | 图元拓扑 |
| `RSSetViewports` | `glViewport` | 视口 |
| `RSSetScissorRects` | `glEnable(GL_SCISSOR_TEST)` + `glScissor` | 裁剪矩形 |

### 使用示例

**D3D11**

```cpp
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

context->RSSetState(rsState);
context->OMSetDepthStencilState(dsState, 0);
context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
```

**OpenGL ES 3.x**

```cpp
// 光栅化状态（注意：GLES 无 glPolygonMode，仅支持 Fill 模式）
glEnable(GL_CULL_FACE);
glCullFace(GL_BACK);
glFrontFace(GL_CCW);
// 无 GL_DEPTH_CLAMP（GLES 不支持），用 gl_FragDepth 或其他方式替代

// 深度状态
glEnable(GL_DEPTH_TEST);
glDepthMask(GL_TRUE);       // DepthWriteMask ALL
glDepthFunc(GL_LESS);

// 混合状态（如需开启）
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glBlendEquation(GL_FUNC_ADD);

// 视口
glViewport(0, 0, 1280, 720);

// 裁剪
glEnable(GL_SCISSOR_TEST);
glScissor(0, 0, 1280, 720);

// 图元拓扑在绘制时指定
glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
```

---

## 10. 资源绑定

### 概念对比

| 维度 | D3D11 | OpenGL ES 3.x |
|------|-------|---------------|
| 模型 | Slot-based（按阶段分 slot） | Binding Point（纹理单元 / UBO 绑定点） |
| 布局 | 隐式（shader 反射） | 显式 `layout(binding=N)` 或运行时查询 |
| 更新 | 直接 `XSSet*` 调用 | `glBindBufferBase` / `glActiveTexture` + `glBindTexture` |
| Uniform | Constant Buffer 整块 | UBO 整块（推荐）或单个 Uniform 变量 |
| SSBO | — | `glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ...)`（GLES 3.1+） |

### 接口映射

| D3D11 | OpenGL ES 3.x | 说明 |
|-------|---------------|------|
| `VSSetConstantBuffers` / `PSSetConstantBuffers` | `glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo)` | UBO 绑定 |
| `PSSetShaderResources` | `glActiveTexture(GL_TEXTURE0+n)` + `glBindTexture` | 纹理绑定 |
| `PSSetSamplers` | `glBindSampler(unit, sampler)` | 采样器绑定 |
| Register (b0, t0, s0) | Binding point / Texture unit | 槽位 |
| — | `glUniformBlockBinding` | 关联 UBO block 与绑定点 |
| — | `glUniform*` | 单个 Uniform（简单场景） |
| UAV (Unordered Access) | `glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ...)` | SSBO（GLES 3.1+） |
| UAV (Image) | `glBindImageTexture` | Image Load/Store（GLES 3.1+） |

### 使用示例

**D3D11**

```cpp
context->VSSetConstantBuffers(0, 1, &mvpBuffer);
context->PSSetConstantBuffers(0, 1, &materialBuffer);
context->PSSetShaderResources(0, 1, &diffuseSRV);
context->PSSetSamplers(0, 1, &linearSampler);
```

**OpenGL ES 3.x**

```cpp
// UBO 绑定（对应 HLSL 的 cbuffer）
// ESSL 中: layout(std140, binding = 0) uniform MVP { mat4 mvp; };
glBindBufferBase(GL_UNIFORM_BUFFER, 0, mvpUBO);

// 也可以运行时关联:
GLuint blockIndex = glGetUniformBlockIndex(program, "MVP");
glUniformBlockBinding(program, blockIndex, 0); // binding point 0

// 材质 UBO
glBindBufferBase(GL_UNIFORM_BUFFER, 1, materialUBO);

// 纹理绑定
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, diffuseTexture);
// ESSL 中使用 layout(binding = 0) 或:
glUniform1i(glGetUniformLocation(program, "diffuseMap"), 0);

// 采样器绑定
glBindSampler(0, linearSampler);

// SSBO 绑定（GLES 3.1+）
// ESSL 310 es: layout(std430, binding = 0) buffer MySSBO { ... };
glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

// Image Load/Store（GLES 3.1+）
glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
```

---

## 11. 渲染目标

### 接口映射

| D3D11 | OpenGL ES 3.x | 说明 |
|-------|---------------|------|
| `CreateRenderTargetView` | `glFramebufferTexture2D(GL_COLOR_ATTACHMENT0, ...)` | RTV |
| `CreateDepthStencilView` | `glFramebufferTexture2D(GL_DEPTH_STENCIL_ATTACHMENT, ...)` | DSV |
| `OMSetRenderTargets` | `glBindFramebuffer(GL_FRAMEBUFFER, fbo)` | 设置 RT |
| `ClearRenderTargetView` | `glClearBufferfv(GL_COLOR, 0, color)` | 清除颜色 |
| `ClearDepthStencilView` | `glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0)` | 清除深度/模板 |
| MRT (多渲染目标) | `glDrawBuffers` | 多颜色附件（GLES 3.0+，最少 4 个） |

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

**OpenGL ES 3.x**

```cpp
// 创建颜色纹理
GLuint colorTex;
glGenTextures(1, &colorTex);
glBindTexture(GL_TEXTURE_2D, colorTex);
glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 720);

// 创建深度/模板纹理
GLuint depthTex;
glGenTextures(1, &depthTex);
glBindTexture(GL_TEXTURE_2D, depthTex);
glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, 1280, 720);

// 创建 FBO 并附加
GLuint fbo;
glGenFramebuffers(1, &fbo);
glBindFramebuffer(GL_FRAMEBUFFER, fbo);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

// 设置绘制缓冲
GLenum drawBufs[] = { GL_COLOR_ATTACHMENT0 };
glDrawBuffers(1, drawBufs);

// 清除
float clearColor[] = { 0, 0, 0, 1 };
glClearBufferfv(GL_COLOR, 0, clearColor);
glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

// 渲染结束后 Invalidate（移动端 TBR 架构性能关键）
GLenum discards[] = { GL_DEPTH_STENCIL_ATTACHMENT };
glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, discards);

// 渲染到默认帧缓冲（后缓冲）
glBindFramebuffer(GL_FRAMEBUFFER, 0);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

---

## 12. 绘制命令

### 接口映射

| D3D11 | OpenGL ES 3.x | 最低版本 |
|-------|---------------|----------|
| `Draw` | `glDrawArrays` | 3.0 |
| `DrawIndexed` | `glDrawElements` | 3.0 |
| `DrawInstanced` | `glDrawArraysInstanced` | 3.0 |
| `DrawIndexedInstanced` | `glDrawElementsInstanced` | 3.0 |
| `DrawInstancedIndirect` | `glDrawArraysIndirect` | 3.1 |
| `DrawIndexedInstancedIndirect` | `glDrawElementsIndirect` | 3.1 |
| `Dispatch` | `glDispatchCompute` | 3.1 |
| `DrawAuto` (StreamOutput) | `glDrawTransformFeedback`（**不支持**） | — |

### 使用示例

**D3D11**

```cpp
context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
context->DrawIndexed(36, 0, 0);              // 索引绘制
context->Draw(3, 0);                          // 非索引
context->DrawIndexedInstanced(36, 100, 0, 0, 0); // 实例化
```

**OpenGL ES 3.x**

```cpp
glBindVertexArray(vao); // 必须绑定 VAO

// 索引绘制
glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

// 非索引绘制
glDrawArrays(GL_TRIANGLES, 0, 3);

// 实例化绘制（GLES 3.0+）
glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr, 100);

// 间接绘制 (GLES 3.1+)
glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr);

// 计算着色器 (GLES 3.1+)
glUseProgram(computeProgram);
glDispatchCompute(groupsX, groupsY, groupsZ);
glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
```

---

## 13. 资源数据传输

### 接口映射

| D3D11 | OpenGL ES 3.x | 说明 |
|-------|---------------|------|
| `Map` / `Unmap` | `glMapBufferRange` / `glUnmapBuffer` | CPU 映射（GLES 3.0+） |
| `UpdateSubresource` | `glBufferSubData` / `glTexSubImage2D` | 更新数据 |
| `CopyResource` | `glCopyBufferSubData` / `glBlitFramebuffer` | GPU 复制 |
| `CopySubresourceRegion` | `glCopyTexSubImage2D` / `glBlitFramebuffer` | 子资源复制 |
| — | `glBlitFramebuffer` | 帧缓冲间 blit（带缩放，GLES 3.0+） |
| Staging Buffer | PBO + `GL_MAP_READ_BIT` | 回读到 CPU |

> **注意**：GLES 3.x 没有 `glCopyImageSubData`（需要 `GL_EXT_copy_image` 扩展）。

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

**OpenGL ES 3.x**

```cpp
// glMapBufferRange 更新动态缓冲（GLES 3.0+ 推荐方式）
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
void* ptr = glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(data),
    GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
memcpy(ptr, &data, sizeof(data));
glUnmapBuffer(GL_UNIFORM_BUFFER);

// 或使用 glBufferSubData（更简单）
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data);

// GPU buffer 复制 (GLES 3.0+)
glBindBuffer(GL_COPY_READ_BUFFER, srcBuffer);
glBindBuffer(GL_COPY_WRITE_BUFFER, dstBuffer);
glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);

// 帧缓冲 Blit（GLES 3.0+）
glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFBO);
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFBO);
glBlitFramebuffer(0, 0, srcW, srcH, 0, 0, dstW, dstH,
    GL_COLOR_BUFFER_BIT, GL_LINEAR);

// 纹理数据回读（通过 PBO）
GLuint pbo;
glGenBuffers(1, &pbo);
glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
glBufferData(GL_PIXEL_PACK_BUFFER, width * height * 4, nullptr, GL_STREAM_READ);
glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
// 稍后映射读取
void* pixels = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, width * height * 4, GL_MAP_READ_BIT);
// 使用 pixels...
glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
```

---

## 14. 同步

### 概念对比

| 维度 | D3D11 | OpenGL ES 3.x |
|------|-------|---------------|
| 模型 | 驱动隐式同步 | 驱动隐式同步（大部分情况） |
| CPU-GPU | Map 时自动等待 | `glMapBufferRange` 自动等待 / `glFenceSync` 手动 |
| GPU-GPU | 隐式 | 隐式 / `glMemoryBarrier`（GLES 3.1+ 计算着色器后） |
| 显式同步 | 无（D3D11 无 Fence） | `glFenceSync` + `glClientWaitSync`（GLES 3.0+） |

### 接口映射

| D3D11 | OpenGL ES 3.x | 说明 |
|-------|---------------|------|
| 隐式 | `glFenceSync` + `glClientWaitSync` | CPU 等 GPU |
| 隐式 | `glMemoryBarrier`（GLES 3.1+） | 内存屏障（计算着色器后） |
| `Flush` | `glFlush` | 强制提交 |
| — | `glFinish` | 等待所有完成 |
| — | `glDeleteSync` | 删除同步对象 |

### 使用示例

**D3D11**

```cpp
// 大部分同步隐式完成
D3D11_MAPPED_SUBRESOURCE mapped;
context->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped); // 驱动自动同步
context->Unmap(buf, 0);
```

**OpenGL ES 3.x**

```cpp
// 大部分情况也是隐式同步（与 D3D11 类似）
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
void* ptr = glMapBufferRange(GL_UNIFORM_BUFFER, 0, size,
    GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT); // 驱动自动同步
memcpy(ptr, &data, size);
glUnmapBuffer(GL_UNIFORM_BUFFER);

// 显式 Fence 同步（用于异步操作或多帧缓冲）
GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
// ... 后续帧 ...
GLenum result = glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000); // 1秒超时
if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
    // GPU 已完成
}
glDeleteSync(fence);

// 计算着色器后的内存屏障（GLES 3.1+）
glDispatchCompute(groupsX, groupsY, groupsZ);
glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
// 现在可以安全读取计算着色器写入的数据

// 强制完成所有命令
glFinish();
```

---

## 15. 枚举映射

### 像素格式

| DXGI_FORMAT | GLES Internal Format | GL Format / Type | 说明 |
|-------------|---------------------|------------------|------|
| `R8G8B8A8_UNORM` | `GL_RGBA8` | `GL_RGBA` / `GL_UNSIGNED_BYTE` | |
| `R8G8B8A8_UNORM_SRGB` | `GL_SRGB8_ALPHA8` | `GL_RGBA` / `GL_UNSIGNED_BYTE` | |
| `R32G32B32A32_FLOAT` | `GL_RGBA32F` | `GL_RGBA` / `GL_FLOAT` | |
| `R32G32_FLOAT` | `GL_RG32F` | `GL_RG` / `GL_FLOAT` | |
| `R32_FLOAT` | `GL_R32F` | `GL_RED` / `GL_FLOAT` | |
| `R16G16B16A16_FLOAT` | `GL_RGBA16F` | `GL_RGBA` / `GL_HALF_FLOAT` | |
| `R16G16_FLOAT` | `GL_RG16F` | `GL_RG` / `GL_HALF_FLOAT` | |
| `R16_FLOAT` | `GL_R16F` | `GL_RED` / `GL_HALF_FLOAT` | |
| `R8_UNORM` | `GL_R8` | `GL_RED` / `GL_UNSIGNED_BYTE` | |
| `R8G8_UNORM` | `GL_RG8` | `GL_RG` / `GL_UNSIGNED_BYTE` | |
| `D24_UNORM_S8_UINT` | `GL_DEPTH24_STENCIL8` | `GL_DEPTH_STENCIL` / `GL_UNSIGNED_INT_24_8` | |
| `D32_FLOAT` | `GL_DEPTH_COMPONENT32F` | `GL_DEPTH_COMPONENT` / `GL_FLOAT` | |
| `D16_UNORM` | `GL_DEPTH_COMPONENT16` | `GL_DEPTH_COMPONENT` / `GL_UNSIGNED_SHORT` | |
| `R16_UINT` | `GL_R16UI` | `GL_RED_INTEGER` / `GL_UNSIGNED_SHORT` | |
| `R32_UINT` | `GL_R32UI` | `GL_RED_INTEGER` / `GL_UNSIGNED_INT` | |
| `R11G11B10_FLOAT` | `GL_R11F_G11F_B10F` | `GL_RGB` / `GL_UNSIGNED_INT_10F_11F_11F_REV` | |
| `BC1_UNORM` | — | — | GLES 不支持 BCn，使用 ETC2/ASTC |
| `BC3_UNORM` | — | — | 同上 |

### 压缩纹理格式（GLES 专用）

| 格式 | GLES Internal Format | 说明 |
|------|---------------------|------|
| ETC2 RGB | `GL_COMPRESSED_RGB8_ETC2` | GLES 3.0 必须支持 |
| ETC2 RGBA | `GL_COMPRESSED_RGBA8_ETC2_EAC` | GLES 3.0 必须支持 |
| ETC2 RGB + 1bit Alpha | `GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2` | GLES 3.0 必须支持 |
| EAC R11 | `GL_COMPRESSED_R11_EAC` | GLES 3.0 必须支持 |
| EAC RG11 | `GL_COMPRESSED_RG11_EAC` | GLES 3.0 必须支持 |
| ASTC 4x4 | `GL_COMPRESSED_RGBA_ASTC_4x4_KHR` | 需 `GL_KHR_texture_compression_astc_ldr` |
| ASTC 6x6 | `GL_COMPRESSED_RGBA_ASTC_6x6_KHR` | 同上 |
| ASTC 8x8 | `GL_COMPRESSED_RGBA_ASTC_8x8_KHR` | 同上 |

### 图元拓扑

| D3D11 | OpenGL ES 3.x | 最低版本 |
|-------|---------------|----------|
| `TRIANGLELIST` | `GL_TRIANGLES` | 3.0 |
| `TRIANGLESTRIP` | `GL_TRIANGLE_STRIP` | 3.0 |
| `LINELIST` | `GL_LINES` | 3.0 |
| `LINESTRIP` | `GL_LINE_STRIP` | 3.0 |
| `POINTLIST` | `GL_POINTS` | 3.0 |
| `TRIANGLELIST_ADJ` | `GL_TRIANGLES_ADJACENCY` | 3.2 |
| `LINELIST_ADJ` | `GL_LINES_ADJACENCY` | 3.2 |
| `PATCHLIST` | `GL_PATCHES` | 3.2 |

### 剔除模式

| D3D11_CULL_MODE | OpenGL ES 3.x |
|-----------------|---------------|
| `D3D11_CULL_NONE` | `glDisable(GL_CULL_FACE)` |
| `D3D11_CULL_FRONT` | `glEnable(GL_CULL_FACE)` + `glCullFace(GL_FRONT)` |
| `D3D11_CULL_BACK` | `glEnable(GL_CULL_FACE)` + `glCullFace(GL_BACK)` |

### 填充模式

| D3D11_FILL_MODE | OpenGL ES 3.x |
|-----------------|---------------|
| `D3D11_FILL_SOLID` | 默认（唯一支持的模式） |
| `D3D11_FILL_WIREFRAME` | **不支持**（可用 GL_LINES 模拟或 `GL_NV_fill_rectangle` 扩展） |

### 比较函数

| D3D11_COMPARISON_FUNC | OpenGL ES 3.x |
|-----------------------|---------------|
| `NEVER` | `GL_NEVER` |
| `LESS` | `GL_LESS` |
| `EQUAL` | `GL_EQUAL` |
| `LESS_EQUAL` | `GL_LEQUAL` |
| `GREATER` | `GL_GREATER` |
| `NOT_EQUAL` | `GL_NOTEQUAL` |
| `GREATER_EQUAL` | `GL_GEQUAL` |
| `ALWAYS` | `GL_ALWAYS` |

### 混合因子

| D3D11_BLEND | OpenGL ES 3.x |
|-------------|---------------|
| `ZERO` | `GL_ZERO` |
| `ONE` | `GL_ONE` |
| `SRC_COLOR` | `GL_SRC_COLOR` |
| `INV_SRC_COLOR` | `GL_ONE_MINUS_SRC_COLOR` |
| `SRC_ALPHA` | `GL_SRC_ALPHA` |
| `INV_SRC_ALPHA` | `GL_ONE_MINUS_SRC_ALPHA` |
| `DEST_ALPHA` | `GL_DST_ALPHA` |
| `INV_DEST_ALPHA` | `GL_ONE_MINUS_DST_ALPHA` |
| `DEST_COLOR` | `GL_DST_COLOR` |
| `INV_DEST_COLOR` | `GL_ONE_MINUS_DST_COLOR` |
| `SRC_ALPHA_SAT` | `GL_SRC_ALPHA_SATURATE` |

### 混合操作

| D3D11_BLEND_OP | OpenGL ES 3.x |
|----------------|---------------|
| `ADD` | `GL_FUNC_ADD` |
| `SUBTRACT` | `GL_FUNC_SUBTRACT` |
| `REV_SUBTRACT` | `GL_FUNC_REVERSE_SUBTRACT` |
| `MIN` | `GL_MIN` |
| `MAX` | `GL_MAX` |

### 纹理寻址模式

| D3D11_TEXTURE_ADDRESS_MODE | OpenGL ES 3.x |
|----------------------------|---------------|
| `WRAP` | `GL_REPEAT` |
| `MIRROR` | `GL_MIRRORED_REPEAT` |
| `CLAMP` | `GL_CLAMP_TO_EDGE` |
| `BORDER` | `GL_CLAMP_TO_BORDER`（GLES 3.2 或 `GL_EXT_texture_border_clamp`） |
| `MIRROR_ONCE` | **不支持** |

### 过滤模式

| D3D11_FILTER | GL_TEXTURE_MIN_FILTER / GL_TEXTURE_MAG_FILTER |
|-------------|-----------------------------------------------|
| `MIN_MAG_MIP_POINT` | min: `GL_NEAREST_MIPMAP_NEAREST`, mag: `GL_NEAREST` |
| `MIN_MAG_MIP_LINEAR` | min: `GL_LINEAR_MIPMAP_LINEAR`, mag: `GL_LINEAR` |
| `MIN_MAG_POINT_MIP_LINEAR` | min: `GL_NEAREST_MIPMAP_LINEAR`, mag: `GL_NEAREST` |
| `MIN_MAG_LINEAR_MIP_POINT` | min: `GL_LINEAR_MIPMAP_NEAREST`, mag: `GL_LINEAR` |
| `ANISOTROPIC` | min: `GL_LINEAR_MIPMAP_LINEAR`, mag: `GL_LINEAR` + `GL_TEXTURE_MAX_ANISOTROPY_EXT` |

---

## 16. 计算着色器（GLES 3.1+）

### 概念对比

| 维度 | D3D11 | OpenGL ES 3.1+ |
|------|-------|----------------|
| 可用性 | 原生支持 | GLES 3.1+ |
| 共享内存 | `groupshared` | `shared` 关键字 |
| UAV | RWBuffer / RWTexture | SSBO / Image Load/Store |
| 同步 | — | `glMemoryBarrier` |

### 接口映射

| D3D11 | OpenGL ES 3.1+ | 说明 |
|-------|----------------|------|
| `CreateComputeShader` | `glCreateShader(GL_COMPUTE_SHADER)` | 创建 CS |
| `CSSetShader` | `glUseProgram(computeProgram)` | 绑定 CS |
| `CSSetUnorderedAccessViews` | `glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ...)` | 绑定 SSBO |
| `CSSetShaderResources` | `glBindImageTexture` | 绑定 Image |
| `Dispatch` | `glDispatchCompute` | 执行 |
| `DispatchIndirect` | `glDispatchComputeIndirect` | 间接执行 |

### 使用示例

**D3D11**

```cpp
ID3D11ComputeShader* cs;
device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &cs);
context->CSSetShader(cs, nullptr, 0);
context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
context->Dispatch(groupsX, groupsY, groupsZ);
```

**OpenGL ES 3.1+**

```cpp
const char* csSrc = R"(#version 310 es
layout(local_size_x = 16, local_size_y = 16) in;

layout(std430, binding = 0) buffer InputBuffer {
    float data[];
} inputBuf;

layout(std430, binding = 1) buffer OutputBuffer {
    float data[];
} outputBuf;

void main() {
    uint idx = gl_GlobalInvocationID.x;
    outputBuf.data[idx] = inputBuf.data[idx] * 2.0;
})";

GLuint cs = glCreateShader(GL_COMPUTE_SHADER);
glShaderSource(cs, 1, &csSrc, nullptr);
glCompileShader(cs);

GLuint computeProgram = glCreateProgram();
glAttachShader(computeProgram, cs);
glLinkProgram(computeProgram);
glDeleteShader(cs);

// 执行
glUseProgram(computeProgram);
glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputSSBO);
glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputSSBO);
glDispatchCompute(groupsX, groupsY, groupsZ);
glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
```

---

## 附录 A：核心差异总结

| 领域 | D3D11 特点 | OpenGL ES 3.x 特点 |
|------|-----------|-------------------|
| 设计模式 | 对象模型，状态封装在对象中 | 状态机模型，全局状态逐项设置 |
| 初始化 | 简单，1 函数创建 Device+Context | EGL 统一管理（跨平台） |
| 内存 | 驱动管理 | 驱动管理（两者类似） |
| 命令 | 即时执行 | 即时执行（两者类似） |
| 渲染通道 | 隐式（绑定 RTV/DSV） | 隐式（绑定 FBO）+ `glInvalidateFramebuffer` 优化 |
| 管线状态 | 独立 State Object 分组管理 | 全局状态逐项设置（无法原子切换） |
| 资源绑定 | Slot-based，按阶段分 | Binding Point + Texture Unit |
| 着色器 | HLSL，编译为字节码 | ESSL，运行时编译 |
| 顶点输入 | Input Layout 对象 | VAO + VBO 配置（VAO 强制使用） |
| 同步 | 驱动隐式 | 驱动隐式（+ Fence/Barrier） |
| 跨平台 | 仅 Windows | Android / Linux / Web（通过 WebGL 2.0） |
| 错误处理 | HRESULT 返回值 | `glGetError` / `KHR_debug` Callback |
| 填充模式 | 支持 Wireframe | 仅 Fill（不支持 Wireframe） |
| 压缩格式 | BC1-BC7 | ETC2/EAC（必须）+ ASTC（扩展） |
| 计算着色器 | 原生支持 | GLES 3.1+ |
| 几何着色器 | 原生支持 | GLES 3.2 |
| 曲面细分 | 原生支持 | GLES 3.2 |

---

## 附录 B：GLES 3.0 / 3.1 / 3.2 版本能力对比

| 特性 | GLES 3.0 | GLES 3.1 | GLES 3.2 |
|------|----------|----------|----------|
| ESSL 版本 | 300 es | 310 es | 320 es |
| VAO | ✓ | ✓ | ✓ |
| UBO | ✓ | ✓ | ✓ |
| MRT (4+) | ✓ | ✓ | ✓ |
| Instancing | ✓ | ✓ | ✓ |
| Transform Feedback | ✓ | ✓ | ✓ |
| 纹理 3D | ✓ | ✓ | ✓ |
| Sampler Object | ✓ | ✓ | ✓ |
| Sync / Fence | ✓ | ✓ | ✓ |
| glBlitFramebuffer | ✓ | ✓ | ✓ |
| glInvalidateFramebuffer | ✓ | ✓ | ✓ |
| ETC2 / EAC 压缩 | ✓ | ✓ | ✓ |
| glMapBufferRange | ✓ | ✓ | ✓ |
| 计算着色器 | ✗ | ✓ | ✓ |
| SSBO | ✗ | ✓ | ✓ |
| Image Load/Store | ✗ | ✓ | ✓ |
| Indirect Draw | ✗ | ✓ | ✓ |
| Atomic Counter | ✗ | ✓ | ✓ |
| glDispatchCompute | ✗ | ✓ | ✓ |
| 几何着色器 | ✗ | ✗ | ✓ |
| 曲面细分 | ✗ | ✗ | ✓ |
| ASTC (mandatory) | ✗ | ✗ | ✓ |
| Texture Border Clamp | ✗ | ✗ | ✓ |
| Advanced Blend | ✗ | ✗ | ✓ |
| Multisample Texture (2D) | ✗ | ✓ | ✓ |
| Stencil Texturing | ✗ | ✓ | ✓ |
| Texture Gather | ✗ | ✓ | ✓ |

---

## 附录 C：移动端性能注意事项

### Tile-Based Rendering (TBR) 架构

移动端 GPU（Mali, Adreno, PowerVR）使用 Tile-Based Rendering 架构，与桌面端 Immediate Mode Rendering (IMR) 有本质区别：

| 操作 | 影响 | 建议 |
|------|------|------|
| `glInvalidateFramebuffer` | 避免 tile 回写到内存 | **每帧结束后对不再需要的附件调用** |
| 全屏 `glClear` | 通知驱动重置 tile | **每帧开头必须 Clear 所有使用的附件** |
| 避免 FBO 切换 | 每次切换可能触发 tile flush | 尽量减少 FBO 切换次数 |
| MRT 带宽 | 每个 RT 占用 tile 内存 | 控制 MRT 数量和格式大小 |
| `glReadPixels` | 强制 tile resolve | 避免帧中间读取 |
| Alpha Test / `discard` | 阻止 Early-Z | 尽量减少使用 |
| `glFlush` / `glFinish` | 强制管线排空 | 避免帧中间使用 |

### 推荐渲染模式

```cpp
// === 每帧推荐流程 ===

// 1. 绑定目标 FBO
glBindFramebuffer(GL_FRAMEBUFFER, fbo);

// 2. 全屏 Clear（触发 tile reset，不会读取旧内容）
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

// 3. 执行所有绘制
// ...

// 4. Invalidate 不再需要的附件（避免写回内存）
GLenum discards[] = { GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT };
glInvalidateFramebuffer(GL_FRAMEBUFFER, 2, discards);

// 5. 切换到默认 FBO 或下一个 pass
glBindFramebuffer(GL_FRAMEBUFFER, 0);

// 6. 呈现
eglSwapBuffers(display, surface);
```
