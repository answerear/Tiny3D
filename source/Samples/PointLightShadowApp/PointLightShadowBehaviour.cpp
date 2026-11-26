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


#include "PointLightShadowBehaviour.h"
#include "PointLightShadowShaders.h"
#include "Kernel/T3DAgent.h"
#include "Kernel/T3DTime.h"
#include "RHI/T3DRHIContext.h"
#include "RHI/T3DRHICapabilities.h"
#include "RHI/T3DRHIRenderer.h"

#include <cmath>

#define LOG_TAG_APP     "APP"


namespace Tiny3D
{
    namespace
    {
        Buffer wrapBytes(void *data, size_t bytes)
        {
            Buffer buf;
            buf.Data = static_cast<uint8_t *>(data);
            buf.DataSize = bytes;
            return buf;
        }

        Buffer allocBytes(size_t bytes)
        {
            Buffer buf;
            buf.DataSize = bytes;
            buf.Data = T3D_POD_NEW_ARRAY(uint8_t, bytes);
            memset(buf.Data, 0, bytes);
            return buf;
        }

        /// Face order and orientation of D3D / GL cube maps in a left-handed world
        struct CubeFace
        {
            Vector3 dir;
            Vector3 up;
        };

        const CubeFace kCubeFaces[6] =
        {
            { Vector3( 1, 0, 0), Vector3(0, 1, 0) },
            { Vector3(-1, 0, 0), Vector3(0, 1, 0) },
            { Vector3( 0, 1, 0), Vector3(0, 0,-1) },
            { Vector3( 0,-1, 0), Vector3(0, 0, 1) },
            { Vector3( 0, 0, 1), Vector3(0, 1, 0) },
            { Vector3( 0, 0,-1), Vector3(0, 1, 0) },
        };

        const Real kRoomHalf = 6.0f;
        const Real kRoomHeight = 6.0f;
        const Real kShadowNear = 0.05f;
    }

    //--------------------------------------------------------------------------

    PointLightShadowBehaviour::PointLightShadowBehaviour(const UUID &uuid)
        : CameraBehaviour(uuid)
    {
    }

    //--------------------------------------------------------------------------

    PointLightShadowBehaviour::~PointLightShadowBehaviour()
    {
        teardown();
    }

    //--------------------------------------------------------------------------

    void PointLightShadowBehaviour::onAwake()
    {
        mCamera = getGameObject()->getComponent<Camera>();
    }

    //--------------------------------------------------------------------------

    void PointLightShadowBehaviour::onUpdate()
    {
        // Time::deltaTime() is milliseconds
        mElapsed += static_cast<Real>(Time::deltaTime()) / 1000.0f;
        const Real a = mElapsed * 0.6f;
        mLightPos = Vector3(2.6f * std::cos(a), 2.4f + 0.9f * std::sin(a * 1.7f), 2.6f * std::sin(a));
    }

    //--------------------------------------------------------------------------

    void PointLightShadowBehaviour::onDestroy()
    {
        teardown();
    }

    //--------------------------------------------------------------------------

    bool PointLightShadowBehaviour::setup(RHIContext *ctx)
    {
        const RHICapabilities &caps = ctx->getCapabilities();
        const String &rendererName = T3D_AGENT.getActiveRHIRenderer()->getName();
        mGLFamily = (rendererName == RHIRenderer::OPENGL4);

        PixelFormat distFormat = PixelFormat::E_PF_UNKNOWN;
        const uint32_t needs = kFormatSupportRenderTarget | kFormatSupportLinearFilter | kFormatSupportTexture;
        if (caps.supportsFormat(PixelFormat::E_PF_R32G32_FLOAT, needs))
        {
            distFormat = PixelFormat::E_PF_R32G32_FLOAT;
        }
        else if (caps.supportsFormat(PixelFormat::E_PF_R16G16_FLOAT, needs))
        {
            distFormat = PixelFormat::E_PF_R16G16_FLOAT;
        }
        if (distFormat == PixelFormat::E_PF_UNKNOWN)
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "PointLightShadow : no filterable two-channel float render target format.");
            return false;
        }

        ShaderPtr dummy;
        mShadowVS = createPLShader(PLShader::kShadowVS, dummy);
        mDummyShaders.push_back(dummy);
        mShadowPS = createPLShader(PLShader::kShadowPS, dummy);
        mDummyShaders.push_back(dummy);
        mSceneVS = createPLShader(PLShader::kSceneVS, dummy);
        mDummyShaders.push_back(dummy);
        mScenePS = createPLShader(PLShader::kScenePS, dummy);
        mDummyShaders.push_back(dummy);
        if (mShadowVS == nullptr || mShadowPS == nullptr || mSceneVS == nullptr || mScenePS == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "PointLightShadow : shaders unavailable on %s, only Direct3D11 and OpenGL 4 are supported.",
                rendererName.c_str());
            return false;
        }

        // Full mip chain filtered on the GPU every frame; the depth cube only serves as the
        // per-face depth attachment and is never sampled
        mDistanceCube = T3D_TEXTURE_MGR.createRenderTexture("PLDistanceCube", TEXTURE_TYPE::TT_CUBE,
            kShadowSize, kShadowSize, 1, distFormat, 0, true, kCPUNone, kGPUGenerateMips);
        mDepthCube = T3D_TEXTURE_MGR.createRenderTexture("PLDepthCube", TEXTURE_TYPE::TT_CUBE,
            kShadowSize, kShadowSize, 1, PixelFormat::E_PF_D24_UNORM_S8_UINT, 1, false);
        if (mDistanceCube == nullptr || mDepthCube == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "PointLightShadow : failed to create the shadow cube render textures.");
            return false;
        }

        for (uint32_t face = 0; face < 6; ++face)
        {
            mFaceTargets[face] = RenderTarget::create(mDistanceCube, TextureSubresource::slice(face),
                mDepthCube, TextureSubresource::slice(face));
            if (mFaceTargets[face] == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_APP, "PointLightShadow : failed to create render target for cube face %u.", face);
                return false;
            }
        }

        VertexAttributes attribs;
        attribs.push_back(VertexAttribute(0, 0, VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_POSITION, 0));
        attribs.push_back(VertexAttribute(0, sizeof(Vector3), VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_NORMAL, 0));
        attribs.push_back(VertexAttribute(0, sizeof(Vector3) * 2, VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_DIFFUSE, 0));
        // D3D11 input layouts are validated against the bytecode they were created for
        mShadowDecl = T3D_RENDER_BUFFER_MGR.addVertexDeclaration(attribs, mShadowVS.get());
        mSceneDecl = T3D_RENDER_BUFFER_MGR.addVertexDeclaration(attribs, mSceneVS.get());

        TArray<Vertex> room;
        buildRoom(room);
        mRoomVertexCount = static_cast<uint32_t>(room.size());
        Buffer roomData = wrapBytes(room.data(), sizeof(Vertex) * room.size());
        mRoomVB = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(sizeof(Vertex), mRoomVertexCount, roomData,
            MemoryType::kBoth, Usage::kImmutable, kCPUNone);
        roomData.Data = nullptr;

        Buffer markerData = allocBytes(sizeof(Vertex) * kMarkerVertexCount);
        mMarkerVB = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(sizeof(Vertex), kMarkerVertexCount, markerData,
            MemoryType::kBoth, Usage::kDynamic, kCPUWrite);
        markerData.release();

        auto makeCB = [](const char *name, size_t bytes) -> ConstantBufferPtr
        {
            Buffer cb = allocBytes(bytes);
            ConstantBufferPtr ptr = T3D_RENDER_BUFFER_MGR.loadConstantBuffer(
                name, 0, cb, MemoryType::kBoth, Usage::kDynamic, kCPUWrite);
            cb.release();
            return ptr;
        };
        mShadowCB = makeCB(kPLShadowParamsName, sizeof(float) * 20);
        mSceneCB = makeCB(kPLSceneParamsName, sizeof(float) * 28);

        // Cube faces seen from the inside flip the winding, culling none keeps both passes simple
        RasterizerDesc raster;
        raster.CullMode = CullingMode::kNone;
        mRasterizer = T3D_RENDER_STATE_MGR.loadRasterizerState(raster);
        DepthStencilDesc depth;
        mDepthState = T3D_RENDER_STATE_MGR.loadDepthStencilState(depth);
        BlendDesc blend;
        mBlendState = T3D_RENDER_STATE_MGR.loadBlendState(blend);
        SamplerDesc sampler;
        sampler.AddressU = TextureAddressMode::kClamp;
        sampler.AddressV = TextureAddressMode::kClamp;
        sampler.AddressW = TextureAddressMode::kClamp;
        mShadowSampler = T3D_RENDER_STATE_MGR.loadSamplerState(sampler);

        const bool ok = (mShadowDecl != nullptr && mSceneDecl != nullptr && mRoomVB != nullptr && mMarkerVB != nullptr
            && mShadowCB != nullptr && mSceneCB != nullptr && mRasterizer != nullptr && mDepthState != nullptr
            && mBlendState != nullptr && mShadowSampler != nullptr);
        if (ok)
        {
            T3D_LOG_INFO(LOG_TAG_APP, "PointLightShadow : %ux%u distance cube in %s with %u mips, regenerated by generateMips every frame.",
                kShadowSize, kShadowSize, PixelFormatUtil::getName(distFormat), mDistanceCube->getMipmaps());
        }
        return ok;
    }

    //--------------------------------------------------------------------------

    void PointLightShadowBehaviour::teardown()
    {
        mReady = false;
        for (RenderTargetPtr &rt : mFaceTargets)
        {
            rt = nullptr;
        }
        if (mDistanceCube != nullptr)
        {
            T3D_TEXTURE_MGR.unload(mDistanceCube);
            mDistanceCube = nullptr;
        }
        if (mDepthCube != nullptr)
        {
            T3D_TEXTURE_MGR.unload(mDepthCube);
            mDepthCube = nullptr;
        }
        mShadowSampler = nullptr;
        mBlendState = nullptr;
        mDepthState = nullptr;
        mRasterizer = nullptr;
        mSceneCB = nullptr;
        mShadowCB = nullptr;
        mMarkerVB = nullptr;
        mRoomVB = nullptr;
        mSceneDecl = nullptr;
        mShadowDecl = nullptr;
        mScenePS = nullptr;
        mSceneVS = nullptr;
        mShadowPS = nullptr;
        mShadowVS = nullptr;
        mDummyShaders.clear();
    }

    //--------------------------------------------------------------------------

    void PointLightShadowBehaviour::buildRoom(TArray<Vertex> &vertices) const
    {
        // Axis aligned box; inward == true turns the normals to the inside for the room shell
        auto addBox = [&vertices](const Vector3 &minPos, const Vector3 &maxPos, const Vector3 &color, bool inward)
        {
            const Vector3 c[8] =
            {
                Vector3(minPos.x(), minPos.y(), minPos.z()), Vector3(maxPos.x(), minPos.y(), minPos.z()),
                Vector3(maxPos.x(), maxPos.y(), minPos.z()), Vector3(minPos.x(), maxPos.y(), minPos.z()),
                Vector3(minPos.x(), minPos.y(), maxPos.z()), Vector3(maxPos.x(), minPos.y(), maxPos.z()),
                Vector3(maxPos.x(), maxPos.y(), maxPos.z()), Vector3(minPos.x(), maxPos.y(), maxPos.z()),
            };
            struct Quad { int a, b, cc, d; Vector3 n; };
            const Quad quads[6] =
            {
                { 0, 1, 2, 3, Vector3( 0, 0,-1) },
                { 5, 4, 7, 6, Vector3( 0, 0, 1) },
                { 4, 0, 3, 7, Vector3(-1, 0, 0) },
                { 1, 5, 6, 2, Vector3( 1, 0, 0) },
                { 3, 2, 6, 7, Vector3( 0, 1, 0) },
                { 4, 5, 1, 0, Vector3( 0,-1, 0) },
            };
            for (const Quad &q : quads)
            {
                const Vector3 n = inward ? -q.n : q.n;
                const int order[6] = { q.a, q.b, q.cc, q.a, q.cc, q.d };
                for (int idx : order)
                {
                    vertices.push_back({ c[idx], n, color });
                }
            }
        };

        // Room shell, the side walls slightly tinted so the shadow directions read clearly
        addBox(Vector3(-kRoomHalf, 0.0f, -kRoomHalf), Vector3(kRoomHalf, kRoomHeight, kRoomHalf), Vector3(0.8f, 0.78f, 0.74f), true);

        // Pillars and crates around the light orbit
        addBox(Vector3(-0.35f, 0.0f, -0.35f), Vector3(0.35f, 4.2f, 0.35f), Vector3(0.85f, 0.4f, 0.3f), false);
        addBox(Vector3( 3.4f, 0.0f, -0.4f), Vector3( 4.2f, 3.0f, 0.4f), Vector3(0.3f, 0.6f, 0.85f), false);
        addBox(Vector3(-4.2f, 0.0f, 1.6f), Vector3(-3.0f, 1.2f, 2.8f), Vector3(0.4f, 0.8f, 0.35f), false);
        addBox(Vector3( 1.2f, 0.0f, 3.2f), Vector3( 2.4f, 1.6f, 4.4f), Vector3(0.85f, 0.75f, 0.3f), false);
        addBox(Vector3(-2.6f, 0.0f, -4.0f), Vector3(-1.4f, 2.2f, -2.8f), Vector3(0.7f, 0.45f, 0.8f), false);
        addBox(Vector3( 1.8f, 1.8f, -2.6f), Vector3( 2.8f, 2.1f, -1.6f), Vector3(0.9f, 0.9f, 0.9f), false);
    }

    //--------------------------------------------------------------------------

    void PointLightShadowBehaviour::buildMarker(TArray<Vertex> &vertices) const
    {
        // Small unlit UV sphere at the light; zero normals mark it as emissive in the scene shader.
        // Quads touching the poles collapse to triangles, so every band emits the same vertex count.
        const Real radius = 0.15f;
        const Vector3 color(1.0f, 0.92f, 0.7f);
        auto point = [&](uint32_t stack, uint32_t slice) -> Vector3
        {
            const Real theta = Math::PI * static_cast<Real>(stack) / static_cast<Real>(kMarkerStacks);
            const Real phi = Math::TWO_PI * static_cast<Real>(slice) / static_cast<Real>(kMarkerSlices);
            const Real r = radius * std::sin(theta);
            return mLightPos + Vector3(r * std::cos(phi), radius * std::cos(theta), r * std::sin(phi));
        };
        for (uint32_t stack = 0; stack < kMarkerStacks; ++stack)
        {
            for (uint32_t slice = 0; slice < kMarkerSlices; ++slice)
            {
                const Vector3 a = point(stack, slice);
                const Vector3 b = point(stack, slice + 1);
                const Vector3 c = point(stack + 1, slice + 1);
                const Vector3 d = point(stack + 1, slice);
                for (const Vector3 &p : { a, b, c, a, c, d })
                {
                    vertices.push_back({ p, Vector3::ZERO, color });
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    TResult PointLightShadowBehaviour::uploadCb(ConstantBuffer *cb, const void *data, size_t bytes)
    {
        Buffer buf = wrapBytes(const_cast<void *>(data), bytes);
        const size_t written = cb->writeData(0, buf, true);
        buf.Data = nullptr;
        return (written == bytes) ? T3D_OK : T3D_ERR_FAIL;
    }

    //--------------------------------------------------------------------------

    void PointLightShadowBehaviour::packMatrix(float *dst, const Matrix4 &m) const
    {
        // HLSL reads row_major with mul(M, v), GLSL reads column-major with M * v
        const Matrix4 packed = mGLFamily ? m.transpose() : m;
        for (int r = 0; r < 4; ++r)
        {
            for (int c = 0; c < 4; ++c)
            {
                dst[r * 4 + c] = packed[r][c];
            }
        }
    }

    //--------------------------------------------------------------------------

    void PointLightShadowBehaviour::bindGeometry(RHIContext *ctx, VertexDeclaration *decl, VertexBuffer *vb)
    {
        ctx->setVertexDeclaration(decl);
        VertexBuffers vbs;
        vbs.push_back(vb);
        VertexStrides strides;
        strides.push_back(sizeof(Vertex));
        VertexOffsets offsets;
        offsets.push_back(0);
        ctx->setVertexBuffers(0, vbs, strides, offsets);
    }

    //--------------------------------------------------------------------------

    void PointLightShadowBehaviour::renderShadowCube(RHIContext *ctx)
    {
        Matrix4 proj;
        proj.perspective_LH(Radian(Math::PI * 0.5f), 1.0f, kShadowNear, mLightRange);

        ConstantBuffers cbs;
        cbs.push_back(mShadowCB);

        for (uint32_t face = 0; face < 6; ++face)
        {
            ctx->setRenderTarget(mFaceTargets[face].get());
            ctx->setViewport(Viewport {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f});

            // The backend remaps depth and flips Y for render textures, so the faces come out
            // in the same memory layout on D3D11 and GL
            Matrix4 view;
            view.lookAt_LH(mLightPos, mLightPos + kCubeFaces[face].dir, kCubeFaces[face].up);
            ctx->setViewProjectionTransform(view, proj);

            alignas(16) float data[20];
            packMatrix(data, ctx->getProjViewMatrix());
            data[16] = mLightPos.x();
            data[17] = mLightPos.y();
            data[18] = mLightPos.z();
            data[19] = mLightRange;
            uploadCb(mShadowCB.get(), data, sizeof(data));

            // Nothing in range casts beyond the far plane, so (1, 1) never shadows
            ctx->clearColor(ColorRGB(1.0f, 1.0f, 1.0f));
            ctx->clearDepthStencil(1.0f, 0);
            ctx->beginPass();

            ctx->setRasterizerState(mRasterizer.get());
            ctx->setDepthStencilState(mDepthState.get());
            ctx->setBlendState(mBlendState.get());
            ctx->setPrimitiveType(PrimitiveType::kTriangleList);
            ctx->setVSConstantBuffers(0, cbs);
            ctx->setPSConstantBuffers(0, cbs);
            ctx->setVertexShader(mShadowVS.get());
            ctx->setPixelShader(mShadowPS.get());
            bindGeometry(ctx, mShadowDecl.get(), mRoomVB.get());
            ctx->render(mRoomVertexCount, 0);

            ctx->endPass();
            ctx->reset();
        }

        mDistanceCube->generateMips();
    }

    //--------------------------------------------------------------------------

    void PointLightShadowBehaviour::renderScene(RHIContext *ctx)
    {
        RenderTarget *rt = mCamera->getSrcRenderTarget();
        if (rt == nullptr)
        {
            rt = mCamera->getRenderTarget();
        }
        ctx->setRenderTarget(rt);
        ctx->setViewport(mCamera->getViewport());
        ctx->setViewProjectionTransform(mCamera->getViewMatrix(), mCamera->getProjectionMatrix());

        alignas(16) float data[28];
        packMatrix(data, ctx->getProjViewMatrix());
        data[16] = mLightPos.x();
        data[17] = mLightPos.y();
        data[18] = mLightPos.z();
        data[19] = mLightRange;
        data[20] = 1.0f;
        data[21] = 0.92f;
        data[22] = 0.78f;
        data[23] = 1.0f;
        data[24] = mShadowMip;
        data[25] = 0.00002f;
        data[26] = 0.3f;
        data[27] = 1.0f;
        uploadCb(mSceneCB.get(), data, sizeof(data));

        TArray<Vertex> marker;
        buildMarker(marker);
        Buffer markerData = wrapBytes(marker.data(), sizeof(Vertex) * marker.size());
        mMarkerVB->writeData(0, markerData, true);
        markerData.Data = nullptr;

        ctx->clearDepthStencil(1.0f, 0);
        ctx->beginPass();

        ctx->setRasterizerState(mRasterizer.get());
        ctx->setDepthStencilState(mDepthState.get());
        ctx->setBlendState(mBlendState.get());
        ctx->setPrimitiveType(PrimitiveType::kTriangleList);

        ConstantBuffers cbs;
        cbs.push_back(mSceneCB);
        ctx->setVSConstantBuffers(0, cbs);
        ctx->setPSConstantBuffers(0, cbs);
        ctx->setVertexShader(mSceneVS.get());
        ctx->setPixelShader(mScenePS.get());

        PixelBuffers srvs;
        srvs.push_back(mDistanceCube->getPixelBuffer());
        ctx->setPSPixelBuffers(0, srvs);
        Samplers samplers;
        samplers.push_back(mShadowSampler);
        ctx->setPSSamplers(0, samplers);

        bindGeometry(ctx, mSceneDecl.get(), mRoomVB.get());
        ctx->render(mRoomVertexCount, 0);
        bindGeometry(ctx, mSceneDecl.get(), mMarkerVB.get());
        ctx->render(kMarkerVertexCount, 0);

        // Next frame writes the cube as render target again, do not leave it bound for sampling
        PixelBuffers none;
        none.push_back(nullptr);
        ctx->setPSPixelBuffers(0, none);

        ctx->endPass();
        ctx->reset();
    }

    //--------------------------------------------------------------------------

    void PointLightShadowBehaviour::onPostRender(RHIContext *ctx)
    {
        if (!mInitialized)
        {
            mInitialized = true;
            mReady = (mCamera != nullptr) && setup(ctx);
            if (!mReady)
            {
                teardown();
            }
        }
        if (!mReady)
        {
            return;
        }

        renderShadowCube(ctx);
        renderScene(ctx);
    }
}
