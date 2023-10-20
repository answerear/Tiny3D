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


#ifndef __T3D_RENDER_PIPELINE_H__
#define __T3D_RENDER_PIPELINE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API RenderPipeline
        : public Object
        , public Singleton<RenderPipeline>
    {
    public:
        enum : uint32_t
        {
            DEFAULT_GROUP = 0,
            NONE_GROUP = 0xFFFFFFFF,
            DEFAULT_ORDER = 0xFFFFFFFF,
        };

        /**
         * \brief 执行渲染
         * \param [in] ctx : RHI 渲染上下文
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult render(RHIContextPtr ctx) = 0;

        /**
         * \brief 附加渲染目标到渲染管线，渲染管线执行渲染
         * \param [in] target : 渲染目标 
         * \param [in] group : 渲染目标分组，默认是 DEFAULT_GROUP 分组，内部都分到一组
         * \param [in] order : 渲染目标的渲染顺序，默认是 DEFAULT_ORDER，就是按照 attach 的顺序 
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult attachRenderTarget(RenderTargetPtr target, uint32_t group = DEFAULT_GROUP, uint32_t order = DEFAULT_ORDER) = 0;

        /**
         * \brief 附加渲染纹理到渲染管线，渲染管线执行渲染。 调用该接口不会渲染到窗口上
         * \param [in] texture : 渲染纹理
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult attachRenderTexture(RenderTexturePtr texture) = 0;

        /**
         * \brief 从渲染管线上解除出来，不再渲染
         * \param [in] name : 渲染目标名称 
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult detachRenderTarget(const String &name) = 0;

        virtual TResult detachAllRenderTargets() = 0;

        /**
         * \brief 根据名称获取渲染目标对象
         * \param [in] name : 想要获取的渲染目标名称
         * \return 调用成功返回渲染目标对象，否则返回 nullptr
         */
        virtual RenderTargetPtr getRenderTarget(const String &name) const = 0;
    };

    #define T3D_RENDER_PIPELINE     (RenderPipeline::getInstance()) 
}


#endif  /*__T3D_RENDER_PIPELINE_H__*/
