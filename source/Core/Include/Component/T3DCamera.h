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
        /**
         * \brief 投影矩阵类型
         */
        TENUM()
        enum class Projection : uint32_t
        {
            /// 正交投影
            kOrthographic = 0,
            /// 透视投影
            kPerspective
        };
        
        static CameraPtr create();

        ~Camera() override;
        
        ComponentPtr clone() const override;

        RenderTargetPtr getRenderTarget() const { return mRenderTarget; }

        void setRenderTarget(RenderTargetPtr target);

        RenderTexturePtr getRenderTexture() const { return mRenderTexture; }

        TPROPERTY(RTTRFuncName="Order", RTTRFuncType="getter")
        uint32_t getOrder() const { return mOrder; }

        TPROPERTY(RTTRFuncName="Order", RTTRFuncType="setter")
        void setOrder(uint32_t order) { mOrder = order; }

        TPROPERTY(RTTRFuncName="Viewport", RTTRFuncType="getter")
        const Viewport &getViewport() const { return mViewport; }

        TPROPERTY(RTTRFuncName="Viewport", RTTRFuncType="setter")
        void setViewport(const Viewport &vp) { mViewport = vp; }

        TPROPERTY(RTTRFuncName="ClearColor", RTTRFuncType="getter")
        const ColorRGB &getClearColor() const { return mClearColor; }

        TPROPERTY(RTTRFuncName="ClearColor", RTTRFuncType="setter")
        void setClearColor(const ColorRGB &color) { mClearColor = color; }

        TPROPERTY(RTTRFuncName="ClearDepth", RTTRFuncType="getter")
        Real getClearDepth() const { return mClearDepth; }

        TPROPERTY(RTTRFuncName="ClearDepth", RTTRFuncType="setter")
        void setClearDepth(Real depth) { mClearDepth = depth; }

        TPROPERTY(RTTRFuncName="ClearStencil", RTTRFuncType="getter")
        uint8_t getClearStencil() const { return mClearStencil; }

        TPROPERTY(RTTRFuncName="ClearStencil", RTTRFuncType="setter")
        void setClearStencil(uint8_t stencil) { mClearStencil = stencil; }

        TPROPERTY(RTTRFuncName="ProjectionType", RTTRFuncType="getter")
        Projection getProjectionType() const { return mProjectionType; }

        TPROPERTY(RTTRFuncName="ProjectionType", RTTRFuncType="setter")
        void setProjectionType(Projection type) { mProjectionType = type; }

        TPROPERTY(RTTRFuncName="FovY", RTTRFuncType="getter")
        const Radian &getFovY() const { return mFovY; }

        TPROPERTY(RTTRFuncName="FovY", RTTRFuncType="setter")
        void setFovY(const Radian &fovY) { mFovY = fovY; }

        TPROPERTY(RTTRFuncName="AspectRatio", RTTRFuncType="getter")
        const Real &getAspectRatio() const { return mAspectRatio; }

        TPROPERTY(RTTRFuncName="AspectRatio", RTTRFuncType="setter")
        void setAspectRatio(const Real &ratio) { mAspectRatio = ratio; }

        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="getter")
        const Real &getWidth() const { return mWidth; }

        TPROPERTY(RTTRFuncName="Width", RTTRFuncType="setter")
        void setWidth(const Real &width) { mWidth = width; }

        TPROPERTY(RTTRFuncName="Height", RTTRFuncType="getter")
        const Real &getHeight() const { return mHeight; }

        TPROPERTY(RTTRFuncName="Height", RTTRFuncType="setter")
        void setHeight(const Real &height) { mHeight = height; }

        TPROPERTY(RTTRFuncName="FarPlane", RTTRFuncType="getter")
        const Real &getFarPlaneDistance() const { return mFar; }

        TPROPERTY(RTTRFuncName="FarPlane", RTTRFuncType="setter")
        void setFarPlaneDistance(const Real &f) { mFar = f; }

        TPROPERTY(RTTRFuncName="NearPlane", RTTRFuncType="getter")
        const Real &getNearPlaneDistance() const { return mNear; }

        TPROPERTY(RTTRFuncName="NearPlane", RTTRFuncType="setter")
        void setNearPlaneDistance(const Real &n) { mNear = n; }
        
        const Matrix4 &getViewMatrix() const;

        const Matrix4 &getProjectMatrix() const;
        
    protected:
        Camera() = default;

        TResult cloneProperties(const Component * const src) override;

        void setupRenderTexture(RenderWindow *window);

        void setupQuad(RenderWindow *window);
        
    protected:
        /// 相机对应的渲染纹理，如果渲染目标是渲染纹理，则直接渲染到渲染目标上，不经过纹理
        RenderTexturePtr    mRenderTexture {nullptr};
        /// 相机对应的渲染目标
        RenderTargetPtr     mRenderTarget {nullptr};
        
        /// 渲染顺序
        uint32_t    mOrder {0};
        /// 剔除掩码，位为 1 的会去判断是否剔除，为 0 的本相机直接不渲染
        uint32_t    mCullingMask {0xFFFFFFFF};
        /// 相机视口
        Viewport    mViewport {};
        /// 背景填充颜色
        ColorRGB    mClearColor {ColorRGB::BLACK};
        /// 深度缓冲填充值
        Real    mClearDepth {1.0f};
        /// 模板缓冲填充值
        uint8_t mClearStencil {0};
        
        /// 视口变换矩阵
        mutable Matrix4 mViewMatrix {}; 
        /// 投影变换矩阵
        mutable Matrix4 mProjectMatrix {};

        /// 投影矩阵是否需要重新计算标记
        mutable bool mIsProjDirty {false};

        /// 投影类型
        Projection  mProjectionType {Projection::kPerspective};

        /// 上下平面夹角，用于透视投影
        Radian  mFovY {Math::PI / REAL_HALF};
        /// 宽高比，用于透视投影
        Real    mAspectRatio {16.0f / 9.0f};
        /// 视口宽度，用于正交投影
        Real    mWidth {REAL_ZERO};
        /// 视口高度，用于正交投影
        Real    mHeight {REAL_ZERO};
        /// 远平面
        Real    mFar {REAL_ZERO};
        /// 近平面
        Real    mNear {REAL_ZERO};
    };
}


#endif    /*__T3D_CAMERA_H__*/
