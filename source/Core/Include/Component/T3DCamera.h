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

#ifndef __T3D_CAMERA_H__
#define __T3D_CAMERA_H__


#include "Component/T3DComponent.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderTexture.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API Camera : public Component
    {
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND
        
    public:
        static CameraPtr create();

        ~Camera() override = default;
        
        ComponentPtr clone() const override;

        RenderTargetPtr getRenderTarget() const { return mRenderarget; }

        void setRenderTarget(RenderTargetPtr target) { mRenderarget = target; }

        uint32_t getOrder() const { return mOrder; }

        void setOrder(uint32_t order) { mOrder = order; }
        
    protected:
        Camera() = default;

    protected:
        /// 相机对应的渲染纹理，如果渲染目标是渲染纹理，则直接渲染到渲染目标上，不经过纹理
        RenderTexturePtr    mRenderTexture {nullptr};
        /// 相机对应的渲染目标
        RenderTargetPtr     mRenderarget {nullptr};
        /// 渲染顺序
        uint32_t    mOrder {0};
        /// 剔除掩码，位为 1 的会去判断是否剔除，为 0 的本相机直接不渲染
        uint32_t    mCullingMask {0xFFFFFFFF};
    };
}


#endif    /*__T3D_CAMERA_H__*/
