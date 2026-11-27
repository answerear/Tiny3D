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


#include "SoftParticleBehaviour.h"
#include "SoftParticleShaders.h"
#include "Kernel/T3DAgent.h"
#include "Kernel/T3DTime.h"
#include "RHI/T3DRHIContext.h"
#include "RHI/T3DRHICapabilities.h"
#include "RHI/T3DRHIRenderer.h"

#include <algorithm>
#include <cmath>
#include <utility>

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

        Real saturate(Real v)
        {
            return std::min(std::max(v, 0.0f), 1.0f);
        }

        Vector3 lerp(const Vector3 &a, const Vector3 &b, Real t)
        {
            return a + (b - a) * t;
        }

        Real hash2(int32_t x, int32_t y)
        {
            uint32_t h = static_cast<uint32_t>(x) * 374761393u + static_cast<uint32_t>(y) * 668265263u;
            h = (h ^ (h >> 13)) * 1274126177u;
            return static_cast<Real>((h ^ (h >> 16)) & 0xFFFFu) / 65535.0f;
        }

        const ColorRGB kSkyColor(0.03f, 0.04f, 0.08f);
        const Vector3 kFirePos(0.0f, 0.0f, 0.0f);
        const Vector3 kFireLightPos(0.0f, 0.7f, 0.0f);
        const Real kFireLightRange = 12.0f;
        const Vector3 kFireBaseColor(1.0f, 0.5f, 0.18f);
        const Real kFireIntensity = 3.2f;
        const Vector3 kAmbient(0.06f, 0.065f, 0.09f);
        const Vector3 kMoonColor(0.18f, 0.22f, 0.36f);

        const Vector3 kTentCenter(2.8f, 0.0f, 1.1f);
        const Real kTentYaw = 0.375f + Math::PI * 0.5f;

        const Real kSmokeSoftRange = 0.9f;
        const Real kFireSoftRange = 0.3f;
    }

    //--------------------------------------------------------------------------

    SoftParticleBehaviour::SoftParticleBehaviour(const UUID &uuid)
        : CameraBehaviour(uuid)
    {
    }

    //--------------------------------------------------------------------------

    SoftParticleBehaviour::~SoftParticleBehaviour()
    {
        teardown();
    }

    //--------------------------------------------------------------------------

    void SoftParticleBehaviour::onAwake()
    {
        mCamera = getGameObject()->getComponent<Camera>();

        // Start with a plume already standing instead of an empty fire pit
        mSmoke.resize(kSmokeCount);
        mFire.resize(kFireCount);
        for (Particle &p : mSmoke)
        {
            respawnSmoke(p);
            p.age = -p.life * random();
        }
        for (Particle &p : mFire)
        {
            respawnFire(p);
            p.age = -p.life * random();
        }
        for (uint32_t step = 0; step < 300; ++step)
        {
            simulate(1.0f / 30.0f);
        }
    }

    //--------------------------------------------------------------------------

    void SoftParticleBehaviour::onUpdate()
    {
        // Time::deltaTime() is milliseconds; a long hitch must not throw the particles away
        const Real dt = std::min(static_cast<Real>(Time::deltaTime()) / 1000.0f, 0.1f);
        mElapsed += dt;
        const Real t = mElapsed;

        // Orbit the camp; around t = 6 s and every lap after it the tent stands between
        // the camera and the fire
        const Real angle = -0.6f + t * 0.17f;
        const Real radius = 8.5f + 1.2f * std::sin(t * 0.29f);
        const Real height = 2.0f + 0.6f * std::sin(t * 0.21f + 0.8f);
        mEye = Vector3(radius * std::cos(angle), height, radius * std::sin(angle));
        mTarget = Vector3(0.6f + 0.5f * std::sin(t * 0.13f), 1.3f + 0.3f * std::sin(t * 0.19f), 0.5f + 0.5f * std::cos(t * 0.11f));
        if (mCamera != nullptr)
        {
            mCamera->lookAt(mEye, mTarget, Vector3::UP);
        }

        // Wind swings slowly between the tent and the trees on either side of it
        const Real windAngle = 0.45f + 1.2f * std::sin(t * 0.08f) + 0.25f * std::sin(t * 0.21f + 1.0f);
        const Real windSpeed = 0.8f + 0.25f * std::sin(t * 0.15f);
        mWind = Vector3(std::cos(windAngle), 0.0f, std::sin(windAngle)) * windSpeed;

        mFlicker = 1.0f + 0.12f * std::sin(t * 11.0f) + 0.08f * std::sin(t * 23.0f + 1.7f) + 0.05f * std::sin(t * 37.0f + 0.3f);

        simulate(dt);
    }

    //--------------------------------------------------------------------------

    void SoftParticleBehaviour::onDestroy()
    {
        teardown();
    }

    //--------------------------------------------------------------------------

    bool SoftParticleBehaviour::setup(RHIContext *ctx)
    {
        const RHICapabilities &caps = ctx->getCapabilities();
        const String &rendererName = T3D_AGENT.getActiveRHIRenderer()->getName();
        mGLFamily = (rendererName == RHIRenderer::OPENGL4);

        ShaderPtr dummy;
        mSceneVS = createSPShader(SPShader::kSceneVS, dummy);
        mDummyShaders.push_back(dummy);
        mScenePS = createSPShader(SPShader::kScenePS, dummy);
        mDummyShaders.push_back(dummy);
        mParticleVS = createSPShader(SPShader::kParticleVS, dummy);
        mDummyShaders.push_back(dummy);
        mParticlePS = createSPShader(SPShader::kParticlePS, dummy);
        mDummyShaders.push_back(dummy);
        if (mSceneVS == nullptr || mScenePS == nullptr || mParticleVS == nullptr || mParticlePS == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "SoftParticle : shaders unavailable on %s, only Direct3D11 and OpenGL 4 are supported.",
                rendererName.c_str());
            return false;
        }

        // The copy into the camera target needs matching size and format
        RenderTarget *dst = mCamera->getSrcRenderTarget();
        if (dst == nullptr)
        {
            dst = mCamera->getRenderTarget();
        }
        uint32_t width = 0;
        uint32_t height = 0;
        PixelFormat colorFormat = PixelFormat::E_PF_R8G8B8A8;
        if (dst != nullptr && dst->getType() == RenderTarget::Type::E_RT_TEXTURE && dst->getRenderTexture() != nullptr)
        {
            width = dst->getRenderTexture()->getWidth();
            height = dst->getRenderTexture()->getHeight();
            colorFormat = dst->getRenderTexture()->getPixelFormat();
        }
        else
        {
            const auto &desc = T3D_AGENT.getDefaultRenderWindow()->getDescriptor();
            width = desc.Width;
            height = desc.Height;
        }

        mSoftParticles = caps.supportsReadOnlyDepthStencil;
        if (!mSoftParticles)
        {
            T3D_LOG_WARNING(LOG_TAG_APP, "SoftParticle : %s has no read-only depth stencil views, the particles are drawn without soft edges.",
                rendererName.c_str());
        }

        mColorTex = T3D_TEXTURE_MGR.createRenderTexture("SPColor", width, height, colorFormat, 1, 1, 0, true);
        mDepthTex = T3D_TEXTURE_MGR.createRenderTexture("SPDepth", width, height, PixelFormat::E_PF_D24_UNORM_S8_UINT, 1, 1, 0, true);
        if (mColorTex == nullptr || mDepthTex == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "SoftParticle : failed to create the %ux%u color and depth render textures.", width, height);
            return false;
        }
        mOpaqueTarget = RenderTarget::create(mColorTex, mDepthTex);
        mParticleTarget = RenderTarget::create(mColorTex, mDepthTex);
        if (mOpaqueTarget == nullptr || mParticleTarget == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "SoftParticle : failed to create the render targets.");
            return false;
        }
        if (mSoftParticles)
        {
            mParticleTarget->setDepthStencilReadOnly(kReadOnlyDepthStencil);
        }
        VertexAttributes sceneAttribs;
        sceneAttribs.push_back(VertexAttribute(0, 0, VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_POSITION, 0));
        sceneAttribs.push_back(VertexAttribute(0, sizeof(Vector3), VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_NORMAL, 0));
        sceneAttribs.push_back(VertexAttribute(0, sizeof(Vector3) * 2, VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_DIFFUSE, 0));
        // D3D11 input layouts are validated against the bytecode they were created for
        mSceneDecl = T3D_RENDER_BUFFER_MGR.addVertexDeclaration(sceneAttribs, mSceneVS.get());

        VertexAttributes particleAttribs;
        particleAttribs.push_back(VertexAttribute(0, 0, VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_POSITION, 0));
        particleAttribs.push_back(VertexAttribute(0, sizeof(Vector3), VertexAttribute::Type::E_VAT_FLOAT4,
            VertexAttribute::Semantic::E_VAS_DIFFUSE, 0));
        particleAttribs.push_back(VertexAttribute(0, sizeof(Vector3) + sizeof(float) * 4, VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_TEXCOORD, 0));
        mParticleDecl = T3D_RENDER_BUFFER_MGR.addVertexDeclaration(particleAttribs, mParticleVS.get());

        TArray<SceneVertex> camp;
        buildCamp(camp);
        mCampVertexCount = static_cast<uint32_t>(camp.size());
        Buffer campData = wrapBytes(camp.data(), sizeof(SceneVertex) * camp.size());
        mCampVB = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(sizeof(SceneVertex), mCampVertexCount, campData,
            MemoryType::kBoth, Usage::kImmutable, kCPUNone);
        campData.Data = nullptr;

        Buffer particleData = allocBytes(sizeof(ParticleVertex) * kParticleVertexCount);
        mParticleVB = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(sizeof(ParticleVertex), kParticleVertexCount, particleData,
            MemoryType::kBoth, Usage::kDynamic, kCPUWrite);
        particleData.release();

        auto makeCB = [](const char *name, size_t bytes) -> ConstantBufferPtr
        {
            Buffer cb = allocBytes(bytes);
            ConstantBufferPtr ptr = T3D_RENDER_BUFFER_MGR.loadConstantBuffer(
                name, 0, cb, MemoryType::kBoth, Usage::kDynamic, kCPUWrite);
            cb.release();
            return ptr;
        };
        mSceneCB = makeCB(kSPSceneParamsName, sizeof(float) * 44);
        mParticleCB = makeCB(kSPParticleParamsName, sizeof(float) * 20);

        // Billboards and the tent ends are seen from both sides
        RasterizerDesc raster;
        raster.CullMode = CullingMode::kNone;
        mRasterizer = T3D_RENDER_STATE_MGR.loadRasterizerState(raster);

        DepthStencilDesc opaqueDepth;
        mOpaqueDepth = T3D_RENDER_STATE_MGR.loadDepthStencilState(opaqueDepth);
        // Particles still test against the camp so the tent hides the smoke behind it
        DepthStencilDesc particleDepth;
        particleDepth.DepthWriteEnable = false;
        mParticleDepth = T3D_RENDER_STATE_MGR.loadDepthStencilState(particleDepth);

        BlendDesc opaqueBlend;
        mOpaqueBlend = T3D_RENDER_STATE_MGR.loadBlendState(opaqueBlend);
        BlendDesc smokeBlend;
        smokeBlend.RenderTargetStates[0].BlendEnable = true;
        smokeBlend.RenderTargetStates[0].SrcBlend = BlendFactor::kSrcAlpha;
        smokeBlend.RenderTargetStates[0].DestBlend = BlendFactor::kOneMinusSrcAlpha;
        smokeBlend.RenderTargetStates[0].SrcBlendAlpha = BlendFactor::kZero;
        smokeBlend.RenderTargetStates[0].DstBlendAlpha = BlendFactor::kOne;
        mSmokeBlend = T3D_RENDER_STATE_MGR.loadBlendState(smokeBlend);
        BlendDesc fireBlend = smokeBlend;
        fireBlend.RenderTargetStates[0].DestBlend = BlendFactor::kOne;
        mFireBlend = T3D_RENDER_STATE_MGR.loadBlendState(fireBlend);

        SamplerDesc sampler;
        sampler.MinFilter = FilterOptions::kPoint;
        sampler.MagFilter = FilterOptions::kPoint;
        sampler.MipFilter = FilterOptions::kPoint;
        sampler.AddressU = TextureAddressMode::kClamp;
        sampler.AddressV = TextureAddressMode::kClamp;
        sampler.AddressW = TextureAddressMode::kClamp;
        mPointSampler = T3D_RENDER_STATE_MGR.loadSamplerState(sampler);

        const bool ok = (mSceneDecl != nullptr && mParticleDecl != nullptr && mCampVB != nullptr && mParticleVB != nullptr
            && mSceneCB != nullptr && mParticleCB != nullptr && mRasterizer != nullptr && mOpaqueDepth != nullptr
            && mParticleDepth != nullptr && mOpaqueBlend != nullptr && mSmokeBlend != nullptr && mFireBlend != nullptr
            && mPointSampler != nullptr);
        if (ok)
        {
            T3D_LOG_INFO(LOG_TAG_APP, "SoftParticle : %ux%u %s color, D24S8 depth, %s.", width, height,
                PixelFormatUtil::getName(colorFormat),
                mSoftParticles ? "particle pass samples the depth through a read-only depth stencil view" : "hard particles");
        }
        return ok;
    }

    //--------------------------------------------------------------------------

    void SoftParticleBehaviour::teardown()
    {
        mReady = false;
        mParticleTarget = nullptr;
        mOpaqueTarget = nullptr;
        if (mColorTex != nullptr)
        {
            T3D_TEXTURE_MGR.unload(mColorTex);
            mColorTex = nullptr;
        }
        if (mDepthTex != nullptr)
        {
            T3D_TEXTURE_MGR.unload(mDepthTex);
            mDepthTex = nullptr;
        }
        mPointSampler = nullptr;
        mFireBlend = nullptr;
        mSmokeBlend = nullptr;
        mOpaqueBlend = nullptr;
        mParticleDepth = nullptr;
        mOpaqueDepth = nullptr;
        mRasterizer = nullptr;
        mParticleCB = nullptr;
        mSceneCB = nullptr;
        mParticleVB = nullptr;
        mCampVB = nullptr;
        mParticleDecl = nullptr;
        mSceneDecl = nullptr;
        mParticlePS = nullptr;
        mParticleVS = nullptr;
        mScenePS = nullptr;
        mSceneVS = nullptr;
        mDummyShaders.clear();
    }

    //--------------------------------------------------------------------------

    void SoftParticleBehaviour::buildCamp(TArray<SceneVertex> &vertices) const
    {
        // Normals face away from 'inside', which is enough for the convex pieces built here
        auto addTri = [&vertices](const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &color, const Vector3 &inside)
        {
            Vector3 n = (b - a).cross(c - a);
            n.normalize();
            if (n.dot((a + b + c) / 3.0f - inside) < 0.0f)
            {
                n = -n;
            }
            vertices.push_back({ a, n, color });
            vertices.push_back({ b, n, color });
            vertices.push_back({ c, n, color });
        };
        auto addQuad = [&addTri](const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d, const Vector3 &color, const Vector3 &inside)
        {
            addTri(a, b, c, color, inside);
            addTri(a, c, d, color, inside);
        };
        // Square beam from p0 to p1, for logs and stones
        auto addBeam = [&addQuad](const Vector3 &p0, const Vector3 &p1, Real thickness, const Vector3 &color)
        {
            Vector3 d = p1 - p0;
            d.normalize();
            const Vector3 ref = (std::abs(d.y()) < 0.9f) ? Vector3::UP : Vector3(1.0f, 0.0f, 0.0f);
            Vector3 s = ref.cross(d);
            s.normalize();
            Vector3 u = d.cross(s);
            u.normalize();
            s = s * (thickness * 0.5f);
            u = u * (thickness * 0.5f);
            const Vector3 c[8] =
            {
                p0 - s - u, p0 + s - u, p0 + s + u, p0 - s + u,
                p1 - s - u, p1 + s - u, p1 + s + u, p1 - s + u,
            };
            const Vector3 mid = (p0 + p1) * 0.5f;
            addQuad(c[0], c[1], c[2], c[3], color, mid);
            addQuad(c[4], c[5], c[6], c[7], color, mid);
            addQuad(c[0], c[1], c[5], c[4], color, mid);
            addQuad(c[1], c[2], c[6], c[5], color, mid);
            addQuad(c[2], c[3], c[7], c[6], color, mid);
            addQuad(c[3], c[0], c[4], c[7], color, mid);
        };
        auto addTree = [&addQuad, &addTri](const Vector3 &base, Real trunkRadius, Real scale)
        {
            const uint32_t segments = 10;
            const Vector3 bark(0.24f, 0.16f, 0.1f);
            const Vector3 needles(0.07f, 0.15f, 0.08f);
            const Real trunkHeight = 3.6f * scale;
            auto ring = [&base](Real radius, Real y, uint32_t i) -> Vector3
            {
                const Real a = Math::TWO_PI * static_cast<Real>(i) / 10.0f;
                return base + Vector3(radius * std::cos(a), y, radius * std::sin(a));
            };
            for (uint32_t i = 0; i < segments; ++i)
            {
                const Vector3 axis = base + Vector3(0.0f, trunkHeight * 0.5f, 0.0f);
                addQuad(ring(trunkRadius, 0.0f, i), ring(trunkRadius, 0.0f, i + 1),
                    ring(trunkRadius * 0.7f, trunkHeight, i + 1), ring(trunkRadius * 0.7f, trunkHeight, i), bark, axis);
            }
            const Real cones[2][3] = { { 2.8f, 1.5f, 5.4f }, { 4.1f, 1.1f, 6.4f } };
            for (const auto &cone : cones)
            {
                const Real y0 = cone[0] * scale;
                const Real r = cone[1] * scale;
                const Vector3 tip = base + Vector3(0.0f, cone[2] * scale, 0.0f);
                const Vector3 inside = base + Vector3(0.0f, y0 + (cone[2] * scale - y0) * 0.25f, 0.0f);
                const Vector3 center = base + Vector3(0.0f, y0, 0.0f);
                for (uint32_t i = 0; i < segments; ++i)
                {
                    addTri(ring(r, y0, i), ring(r, y0, i + 1), tip, needles, inside);
                    addTri(ring(r, y0, i), ring(r, y0, i + 1), center, needles * 0.7f, center + Vector3::UP);
                }
            }
        };

        // Ground: grass and dirt patches, ash around the fire pit
        const int32_t half = 30;
        auto groundColor = [](int32_t x, int32_t z) -> Vector3
        {
            const Vector3 grass(0.16f, 0.2f, 0.1f);
            const Vector3 dirt(0.24f, 0.19f, 0.13f);
            const Vector3 ash(0.12f, 0.11f, 0.1f);
            Vector3 c = lerp(grass, dirt, hash2(x, z) * 0.6f + hash2(x / 3, z / 3) * 0.4f);
            const Real r = std::sqrt(static_cast<Real>(x * x + z * z)) * 0.5f;
            return lerp(ash, c, saturate(r - 0.6f));
        };
        for (int32_t z = -half * 2; z < half * 2; ++z)
        {
            for (int32_t x = -half * 2; x < half * 2; ++x)
            {
                const Vector3 p00(x * 0.5f, 0.0f, z * 0.5f);
                const Vector3 p10((x + 1) * 0.5f, 0.0f, z * 0.5f);
                const Vector3 p11((x + 1) * 0.5f, 0.0f, (z + 1) * 0.5f);
                const Vector3 p01(x * 0.5f, 0.0f, (z + 1) * 0.5f);
                const Vector3 c00 = groundColor(x, z), c10 = groundColor(x + 1, z);
                const Vector3 c11 = groundColor(x + 1, z + 1), c01 = groundColor(x, z + 1);
                for (const auto &v : { std::make_pair(p00, c00), std::make_pair(p10, c10), std::make_pair(p11, c11),
                    std::make_pair(p00, c00), std::make_pair(p11, c11), std::make_pair(p01, c01) })
                {
                    vertices.push_back({ v.first, Vector3::UP, v.second });
                }
            }
        }

        // Tent: a triangular prism with its ridge across the line to the fire
        {
            const Real w = 1.05f;
            const Real h = 1.7f;
            const Real l = 1.3f;
            const Real cs = std::cos(kTentYaw);
            const Real sn = std::sin(kTentYaw);
            auto local = [&](Real x, Real y, Real z) -> Vector3
            {
                return kTentCenter + Vector3(x * cs - z * sn, y, x * sn + z * cs);
            };
            const Vector3 a0 = local(-w, 0.0f, -l), b0 = local(w, 0.0f, -l), c0 = local(0.0f, h, -l);
            const Vector3 a1 = local(-w, 0.0f, l), b1 = local(w, 0.0f, l), c1 = local(0.0f, h, l);
            const Vector3 inside = kTentCenter + Vector3(0.0f, h / 3.0f, 0.0f);
            const Vector3 canvas(0.5f, 0.4f, 0.24f);
            addQuad(a0, c0, c1, a1, canvas, inside);
            addQuad(b0, c0, c1, b1, canvas, inside);
            addTri(a0, b0, c0, canvas * 0.9f, inside);
            addTri(a1, b1, c1, canvas * 0.9f, inside);
            // Ridge pole sticking out at both ends
            addBeam(local(0.0f, h, -l - 0.15f), local(0.0f, h, l + 0.15f), 0.06f, Vector3(0.3f, 0.22f, 0.14f));
        }

        // Pines close enough for the smoke to drift through their trunks, and a far ring
        const Real nearTrees[][3] = { { 1.3f, 3.4f, 1.0f }, { -0.3f, 4.0f, 1.1f }, { 2.5f, 4.6f, 0.9f },
            { 3.7f, 5.0f, 1.2f }, { 5.0f, 4.4f, 1.0f } };
        for (const auto &tree : nearTrees)
        {
            addTree(Vector3(tree[1] * std::cos(tree[0]), 0.0f, tree[1] * std::sin(tree[0])), 0.22f * tree[2], tree[2]);
        }
        for (uint32_t i = 0; i < 14; ++i)
        {
            const Real a = Math::TWO_PI * (static_cast<Real>(i) + 0.3f * hash2(i, 7)) / 14.0f;
            const Real r = 11.0f + 4.0f * hash2(i, 3);
            const Real s = 1.0f + 0.5f * hash2(i, 11);
            addTree(Vector3(r * std::cos(a), 0.0f, r * std::sin(a)), 0.25f * s, s);
        }

        // Fire pit: stone ring, a teepee of logs and glowing embers
        for (uint32_t i = 0; i < 10; ++i)
        {
            const Real a = Math::TWO_PI * static_cast<Real>(i) / 10.0f;
            const Vector3 dir(std::cos(a), 0.0f, std::sin(a));
            const Vector3 p = kFirePos + dir * 0.8f;
            const Vector3 side(-dir.z(), 0.0f, dir.x());
            addBeam(p - side * 0.12f + Vector3(0.0f, 0.07f, 0.0f), p + side * 0.12f + Vector3(0.0f, 0.07f, 0.0f),
                0.16f + 0.04f * hash2(i, 5), Vector3(0.34f, 0.33f, 0.31f));
        }
        for (uint32_t i = 0; i < 5; ++i)
        {
            const Real a = Math::TWO_PI * static_cast<Real>(i) / 5.0f + 0.3f;
            const Vector3 foot = kFirePos + Vector3(0.45f * std::cos(a), 0.05f, 0.45f * std::sin(a));
            addBeam(foot, kFirePos + Vector3(0.0f, 0.75f, 0.0f), 0.09f, Vector3(0.3f, 0.2f, 0.12f));
        }
        {
            const Vector3 ember(0.85f, 0.28f, 0.05f);
            const Vector3 center = kFirePos + Vector3(0.0f, 0.02f, 0.0f);
            for (uint32_t i = 0; i < 12; ++i)
            {
                const Real a0 = Math::TWO_PI * static_cast<Real>(i) / 12.0f;
                const Real a1 = Math::TWO_PI * static_cast<Real>(i + 1) / 12.0f;
                vertices.push_back({ center, Vector3::ZERO, ember * 1.3f });
                vertices.push_back({ center + Vector3(0.42f * std::cos(a0), 0.0f, 0.42f * std::sin(a0)), Vector3::ZERO, ember * 0.5f });
                vertices.push_back({ center + Vector3(0.42f * std::cos(a1), 0.0f, 0.42f * std::sin(a1)), Vector3::ZERO, ember * 0.5f });
            }
        }

        // Log seats near the fire
        addBeam(Vector3(-1.9f, 0.18f, -0.9f), Vector3(-1.2f, 0.18f, -2.0f), 0.34f, Vector3(0.28f, 0.19f, 0.12f));
        addBeam(Vector3(-1.6f, 0.18f, 1.4f), Vector3(-0.5f, 0.18f, 2.0f), 0.34f, Vector3(0.28f, 0.19f, 0.12f));
    }

    //--------------------------------------------------------------------------

    Real SoftParticleBehaviour::random()
    {
        mRandomState ^= mRandomState << 13;
        mRandomState ^= mRandomState >> 17;
        mRandomState ^= mRandomState << 5;
        return static_cast<Real>(mRandomState & 0xFFFFFFu) / static_cast<Real>(0xFFFFFFu);
    }

    //--------------------------------------------------------------------------

    void SoftParticleBehaviour::respawnSmoke(Particle &p)
    {
        const Real a = Math::TWO_PI * random();
        const Real r = 0.25f * std::sqrt(random());
        p.position = kFirePos + Vector3(r * std::cos(a), 0.45f + 0.1f * random(), r * std::sin(a));
        p.velocity = Vector3(0.0f, 0.5f + 0.2f * random(), 0.0f);
        p.age = 0.0f;
        p.life = 6.0f + 2.5f * random();
        p.seed = 100.0f * random();
    }

    //--------------------------------------------------------------------------

    void SoftParticleBehaviour::respawnFire(Particle &p)
    {
        const Real a = Math::TWO_PI * random();
        const Real r = 0.32f * std::sqrt(random());
        p.position = kFirePos + Vector3(r * std::cos(a), 0.08f, r * std::sin(a));
        p.velocity = Vector3(0.0f, 1.0f + 0.8f * random(), 0.0f);
        p.age = 0.0f;
        p.life = 0.45f + 0.45f * random();
        p.seed = 100.0f * random();
    }

    //--------------------------------------------------------------------------

    void SoftParticleBehaviour::simulate(Real dt)
    {
        mSimTime += dt;
        const Real t = mSimTime;

        for (Particle &p : mSmoke)
        {
            p.age += dt;
            if (p.age < 0.0f)
            {
                continue;
            }
            if (p.age >= p.life)
            {
                respawnSmoke(p);
            }
            // Wind takes hold once the smoke leaves the shelter of the fire pit; buoyancy fades with age
            const Real x = p.age / p.life;
            const Vector3 turbulence(std::sin(p.seed + t * 1.3f), 0.4f * std::sin(p.seed * 1.7f + t * 0.9f), std::cos(p.seed * 2.3f + t * 1.1f));
            const Vector3 target = mWind * std::min(1.0f, 0.35f + p.position.y() * 0.5f)
                + Vector3(0.0f, 0.42f * (1.0f - 0.5f * x), 0.0f) + turbulence * 0.18f;
            p.velocity = p.velocity + (target - p.velocity) * std::min(1.0f, dt * 1.2f);
            p.position = p.position + p.velocity * dt;
        }

        for (Particle &p : mFire)
        {
            p.age += dt;
            if (p.age < 0.0f)
            {
                continue;
            }
            if (p.age >= p.life)
            {
                respawnFire(p);
            }
            // Flames lean with the wind, speed up as they rise and gather towards the tip
            const Vector3 sway(std::sin(p.seed + t * 7.0f), 0.0f, std::cos(p.seed * 1.3f + t * 6.0f));
            const Vector3 horizontal(p.velocity.x(), 0.0f, p.velocity.z());
            p.velocity = p.velocity + (mWind * 0.35f + sway * 0.2f - horizontal) * std::min(1.0f, dt * 4.0f)
                + Vector3(0.0f, 0.6f * dt, 0.0f);
            Vector3 pos = p.position + p.velocity * dt;
            const Real pull = std::max(0.0f, 1.0f - dt * 0.8f);
            pos = Vector3(kFirePos.x() + (pos.x() - kFirePos.x()) * pull, pos.y(), kFirePos.z() + (pos.z() - kFirePos.z()) * pull);
            p.position = pos;
        }
    }

    //--------------------------------------------------------------------------

    Vector3 SoftParticleBehaviour::fireLightColor() const
    {
        return kFireBaseColor * (kFireIntensity * mFlicker);
    }

    //--------------------------------------------------------------------------

    void SoftParticleBehaviour::buildParticles(TArray<ParticleVertex> &vertices, uint32_t &smokeVertexCount) const
    {
        Vector3 forward = mTarget - mEye;
        forward.normalize();
        Vector3 right = Vector3::UP.cross(forward);
        right.normalize();
        Vector3 up = forward.cross(right);
        up.normalize();

        auto addQuad = [&](const Vector3 &center, Real halfSize, const Vector3 &color, Real alpha, Real softRange)
        {
            const Vector3 r = right * halfSize;
            const Vector3 u = up * halfSize;
            const Vector3 corners[4] = { center - r - u, center + r - u, center + r + u, center - r + u };
            const Real uvs[4][2] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
            const int order[6] = { 0, 1, 2, 0, 2, 3 };
            for (int idx : order)
            {
                ParticleVertex v;
                v.position = corners[idx];
                v.color[0] = color.x();
                v.color[1] = color.y();
                v.color[2] = color.z();
                v.color[3] = alpha;
                v.params = Vector3(uvs[idx][0], uvs[idx][1], softRange);
                vertices.push_back(v);
            }
        };

        // Alpha blended smoke back to front
        TArray<const Particle *> order;
        order.reserve(mSmoke.size());
        for (const Particle &p : mSmoke)
        {
            if (p.age > 0.0f)
            {
                order.push_back(&p);
            }
        }
        std::sort(order.begin(), order.end(), [&](const Particle *a, const Particle *b)
        {
            return (a->position - mEye).dot(forward) > (b->position - mEye).dot(forward);
        });

        Vector3 moonDir(-0.35f, 0.8f, -0.5f);
        moonDir.normalize();
        const Vector3 fireColor = fireLightColor();
        const Vector3 smokeBase(0.6f, 0.6f, 0.62f);
        for (const Particle *p : order)
        {
            const Real x = p->age / p->life;
            const Real dist = (p->position - kFireLightPos).length();
            Real atten = saturate(1.0f - dist / kFireLightRange);
            atten *= atten * atten;
            const Vector3 light = kAmbient + kMoonColor * 1.4f + fireColor * (atten * 0.35f);
            const Vector3 color(smokeBase.x() * light.x(), smokeBase.y() * light.y(), smokeBase.z() * light.z());
            const Real alpha = 0.38f * saturate(p->age / 0.8f) * std::pow(1.0f - x, 1.3f);
            addQuad(p->position, 0.3f + 1.2f * std::sqrt(x), color, alpha, kSmokeSoftRange);
        }
        smokeVertexCount = static_cast<uint32_t>(vertices.size());

        // Additive flames, order free
        const Vector3 yellow(1.0f, 0.82f, 0.42f);
        const Vector3 orange(1.0f, 0.42f, 0.08f);
        const Vector3 red(0.55f, 0.08f, 0.02f);
        for (const Particle &p : mFire)
        {
            if (p.age <= 0.0f)
            {
                continue;
            }
            const Real x = p.age / p.life;
            const Vector3 color = (x < 0.5f ? lerp(yellow, orange, x * 2.0f) : lerp(orange, red, x * 2.0f - 1.0f)) * (1.6f * mFlicker);
            const Real alpha = saturate(p.age / 0.08f) * (1.0f - x) * 0.85f;
            addQuad(p.position, 0.32f * (1.0f - 0.65f * x), color, alpha, kFireSoftRange);
        }
    }

    //--------------------------------------------------------------------------

    TResult SoftParticleBehaviour::uploadCb(ConstantBuffer *cb, const void *data, size_t bytes)
    {
        Buffer buf = wrapBytes(const_cast<void *>(data), bytes);
        const size_t written = cb->writeData(0, buf, true);
        buf.Data = nullptr;        return (written == bytes) ? T3D_OK : T3D_ERR_FAIL;
    }

    //--------------------------------------------------------------------------

    void SoftParticleBehaviour::packMatrix(float *dst, const Matrix4 &m) const
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

    void SoftParticleBehaviour::renderOpaque(RHIContext *ctx)
    {
        ctx->setRenderTarget(mOpaqueTarget.get());
        ctx->setViewport(Viewport {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f});
        ctx->setViewProjectionTransform(mCamera->getViewMatrix(), mCamera->getProjectionMatrix());

        Vector3 moonDir(-0.35f, 0.8f, -0.5f);
        moonDir.normalize();
        const Vector3 fireColor = fireLightColor();
        alignas(16) float data[44];
        packMatrix(data, ctx->getProjViewMatrix());
        const float vectors[7][4] =
        {
            { kFireLightPos.x(), kFireLightPos.y(), kFireLightPos.z(), kFireLightRange },
            { fireColor.x(), fireColor.y(), fireColor.z(), 0.0f },
            { moonDir.x(), moonDir.y(), moonDir.z(), 0.0f },
            { kMoonColor.x(), kMoonColor.y(), kMoonColor.z(), 0.0f },
            { kAmbient.x(), kAmbient.y(), kAmbient.z(), 0.0f },
            { mEye.x(), mEye.y(), mEye.z(), 0.035f },
            { kSkyColor.red(), kSkyColor.green(), kSkyColor.blue(), 0.0f },
        };
        memcpy(data + 16, vectors, sizeof(vectors));
        uploadCb(mSceneCB.get(), data, sizeof(data));

        ctx->clearColor(kSkyColor);
        ctx->clearDepthStencil(1.0f, 0);
        ctx->beginPass();

        ctx->setRasterizerState(mRasterizer.get());
        ctx->setDepthStencilState(mOpaqueDepth.get());
        ctx->setBlendState(mOpaqueBlend.get());
        ctx->setPrimitiveType(PrimitiveType::kTriangleList);

        ConstantBuffers cbs;
        cbs.push_back(mSceneCB);
        ctx->setVSConstantBuffers(0, cbs);
        ctx->setPSConstantBuffers(0, cbs);
        ctx->setVertexShader(mSceneVS.get());
        ctx->setPixelShader(mScenePS.get());

        ctx->setVertexDeclaration(mSceneDecl.get());
        VertexBuffers vbs;
        vbs.push_back(mCampVB);
        VertexStrides strides;
        strides.push_back(sizeof(SceneVertex));
        VertexOffsets offsets;
        offsets.push_back(0);
        ctx->setVertexBuffers(0, vbs, strides, offsets);
        ctx->render(mCampVertexCount, 0);

        ctx->endPass();
        ctx->reset();
    }

    //--------------------------------------------------------------------------

    void SoftParticleBehaviour::renderParticles(RHIContext *ctx)
    {
        TArray<ParticleVertex> vertices;
        vertices.reserve(kParticleVertexCount);
        uint32_t smokeVertexCount = 0;
        buildParticles(vertices, smokeVertexCount);
        const uint32_t vertexCount = static_cast<uint32_t>(vertices.size());
        if (vertexCount == 0)
        {
            return;
        }
        Buffer particleData = wrapBytes(vertices.data(), sizeof(ParticleVertex) * vertices.size());
        mParticleVB->writeData(0, particleData, true);
        particleData.Data = nullptr;

        // Same textures as the opaque pass; with soft particles the depth stencil is read-only,
        // which is what lets the depth texture be sampled while it is still depth tested against
        ctx->setRenderTarget(mParticleTarget.get());
        ctx->setViewport(Viewport {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f});
        ctx->setViewProjectionTransform(mCamera->getViewMatrix(), mCamera->getProjectionMatrix());

        alignas(16) float data[20];
        packMatrix(data, ctx->getProjViewMatrix());
        data[16] = mCamera->getNearPlaneDistance();
        data[17] = mCamera->getFarPlaneDistance();
        data[18] = mSoftParticles ? 1.0f : 0.0f;
        data[19] = 0.4f;
        uploadCb(mParticleCB.get(), data, sizeof(data));

        ctx->beginPass();

        ctx->setRasterizerState(mRasterizer.get());
        ctx->setDepthStencilState(mParticleDepth.get());
        ctx->setBlendState(mSmokeBlend.get());
        ctx->setPrimitiveType(PrimitiveType::kTriangleList);

        ConstantBuffers cbs;
        cbs.push_back(mParticleCB);
        ctx->setVSConstantBuffers(0, cbs);
        ctx->setPSConstantBuffers(0, cbs);
        ctx->setVertexShader(mParticleVS.get());
        ctx->setPixelShader(mParticlePS.get());

        if (mSoftParticles)
        {
            PixelBuffers srvs;
            srvs.push_back(mDepthTex->getPixelBuffer());
            ctx->setPSPixelBuffers(0, srvs);
            Samplers samplers;
            samplers.push_back(mPointSampler);
            ctx->setPSSamplers(0, samplers);
        }

        ctx->setVertexDeclaration(mParticleDecl.get());
        VertexBuffers vbs;
        vbs.push_back(mParticleVB);
        VertexStrides strides;
        strides.push_back(sizeof(ParticleVertex));
        VertexOffsets offsets;
        offsets.push_back(0);
        ctx->setVertexBuffers(0, vbs, strides, offsets);
        if (smokeVertexCount > 0)
        {
            ctx->render(smokeVertexCount, 0);
        }
        if (vertexCount > smokeVertexCount)
        {
            ctx->setBlendState(mFireBlend.get());
            ctx->render(vertexCount - smokeVertexCount, smokeVertexCount);
        }

        // Next frame's opaque pass writes this depth again, it must not stay bound for sampling
        if (mSoftParticles)
        {
            PixelBuffers none;
            none.push_back(nullptr);
            ctx->setPSPixelBuffers(0, none);
        }

        ctx->endPass();
        ctx->reset();
    }

    //--------------------------------------------------------------------------

    void SoftParticleBehaviour::onPostRender(RHIContext *ctx)
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

        renderOpaque(ctx);
        renderParticles(ctx);

        RenderTarget *dst = mCamera->getSrcRenderTarget();
        if (dst == nullptr)
        {
            dst = mCamera->getRenderTarget();
        }
        ctx->blit(mOpaqueTarget.get(), dst);
    }
}
