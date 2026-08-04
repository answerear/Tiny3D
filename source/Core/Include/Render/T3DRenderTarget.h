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
     * \brief 渲染目标，绑定窗口或纹理（含 MRT）及可选深度/模板附件
     */
    class T3D_ENGINE_API RenderTarget : public Object
    {
    public:
        /**
         * \brief 渲染目标类型
         */
        enum class Type : uint32_t
        {
            /// 渲染到窗口（mRenderWindow 非空）
            E_RT_WINDOW = 0,
            /// 渲染到纹理（mRenderWindow 为空）
            E_RT_TEXTURE,
            E_RT_MAX
        };

        /**
         * \brief 创建以窗口为颜色附件的渲染目标
         * \param [in] renderWindow : 渲染窗口
         * \param [in] depthStencil : 深度/模板纹理，可为 nullptr
         * \return 新创建的 RenderTarget 指针
         */
        static RenderTargetPtr create(RenderWindow *renderWindow, RenderTexture *depthStencil = nullptr);

        /**
         * \brief 创建以单张纹理为颜色附件的渲染目标
         * \param [in] renderTexture : 颜色附件纹理；为 nullptr 时 mNumOfTextures 为 0
         * \param [in] depthStencil : 深度/模板纹理，可为 nullptr
         * \return 新创建的 RenderTarget 指针
         */
        static RenderTargetPtr create(RenderTexture *renderTexture, RenderTexture *depthStencil = nullptr);

        /**
         * \brief 创建 MRT 渲染目标
         * \param [in] renderTextures : 颜色附件纹理列表
         * \param [in] depthStencil : 深度/模板纹理，可为 nullptr
         * \return 新创建的 RenderTarget 指针
         */
        static RenderTargetPtr create(const RenderTextures &renderTextures, RenderTexture *depthStencil = nullptr);

        ~RenderTarget() override;
        
        /**
         * \brief 返回渲染目标类型（mRenderWindow 非空则为 E_RT_WINDOW，否则 E_RT_TEXTURE）
         * \return 类型枚举值
         */
        Type getType() const;

        /// 返回关联的渲染窗口；纹理目标时为 nullptr
        const RenderWindowPtr &getRenderWindow() const { return mRenderWindow; }

        /// 返回指定索引的颜色附件纹理
        const RenderTexturePtr &getRenderTexture(uint32_t index) const { return mRenderTextures[index]; }

        /// 返回首个颜色附件纹理（索引 0）
        const RenderTexturePtr &getRenderTexture() const { return mRenderTextures[0];}

        /// 返回深度/模板附件纹理
        RenderTexturePtr getDepthStencil() const { return mDepthStencil; }

        /// 返回实际使用的颜色附件数量
        uint32_t getNumOfRenderTextures() const { return mNumOfTextures; }

        /// 返回颜色附件纹理数组首指针
        const RenderTexturePtr *getRenderTextures() const { return mRenderTextures; }

        /**
         * \brief 通过 TextureManager 卸载全部颜色附件与深度/模板纹理并置空
         */
        void releaseAllResources();

    protected:
        /**
         * \brief 构造窗口型渲染目标
         * \param [in] renderWindow : 渲染窗口
         * \param [in] depthStencil : 深度/模板纹理，可为 nullptr
         */
        RenderTarget(RenderWindow *renderWindow, RenderTexture *depthStencil);

        /**
         * \brief 构造单纹理渲染目标
         * \param [in] renderTexture : 颜色附件纹理
         * \param [in] depthStencil : 深度/模板纹理，可为 nullptr
         */
        RenderTarget(RenderTexture *renderTexture, RenderTexture *depthStencil);

        /**
         * \brief 构造 MRT 渲染目标
         * \param [in] renderTextures : 颜色附件纹理列表
         * \param [in] depthStencil : 深度/模板纹理，可为 nullptr
         */
        RenderTarget(const RenderTextures &renderTextures, RenderTexture *depthStencil);

        /// 渲染窗口；与 mRenderTextures 互斥，窗口目标时非空
        RenderWindowPtr     mRenderWindow {nullptr};
        /// 颜色附件纹理数组，最多 T3D_MAX_RENDER_TARGET 个
        RenderTexturePtr    mRenderTextures[T3D_MAX_RENDER_TARGET] {nullptr};
        /// 深度/模板附件纹理
        RenderTexturePtr    mDepthStencil {nullptr};
        /// 实际使用的颜色附件数量
        uint32_t mNumOfTextures {0};
    };
}


#include "T3DRenderTarget.inl"


#endif  /*__T3D_RENDER_TARGET_H__*/
