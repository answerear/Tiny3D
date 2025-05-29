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


#ifndef __T3D_RENDER_TARGET_H__
#define __T3D_RENDER_TARGET_H__


#include "T3DPrerequisites.h"
#include "T3DRenderConstant.h"
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

        static RenderTargetPtr create(RenderWindow *renderWindow, RenderTexture *depthStencil = nullptr);

        static RenderTargetPtr create(RenderTexture *renderTexture, RenderTexture *depthStencil = nullptr);

        static RenderTargetPtr create(const RenderTextures &renderTextures, RenderTexture *depthStencil = nullptr);

        ~RenderTarget() override;
        
        Type getType() const;

        const RenderWindowPtr &getRenderWindow() const { return mRenderWindow; }

        const RenderTexturePtr &getRenderTexture(uint32_t index) const { return mRenderTextures[index]; }

        const RenderTexturePtr &getRenderTexture() const { return mRenderTextures[0];}

        RenderTexturePtr getDepthStencil() const { return mDepthStencil; }

        uint32_t getNumOfRenderTextures() const { return mNumOfTextures; }

        const RenderTexturePtr *getRenderTextures() const { return mRenderTextures; }

        void releaseAllResources();

    protected:
        RenderTarget(RenderWindow *renderWindow, RenderTexture *depthStencil);

        RenderTarget(RenderTexture *renderTexture, RenderTexture *depthStencil);

        RenderTarget(const RenderTextures &renderTextures, RenderTexture *depthStencil);

        /// 渲染窗口，渲染窗口和渲染纹理只能二选一
        RenderWindowPtr     mRenderWindow {nullptr};
        /// 渲染纹理，渲染纹理和渲染窗口只能二选一
        RenderTexturePtr    mRenderTextures[T3D_MAX_RENDER_TARGET] {nullptr};
        /// 深度模板纹理，用于渲染目标
        RenderTexturePtr    mDepthStencil {nullptr};
        /// 实际使用的渲染纹理数量
        uint32_t mNumOfTextures {0};
    };
}


#include "T3DRenderTarget.inl"


#endif  /*__T3D_RENDER_TARGET_H__*/
