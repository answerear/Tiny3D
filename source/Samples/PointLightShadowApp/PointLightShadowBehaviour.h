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

#ifndef __POINT_LIGHT_SHADOW_BEHAVIOUR_H__
#define __POINT_LIGHT_SHADOW_BEHAVIOUR_H__


#include "Behaviour/T3DCameraBehaviour.h"
#include "Component/T3DCamera.h"


namespace Tiny3D
{
    /**
     * \brief Omnidirectional shadow of an orbiting point light, drawn with raw RHI calls
     * \remarks
     *   Every frame the six faces of a cube render texture receive the linear light distance
     *   moments (d, d^2), one RenderTarget per face selecting that cube slice, with a depth cube
     *   selected the same way. generateMips() then filters the whole chain and the lit pass
     *   samples a blurrier mip for soft variance shadow edges.
     *   The world-space room geometry lives in the behaviour, the engine scene stays empty.
     */
    TCLASS()
    class PointLightShadowBehaviour : public CameraBehaviour
    {
        TRTTI_ENABLE(CameraBehaviour)
        TRTTI_FRIEND

    public:
        ~PointLightShadowBehaviour() override;

        TPROPERTY(RTTRFuncName="ShadowMip", RTTRFuncType="getter")
        Real getShadowMip() const { return mShadowMip; }

        TPROPERTY(RTTRFuncName="ShadowMip", RTTRFuncType="setter")
        void setShadowMip(Real mip) { mShadowMip = mip; }

    protected:
        PointLightShadowBehaviour() = default;
        explicit PointLightShadowBehaviour(const UUID &uuid);

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

        static constexpr uint32_t kShadowSize = 512;
        static constexpr uint32_t kMarkerStacks = 12;
        static constexpr uint32_t kMarkerSlices = 24;
        static constexpr uint32_t kMarkerVertexCount = kMarkerStacks * kMarkerSlices * 6;

        bool setup(RHIContext *ctx);
        void teardown();
        void buildRoom(TArray<Vertex> &vertices) const;
        void buildMarker(TArray<Vertex> &vertices) const;
        TResult uploadCb(ConstantBuffer *cb, const void *data, size_t bytes);
        void packMatrix(float *dst, const Matrix4 &m) const;
        void bindGeometry(RHIContext *ctx, VertexDeclaration *decl, VertexBuffer *vb);
        void renderShadowCube(RHIContext *ctx);
        void renderScene(RHIContext *ctx);

        CameraPtr mCamera {nullptr};

        ShaderVariantPtr mShadowVS {nullptr};
        ShaderVariantPtr mShadowPS {nullptr};
        ShaderVariantPtr mSceneVS {nullptr};
        ShaderVariantPtr mScenePS {nullptr};
        TArray<ShaderPtr> mDummyShaders;

        VertexDeclarationPtr mShadowDecl {nullptr};
        VertexDeclarationPtr mSceneDecl {nullptr};
        VertexBufferPtr mRoomVB {nullptr};
        VertexBufferPtr mMarkerVB {nullptr};
        uint32_t mRoomVertexCount {0};

        ConstantBufferPtr mShadowCB {nullptr};
        ConstantBufferPtr mSceneCB {nullptr};

        RasterizerStatePtr mRasterizer {nullptr};
        DepthStencilStatePtr mDepthState {nullptr};
        BlendStatePtr mBlendState {nullptr};
        SamplerStatePtr mShadowSampler {nullptr};

        /// (d, d^2) moments, full mip chain refreshed by generateMips() after the six faces
        RenderTexturePtr mDistanceCube {nullptr};
        RenderTexturePtr mDepthCube {nullptr};
        RenderTargetPtr mFaceTargets[6] {};

        bool mInitialized {false};
        bool mReady {false};
        bool mGLFamily {false};

        Real mElapsed {0.0f};
        Vector3 mLightPos {0.0f, 2.5f, 0.0f};
        Real mLightRange {16.0f};
        Real mShadowMip {1.5f};
    };
}


#endif  /*__POINT_LIGHT_SHADOW_BEHAVIOUR_H__*/
