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


#include "ImGuiImplTiny3D.h"
#include <SDL.h>


namespace Tiny3D
{
    namespace Editor
    {
        //----------------------------------------------------------------------
        // 内嵌 HLSL 着色器源码（用于 D3D11 等后端）
        //----------------------------------------------------------------------

        const char *ImGuiImplTiny3D::sImGuiVS_HLSL =
            "cbuffer ProjectionMatrix : register(b0)\n"
            "{\n"
            "    float4x4 ProjectionMatrix;\n"
            "};\n"
            "\n"
            "struct VS_INPUT\n"
            "{\n"
            "    float2 pos : POSITION;\n"
            "    float2 uv  : TEXCOORD0;\n"
            "    float4 col : COLOR0;\n"
            "};\n"
            "\n"
            "struct PS_INPUT\n"
            "{\n"
            "    float4 pos : SV_POSITION;\n"
            "    float4 col : COLOR0;\n"
            "    float2 uv  : TEXCOORD0;\n"
            "};\n"
            "\n"
            "PS_INPUT main(VS_INPUT input)\n"
            "{\n"
            "    PS_INPUT output;\n"
            "    output.pos = mul(ProjectionMatrix, float4(input.pos.xy, 0.0f, 1.0f));\n"
            "    output.col = input.col;\n"
            "    output.uv  = input.uv;\n"
            "    return output;\n"
            "}\n";

        const char *ImGuiImplTiny3D::sImGuiPS_HLSL =
            "struct PS_INPUT\n"
            "{\n"
            "    float4 pos : SV_POSITION;\n"
            "    float4 col : COLOR0;\n"
            "    float2 uv  : TEXCOORD0;\n"
            "};\n"
            "\n"
            "SamplerState sampler0 : register(s0);\n"
            "Texture2D texture0 : register(t0);\n"
            "\n"
            "float4 main(PS_INPUT input) : SV_Target\n"
            "{\n"
            "    float4 out_col = input.col * texture0.Sample(sampler0, input.uv);\n"
            "    return out_col;\n"
            "}\n";

        //----------------------------------------------------------------------
        // 内嵌 GLSL 着色器源码（用于 OpenGL 等后端）
        //----------------------------------------------------------------------

        const char *ImGuiImplTiny3D::sImGuiVS_GLSL =
            "#version 330 core\n"
            "layout (location = 0) in vec2 Position;\n"
            "layout (location = 1) in vec2 UV;\n"
            "layout (location = 2) in vec4 Color;\n"
            "layout(std140) uniform ImGuiProjMtx { mat4 ProjMtx; };\n"
            "out vec2 Frag_UV;\n"
            "out vec4 Frag_Color;\n"
            "void main()\n"
            "{\n"
            "    Frag_UV = UV;\n"
            "    Frag_Color = Color;\n"
            "    gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
            "}\n";

        const char *ImGuiImplTiny3D::sImGuiPS_GLSL =
            "#version 330 core\n"
            "in vec2 Frag_UV;\n"
            "in vec4 Frag_Color;\n"
            "uniform sampler2D Texture;\n"
            "layout (location = 0) out vec4 Out_Color;\n"
            "void main()\n"
            "{\n"
            "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
            "}\n";

        //----------------------------------------------------------------------

        ImGuiImplTiny3D::ImGuiImplTiny3D()
        {
        }

        //----------------------------------------------------------------------

        ImGuiImplTiny3D::~ImGuiImplTiny3D()
        {
            if (mInitialized)
            {
                // 清理 multi-viewport Renderer 回调指针（必须在 SDL Shutdown 和 DLL 卸载之前），
                // 否则后续 ImGui::DestroyContext → DestroyPlatformWindows 会调用悬空指针。
                ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();
                platform_io.Renderer_CreateWindow = nullptr;
                platform_io.Renderer_DestroyWindow = nullptr;
                platform_io.Renderer_RenderWindow = nullptr;
                platform_io.Renderer_SetWindowSize = nullptr;
                platform_io.Renderer_SwapBuffers = nullptr;

                ImGuiIO &io = ImGui::GetIO();
                io.BackendRendererUserData = nullptr;
                io.BackendFlags &= ~ImGuiBackendFlags_RendererHasViewports;

                ImGui_ImplSDL2_Shutdown();
            }

            mVertexBuffer = nullptr;
            mIndexBuffer = nullptr;
            mConstantBuffer = nullptr;
            mVertexDecl = nullptr;

            // 按层级从内到外释放材质对象
            mVertexShader = nullptr;
            mPixelShader = nullptr;
            mImGuiPass = nullptr;
            mImGuiTechnique = nullptr;
            mImGuiShader = nullptr;
            mBlendState = nullptr;
            mDepthStencilState = nullptr;
            mRasterizerState = nullptr;
            mSamplerState = nullptr;
            mFontTexture = nullptr;
        }

        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::init(void *initData)
        {
            // initData 传入的是 RenderWindow 指针
            mRenderWindow = static_cast<RenderWindow *>(initData);
            mSDLWindow = static_cast<SDL_Window *>(mRenderWindow->getNativeObject());

            // 通过 RHI 接口获取原生 context（GL4 返回 HGLRC/GLXContext，D3D11 返回 nullptr）
            RHIContextPtr ctx = T3D_AGENT.getActiveRHIContext();
            void *nativeCtx = ctx->getNativeContext();

            T3D_LOG_INFO(LOG_TAG_IMGUITINY3D, "init: nativeContext=%p", nativeCtx);

            // 初始化 SDL2 平台层
            if (nativeCtx != nullptr)
            {
                // 让 SDL 追踪引擎的 GL context（引擎通过 wglCreateContextAttribsARB 创建，
                // SDL 未追踪，需要先 MakeCurrent 一次让 SDL 内部记录）。
                // 这样后续 SDL_GL_GetCurrentContext() 才能返回正确的 context，
                // 子窗口创建流程中的 backup/restore 逻辑才能正常工作。
                SDL_GL_MakeCurrent(mSDLWindow, (SDL_GLContext)nativeCtx);

                // GL 后端：使用原生 context 初始化 SDL2 OpenGL 支持
                ImGui_ImplSDL2_InitForOpenGL(mSDLWindow, nativeCtx);
            }
            else
            {
                // D3D11 等非 GL 后端
                ImGui_ImplSDL2_InitForOther(mSDLWindow);
            }

            // Renderer multi-viewport 支持
            ImGuiIO &io = ImGui::GetIO();
            io.BackendRendererUserData = this;

            // 仅在有原生 context 时启用 multi-viewport Renderer 回调
            if (nativeCtx != nullptr)
            {
                io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
                initMultiViewportSupport();
            }
            else
            {
                // 无原生 context，关闭 ViewportsEnable
                io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
            }

            // 通过引擎 RHI 创建渲染资源
            createShaders();
            createRenderStates();
            createFontTexture();

            mInitialized = true;
        }

        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::refreshInfo(void *initData)
        {
            mRenderWindow = static_cast<RenderWindow *>(initData);
            mSDLWindow = static_cast<SDL_Window *>(mRenderWindow->getNativeObject());
            mRenderTarget = nullptr; // 窗口 resize 后需要重建
        }

        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::processEvents(void *event)
        {
            ImGui_ImplSDL2_ProcessEvent(static_cast<SDL_Event *>(event));
        }

        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::update()
        {
            ImGui_ImplSDL2_NewFrame();
        }

        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::preRender()
        {
            RHIContextPtr ctx = T3D_AGENT.getActiveRHIContext();

            // 先 reset 清理上一帧遗留状态
            ctx->reset();

            // 再设置渲染目标并清屏
            if (mRenderTarget == nullptr)
            {
                mRenderTarget = RenderTarget::create(mRenderWindow);
            }
            ctx->setRenderTarget(mRenderTarget.get());
            ctx->clearColor(ColorRGB(0.45f, 0.55f, 0.60f));
        }

        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::postRender()
        {
            ImDrawData *drawData = ImGui::GetDrawData();
            if (drawData == nullptr || drawData->TotalVtxCount == 0)
                return;

            renderDrawData(drawData);

            RHIContextPtr ctx = T3D_AGENT.getActiveRHIContext();
            ctx->reset();

            // Multi-viewport: 渲染次级 viewport 窗口
            ImGuiIO &io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();

                // 恢复主窗口渲染 context（GL4: wglMakeCurrent, D3D11: no-op）
                ctx->restoreNativeContext();
                ctx->reset();
            }
        }

        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::createShaders()
        {
            // 根据当前渲染器类型选择着色器源码
            RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
            const String &rendererName = renderer->getName();
            const char *vsSource = nullptr;
            const char *psSource = nullptr;

            if (rendererName == RHIRenderer::DIRECT3D11 
                || rendererName == RHIRenderer::NULL_DIRECT3D11)
            {
                vsSource = sImGuiVS_HLSL;
                psSource = sImGuiPS_HLSL;
            }
            else
            {
                // OpenGL 4 及其他
                vsSource = sImGuiVS_GLSL;
                psSource = sImGuiPS_GLSL;
            }

            // 1. 创建 ShaderVariant
            ShaderKeyword vsKeyword;
            mVertexShader = ShaderVariant::create(std::move(vsKeyword), vsSource);
            mVertexShader->setShaderStage(SHADER_STAGE::kVertex);

            ShaderKeyword psKeyword;
            mPixelShader = ShaderVariant::create(std::move(psKeyword), psSource);
            mPixelShader->setShaderStage(SHADER_STAGE::kPixel);

            // 2. 创建 Pass，添加 ShaderVariant（自动设置 variant->setPass(this)）
            mImGuiPass = Pass::create("0");
            mImGuiPass->addShaderVariant(mVertexShader->getShaderKeyword(), mVertexShader);
            mImGuiPass->addShaderVariant(mPixelShader->getShaderKeyword(), mPixelShader);

            // 3. 创建 Technique，添加 Pass（自动设置 pass->setTechnique(tech)）
            mImGuiTechnique = Technique::create("ImGui-Technique");
            mImGuiTechnique->addPass(mImGuiPass);

            // 4. 创建 Shader，添加 Technique（自动设置 tech->setShader(shader)）
            mImGuiShader = T3D_SHADER_MGR.createShader("ImGui-Shader");
            mImGuiShader->addTechnique(mImGuiTechnique);

            // 5. 通过 ShaderVariant::compile() 编译（内部完成 compileShader + createXXXShader）
            TResult ret = mVertexShader->compile();
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_IMGUITINY3D, "Failed to compile ImGui vertex shader!");
                return;
            }

            ret = mPixelShader->compile();
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_IMGUITINY3D, "Failed to compile ImGui pixel shader!");
                return;
            }
        }

        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::createRenderStates()
        {
            // Alpha 混合状态：SrcAlpha / OneMinusSrcAlpha
            BlendDesc blendDesc;
            blendDesc.AlphaToCoverageEnable = false;
            blendDesc.IndependentBlendEnable = false;
            blendDesc.RenderTargetStates[0].BlendEnable = true;
            blendDesc.RenderTargetStates[0].SrcBlend = BlendFactor::kSrcAlpha;
            blendDesc.RenderTargetStates[0].DestBlend = BlendFactor::kOneMinusSrcAlpha;
            blendDesc.RenderTargetStates[0].BlendOp = BlendOperation::kAdd;
            blendDesc.RenderTargetStates[0].SrcBlendAlpha = BlendFactor::kOne;
            blendDesc.RenderTargetStates[0].DstBlendAlpha = BlendFactor::kOneMinusSrcAlpha;
            blendDesc.RenderTargetStates[0].BlendOpAlpha = BlendOperation::kAdd;
            blendDesc.RenderTargetStates[0].ColorMask = kWriteMaskAll;
            mBlendState = T3D_RENDER_STATE_MGR.loadBlendState(blendDesc);

            // 深度模板状态：禁用深度测试和写入
            DepthStencilDesc dsDesc;
            dsDesc.DepthTestEnable = false;
            dsDesc.DepthWriteEnable = false;
            dsDesc.StencilEnable = false;
            mDepthStencilState = T3D_RENDER_STATE_MGR.loadDepthStencilState(dsDesc);

            // 光栅化状态：无背面剔除，实心填充，启用裁剪矩形
            RasterizerDesc rsDesc;
            rsDesc.FillMode = PolygonMode::kSolid;
            rsDesc.CullMode = CullingMode::kNone;
            rsDesc.DepthClipEnable = true;
            rsDesc.ScissorEnable = false;
            rsDesc.MultisampleEnable = false;
            rsDesc.AntialiasedLineEnable = false;
            mRasterizerState = T3D_RENDER_STATE_MGR.loadRasterizerState(rsDesc);

            // 纹理采样器：线性过滤，Wrap 寻址
            SamplerDesc sampDesc;
            sampDesc.MinFilter = FilterOptions::kLinear;
            sampDesc.MagFilter = FilterOptions::kLinear;
            sampDesc.MipFilter = FilterOptions::kNone;
            sampDesc.AddressU = TextureAddressMode::kWrap;
            sampDesc.AddressV = TextureAddressMode::kWrap;
            sampDesc.AddressW = TextureAddressMode::kWrap;
            mSamplerState = T3D_RENDER_STATE_MGR.loadSamplerState(sampDesc);
        }

        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::createFontTexture()
        {
            ImGuiIO &io = ImGui::GetIO();
            unsigned char *pixels = nullptr;
            int width = 0, height = 0;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

            // 创建 PixelBuffer2D 作为字体纹理
            // 注意：PixelBufferT 内部保存 desc 指针（mDesc），因此 desc 必须堆分配，
            // 生命周期与 PixelBuffer2D 对象一致。栈变量会在函数返回后被销毁导致悬空指针。
            PixelBuffer2DDesc *desc = new PixelBuffer2DDesc();
            desc->width = width;
            desc->height = height;
            desc->mipmaps = 1;
            desc->arraySize = 1;
            desc->format = PixelFormat::E_PF_R8G8B8A8;
            desc->shaderReadable = true;
            desc->buffer.DataSize = width * height * 4;
            desc->buffer.Data = T3D_POD_NEW_ARRAY(uint8_t, desc->buffer.DataSize);
            memcpy(desc->buffer.Data, pixels, desc->buffer.DataSize);

            mFontTexture = T3D_RENDER_BUFFER_MGR.loadPixelBuffer2D(desc, MemoryType::kVRAM, Usage::kStatic, kCPUNone);

            // 注册字体纹理并设置 ImTextureID
            ImTextureID texID = registerTexture(mFontTexture);
            io.Fonts->SetTexID(texID);
        }

        //----------------------------------------------------------------------

        ImTextureID ImGuiImplTiny3D::registerTexture(const PixelBuffer2DPtr &texture)
        {
            ImTextureID texID = reinterpret_cast<ImTextureID>(texture.get());
            mTextureMap[texID] = texture;
            return texID;
        }

        //----------------------------------------------------------------------

        PixelBuffer2DPtr ImGuiImplTiny3D::findTexture(ImTextureID texID) const
        {
            auto itr = mTextureMap.find(texID);
            if (itr != mTextureMap.end())
            {
                return itr->second;
            }
            return nullptr;
        }

        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::createBuffers(int totalVtxCount, int totalIdxCount)
        {
            // 如果当前缓冲区容量不足，重新创建
            if (mVertexBuffer == nullptr || mVertexBufferSize < totalVtxCount)
            {
                mVertexBuffer = nullptr;
                mVertexBufferSize = totalVtxCount + 5000;

                Buffer vbData;
                vbData.DataSize = mVertexBufferSize * sizeof(ImDrawVert);
                vbData.Data = T3D_POD_NEW_ARRAY(uint8_t, vbData.DataSize);
                memset(vbData.Data, 0, vbData.DataSize);

                mVertexBuffer = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(
                    sizeof(ImDrawVert),
                    mVertexBufferSize,
                    vbData,
                    MemoryType::kVRAM,
                    Usage::kDynamic,
                    kCPUWrite);
            }

            if (mIndexBuffer == nullptr || mIndexBufferSize < totalIdxCount)
            {
                mIndexBuffer = nullptr;
                mIndexBufferSize = totalIdxCount + 10000;

                Buffer ibData;
                ibData.DataSize = mIndexBufferSize * sizeof(ImDrawIdx);
                ibData.Data = T3D_POD_NEW_ARRAY(uint8_t, ibData.DataSize);
                memset(ibData.Data, 0, ibData.DataSize);

                mIndexBuffer = T3D_RENDER_BUFFER_MGR.loadIndexBuffer(
                    sizeof(ImDrawIdx) == 2 ? IndexType::E_IT_16BITS : IndexType::E_IT_32BITS,
                    mIndexBufferSize,
                    ibData,
                    MemoryType::kVRAM,
                    Usage::kDynamic,
                    kCPUWrite);
            }

            // 创建或重建正交投影矩阵的常量缓冲区
            if (mConstantBuffer == nullptr)
            {
                Buffer cbData;
                cbData.DataSize = sizeof(float) * 16;
                cbData.Data = T3D_POD_NEW_ARRAY(uint8_t, cbData.DataSize);
                memset(cbData.Data, 0, cbData.DataSize);

                mConstantBuffer = T3D_RENDER_BUFFER_MGR.loadConstantBuffer(
                    "ImGuiProjMtx",
                    0,
                    cbData,
                    MemoryType::kVRAM,
                    Usage::kDynamic,
                    kCPUWrite);
            }
        }

        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::setupRenderState(ImDrawData *drawData)
        {
            RHIContextPtr ctx = T3D_AGENT.getActiveRHIContext();

            // 设置视口（Viewport 使用比例值 [0,1]）
            Viewport vp;
            vp.Left = 0.0f;
            vp.Top = 0.0f;
            vp.Width = 1.0f;
            vp.Height = 1.0f;
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;
            ctx->setViewport(vp);

            // 设置渲染状态
            ctx->setBlendState(mBlendState.get());
            ctx->setDepthStencilState(mDepthStencilState.get());
            ctx->setRasterizerState(mRasterizerState.get());
            ctx->setPrimitiveType(PrimitiveType::kTriangleList);

            // 设置着色器
            ctx->setVertexShader(mVertexShader.get());
            ctx->setPixelShader(mPixelShader.get());

            // 设置纹理采样器
            Samplers samplers;
            samplers.push_back(mSamplerState);
            ctx->setPSSamplers(0, samplers);

            // 创建正交投影矩阵
            float L = drawData->DisplayPos.x;
            float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
            float T = drawData->DisplayPos.y;
            float B = drawData->DisplayPos.y + drawData->DisplaySize.y;

            // 首帧诊断日志
            {
                static bool sFirstSetup = true;
                if (sFirstSetup)
                {
                    T3D_LOG_INFO(LOG_TAG_IMGUITINY3D,
                        "setupRenderState [DIAG]: L=%.1f R=%.1f T=%.1f B=%.1f (W=%.1f H=%.1f)",
                        L, R, T, B, R - L, B - T);
                    sFirstSetup = false;
                }
            }

            float mvp[4][4] =
            {
                { 2.0f / (R - L),       0.0f,                0.0f, 0.0f },
                { 0.0f,                  2.0f / (T - B),      0.0f, 0.0f },
                { 0.0f,                  0.0f,                0.5f, 0.0f },
                { (R + L) / (L - R),     (T + B) / (B - T),  0.5f, 1.0f },
            };

            // 更新常量缓冲区
            Buffer cbData;
            cbData.DataSize = sizeof(mvp);
            cbData.Data = reinterpret_cast<uint8_t *>(&mvp[0][0]);
            mConstantBuffer->writeData(0, cbData, true);

            // 绑定常量缓冲区到 VS
            ConstantBuffers cbs;
            cbs.push_back(mConstantBuffer);
            ctx->setVSConstantBuffers(0, cbs);

            // 创建并设置顶点声明（InputLayout / VAO）
            // 注意：在 OpenGL 中必须先绑定 VAO，再设置 VB/IB，
            // 因为 GL_ELEMENT_ARRAY_BUFFER 绑定是 VAO 状态的一部分。
            if (mVertexDecl == nullptr)
            {
                VertexAttributes attrs;
                uint32_t offset = 0;

                // Position: float2
                attrs.push_back(VertexAttribute(
                    0, offset,
                    VertexAttribute::Type::E_VAT_FLOAT2,
                    VertexAttribute::Semantic::E_VAS_POSITION, 0));
                offset += sizeof(float) * 2;

                // TexCoord: float2
                attrs.push_back(VertexAttribute(
                    0, offset,
                    VertexAttribute::Type::E_VAT_FLOAT2,
                    VertexAttribute::Semantic::E_VAS_TEXCOORD, 0));
                offset += sizeof(float) * 2;

                // Color: ubyte4_norm (RGBA 4字节归一化)
                attrs.push_back(VertexAttribute(
                    0, offset,
                    VertexAttribute::Type::E_VAT_UBYTE4_NORM,
                    VertexAttribute::Semantic::E_VAS_DIFFUSE, 0));

                mVertexDecl = T3D_RENDER_BUFFER_MGR.addVertexDeclaration(attrs, mVertexShader.get());
            }

            ctx->setVertexDeclaration(mVertexDecl.get());

            // 设置顶点缓冲
            VertexBuffers vbs;
            vbs.push_back(mVertexBuffer);
            VertexStrides strides;
            strides.push_back(sizeof(ImDrawVert));
            VertexOffsets offsets;
            offsets.push_back(0);
            ctx->setVertexBuffers(0, vbs, strides, offsets);

            // 设置索引缓冲
            ctx->setIndexBuffer(mIndexBuffer.get());
        }

        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::renderDrawData(ImDrawData *drawData)
        {
            // 避免在最小化时渲染
            if (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f)
                return;

            // 首帧诊断日志
            static bool sFirstFrame = true;
            if (sFirstFrame)
            {
                T3D_LOG_INFO(LOG_TAG_IMGUITINY3D,
                    "renderDrawData [DIAG]: DisplayPos=(%.1f,%.1f) DisplaySize=(%.1f,%.1f) "
                    "FbScale=(%.2f,%.2f) TotalVtxCount=%d TotalIdxCount=%d CmdListsCount=%d",
                    drawData->DisplayPos.x, drawData->DisplayPos.y,
                    drawData->DisplaySize.x, drawData->DisplaySize.y,
                    drawData->FramebufferScale.x, drawData->FramebufferScale.y,
                    drawData->TotalVtxCount, drawData->TotalIdxCount,
                    drawData->CmdListsCount);
                sFirstFrame = false;
            }

            RHIContextPtr ctx = T3D_AGENT.getActiveRHIContext();

            // 清理引擎渲染管线遗留的 GL 状态（program、VAO、FBO 等），
            // 确保后续 ImGui 渲染的状态链完整、从零开始设置。
            // 注意：reset() 会将 mCurrentRenderTarget 置为 nullptr，
            // 因此必须在之后重新 setRenderTarget。
            ctx->reset();

            // 重新设置 render target
            if (mRenderTarget != nullptr)
            {
                ctx->setRenderTarget(mRenderTarget.get());
            }

            // 确保缓冲区足够大
            createBuffers(drawData->TotalVtxCount, drawData->TotalIdxCount);

            // 上传顶点和索引数据到 GPU 缓冲区（合并后一次写入）
            {
                size_t totalVtxSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
                size_t totalIdxSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);

                uint8_t *vtxDst = T3D_POD_NEW_ARRAY(uint8_t, totalVtxSize);
                uint8_t *idxDst = T3D_POD_NEW_ARRAY(uint8_t, totalIdxSize);

                size_t vtxOffset = 0;
                size_t idxOffset = 0;

                for (int n = 0; n < drawData->CmdListsCount; n++)
                {
                    const ImDrawList *cmdList = drawData->CmdLists[n];

                    size_t vtxSize = cmdList->VtxBuffer.Size * sizeof(ImDrawVert);
                    memcpy(vtxDst + vtxOffset, cmdList->VtxBuffer.Data, vtxSize);
                    vtxOffset += vtxSize;

                    size_t idxSize = cmdList->IdxBuffer.Size * sizeof(ImDrawIdx);
                    memcpy(idxDst + idxOffset, cmdList->IdxBuffer.Data, idxSize);
                    idxOffset += idxSize;
                }

                // 如果实际数据量超过缓冲区容量，强制重建
                if (mVertexBuffer->getBufferSize() < totalVtxSize)
                {
                    mVertexBuffer = nullptr;
                    mVertexBufferSize = drawData->TotalVtxCount + 5000;

                    Buffer vbResize;
                    vbResize.DataSize = mVertexBufferSize * sizeof(ImDrawVert);
                    vbResize.Data = T3D_POD_NEW_ARRAY(uint8_t, vbResize.DataSize);
                    memcpy(vbResize.Data, vtxDst, totalVtxSize);
                    memset(vbResize.Data + totalVtxSize, 0, vbResize.DataSize - totalVtxSize);

                    mVertexBuffer = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(
                        sizeof(ImDrawVert), mVertexBufferSize, vbResize,
                        MemoryType::kVRAM, Usage::kDynamic, kCPUWrite);
                }
                else
                {
                    Buffer vtxData;
                    vtxData.DataSize = totalVtxSize;
                    vtxData.Data = vtxDst;
                    mVertexBuffer->writeData(0, vtxData, true);
                    vtxData.Data = nullptr;
                }

                if (mIndexBuffer->getBufferSize() < totalIdxSize)
                {
                    mIndexBuffer = nullptr;
                    mIndexBufferSize = drawData->TotalIdxCount + 10000;

                    Buffer ibResize;
                    ibResize.DataSize = mIndexBufferSize * sizeof(ImDrawIdx);
                    ibResize.Data = T3D_POD_NEW_ARRAY(uint8_t, ibResize.DataSize);
                    memcpy(ibResize.Data, idxDst, totalIdxSize);
                    memset(ibResize.Data + totalIdxSize, 0, ibResize.DataSize - totalIdxSize);

                    mIndexBuffer = T3D_RENDER_BUFFER_MGR.loadIndexBuffer(
                        sizeof(ImDrawIdx) == 2 ? IndexType::E_IT_16BITS : IndexType::E_IT_32BITS,
                        mIndexBufferSize, ibResize,
                        MemoryType::kVRAM, Usage::kDynamic, kCPUWrite);
                }
                else
                {
                    Buffer idxData;
                    idxData.DataSize = totalIdxSize;
                    idxData.Data = idxDst;
                    mIndexBuffer->writeData(0, idxData, true);
                    idxData.Data = nullptr;
                }

                T3D_POD_SAFE_DELETE_ARRAY(vtxDst);
                T3D_POD_SAFE_DELETE_ARRAY(idxDst);
            }

            // 设置渲染状态
            setupRenderState(drawData);

            // 渲染 ImGui 命令列表
            int globalIdxOffset = 0;
            int globalVtxOffset = 0;
            ImVec2 clipOff = drawData->DisplayPos;

            for (int n = 0; n < drawData->CmdListsCount; n++)
            {
                const ImDrawList *cmdList = drawData->CmdLists[n];

                for (int cmdIdx = 0; cmdIdx < cmdList->CmdBuffer.Size; cmdIdx++)
                {
                    const ImDrawCmd *pcmd = &cmdList->CmdBuffer[cmdIdx];

                    if (pcmd->UserCallback != nullptr)
                    {
                        if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                        {
                            setupRenderState(drawData);
                        }
                        else
                        {
                            pcmd->UserCallback(cmdList, pcmd);
                        }
                    }
                    else
                    {
                        // 计算裁剪矩形
                        ImVec2 clipMin(pcmd->ClipRect.x - clipOff.x, pcmd->ClipRect.y - clipOff.y);
                        ImVec2 clipMax(pcmd->ClipRect.z - clipOff.x, pcmd->ClipRect.w - clipOff.y);
                        if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
                            continue;

                        // 设置裁剪矩形（通过视口实现，或通过 RHI 裁剪矩形）
                        // TODO: 使用 RHI scissor rect API（如果引擎支持）
                        // 目前通过视口不处理裁剪，依赖光栅化状态的 ScissorEnable

                        // 绑定纹理
                        // ImTextureID 统一约定为 PixelBuffer2D* 指针
                        // （来自 registerTexture、ImTextureManager::loadTexture、EditorSceneImpl::getSceneRT 等）
                        ImTextureID texID = pcmd->GetTexID();
                        PixelBuffer2D *rawTexPtr = reinterpret_cast<PixelBuffer2D *>(texID);
                        if (rawTexPtr == nullptr)
                        {
                            rawTexPtr = mFontTexture.get();
                        }
                        if (rawTexPtr != nullptr)
                        {
                            PixelBuffers texBuffers;
                            texBuffers.push_back(PixelBuffer2DPtr(rawTexPtr));
                            ctx->setPSPixelBuffers(0, texBuffers);
                        }

                        // 执行带索引的绘制调用
                        ctx->render(
                            pcmd->ElemCount,
                            pcmd->IdxOffset + globalIdxOffset,
                            pcmd->VtxOffset + globalVtxOffset);
                    }
                }

                globalIdxOffset += cmdList->IdxBuffer.Size;
                globalVtxOffset += cmdList->VtxBuffer.Size;
            }
        }

        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::initMultiViewportSupport()
        {
            ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();
            platform_io.Renderer_CreateWindow = ImGui_Renderer_CreateWindow;
            platform_io.Renderer_DestroyWindow = ImGui_Renderer_DestroyWindow;
            platform_io.Renderer_RenderWindow = ImGui_Renderer_RenderWindow;
            platform_io.Renderer_SetWindowSize = ImGui_Renderer_SetWindowSize;
        }

        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::renderViewportDrawData(ImDrawData *drawData)
        {
            if (drawData == nullptr || drawData->TotalVtxCount == 0)
                return;

            // 避免在最小化时渲染
            if (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f)
                return;

            RHIContextPtr ctx = T3D_AGENT.getActiveRHIContext();

            // 次级 viewport 渲染：GL context 已被 Platform_RenderWindow 切换到子窗口，
            // 引擎的 GL 状态追踪（mCurrentProgram、mCurrentVAO 等）已失效，先 reset。
            ctx->reset();

            // 不绑定引擎 mRenderTarget（那是主窗口的），
            // 直接渲染到子窗口的默认 framebuffer (FBO 0)。
            // resetRenderTarget 会 glBindFramebuffer(GL_FRAMEBUFFER, 0)。
            ctx->resetRenderTarget();

            // 确保缓冲区足够大
            createBuffers(drawData->TotalVtxCount, drawData->TotalIdxCount);

            // 上传顶点和索引数据到 GPU 缓冲区
            {
                size_t totalVtxSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
                size_t totalIdxSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);

                uint8_t *vtxDst = T3D_POD_NEW_ARRAY(uint8_t, totalVtxSize);
                uint8_t *idxDst = T3D_POD_NEW_ARRAY(uint8_t, totalIdxSize);

                size_t vtxOffset = 0;
                size_t idxOffset = 0;

                for (int n = 0; n < drawData->CmdListsCount; n++)
                {
                    const ImDrawList *cmdList = drawData->CmdLists[n];

                    size_t vtxSize = cmdList->VtxBuffer.Size * sizeof(ImDrawVert);
                    memcpy(vtxDst + vtxOffset, cmdList->VtxBuffer.Data, vtxSize);
                    vtxOffset += vtxSize;

                    size_t idxSize = cmdList->IdxBuffer.Size * sizeof(ImDrawIdx);
                    memcpy(idxDst + idxOffset, cmdList->IdxBuffer.Data, idxSize);
                    idxOffset += idxSize;
                }

                if (mVertexBuffer->getBufferSize() < totalVtxSize)
                {
                    mVertexBuffer = nullptr;
                    mVertexBufferSize = drawData->TotalVtxCount + 5000;

                    Buffer vbResize;
                    vbResize.DataSize = mVertexBufferSize * sizeof(ImDrawVert);
                    vbResize.Data = T3D_POD_NEW_ARRAY(uint8_t, vbResize.DataSize);
                    memcpy(vbResize.Data, vtxDst, totalVtxSize);
                    memset(vbResize.Data + totalVtxSize, 0, vbResize.DataSize - totalVtxSize);

                    mVertexBuffer = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(
                        sizeof(ImDrawVert), mVertexBufferSize, vbResize,
                        MemoryType::kVRAM, Usage::kDynamic, kCPUWrite);
                }
                else
                {
                    Buffer vtxData;
                    vtxData.DataSize = totalVtxSize;
                    vtxData.Data = vtxDst;
                    mVertexBuffer->writeData(0, vtxData, true);
                    vtxData.Data = nullptr;
                }

                if (mIndexBuffer->getBufferSize() < totalIdxSize)
                {
                    mIndexBuffer = nullptr;
                    mIndexBufferSize = drawData->TotalIdxCount + 10000;

                    Buffer ibResize;
                    ibResize.DataSize = mIndexBufferSize * sizeof(ImDrawIdx);
                    ibResize.Data = T3D_POD_NEW_ARRAY(uint8_t, ibResize.DataSize);
                    memcpy(ibResize.Data, idxDst, totalIdxSize);
                    memset(ibResize.Data + totalIdxSize, 0, ibResize.DataSize - totalIdxSize);

                    mIndexBuffer = T3D_RENDER_BUFFER_MGR.loadIndexBuffer(
                        sizeof(ImDrawIdx) == 2 ? IndexType::E_IT_16BITS : IndexType::E_IT_32BITS,
                        mIndexBufferSize, ibResize,
                        MemoryType::kVRAM, Usage::kDynamic, kCPUWrite);
                }
                else
                {
                    Buffer idxData;
                    idxData.DataSize = totalIdxSize;
                    idxData.Data = idxDst;
                    mIndexBuffer->writeData(0, idxData, true);
                    idxData.Data = nullptr;
                }

                T3D_POD_SAFE_DELETE_ARRAY(vtxDst);
                T3D_POD_SAFE_DELETE_ARRAY(idxDst);
            }

            // 设置渲染状态
            setupRenderState(drawData);

            // 渲染 ImGui 命令列表
            int globalIdxOffset = 0;
            int globalVtxOffset = 0;
            ImVec2 clipOff = drawData->DisplayPos;

            for (int n = 0; n < drawData->CmdListsCount; n++)
            {
                const ImDrawList *cmdList = drawData->CmdLists[n];

                for (int cmdIdx = 0; cmdIdx < cmdList->CmdBuffer.Size; cmdIdx++)
                {
                    const ImDrawCmd *pcmd = &cmdList->CmdBuffer[cmdIdx];

                    if (pcmd->UserCallback != nullptr)
                    {
                        if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                        {
                            setupRenderState(drawData);
                        }
                        else
                        {
                            pcmd->UserCallback(cmdList, pcmd);
                        }
                    }
                    else
                    {
                        ImVec2 clipMin(pcmd->ClipRect.x - clipOff.x, pcmd->ClipRect.y - clipOff.y);
                        ImVec2 clipMax(pcmd->ClipRect.z - clipOff.x, pcmd->ClipRect.w - clipOff.y);
                        if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
                            continue;

                        ImTextureID texID = pcmd->GetTexID();
                        PixelBuffer2D *rawTexPtr = reinterpret_cast<PixelBuffer2D *>(texID);
                        if (rawTexPtr == nullptr)
                        {
                            rawTexPtr = mFontTexture.get();
                        }
                        if (rawTexPtr != nullptr)
                        {
                            PixelBuffers texBuffers;
                            texBuffers.push_back(PixelBuffer2DPtr(rawTexPtr));
                            ctx->setPSPixelBuffers(0, texBuffers);
                        }

                        ctx->render(
                            pcmd->ElemCount,
                            pcmd->IdxOffset + globalIdxOffset,
                            pcmd->VtxOffset + globalVtxOffset);
                    }
                }

                globalIdxOffset += cmdList->IdxBuffer.Size;
                globalVtxOffset += cmdList->VtxBuffer.Size;
            }
        }

        //----------------------------------------------------------------------
        // Multi-viewport Renderer 端回调
        //----------------------------------------------------------------------

        void ImGuiImplTiny3D::ImGui_Renderer_CreateWindow(ImGuiViewport *vp)
        {
            // 次级 viewport 窗口已由 SDL2 Platform_CreateWindow 创建，
            // GL context 也已由 SDL2 通过 SDL_GL_CreateContext 创建（共享资源）。
            // Renderer 端无需额外的创建操作。
        }

        void ImGuiImplTiny3D::ImGui_Renderer_DestroyWindow(ImGuiViewport *vp)
        {
            // GL context 由 SDL2 Platform_DestroyWindow 销毁。
            // Renderer 端无需额外的销毁操作。
        }

        void ImGuiImplTiny3D::ImGui_Renderer_RenderWindow(ImGuiViewport *vp, void *render_arg)
        {
            // 此回调在 Platform_RenderWindow（SDL_GL_MakeCurrent 子 context）之后执行。
            // 当前 GL context 已切换到子窗口的共享 context。
            ImGuiIO &io = ImGui::GetIO();
            ImGuiImplTiny3D *self = static_cast<ImGuiImplTiny3D *>(io.BackendRendererUserData);
            if (self != nullptr)
            {
                self->renderViewportDrawData(vp->DrawData);
            }
        }

        void ImGuiImplTiny3D::ImGui_Renderer_SetWindowSize(ImGuiViewport *vp, ImVec2 size)
        {
            // 子窗口 resize 时由 SDL2 处理，GL viewport 在 renderViewportDrawData
            // 的 setupRenderState 中通过 setViewport 设置，无需额外操作。
        }

        //----------------------------------------------------------------------
    }
}
