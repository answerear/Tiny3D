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


#include "OcclusionCullingBehaviour.h"
#include "OcclusionCullingShaders.h"
#include "Kernel/T3DAgent.h"
#include "Kernel/T3DTime.h"
#include "RHI/T3DRHIContext.h"
#include "RHI/T3DRHICapabilities.h"
#include "RHI/T3DRHIQuery.h"
#include "RHI/T3DRHIRenderer.h"
#include "Render/T3DGPUProfiler.h"
#include "Render/T3DRenderPipeline.h"

#include <algorithm>
#include <cmath>
#include <numeric>

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

        /// Deterministic so every run (and every backend) builds the same city
        struct Random
        {
            uint32_t state {0x1234567u};

            Real next()
            {
                state = state * 1664525u + 1013904223u;
                return static_cast<Real>(state >> 8) / static_cast<Real>(1u << 24);
            }

            Real range(Real lo, Real hi) { return lo + (hi - lo) * next(); }
        };

        // City layout: kBlocks x kBlocks blocks separated by streets. Street i spans
        // [streetMin(i), streetMin(i) + kStreetWidth]; block i spans the gap after it.
        const uint32_t kBlocks = 8;
        const Real kBlockSize = 40.0f;
        const Real kStreetWidth = 14.0f;
        const Real kPitch = kBlockSize + kStreetWidth;
        const Real kCityOrigin = -0.5f * kPitch * static_cast<Real>(kBlocks);
        const Real kCurbHeight = 0.15f;

        Real streetMin(uint32_t i) { return kCityOrigin + kPitch * static_cast<Real>(i); }
        Real streetCenter(uint32_t i) { return streetMin(i) + 0.5f * kStreetWidth; }
        Real blockMin(uint32_t i) { return streetMin(i) + kStreetWidth; }
        Real blockMax(uint32_t i) { return blockMin(i) + kBlockSize; }

        const Real kEyeHeight = 2.2f;
        const Real kLookAhead = 10.0f;
        const Real kLogInterval = 2.0f;
    }

    //--------------------------------------------------------------------------

    OcclusionCullingBehaviour::OcclusionCullingBehaviour(const UUID &uuid)
        : CameraBehaviour(uuid)
    {
    }

    //--------------------------------------------------------------------------

    OcclusionCullingBehaviour::~OcclusionCullingBehaviour()
    {
        teardown();
    }

    //--------------------------------------------------------------------------

    void OcclusionCullingBehaviour::onAwake()
    {
        mCamera = getGameObject()->getComponent<Camera>();
        buildRoute();
    }

    //--------------------------------------------------------------------------

    void OcclusionCullingBehaviour::onUpdate()
    {
        // Time::deltaTime() is milliseconds
        const Real dtMs = static_cast<Real>(Time::deltaTime());
        mElapsed += dtMs / 1000.0f;

        if (mCamera != nullptr && mRouteLength > REAL_ZERO)
        {
            const Real distance = mElapsed * mSpeed;
            mEye = routePoint(distance) + Vector3(0.0f, kEyeHeight, 0.0f);
            const Vector3 target = routePoint(distance + kLookAhead) + Vector3(0.0f, kEyeHeight - 0.2f, 0.0f);
            mCamera->lookAt(mEye, target, Vector3::UP);
        }

        if (mReady)
        {
            mCpuFrameMs += dtMs;
            mLogFrames++;
            mLogTimer += dtMs / 1000.0f;
            if (mLogTimer >= kLogInterval)
            {
                logStats();
                mLogTimer = 0.0f;
            }
        }
    }

    //--------------------------------------------------------------------------

    void OcclusionCullingBehaviour::onDestroy()
    {
        teardown();
    }

    //--------------------------------------------------------------------------

    void OcclusionCullingBehaviour::buildRoute()
    {
        // A closed loop along street centers with left and right turns, so the view keeps
        // swinging from long avenues into narrow canyons
        const uint32_t corners[][2] =
        {
            { 1, 1 }, { 5, 1 }, { 5, 4 }, { 3, 4 }, { 3, 7 }, { 1, 7 },
        };

        mRoute.clear();
        for (const auto &c : corners)
        {
            mRoute.push_back(Vector3(streetCenter(c[0]), 0.0f, streetCenter(c[1])));
        }

        mRouteDistances.clear();
        mRouteLength = 0.0f;
        for (size_t i = 0; i < mRoute.size(); ++i)
        {
            mRouteDistances.push_back(mRouteLength);
            mRouteLength += (mRoute[(i + 1) % mRoute.size()] - mRoute[i]).length();
        }
    }

    //--------------------------------------------------------------------------

    Vector3 OcclusionCullingBehaviour::routePoint(Real distance) const
    {
        Real d = std::fmod(distance, mRouteLength);
        if (d < 0.0f)
        {
            d += mRouteLength;
        }

        for (size_t i = mRoute.size(); i-- > 0;)
        {
            if (d >= mRouteDistances[i])
            {
                const Vector3 &a = mRoute[i];
                const Vector3 &b = mRoute[(i + 1) % mRoute.size()];
                const Real len = (b - a).length();
                const Real t = (len > REAL_ZERO) ? (d - mRouteDistances[i]) / len : 0.0f;
                return a + (b - a) * t;
            }
        }
        return mRoute.front();
    }

    //--------------------------------------------------------------------------

    void OcclusionCullingBehaviour::buildCity(TArray<Vertex> &ground, TArray<Vertex> &objects)
    {
        auto addBox = [](TArray<Vertex> &vertices, const Vector3 &minPos, const Vector3 &maxPos, const Vector3 &color)
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
                const int order[6] = { q.a, q.b, q.cc, q.a, q.cc, q.d };
                for (int idx : order)
                {
                    vertices.push_back({ c[idx], q.n, color });
                }
            }
        };

        Random rng;
        mObjects.clear();

        // Each object is a few boxes; the union of them is its bounding box
        struct Part { Vector3 minPos; Vector3 maxPos; Vector3 color; };
        auto addObject = [&](Kind kind, std::initializer_list<Part> parts)
        {
            CityObject obj;
            obj.kind = kind;
            obj.start = static_cast<uint32_t>(objects.size());
            obj.minPos = parts.begin()->minPos;
            obj.maxPos = parts.begin()->maxPos;
            for (const Part &p : parts)
            {
                addBox(objects, p.minPos, p.maxPos, p.color);
                obj.minPos = Vector3(std::min(obj.minPos.x(), p.minPos.x()), std::min(obj.minPos.y(), p.minPos.y()),
                    std::min(obj.minPos.z(), p.minPos.z()));
                obj.maxPos = Vector3(std::max(obj.maxPos.x(), p.maxPos.x()), std::max(obj.maxPos.y(), p.maxPos.y()),
                    std::max(obj.maxPos.z(), p.maxPos.z()));
            }
            obj.count = static_cast<uint32_t>(objects.size()) - obj.start;
            mObjects.push_back(obj);
        };

        // Asphalt under the whole city, a raised concrete slab per block
        {
            const Real lo = kCityOrigin - 40.0f;
            const Real hi = -kCityOrigin + kStreetWidth + 40.0f;
            const Vector3 asphalt(0.16f, 0.16f, 0.18f);
            const Vector3 up(0.0f, 1.0f, 0.0f);
            const Vector3 quad[6] =
            {
                Vector3(lo, 0.0f, lo), Vector3(lo, 0.0f, hi), Vector3(hi, 0.0f, hi),
                Vector3(lo, 0.0f, lo), Vector3(hi, 0.0f, hi), Vector3(hi, 0.0f, lo),
            };
            for (const Vector3 &p : quad)
            {
                ground.push_back({ p, up, asphalt });
            }
        }
        for (uint32_t i = 0; i < kBlocks; ++i)
        {
            for (uint32_t j = 0; j < kBlocks; ++j)
            {
                addBox(ground, Vector3(blockMin(i), 0.0f, blockMin(j)), Vector3(blockMax(i), kCurbHeight, blockMax(j)),
                    Vector3(0.42f, 0.42f, 0.40f));
            }
        }

        // Buildings: 2 x 2 towers per block, every few blocks a low plaza pavilion that
        // opens a sight line to the towers behind it
        for (uint32_t i = 0; i < kBlocks; ++i)
        {
            for (uint32_t j = 0; j < kBlocks; ++j)
            {
                const Real bx = blockMin(i);
                const Real bz = blockMin(j);
                if ((i * 3 + j * 5) % 7 == 0)
                {
                    const Real h = rng.range(3.0f, 6.0f);
                    addObject(Kind::kBuilding, {
                        { Vector3(bx + 9.0f, kCurbHeight, bz + 9.0f), Vector3(bx + 31.0f, h, bz + 31.0f),
                          Vector3(0.62f, 0.58f, 0.50f) } });
                    continue;
                }

                for (uint32_t a = 0; a < 2; ++a)
                {
                    for (uint32_t b = 0; b < 2; ++b)
                    {
                        const Real inset = rng.range(0.0f, 2.0f);
                        const Real x0 = bx + 3.0f + 18.0f * static_cast<Real>(a) + inset;
                        const Real z0 = bz + 3.0f + 18.0f * static_cast<Real>(b) + inset;
                        const Real x1 = x0 + 16.0f - 2.0f * inset;
                        const Real z1 = z0 + 16.0f - 2.0f * inset;
                        const Real h = rng.range(14.0f, 85.0f);
                        const Real tone = rng.range(0.45f, 0.75f);
                        const Vector3 color(tone + rng.range(0.0f, 0.12f), tone + rng.range(0.0f, 0.08f), tone + rng.range(0.0f, 0.15f));
                        addObject(Kind::kBuilding, {
                            { Vector3(x0, kCurbHeight, z0), Vector3(x1, h, z1), color },
                            // Rooftop machine room
                            { Vector3(x0 + 4.0f, h, z0 + 4.0f), Vector3(x1 - 5.0f, h + 3.0f, z1 - 6.0f), color * 0.8f } });
                    }
                }
            }
        }

        // Street lamps on both curbs of every street segment, arms reaching over the road
        const Vector3 poleColor(0.24f, 0.25f, 0.27f);
        const Vector3 headColor(1.0f, 0.88f, 0.55f);
        const Real lampOffsets[3] = { 6.0f, 20.0f, 34.0f };
        for (uint32_t s = 0; s <= kBlocks; ++s)
        {
            for (uint32_t seg = 0; seg < kBlocks; ++seg)
            {
                for (Real offset : lampOffsets)
                {
                    const Real along = blockMin(seg) + offset;
                    for (int side = 0; side < 2; ++side)
                    {
                        if ((side == 0 && s == 0) || (side == 1 && s == kBlocks))
                        {
                            continue;
                        }
                        // side 0 stands on the curb before the street and leans toward +axis
                        const Real across = (side == 0) ? streetMin(s) - 1.2f : streetMin(s) + kStreetWidth + 1.2f;
                        const Real armLo = (side == 0) ? across : across - 1.6f;
                        const Real armHi = (side == 0) ? across + 1.6f : across;
                        for (int axis = 0; axis < 2; ++axis)
                        {
                            // axis 0: street runs along X, lamp position (along, across); axis 1 swaps them
                            auto pt = [axis](Real a, Real y, Real c)
                            {
                                return (axis == 0) ? Vector3(a, y, c) : Vector3(c, y, a);
                            };
                            auto box = [&](Real a0, Real a1, Real y0, Real y1, Real c0, Real c1, const Vector3 &color)
                            {
                                const Vector3 p0 = pt(a0, y0, c0);
                                const Vector3 p1 = pt(a1, y1, c1);
                                return Part { Vector3(std::min(p0.x(), p1.x()), y0, std::min(p0.z(), p1.z())),
                                    Vector3(std::max(p0.x(), p1.x()), y1, std::max(p0.z(), p1.z())), color };
                            };
                            addObject(Kind::kLamp, {
                                box(along - 0.12f, along + 0.12f, kCurbHeight, 6.0f, across - 0.12f, across + 0.12f, poleColor),
                                box(along - 0.25f, along + 0.25f, 5.8f, 6.1f, armLo, armHi, headColor) });
                        }
                    }
                }
            }
        }

        // Parked and waiting cars in both lanes, at most one per slot so they never overlap
        for (uint32_t s = 0; s <= kBlocks; ++s)
        {
            for (uint32_t seg = 0; seg < kBlocks; ++seg)
            {
                for (int lane = 0; lane < 2; ++lane)
                {
                    for (int slot = 0; slot < 6; ++slot)
                    {
                        for (int axis = 0; axis < 2; ++axis)
                        {
                            if (rng.next() > 0.12f)
                            {
                                continue;
                            }
                            const Real along = blockMin(seg) + 4.0f + 6.4f * static_cast<Real>(slot);
                            const Real across = streetCenter(s) + ((lane == 0) ? -3.5f : 3.5f);
                            const Vector3 color(rng.range(0.1f, 0.9f), rng.range(0.1f, 0.9f), rng.range(0.1f, 0.9f));
                            auto part = [axis](Real a0, Real a1, Real y0, Real y1, Real c0, Real c1, const Vector3 &col)
                            {
                                return (axis == 0)
                                    ? Part { Vector3(a0, y0, c0), Vector3(a1, y1, c1), col }
                                    : Part { Vector3(c0, y0, a0), Vector3(c1, y1, a1), col };
                            };
                            addObject(Kind::kCar, {
                                part(along - 2.1f, along + 2.1f, 0.25f, 1.15f, across - 0.95f, across + 0.95f, color),
                                part(along - 1.1f, along + 0.9f, 1.15f, 1.75f, across - 0.8f, across + 0.8f, color * 0.55f) });
                        }
                    }
                }
            }
        }

        // Bounding box proxies follow the meshes in the same buffer
        for (CityObject &obj : mObjects)
        {
            obj.proxyStart = static_cast<uint32_t>(objects.size());
            addBox(objects, obj.minPos, obj.maxPos, Vector3::ZERO);
        }
    }

    //--------------------------------------------------------------------------

    bool OcclusionCullingBehaviour::setup(RHIContext *ctx)
    {
        const RHICapabilities &caps = ctx->getCapabilities();
        const String &rendererName = T3D_AGENT.getActiveRHIRenderer()->getName();
        mGLFamily = (rendererName == RHIRenderer::OPENGL4);

        T3D_LOG_INFO(LOG_TAG_APP, "OcclusionCulling : %s queries - occlusion %d, binary occlusion %d, timestamp %d, "
            "pipeline statistics %d, predication %d, debug annotations %d",
            rendererName.c_str(), caps.supportsOcclusionQuery, caps.supportsBinaryOcclusionQuery,
            caps.supportsTimestampQuery, caps.supportsPipelineStatisticsQuery, caps.supportsPredication,
            caps.supportsDebugAnnotations);

        ShaderPtr dummy;
        mVS = createCityShader(CityShader::kVS, dummy);
        mDummyShaders.push_back(dummy);
        mPS = createCityShader(CityShader::kPS, dummy);
        mDummyShaders.push_back(dummy);
        if (mVS == nullptr || mPS == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "OcclusionCulling : shaders unavailable on %s, only Direct3D11 and OpenGL 4 are supported.",
                rendererName.c_str());
            return false;
        }

        VertexAttributes attribs;
        attribs.push_back(VertexAttribute(0, 0, VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_POSITION, 0));
        attribs.push_back(VertexAttribute(0, sizeof(Vector3), VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_NORMAL, 0));
        attribs.push_back(VertexAttribute(0, sizeof(Vector3) * 2, VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_DIFFUSE, 0));
        mDecl = T3D_RENDER_BUFFER_MGR.addVertexDeclaration(attribs, mVS.get());

        TArray<Vertex> ground;
        TArray<Vertex> objects;
        buildCity(ground, objects);
        mGroundVertexCount = static_cast<uint32_t>(ground.size());

        Buffer groundData = wrapBytes(ground.data(), sizeof(Vertex) * ground.size());
        mGroundVB = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(sizeof(Vertex), mGroundVertexCount, groundData,
            MemoryType::kBoth, Usage::kImmutable, kCPUNone);
        groundData.Data = nullptr;

        Buffer objectData = wrapBytes(objects.data(), sizeof(Vertex) * objects.size());
        mObjectVB = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(sizeof(Vertex), static_cast<uint32_t>(objects.size()),
            objectData, MemoryType::kBoth, Usage::kImmutable, kCPUNone);
        objectData.Data = nullptr;

        Buffer cbData = allocBytes(sizeof(float) * 28);
        mCityCB = T3D_RENDER_BUFFER_MGR.loadConstantBuffer(kCityParamsName, 0, cbData, MemoryType::kBoth,
            Usage::kDynamic, kCPUWrite);
        cbData.release();

        if (mGroundVB != nullptr)
        {
            mGroundVB->setDebugName("City Ground VB");
        }
        if (mObjectVB != nullptr)
        {
            mObjectVB->setDebugName("City Objects + Proxies VB");
        }

        RasterizerDesc raster;
        raster.CullMode = CullingMode::kNone;
        mRasterizer = T3D_RENDER_STATE_MGR.loadRasterizerState(raster);

        DepthStencilDesc depth;
        mDepthState = T3D_RENDER_STATE_MGR.loadDepthStencilState(depth);
        // Proxies only test against what is already drawn, they must not occlude anything themselves
        DepthStencilDesc proxyDepth;
        proxyDepth.DepthWriteEnable = false;
        proxyDepth.DepthFunc = CompareFunction::kLessEqual;
        mProxyDepthState = T3D_RENDER_STATE_MGR.loadDepthStencilState(proxyDepth);

        BlendDesc blend;
        mBlendState = T3D_RENDER_STATE_MGR.loadBlendState(blend);
        BlendDesc proxyBlend;
        proxyBlend.RenderTargetStates[0].ColorMask = kWriteMaskNone;
        mProxyBlendState = T3D_RENDER_STATE_MGR.loadBlendState(proxyBlend);

        const bool ok = (mDecl != nullptr && mGroundVB != nullptr && mObjectVB != nullptr && mCityCB != nullptr
            && mRasterizer != nullptr && mDepthState != nullptr && mProxyDepthState != nullptr
            && mBlendState != nullptr && mProxyBlendState != nullptr);
        if (!ok)
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "OcclusionCulling : failed to create buffers or render states.");
            return false;
        }

        const uint32_t objectCount = static_cast<uint32_t>(mObjects.size());
        if (caps.supportsBinaryOcclusionQuery)
        {
            for (uint32_t slot = 0; slot < kQueryRing; ++slot)
            {
                mQueries[slot].resize(objectCount);
                mQueryIssued[slot].assign(objectCount, 0);
                for (uint32_t i = 0; i < objectCount; ++i)
                {
                    mQueries[slot][i] = ctx->createQuery(QueryType::kBinaryOcclusion);
                }
            }
        }
        else
        {
            T3D_LOG_WARNING(LOG_TAG_APP, "OcclusionCulling : no binary occlusion query on %s, every object is drawn.",
                rendererName.c_str());
        }

        mUsePredication = caps.supportsBinaryOcclusionQuery && caps.supportsPredication;
        if (caps.supportsBinaryOcclusionQuery && !mUsePredication)
        {
            T3D_LOG_WARNING(LOG_TAG_APP, "OcclusionCulling : no predication on %s, queries only feed the statistics and "
                "every object is drawn.", rendererName.c_str());
        }

        if (caps.supportsPipelineStatisticsQuery)
        {
            for (uint32_t slot = 0; slot < kQueryRing; ++slot)
            {
                mStatsQueries[slot] = ctx->createQuery(QueryType::kPipelineStatistics);
            }
        }

        RenderPipeline *pipeline = T3D_AGENT.getRenderPipeline();
        if (!caps.supportsTimestampQuery || pipeline == nullptr || pipeline->getGPUProfiler() == nullptr)
        {
            T3D_LOG_WARNING(LOG_TAG_APP, "OcclusionCulling : GPU timings unavailable on %s.", rendererName.c_str());
        }

        uint32_t counts[3] = { 0, 0, 0 };
        for (const CityObject &obj : mObjects)
        {
            counts[static_cast<uint32_t>(obj.kind)]++;
        }
        T3D_LOG_INFO(LOG_TAG_APP, "OcclusionCulling : %u objects (%u buildings, %u lamps, %u cars), %u vertices, "
            "route %.0f m, culling %s.",
            objectCount, counts[0], counts[1], counts[2], static_cast<uint32_t>(objects.size()), mRouteLength,
            mUsePredication ? "occlusion query + predication" : "off");

        mDrawOrder.resize(objectCount);
        std::iota(mDrawOrder.begin(), mDrawOrder.end(), 0u);
        mSortKeys.resize(objectCount);
        return true;
    }

    //--------------------------------------------------------------------------

    void OcclusionCullingBehaviour::teardown()
    {
        mReady = false;
        for (uint32_t slot = 0; slot < kQueryRing; ++slot)
        {
            mQueries[slot].clear();
            mQueryIssued[slot].clear();
            mStatsQueries[slot] = nullptr;
            mStatsIssued[slot] = false;
        }
        mProxyBlendState = nullptr;
        mBlendState = nullptr;
        mProxyDepthState = nullptr;
        mDepthState = nullptr;
        mRasterizer = nullptr;
        mCityCB = nullptr;
        mObjectVB = nullptr;
        mGroundVB = nullptr;
        mDecl = nullptr;
        mPS = nullptr;
        mVS = nullptr;
        mDummyShaders.clear();
    }

    //--------------------------------------------------------------------------

    void OcclusionCullingBehaviour::packMatrix(float *dst, const Matrix4 &m) const
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

    void OcclusionCullingBehaviour::bindGeometry(RHIContext *ctx, VertexBuffer *vb)
    {
        ctx->setVertexDeclaration(mDecl.get());
        VertexBuffers vbs;
        vbs.push_back(vb);
        VertexStrides strides;
        strides.push_back(sizeof(Vertex));
        VertexOffsets offsets;
        offsets.push_back(0);
        ctx->setVertexBuffers(0, vbs, strides, offsets);
    }

    //--------------------------------------------------------------------------

    void OcclusionCullingBehaviour::collectStats(uint32_t slot)
    {
        // The set about to be reused was issued kQueryRing frames ago, its results are normally in
        uint32_t visible = 0;
        uint32_t tested = 0;
        uint32_t pending = 0;
        TArray<RHIQueryPtr> &queries = mQueries[slot];
        TArray<uint8_t> &issued = mQueryIssued[slot];
        for (size_t i = 0; i < queries.size(); ++i)
        {
            if (!issued[i] || queries[i] == nullptr)
            {
                continue;
            }
            issued[i] = 0;
            tested++;

            QueryResult result;
            const QueryStatus status = queries[i]->resolve(result);
            if (status == QueryStatus::kReady)
            {
                visible += (result.samplesPassed != 0) ? 1 : 0;
            }
            else if (status == QueryStatus::kPending)
            {
                pending++;
            }
        }
        if (tested > 0)
        {
            mVisibleSum += visible;
            mTestedSum += tested;
            mPendingSum += pending;
            mStatsFrames++;
        }

        if (mStatsIssued[slot] && mStatsQueries[slot] != nullptr)
        {
            mStatsIssued[slot] = false;
            QueryResult result;
            if (mStatsQueries[slot]->resolve(result) == QueryStatus::kReady)
            {
                mLastPipelineStats = result.pipelineStatistics;
                mHasPipelineStats = true;
            }
        }
    }

    //--------------------------------------------------------------------------

    void OcclusionCullingBehaviour::logStats()
    {
        const double cpuMs = (mLogFrames > 0) ? mCpuFrameMs / static_cast<double>(mLogFrames) : 0.0;

        if (mStatsFrames > 0)
        {
            const double frames = static_cast<double>(mStatsFrames);
            T3D_LOG_INFO(LOG_TAG_APP, "OcclusionCulling : eye (%.0f, %.0f), visible %.1f / tested %.1f of %u objects per frame, "
                "pending %.1f, CPU frame %.2f ms",
                mEye.x(), mEye.z(), static_cast<double>(mVisibleSum) / frames, static_cast<double>(mTestedSum) / frames,
                static_cast<uint32_t>(mObjects.size()), static_cast<double>(mPendingSum) / frames, cpuMs);
        }
        else
        {
            T3D_LOG_INFO(LOG_TAG_APP, "OcclusionCulling : eye (%.0f, %.0f), no occlusion results, CPU frame %.2f ms",
                mEye.x(), mEye.z(), cpuMs);
        }

        RenderPipeline *pipeline = T3D_AGENT.getRenderPipeline();
        GPUProfiler *profiler = (pipeline != nullptr) ? pipeline->getGPUProfiler() : nullptr;
        if (profiler != nullptr && profiler->isSupported() && profiler->getResolvedFrameIndex() > 0)
        {
            double topLevelSum = 0.0;
            for (const GPUProfiler::Timing &timing : profiler->getTimings())
            {
                if (timing.depth == 0)
                {
                    topLevelSum += timing.milliseconds;
                }
                T3D_LOG_INFO(LOG_TAG_APP, "OcclusionCulling :   GPU %*s%s %.3f ms (x%u)",
                    static_cast<int>(timing.depth * 2), "", timing.name.c_str(), timing.milliseconds, timing.callCount);
            }
            T3D_LOG_INFO(LOG_TAG_APP, "OcclusionCulling :   GPU frame %.3f ms, top-level sections %.3f ms, CPU frame %.2f ms, "
                "dropped frames %llu",
                profiler->getFrameTime(), topLevelSum, cpuMs,
                static_cast<unsigned long long>(profiler->getDroppedFrameCount()));
        }

        if (mHasPipelineStats)
        {
            T3D_LOG_INFO(LOG_TAG_APP, "OcclusionCulling :   pipeline statistics of the city pass - IA vertices %llu, "
                "VS invocations %llu, clipper primitives in %llu / out %llu, PS invocations %llu",
                static_cast<unsigned long long>(mLastPipelineStats.inputVertices),
                static_cast<unsigned long long>(mLastPipelineStats.vertexShaderInvocations),
                static_cast<unsigned long long>(mLastPipelineStats.clippingInvocations),
                static_cast<unsigned long long>(mLastPipelineStats.clippingPrimitives),
                static_cast<unsigned long long>(mLastPipelineStats.pixelShaderInvocations));
        }

        mCpuFrameMs = 0.0;
        mLogFrames = 0;
        mVisibleSum = 0;
        mTestedSum = 0;
        mPendingSum = 0;
        mStatsFrames = 0;
    }

    //--------------------------------------------------------------------------

    void OcclusionCullingBehaviour::renderCity(RHIContext *ctx)
    {
        T3D_RHI_DEBUG_GROUP(ctx, "City");

        RenderPipeline *pipeline = T3D_AGENT.getRenderPipeline();
        GPUProfiler *profiler = (pipeline != nullptr) ? pipeline->getGPUProfiler() : nullptr;

        const uint32_t slot = static_cast<uint32_t>(mFrameIndex % kQueryRing);
        collectStats(slot);

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
        data[16] = mEye.x();
        data[17] = mEye.y();
        data[18] = mEye.z();
        data[19] = 0.0042f;
        Vector3 toSun(0.35f, 0.8f, 0.25f);
        toSun.normalize();
        data[20] = toSun.x();
        data[21] = toSun.y();
        data[22] = toSun.z();
        data[23] = 0.0f;
        const ColorRGB &sky = mCamera->getClearColor();
        data[24] = sky.red();
        data[25] = sky.green();
        data[26] = sky.blue();
        data[27] = 1.0f;
        Buffer cbData = wrapBytes(data, sizeof(data));
        mCityCB->writeData(0, cbData, true);
        cbData.Data = nullptr;

        ctx->clearDepthStencil(1.0f, 0);
        ctx->beginPass();

        ctx->setRasterizerState(mRasterizer.get());
        ctx->setPrimitiveType(PrimitiveType::kTriangleList);
        ConstantBuffers cbs;
        cbs.push_back(mCityCB);
        ctx->setVSConstantBuffers(0, cbs);
        ctx->setPSConstantBuffers(0, cbs);
        ctx->setVertexShader(mVS.get());
        ctx->setPixelShader(mPS.get());

        {
            T3D_RHI_DEBUG_GROUP(ctx, "City Ground");
            if (profiler != nullptr)
            {
                profiler->beginScope(ctx, "City Ground");
            }
            ctx->setDepthStencilState(mDepthState.get());
            ctx->setBlendState(mBlendState.get());
            bindGeometry(ctx, mGroundVB.get());
            ctx->render(mGroundVertexCount, 0);
            if (profiler != nullptr)
            {
                profiler->endScope(ctx);
            }
        }

        // Front to back by the distance to the nearest point of each box, so near buildings
        // fill the depth buffer before the proxies behind them are tested
        const size_t objectCount = mObjects.size();
        for (size_t i = 0; i < objectCount; ++i)
        {
            const CityObject &obj = mObjects[i];
            const Vector3 nearest(
                std::max(obj.minPos.x(), std::min(mEye.x(), obj.maxPos.x())),
                std::max(obj.minPos.y(), std::min(mEye.y(), obj.maxPos.y())),
                std::max(obj.minPos.z(), std::min(mEye.z(), obj.maxPos.z())));
            mSortKeys[i] = nearest.distance2(mEye);
        }
        std::sort(mDrawOrder.begin(), mDrawOrder.end(),
            [this](uint32_t a, uint32_t b) { return mSortKeys[a] < mSortKeys[b]; });

        {
            T3D_RHI_DEBUG_GROUP(ctx, "City Objects");
            if (profiler != nullptr)
            {
                profiler->beginScope(ctx, "City Objects");
            }
            if (!mUsePredication)
            {
                T3D_RHI_DEBUG_MARKER(ctx, "Predication unavailable, drawing everything");
            }

            RHIQuery *statsQuery = mStatsQueries[slot].get();
            if (statsQuery != nullptr && T3D_SUCCEEDED(ctx->beginQuery(statsQuery)))
            {
                mStatsIssued[slot] = true;
            }
            else
            {
                statsQuery = nullptr;
            }

            bindGeometry(ctx, mObjectVB.get());
            TArray<RHIQueryPtr> &queries = mQueries[slot];
            TArray<uint8_t> &issued = mQueryIssued[slot];
            for (uint32_t index : mDrawOrder)
            {
                const CityObject &obj = mObjects[index];
                RHIQuery *query = queries.empty() ? nullptr : queries[index].get();

                // Inside or touching the box the proxy may be clipped by the near plane, never cull then
                const bool testable = (query != nullptr) && (mSortKeys[index] > 1.0f);
                bool predicated = false;
                if (testable)
                {
                    // The previous object's predicate must not skip this proxy, or its query reads 0
                    if (mUsePredication)
                    {
                        ctx->setPredication(nullptr);
                    }
                    ctx->setDepthStencilState(mProxyDepthState.get());
                    ctx->setBlendState(mProxyBlendState.get());
                    if (T3D_SUCCEEDED(ctx->beginQuery(query)))
                    {
                        ctx->render(kBoxVertexCount, obj.proxyStart);
                        if (T3D_SUCCEEDED(ctx->endQuery(query)))
                        {
                            issued[index] = 1;
                            predicated = mUsePredication;
                        }
                    }
                    ctx->setDepthStencilState(mDepthState.get());
                    ctx->setBlendState(mBlendState.get());
                }
                else
                {
                    ctx->setDepthStencilState(mDepthState.get());
                    ctx->setBlendState(mBlendState.get());
                }

                if (mUsePredication)
                {
                    ctx->setPredication(predicated ? query : nullptr);
                }
                ctx->render(obj.count, obj.start);
            }

            if (mUsePredication)
            {
                ctx->setPredication(nullptr);
            }
            if (statsQuery != nullptr)
            {
                ctx->endQuery(statsQuery);
            }
            if (profiler != nullptr)
            {
                profiler->endScope(ctx);
            }
        }

        ctx->endPass();
        ctx->reset();

        mFrameIndex++;
    }

    //--------------------------------------------------------------------------

    void OcclusionCullingBehaviour::onPostRender(RHIContext *ctx)
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

        renderCity(ctx);
    }
}
