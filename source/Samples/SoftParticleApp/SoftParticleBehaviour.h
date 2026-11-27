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

#ifndef __SOFT_PARTICLE_BEHAVIOUR_H__
#define __SOFT_PARTICLE_BEHAVIOUR_H__


#include "Behaviour/T3DCameraBehaviour.h"
#include "Component/T3DCamera.h"


namespace Tiny3D
{
    /**
     * \brief Campfire smoke and flames drawn as soft particles with raw RHI calls
     * \remarks
     *   The opaque camp goes into a color texture plus a sampleable D24S8 depth texture.
     *   The particle pass binds a second RenderTarget over the same two textures with the
     *   depth stencil marked read-only, so depth testing still hides smoke behind the tent
     *   while the pixel shader reads the same depth to fade particles where they meet the
     *   ground, the tent and the trunks. The result is copied into the camera target.
     *   Without read-only depth support the particles are drawn hard, without sampling depth.
     */
    TCLASS()
    class SoftParticleBehaviour : public CameraBehaviour
    {
        TRTTI_ENABLE(CameraBehaviour)
        TRTTI_FRIEND

    public:
        ~SoftParticleBehaviour() override;

    protected:
        SoftParticleBehaviour() = default;
        explicit SoftParticleBehaviour(const UUID &uuid);

        void onAwake() override;
        void onUpdate() override;
        void onDestroy() override;
        void onPostRender(RHIContext *ctx) override;

    private:
        struct SceneVertex
        {
            Vector3 position;
            Vector3 normal;
            Vector3 color;
        };

        struct ParticleVertex
        {
            Vector3 position;
            float color[4];
            /// xy - corner uv, z - soft fade distance
            Vector3 params;
        };

        struct Particle
        {
            Vector3 position;
            Vector3 velocity;
            Real age {0.0f};
            Real life {1.0f};
            Real seed {0.0f};
        };

        static constexpr uint32_t kSmokeCount = 150;
        static constexpr uint32_t kFireCount = 80;
        static constexpr uint32_t kParticleVertexCount = (kSmokeCount + kFireCount) * 6;

        bool setup(RHIContext *ctx);
        void teardown();
        void buildCamp(TArray<SceneVertex> &vertices) const;
        Real random();
        void respawnSmoke(Particle &p);
        void respawnFire(Particle &p);
        void simulate(Real dt);
        Vector3 fireLightColor() const;
        void buildParticles(TArray<ParticleVertex> &vertices, uint32_t &smokeVertexCount) const;
        TResult uploadCb(ConstantBuffer *cb, const void *data, size_t bytes);
        void packMatrix(float *dst, const Matrix4 &m) const;
        void renderOpaque(RHIContext *ctx);
        void renderParticles(RHIContext *ctx);

        CameraPtr mCamera {nullptr};

        ShaderVariantPtr mSceneVS {nullptr};
        ShaderVariantPtr mScenePS {nullptr};
        ShaderVariantPtr mParticleVS {nullptr};
        ShaderVariantPtr mParticlePS {nullptr};
        TArray<ShaderPtr> mDummyShaders;

        VertexDeclarationPtr mSceneDecl {nullptr};
        VertexDeclarationPtr mParticleDecl {nullptr};
        VertexBufferPtr mCampVB {nullptr};
        VertexBufferPtr mParticleVB {nullptr};
        uint32_t mCampVertexCount {0};

        ConstantBufferPtr mSceneCB {nullptr};
        ConstantBufferPtr mParticleCB {nullptr};

        RasterizerStatePtr mRasterizer {nullptr};
        DepthStencilStatePtr mOpaqueDepth {nullptr};
        DepthStencilStatePtr mParticleDepth {nullptr};
        BlendStatePtr mOpaqueBlend {nullptr};
        BlendStatePtr mSmokeBlend {nullptr};
        BlendStatePtr mFireBlend {nullptr};
        SamplerStatePtr mPointSampler {nullptr};

        RenderTexturePtr mColorTex {nullptr};
        /// Written by the opaque pass, sampled by the particle pass
        RenderTexturePtr mDepthTex {nullptr};
        RenderTargetPtr mOpaqueTarget {nullptr};
        /// Same textures as mOpaqueTarget, depth stencil read-only when soft particles are on
        RenderTargetPtr mParticleTarget {nullptr};

        TArray<Particle> mSmoke;
        TArray<Particle> mFire;

        bool mInitialized {false};
        bool mReady {false};
        bool mGLFamily {false};
        bool mSoftParticles {false};

        Real mElapsed {0.0f};
        Real mSimTime {0.0f};
        uint32_t mRandomState {0x2545F491u};
        Vector3 mEye {6.0f, 2.0f, -4.0f};
        Vector3 mTarget {0.5f, 1.2f, 0.5f};
        Vector3 mWind {0.7f, 0.0f, 0.3f};
        Real mFlicker {1.0f};
    };
}


#endif  /*__SOFT_PARTICLE_BEHAVIOUR_H__*/
