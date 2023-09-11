/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef __T3D_RHI_RENDERER_H__
#define __T3D_RHI_RENDERER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "RHI/T3DRHIRenderWindow.h"
#include "RHI/T3DRHIContext.h"


namespace Tiny3D
{
    class T3D_ENGINE_API RHIRenderer : public Object
    {
    public:
        static const char * const NULLRENDERER;/**< 空渲染器 */
        static const char * const REFERENCE3D;  /**< The reference 3D */
        static const char * const DIRECT3D9;    /**< Direct3D9 渲染器 */
        static const char * const DIRECT3D11;   /**< Direct3D11 渲染器 */
        static const char * const DIRECT3D12;   /**< Direct3D12 渲染器 */
        static const char * const OPENGL3;      /**< OpenGL 3.x 渲染器 */
        static const char * const OPENGLES2;    /**< OpenGL ES 2 渲染器 */
        static const char * const OPENGLES3;    /**< OpenGL ES 3 渲染器 */
        static const char * const VULKAN;       /**< Vulkan 渲染器 */
        static const char * const METAL;        /**< Metal 渲染器 */

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
         * @fn  const String Renderer::&getName() const;
         * @brief   获取渲染器名称
         * @returns The name.
         */
        const String &getName() const { return mName; }
        
        virtual RHIRenderWindowPtr createRenderWindow(
           const String &name, const RenderWindowCreateParam &param) = 0;

        RHIContextPtr getContext() { return mContext; }
        
    protected:
        String                  mName;              /**< 渲染器名称 */

        RHIContextPtr           mContext;           /**< RHI 渲染上下文 */
        RHIRenderWindowPtr      mPrimaryWindow;     /**< 主窗口 */
    };
}


#endif    /*__T3D_RHI_RENDERER_H__*/
