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

#ifndef __OCCLUSION_CULLING_BEHAVIOUR_H__
#define __OCCLUSION_CULLING_BEHAVIOUR_H__


#include "Behaviour/T3DCameraBehaviour.h"
#include "Component/T3DCamera.h"
#include "RHI/T3DRHIQuery.h"


namespace Tiny3D
{
    /**
     * \brief City block driven through by the camera, every building and street prop culled
     *        by an occlusion query plus predication, drawn with raw RHI calls
     * \remarks
     *   Objects are sorted front to back each frame. Each one first draws its bounding box with
     *   color and depth writes off inside a binary occlusion query, then draws its real mesh
     *   predicated on that query, so hidden objects are skipped by the GPU in the same frame and
     *   nothing pops in late around corners.
     *   Queries rotate over kQueryRing frames so their results can be read back without stalling;
     *   the visible counts, the per-section GPU timings of the pipeline's GPUProfiler and a
     *   pipeline statistics query are logged every couple of seconds.
     *   Every section carries a debug group and every buffer a debug name for RenderDoc.
     */
    TCLASS()
    class OcclusionCullingBehaviour : public CameraBehaviour
    {
        TRTTI_ENABLE(CameraBehaviour)
        TRTTI_FRIEND

    public:
        ~OcclusionCullingBehaviour() override;

        TPROPERTY(RTTRFuncName="Speed", RTTRFuncType="getter")
        Real getSpeed() const { return mSpeed; }

        TPROPERTY(RTTRFuncName="Speed", RTTRFuncType="setter")
        void setSpeed(Real speed) { mSpeed = speed; }

    protected:
        OcclusionCullingBehaviour() = default;
        explicit OcclusionCullingBehaviour(const UUID &uuid);

        void onAwake() override;
        void onUpdate() override;
        void onDestroy() override;
        void onPostRender(RHIContext *ctx) override;

    private:
        struct Vertex
        {
            Vector3 position;
            Vector3 normal;
            Vector3 color;
        };

        enum class Kind : uint32_t
        {
            kBuilding = 0,
            kLamp,
            kCar,
        };

        /// One independently culled and drawn object
        struct CityObject
        {
            Kind        kind {Kind::kBuilding};
            uint32_t    start {0};
            uint32_t    count {0};
            uint32_t    proxyStart {0};
            Vector3     minPos {};
            Vector3     maxPos {};
        };

        /// Frames the query sets rotate over; results are read just before a set is reused
        static constexpr uint32_t kQueryRing = 4;
        static constexpr uint32_t kBoxVertexCount = 36;

        bool setup(RHIContext *ctx);
        void teardown();
        void buildCity(TArray<Vertex> &ground, TArray<Vertex> &objects);
        void buildRoute();
        Vector3 routePoint(Real distance) const;
        void collectStats(uint32_t slot);
        void logStats();
        void packMatrix(float *dst, const Matrix4 &m) const;
        void bindGeometry(RHIContext *ctx, VertexBuffer *vb);
        void renderCity(RHIContext *ctx);

        CameraPtr mCamera {nullptr};

        ShaderVariantPtr mVS {nullptr};
        ShaderVariantPtr mPS {nullptr};
        TArray<ShaderPtr> mDummyShaders;

        VertexDeclarationPtr mDecl {nullptr};
        VertexBufferPtr mGroundVB {nullptr};
        /// Object meshes followed by their bounding box proxies, one buffer so the loop never rebinds
        VertexBufferPtr mObjectVB {nullptr};
        uint32_t mGroundVertexCount {0};
        ConstantBufferPtr mCityCB {nullptr};

        /// Double sided: proxies must still rasterize when the camera stands right next to a box
        RasterizerStatePtr mRasterizer {nullptr};
        DepthStencilStatePtr mDepthState {nullptr};
        DepthStencilStatePtr mProxyDepthState {nullptr};
        BlendStatePtr mBlendState {nullptr};
        BlendStatePtr mProxyBlendState {nullptr};

        TArray<CityObject> mObjects;
        TArray<uint32_t> mDrawOrder;
        TArray<Real> mSortKeys;

        /// mQueries[slot][object]; nullptr when the backend has no binary occlusion query
        TArray<RHIQueryPtr> mQueries[kQueryRing];
        /// Whether mQueries[slot][object] was issued in the frame that last used the slot
        TArray<uint8_t> mQueryIssued[kQueryRing];
        RHIQueryPtr mStatsQueries[kQueryRing] {};
        bool mStatsIssued[kQueryRing] {};
        uint64_t mFrameIndex {0};

        TArray<Vector3> mRoute;
        TArray<Real> mRouteDistances;
        Real mRouteLength {0.0f};

        bool mInitialized {false};
        bool mReady {false};
        bool mGLFamily {false};
        bool mUsePredication {false};

        Real mElapsed {0.0f};
        Real mSpeed {9.0f};
        Vector3 mEye {};

        /// Accumulated between two log lines
        Real mLogTimer {0.0f};
        uint32_t mLogFrames {0};
        double mCpuFrameMs {0.0};
        uint64_t mVisibleSum {0};
        uint64_t mTestedSum {0};
        uint64_t mPendingSum {0};
        uint32_t mStatsFrames {0};
        PipelineStatistics mLastPipelineStats {};
        bool mHasPipelineStats {false};
    };
}


#endif  /*__OCCLUSION_CULLING_BEHAVIOUR_H__*/
