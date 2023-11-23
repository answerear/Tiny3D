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


#ifndef __T3D_RENDER_TARGET_H__
#define __T3D_RENDER_TARGET_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Render/T3DViewport.h"
//#include "RHI/T3DRHIRenderer.h"


namespace Tiny3D
{
    /**
     * /brief 渲染目标
     */
    class T3D_ENGINE_API RenderTarget : public Object
    {
    public:
        /**
         * /brief 渲染目标类型
         */
        enum class Type : uint32_t
        {
            /// 渲染到窗口
            E_RT_WINDOW = 0,
            /// 渲染到纹理
            E_RT_TEXTURE,
            E_RT_MAX
        };

        static RenderTargetPtr create(RenderWindowPtr renderWindow);

        static RenderTargetPtr create(RenderTexturePtr renderTexture);

        ~RenderTarget() override;
        
        Type getType() const;

        RenderWindowPtr getRenderWindow() const { return mRenderWindow; }

        RenderTexturePtr getRenderTexture() const { return mRenderTexture; }

    protected:
        RenderTarget(RenderWindowPtr renderWindow);

        RenderTarget(RenderTexturePtr renderTexture);

        /// 渲染窗口，渲染窗口和渲染纹理只能二选一
        RenderWindowPtr     mRenderWindow {nullptr};
        /// 渲染纹理，渲染纹理和渲染窗口只能二选一
        RenderTexturePtr    mRenderTexture {nullptr};
    };
}


#include "T3DRenderTarget.inl"


#endif  /*__T3D_RENDER_TARGET_H__*/
