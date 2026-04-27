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


#pragma once


#include "ImGuiTiny3DPrerequisites.h"


namespace Tiny3D
{
    namespace Editor
    {
        /**
         * @brief ImGui 的 RHI 渲染后端，通过引擎 RHI 抽象层渲染 ImGui，
         *        不依赖任何特定图形 API（DX11/OpenGL/Vulkan 等）。
         */
        class ImGuiImplTiny3D : public ImGuiImpl
        {
        public:
            ImGuiImplTiny3D();

            ~ImGuiImplTiny3D() override;

            void init(void *initData) override;

            void refreshInfo(void *initData) override;

            void processEvents(void *event) override;

            void update() override;

            void preRender() override;

            void postRender() override;

        private:
            void createShaders();
            void createRenderStates();
            void createFontTexture();
            void createBuffers(int totalVtxCount, int totalIdxCount);
            void setupRenderState(ImDrawData *drawData);
            void renderDrawData(ImDrawData *drawData);

            /// 渲染次级 viewport 的 DrawData（不绑定引擎 RenderTarget，直接渲染到当前 FBO）
            void renderViewportDrawData(ImDrawData *drawData);

            /// 初始化 multi-viewport 支持：注册 Renderer 端回调
            void initMultiViewportSupport();

            /// 注册纹理到映射表，返回 ImTextureID
            ImTextureID registerTexture(const PixelBuffer2DPtr &texture);
            /// 根据 ImTextureID 查找纹理智能指针
            PixelBuffer2DPtr findTexture(ImTextureID texID) const;

            // ImGui multi-viewport Renderer 端回调（静态函数）
            static void ImGui_Renderer_CreateWindow(ImGuiViewport *vp);
            static void ImGui_Renderer_DestroyWindow(ImGuiViewport *vp);
            static void ImGui_Renderer_RenderWindow(ImGuiViewport *vp, void *render_arg);
            static void ImGui_Renderer_SetWindowSize(ImGuiViewport *vp, ImVec2 size);

            // 内嵌着色器源码
            static const char *sImGuiVS_HLSL;
            static const char *sImGuiPS_HLSL;
            static const char *sImGuiVS_GLSL;
            static const char *sImGuiPS_GLSL;

            // 平台层
            SDL_Window         *mSDLWindow {nullptr};
            RenderWindow       *mRenderWindow {nullptr};
            RenderTargetPtr     mRenderTarget;

            // RHI 资源
            VertexBufferPtr         mVertexBuffer;
            IndexBufferPtr          mIndexBuffer;
            ConstantBufferPtr       mConstantBuffer;
            VertexDeclarationPtr    mVertexDecl;

            // 材质层级：Shader → Technique → Pass → ShaderVariant
            ShaderPtr               mImGuiShader;
            TechniquePtr            mImGuiTechnique;
            PassPtr                 mImGuiPass;
            ShaderVariantPtr        mVertexShader;
            ShaderVariantPtr        mPixelShader;
            BlendStatePtr           mBlendState;
            DepthStencilStatePtr    mDepthStencilState;
            RasterizerStatePtr      mRasterizerState;
            SamplerStatePtr         mSamplerState;
            PixelBuffer2DPtr        mFontTexture;

            // ImTextureID -> PixelBuffer2DPtr 映射，避免从裸指针构造智能指针
            TMap<ImTextureID, PixelBuffer2DPtr> mTextureMap;

            // 缓冲容量追踪
            int mVertexBufferSize {0};
            int mIndexBufferSize {0};

            bool mInitialized {false};
        };
    }
}
