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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#ifndef __T3D_RHI_RENDERER_H__
#define __T3D_RHI_RENDERER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief RHI 渲染器抽象基类，持有 RHIContext 并提供后端初始化与着色语言映射
     */
    class T3D_ENGINE_API RHIRenderer : public Object
    {
    public:
        /// Null 渲染器插件名
        static const char * const NULLRENDERER;
        /// Reference3D 渲染器插件名
        static const char * const REFERENCE3D;
        /// Direct3D9 渲染器插件名
        static const char * const DIRECT3D9;
        /// Direct3D11 窗口模式渲染器插件名
        static const char * const DIRECT3D11;
        /// Direct3D11 控制台模式渲染器插件名
        static const char * const DIRECT3D11_CONSOLE;
        /// OpenGL4 控制台模式渲染器插件名
        static const char * const OPENGL4_CONSOLE;
        /// Direct3D12 渲染器插件名
        static const char * const DIRECT3D12;
        /// OpenGL 4.x 渲染器插件名
        static const char * const OPENGL4;
        /// OpenGL ES 2 渲染器插件名
        static const char * const OPENGLES2;
        /// OpenGL ES 3 渲染器插件名
        static const char * const OPENGLES3;
        /// Vulkan 渲染器插件名
        static const char * const VULKAN;
        /// Vulkan 控制台模式渲染器插件名
        static const char * const VULKAN_CONSOLE;
        /// Metal 渲染器插件名
        static const char * const METAL;

        /**
         * \brief 初始化渲染器及 RHI 子系统
         * \return 调用成功返回 T3D_OK；由具体后端实现
         */
        virtual TResult init() = 0;

        /**
         * \brief 销毁渲染器并释放 RHI 资源
         * \return 调用成功返回 T3D_OK；由具体后端实现
         */
        virtual TResult destroy() = 0;

        /**
         * \brief 填充编辑器所需的渲染器/窗口信息
         * \param [out] info : 由实现者写入的编辑器信息结构，类型由调用方约定
         * \param [in] window : 关联的 RenderWindow，可为 nullptr
         */
        virtual void getEditorInfo(void *info, RenderWindow *window) = 0;

        /**
         * \brief 获取渲染器名称字符串
         * \return 与插件注册名对应的 mName 常引用
         */
        const String &getName() const { return mName; }

        /**
         * \brief 根据渲染器名称推导对应的着色语言
         * \return 当前后端对应的 SHADER_LANGUAGE
         * \remarks 映射规则见 T3DRHIRenderer.cpp：OpenGL4→kGLSL，OpenGLES→kESSL，
         *          Vulkan→kSPIRV，Metal→kMSL，其余（含 D3D 系列）默认 kHLSL
         */
        SHADER_LANGUAGE getShadingLanguage() const;

        /**
         * \brief 获取本渲染器绑定的 RHI 上下文
         * \return RHIContext 智能指针，未初始化时可能为 nullptr
         */
        RHIContextPtr getContext() { return mContext; }

    protected:
        /// 渲染器插件名称（与静态常量对应）
        String                  mName {};
        /// 本渲染器创建的 RHI 上下文
        RHIContextPtr           mContext {nullptr};
    };
}


#endif    /*__T3D_RHI_RENDERER_H__*/
