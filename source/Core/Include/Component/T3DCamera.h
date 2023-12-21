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
#include "Render/T3DViewport.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API Camera : public Component
    {
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND
        
    public:
        static CameraPtr create();

        ~Camera() override;
        
        ComponentPtr clone() const override;

        RenderTargetPtr getRenderTarget() const { return mRenderarget; }

        void setRenderTarget(RenderTargetPtr target);

        RenderTexturePtr getRenderTexture() const { return mRenderTexture; }

        uint32_t getOrder() const { return mOrder; }

        void setOrder(uint32_t order) { mOrder = order; }

        const Viewport &getViewport() const { return mViewport; }

        void setViewport(const Viewport &vp) { mViewport = vp; }

        const ColorRGB &getClearColor() const { return mClearColor; }

        void setClearColor(const ColorRGB &color) { mClearColor = color; }

        Real getClearDepth() const { return mClearDepth; }

        void setClearDepth(Real depth) { mClearDepth = depth; }

        uint8_t getClearStencil() const { return mClearStencil; }

        void setClearStencil(uint8_t stencil) { mClearStencil = stencil; }
        
    protected:
        Camera() = default;

        void setupRenderTexture(RenderWindow *window);

        void setupQuad(RenderWindow *window);
        
    protected:
        /// 相机对应的渲染纹理，如果渲染目标是渲染纹理，则直接渲染到渲染目标上，不经过纹理
        RenderTexturePtr    mRenderTexture {nullptr};
        /// 相机对应的渲染目标
        RenderTargetPtr     mRenderarget {nullptr};
        /// 渲染顺序
        uint32_t    mOrder {0};
        /// 剔除掩码，位为 1 的会去判断是否剔除，为 0 的本相机直接不渲染
        uint32_t    mCullingMask {0xFFFFFFFF};
        /// 相机视口
        Viewport    mViewport {};
        /// 背景填充颜色
        ColorRGB    mClearColor {ColorRGB::BLACK};
        /// 深度缓冲填充值
        Real    mClearDepth {0.0f};
        /// 模板缓冲填充值
        uint8_t mClearStencil {0};
    };
}


#endif    /*__T3D_CAMERA_H__*/
