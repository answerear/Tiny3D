# Direct3D 11 渲染器后端架构

## 概述

Tiny3D 的 Direct3D 11 渲染器后端以**插件**（Plugin）形式存在，通过 DLL 动态加载到引擎中。整个后端分为三个构建目标，面向不同使用场景：

| 构建目标 | 场景 | 特点 |
|----------|------|------|
| **D3D11Renderer** | 游戏运行时 | 完整 GPU 渲染，链接 T3DCore |
| **D3D11RendererEditor** | 编辑器内嵌视口 | 同 Runtime 代码 + T3D_EDITOR 宏，暴露 Device/Context 给 ImGui |
| **D3D11RendererConsole** | 编辑器后台/离屏处理 | 轻量桩实现，无真实 GPU 操作 |

三者共享 `Base/` 目录中的基类代码，通过分层继承实现代码复用。

---

## 目录结构

```
source/Plugins/Renderer/Direct3D11/
├── CMakeLists.txt                          # 顶层: add_subdirectory(Window) + add_subdirectory(Console)
├── Base/                                    # 共享基类
│   ├── Include/
│   │   ├── T3DD3D11ContextBase.h           # D3D11ContextBase 抽象基类
│   │   ├── T3DD3D11Error.h                 # D3D11 错误码枚举
│   │   ├── T3DD3D11Mapping.h               # 引擎枚举 ↔ D3D11 枚举映射
│   │   └── T3DD3D11PrerequisitesBase.h     # 基础前置声明、DLL导出宏
│   └── Source/
│       ├── T3DD3D11ContextBase.cpp         # Shader 编译/反射/投影矩阵变换
│       └── T3DD3D11Mapping.cpp             # 映射表实现
│
├── Window/                                  # 窗口渲染器（完整 GPU 渲染）
│   ├── CMakeLists.txt                      # add_subdirectory(Runtime) + add_subdirectory(Editor)
│   ├── Include/
│   │   ├── T3DD3D11Context.h              # D3D11Context 核心 RHI 上下文
│   │   ├── T3DD3D11Prerequisites.h        # 前置声明 + 智能指针 typedef
│   │   ├── T3DD3D11RenderBuffer.h         # 顶点/索引/常量/像素缓冲区
│   │   ├── T3DD3D11Renderer.h             # D3D11Renderer 单例
│   │   ├── T3DD3D11RenderState.h          # Blend/Rasterizer/DepthStencil/Sampler
│   │   ├── T3DD3D11RenderWindow.h         # SwapChain + RTV + DSV
│   │   └── T3DD3D11Shader.h              # VS/PS/GS/HS/DS/CS 封装
│   ├── Source/
│   │   ├── T3DD3D11Context.cpp            # 上下文完整实现
│   │   ├── T3DD3D11RenderBuffer.cpp       # Buffer 创建/映射/销毁
│   │   ├── T3DD3D11Renderer.cpp           # Renderer 初始化/销毁
│   │   ├── T3DD3D11RenderState.cpp        # 渲染状态创建/销毁
│   │   ├── T3DD3D11RenderWindow.cpp       # SwapChain 管理
│   │   └── T3DD3D11Shader.cpp            # Shader 编译/创建
│   ├── Runtime/                            # → D3D11Renderer.dll
│   │   ├── CMakeLists.txt
│   │   ├── Include/T3DD3D11Plugin.h
│   │   └── Source/
│   │       ├── T3DD3D11Plugin.cpp
│   │       └── T3DD3D11PluginDLL.cpp      # DLL 导出入口
│   └── Editor/                            # → D3D11RendererEditor.dll
│       ├── CMakeLists.txt
│       ├── Include/T3DD3D11PluginEditor.h
│       └── Source/
│           ├── T3DD3D11PluginEditor.cpp
│           └── T3DD3D11PluginEditorDLL.cpp
│
└── Console/                                # → D3D11RendererConsole.dll（桩实现）
    ├── CMakeLists.txt
    ├── Include/
    │   ├── T3DD3D11ConsoleContext.h
    │   ├── T3DD3D11ConsolePlugin.h
    │   ├── T3DD3D11ConsolePrerequisites.h
    │   ├── T3DD3D11ConsoleRenderBuffer.h
    │   ├── T3DD3D11ConsoleRenderer.h
    │   ├── T3DD3D11ConsoleRenderState.h
    │   ├── T3DD3D11ConsoleShader.h
    │   └── T3DD3D11ConsoleWindow.h
    └── Source/
        ├── T3DD3D11ConsoleContext.cpp
        ├── T3DD3D11ConsolePlugin.cpp
        ├── T3DD3D11ConsolePluginDLL.cpp
        ├── T3DD3D11ConsoleRenderBuffer.cpp
        ├── T3DD3D11ConsoleRenderer.cpp
        ├── T3DD3D11ConsoleRenderState.cpp
        ├── T3DD3D11ConsoleShader.cpp
        └── T3DD3D11ConsoleWindow.cpp
```

---

## CMake 构建体系

### 构建目标对比

| 目标 | 输出类型 | 链接库 | 关键编译宏 |
|------|---------|--------|-----------|
| D3D11Renderer | SHARED (DLL) | DirectX11, d3dcompiler, T3DMath, T3DLog, T3DUtils, T3DPlatform, T3DSystem, **T3DCore**, rttr_core | `D3D11RENDERER_EXPORT` |
| D3D11RendererEditor | SHARED (DLL) | 同上 + **T3DCoreEditor** 替代 T3DCore | `D3D11RENDERER_EXPORT`, **`T3D_EDITOR`** |
| D3D11RendererConsole | SHARED (DLL) | DirectX11, d3dcompiler, T3DMath, T3DLog, T3DUtils, T3DPlatform, T3DSystem, **T3DCoreEditor**, rttr_core | `D3D11CONSOLERDR_EXPORT`, **`T3D_EDITOR`** |

### 源文件组织

- **D3D11Renderer** 和 **D3D11RendererEditor** 共用 `Window/Include` + `Window/Source` + `Base/` 的全部源码，仅在 Plugin 入口和链接库上有差异。
- **D3D11RendererConsole** 使用独立的 `Console/` 源码 + `Base/`，实现最简化的桩（stub）渲染。

---

## 类继承层次

### 核心继承关系

```
RHIContext (引擎接口)
  └── D3D11ContextBase (Base/)         ← Shader 编译/反射/投影变换
        ├── D3D11Context (Window/)     ← 完整 GPU 渲染实现
        └── D3D11ConsoleContext (Console/) ← 桩实现

RHIRenderer (引擎接口)
  ├── D3D11Renderer (Window/)          ← 单例, 管理 Context 生命周期
  └── D3D11RendererConsole (Console/)  ← 单例, 管理 ConsoleContext

Plugin (引擎接口)
  ├── D3D11Plugin (Window/Runtime/)    ← 运行时插件
  ├── D3D11Plugin (Window/Editor/)     ← 编辑器插件 (同名不同文件)
  └── D3D11ConsolePlugin (Console/)    ← 控制台插件

RHIRenderWindow (引擎接口)
  ├── D3D11RenderWindow (Window/)      ← 真实 SwapChain
  └── D3D11ConsoleWindow (Console/)    ← 桩窗口

RHIShader (引擎接口)
  ├── D3D11VertexShader               ← ID3D11VertexShader
  ├── D3D11PixelShader                ← ID3D11PixelShader
  ├── D3D11GeometryShader             ← ID3D11GeometryShader
  ├── D3D11HullShader                 ← ID3D11HullShader
  ├── D3D11DomainShader               ← ID3D11DomainShader
  ├── D3D11ComputeShader              ← ID3D11ComputeShader
  └── D3D11ConsoleShader              ← 桩

RHIVertexBuffer / RHIIndexBuffer / RHIConstantBuffer / RHIPixelBuffer (引擎接口)
  ├── D3D11VertexBuffer / D3D11IndexBuffer / D3D11ConstantBuffer / D3D11PixelBuffer*  ← 真实 ID3D11Buffer
  └── D3D11Console* 对应版本           ← 桩

RHIBlendState / RHIRasterizerState / RHIDepthStencilState / RHISamplerState (引擎接口)
  ├── D3D11BlendState / D3D11RasterizerState / D3D11DepthStencilState / D3D11SamplerState
  └── D3D11Console* 对应版本           ← 桩
```

---

## 插件加载机制

### DLL 入口

每个构建目标导出两个 C 函数：

```cpp
// T3DD3D11PluginDLL.cpp
extern "C" T3D_DLL_EXPORT void dllStartPlugin();
extern "C" T3D_DLL_EXPORT void dllStopPlugin();
```

### 加载流程

```
Agent::loadPlugin("D3D11Renderer")
  → LoadLibrary / dlopen
    → dllStartPlugin()
      → new D3D11Plugin()
        → Agent::installPlugin(plugin)
          → plugin->install()                  // 注册
          → plugin->startup()
            → D3D11Renderer::create()          // 创建 Renderer 单例
            → Agent::addRHIRenderer(renderer)  // 注册到引擎
```

### 卸载流程

```
Agent::unloadPlugin("D3D11Renderer")
  → plugin->shutdown()
    → Agent::removeRHIRenderer()
    → D3D11Renderer::destroy()       // 销毁 Context + 释放 COM 资源
  → plugin->uninstall()
  → dllStopPlugin()
    → delete plugin
  → FreeLibrary / dlclose
```

---

## 核心类详解

### D3D11ContextBase（Base 层）

**职责**: 提供与窗口/Console 无关的共享功能。

```cpp
class D3D11ContextBase : public RHIContext
{
protected:
    // Shader 源码编译 (HLSL → bytecode)
    virtual TResult compileShader(const String &source, const String &entry,
                                   const String &target, ID3DBlob **blob);

    // Shader 反射：提取 ConstantBuffer / 输入布局
    virtual TResult reflectShader(ID3DBlob *blob, ShaderReflection &reflection);

    // OpenGL → D3D 投影矩阵转换
    virtual Matrix4 convertProjectionMatrix(const Matrix4 &proj);
};
```

### D3D11Context（Window 层 - 完整实现）

**职责**: 持有 D3D11 设备和上下文，实现全部 RHI 接口。

**关键成员**:
```cpp
class D3D11Context : public D3D11ContextBase
{
    ID3D11Device           *mD3DDevice;          // GPU 设备
    ID3D11DeviceContext    *mD3DDeviceContext;    // 即时上下文
    IDXGIFactory           *mDXGIFactory;         // DXGI 工厂
    D3D_FEATURE_LEVEL       mFeatureLevel;        // 硬件特性级别

#ifdef T3D_EDITOR
    EditorInfoDX11          mEditorInfo;          // 暴露给 ImGui 的信息
#endif
};
```

**核心方法分组**:

| 方法组 | 职责 |
|--------|------|
| `init() / destroy()` | 创建/释放 Device + DeviceContext |
| `createRenderWindow()` | 创建 SwapChain 窗口 |
| `createVertexBuffer() / createIndexBuffer() / createConstantBuffer()` | GPU Buffer 工厂 |
| `createPixelBuffer1D/2D/3D()` | 纹理资源工厂 |
| `createBlendState() / createRasterizerState() / ...` | 渲染状态工厂 |
| `createVertexShader() / createPixelShader() / ...` | Shader 工厂 |
| `renderAllTargets()` | 主渲染循环入口 |
| `setViewport() / setScissorRect()` | 视口/裁剪 |
| `bindRenderTarget() / clearRenderTarget()` | RTV/DSV 绑定 |
| `drawIndexed() / drawInstanced()` | 绘制命令 |

**Editor 模式扩展**:
```cpp
#ifdef T3D_EDITOR
    struct EditorInfoDX11 {
        ID3D11Device          *Device;
        ID3D11DeviceContext   *DeviceContext;
        ID3D11RenderTargetView *MainRTView;
    };

    const EditorInfoDX11 &getEditorInfo() const;
#endif
```

### D3D11Renderer（单例）

**职责**: 管理 Context 的创建与销毁，作为插件和引擎之间的桥梁。

```cpp
class D3D11Renderer : public RHIRenderer, public Singleton<D3D11Renderer>
{
public:
    static D3D11RendererPtr create();

    virtual RHIContextPtr getContext() const override;

#ifdef T3D_EDITOR
    virtual const EditorInfoDX11 &getEditorInfo() const;
#endif

private:
    D3D11ContextPtr mContext;
};
```

### D3D11RenderWindow

**职责**: 封装 DXGI SwapChain + BackBuffer RTV + DepthStencil DSV。

```cpp
class D3D11RenderWindow : public RHIRenderWindow
{
    IDXGISwapChain              *mSwapChain;
    ID3D11RenderTargetView      *mRenderTargetView;   // BackBuffer RTV
    ID3D11Texture2D             *mDepthStencilBuffer;  // DS 纹理
    ID3D11DepthStencilView      *mDepthStencilView;    // DSV

public:
    virtual TResult create(/* params */) override;
    virtual TResult destroy() override;
    virtual TResult present() override;       // SwapChain::Present
    virtual TResult resize(uint32_t w, uint32_t h) override;
};
```

### D3D11Shader 系列

六种 Shader 阶段各有独立类，统一继承 `RHIShader`：

```cpp
class D3D11VertexShader   : public RHIShader { ID3D11VertexShader   *mShader; ID3DBlob *mBlob; };
class D3D11PixelShader    : public RHIShader { ID3D11PixelShader    *mShader; };
class D3D11GeometryShader : public RHIShader { ID3D11GeometryShader *mShader; };
class D3D11HullShader     : public RHIShader { ID3D11HullShader     *mShader; };
class D3D11DomainShader   : public RHIShader { ID3D11DomainShader   *mShader; };
class D3D11ComputeShader  : public RHIShader { ID3D11ComputeShader  *mShader; };
```

> 注：VertexShader 额外保存 `ID3DBlob*` 用于创建 InputLayout。

### D3D11RenderBuffer 系列

| 类 | D3D11 资源 | 用途 |
|----|-----------|------|
| D3D11VertexBuffer | ID3D11Buffer (VERTEX) | 顶点数据 |
| D3D11IndexBuffer | ID3D11Buffer (INDEX) | 索引数据 |
| D3D11ConstantBuffer | ID3D11Buffer (CONSTANT) | Shader 常量 |
| D3D11PixelBuffer1D | ID3D11Texture1D | 1D 纹理 |
| D3D11PixelBuffer2D | ID3D11Texture2D + SRV | 2D 纹理 |
| D3D11PixelBuffer3D | ID3D11Texture3D | 3D 纹理/体积 |

### D3D11RenderState 系列

| 类 | D3D11 State Object | 关键参数 |
|----|-------------------|----------|
| D3D11BlendState | ID3D11BlendState | 混合因子、独立RT混合 |
| D3D11RasterizerState | ID3D11RasterizerState | 填充模式、裁剪模式、偏移 |
| D3D11DepthStencilState | ID3D11DepthStencilState | 深度测试、模板操作 |
| D3D11SamplerState | ID3D11SamplerState | 过滤、寻址、各向异性 |

### D3D11Mapping（工具类）

提供引擎枚举到 D3D11 枚举的静态映射：

```cpp
class D3D11Mapping
{
public:
    static DXGI_FORMAT          toD3DFormat(PixelFormat fmt);
    static D3D11_BLEND          toD3DBlend(BlendFactor factor);
    static D3D11_BLEND_OP       toD3DBlendOp(BlendOperation op);
    static D3D11_COMPARISON_FUNC toD3DCompareFunc(CompareFunction func);
    static D3D11_STENCIL_OP     toD3DStencilOp(StencilOperation op);
    static D3D11_CULL_MODE      toD3DCullMode(CullMode mode);
    static D3D11_FILL_MODE      toD3DFillMode(PolygonMode mode);
    static D3D11_FILTER         toD3DFilter(FilterOptions min, FilterOptions mag, FilterOptions mip);
    static D3D11_TEXTURE_ADDRESS_MODE toD3DAddressMode(TextureAddressMode mode);
    static D3D_PRIMITIVE_TOPOLOGY toD3DPrimitiveTopology(PrimitiveType type);
    // ... 等等
};
```

---

## 三个构建目标详细对比

### D3D11Renderer vs D3D11RendererEditor

两者共用完全相同的 `Window/Include` 和 `Window/Source` 代码，差异仅在于：

| 方面 | D3D11Renderer (Runtime) | D3D11RendererEditor |
|------|------------------------|---------------------|
| 链接库 | T3DCore | T3DCoreEditor |
| 编译宏 | 无 `T3D_EDITOR` | 有 `T3D_EDITOR` |
| Plugin 入口 | T3DD3D11Plugin.cpp | T3DD3D11PluginEditor.cpp |
| Editor 功能 | 不可用 | `getEditorInfo()` 暴露 Device/Context/RTView 给 ImGui |
| 使用场景 | 独立游戏可执行文件 | 编辑器主进程中渲染 |

### D3D11RendererConsole

| 方面 | Window 版本 | Console 版本 |
|------|------------|-------------|
| GPU 设备 | 真实 ID3D11Device | 无 (空实现) |
| SwapChain | 真实 IDXGISwapChain | 无 |
| Buffer/Texture | 真实 GPU 资源 | 空壳对象 |
| Shader 编译 | 运行时 D3DCompile | 仅编译不创建 GPU Shader |
| 渲染命令 | 真实 Draw/Dispatch | 空操作 |
| 用途 | 显示画面 | 资源预处理、序列化验证 |
| 体积 | 完整代码 | 最小化桩代码 |

---

## 设计模式

### 1. Plugin Pattern（插件模式）

- DLL 导出 `dllStartPlugin` / `dllStopPlugin`
- Plugin 类实现 `install` / `startup` / `shutdown` / `uninstall` 四阶段生命周期
- 引擎通过 `Agent::loadPlugin(name)` 动态加载

### 2. Abstract Factory（抽象工厂）

- `RHIContext` 定义所有 GPU 资源创建接口
- `D3D11Context` 实现创建真实 D3D11 资源
- `D3D11ConsoleContext` 实现创建桩对象
- 上层代码仅依赖 `RHIContext` 接口

### 3. Singleton（单例模式）

- `D3D11Renderer` 继承 `Singleton<D3D11Renderer>`
- 全局唯一渲染器实例，通过 `D3D11Renderer::getInstance()` 访问

### 4. Smart Pointer（智能指针）

- 所有 RHI 对象使用引擎自定义智能指针 (`T3DSmartPtr`)
- 在 `T3DD3D11Prerequisites.h` 中为每个类声明 `typedef T3DSmartPtr<ClassName> ClassNamePtr`

### 5. 分层继承

```
Base (共享编译/反射)
 ├── Window (完整 GPU 实现)
 └── Console (桩实现)
```

### 6. 条件编译

- `#ifdef T3D_EDITOR` 控制编辑器专属功能
- 同一套源码同时服务 Runtime 和 Editor 两个目标

---

## 数据流与渲染循环

```
┌─────────────────────────────────────────────┐
│              Agent (引擎主循环)               │
│                                             │
│  每帧调用 RHIRenderer::renderAllTargets()    │
└───────────────────────┬─────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────┐
│           D3D11Context::renderAllTargets()    │
│                                             │
│  1. 遍历所有 RenderTarget                    │
│  2. bindRenderTarget (设置 RTV/DSV)          │
│  3. clearRenderTarget                        │
│  4. setViewport / setScissorRect             │
│  5. 遍历渲染队列                             │
│     ├── bindShader (VS/PS/GS/...)            │
│     ├── bindConstantBuffer                   │
│     ├── bindTexture (SRV + Sampler)          │
│     ├── setRenderState (Blend/DS/Rast)       │
│     ├── bindVertexBuffer + IndexBuffer       │
│     └── drawIndexed / drawInstanced          │
│  6. present (SwapChain::Present)             │
└─────────────────────────────────────────────┘
```

---

## 资源生命周期管理

### 创建

```
Context::createXxx(params)
  → new D3D11Xxx()
    → D3D11Device::CreateXxx(desc, &resource)
      → 返回智能指针
```

### 销毁

```
智能指针引用计数归零
  → D3D11Xxx::~D3D11Xxx()
    → resource->Release()   (COM Release)
```

### SwapChain 特殊处理

```
窗口 Resize 事件
  → D3D11RenderWindow::resize(w, h)
    → Release 旧 RTV/DSV
    → SwapChain->ResizeBuffers()
    → 重新创建 BackBuffer RTV + DepthStencil DSV
```

---

## 错误处理

`T3DD3D11Error.h` 定义 D3D11 特有错误码，与引擎错误系统集成：

```cpp
enum D3D11Error
{
    E_D3D11_CREATE_DEVICE_FAILED,
    E_D3D11_CREATE_SWAP_CHAIN_FAILED,
    E_D3D11_CREATE_RENDER_TARGET_FAILED,
    E_D3D11_COMPILE_SHADER_FAILED,
    // ...
};
```

所有 D3D11 API 调用结果通过 `HRESULT` 检查，失败时转换为引擎 `TResult` 返回。

---

## 依赖关系

```
┌──────────────────────────────────────────────────────────┐
│                    D3D11Renderer (DLL)                    │
├──────────────────────────────────────────────────────────┤
│  Window/Source + Window/Include + Base/Source + Base/Inc  │
├──────────────────────────────────────────────────────────┤
│               引擎核心库 (T3DCore / T3DCoreEditor)        │
├──────────────────────────────────────────────────────────┤
│      T3DSystem │ T3DPlatform │ T3DUtils │ T3DLog │ T3DMath│
├──────────────────────────────────────────────────────────┤
│              第三方: rttr_core │ DirectX11 SDK             │
│              d3d11.lib │ dxgi.lib │ d3dcompiler.lib       │
└──────────────────────────────────────────────────────────┘
```

---

## 扩展说明

### 如何新增渲染器后端

1. 在 `source/Plugins/Renderer/` 下创建新目录（如 `Direct3D12/`）
2. 实现 `RHIContext` 接口的所有纯虚方法
3. 实现 `RHIRenderer` 单例
4. 创建 Plugin 类，导出 `dllStartPlugin` / `dllStopPlugin`
5. 按需创建 Runtime / Editor / Console 三个变体

### 同类后端参考

引擎中已有的其他后端：
- `Vulkan/` — Vulkan 渲染后端（同样的三变体架构）
- `OpenGL3/` — OpenGL 3.x 后端
- `OpenGLES3/` — OpenGL ES 3.x 后端（移动端）
- `Metal/` — Apple Metal 后端
- `NullGL4/` — 空实现参考模板
