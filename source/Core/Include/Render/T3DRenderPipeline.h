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
         * \brief 场景物体遍历和剔除
         * \param [in] scene : 场景对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult cull(Scene *scene) = 0;

        /**
         * \brief 执行渲染
         * \param [in] ctx : RHI 渲染上下文
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult render(RHIContext *ctx) = 0;

        virtual TResult addRenderable(Camera *camera, Renderable *renderable) = 0;

        virtual TResult removeRenderable(Renderable *renderable) = 0;
    };

    #define T3D_RENDER_PIPELINE     (RenderPipeline::getInstance()) 
}


#endif  /*__T3D_RENDER_PIPELINE_H__*/
