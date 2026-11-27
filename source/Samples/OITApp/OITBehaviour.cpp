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


#include "OITBehaviour.h"
#include "OITShaders.h"
#include "Kernel/T3DAgent.h"
#include "Kernel/T3DTime.h"
#include "RHI/T3DRHIContext.h"
#include "RHI/T3DRHICapabilities.h"
#include "RHI/T3DRHIRenderer.h"

#include <algorithm>
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

        Real saturate(Real v)
        {
            return std::min(std::max(v, 0.0f), 1.0f);
        }

        Real smoothstep(Real e0, Real e1, Real x)
        {
            const Real t = saturate((x - e0) / (e1 - e0));
            return t * t * (3.0f - 2.0f * t);
        }

        Real hash1(int32_t x)
        {
            uint32_t h = static_cast<uint32_t>(x) * 374761393u + 668265263u;
            h = (h ^ (h >> 13)) * 1274126177u;
            return static_cast<Real>((h ^ (h >> 16)) & 0xFFFFu) / 65535.0f;
        }

        Vector3 catmullRom(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, Real t)
        {
            const Real t2 = t * t;
            const Real t3 = t2 * t;
            return (p1 * 2.0f + (p2 - p0) * t + (p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3) * t2
                + (p1 * 3.0f - p0 - p2 * 3.0f + p3) * t3) * 0.5f;
        }

        const ColorRGB kClearColor(0.02f, 0.02f, 0.025f);
        const Vector3 kAmbient(0.12f, 0.1f, 0.09f);

        const Vector3 kLampPos[3] = { { -3.4f, 2.55f, 1.6f }, { -0.75f, 2.55f, 1.6f }, { 1.9f, 2.55f, 1.6f } };
        const Vector3 kLampColor(1.8f, 1.3f, 0.75f);
        const Real kLampRange = 5.5f;
        const Vector3 kCaseLightPos(6.3f, 3.0f, -0.3f);
        const Vector3 kCaseLightColor(1.4f, 1.45f, 1.6f);
        const Real kCaseLightRange = 4.5f;

        /// Window opening in the back wall (z = kBackWallZ)
        const Real kBackWallZ = 4.5f;
        const Real kWindowX0 = -3.2f, kWindowX1 = 3.2f, kWindowY0 = 1.3f, kWindowY1 = 3.3f;

        /// Display case: plinth below kCaseGlassY0, glass box up to kCaseGlassY1
        const Real kCaseX0 = 5.6f, kCaseX1 = 7.0f, kCaseZ0 = -0.65f, kCaseZ1 = 0.65f;
        const Real kCaseGlassY0 = 0.85f, kCaseGlassY1 = 2.25f;
        const Vector3 kCaseCenter(6.3f, 1.55f, 0.0f);
        /// Opacity of the case glass seen from afar, and the camera distances it fades between
        const Real kCaseGlassAlpha = 0.5f;
        const Real kCaseFadeNear = 1.7f, kCaseFadeFar = 3.0f;

        /// Camera path: eye and look-at pairs, looped with Catmull-Rom
        struct Waypoint
        {
            Vector3 eye;
            Vector3 lookAt;
        };
        const Waypoint kPath[] =
        {
            { { -6.5f, 1.7f, -4.5f }, { -1.0f, 1.6f, 3.0f } },      // whole bar and window
            { { -2.5f, 1.6f, -2.5f }, { 0.0f, 1.9f, 4.5f } },       // towards the window
            { { 0.5f, 1.5f, -1.0f }, { 0.0f, 2.1f, 4.5f } },        // window fills the view
            { { 3.0f, 1.6f, -2.0f }, { 6.3f, 1.4f, 0.0f } },        // case seen from afar
            { { 4.6f, 1.5f, -1.4f }, { 6.3f, 1.4f, 0.0f } },        // walking up to the case
            { { 5.2f, 1.45f, -1.05f }, { 6.3f, 1.3f, 0.0f } },      // right at the glass
            { { 4.0f, 1.7f, -3.5f }, { 4.0f, 1.4f, 2.0f } },        // leaving
            { { -1.0f, 1.8f, -4.5f }, { -2.0f, 1.6f, 3.0f } },
        };
        const uint32_t kPathCount = sizeof(kPath) / sizeof(kPath[0]);
        const Real kSegmentTime = 4.5f;

        const Vector3 kWood(0.32f, 0.17f, 0.08f);
        const Vector3 kWoodTop(0.42f, 0.23f, 0.11f);
        const Vector3 kDarkWood(0.2f, 0.11f, 0.06f);
        const Vector3 kBrass(0.7f, 0.55f, 0.22f);
        const Vector3 kPlaster(0.4f, 0.2f, 0.16f);
    }

    //--------------------------------------------------------------------------

    OITBehaviour::OITBehaviour(const UUID &uuid)
        : CameraBehaviour(uuid)
    {
    }

    //--------------------------------------------------------------------------

    OITBehaviour::~OITBehaviour()
    {
        teardown();
    }

    //--------------------------------------------------------------------------

    void OITBehaviour::onAwake()
    {
        mCamera = getGameObject()->getComponent<Camera>();
    }

    //--------------------------------------------------------------------------

    void OITBehaviour::onUpdate()
    {
        // Time::deltaTime() is milliseconds
        const Real dt = std::min(static_cast<Real>(Time::deltaTime()) / 1000.0f, 0.1f);
        mElapsed += dt;

        const Real u = std::fmod(mElapsed / kSegmentTime, static_cast<Real>(kPathCount));
        const uint32_t i = static_cast<uint32_t>(u);
        const Real f = u - static_cast<Real>(i);
        const Waypoint &w0 = kPath[(i + kPathCount - 1) % kPathCount];
        const Waypoint &w1 = kPath[i % kPathCount];
        const Waypoint &w2 = kPath[(i + 1) % kPathCount];
        const Waypoint &w3 = kPath[(i + 2) % kPathCount];
        mEye = catmullRom(w0.eye, w1.eye, w2.eye, w3.eye, f);
        mLookAt = catmullRom(w0.lookAt, w1.lookAt, w2.lookAt, w3.lookAt, f);
        if (mCamera != nullptr)
        {
            mCamera->lookAt(mEye, mLookAt, Vector3::UP);
        }
    }

    //--------------------------------------------------------------------------

    void OITBehaviour::onDestroy()
    {
        teardown();
    }

    //--------------------------------------------------------------------------

    bool OITBehaviour::setup(RHIContext *ctx)
    {
        const RHICapabilities &caps = ctx->getCapabilities();
        const String &rendererName = T3D_AGENT.getActiveRHIRenderer()->getName();
        mGLFamily = (rendererName == RHIRenderer::OPENGL4);

        mDualSource = caps.supportsDualSourceBlend;
        if (!mDualSource)
        {
            T3D_LOG_WARNING(LOG_TAG_APP, "OIT : %s has no dual source blending, the stained glass only multiplies its transmittance into the scene and loses the light it scatters.",
                rendererName.c_str());
        }

        ShaderPtr dummy;
        mSceneVS = createOITShader(OITShader::kSceneVS, dummy);
        mDummyShaders.push_back(dummy);
        mScenePS = createOITShader(OITShader::kScenePS, dummy);
        mDummyShaders.push_back(dummy);
        mWindowPS = createOITShader(mDualSource ? OITShader::kWindowPS : OITShader::kWindowFallbackPS, dummy);
        mDummyShaders.push_back(dummy);
        mCaseGlassPS = createOITShader(OITShader::kCaseGlassPS, dummy);
        mDummyShaders.push_back(dummy);
        if (mSceneVS == nullptr || mScenePS == nullptr || mWindowPS == nullptr || mCaseGlassPS == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "OIT : shaders unavailable on %s, only Direct3D11 and OpenGL 4 are supported.",
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

        mColorTex = T3D_TEXTURE_MGR.createRenderTexture("OITColor", width, height, colorFormat, 1, 1, 0, true);
        mDepthTex = T3D_TEXTURE_MGR.createRenderTexture("OITDepth", width, height, PixelFormat::E_PF_D24_UNORM_S8_UINT, 1, 1, 0, true);
        if (mColorTex == nullptr || mDepthTex == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "OIT : failed to create the %ux%u color and depth render textures.", width, height);
            return false;
        }
        mTarget = RenderTarget::create(mColorTex, mDepthTex);
        if (mTarget == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "OIT : failed to create the render target.");
            return false;
        }

        VertexAttributes attribs;
        attribs.push_back(VertexAttribute(0, 0, VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_POSITION, 0));
        attribs.push_back(VertexAttribute(0, sizeof(Vector3), VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_NORMAL, 0));
        attribs.push_back(VertexAttribute(0, sizeof(Vector3) * 2, VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_DIFFUSE, 0));
        // D3D11 input layouts are validated against the bytecode they were created for
        mSceneDecl = T3D_RENDER_BUFFER_MGR.addVertexDeclaration(attribs, mSceneVS.get());

        TArray<SceneVertex> opaque;
        TArray<SceneVertex> window;
        TArray<SceneVertex> caseGlass;
        buildBar(opaque, window, caseGlass);
        mOpaqueVertexCount = static_cast<uint32_t>(opaque.size());
        mWindowVertexCount = static_cast<uint32_t>(window.size());
        mCaseGlassVertexCount = static_cast<uint32_t>(caseGlass.size());
        mOpaqueVB = createVB(opaque);
        mWindowVB = createVB(window);
        mCaseGlassVB = createVB(caseGlass);

        Buffer cb = allocBytes(sizeof(float) * 60);
        mSceneCB = T3D_RENDER_BUFFER_MGR.loadConstantBuffer(kOITSceneParamsName, 0, cb, MemoryType::kBoth, Usage::kDynamic, kCPUWrite);
        cb.release();

        // Glass panes and the lamp shades are seen from both sides
        RasterizerDesc raster;
        raster.CullMode = CullingMode::kNone;
        mRasterizer = T3D_RENDER_STATE_MGR.loadRasterizerState(raster);

        DepthStencilDesc opaqueDepth;
        mOpaqueDepth = T3D_RENDER_STATE_MGR.loadDepthStencilState(opaqueDepth);
        DepthStencilDesc glassDepth;
        glassDepth.DepthWriteEnable = false;
        mGlassDepth = T3D_RENDER_STATE_MGR.loadDepthStencilState(glassDepth);

        BlendDesc opaqueBlend;
        mOpaqueBlend = T3D_RENDER_STATE_MGR.loadBlendState(opaqueBlend);

        BlendDesc windowBlend;
        BlendDesc::RTBlendDesc &win = windowBlend.RenderTargetStates[0];
        win.BlendEnable = true;
        if (mDualSource)
        {
            // dst = scatter + dst * transmittance, per channel
            win.SrcBlend = BlendFactor::kOne;
            win.DestBlend = BlendFactor::kSrc1Color;
        }
        else
        {
            win.SrcBlend = BlendFactor::kZero;
            win.DestBlend = BlendFactor::kSrcColor;
        }
        win.SrcBlendAlpha = BlendFactor::kZero;
        win.DstBlendAlpha = BlendFactor::kOne;
        mWindowBlend = T3D_RENDER_STATE_MGR.loadBlendState(windowBlend);

        // dst = glass * c + dst * (1 - c); c comes from setBlendConstant every frame
        BlendDesc caseBlend;
        BlendDesc::RTBlendDesc &glass = caseBlend.RenderTargetStates[0];
        glass.BlendEnable = true;
        glass.SrcBlend = BlendFactor::kBlendConstant;
        glass.DestBlend = BlendFactor::kOneMinusBlendConstant;
        glass.SrcBlendAlpha = BlendFactor::kZero;
        glass.DstBlendAlpha = BlendFactor::kOne;
        mCaseGlassBlend = T3D_RENDER_STATE_MGR.loadBlendState(caseBlend);

        const bool ok = (mSceneDecl != nullptr && mOpaqueVB != nullptr && mWindowVB != nullptr && mCaseGlassVB != nullptr
            && mSceneCB != nullptr && mRasterizer != nullptr && mOpaqueDepth != nullptr && mGlassDepth != nullptr
            && mOpaqueBlend != nullptr && mWindowBlend != nullptr && mCaseGlassBlend != nullptr);
        if (ok)
        {
            T3D_LOG_INFO(LOG_TAG_APP, "OIT : %ux%u %s, %u opaque / %u stained glass / %u display case vertices, stained glass uses %s, display case glass fades with the blend constant.",
                width, height, PixelFormatUtil::getName(colorFormat), mOpaqueVertexCount, mWindowVertexCount, mCaseGlassVertexCount,
                mDualSource ? "dual source blending (One, Src1Color)" : "multiply blending (Zero, SrcColor)");
        }
        else
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "OIT : failed to create buffers or render states.");
        }
        return ok;
    }

    //--------------------------------------------------------------------------

    void OITBehaviour::teardown()
    {
        mReady = false;
        mTarget = nullptr;
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
        mCaseGlassBlend = nullptr;
        mWindowBlend = nullptr;
        mOpaqueBlend = nullptr;
        mGlassDepth = nullptr;
        mOpaqueDepth = nullptr;
        mRasterizer = nullptr;
        mSceneCB = nullptr;
        mCaseGlassVB = nullptr;
        mWindowVB = nullptr;
        mOpaqueVB = nullptr;
        mSceneDecl = nullptr;
        mCaseGlassPS = nullptr;
        mWindowPS = nullptr;
        mScenePS = nullptr;
        mSceneVS = nullptr;
        mDummyShaders.clear();
    }

    //--------------------------------------------------------------------------

    VertexBufferPtr OITBehaviour::createVB(const TArray<SceneVertex> &vertices) const
    {
        Buffer data = wrapBytes(const_cast<SceneVertex *>(vertices.data()), sizeof(SceneVertex) * vertices.size());
        VertexBufferPtr vb = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(sizeof(SceneVertex), static_cast<uint32_t>(vertices.size()),
            data, MemoryType::kBoth, Usage::kImmutable, kCPUNone);
        data.Data = nullptr;
        return vb;
    }

    //--------------------------------------------------------------------------

    void OITBehaviour::buildBar(TArray<SceneVertex> &opaque, TArray<SceneVertex> &window, TArray<SceneVertex> &caseGlass) const
    {
        auto addQuad = [](TArray<SceneVertex> &out, const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d,
            const Vector3 &n, const Vector3 &color)
        {
            for (const Vector3 *p : { &a, &b, &c, &a, &c, &d })
            {
                out.push_back({ *p, n, color });
            }
        };
        // Axis aligned box; 'unlit' boxes get no normal and a fixed brightness per face
        auto addBox = [&addQuad](TArray<SceneVertex> &out, const Vector3 &lo, const Vector3 &hi, const Vector3 &color, bool unlit)
        {
            const Real x0 = lo.x(), y0 = lo.y(), z0 = lo.z(), x1 = hi.x(), y1 = hi.y(), z1 = hi.z();
            auto n = [unlit](Real x, Real y, Real z) { return unlit ? Vector3::ZERO : Vector3(x, y, z); };
            auto c = [unlit, &color](Real shade) { return unlit ? color * shade : color; };
            addQuad(out, { x0, y0, z0 }, { x1, y0, z0 }, { x1, y1, z0 }, { x0, y1, z0 }, n(0, 0, -1), c(0.85f));
            addQuad(out, { x1, y0, z1 }, { x0, y0, z1 }, { x0, y1, z1 }, { x1, y1, z1 }, n(0, 0, 1), c(0.85f));
            addQuad(out, { x0, y0, z1 }, { x0, y0, z0 }, { x0, y1, z0 }, { x0, y1, z1 }, n(-1, 0, 0), c(0.7f));
            addQuad(out, { x1, y0, z0 }, { x1, y0, z1 }, { x1, y1, z1 }, { x1, y1, z0 }, n(1, 0, 0), c(0.7f));
            addQuad(out, { x0, y1, z0 }, { x1, y1, z0 }, { x1, y1, z1 }, { x0, y1, z1 }, n(0, 1, 0), c(1.0f));
            addQuad(out, { x0, y0, z1 }, { x1, y0, z1 }, { x1, y0, z0 }, { x0, y0, z0 }, n(0, -1, 0), c(0.6f));
        };
        // Square beam from p0 to p1
        auto addBeam = [&addQuad](TArray<SceneVertex> &out, const Vector3 &p0, const Vector3 &p1, Real thickness, const Vector3 &color)
        {
            Vector3 d = p1 - p0;
            d.normalize();
            const Vector3 ref = (std::abs(d.y()) < 0.9f) ? Vector3::UP : Vector3(1.0f, 0.0f, 0.0f);
            Vector3 s = ref.cross(d);
            s.normalize();
            Vector3 u = d.cross(s);
            u.normalize();
            const Vector3 hs = s * (thickness * 0.5f);
            const Vector3 hu = u * (thickness * 0.5f);
            addQuad(out, p0 - hs - hu, p1 - hs - hu, p1 + hs - hu, p0 + hs - hu, -u, color);
            addQuad(out, p0 + hs + hu, p1 + hs + hu, p1 - hs + hu, p0 - hs + hu, u, color);
            addQuad(out, p0 - hs + hu, p1 - hs + hu, p1 - hs - hu, p0 - hs - hu, -s, color);
            addQuad(out, p0 + hs - hu, p1 + hs - hu, p1 + hs + hu, p0 + hs + hu, s, color);
        };
        // Solid of revolution around a vertical axis; profile is (radius, height) from bottom to top
        auto addLathe = [](TArray<SceneVertex> &out, const Vector3 &base, std::initializer_list<std::pair<Real, Real>> profile,
            const Vector3 &color, bool unlit)
        {
            const uint32_t segments = 20;
            const TArray<std::pair<Real, Real>> pts(profile);
            for (size_t k = 0; k + 1 < pts.size(); ++k)
            {
                const Real r0 = pts[k].first, y0 = pts[k].second, r1 = pts[k + 1].first, y1 = pts[k + 1].second;
                // Outward normal of the profile segment in the (radius, height) plane
                Real nr = y1 - y0, ny = r0 - r1;
                const Real len = std::sqrt(nr * nr + ny * ny);
                if (len < 1e-6f)
                {
                    continue;
                }
                nr /= len;
                ny /= len;
                for (uint32_t i = 0; i < segments; ++i)
                {
                    const Real a0 = Math::TWO_PI * static_cast<Real>(i) / segments;
                    const Real a1 = Math::TWO_PI * static_cast<Real>(i + 1) / segments;
                    const Real c0 = std::cos(a0), s0 = std::sin(a0), c1 = std::cos(a1), s1 = std::sin(a1);
                    const Vector3 p00 = base + Vector3(r0 * c0, y0, r0 * s0);
                    const Vector3 p01 = base + Vector3(r0 * c1, y0, r0 * s1);
                    const Vector3 p10 = base + Vector3(r1 * c0, y1, r1 * s0);
                    const Vector3 p11 = base + Vector3(r1 * c1, y1, r1 * s1);
                    const Vector3 n0 = unlit ? Vector3::ZERO : Vector3(nr * c0, ny, nr * s0);
                    const Vector3 n1 = unlit ? Vector3::ZERO : Vector3(nr * c1, ny, nr * s1);
                    out.push_back({ p00, n0, color });
                    out.push_back({ p01, n1, color });
                    out.push_back({ p11, n1, color });
                    out.push_back({ p00, n0, color });
                    out.push_back({ p11, n1, color });
                    out.push_back({ p10, n0, color });
                }
            }
        };
        auto addBottle = [&addLathe](TArray<SceneVertex> &out, const Vector3 &base, Real height, Real radius, const Vector3 &color)
        {
            const Real h = height;
            const Real r = radius;
            addLathe(out, base, { { 0.0f, 0.0f }, { r, 0.0f }, { r, h * 0.62f }, { r * 0.8f, h * 0.7f },
                { r * 0.32f, h * 0.78f }, { r * 0.3f, h * 0.97f }, { r * 0.36f, h * 0.98f }, { r * 0.36f, h }, { 0.0f, h } }, color, false);
        };

        // Room: plank floor, plaster walls with wood panelling below, dark ceiling
        const Real roomX0 = -9.0f, roomX1 = 9.0f, roomZ0 = -7.0f, roomY1 = 3.6f;
        {
            int32_t k = 0;
            for (Real z = roomZ0; z < kBackWallZ - 1e-3f; z += 0.25f, ++k)
            {
                const Real z1 = std::min(z + 0.25f, kBackWallZ);
                const Vector3 c = kDarkWood * (0.8f + 0.4f * hash1(k));
                addQuad(opaque, { roomX0, 0.0f, z }, { roomX0, 0.0f, z1 }, { roomX1, 0.0f, z1 }, { roomX1, 0.0f, z }, Vector3::UP, c);
            }
            addQuad(opaque, { roomX0, roomY1, roomZ0 }, { roomX1, roomY1, roomZ0 }, { roomX1, roomY1, kBackWallZ }, { roomX0, roomY1, kBackWallZ },
                -Vector3::UP, Vector3(0.12f, 0.1f, 0.09f));

            auto wall = [&](const Vector3 &a, const Vector3 &b, const Vector3 &n)
            {
                // a and b are the bottom corners, the wall spans the full height
                const Vector3 up1(0.0f, 1.0f, 0.0f);
                const Vector3 up2(0.0f, roomY1, 0.0f);
                addQuad(opaque, a, b, b + up1, a + up1, n, kWood * 0.8f);
                addQuad(opaque, a + up1, b + up1, b + up2, a + up2, n, kPlaster);
            };
            wall({ roomX0, 0.0f, kBackWallZ }, { roomX0, 0.0f, roomZ0 }, Vector3(1.0f, 0.0f, 0.0f));
            wall({ roomX1, 0.0f, roomZ0 }, { roomX1, 0.0f, kBackWallZ }, Vector3(-1.0f, 0.0f, 0.0f));
            wall({ roomX0, 0.0f, roomZ0 }, { roomX1, 0.0f, roomZ0 }, Vector3(0.0f, 0.0f, 1.0f));
        }

        // Back wall around the window opening, with a deep reveal
        {
            const Vector3 n(0.0f, 0.0f, -1.0f);
            const Real z = kBackWallZ;
            const Real depth = 0.3f;
            auto rect = [&](Real x0, Real y0, Real x1, Real y1, const Vector3 &color)
            {
                addQuad(opaque, { x0, y0, z }, { x1, y0, z }, { x1, y1, z }, { x0, y1, z }, n, color);
            };
            rect(roomX0, 1.0f, kWindowX0, roomY1, kPlaster);
            rect(kWindowX1, 1.0f, roomX1, roomY1, kPlaster);
            rect(kWindowX0, kWindowY1, kWindowX1, roomY1, kPlaster);
            rect(kWindowX0, 1.0f, kWindowX1, kWindowY0, kPlaster);
            rect(roomX0, 0.0f, roomX1, 1.0f, kWood * 0.8f);
            const Vector3 reveal = kPlaster * 0.9f;
            addQuad(opaque, { kWindowX0, kWindowY0, z }, { kWindowX1, kWindowY0, z }, { kWindowX1, kWindowY0, z + depth },
                { kWindowX0, kWindowY0, z + depth }, Vector3::UP, reveal);
            addQuad(opaque, { kWindowX0, kWindowY1, z }, { kWindowX1, kWindowY1, z }, { kWindowX1, kWindowY1, z + depth },
                { kWindowX0, kWindowY1, z + depth }, -Vector3::UP, reveal);
            addQuad(opaque, { kWindowX0, kWindowY0, z }, { kWindowX0, kWindowY1, z }, { kWindowX0, kWindowY1, z + depth },
                { kWindowX0, kWindowY0, z + depth }, Vector3(1.0f, 0.0f, 0.0f), reveal);
            addQuad(opaque, { kWindowX1, kWindowY0, z }, { kWindowX1, kWindowY1, z }, { kWindowX1, kWindowY1, z + depth },
                { kWindowX1, kWindowY0, z + depth }, Vector3(-1.0f, 0.0f, 0.0f), reveal);
        }

        // Sunlit yard behind the window, unlit and bright: white garden wall under a blue sky,
        // a green hedge and grey stone columns. Through the clear pane they keep their colors,
        // through the red pane the sky and the hedge go dark while the white wall turns red.
        {
            const Real zFar = 10.0f;
            addQuad(opaque, { -12.0f, 2.6f, zFar }, { 12.0f, 2.6f, zFar }, { 12.0f, 8.0f, zFar }, { -12.0f, 8.0f, zFar },
                Vector3::ZERO, Vector3(0.45f, 0.68f, 0.95f));
            addQuad(opaque, { -12.0f, -0.5f, zFar }, { 12.0f, -0.5f, zFar }, { 12.0f, 2.6f, zFar }, { -12.0f, 2.6f, zFar },
                Vector3::ZERO, Vector3(0.96f, 0.94f, 0.88f));
            addQuad(opaque, { -12.0f, 0.0f, kBackWallZ }, { 12.0f, 0.0f, kBackWallZ }, { 12.0f, 0.0f, zFar }, { -12.0f, 0.0f, zFar },
                Vector3::ZERO, Vector3(0.72f, 0.7f, 0.64f));
            addBox(opaque, { -12.0f, 0.0f, 8.6f }, { 12.0f, 2.0f, 9.4f }, Vector3(0.22f, 0.6f, 0.2f), true);
            for (Real x : { -4.2f, -1.6f, 1.0f, 3.6f })
            {
                addBox(opaque, { x - 0.3f, 0.0f, 6.8f }, { x + 0.3f, 6.0f, 7.4f }, Vector3(0.62f, 0.6f, 0.57f), true);
            }
        }

        // Stained glass: 4 x 2 panes in lead cames, one clear pane for reference
        {
            const Vector3 tints[2][4] =
            {
                { { 0.92f, 0.16f, 0.12f }, { 0.95f, 0.62f, 0.12f }, { 0.9f, 0.9f, 0.88f }, { 0.15f, 0.35f, 0.95f } },
                { { 0.15f, 0.35f, 0.95f }, { 0.2f, 0.8f, 0.3f }, { 0.92f, 0.16f, 0.12f }, { 0.62f, 0.2f, 0.88f } },
            };
            const Real z = kBackWallZ + 0.12f;
            const Real w = (kWindowX1 - kWindowX0) / 4.0f;
            const Real h = (kWindowY1 - kWindowY0) / 2.0f;
            for (uint32_t row = 0; row < 2; ++row)
            {
                for (uint32_t col = 0; col < 4; ++col)
                {
                    const Real x0 = kWindowX0 + w * col, x1 = x0 + w;
                    const Real y1 = kWindowY1 - h * row, y0 = y1 - h;
                    addQuad(window, { x0, y0, z }, { x1, y0, z }, { x1, y1, z }, { x0, y1, z }, Vector3(0.0f, 0.0f, -1.0f), tints[row][col]);
                }
            }
            const Vector3 lead(0.06f, 0.06f, 0.07f);
            for (uint32_t col = 0; col <= 4; ++col)
            {
                const Real x = kWindowX0 + w * col;
                addBox(opaque, { x - 0.035f, kWindowY0, z - 0.03f }, { x + 0.035f, kWindowY1, z + 0.03f }, lead, false);
            }
            for (uint32_t row = 0; row <= 2; ++row)
            {
                const Real y = kWindowY0 + h * row;
                addBox(opaque, { kWindowX0, y - 0.035f, z - 0.03f }, { kWindowX1, y + 0.035f, z + 0.03f }, lead, false);
            }
        }

        // Back counter below the window with a row of bottles
        {
            addBox(opaque, { -6.0f, 0.0f, 3.9f }, { 6.0f, 1.0f, kBackWallZ }, kDarkWood, false);
            addBox(opaque, { -6.05f, 1.0f, 3.85f }, { 6.05f, 1.04f, kBackWallZ }, kWoodTop * 0.8f, false);
            const Vector3 colors[] =
            {
                { 0.1f, 0.35f, 0.12f }, { 0.55f, 0.3f, 0.06f }, { 0.4f, 0.05f, 0.07f }, { 0.6f, 0.65f, 0.62f }, { 0.1f, 0.15f, 0.45f },
            };
            int32_t k = 0;
            for (Real x = -5.6f; x < 5.7f; x += 0.42f, ++k)
            {
                const Real height = 0.3f + 0.12f * hash1(k * 3 + 1);
                const Real radius = 0.045f + 0.015f * hash1(k * 5 + 2);
                addBottle(opaque, { x, 1.04f, 4.15f + 0.1f * hash1(k * 7) }, height, radius, colors[k % 5]);
            }
        }

        // Bar counter with a brass foot rail, stools and a few bottles on top
        {
            addBox(opaque, { -5.0f, 0.0f, 1.2f }, { 3.5f, 1.08f, 2.0f }, kWood, false);
            addBox(opaque, { -5.1f, 1.08f, 1.05f }, { 3.6f, 1.14f, 2.1f }, kWoodTop, false);
            addBeam(opaque, { -5.0f, 0.22f, 1.05f }, { 3.5f, 0.22f, 1.05f }, 0.05f, kBrass);
            for (Real x : { -4.0f, -2.5f, -1.0f, 0.5f, 2.0f })
            {
                const Vector3 base(x, 0.0f, 0.5f);
                addLathe(opaque, base, { { 0.0f, 0.0f }, { 0.22f, 0.0f }, { 0.22f, 0.03f }, { 0.04f, 0.04f }, { 0.04f, 0.74f }, { 0.0f, 0.74f } },
                    kBrass * 0.8f, false);
                addLathe(opaque, base, { { 0.0f, 0.74f }, { 0.2f, 0.74f }, { 0.21f, 0.79f }, { 0.18f, 0.83f }, { 0.0f, 0.83f } },
                    Vector3(0.45f, 0.06f, 0.05f), false);
            }
            addBottle(opaque, { -3.9f, 1.14f, 1.6f }, 0.34f, 0.05f, Vector3(0.1f, 0.35f, 0.12f));
            addBottle(opaque, { -1.6f, 1.14f, 1.75f }, 0.3f, 0.055f, Vector3(0.55f, 0.3f, 0.06f));
            addBottle(opaque, { 1.2f, 1.14f, 1.5f }, 0.38f, 0.045f, Vector3(0.4f, 0.05f, 0.07f));
            for (Real x : { -2.9f, -0.2f, 2.6f })
            {
                addLathe(opaque, { x, 1.14f, 1.45f }, { { 0.0f, 0.0f }, { 0.04f, 0.0f }, { 0.045f, 0.1f }, { 0.0f, 0.1f } },
                    Vector3(0.55f, 0.58f, 0.56f), false);
            }
        }

        // Pendant lamps over the bar: cord, brass shade, glowing bulb
        for (const Vector3 &p : kLampPos)
        {
            addBeam(opaque, p + Vector3(0.0f, 0.3f, 0.0f), Vector3(p.x(), roomY1, p.z()), 0.015f, Vector3(0.05f, 0.05f, 0.05f));
            addLathe(opaque, p + Vector3(0.0f, -0.02f, 0.0f), { { 0.24f, 0.0f }, { 0.12f, 0.2f }, { 0.04f, 0.32f }, { 0.0f, 0.32f } },
                Vector3(0.18f, 0.32f, 0.22f), false);
            addLathe(opaque, p + Vector3(0.0f, -0.07f, 0.0f), { { 0.0f, 0.0f }, { 0.06f, 0.03f }, { 0.07f, 0.08f }, { 0.04f, 0.13f }, { 0.0f, 0.14f } },
                Vector3(1.0f, 0.86f, 0.6f), true);
        }

        // Display case: wooden plinth, brass frame, a trophy and two bottles inside
        {
            addBox(opaque, { kCaseX0, 0.0f, kCaseZ0 }, { kCaseX1, kCaseGlassY0, kCaseZ1 }, kDarkWood, false);
            addBox(opaque, { kCaseLightPos.x() - 0.2f, roomY1 - 0.05f, kCaseLightPos.z() - 0.2f },
                { kCaseLightPos.x() + 0.2f, roomY1, kCaseLightPos.z() + 0.2f }, Vector3(0.95f, 0.97f, 1.0f), true);

            const Real t = 0.035f;
            const Vector3 corners[4] = { { kCaseX0, 0.0f, kCaseZ0 }, { kCaseX1, 0.0f, kCaseZ0 }, { kCaseX1, 0.0f, kCaseZ1 }, { kCaseX0, 0.0f, kCaseZ1 } };
            for (uint32_t i = 0; i < 4; ++i)
            {
                const Vector3 &a = corners[i];
                const Vector3 &b = corners[(i + 1) % 4];
                addBeam(opaque, a + Vector3(0.0f, kCaseGlassY0, 0.0f), a + Vector3(0.0f, kCaseGlassY1, 0.0f), t, kBrass);
                addBeam(opaque, a + Vector3(0.0f, kCaseGlassY0, 0.0f), b + Vector3(0.0f, kCaseGlassY0, 0.0f), t, kBrass);
                addBeam(opaque, a + Vector3(0.0f, kCaseGlassY1, 0.0f), b + Vector3(0.0f, kCaseGlassY1, 0.0f), t, kBrass);
            }

            addLathe(opaque, { 6.0f, kCaseGlassY0, -0.1f }, { { 0.0f, 0.0f }, { 0.14f, 0.0f }, { 0.14f, 0.06f }, { 0.04f, 0.1f },
                { 0.035f, 0.32f }, { 0.08f, 0.36f }, { 0.16f, 0.5f }, { 0.17f, 0.62f }, { 0.0f, 0.62f } }, Vector3(0.85f, 0.62f, 0.18f), false);
            addLathe(opaque, { 6.6f, kCaseGlassY0, 0.2f }, { { 0.0f, 0.0f }, { 0.12f, 0.0f }, { 0.14f, 0.12f }, { 0.1f, 0.26f },
                { 0.035f, 0.32f }, { 0.035f, 0.42f }, { 0.06f, 0.44f }, { 0.0f, 0.5f } }, Vector3(0.5f, 0.04f, 0.06f), false);
            addBottle(opaque, { 6.7f, kCaseGlassY0, -0.35f }, 0.42f, 0.06f, Vector3(0.55f, 0.32f, 0.06f));

            // Far panes first: the camera always looks at the case from the -x / -z side
            const Vector3 tint(0.55f, 0.68f, 0.66f);
            const Real y0 = kCaseGlassY0, y1 = kCaseGlassY1;
            addQuad(caseGlass, { kCaseX0, y0, kCaseZ1 }, { kCaseX1, y0, kCaseZ1 }, { kCaseX1, y1, kCaseZ1 }, { kCaseX0, y1, kCaseZ1 },
                Vector3(0.0f, 0.0f, 1.0f), tint);
            addQuad(caseGlass, { kCaseX1, y0, kCaseZ0 }, { kCaseX1, y0, kCaseZ1 }, { kCaseX1, y1, kCaseZ1 }, { kCaseX1, y1, kCaseZ0 },
                Vector3(1.0f, 0.0f, 0.0f), tint);
            addQuad(caseGlass, { kCaseX0, y1, kCaseZ0 }, { kCaseX1, y1, kCaseZ0 }, { kCaseX1, y1, kCaseZ1 }, { kCaseX0, y1, kCaseZ1 },
                Vector3::UP, tint);
            addQuad(caseGlass, { kCaseX0, y0, kCaseZ0 }, { kCaseX0, y0, kCaseZ1 }, { kCaseX0, y1, kCaseZ1 }, { kCaseX0, y1, kCaseZ0 },
                Vector3(-1.0f, 0.0f, 0.0f), tint);
            addQuad(caseGlass, { kCaseX0, y0, kCaseZ0 }, { kCaseX1, y0, kCaseZ0 }, { kCaseX1, y1, kCaseZ0 }, { kCaseX0, y1, kCaseZ0 },
                Vector3(0.0f, 0.0f, -1.0f), tint);
        }
    }

    //--------------------------------------------------------------------------

    Real OITBehaviour::caseGlassOpacity() const
    {
        const Real dist = (mEye - kCaseCenter).length();
        return kCaseGlassAlpha * smoothstep(kCaseFadeNear, kCaseFadeFar, dist);
    }

    //--------------------------------------------------------------------------

    TResult OITBehaviour::uploadCb(ConstantBuffer *cb, const void *data, size_t bytes)
    {
        Buffer buf = wrapBytes(const_cast<void *>(data), bytes);
        const size_t written = cb->writeData(0, buf, true);
        buf.Data = nullptr;
        return (written == bytes) ? T3D_OK : T3D_ERR_FAIL;
    }

    //--------------------------------------------------------------------------

    void OITBehaviour::packMatrix(float *dst, const Matrix4 &m) const
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

    void OITBehaviour::drawVB(RHIContext *ctx, const VertexBufferPtr &vb, uint32_t vertexCount)
    {
        VertexBuffers vbs;
        vbs.push_back(vb);
        VertexStrides strides;
        strides.push_back(sizeof(SceneVertex));
        VertexOffsets offsets;
        offsets.push_back(0);
        ctx->setVertexBuffers(0, vbs, strides, offsets);
        ctx->render(vertexCount, 0);
    }

    //--------------------------------------------------------------------------

    void OITBehaviour::render(RHIContext *ctx)
    {
        ctx->setRenderTarget(mTarget.get());
        ctx->setViewport(Viewport {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f});
        ctx->setViewProjectionTransform(mCamera->getViewMatrix(), mCamera->getProjectionMatrix());

        alignas(16) float data[60];
        packMatrix(data, ctx->getProjViewMatrix());
        float *lightPos = data + 16;
        float *lightColor = data + 16 + kOITLightCount * 4;
        for (uint32_t i = 0; i < kOITLightCount; ++i)
        {
            const bool lamp = (i < 3);
            const Vector3 &p = lamp ? kLampPos[i] : kCaseLightPos;
            const Vector3 &c = lamp ? kLampColor : kCaseLightColor;
            const float pos[4] = { p.x(), p.y(), p.z(), lamp ? kLampRange : kCaseLightRange };
            const float color[4] = { c.x(), c.y(), c.z(), 0.0f };
            memcpy(lightPos + i * 4, pos, sizeof(pos));
            memcpy(lightColor + i * 4, color, sizeof(color));
        }
        const float tail[3][4] =
        {
            { kAmbient.x(), kAmbient.y(), kAmbient.z(), 0.0f },
            { mEye.x(), mEye.y(), mEye.z(), 0.0f },
            { 0.35f, 0.18f, 0.9f, 0.0f },
        };
        memcpy(data + 48, tail, sizeof(tail));
        uploadCb(mSceneCB.get(), data, sizeof(data));

        ctx->clearColor(kClearColor);
        ctx->clearDepthStencil(1.0f, 0);
        ctx->beginPass();

        ctx->setRasterizerState(mRasterizer.get());
        ctx->setPrimitiveType(PrimitiveType::kTriangleList);
        ConstantBuffers cbs;
        cbs.push_back(mSceneCB);
        ctx->setVSConstantBuffers(0, cbs);
        ctx->setPSConstantBuffers(0, cbs);
        ctx->setVertexShader(mSceneVS.get());
        ctx->setVertexDeclaration(mSceneDecl.get());

        ctx->setDepthStencilState(mOpaqueDepth.get());
        ctx->setBlendState(mOpaqueBlend.get());
        ctx->setPixelShader(mScenePS.get());
        drawVB(ctx, mOpaqueVB, mOpaqueVertexCount);

        // Glass tests against the bar but leaves the depth alone
        ctx->setDepthStencilState(mGlassDepth.get());
        ctx->setBlendState(mWindowBlend.get());
        ctx->setPixelShader(mWindowPS.get());
        drawVB(ctx, mWindowVB, mWindowVertexCount);

        // The constant is context state, it is set before the blend state that reads it
        const Real opacity = caseGlassOpacity();
        ctx->setBlendConstant(ColorRGBA(opacity, opacity, opacity, opacity));
        ctx->setBlendState(mCaseGlassBlend.get());
        ctx->setPixelShader(mCaseGlassPS.get());
        drawVB(ctx, mCaseGlassVB, mCaseGlassVertexCount);

        ctx->endPass();
        ctx->reset();

        if (mElapsed >= mNextLog)
        {
            mNextLog = mElapsed + 2.0f;
            T3D_LOG_INFO(LOG_TAG_APP, "OIT : t = %.1f s, camera %.2f m from the display case, case glass blend constant %.3f",
                mElapsed, (mEye - kCaseCenter).length(), opacity);
        }
    }

    //--------------------------------------------------------------------------

    void OITBehaviour::onPostRender(RHIContext *ctx)
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

        render(ctx);

        RenderTarget *dst = mCamera->getSrcRenderTarget();
        if (dst == nullptr)
        {
            dst = mCamera->getRenderTarget();
        }
        ctx->blit(mTarget.get(), dst);
    }
}
