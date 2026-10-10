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

#include "T3DRmlRenderInterface.h"

#include "ImageCodec/T3DImageCodec.h"
#include "Kernel/T3DArchive.h"
#include "Material/T3DTechniqueInstance.h"
#include "Render/T3DBlendState.h"
#include "Render/T3DRenderConstant.h"
#include "Render/T3DDepthStencilState.h"
#include "Render/T3DRasterizerState.h"
#include "Render/T3DRenderBufferDesc.h"
#include "Render/T3DRenderResourceManager.h"
#include "Render/T3DSamplerState.h"
#include "Material/T3DPassInstance.h"
#include "Render/T3DVertexAttribute.h"
#include "Resource/T3DAssetManager.h"
#include "Resource/T3DImage.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DTexture.h"

#include <RmlUi/Core/Matrix4.h>

#include <cstddef>
#include <cstring>
#include <string>


namespace Tiny3D
{
    namespace
    {
        const char *kColorShader = "assets/samples/ui/RmlUI-Color.tshader";
        const char *kTextureShader = "assets/samples/ui/RmlUI-Texture.tshader";

        struct RmlCompiledMesh
        {
            VertexBufferPtr Vertices {nullptr};
            IndexBufferPtr Indices {nullptr};
            uint32_t IndexCount {0};
        };

        uint8_t premultiplyChannel(uint8_t channel, uint8_t alpha)
        {
            return static_cast<uint8_t>((static_cast<uint32_t>(channel) * alpha + 127u) / 255u);
        }

        /**
         * \brief 把 RmlUi 列主序矩阵转成引擎的行主序 Matrix4
         * \remarks 两边都是列向量，平移在最后一列。Rml 的 data() 按列存放，
         *          这里转置后才能交给 row_major 的 mul(Transform, pos)。
         */
        Matrix4 fromRmlMatrix(const Rml::Matrix4f &source)
        {
            const float *value = source.data();
            return Matrix4(
                value[0], value[4], value[8], value[12],
                value[1], value[5], value[9], value[13],
                value[2], value[6], value[10], value[14],
                value[3], value[7], value[11], value[15]);
        }
    }

    //--------------------------------------------------------------------------

    RmlRenderInterface::~RmlRenderInterface() = default;

    //--------------------------------------------------------------------------

    Rml::CompiledGeometryHandle RmlRenderInterface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
        Rml::Span<const int> indices)
    {
        if (vertices.empty() || indices.empty())
        {
            return 0;
        }

        auto *geometry = T3D_NEW RmlCompiledMesh();

        Buffer vertexData;
        vertexData.setData(vertices.data(), vertices.size() * sizeof(Rml::Vertex));
        geometry->Vertices = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(
            static_cast<uint32_t>(sizeof(Rml::Vertex)),
            static_cast<uint32_t>(vertices.size()),
            vertexData,
            MemoryType::kBoth,
            Usage::kStatic,
            kCPUNone);
        vertexData.release();

        Buffer indexData;
        indexData.setData(indices.data(), indices.size() * sizeof(int));
        geometry->Indices = T3D_RENDER_BUFFER_MGR.loadIndexBuffer(
            IndexType::E_IT_32BITS,
            static_cast<uint32_t>(indices.size()),
            indexData,
            MemoryType::kBoth,
            Usage::kStatic,
            kCPUNone);
        indexData.release();
        geometry->IndexCount = static_cast<uint32_t>(indices.size());

        if (geometry->Vertices == nullptr || geometry->Indices == nullptr)
        {
            T3D_DELETE geometry;
            return 0;
        }

        ++mGeometryCount;
        return reinterpret_cast<Rml::CompiledGeometryHandle>(geometry);
    }

    //--------------------------------------------------------------------------

    void RmlRenderInterface::RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation,
        Rml::TextureHandle texture)
    {
        if (mCtx == nullptr || geometry == 0 || mColorPass == nullptr)
        {
            return;
        }

        auto *mesh = reinterpret_cast<RmlCompiledMesh *>(geometry);
        PixelBuffer2D *pixels = texture != 0 ? textureFromHandle(texture) : nullptr;
        if (texture != 0 && pixels == nullptr)
        {
            return;
        }

        const bool textured = pixels != nullptr && mTexturePass != nullptr;
        Material *material = textured ? mTextureMaterial.get() : mColorMaterial.get();
        PassInstance *pass = textured ? mTexturePass : mColorPass;
        if (material == nullptr || pass == nullptr)
        {
            return;
        }

        material->setMatrix("Transform", mTransform);
        material->setVector("Translation", Vector4(translation.x, translation.y, 0.0f, 0.0f));
        pass->bind(mCtx, material);
        applyColorAndStencil();

        if (textured)
        {
            ShaderVariantInstance *pixelShader = pass->getCurrentPixelShader();
            const uint32_t samplerSlot = pixelShader != nullptr ? pixelShader->getSamplerStartSlot() : 0;
            const uint32_t textureSlot = pixelShader != nullptr ? pixelShader->getPixelBufferStartSlot() : 0;

            Samplers samplers;
            samplers.push_back(mSampler);
            mCtx->setPSSamplers(samplerSlot, samplers);

            PixelBuffers buffers;
            buffers.push_back(PixelBuffer2DPtr(pixels));
            mCtx->setPSPixelBuffers(textureSlot, buffers);
        }

        mCtx->setVertexDeclaration(mVertexDecl.get());

        VertexBuffers vertexBuffers;
        vertexBuffers.push_back(mesh->Vertices);
        VertexStrides strides;
        strides.push_back(static_cast<uint32_t>(sizeof(Rml::Vertex)));
        VertexOffsets offsets;
        offsets.push_back(0);
        mCtx->setVertexBuffers(0, vertexBuffers, strides, offsets);
        mCtx->setIndexBuffer(mesh->Indices.get());
        ++mFrameDraws;
        mCtx->render(mesh->IndexCount, 0, 0);
    }

    //--------------------------------------------------------------------------

    void RmlRenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
    {
        if (geometry == 0)
        {
            return;
        }
        if (mGeometryCount > 0)
        {
            --mGeometryCount;
        }
        T3D_DELETE reinterpret_cast<RmlCompiledMesh *>(geometry);
    }

    //--------------------------------------------------------------------------

    Rml::TextureHandle RmlRenderInterface::uploadRGBA(const uint8_t *rgba, int32_t width, int32_t height)
    {
        if (rgba == nullptr || width <= 0 || height <= 0)
        {
            return 0;
        }

        // desc 由 PixelBuffer 持有指针，和 ImGui 字体纹理一样不在这里释放。
        auto *desc = new PixelBuffer2DDesc();
        desc->width = static_cast<uint32_t>(width);
        desc->height = static_cast<uint32_t>(height);
        desc->mipmaps = 1;
        desc->arraySize = 1;
        desc->format = PixelFormat::E_PF_R8G8B8A8;
        desc->shaderReadable = true;
        desc->buffer.DataSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4u;
        desc->buffer.Data = T3D_POD_NEW_ARRAY(uint8_t, desc->buffer.DataSize);
        memcpy(desc->buffer.Data, rgba, desc->buffer.DataSize);

        PixelBuffer2DPtr texture = T3D_RENDER_BUFFER_MGR.loadPixelBuffer2D(
            desc, MemoryType::kVRAM, Usage::kStatic, kCPUNone);
        if (texture == nullptr)
        {
            return 0;
        }

        PixelBuffer2D *raw = texture.get();
        mTextures.emplace(raw, texture);
        return reinterpret_cast<Rml::TextureHandle>(raw);
    }

    //--------------------------------------------------------------------------

    static bool endsWithIgnoreCase(const Rml::String &path, const char *suffix)
    {
        const size_t length = std::char_traits<char>::length(suffix);
        if (path.size() < length)
        {
            return false;
        }

        const size_t offset = path.size() - length;
        for (size_t i = 0; i < length; ++i)
        {
            unsigned char left = static_cast<unsigned char>(path[offset + i]);
            unsigned char right = static_cast<unsigned char>(suffix[i]);
            if (left >= 'A' && left <= 'Z')
            {
                left = static_cast<unsigned char>(left - 'A' + 'a');
            }
            if (right >= 'A' && right <= 'Z')
            {
                right = static_cast<unsigned char>(right - 'A' + 'a');
            }
            if (left != right)
            {
                return false;
            }
        }
        return true;
    }

    //--------------------------------------------------------------------------

    Rml::TextureHandle RmlRenderInterface::loadEngineTexture(Rml::Vector2i &texture_dimensions, const Rml::String &source)
    {
        TexturePtr texture = T3D_ASSET_MGR.loadTexture(source);
        if (texture == nullptr || texture->getTextureType() != TEXTURE_TYPE::TT_2D)
        {
            T3D_LOG_WARNING(LOG_TAG_RMLUI, "LoadTexture failed to reuse engine texture %s", source.c_str());
            return 0;
        }

        auto *tex2d = static_cast<Texture2D *>(texture.get());
        auto *pixels = static_cast<PixelBuffer2D *>(tex2d->getPixelBuffer());
        if (pixels == nullptr || tex2d->getWidth() == 0 || tex2d->getHeight() == 0)
        {
            T3D_LOG_WARNING(LOG_TAG_RMLUI, "LoadTexture engine texture has no GPU buffer %s", source.c_str());
            return 0;
        }

        texture_dimensions = Rml::Vector2i(static_cast<int>(tex2d->getWidth()), static_cast<int>(tex2d->getHeight()));
        if (mTextures.find(pixels) == mTextures.end())
        {
            mTextures.emplace(pixels, PixelBuffer2DPtr(pixels));
            mAssetTextures.emplace(pixels, texture);
        }
        return reinterpret_cast<Rml::TextureHandle>(pixels);
    }

    //--------------------------------------------------------------------------

    Rml::TextureHandle RmlRenderInterface::LoadTexture(Rml::Vector2i &texture_dimensions, const Rml::String &source)
    {
        texture_dimensions = Rml::Vector2i(0, 0);

        // 引擎纹理已经在 GPU 上，带压缩和 mip。不要再解码并预乘。
        if (endsWithIgnoreCase(source, ".ttexture") || endsWithIgnoreCase(source, ".ttex"))
        {
            return loadEngineTexture(texture_dimensions, source);
        }

        Archive *archive = T3D_ASSET_MGR.getArchive();
        if (archive == nullptr)
        {
            return 0;
        }

        TArray<uint8_t> bytes;
        struct Payload
        {
            TArray<uint8_t> *Bytes {nullptr};
            bool Ok {false};
        } payload;
        payload.Bytes = &bytes;

        TResult ret = archive->read(source, [](DataStream &stream, const String &, void *userData) -> TResult
        {
            auto *payload = static_cast<Payload *>(userData);
            const long_t length = stream.size();
            if (length <= 0)
            {
                return T3D_ERR_FAIL;
            }
            payload->Bytes->resize(static_cast<size_t>(length));
            const size_t got = stream.read(payload->Bytes->data(), payload->Bytes->size());
            payload->Ok = got == payload->Bytes->size();
            return payload->Ok ? T3D_OK : T3D_ERR_FAIL;
        }, &payload);

        if (ret != T3D_OK || !payload.Ok)
        {
            T3D_LOG_WARNING(LOG_TAG_RMLUI, "LoadTexture failed to read %s", source.c_str());
            return 0;
        }

        ImagePtr image = Image::create(source);
        if (image == nullptr)
        {
            return 0;
        }

        ret = T3D_IMAGE_CODEC.decode(bytes.data(), bytes.size(), *image);
        if (T3D_FAILED(ret) || image->getData() == nullptr || image->getWidth() == 0 || image->getHeight() == 0)
        {
            T3D_LOG_WARNING(LOG_TAG_RMLUI, "LoadTexture failed to decode %s", source.c_str());
            return 0;
        }

        const uint32_t width = image->getWidth();
        const uint32_t height = image->getHeight();
        const PixelFormat format = image->getFormat();
        uint32_t bpp = 0;
        if (format == PixelFormat::E_PF_R8G8B8)
        {
            bpp = 3;
        }
        else if (format == PixelFormat::E_PF_R8G8B8A8
            || format == PixelFormat::E_PF_B8G8R8A8
            || format == PixelFormat::E_PF_R8G8B8X8)
        {
            bpp = 4;
        }
        else
        {
            T3D_LOG_WARNING(LOG_TAG_RMLUI, "LoadTexture unsupported format %u for %s",
                static_cast<uint32_t>(format), source.c_str());
            return 0;
        }

        uint32_t pitch = image->getPitch();
        if (pitch == 0)
        {
            pitch = width * bpp;
        }

        TArray<uint8_t> rgba(static_cast<size_t>(width) * height * 4u);
        const uint8_t *src = image->getData();
        for (uint32_t y = 0; y < height; ++y)
        {
            const uint8_t *row = src + static_cast<size_t>(y) * pitch;
            uint8_t *dst = rgba.data() + static_cast<size_t>(y) * width * 4u;
            for (uint32_t x = 0; x < width; ++x)
            {
                const uint8_t *pixel = row + static_cast<size_t>(x) * bpp;
                uint8_t r = pixel[0];
                uint8_t g = pixel[1];
                uint8_t b = pixel[2];
                uint8_t a = 255;
                if (format == PixelFormat::E_PF_B8G8R8A8)
                {
                    b = pixel[0];
                    g = pixel[1];
                    r = pixel[2];
                    a = pixel[3];
                }
                else if (bpp == 4)
                {
                    a = pixel[3];
                }

                dst[0] = premultiplyChannel(r, a);
                dst[1] = premultiplyChannel(g, a);
                dst[2] = premultiplyChannel(b, a);
                dst[3] = a;
                dst += 4;
            }
        }

        texture_dimensions = Rml::Vector2i(static_cast<int>(width), static_cast<int>(height));
        return uploadRGBA(rgba.data(), static_cast<int32_t>(width), static_cast<int32_t>(height));
    }

    //--------------------------------------------------------------------------

    Rml::TextureHandle RmlRenderInterface::GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions)
    {
        const size_t bytes = static_cast<size_t>(source_dimensions.x) * static_cast<size_t>(source_dimensions.y) * 4u;
        if (source_dimensions.x <= 0 || source_dimensions.y <= 0 || source.size() < bytes)
        {
            return 0;
        }

        // GenerateTexture 的输入已经是预乘 RGBA8。
        return uploadRGBA(source.data(), source_dimensions.x, source_dimensions.y);
    }

    //--------------------------------------------------------------------------

    void RmlRenderInterface::ReleaseTexture(Rml::TextureHandle texture)
    {
        auto *pixels = reinterpret_cast<PixelBuffer2D *>(texture);
        mTextures.erase(pixels);
        mAssetTextures.erase(pixels);
    }

    //--------------------------------------------------------------------------

    PixelBuffer2D *RmlRenderInterface::textureFromHandle(Rml::TextureHandle texture) const
    {
        auto *pixels = reinterpret_cast<PixelBuffer2D *>(texture);
        return mTextures.find(pixels) != mTextures.end() ? pixels : nullptr;
    }

    //--------------------------------------------------------------------------

    void RmlRenderInterface::EnableScissorRegion(bool enable)
    {
        mScissor = enable;
        if (mCtx == nullptr)
        {
            return;
        }

        RasterizerState *state = enable ? mRasterizerScissor.get() : mRasterizerNoScissor.get();
        if (state != nullptr)
        {
            mCtx->setRasterizerState(state);
        }
    }

    //--------------------------------------------------------------------------

    void RmlRenderInterface::SetScissorRegion(Rml::Rectanglei region)
    {
        if (mCtx == nullptr)
        {
            return;
        }

        int32_t width = region.Width();
        int32_t height = region.Height();
        if (width < 0)
        {
            width = 0;
        }
        if (height < 0)
        {
            height = 0;
        }

        mCtx->setScissorRect(mOriginX + region.Left(), mOriginY + region.Top(),
            static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }

    //--------------------------------------------------------------------------

    bool RmlRenderInterface::ensureShaders()
    {
        if (mShadersReady)
        {
            return true;
        }

        if (mBlendState == nullptr)
        {
            BlendDesc blendDesc;
            blendDesc.AlphaToCoverageEnable = false;
            blendDesc.IndependentBlendEnable = false;
            blendDesc.RenderTargetStates[0].BlendEnable = true;
            blendDesc.RenderTargetStates[0].SrcBlend = BlendFactor::kOne;
            blendDesc.RenderTargetStates[0].DestBlend = BlendFactor::kOneMinusSrcAlpha;
            blendDesc.RenderTargetStates[0].BlendOp = BlendOperation::kAdd;
            blendDesc.RenderTargetStates[0].SrcBlendAlpha = BlendFactor::kOne;
            blendDesc.RenderTargetStates[0].DstBlendAlpha = BlendFactor::kOneMinusSrcAlpha;
            blendDesc.RenderTargetStates[0].BlendOpAlpha = BlendOperation::kAdd;
            blendDesc.RenderTargetStates[0].ColorMask = kWriteMaskAll;
            mBlendState = T3D_RENDER_STATE_MGR.loadBlendState(blendDesc);
            blendDesc.RenderTargetStates[0].ColorMask = kWriteMaskNone;
            mNoColorBlendState = T3D_RENDER_STATE_MGR.loadBlendState(blendDesc);

            DepthStencilDesc depthDesc;
            depthDesc.DepthTestEnable = false;
            depthDesc.DepthWriteEnable = false;
            depthDesc.StencilEnable = false;
            mDepthStencilState = T3D_RENDER_STATE_MGR.loadDepthStencilState(depthDesc);

            RasterizerDesc rasterDesc;
            rasterDesc.FillMode = PolygonMode::kSolid;
            rasterDesc.CullMode = CullingMode::kNone;
            rasterDesc.DepthClipEnable = false;
            rasterDesc.ScissorEnable = true;
            mRasterizerScissor = T3D_RENDER_STATE_MGR.loadRasterizerState(rasterDesc);
            rasterDesc.ScissorEnable = false;
            mRasterizerNoScissor = T3D_RENDER_STATE_MGR.loadRasterizerState(rasterDesc);

            SamplerDesc samplerDesc;
            samplerDesc.MinFilter = FilterOptions::kLinear;
            samplerDesc.MagFilter = FilterOptions::kLinear;
            samplerDesc.MipFilter = FilterOptions::kNone;
            samplerDesc.AddressU = TextureAddressMode::kClamp;
            samplerDesc.AddressV = TextureAddressMode::kClamp;
            samplerDesc.AddressW = TextureAddressMode::kClamp;
            mSampler = T3D_RENDER_STATE_MGR.loadSamplerState(samplerDesc);
        }

        if (mColorMaterial == nullptr || mTextureMaterial == nullptr)
        {
            ShaderPtr colorShader = T3D_ASSET_MGR.loadShader(kColorShader);
            ShaderPtr textureShader = T3D_ASSET_MGR.loadShader(kTextureShader);
            if (colorShader == nullptr || textureShader == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_RMLUI, "RmlUi shaders are not ready (%s, %s).", kColorShader, kTextureShader);
                return false;
            }

            mColorMaterial = T3D_MATERIAL_MGR.createMaterial("RmlUI-Color", colorShader.get());
            mTextureMaterial = T3D_MATERIAL_MGR.createMaterial("RmlUI-Texture", textureShader.get());
            if (mColorMaterial == nullptr || mTextureMaterial == nullptr)
            {
                return false;
            }
        }

        auto bindPass = [](Material *material, PassInstance *&pass) -> bool
        {
            TechniqueInstancePtr technique = material->getCurrentTechnique();
            if (technique == nullptr)
            {
                return false;
            }
            const auto &passes = technique->getPassInstances();
            const auto found = passes.find(ShaderLab::kBuiltinLightModeForwardBase);
            if (found == passes.end() || found->second == nullptr)
            {
                return false;
            }
            pass = found->second.get();
            return pass->getCurrentVertexShader() != nullptr;
        };

        if (!bindPass(mColorMaterial.get(), mColorPass) || !bindPass(mTextureMaterial.get(), mTexturePass))
        {
            T3D_LOG_ERROR(LOG_TAG_RMLUI, "RmlUi shader passes are not ready.");
            return false;
        }

        VertexAttributes attributes;
        attributes.push_back(VertexAttribute(0, static_cast<uint32_t>(offsetof(Rml::Vertex, position)),
            VertexAttribute::Type::E_VAT_FLOAT2, VertexAttribute::Semantic::E_VAS_POSITION, 0));
        attributes.push_back(VertexAttribute(0, static_cast<uint32_t>(offsetof(Rml::Vertex, colour)),
            VertexAttribute::Type::E_VAT_UBYTE4_NORM, VertexAttribute::Semantic::E_VAS_DIFFUSE, 0));
        attributes.push_back(VertexAttribute(0, static_cast<uint32_t>(offsetof(Rml::Vertex, tex_coord)),
            VertexAttribute::Type::E_VAT_FLOAT2, VertexAttribute::Semantic::E_VAS_TEXCOORD, 0));

        ShaderVariantInstance *vertexShader = mColorPass->getCurrentVertexShader();
        mVertexDecl = T3D_RENDER_BUFFER_MGR.addVertexDeclaration(attributes, vertexShader->getShaderVariant().get());
        if (mVertexDecl == nullptr)
        {
            return false;
        }

        mShadersReady = true;
        return true;
    }

    //--------------------------------------------------------------------------

    bool RmlRenderInterface::beginFrame(RHIContext *ctx, int32_t width, int32_t height, int32_t originX, int32_t originY,
        bool stencilAvailable)
    {
        mCtx = ctx;
        mWidth = width;
        mHeight = height;
        mOriginX = originX;
        mOriginY = originY;
        mStencilAvailable = stencilAvailable;
        mClipEnabled = false;
        mClipWrite = false;
        mClipRef = 0;
        mTransform = Matrix4::IDENTITY;
        if (ctx == nullptr || width <= 0 || height <= 0 || !ensureShaders())
        {
            return false;
        }

        ctx->setBlendState(mBlendState.get());
        ctx->setDepthStencilState(mDepthStencilState.get());
        ctx->setRasterizerState(mScissor ? mRasterizerScissor.get() : mRasterizerNoScissor.get());
        ctx->setPrimitiveType(PrimitiveType::kTriangleList);

        const Real widthF = static_cast<Real>(width);
        const Real heightF = static_cast<Real>(height);
        // 列向量，平移在最后一列。像素原点在视口左上，y 向下。
        // z 留在 OpenGL 的 [-1, 1]，D3D11 / Vulkan 的 setViewProjectionTransform 会再做深度重映射。
        // 平移若写到最后一行，w 会变成 1 - x + y，整块界面被透视除法拉成一条斜线。
        const Matrix4 ortho(
            Real(2) / widthF, 0, 0, Real(-1),
            0, Real(-2) / heightF, 0, Real(1),
            0, 0, Real(1), Real(0),
            0, 0, 0, Real(1));

        ctx->setViewProjectionTransform(Matrix4::IDENTITY, ortho);
        const Matrix4 &viewProjection = ctx->getProjViewMatrix();
        mColorMaterial->setMatrix("tiny3d_MatrixVP", viewProjection);
        mTextureMaterial->setMatrix("tiny3d_MatrixVP", viewProjection);
        return true;
    }

    //--------------------------------------------------------------------------

    void RmlRenderInterface::endFrame()
    {
        if (mCtx != nullptr)
        {
            const uint32_t width = mWidth > 0 ? static_cast<uint32_t>(mWidth) : 0;
            const uint32_t height = mHeight > 0 ? static_cast<uint32_t>(mHeight) : 0;
            mCtx->setScissorRect(mOriginX, mOriginY, width, height);
            if (mRasterizerNoScissor != nullptr)
            {
                mCtx->setRasterizerState(mRasterizerNoScissor.get());
            }
            if (mDepthStencilState != nullptr)
            {
                mCtx->setDepthStencilState(mDepthStencilState.get());
            }
        }

        mScissor = false;
        mClipEnabled = false;
        mClipWrite = false;
        mClipRef = 0;
        mTransform = Matrix4::IDENTITY;
        mCtx = nullptr;
    }

    //--------------------------------------------------------------------------

    void RmlRenderInterface::SetTransform(const Rml::Matrix4f *transform)
    {
        mTransform = transform != nullptr ? fromRmlMatrix(*transform) : Matrix4::IDENTITY;
    }

    //--------------------------------------------------------------------------

    void RmlRenderInterface::applyColorAndStencil()
    {
        if (mCtx == nullptr)
        {
            return;
        }

        if (mClipWrite)
        {
            if (mNoColorBlendState != nullptr)
            {
                mCtx->setBlendState(mNoColorBlendState.get());
            }
            return;
        }

        if (mBlendState != nullptr)
        {
            mCtx->setBlendState(mBlendState.get());
        }

        DepthStencilState *state = mDepthStencilState.get();
        if (mClipEnabled && mStencilAvailable)
        {
            state = stencilState(CompareFunction::kEqual, StencilOp::kKeep, mClipRef);
        }
        if (state != nullptr)
        {
            mCtx->setDepthStencilState(state);
        }
    }

    //--------------------------------------------------------------------------

    DepthStencilState *RmlRenderInterface::stencilState(CompareFunction func, StencilOp passOp, uint32_t ref)
    {
        const uint32_t key = (static_cast<uint32_t>(func) << 24)
            | (static_cast<uint32_t>(passOp) << 16)
            | (ref & 0xFFFFu);
        const auto found = mStencilStates.find(key);
        if (found != mStencilStates.end())
        {
            return found->second.get();
        }

        DepthStencilDesc desc;
        desc.DepthTestEnable = false;
        desc.DepthWriteEnable = false;
        desc.StencilEnable = true;
        desc.StencilRef = ref;
        desc.StencilReadMask = 0xFF;
        desc.StencilWriteMask = 0xFF;
        desc.FrontFace.StencilFunc = func;
        desc.FrontFace.StencilFailOp = StencilOp::kKeep;
        desc.FrontFace.StencilDepthFailOp = StencilOp::kKeep;
        desc.FrontFace.StencilPassOp = passOp;
        desc.BackFace = desc.FrontFace;

        DepthStencilStatePtr state = T3D_RENDER_STATE_MGR.loadDepthStencilState(desc);
        DepthStencilState *raw = state.get();
        mStencilStates.emplace(key, state);
        return raw;
    }

    //--------------------------------------------------------------------------

    void RmlRenderInterface::EnableClipMask(bool enable)
    {
        mClipEnabled = enable && mStencilAvailable;
        if (enable && !mStencilAvailable && !mStencilWarned)
        {
            mStencilWarned = true;
            T3D_LOG_WARNING(LOG_TAG_RMLUI,
                "Clip mask needs a stencil buffer on the current target. Clipping is disabled.");
        }

        if (mCtx == nullptr || mClipWrite)
        {
            return;
        }

        DepthStencilState *state = mDepthStencilState.get();
        if (mClipEnabled)
        {
            state = stencilState(CompareFunction::kEqual, StencilOp::kKeep, mClipRef);
        }
        if (state != nullptr)
        {
            mCtx->setDepthStencilState(state);
        }
    }

    //--------------------------------------------------------------------------

    void RmlRenderInterface::RenderToClipMask(Rml::ClipMaskOperation operation, Rml::CompiledGeometryHandle geometry,
        Rml::Vector2f translation)
    {
        if (!mStencilAvailable)
        {
            EnableClipMask(true);
            return;
        }

        uint32_t writeRef = 1;
        StencilOp passOp = StencilOp::kReplace;
        switch (operation)
        {
        case Rml::ClipMaskOperation::Set:
            if (mCtx != nullptr)
            {
                mCtx->clearStencil(0);
            }
            writeRef = 1;
            mClipRef = 1;
            passOp = StencilOp::kReplace;
            break;
        case Rml::ClipMaskOperation::SetInverse:
            if (mCtx != nullptr)
            {
                mCtx->clearStencil(1);
            }
            writeRef = 0;
            mClipRef = 1;
            passOp = StencilOp::kReplace;
            break;
        case Rml::ClipMaskOperation::Intersect:
            writeRef = mClipRef + 1;
            mClipRef = writeRef;
            passOp = StencilOp::kInc;
            break;
        }

        DepthStencilState *writeState = stencilState(CompareFunction::kAlwaysPass, passOp, writeRef);
        if (mCtx != nullptr && writeState != nullptr)
        {
            mCtx->setDepthStencilState(writeState);
        }

        mClipWrite = true;
        RenderGeometry(geometry, translation, 0);
        mClipWrite = false;
        mClipEnabled = true;
        applyColorAndStencil();
    }

    //--------------------------------------------------------------------------
}
