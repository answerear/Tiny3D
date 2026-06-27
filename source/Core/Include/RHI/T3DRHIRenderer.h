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
    class T3D_ENGINE_API RHIRenderer : public Object
    {
    public:
        /// 空渲染器
        static const char * const NULLRENDERER;
        /// The reference 3D
        static const char * const REFERENCE3D;
        /// Direct3D9 渲染器
        static const char * const DIRECT3D9;
        /// 窗口模式下的 Direct3D11 渲染器
        static const char * const DIRECT3D11;
        /// 控制台模式下的 D3D11
        static const char * const DIRECT3D11_CONSOLE;
        /// 控制台模式下的 OpenGL4
        static const char * const OPENGL4_CONSOLE;
        /// Direct3D12 渲染器
        static const char * const DIRECT3D12;
        /// OpenGL 4.x 渲染器
        static const char * const OPENGL4;
        /// OpenGL ES 2 渲染器
        static const char * const OPENGLES2;
        /// OpenGL ES 3 渲染器
        static const char * const OPENGLES3;
        /// Vulkan 渲染器
        static const char * const VULKAN;
        /// 控制台模式下的 Vulkan
        static const char * const VULKAN_CONSOLE;
        /// Metal 渲染器
        static const char * const METAL;

        /**
         * @fn  virtual TResult Renderer::init() = 0;
         * @brief   初始化渲染器
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult init() = 0;

        /**
         * @fn  virtual TResult Renderer::destroy() = 0;
         * @brief   销毁渲染器
         * @returns 调用成功返回 T3D_OK.
         */
        virtual TResult destroy() = 0;

        /**
         * @brief 获取编辑需要的信息
         * @param [in] info : 存放编辑器的信息，实现者负责填充
         */
        virtual void getEditorInfo(void *info, RenderWindow *window) = 0;
        
        /**
         * @fn  const String Renderer::&getName() const;
         * @brief   获取渲染器名称
         * @returns The name.
         */
        const String &getName() const { return mName; }

        /**
         * @brief   根据渲染器类型推导对应的着色语言（Graphics API 维度）
         * @returns 当前渲染后端对应的 SHADER_LANGUAGE
         * @remarks 这是「渲染后端 → 着色语言」的唯一映射来源，运行时按此选取
         *          ShaderVariantSet 中对应语言的变体。
         */
        SHADER_LANGUAGE getShadingLanguage() const;

        RHIContextPtr getContext() { return mContext; }
        
    protected:
        /// 渲染器名称
        String                  mName {};
        /// RHI 渲染上下文
        RHIContextPtr           mContext {nullptr};
    };
}


#endif    /*__T3D_RHI_RENDERER_H__*/
