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
    using ViewportList = TMap<long_t, ViewportPtr>;
    
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
            /**< 渲染到窗口 */
            E_RT_WINDOW = 0,
            /**< 渲染到纹理 */
            E_RT_TEXTURE,
            E_RT_MAX
        };
        
        const String &getName() const;

        virtual Type getType() const = 0;
        
        void getMetrics(size_t &width, size_t &height, size_t &clrDepth) const;

        size_t getWidth() const;

        size_t getHeight() const;

        size_t getColorDepth() const;

        size_t getPitch() const;

        ViewportPtr addViewport(long_t zOrder, Real left, Real top, Real width, Real height);
        
        TResult removeViewport(long_t zOrder);
        
        TResult removeAllViewports();
        
        const ViewportList &getViewports() const;
        
        ViewportPtr getViewport(long_t zOrder) const;

        RHIRenderTargetPtr getRHIRenderTarget() const;

    protected:
        RenderTarget(const String &name);

        ~RenderTarget() override;

        /**< 渲染目标宽度 */
        size_t mWidth {0};
        /**< 渲染目标高度 */
        size_t mHeight {0};
        /**< 渲染目标色深 */
        size_t mColorDepth {0};
        /**< 渲染目标行间距 */
        size_t mPitch {0};
        /**< 渲染目标名称 */
        String mName {};
        /**< 视口列表 */
        ViewportList mViewports {};

        RHIRenderTargetPtr  mRHIRenderTarget {nullptr};
    };
}


#include "T3DRenderTarget.inl"


#endif  /*__T3D_RENDER_TARGET_H__*/
