# OpenGL 4 渲染器后端架构

## 概述

Tiny3D 的 OpenGL 4 渲染器后端以**插件**（Plugin）形式存在，通过 DLL 动态加载到引擎中。整个后端分为三个构建目标，面向不同使用场景：

| 构建目标 | 场景 | 特点 |
|----------|------|------|
| **GL4Renderer** | 游戏运行时 | 完整 GPU 渲染，链接 T3DCore |
| **GL4RendererEditor** | 编辑器内嵌视口 | 同 Runtime 代码 + T3D_EDITOR 宏 |
| **GL4RendererConsole** | 编辑器后台/离屏处理 | 轻量桩实现 + 真实 Shader 编译（glslang） |

三者共享 `Base/` 目录中的基类代码，通过分层继承实现代码复用。

---

## 目录结构

```
source/Plugins/Renderer/OpenGL4/
├── CMakeLists.txt                          # 顶层: add_subdirectory(Window) + add_subdirectory(Console)
├── Base/                                    # 共享基类
│   ├── Include/
│   │   ├── T3DGL4ContextBase.h             # GL4ContextBase 抽象基类
│   │   ├── T3DGL4Error.h                   # GL4 错误码枚举
│   │   ├── T3DGL4Mapping.h                 # 引擎枚举 ↔ GL 枚举映射
│   │   └── T3DGL4PrerequisitesBase.h       # 基础前置声明、GLAD 头、调试宏
│   └── Source/
│       ├── T3DGL4ContextBase.cpp           # Dummy Context + glslang 编译/反射
│       └── T3DGL4Mapping.cpp               # 映射表实现
│
├── Window/                                  # 窗口渲染器（完整 GPU 渲染）
│   ├── CMakeLists.txt                      # add_subdirectory(Runtime) + add_subdirectory(Editor)
│   ├── Include/
│   │   ├── T3DGL4Context.h                # GL4Context 核心 RHI 上下文
│   │   ├── T3DGL4Prerequisites.h          # WGL 常量、前置声明、智能指针 typedef
│   │   ├── T3DGL4RenderBuffer.h           # VAO / VBO / IBO / UBO / Texture / FBO
│   │   ├── T3DGL4Renderer.h              # GL4Renderer 单例
│   │   ├── T3DGL4RenderState.h           # Blend / Rasterizer / DepthStencil / Sampler
│   │   ├── T3DGL4RenderWindow.h          # GL Context + FBO + DepthRBO
│   │   └── T3DGL4Shader.h               # VS/PS/GS/HS/DS/CS 封装
│   ├── Source/
│   │   ├── T3DGL4Context.cpp             # 上下文完整实现
│   │   ├── T3DGL4RenderBuffer.cpp        # Buffer 创建/销毁
│   │   ├── T3DGL4Renderer.cpp            # Renderer 初始化/销毁
│   │   ├── T3DGL4RenderState.cpp         # 渲染状态创建/销毁
│   │   ├── T3DGL4RenderWindow.cpp        # GL Context + MSAA 管理
│   │   └── T3DGL4Shader.cpp             # Shader Handle 释放
│   ├── Runtime/                            # → GL4Renderer.dll
│   │   ├── CMakeLists.txt
│   │   ├── Include/T3DGL4Plugin.h
│   │   └── Source/
│   │       ├── T3DGL4Plugin.cpp
│   │       └── T3DGL4PluginDLL.cpp        # DLL 导出入口
│   └── Editor/                            # → GL4RendererEditor.dll
│       ├── CMakeLists.txt
│       ├── Include/T3DGL4PluginEditor.h
│       └── Source/
│           ├── T3DGL4PluginEditor.cpp
│           └── T3DGL4PluginEditorDLL.cpp
│
└── Console/                                # → GL4RendererConsole.dll（桩 + 真实 Shader 编译）
    ├── CMakeLists.txt
    ├── Include/
    │   ├── T3DGL4ConsoleContext.h
    │   ├── T3DGL4ConsolePlugin.h
    │   ├── T3DGL4ConsolePrerequisites.h
    │   ├── T3DGL4ConsoleRenderBuffer.h
    │   ├── T3DGL4ConsoleRenderer.h
    │   ├── T3DGL4ConsoleRenderState.h
    │   ├── T3DGL4ConsoleShader.h
    │   └── T3DGL4ConsoleWindow.h
    └── Source/
        ├── T3DGL4ConsoleContext.cpp
        ├── T3DGL4ConsolePlugin.cpp
        ├── T3DGL4ConsolePluginDLL.cpp
        ├── T3DGL4ConsoleRenderBuffer.cpp
        ├── T3DGL4ConsoleRenderer.cpp
        ├── T3DGL4ConsoleRenderState.cpp
        ├── T3DGL4ConsoleShader.cpp
        └── T3DGL4ConsoleWindow.cpp
```

---

## CMake 构建体系

### 构建目标对比

| 目标 | 输出类型 | 链接库 | 关键编译宏 |
|------|---------|--------|-----------|
| GL4Renderer | SHARED (DLL) | OpenGL, GLAD, glslang, T3DMath, T3DLog, T3DUtils, T3DPlatform, T3DSystem, **T3DCore**, rttr_core | `GL4RENDERER_EXPORT` |
| GL4RendererEditor | SHARED (DLL) | 同上 + **T3DCoreEditor** 替代 T3DCore | `GL4RENDERER_EXPORT`, **`T3D_EDITOR`** |
| GL4RendererConsole | SHARED (DLL) | OpenGL, GLAD, glslang, T3DMath, T3DLog, T3DUtils, T3DPlatform, T3DSystem, **T3DCoreEditor**, rttr_core | `T3D_GL4CONSOLE_EXPORT`, **`T3D_EDITOR`** |

### 平台链接库

| 平台 | 额外链接 |
|------|---------|
| Windows | `opengl32` |
| Linux | `GL`, `X11` |

### 源文件组织

- **GL4Renderer** 和 **GL4RendererEditor** 共用 `Window/Include` + `Window/Source` + `Base/` 的全部源码，仅在 Plugin 入口和链接库上有差异。
- **GL4RendererConsole** 使用独立的 `Console/` 源码 + `Base/`，实现 Shader 编译/反射 + 最简化的桩渲染。
- 三个目标均包含 **GLAD 源文件**（`${GLAD_SOURCES}`）以提供 GL 函数指针加载。

---

## 类继承层次

### 核心继承关系

```
RHIContext (引擎接口)
  └── GL4ContextBase (Base/)             ← Dummy Context / GLAD / glslang 编译/反射
        ├── GL4Context (Window/)         ← 完整 GPU 渲染实现
        └── GL4ConsoleContext (Console/) ← 桩实现 + 真实 Shader 编译

RHIRenderer (引擎接口)
  ├── GL4Renderer (Window/)              ← 单例 (Singleton), 管理 Context 生命周期
  └── GL4RendererConsole (Console/)      ← 管理 ConsoleContext

Plugin (引擎接口)
  ├── GL4Plugin (Window/Runtime/)        ← 运行时插件
  ├── GL4Plugin (Window/Editor/)         ← 编辑器插件 (同名不同文件)
  └── GL4ConsolePlugin (Console/)        ← 控制台插件

RHIRenderWindow (引擎接口)
  ├── GL4RenderWindow (Window/)          ← 真实 GL Context + FBO
  └── GL4ConsoleWindow (Console/)        ← 桩窗口

RHIShader (引擎接口)
  └── GL4Shader                          ← 基类, 持有 GLuint GLShaderHandle
        ├── GL4VertexShader              ← GL_VERTEX_SHADER
        ├── GL4PixelShader               ← GL_FRAGMENT_SHADER
        ├── GL4GeometryShader            ← GL_GEOMETRY_SHADER
        ├── GL4HullShader                ← GL_TESS_CONTROL_SHADER
        ├── GL4DomainShader              ← GL_TESS_EVALUATION_SHADER
        └── GL4ComputeShader             ← GL_COMPUTE_SHADER
  └── GL4ConsoleShader                   ← Console 基类, 同样持有 GLShaderHandle
        ├── GL4ConsoleVertexShader
        ├── GL4ConsolePixelShader
        ├── GL4ConsoleHullShader
        ├── GL4ConsoleDomainShader
        ├── GL4ConsoleGeometryShader
        └── GL4ConsoleComputeShader

RHIVertexDeclaration (引擎接口)
  ├── GL4VertexDeclaration               ← GLuint GLVAO (Vertex Array Object)
  └── GL4ConsoleVertexDeclaration        ← 桩

RHIVertexBuffer / RHIIndexBuffer / RHIConstantBuffer (引擎接口)
  ├── GL4VertexBuffer / GL4IndexBuffer / GL4ConstantBuffer ← 真实 GLuint GLBuffer
  └── GL4Console* 对应版本               ← 桩

RHIPixelBuffer1D / RHIPixelBuffer2D / RHIPixelBuffer3D (引擎接口)
  ├── GL4PixelBuffer1D                   ← GLuint GLTexture
  ├── GL4PixelBuffer2D                   ← GLTexture + GLFBO + GLDepthRBO + MSAA Resolve
  ├── GL4PixelBuffer3D                   ← GLuint GLTexture
  └── GL4Console* 对应版本               ← 桩

RHIBlendState / RHIRasterizerState / RHIDepthStencilState / RHISamplerState (引擎接口)
  ├── GL4BlendState / GL4RasterizerState / GL4DepthStencilState ← POD 数据容器
  ├── GL4SamplerState                    ← 真实 GL Sampler Object (GLuint GLSampler)
  └── GL4Console* 对应版本               ← 桩
```

---

## 插件加载机制

### DLL 入口

每个构建目标导出两个 C 函数：

```cpp
// T3DGL4PluginDLL.cpp
extern "C" T3D_GL4RENDERER_API TResult dllStartPlugin();
extern "C" T3D_GL4RENDERER_API TResult dllStopPlugin();
```

### 加载流程

```
Agent::loadPlugin("GL4Renderer")
  → LoadLibrary / dlopen
    → dllStartPlugin()
      → new GL4Plugin()
        → Agent::installPlugin(plugin)
          → plugin->install()
            → GL4Renderer::create()          // 创建 Renderer 单例
            → Agent::addRHIRenderer(renderer) // 注册到引擎
          → plugin->startup()
```

### 卸载流程

```
Agent::unloadPlugin("GL4Renderer")
  → plugin->shutdown()
  → plugin->uninstall()
    → Agent::removeRHIRenderer(renderer)
    → mRenderer = nullptr                   // 智能指针释放 → GL4Renderer 析构
  → dllStopPlugin()
    → delete plugin
  → FreeLibrary / dlclose
```

---

## 核心类详解

### GL4ContextBase（Base 层）

**职责**: 提供与窗口/Console 无关的共享功能 —— Dummy GL Context 初始化、GLAD 加载、glslang 编译与反射。

```cpp
class GL4ContextBase : public RHIContext
{
public:
    TResult init();

    // glslang CPU 侧 Shader 编译与反射（Window 和 Console 共享）
    TResult compileShader(ShaderVariant *shader) override;
    TResult reflectShaderAllBindings(ShaderVariant *shader,
        ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams) override;
    TResult reflectSamplerBindings(ShaderVariant *shader,
        ShaderSamplerParams &samplerParams) override;

protected:
    TResult initDummyContext();       // 创建不可见窗口 + 临时 GL Context → GLAD 加载
    void destroyDummyContext();

    TResult glslangCompileAndReflect(ShaderVariant *shader);

    // glslang 反射缓存
    struct GlslangBlockInfo  { String name; int size; };
    struct GlslangUniformInfo { String name; int glDefineType; int offset;
                                 int size; int blockIndex; int arrayStride; };
    struct GlslangReflectionData {
        TArray<GlslangBlockInfo> blocks;
        TArray<GlslangUniformInfo> uniforms;
    };

    TMap<ShaderVariant*, GlslangReflectionData> mReflectionCache;
    bool mGlslangInitialized {false};

    // Dummy 窗口/上下文（平台相关）
#if defined(T3D_OS_WINDOWS)
    HWND  mDummyHWND {nullptr};
    HDC   mDummyHDC {nullptr};
    HGLRC mDummyHGLRC {nullptr};
#elif defined(T3D_OS_LINUX)
    Display   *mDummyDisplay {nullptr};
    ::Window   mDummyWindow {0};
    GLXContext mDummyGLXContext {nullptr};
#endif
    bool mGLADLoaded {false};
};
```

**Dummy Context 初始化流程**:

```
GL4ContextBase::initDummyContext()
  → 创建不可见窗口 (RegisterClass + CreateWindow / XCreateSimpleWindow)
  → 设置基础像素格式 (SetPixelFormat / glXChooseVisual)
  → 创建临时 GL 上下文 (wglCreateContext / glXCreateContext)
  → 激活上下文 (wglMakeCurrent / glXMakeCurrent)
  → gladLoadGLLoader() 加载全部 GL 函数指针
  → mGLADLoaded = true
  → 获取 WGL 扩展函数指针 (wglGetProcAddress):
      - wglChoosePixelFormatARB   → MSAA 像素格式选择
      - wglCreateContextAttribsARB → Core Profile 上下文创建
      - wglGetExtensionsStringARB
```

### GL4Context（Window 层 - 完整实现）

**职责**: 持有 GL 上下文状态，实现全部 RHI 接口。

**关键成员**:
```cpp
class GL4Context : public GL4ContextBase
{
protected:
    GLuint  mCurrentProgram {0};          // 当前激活的 GL Program
    GLenum  mPrimitiveType {GL_TRIANGLES};// 当前图元类型
    GLenum  mIndexType {GL_UNSIGNED_SHORT};
    uint32_t mIndexSize {2};
    RenderTargetPtr mCurrentRenderTarget; // 当前渲染目标
    GLuint  mCurrentVAO {0};             // 当前绑定的 VAO
    VertexDeclaration *mPendingVertexDecl;// 延迟配置顶点属性
    TMap<String, GLuint> mPendingUBOs;   // 延迟 UBO 绑定 (cbuffer 名 → GL handle)
    bool    mProgramDirty {false};       // Program 是否需要重新 link
    bool    mRenderingToFBO {false};     // 是否渲染到 FBO (Y 翻转判断)
    ShaderVariant *mCurrentVSVariant;    // 当前 VS
    ShaderVariant *mCurrentPSVariant;    // 当前 PS

    // WGL 扩展函数指针
#if defined(T3D_OS_WINDOWS)
    PFNWGLCHOOSEPIXELFORMATARBPROC   mWglChoosePixelFormatARB;
    PFNWGLGETEXTENSIONSSTRINGARBPROC mWglGetExtensionsStringARB;
    PFNWGLCREATECONTEXTATTRIBSARBPROC mWglCreateContextAttribsARB;
#endif
};
```

**核心方法分组**:

| 方法组 | 职责 |
|--------|------|
| `init()` | 初始化 Dummy Context → GLAD 加载 → glslang 初始化 |
| `createRenderWindow()` | 创建 GL4RenderWindow (平台 GL Context + MSAA) |
| `createRenderTexture()` | 创建 FBO 渲染纹理 (含 MSAA Resolve) |
| `createVertexBuffer() / createIndexBuffer() / createConstantBuffer()` | GPU Buffer 工厂 (glGenBuffers) |
| `createPixelBuffer1D/2D/3D()` | 纹理资源工厂 (glGenTextures) |
| `createVertexDeclaration()` | VAO 工厂 (glGenVertexArrays) |
| `createBlendState() / createRasterizerState() / ...` | 渲染状态工厂 |
| `createVertexShader() / createPixelShader() / ...` | Shader 工厂 (glCreateShader + glCompileShader) |
| `setRenderTarget() / resetRenderTarget()` | FBO 绑定 (glBindFramebuffer) |
| `setViewport()` | 视口设置 (glViewport) |
| `clearColor() / clearDepth() / clearDepthStencil()` | 清除操作 (glClear) |
| `setBlendState() / setDepthStencilState() / setRasterizerState()` | 渲染状态应用 |
| `setVertexShader() / setPixelShader()` | Shader 挂载 → 标记 mProgramDirty |
| `setVSConstantBuffers() / setPSConstantBuffers()` | 延迟 UBO 暂存 (stageConstantBuffers) |
| `setPSPixelBuffers() / setPSSamplers()` | 纹理和采样器绑定 |
| `render()` | 延迟 Program Link + 绘制 (glDrawElementsBaseVertex / glDrawArrays) |
| `blit()` | 帧缓冲拷贝 (glBlitFramebuffer) |
| `writeBuffer()` | Buffer 数据写入 (DSA: glNamedBufferData / glNamedBufferSubData) |
| `swapBackBuffer()` | 交换前后缓冲 (SwapBuffers / glXSwapBuffers) |

### GL4Renderer（单例）

**职责**: 管理 Context 的创建与销毁，作为插件和引擎之间的桥梁。

```cpp
class GL4Renderer
    : public RHIRenderer
    , public Singleton<GL4Renderer>
{
public:
    static GL4RendererPtr create();

    TResult init() override;
    TResult destroy() override;
    void getEditorInfo(void *info, RenderWindow *window) override;

protected:
    void cleanup();
};

#define GL4_RENDERER    (GL4Renderer::getInstance())
#define GL4_CONTEXT     (smart_pointer_cast<GL4Context>(GL4_RENDERER.getContext()))
```

### GL4RenderWindow

**职责**: 封装平台原生 GL 上下文 + 默认 FBO + 深度 RBO。

```cpp
class GL4RenderWindow : public RHIRenderWindow
{
public:
    // 平台 GL 上下文
#if defined(T3D_OS_WINDOWS)
    HGLRC   GLContext {nullptr};       // OpenGL 渲染上下文
    HDC     GLDeviceContext {nullptr};  // 设备上下文
#elif defined(T3D_OS_LINUX)
    GLXContext  GLContext {nullptr};
    Display    *GLDisplay {nullptr};
    ::Window    GLWindow {0};
#endif
    GLuint  GLDefaultFBO {0};          // 默认帧缓冲对象
    GLuint  GLDepthRBO {0};            // 深度渲染缓冲对象
    uint32_t mWidth {0};
    uint32_t mHeight {0};
    uint32_t mMSAACount {1};           // 实际获得的 MSAA 采样数

    bool init(RenderWindow *renderWindow);
    TResult swapBuffers() override;    // SwapBuffers / glXSwapBuffers
    TResult resize(uint32_t w, uint32_t h) override;
};
```

**GL Context 创建流程** (Windows):

```
GL4RenderWindow::init()
  → 获取 GL4Context 的 WGL 扩展函数指针
  → wglChoosePixelFormatARB (MSAA 像素格式)
  → SetPixelFormat
  → wglCreateContextAttribsARB (Core Profile 4.5, Debug bit)
  → wglShareLists (与 Dummy Context 共享纹理/Buffer)
  → wglMakeCurrent
  → glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE)  ← 深度范围统一
  → glEnable(GL_MULTISAMPLE)  ← 若 MSAA > 1
```

### GL4Shader 系列

六种 Shader 阶段各有独立类，统一继承 `GL4Shader` 基类：

```cpp
class GL4Shader : public RHIShader
{
public:
    GLuint GLShaderHandle {0};     // glCreateShader 返回的句柄
};

class GL4VertexShader   : public GL4Shader { /* glDeleteShader on ~dtor */ };
class GL4PixelShader    : public GL4Shader { /* ... */ };
class GL4GeometryShader : public GL4Shader { /* ... */ };
class GL4HullShader     : public GL4Shader { /* ... */ };
class GL4DomainShader   : public GL4Shader { /* ... */ };
class GL4ComputeShader  : public GL4Shader { /* ... */ };
```

> 注：与 D3D11 不同，GL4 的 VertexShader 无需额外保存 Blob 用于 InputLayout —— 顶点属性通过 VAO + `glVertexAttribPointer` 配置。

### GL4RenderBuffer 系列

| 类 | GL 资源 | 用途 |
|----|---------|------|
| GL4VertexDeclaration | GLuint GLVAO | 顶点数组对象 (VAO) |
| GL4VertexBuffer | GLuint GLBuffer (ARRAY_BUFFER) | 顶点数据 |
| GL4IndexBuffer | GLuint GLBuffer (ELEMENT_ARRAY_BUFFER) | 索引数据 |
| GL4ConstantBuffer | GLuint GLBuffer (UNIFORM_BUFFER) | Shader 常量 (UBO) |
| GL4PixelBuffer1D | GLuint GLTexture | 1D 纹理 |
| GL4PixelBuffer2D | GLTexture + GLFBO + GLDepthRBO + GLResolveTex + GLResolveFBO | 2D 纹理 / Render Texture (含 MSAA Resolve) |
| GL4PixelBuffer3D | GLuint GLTexture | 3D 纹理/体积 |

### GL4RenderState 系列

| 类 | GL 资源类型 | 关键参数 |
|----|------------|----------|
| GL4BlendState | POD (GL4BlendStateData) | 混合因子、混合操作、颜色掩码 |
| GL4RasterizerState | POD (GL4RasterizerStateData) | 填充模式、裁剪模式、深度偏移、MSAA |
| GL4DepthStencilState | POD (GL4DepthStencilStateData) | 深度测试、模板操作、前/后面 |
| GL4SamplerState | GLuint GLSampler (glGenSamplers) | 过滤、寻址、各向异性 |

> 注：GL4 的 Blend/DepthStencil/Rasterizer 状态为 **POD 数据容器**，在 `setXxxState()` 时通过 `glEnable/glDisable` + 对应 GL 状态调用应用。只有 SamplerState 使用真正的 GL Sampler Object。

### GL4Mapping（工具类）

提供引擎枚举到 GL 枚举的静态映射：

```cpp
class GL4Mapping
{
public:
    static GLenum get(PixelFormat format);
    static GLenum getInternalFormat(PixelFormat format);
    static GLenum getPixelType(PixelFormat format);
    static GLenum get(BlendFactor factor);
    static GLenum get(BlendOperation op);
    static GLenum get(CompareFunction func);
    static GLenum get(StencilOp op);
    static GLenum get(PolygonMode mode);
    static GLenum get(CullingMode mode);
    static GLenum get(TextureAddressMode mode);
    static GLenum getMinFilter(FilterOptions minFilter, FilterOptions mipFilter);
    static GLenum getMagFilter(FilterOptions magFilter);
    static GLenum get(PrimitiveType primitive);
    static GLenum get(IndexType idxType);
    static GLenum getBufferUsage(Usage usage);
    static GLint getVertexAttribSize(VertexAttribute::Type type);
    static GLenum getVertexAttribType(VertexAttribute::Type type);
    static GLboolean getVertexAttribNormalized(VertexAttribute::Type type);
    static bool isIntegerAttrib(VertexAttribute::Type type);
};
```

---

## 三个构建目标详细对比

### GL4Renderer vs GL4RendererEditor

两者共用完全相同的 `Window/Include` 和 `Window/Source` 代码，差异仅在于：

| 方面 | GL4Renderer (Runtime) | GL4RendererEditor |
|------|----------------------|-------------------|
| 链接库 | T3DCore | T3DCoreEditor |
| 编译宏 | 无 `T3D_EDITOR` | 有 `T3D_EDITOR` |
| Plugin 入口 | T3DGL4Plugin.cpp / T3DGL4PluginDLL.cpp | T3DGL4PluginEditor.cpp / T3DGL4PluginEditorDLL.cpp |
| Editor 功能 | 不可用 | `getEditorInfo()` 暴露 GL Context 信息 |
| 使用场景 | 独立游戏可执行文件 | 编辑器主进程中渲染 |

### GL4RendererConsole

| 方面 | Window 版本 | Console 版本 |
|------|------------|-------------|
| GL 上下文 | 真实 Core Profile 4.5 Context | Dummy Context (仅用于 GLAD + glslang) |
| SwapChain | 真实 SwapBuffers | 无 |
| Buffer/Texture | 真实 GPU 资源 (GLuint handles) | 空壳对象 |
| Shader 编译 | 运行时 glslang + glCreateShader | 仅 glslang CPU 侧编译/反射 |
| 渲染命令 | 真实 Draw/Blit | 空操作 (返回 T3D_OK) |
| 用途 | 显示画面 | 资源烘焙、Shader 预编译/反射验证 |
| 体积 | 完整代码 | 最小化桩代码 |

---

## 设计模式

### 1. Plugin Pattern（插件模式）

- DLL 导出 `dllStartPlugin` / `dllStopPlugin`
- Plugin 类实现 `install` / `startup` / `shutdown` / `uninstall` 四阶段生命周期
- 引擎通过 `Agent::loadPlugin(name)` 动态加载

### 2. Abstract Factory（抽象工厂）

- `RHIContext` 定义所有 GPU 资源创建接口
- `GL4Context` 实现创建真实 GL 资源
- `GL4ConsoleContext` 实现创建桩对象
- 上层代码仅依赖 `RHIContext` 接口

### 3. Singleton（单例模式）

- `GL4Renderer` 继承 `Singleton<GL4Renderer>`
- 全局唯一渲染器实例，通过 `GL4_RENDERER` 宏访问

### 4. Smart Pointer（智能指针）

- 所有 RHI 对象使用引擎自定义智能指针 (`T3DSmartPtr`)
- 在 `T3DGL4Prerequisites.h` 中为每个类声明 `T3D_DECLARE_SMART_PTR(ClassName)`

### 5. 分层继承

```
Base (共享 Dummy Context / GLAD / glslang)
 ├── Window (完整 GPU 实现)
 └── Console (桩 + Shader 编译)
```

### 6. 条件编译

- `#ifdef T3D_EDITOR` 控制编辑器专属功能
- `#if defined(T3D_OS_WINDOWS)` / `#elif defined(T3D_OS_LINUX)` 控制平台相关代码 (WGL vs GLX)
- `#if defined(T3D_DEBUG)` 控制 `GL_CHECK_ERROR` 调试宏

### 7. Dummy Context 模式（GL4 特有）

在真正的渲染窗口创建之前，先建立一个不可见的 Dummy Window + GL Context 用于：

- 加载 GLAD（GL 函数指针绑定）
- 获取 WGL/GLX 扩展函数（`wglChoosePixelFormatARB`、`wglCreateContextAttribsARB`）
- 为后续 Core Profile 4.5 上下文创建做准备

### 8. 延迟 Program 链接（GL4 特有）

- `setVertexShader()` / `setPixelShader()` 仅 attach Shader 到 Program，标记 `mProgramDirty = true`
- 真正的 `glLinkProgram` + `glUseProgram` 推迟到 `render()` 时执行
- 确保 VS+PS 都 attach 完毕后才 link，避免冗余 link 操作

---

## 数据流与渲染循环

```
┌─────────────────────────────────────────────┐
│              Agent (引擎主循环)               │
│                                             │
│  每帧调用渲染管线                             │
└───────────────────────┬─────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────┐
│             GL4Context 渲染流程               │
│                                             │
│  1. setRenderTarget                          │
│     → glBindFramebuffer(GL_FRAMEBUFFER, fbo) │
│     → 更新 mRenderingToFBO 标记              │
│                                             │
│  2. clearColor / clearDepth                  │
│     → glClearColor + glClear                 │
│                                             │
│  3. setViewport                              │
│     → glViewport                             │
│                                             │
│  4. 遍历渲染队列                             │
│     ├── setBlendState                        │
│     │   → glEnable(GL_BLEND) + glBlendFunc   │
│     ├── setDepthStencilState                 │
│     │   → glEnable(GL_DEPTH_TEST) + ...      │
│     ├── setRasterizerState                   │
│     │   → glPolygonMode + glCullFace         │
│     │   → (Y 翻转时交换 Front/Back)          │
│     ├── setVertexDeclaration                 │
│     │   → glBindVertexArray(VAO)             │
│     ├── setVertexBuffers + setIndexBuffer    │
│     │   → glBindBuffer + glVertexAttribPointer│
│     ├── setVertexShader (attach, dirty=true) │
│     ├── setPixelShader (attach, dirty=true)  │
│     ├── setVSConstantBuffers                 │
│     │   → stageConstantBuffers (→mPendingUBOs)│
│     ├── setPSPixelBuffers                    │
│     │   → glActiveTexture + glBindTexture    │
│     ├── setPSSamplers                        │
│     │   → glBindSampler                      │
│     └── render(indexCount, startIndex, base) │
│         ├── if mProgramDirty:                │
│         │   ├── glLinkProgram                │
│         │   ├── glUseProgram                 │
│         │   ├── bindPendingUniformBlocks     │
│         │   │   → glUniformBlockBinding      │
│         │   │   → glBindBufferBase(UBO)      │
│         │   └── setupSamplerBindings         │
│         │       → glUniform1i (texture unit) │
│         └── glDrawElementsBaseVertex         │
│             或 glDrawArrays                  │
│                                             │
│  5. swapBackBuffer                           │
│     → SwapBuffers / glXSwapBuffers           │
└─────────────────────────────────────────────┘
```

---

## 资源生命周期管理

### 创建

```
Context::createXxx(params)
  → XxxPtr::create()        // 智能指针工厂
    → glGenXxx(&handle)     // GL 对象创建
    → glBindXxx(handle)     // 绑定 + 初始化
    → 返回智能指针
```

### 销毁 (RAII)

```
智能指针引用计数归零
  → GL4Xxx::~GL4Xxx()
    → GL_SAFE_DELETE_XXX(handle)   // 安全释放宏
```

### GL 安全释放宏

```cpp
#define GL_SAFE_DELETE_BUFFER(p)  if ((p) != 0) { glDeleteBuffers(1, &(p)); (p) = 0; }
#define GL_SAFE_DELETE_TEXTURE(p) if ((p) != 0) { glDeleteTextures(1, &(p)); (p) = 0; }
#define GL_SAFE_DELETE_VAO(p)     if ((p) != 0) { glDeleteVertexArrays(1, &(p)); (p) = 0; }
#define GL_SAFE_DELETE_SAMPLER(p) if ((p) != 0) { glDeleteSamplers(1, &(p)); (p) = 0; }
#define GL_SAFE_DELETE_FBO(p)     if ((p) != 0) { glDeleteFramebuffers(1, &(p)); (p) = 0; }
#define GL_SAFE_DELETE_RBO(p)     if ((p) != 0) { glDeleteRenderbuffers(1, &(p)); (p) = 0; }
#define GL_SAFE_DELETE_SHADER(p)  if ((p) != 0) { glDeleteShader(p); (p) = 0; }
#define GL_SAFE_DELETE_PROGRAM(p) if ((p) != 0) { glDeleteProgram(p); (p) = 0; }
```

### RenderWindow 特殊处理

```
窗口 Resize 事件
  → GL4RenderWindow::resize(w, h)
    → 更新 mWidth / mHeight
    → 重新创建深度 RBO (如需要)
    → glViewport(0, 0, w, h)
```

### Render Texture MSAA Resolve

```
渲染到 GL4PixelBuffer2D (MSAA)
  → 绑定多采样 FBO (GLFBO) 渲染
  → blit 时：
    → glBindFramebuffer(GL_READ_FRAMEBUFFER, GLFBO)
    → glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GLResolveFBO)
    → glBlitFramebuffer(..., GL_COLOR_BUFFER_BIT, GL_NEAREST)
    → Shader 读取 GLResolveTex (非多采样纹理)
```

---

## 错误处理

`T3DGL4Error.h` 定义 GL4 特有错误码，与引擎错误系统集成：

```cpp
enum GL4ErrorCode : int32_t
{
    T3D_ERR_GL4_WINDOW_ALREADY,           // 窗口已经创建
    T3D_ERR_GL4_INVALID_TARGET,           // 错误的渲染目标
    T3D_ERR_GL4_CREATE_CONTEXT,           // 创建 GL 上下文失败
    T3D_ERR_GL4_SET_PIXEL_FORMAT,         // 设置像素格式失败
    T3D_ERR_GL4_GLAD_LOAD,               // GLAD 加载失败
    T3D_ERR_GL4_CREATE_SHADER,            // 创建 Shader 失败
    T3D_ERR_GL4_COMPILE_SHADER,           // Shader 编译失败
    T3D_ERR_GL4_LINK_PROGRAM,             // Shader 链接失败
    T3D_ERR_GL4_SHADER_REFLECTION,        // Shader 反射失败
    T3D_ERR_GL4_CREATE_BUFFER,            // 创建缓冲区失败
    T3D_ERR_GL4_CREATE_TEXTURE,           // 创建纹理失败
    T3D_ERR_GL4_CREATE_FBO,               // 创建 FBO 失败
    T3D_ERR_GL4_CREATE_VAO,               // 创建 VAO 失败
    T3D_ERR_GL4_CREATE_SAMPLER,           // 创建 Sampler 失败
    T3D_ERR_GL4_CREATE_BLEND_STATE,       // 创建 BlendState 失败
    T3D_ERR_GL4_CREATE_DEPTH_STENCIL_STATE,
    T3D_ERR_GL4_CREATE_RASTERIZER_STATE,
    T3D_ERR_GL4_MAP_BUFFER,               // 映射缓冲区失败
    T3D_ERR_GL4_SWAP_BUFFERS,             // SwapBuffers 失败
    T3D_ERR_GL4_NOT_SUPPORTED,            // 不支持的操作
    T3D_ERR_GL4_RESIZE_BUFFERS,           // 调整窗口大小失败
    T3D_ERR_GL4_PRESENT,                  // Present 失败
    T3D_ERR_GL4_INVALID_USAGE,            // 无效的用途
};
```

### GL 运行时错误检查

Debug 模式下通过 `GL_CHECK_ERROR` 宏在关键 GL 操作后检查错误：

```cpp
#if defined(T3D_DEBUG)
    #define GL_CHECK_ERROR(tag, msg) \
        { \
            GLenum _glErr; \
            int _glErrCount = 0; \
            while ((_glErr = glGetError()) != GL_NO_ERROR && _glErrCount < 16) \
            { \
                T3D_LOG_ERROR(tag, "%s : GL Error 0x%04X", msg, _glErr); \
                ++_glErrCount; \
            } \
        }
#else
    #define GL_CHECK_ERROR(tag, msg) (void)0
#endif
```

---

## 依赖关系

```
┌──────────────────────────────────────────────────────────┐
│                    GL4Renderer (DLL)                       │
├──────────────────────────────────────────────────────────┤
│  Window/Source + Window/Include + Base/Source + Base/Inc   │
├──────────────────────────────────────────────────────────┤
│               引擎核心库 (T3DCore / T3DCoreEditor)        │
├──────────────────────────────────────────────────────────┤
│      T3DSystem │ T3DPlatform │ T3DUtils │ T3DLog │ T3DMath│
├──────────────────────────────────────────────────────────┤
│              第三方: rttr_core │ GLAD │ glslang            │
│              OpenGL (opengl32.lib / libGL.so)              │
│              平台: X11 (Linux)                             │
└──────────────────────────────────────────────────────────┘
```

---

## 与 D3D11 后端的关键差异

| 方面 | D3D11 后端 | GL4 后端 |
|------|-----------|----------|
| **API 加载** | 直接链接 d3d11.lib / dxgi.lib | GLAD 动态加载 GL 函数指针 |
| **Shader 编译** | d3dcompiler (HLSL → bytecode) | glslang (GLSL → CPU 反射) + glCreateShader (GPU 编译) |
| **初始化** | 直接 D3D11CreateDevice | Dummy Context → GLAD → Core Profile 4.5 升级 |
| **顶点布局** | ID3D11InputLayout (需 VS bytecode) | VAO (glGenVertexArrays + glVertexAttribPointer) |
| **常量缓冲** | ID3D11Buffer (CONSTANT) + SetConstantBuffers | GLuint UBO + 延迟 glUniformBlockBinding |
| **Shader 状态** | 独立 VS/PS 对象，直接绑定 | attach 到 Program → 延迟 Link |
| **渲染状态** | State Object (ID3D11BlendState 等) | POD 数据容器 + 即时 glEnable/glDisable |
| **采样器** | ID3D11SamplerState | GLuint Sampler Object (glGenSamplers) |
| **深度范围** | 原生 [0,1] | glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE) 统一为 [0,1] |
| **投影矩阵** | 原生 Y 朝上 | FBO 渲染时自动 Y 翻转 + 交换 Cull Face |
| **纹理资源** | ID3D11Texture2D + SRV | GLuint GLTexture，RenderTexture 额外持有 FBO + DepthRBO + MSAA Resolve |
| **资源释放** | COM Release (resource->Release()) | GL_SAFE_DELETE_* 宏 (glDeleteXxx) |
| **多平台** | 仅 Windows | Windows (WGL) + Linux (GLX) |
| **调试** | HRESULT 检查 | GL_CHECK_ERROR (glGetError 循环) |

---

## 扩展说明

### 如何新增渲染器后端

1. 在 `source/Plugins/Renderer/` 下创建新目录（如 `Vulkan/`）
2. 实现 `RHIContext` 接口的所有纯虚方法
3. 实现 `RHIRenderer` 单例
4. 创建 Plugin 类，导出 `dllStartPlugin` / `dllStopPlugin`
5. 按需创建 Runtime / Editor / Console 三个变体

### 同类后端参考

引擎中已有的其他后端：
- `Direct3D11/` — Direct3D 11 渲染后端（同样的三变体架构）
- `Vulkan/` — Vulkan 渲染后端
- `OpenGL3/` — OpenGL 3.x 后端
- `OpenGLES3/` — OpenGL ES 3.x 后端（移动端）
- `Metal/` — Apple Metal 后端
- `NullGL4/` — 空实现参考模板
