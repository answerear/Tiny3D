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

#ifndef __T3D_RML_RENDER_INTERFACE_H__
#define __T3D_RML_RENDER_INTERFACE_H__


#include "T3DRmlUiPrerequisites.h"

#include <RmlUi/Core/RenderInterface.h>


namespace Tiny3D
{
    /**
     * \brief RmlUi 渲染接口
     * \remarks 几何、纹理、scissor、CSS transform，以及用模板缓冲做的 clip mask。
     */
    class RmlRenderInterface : public Rml::RenderInterface
    {
    public:
        ~RmlRenderInterface() override;

        Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
            Rml::Span<const int> indices) override;
        void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation,
            Rml::TextureHandle texture) override;
        void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

        Rml::TextureHandle LoadTexture(Rml::Vector2i &texture_dimensions, const Rml::String &source) override;
        Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override;
        void ReleaseTexture(Rml::TextureHandle texture) override;

        void EnableScissorRegion(bool enable) override;
        void SetScissorRegion(Rml::Rectanglei region) override;

        void SetTransform(const Rml::Matrix4f *transform) override;
        void EnableClipMask(bool enable) override;
        void RenderToClipMask(Rml::ClipMaskOperation operation, Rml::CompiledGeometryHandle geometry,
            Rml::Vector2f translation) override;

        /**
         * \brief 为一台相机的一次 UI 绘制准备投影和状态
         * \param [in] ctx : 当前 RHI 上下文，进入前已经 setRenderTarget
         * \param [in] width : 视口宽度（像素）
         * \param [in] height : 视口高度（像素）
         * \param [in] originX : 视口左上角在目标上的像素 X
         * \param [in] originY : 视口左上角在目标上的像素 Y
         * \param [in] stencilAvailable : 当前目标带模板缓冲。没有时 clip mask 降级为不裁剪
         * \return shader 尚未就绪时返回 false
         */
        bool beginFrame(RHIContext *ctx, int32_t width, int32_t height, int32_t originX, int32_t originY,
            bool stencilAvailable);

        /// 把 scissor 恢复成整屏，并关掉模板测试
        void endFrame();

    private:
        bool ensureShaders();
        void applyColorAndStencil();
        DepthStencilState *stencilState(CompareFunction func, StencilOp passOp, uint32_t ref);
        Rml::TextureHandle uploadRGBA(const uint8_t *rgba, int32_t width, int32_t height);
        PixelBuffer2D *textureFromHandle(Rml::TextureHandle texture) const;

        RHIContext *mCtx {nullptr};
        int32_t mWidth {0};
        int32_t mHeight {0};
        int32_t mOriginX {0};
        int32_t mOriginY {0};
        bool mScissor {false};
        bool mShadersReady {false};
        bool mStencilAvailable {false};
        bool mStencilWarned {false};
        bool mClipEnabled {false};
        bool mClipWrite {false};
        uint32_t mClipRef {0};

        MaterialPtr mColorMaterial {nullptr};
        MaterialPtr mTextureMaterial {nullptr};
        PassInstance *mColorPass {nullptr};
        PassInstance *mTexturePass {nullptr};
        VertexDeclarationPtr mVertexDecl {nullptr};

        BlendStatePtr mBlendState {nullptr};
        BlendStatePtr mNoColorBlendState {nullptr};
        DepthStencilStatePtr mDepthStencilState {nullptr};
        TUnorderedMap<uint32_t, DepthStencilStatePtr> mStencilStates {};
        RasterizerStatePtr mRasterizerScissor {nullptr};
        RasterizerStatePtr mRasterizerNoScissor {nullptr};
        SamplerStatePtr mSampler {nullptr};

        TUnorderedMap<PixelBuffer2D *, PixelBuffer2DPtr> mTextures {};
        Matrix4 mTransform {Matrix4::IDENTITY};
    };
}


#endif  /*__T3D_RML_RENDER_INTERFACE_H__*/
