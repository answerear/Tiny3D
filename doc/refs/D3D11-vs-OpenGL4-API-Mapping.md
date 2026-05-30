# D3D11 vs OpenGL4 API 映射参考

> 以 Direct3D 11 API 为基准，列出 OpenGL 4.x Core Profile 中对应能力的接口，并提供典型使用代码示例。
>
> 命名模式：`D3D11-vs-{API}-API-Mapping.md`

---

## 1. 初始化与设备

### 概念对比

| 维度 | D3D11 | OpenGL4 |
|------|-------|---------|
| 设计理念 | 对象模型，驱动隐式管理 | 状态机模型，驱动隐式管理 |
| 设备创建 | 单步完成（Device + Context） | 创建 OpenGL Context（平台相关：WGL/GLX/EGL） |
| 队列管理 | 隐式（Context 内部） | 隐式（单线程 Context 绑定） |
| 多线程 | Deferred Context | 需共享 Context 或显式同步 |

### 接口映射

| D3D11 | OpenGL4 | 说明 |
|-------|---------|------|
| `D3D11CreateDevice` | `wglCreateContextAttribsARB` / `glXCreateContextAttribsARB` | 创建渲染上下文 |
| `ID3D11Device` | OpenGL Context | 逻辑设备（状态机） |
| `ID3D11DeviceContext` | OpenGL Context（当前绑定） | 命令发送 |
| `IDXGIAdapter` | `glGetString(GL_RENDERER)` | 物理显卡信息 |
| `IDXGIFactory` | 平台窗口系统（WGL/GLX/EGL） | 顶层管理 |
| `D3D_FEATURE_LEVEL` | `glGetIntegerv` / `glGetString(GL_VERSION)` | 能力查询 |

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

**OpenGL4**

```cpp
// 需要先创建临时上下文，再创建 4.x Core Profile 上下文
// 以 Windows WGL 为例（实际项目中通常使用 GLFW/SDL 封装）

// 1. 设置像素格式
PIXELFORMATDESCRIPTOR pfd{};
pfd.nSize = sizeof(pfd);
pfd.nVersion = 1;
pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
pfd.iPixelType = PFD_TYPE_RGBA;
pfd.cColorBits = 32;
pfd.cDepthBits = 24;
pfd.cStencilBits = 8;
HDC hdc = GetDC(hwnd);
int pixelFormat = ChoosePixelFormat(hdc, &pfd);
SetPixelFormat(hdc, pixelFormat, &pfd);

// 2. 创建临时上下文以获取扩展函数
HGLRC tempRC = wglCreateContext(hdc);
wglMakeCurrent(hdc, tempRC);
// 加载 wglCreateContextAttribsARB（通过 GLAD/GLEW）

// 3. 创建 OpenGL 4.x Core Profile 上下文
int attribs[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
    WGL_CONTEXT_MINOR_VERSION_ARB, 0,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
    0
};
HGLRC glContext = wglCreateContextAttribsARB(hdc, nullptr, attribs);
wglMakeCurrent(hdc, glContext);
wglDeleteContext(tempRC);

// 4. 查询能力
const char* version = (const char*)glGetString(GL_VERSION);
const char* renderer = (const char*)glGetString(GL_RENDERER);
```

---

## 2. 交换链与呈现

### 概念对比

| 维度 | D3D11 | OpenGL4 |
|------|-------|---------|
| 交换链归属 | DXGI 层 | 平台窗口系统（WGL/GLX/EGL） |
| Surface | 隐含在 HWND | 隐含在 HDC / Display |
| 缓冲切换 | `Present` 调用 | `SwapBuffers` / `glXSwapBuffers` |
| 双缓冲 | 通过 SwapChain 描述 | 在像素格式中指定 PFD_DOUBLEBUFFER |

### 接口映射

| D3D11 / DXGI | OpenGL4 | 说明 |
|-------|---------|------|
| `IDXGIFactory::CreateSwapChain` | 像素格式 + 双缓冲设置 | 创建时隐含 |
| `IDXGISwapChain` | 默认帧缓冲 | 系统管理 |
| `IDXGISwapChain::Present` | `SwapBuffers(hdc)` / `glXSwapBuffers` | 呈现 |
| `IDXGISwapChain::GetBuffer` | 默认帧缓冲（FBO 0） | 后缓冲 |
| `IDXGISwapChain::ResizeBuffers` | 窗口 resize 后自动调整 / `glViewport` | 窗口 resize |
| VSync 控制 | `wglSwapIntervalEXT(1)` | 垂直同步 |

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

**OpenGL4**

```cpp
// 交换链由窗口系统在上下文创建时隐式建立
// 双缓冲在 PixelFormat 中指定（见第1节）

// 设置 VSync
wglSwapIntervalEXT(1); // 1 = VSync 开启, 0 = 关闭

// 窗口大小变化时更新视口
glViewport(0, 0, 1280, 720);

// 每帧结束：呈现
SwapBuffers(hdc);
```

---

## 3. 命令录制与提交

### 概念对比

| 维度 | D3D11 | OpenGL4 |
|------|-------|---------|
| 模式 | 即时模式（调用即执行） | 即时模式（调用即执行） |
| 多线程 | Deferred Context（可选） | 需共享 Context，多线程支持有限 |
| 命令缓冲 | 通过 Deferred Context 生成 CommandList | 无原生 Command Buffer |
| 批处理 | 驱动内部优化 | 驱动内部优化 |

### 接口映射

| D3D11 | OpenGL4 | 说明 |
|-------|---------|------|
| `ID3D11DeviceContext` (Immediate) | 当前 OpenGL Context | 即时命令 |
| `ID3D11DeviceContext` (Deferred) | 无直接等价（可用 NV_command_list 扩展） | 多线程录制 |
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

**OpenGL4**

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

| 维度 | D3D11 | OpenGL4 |
|------|-------|---------|
| 渲染通道 | 隐式（绑定 RTV/DSV 即开始） | 隐式（绑定 FBO 即开始） |
| Attachment | 绑定 View 对象 | FBO Attachment（颜色/深度/模板） |
| Framebuffer | 隐含在 RTV/DSV | 显式 FBO 对象（0 = 默认帧缓冲） |
| 多渲染目标 | 绑定多个 RTV | `glDrawBuffers` 指定多个颜色附件 |

### 接口映射

| D3D11 | OpenGL4 | 说明 |
|-------|---------|------|
| `OMSetRenderTargets` | `glBindFramebuffer` + `glFramebufferTexture2D` | 设置渲染目标 |
| — (隐式结束) | `glBindFramebuffer(GL_FRAMEBUFFER, 0)` | 切换回默认帧缓冲 |
| `ID3D11RenderTargetView` | FBO Color Attachment | 颜色附件 |
| `ID3D11DepthStencilView` | FBO Depth/Stencil Attachment | 深度模板附件 |
| — | `glGenFramebuffers` / `glDeleteFramebuffers` | FBO 生命周期 |
| — | `glCheckFramebufferStatus` | 完整性检查 |

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

**OpenGL4**

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

// 切换回默认帧缓冲
glBindFramebuffer(GL_FRAMEBUFFER, 0);
```

---

## 5. 缓冲区

### 概念对比

| 维度 | D3D11 | OpenGL4 |
|------|-------|---------|
| 内存管理 | 驱动自动 | 驱动自动 |
| 创建步骤 | 一步 CreateBuffer | `glGenBuffers` + `glBufferData` |
| 类型区分 | BindFlags 区分用途 | Target 绑定点区分（GL_ARRAY_BUFFER 等） |
| 数据更新 | Map/Unmap 或 UpdateSubresource | `glMapBuffer` / `glBufferSubData` |

### 接口映射

| D3D11 | OpenGL4 | 说明 |
|-------|---------|------|
| `ID3D11Device::CreateBuffer` | `glGenBuffers` + `glBufferData` | 创建缓冲 |
| `ID3D11Buffer` | `GLuint` (buffer name) | 缓冲对象 |
| `IASetVertexBuffers` | `glBindVertexBuffer` / `glVertexAttribPointer` (在 VAO 中) | 绑定 VB |
| `IASetIndexBuffer` | `glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ...)` (在 VAO 中) | 绑定 IB |
| `VSSetConstantBuffers` / `PSSetConstantBuffers` | `glBindBufferBase(GL_UNIFORM_BUFFER, ...)` | 绑定 UBO |
| `D3D11_USAGE_DYNAMIC` | `GL_DYNAMIC_DRAW` | 动态用途提示 |
| `D3D11_USAGE_DEFAULT` | `GL_STATIC_DRAW` | 静态用途提示 |

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

**OpenGL4**

```cpp
// 创建顶点缓冲
GLuint vbo;
glGenBuffers(1, &vbo);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// 在 VAO 中配置顶点属性
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
```

---

## 6. 纹理与图像

### 概念对比

| 维度 | D3D11 | OpenGL4 |
|------|-------|---------|
| 纹理对象 | `ID3D11Texture2D` + View 分离 | `GLuint` 纹理对象统一 |
| 视图 | SRV/RTV/DSV/UAV 分离 | 纹理本身可直接绑定，FBO 附件作为"视图" |
| 数据上传 | `UpdateSubresource` / Map | `glTexImage2D` / `glTexSubImage2D` |
| 布局转换 | 驱动自动 | 驱动自动 |

### 接口映射

| D3D11 | OpenGL4 | 说明 |
|-------|---------|------|
| `CreateTexture2D` | `glGenTextures` + `glTexImage2D` | 创建纹理 |
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

**OpenGL4**

```cpp
// 创建纹理
GLuint texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);

// 设置纹理参数
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// 上传数据
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
glGenerateMipmap(GL_TEXTURE_2D);

// 绑定到着色器（纹理单元 0）
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texture);
// 在 shader 中: uniform sampler2D diffuseMap; 对应 location 0
glUniform1i(glGetUniformLocation(program, "diffuseMap"), 0);
```

---

## 7. 采样器

### 概念对比

| 维度 | D3D11 | OpenGL4 |
|------|-------|---------|
| 采样器对象 | 独立 SamplerState 对象 | 独立 Sampler Object（GL 3.3+） 或纹理参数 |
| 绑定方式 | 按 slot 绑定 | 按纹理单元绑定 |
| 兼容模式 | — | 也可直接设置纹理参数（旧方式） |

### 接口映射

| D3D11 | OpenGL4 | 说明 |
|-------|---------|------|
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

**OpenGL4**

```cpp
// 使用独立 Sampler Object（推荐）
GLuint sampler;
glGenSamplers(1, &sampler);
glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, GL_REPEAT);
glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
glSamplerParameterf(sampler, GL_TEXTURE_MIN_LOD, 0.0f);
glSamplerParameterf(sampler, GL_TEXTURE_MAX_LOD, 1000.0f);

// 绑定到纹理单元 0
glBindSampler(0, sampler);
```

---

## 8. 着色器

### 概念对比

| 维度 | D3D11 | OpenGL4 |
|------|-------|---------|
| 语言 | HLSL | GLSL |
| 编译 | 运行时 D3DCompile 或离线 fxc | 运行时 `glCompileShader` 或离线（SPIR-V via GL_ARB_gl_spirv） |
| 对象 | 按阶段分类（ID3D11VertexShader 等） | 统一 Shader Object + Program 链接 |
| 反射 | D3DReflect | `glGetActiveUniform` / `glGetProgramResource` |
| 链接 | 隐式 | 显式 Program 链接 |

### 接口映射

| D3D11 | OpenGL4 | 说明 |
|-------|---------|------|
| `D3DCompile` | `glShaderSource` + `glCompileShader` | 编译着色器 |
| `CreateVertexShader` | `glCreateShader(GL_VERTEX_SHADER)` | 创建 VS |
| `CreatePixelShader` | `glCreateShader(GL_FRAGMENT_SHADER)` | 创建 PS/FS |
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

**OpenGL4**

```cpp
// 顶点着色器
const char* vsSrc = R"(
#version 400 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
uniform mat4 uMVP;
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

// 片段着色器
const char* fsSrc = R"(
#version 400 core
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

| 维度 | D3D11 | OpenGL4 |
|------|-------|---------|
| 管理方式 | 独立 State Object 分别创建/绑定 | 全局状态机，逐项设置 |
| 混合 | `ID3D11BlendState`（独立对象） | `glEnable(GL_BLEND)` + `glBlendFunc` |
| 光栅化 | `ID3D11RasterizerState`（独立对象） | `glPolygonMode` / `glCullFace` |
| 深度模板 | `ID3D11DepthStencilState`（独立对象） | `glEnable(GL_DEPTH_TEST)` + `glDepthFunc` |
| 输入布局 | `ID3D11InputLayout` 对象 | VAO 中配置 |

### 接口映射

| D3D11 | OpenGL4 | 说明 |
|-------|---------|------|
| `CreateBlendState` | `glBlendFunc` / `glBlendEquation` | 混合状态 |
| `CreateRasterizerState` | `glPolygonMode` / `glCullFace` / `glFrontFace` | 光栅化状态 |
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

**OpenGL4**

```cpp
// 光栅化状态
glEnable(GL_CULL_FACE);
glCullFace(GL_BACK);
glFrontFace(GL_CCW);
glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
glEnable(GL_DEPTH_CLAMP); // 等效 DepthClipEnable

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

| 维度 | D3D11 | OpenGL4 |
|------|-------|---------|
| 模型 | Slot-based（按阶段分 slot） | Binding Point（纹理单元 / UBO 绑定点） |
| 布局 | 隐式（shader 反射） | 显式 `layout(binding=N)` 或运行时查询 |
| 更新 | 直接 `XSSet*` 调用 | `glBindBufferBase` / `glActiveTexture` + `glBindTexture` |
| Uniform | Constant Buffer 整块 | UBO 整块 或单个 Uniform 变量 |

### 接口映射

| D3D11 | OpenGL4 | 说明 |
|-------|---------|------|
| `VSSetConstantBuffers` / `PSSetConstantBuffers` | `glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo)` | UBO 绑定 |
| `PSSetShaderResources` | `glActiveTexture(GL_TEXTURE0+n)` + `glBindTexture` | 纹理绑定 |
| `PSSetSamplers` | `glBindSampler(unit, sampler)` | 采样器绑定 |
| Register (b0, t0, s0) | Binding point / Texture unit | 槽位 |
| — | `glUniformBlockBinding` | 关联 UBO block 与绑定点 |
| — | `glUniform*` | 单个 Uniform（简单场景） |

### 使用示例

**D3D11**

```cpp
context->VSSetConstantBuffers(0, 1, &mvpBuffer);
context->PSSetConstantBuffers(0, 1, &materialBuffer);
context->PSSetShaderResources(0, 1, &diffuseSRV);
context->PSSetSamplers(0, 1, &linearSampler);
```

**OpenGL4**

```cpp
// UBO 绑定（对应 HLSL 的 cbuffer）
// GLSL 中: layout(std140, binding = 0) uniform MVP { mat4 mvp; };
glBindBufferBase(GL_UNIFORM_BUFFER, 0, mvpUBO);

// 也可以运行时关联:
GLuint blockIndex = glGetUniformBlockIndex(program, "MVP");
glUniformBlockBinding(program, blockIndex, 0); // binding point 0

// 材质 UBO
glBindBufferBase(GL_UNIFORM_BUFFER, 1, materialUBO);

// 纹理绑定
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, diffuseTexture);
glUniform1i(glGetUniformLocation(program, "diffuseMap"), 0);

// 采样器绑定
glBindSampler(0, linearSampler);
```

---

## 11. 渲染目标

### 接口映射

| D3D11 | OpenGL4 | 说明 |
|-------|---------|------|
| `CreateRenderTargetView` | `glFramebufferTexture2D(GL_COLOR_ATTACHMENT0, ...)` | RTV |
| `CreateDepthStencilView` | `glFramebufferTexture2D(GL_DEPTH_STENCIL_ATTACHMENT, ...)` | DSV |
| `OMSetRenderTargets` | `glBindFramebuffer(GL_FRAMEBUFFER, fbo)` | 设置 RT |
| `ClearRenderTargetView` | `glClearBufferfv(GL_COLOR, 0, color)` | 清除颜色 |
| `ClearDepthStencilView` | `glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0)` | 清除深度/模板 |
| MRT (多渲染目标) | `glDrawBuffers` | 多颜色附件 |

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

**OpenGL4**

```cpp
// 创建颜色纹理
GLuint colorTex;
glGenTextures(1, &colorTex);
glBindTexture(GL_TEXTURE_2D, colorTex);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1280, 720, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

// 创建深度/模板纹理
GLuint depthTex;
glGenTextures(1, &depthTex);
glBindTexture(GL_TEXTURE_2D, depthTex);
glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 1280, 720, 0,
    GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

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

// 渲染到默认帧缓冲（后缓冲）
glBindFramebuffer(GL_FRAMEBUFFER, 0);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

---

## 12. 绘制命令

### 接口映射

| D3D11 | OpenGL4 | 说明 |
|-------|---------|------|
| `Draw` | `glDrawArrays` | 非索引 |
| `DrawIndexed` | `glDrawElements` | 索引 |
| `DrawInstanced` | `glDrawArraysInstanced` | 实例化 |
| `DrawIndexedInstanced` | `glDrawElementsInstanced` | 索引实例化 |
| `DrawInstancedIndirect` | `glDrawArraysIndirect` | 间接 |
| `DrawIndexedInstancedIndirect` | `glDrawElementsIndirect` | 间接索引 |
| `Dispatch` | `glDispatchCompute` | 计算着色器 |
| `DrawAuto` (StreamOutput) | `glDrawTransformFeedback` | Transform Feedback |

### 使用示例

**D3D11**

```cpp
context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
context->DrawIndexed(36, 0, 0);              // 索引绘制
context->Draw(3, 0);                          // 非索引
context->DrawIndexedInstanced(36, 100, 0, 0, 0); // 实例化
```

**OpenGL4**

```cpp
glBindVertexArray(vao); // 必须绑定 VAO

// 索引绘制
glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

// 非索引绘制
glDrawArrays(GL_TRIANGLES, 0, 3);

// 实例化绘制
glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr, 100);

// 间接绘制 (GL 4.0+)
// 参数存储在 GL_DRAW_INDIRECT_BUFFER 中
glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr);

// 计算着色器 (GL 4.3+)
glUseProgram(computeProgram);
glDispatchCompute(groupsX, groupsY, groupsZ);
glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
```

---

## 13. 资源数据传输

### 接口映射

| D3D11 | OpenGL4 | 说明 |
|-------|---------|------|
| `Map` / `Unmap` | `glMapBuffer` / `glUnmapBuffer` | CPU 映射 |
| — | `glMapBufferRange` | 映射部分区域（更灵活） |
| `UpdateSubresource` | `glBufferSubData` / `glTexSubImage2D` | 更新数据 |
| `CopyResource` | `glCopyBufferSubData` / `glCopyImageSubData` | GPU 复制 |
| `CopySubresourceRegion` | `glCopyImageSubData` (GL 4.3+) | 子资源复制 |
| — | `glBlitFramebuffer` | 帧缓冲间 blit（带缩放） |
| Staging Buffer | `GL_MAP_READ_BIT` + PBO | 回读到 CPU |

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

**OpenGL4**

```cpp
// glMapBufferRange 更新动态缓冲（推荐）
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
void* ptr = glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(data),
    GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
memcpy(ptr, &data, sizeof(data));
glUnmapBuffer(GL_UNIFORM_BUFFER);

// 或使用 glBufferSubData（更简单）
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data);

// GPU buffer 复制 (GL 3.1+)
glBindBuffer(GL_COPY_READ_BUFFER, srcBuffer);
glBindBuffer(GL_COPY_WRITE_BUFFER, dstBuffer);
glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);

// 帧缓冲 Blit（带缩放）
glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFBO);
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFBO);
glBlitFramebuffer(0, 0, srcW, srcH, 0, 0, dstW, dstH,
    GL_COLOR_BUFFER_BIT, GL_LINEAR);

// 图像复制 (GL 4.3+)
glCopyImageSubData(srcTex, GL_TEXTURE_2D, 0, 0, 0, 0,
    dstTex, GL_TEXTURE_2D, 0, 0, 0, 0, width, height, 1);
```

---

## 14. 同步

### 概念对比

| 维度 | D3D11 | OpenGL4 |
|------|-------|---------|
| 模型 | 驱动隐式同步 | 驱动隐式同步（大部分情况） |
| CPU-GPU | Map 时自动等待 | `glMapBuffer` 自动等待 / `glFenceSync` 手动 |
| GPU-GPU | 隐式 | 隐式 / `glMemoryBarrier` (计算着色器) |
| 显式同步 | 无（D3D11 无 Fence） | `glFenceSync` + `glClientWaitSync` (GL 3.2+) |

### 接口映射

| D3D11 | OpenGL4 | 说明 |
|-------|---------|------|
| 隐式 | `glFenceSync` + `glClientWaitSync` | CPU 等 GPU |
| 隐式 | `glMemoryBarrier` | 内存屏障（计算着色器后） |
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

**OpenGL4**

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

// 计算着色器后的内存屏障
glDispatchCompute(groupsX, groupsY, groupsZ);
glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
// 现在可以安全读取计算着色器写入的数据

// 强制完成所有命令
glFinish();
```

---

## 15. 枚举映射

### 像素格式

| DXGI_FORMAT | OpenGL Internal Format | GL Format / Type |
|-------------|----------------------|------------------|
| `R8G8B8A8_UNORM` | `GL_RGBA8` | `GL_RGBA` / `GL_UNSIGNED_BYTE` |
| `B8G8R8A8_UNORM` | `GL_RGBA8` (需 swizzle 或 `GL_BGRA`) | `GL_BGRA` / `GL_UNSIGNED_BYTE` |
| `R32G32B32A32_FLOAT` | `GL_RGBA32F` | `GL_RGBA` / `GL_FLOAT` |
| `R32G32B32_FLOAT` | `GL_RGB32F` | `GL_RGB` / `GL_FLOAT` |
| `R32G32_FLOAT` | `GL_RG32F` | `GL_RG` / `GL_FLOAT` |
| `R32_FLOAT` | `GL_R32F` | `GL_RED` / `GL_FLOAT` |
| `R16G16B16A16_FLOAT` | `GL_RGBA16F` | `GL_RGBA` / `GL_HALF_FLOAT` |
| `D24_UNORM_S8_UINT` | `GL_DEPTH24_STENCIL8` | `GL_DEPTH_STENCIL` / `GL_UNSIGNED_INT_24_8` |
| `D32_FLOAT` | `GL_DEPTH_COMPONENT32F` | `GL_DEPTH_COMPONENT` / `GL_FLOAT` |
| `D32_FLOAT_S8X24_UINT` | `GL_DEPTH32F_STENCIL8` | `GL_DEPTH_STENCIL` / `GL_FLOAT_32_UNSIGNED_INT_24_8_REV` |
| `R16_UINT` | `GL_R16UI` | `GL_RED_INTEGER` / `GL_UNSIGNED_SHORT` |
| `R32_UINT` | `GL_R32UI` | `GL_RED_INTEGER` / `GL_UNSIGNED_INT` |
| `BC1_UNORM` | `GL_COMPRESSED_RGBA_S3TC_DXT1_EXT` | — (compressed) |
| `BC3_UNORM` | `GL_COMPRESSED_RGBA_S3TC_DXT5_EXT` | — (compressed) |
| `BC5_UNORM` | `GL_COMPRESSED_RG_RGTC2` | — (compressed) |
| `BC7_UNORM` | `GL_COMPRESSED_RGBA_BPTC_UNORM` | — (compressed) |

### 图元拓扑

| D3D11 | OpenGL4 |
|-------|---------|
| `TRIANGLELIST` | `GL_TRIANGLES` |
| `TRIANGLESTRIP` | `GL_TRIANGLE_STRIP` |
| `LINELIST` | `GL_LINES` |
| `LINESTRIP` | `GL_LINE_STRIP` |
| `POINTLIST` | `GL_POINTS` |
| `TRIANGLELIST_ADJ` | `GL_TRIANGLES_ADJACENCY` |
| `LINELIST_ADJ` | `GL_LINES_ADJACENCY` |

### 填充模式

| D3D11_FILL_MODE | OpenGL4 |
|-----------------|---------|
| `D3D11_FILL_SOLID` | `GL_FILL` (`glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)`) |
| `D3D11_FILL_WIREFRAME` | `GL_LINE` (`glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)`) |

### 剔除模式

| D3D11_CULL_MODE | OpenGL4 |
|-----------------|---------|
| `D3D11_CULL_NONE` | `glDisable(GL_CULL_FACE)` |
| `D3D11_CULL_FRONT` | `glEnable(GL_CULL_FACE)` + `glCullFace(GL_FRONT)` |
| `D3D11_CULL_BACK` | `glEnable(GL_CULL_FACE)` + `glCullFace(GL_BACK)` |

### 比较函数

| D3D11_COMPARISON_FUNC | OpenGL4 |
|-----------------------|---------|
| `NEVER` | `GL_NEVER` |
| `LESS` | `GL_LESS` |
| `EQUAL` | `GL_EQUAL` |
| `LESS_EQUAL` | `GL_LEQUAL` |
| `GREATER` | `GL_GREATER` |
| `NOT_EQUAL` | `GL_NOTEQUAL` |
| `GREATER_EQUAL` | `GL_GEQUAL` |
| `ALWAYS` | `GL_ALWAYS` |

### 混合因子

| D3D11_BLEND | OpenGL4 |
|-------------|---------|
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

| D3D11_BLEND_OP | OpenGL4 |
|----------------|---------|
| `ADD` | `GL_FUNC_ADD` |
| `SUBTRACT` | `GL_FUNC_SUBTRACT` |
| `REV_SUBTRACT` | `GL_FUNC_REVERSE_SUBTRACT` |
| `MIN` | `GL_MIN` |
| `MAX` | `GL_MAX` |

### 纹理寻址模式

| D3D11_TEXTURE_ADDRESS_MODE | OpenGL4 |
|----------------------------|---------|
| `WRAP` | `GL_REPEAT` |
| `MIRROR` | `GL_MIRRORED_REPEAT` |
| `CLAMP` | `GL_CLAMP_TO_EDGE` |
| `BORDER` | `GL_CLAMP_TO_BORDER` |
| `MIRROR_ONCE` | `GL_MIRROR_CLAMP_TO_EDGE` (GL 4.4+) |

### 过滤模式

| D3D11_FILTER | GL_TEXTURE_MIN_FILTER / GL_TEXTURE_MAG_FILTER |
|-------------|-----------------------------------------------|
| `MIN_MAG_MIP_POINT` | min: `GL_NEAREST_MIPMAP_NEAREST`, mag: `GL_NEAREST` |
| `MIN_MAG_MIP_LINEAR` | min: `GL_LINEAR_MIPMAP_LINEAR`, mag: `GL_LINEAR` |
| `MIN_MAG_POINT_MIP_LINEAR` | min: `GL_NEAREST_MIPMAP_LINEAR`, mag: `GL_NEAREST` |
| `MIN_MAG_LINEAR_MIP_POINT` | min: `GL_LINEAR_MIPMAP_NEAREST`, mag: `GL_LINEAR` |
| `ANISOTROPIC` | min: `GL_LINEAR_MIPMAP_LINEAR`, mag: `GL_LINEAR` + `GL_TEXTURE_MAX_ANISOTROPY_EXT` |

---

## 附录：核心差异总结

| 领域 | D3D11 特点 | OpenGL4 特点 |
|------|-----------|-------------|
| 设计模式 | 对象模型，状态封装在对象中 | 状态机模型，全局状态逐项设置 |
| 初始化 | 简单，1 函数创建 Device+Context | 平台相关，需要窗口系统集成（WGL/GLX/EGL） |
| 内存 | 驱动管理 | 驱动管理（两者类似） |
| 命令 | 即时执行 | 即时执行（两者类似） |
| 渲染通道 | 隐式（绑定 RTV/DSV） | 隐式（绑定 FBO） |
| 管线状态 | 独立 State Object 分组管理 | 全局状态逐项设置（无法原子切换） |
| 资源绑定 | Slot-based，按阶段分 | Binding Point + Texture Unit |
| 着色器 | HLSL，编译为字节码 | GLSL，运行时编译（或 SPIR-V） |
| 顶点输入 | Input Layout 对象 | VAO + VBO 配置 |
| 同步 | 驱动隐式 | 驱动隐式（+ 可选 Fence/Barrier） |
| 跨平台 | 仅 Windows | 跨平台（Windows/Linux/macOS*） |
| 错误处理 | HRESULT 返回值 | `glGetError` / Debug Callback |
| 扩展机制 | Feature Level 分级 | Extension 逐项查询 |

> *注：macOS 上 OpenGL 已被废弃（最高支持 4.1），建议使用 Metal。
