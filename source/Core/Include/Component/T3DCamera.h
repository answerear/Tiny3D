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
