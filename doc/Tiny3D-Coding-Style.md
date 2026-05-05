# Tiny3D C++ 编码风格规范

本文档基于 Tiny3D Core 模块的实际源码总结而成，作为项目统一的编码规范，供所有开发人员遵循。

---

## 目录

1. [文件组织](#1-文件组织)
2. [命名规范](#2-命名规范)
3. [代码格式](#3-代码格式)
4. [Include 规范](#4-include-规范)
5. [注释规范](#5-注释规范)
6. [项目特有约定](#6-项目特有约定)

---

## 1. 文件组织

### 1.1 文件命名

- 所有源文件使用 `T3D` 前缀 + PascalCase 命名
- 头文件扩展名：`.h`
- 源文件扩展名：`.cpp`

```
T3DCamera.h
T3DCamera.cpp
T3DRHIRenderer.h
T3DRHIRenderer.cpp
T3DTransformNode.h
```

### 1.2 文件头版权声明

每个 `.h` 和 `.cpp` 文件开头**必须**包含 MIT License 版权声明，格式如下：

```cpp
/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/
```

- 上下边框使用 `*` 填充，总宽度 80 个字符
- 边框行格式：`/****...****/`（开头）和 ` ***...****/`（结尾）

### 1.3 Include Guard

使用传统 `#ifndef` / `#define` / `#endif` 风格，**不使用** `#pragma once`。

```cpp
#ifndef __T3D_CAMERA_H__
#define __T3D_CAMERA_H__

// 文件内容...

#endif    /*__T3D_CAMERA_H__*/
```

**规则：**
- 格式：`__T3D_<模块名大写>_H__`（双下划线前后缀）
- `#endif` 后紧跟 `/*__T3D_XXX_H__*/` 注释

---

## 2. 命名规范

### 2.1 命名空间

| 项目 | 规则 |
|------|------|
| 命名风格 | PascalCase |
| 项目根命名空间 | `Tiny3D` |
| 子命名空间 | PascalCase，如 `ShaderLab` |

```cpp
namespace Tiny3D
{
    // 内容缩进 4 空格
}
```

### 2.2 类与结构体

| 项目 | 规则 | 示例 |
|------|------|------|
| 类名 | PascalCase，无前缀 | `Camera`, `Agent`, `TransformNode` |
| RHI 层类 | `RHI` 前缀 + PascalCase | `RHIRenderer`, `RHIContext`, `RHIBuffer` |
| 结构体 | PascalCase | `Transform`, `Viewport`, `BlendDesc` |
| 智能指针类型 | 类名 + `Ptr` 后缀 | `CameraPtr`, `RenderWindowPtr` |
| 导出标记 | 类声明前加 `T3D_ENGINE_API` | — |

```cpp
class T3D_ENGINE_API Camera : public Component
{
    // ...
};
```

### 2.3 成员变量

- **前缀**：小写 `m` + PascalCase 变量名
- **初始化**：使用 C++11 就地初始化 `{}`
- **对齐**：类型名和变量名通过空格列对齐

```cpp
// 正确 ✓
RenderTargetPtr     mSrcRenderTarget {nullptr};
uint32_t            mOrder {0};
bool                mIsRunning {false};
String              mAppPath {};
Projection          mProjectionType {Projection::kPerspective};

// 错误 ✗
RenderTargetPtr m_srcRenderTarget;     // 不要使用 m_ 前缀
uint32_t mOrder = 0;                   // 不要使用 = 初始化
bool bIsRunning {false};               // 不要使用匈牙利前缀
```

### 2.4 方法命名

- 使用 **camelCase**（小驼峰），首字母小写

| 类型 | 规则 | 示例 |
|------|------|------|
| Getter | `getXxx()` / `isXxx()` | `getOrder()`, `isRunning()` |
| Setter | `setXxx()` | `setFovY()`, `setOrder()` |
| 动作方法 | 动词开头 | `lookAt()`, `addChild()`, `renderOneFrame()` |
| 生命周期回调 | `onXxx()` | `onStart()`, `onDestroy()`, `onCreate()` |

```cpp
// 正确 ✓
uint32_t getOrder() const { return mOrder; }
void setOrder(uint32_t order) { mOrder = order; }
TResult lookAt(const Vector3 &eye, const Vector3 &obj, const Vector3 &up);

// 错误 ✗
uint32_t GetOrder() const;     // 不要使用 PascalCase
void set_order(uint32_t o);    // 不要使用 snake_case
```

### 2.5 枚举

- 使用 `enum class`（强类型枚举）
- 枚举类型名：PascalCase
- 枚举值：`k` 前缀 + PascalCase

```cpp
enum class Projection : uint32_t
{
    kOrthographic = 0,
    kPerspective
};

enum class BlendFactor : uint32_t
{
    kOne = 0,
    kZero,
    kDstColor,
    kSrcColor,
    kOneMinusDstColor,
};
```

位掩码枚举（需要位运算时可使用非 class 枚举）：

```cpp
enum WriteMask : uint32_t
{
    kWriteMaskNone  = 0x00000000,
    kWriteMaskRed   = 0x00000001,
    kWriteMaskGreen = 0x00000002,
    kWriteMaskBlue  = 0x00000004,
    kWriteMaskAll   = 0x0000000F,
};
```

### 2.6 常量

| 类型 | 规则 | 示例 |
|------|------|------|
| 命名空间级常量 | `k` 前缀 + PascalCase | `kBuiltinTagQueue`, `kMaxLightCount` |
| 宏定义常量 | 全大写 + 下划线 + `T3D_` 前缀 | `T3D_MAX_RENDER_TARGET`, `T3D_OK` |

```cpp
const char *const kBuiltinTagQueue = "Queue";
const uint32_t kMaxBoneCount = 128;

#define T3D_MAX_RENDER_TARGET   8
#define T3D_VERSION_STR         "0.0.0.1"
```

### 2.7 宏

- **全大写 + 下划线分隔**
- 项目宏使用 `T3D_` 前缀

| 类别 | 示例 |
|------|------|
| API 导出 | `T3D_ENGINE_API` |
| 智能指针声明 | `T3D_DECLARE_SMART_PTR(ClassName)` |
| 对象创建 | `T3D_NEW` |
| 返回值判断 | `T3D_FAILED(ret)`, `T3D_OK` |
| 单例访问 | `T3D_AGENT`, `T3D_SCENE_MGR`, `T3D_TEXTURE_MGR` |
| 日志标签 | `LOG_TAG_ENGINE`, `LOG_TAG_RENDER` |
| Include Guard | `__T3D_CAMERA_H__` |
| RTTI | `TCLASS()`, `TENUM()`, `TPROPERTY(...)`, `TRTTI_ENABLE(...)` |

### 2.8 函数参数

- 使用 camelCase，无前缀
- 常量引用：`const Type &name`
- 指针：`Type *name`

```cpp
TResult lookAt(const Vector3 &eye, const Vector3 &obj, const Vector3 &up);
TResult installPlugin(Plugin *plugin);
void setViewport(const Viewport &viewport);
```

---

## 3. 代码格式

### 3.1 缩进

- 使用 **4 个空格**，**禁止使用 Tab**
- 命名空间内部内容也缩进 4 空格

```cpp
namespace Tiny3D
{
    class Camera : public Component
    {
    public:
        void onStart() override;

    private:
        float mFovY {60.0f};
    };
}
```

### 3.2 大括号风格

使用 **Allman 风格**（大括号独占一行）：

```cpp
// 类定义
class Camera : public Component
{
public:
    TResult init();
};

// 函数定义
TResult Camera::init()
{
    // ...
    return T3D_OK;
}

// 控制结构
if (mSrcRenderTarget != nullptr)
{
    mSrcRenderTarget->releaseAllResources();
    mSrcRenderTarget = nullptr;
}

for (auto &item : mRenderWindows)
{
    item.second->destroy();
}

switch (mProjectionType)
{
case Projection::kPerspective:
    {
        // ...
    }
    break;
case Projection::kOrthographic:
    {
        // ...
    }
    break;
}
```

**例外**：简短的单行内联函数可以写在一行：

```cpp
uint32_t getOrder() const { return mOrder; }
bool isRunning() const { return mIsRunning; }
```

### 3.3 访问说明符

- `public:`、`protected:`、`private:` 与类的大括号对齐（不额外缩进）
- 访问说明符前后保留空行

```cpp
    class Camera : public Component
    {
    public:
        Camera();
        ~Camera() override;

        uint32_t getOrder() const { return mOrder; }

    protected:
        virtual TResult onInit();

    private:
        uint32_t mOrder {0};
    };
```

### 3.4 指针与引用对齐

`*` 和 `&` **靠近变量名/参数名**（右对齐）：

```cpp
// 正确 ✓
const char *getVersionString() const;
Plugin *plugin;
const Vector3 &eye;
RenderWindow *window = nullptr;

// 错误 ✗
const char* getVersionString() const;   // * 不要靠近类型
const Vector3& eye;                      // & 不要靠近类型
```

### 3.5 空行规则

- 函数之间保留一个空行
- 访问说明符前后各一个空行
- 逻辑段落之间使用空行分隔
- 文件末尾保留一个空行

### 3.6 行宽

- 建议每行不超过 **100** 个字符
- 超长行在合适位置折行，折行部分增加一级缩进（4 空格）

---

## 4. Include 规范

### 4.1 Include Guard（参见 1.3）

### 4.2 引用符号

| 类型 | 符号 | 示例 |
|------|------|------|
| 项目内头文件 | 双引号 `""` | `#include "T3DCamera.h"` |
| 第三方/系统库 | 尖括号 `<>` | `#include <algorithm>` |

### 4.3 Include 顺序

**头文件（.h）中：**

1. 项目前置头文件（`T3DPrerequisites.h`、`T3DConfig.h`）
2. 同模块相关头文件
3. 其他模块头文件
4. 第三方库头文件（如 `<rttr/registration>`）
5. 标准库头文件（如 `<algorithm>`）

**源文件（.cpp）中：**

1. `T3DConfig.h`（通常第一个）
2. 对应的头文件
3. 项目其他模块头文件
4. 第三方库头文件
5. 标准库头文件

- 各组之间使用**空行**分隔

```cpp
// T3DCamera.cpp
#include "T3DConfig.h"
#include "T3DCamera.h"

#include "T3DTransformNode.h"
#include "T3DSceneManager.h"

#include <rttr/registration>

#include <algorithm>
```

---

## 5. 注释规范

### 5.1 行注释

使用 `//` 进行代码内简短注释：

```cpp
uint32_t mOrder {0};    // 渲染顺序
```

### 5.2 块注释

仅用于文件头版权声明（参见 1.2）。

### 5.3 Doxygen 文档注释

使用 `/** */` 格式，支持 `\brief` 或 `@brief` 风格：

```cpp
/**
 * \brief 获取版本号
 * \return 返回整数型版本号
 */
uint32_t getVersion() const;

/**
 * @fn      TResult Agent::addRenderer(RHIRendererPtr renderer)
 * @brief   添加渲染器
 * @param [in]  renderer : 要添加的渲染器对象.
 * @return  成功返回 T3D_OK.
 */
TResult addRenderer(RHIRendererPtr renderer);
```

成员变量简短说明使用三斜杠 `///`：

```cpp
/// 渲染管线
RenderPipelinePtr mRenderPipeline {nullptr};

/// 是否正在运行
bool mIsRunning {false};
```

### 5.4 函数分隔符

在 `.cpp` 文件中，函数之间使用横线注释分隔：

```cpp
//--------------------------------------------------------------------------

TResult Camera::init()
{
    // ...
}

//--------------------------------------------------------------------------

void Camera::onDestroy()
{
    // ...
}
```

---

## 6. 项目特有约定

### 6.1 智能指针

使用 `T3D_DECLARE_SMART_PTR` 宏为类声明对应的智能指针类型：

```cpp
T3D_DECLARE_SMART_PTR(Camera);       // 生成 CameraPtr 类型
T3D_DECLARE_SMART_PTR(RenderWindow); // 生成 RenderWindowPtr 类型
```

### 6.2 对象创建

使用 `T3D_NEW` 宏代替直接 `new`：

```cpp
// 正确 ✓
CameraPtr camera = T3D_NEW Camera();

// 错误 ✗
Camera *camera = new Camera();
```

### 6.3 错误处理模式

使用 `do { ... } while (false)` + `break` 的错误退出模式：

```cpp
TResult Camera::init()
{
    TResult ret = T3D_OK;

    do
    {
        ret = loadConfig(mConfigPath);
        if (T3D_FAILED(ret))
        {
            break;
        }

        ret = initResources();
        if (T3D_FAILED(ret))
        {
            break;
        }
    } while (false);

    return ret;
}
```

### 6.4 返回值

- 统一使用 `TResult` 作为返回值类型
- 成功返回 `T3D_OK`
- 使用 `T3D_FAILED(ret)` 宏判断失败

### 6.5 容器类型别名

使用 `T` 前缀包装标准容器：

| 别名 | 对应标准容器 |
|------|------|
| `TMap<K, V>` | `std::map<K, V>` |
| `TUnorderedMap<K, V>` | `std::unordered_map<K, V>` |
| `TList<T>` | `std::list<T>` |
| `TArray<T>` | `std::vector<T>` |
| `TFunction<Sig>` | `std::function<Sig>` |

```cpp
using RenderWindows = TMap<String, RenderWindowPtr>;
using Components = TArray<ComponentPtr>;
```

### 6.6 类型别名

- 新代码使用 `using` 声明
- 旧代码中的 `typedef` 不强制修改，但新增代码必须使用 `using`

```cpp
// 正确 ✓
using RenderWindows = TMap<String, RenderWindowPtr>;

// 不推荐（旧风格）
typedef TMap<String, RenderWindowPtr> RenderWindows;
```

### 6.7 virtual / override

| 场景 | 规则 |
|------|------|
| 基类虚函数 | 使用 `virtual` |
| 派生类重写 | 只用 `override`，**不重复** `virtual` |
| 纯虚函数 | `virtual Type func() = 0;` |
| 析构函数重写 | `~ClassName() override;` |

```cpp
// 基类
class Component
{
public:
    virtual ~Component();
    virtual void onStart() = 0;
    virtual void onDestroy() = 0;
};

// 派生类
class Camera : public Component
{
public:
    ~Camera() override;
    void onStart() override;
    void onDestroy() override;
};
```

### 6.8 RTTI 宏

使用项目自定义的 RTTI 宏标注可反射的类、属性和方法：

```cpp
class Camera : public Component
{
    TCLASS()
    TRTTI_ENABLE(Component)
    TRTTI_FRIEND

public:
    TPROPERTY(Serializable)
    float mFovY {60.0f};

    TFUNCTION()
    void setFovY(const Radian &fovY);
};
```

### 6.9 单例访问

通过宏访问全局单例对象：

```cpp
T3D_AGENT           // Agent 单例
T3D_SCENE_MGR       // SceneManager 单例
T3D_TEXTURE_MGR     // TextureManager 单例
```

---

## 附录：快速检查清单

| # | 检查项 | 要求 |
|---|--------|------|
| 1 | 文件头版权 | 每个文件包含 MIT License 块注释 |
| 2 | Include Guard | `__T3D_XXX_H__` 格式，非 `#pragma once` |
| 3 | 文件名 | `T3D` + PascalCase |
| 4 | 类名 | PascalCase |
| 5 | 成员变量 | `m` + PascalCase，`{}` 初始化 |
| 6 | 方法名 | camelCase |
| 7 | 枚举值 | `k` + PascalCase |
| 8 | 宏 | 全大写 + `T3D_` 前缀 |
| 9 | 缩进 | 4 空格，无 Tab |
| 10 | 大括号 | Allman 风格 |
| 11 | `*` / `&` | 靠近变量名 |
| 12 | 错误处理 | `do-while(false)` + `TResult` |
| 13 | 智能指针 | `T3D_DECLARE_SMART_PTR` |
| 14 | 对象创建 | `T3D_NEW` |
| 15 | override | 派生类不重复 `virtual` |
