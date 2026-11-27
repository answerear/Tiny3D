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

#ifndef __OIT_BEHAVIOUR_H__
#define __OIT_BEHAVIOUR_H__


#include "Behaviour/T3DCameraBehaviour.h"
#include "Component/T3DCamera.h"


namespace Tiny3D
{
    /**
     * \brief Bar counter with a stained glass window and a glass display case, drawn with raw RHI calls
     * \remarks
     *   The stained glass window uses dual source blending: the pixel shader writes the
     *   light the glass scatters to SV_Target0 and the per channel transmittance to
     *   SV_Target1, blended as dst = src0 + dst * src1, so the sunlit yard behind a red pane
     *   turns red and behind a blue pane turns blue. Without dual source support the panes
     *   only multiply the transmittance into the target and lose the scattered light.
     *
     *   The display case glass is blended with the blend constant (src * c + dst * (1 - c)).
     *   The constant follows the distance from the camera to the case, so the glass fades out
     *   as the camera walks up to it and fades back in as it leaves; the BlendState never changes.
     */
    TCLASS()
    class OITBehaviour : public CameraBehaviour
    {
        TRTTI_ENABLE(CameraBehaviour)
        TRTTI_FRIEND

    public:
        ~OITBehaviour() override;

    protected:
        OITBehaviour() = default;
        explicit OITBehaviour(const UUID &uuid);

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

        bool setup(RHIContext *ctx);
        void teardown();
        void buildBar(TArray<SceneVertex> &opaque, TArray<SceneVertex> &window, TArray<SceneVertex> &caseGlass) const;
        VertexBufferPtr createVB(const TArray<SceneVertex> &vertices) const;
        Real caseGlassOpacity() const;
        TResult uploadCb(ConstantBuffer *cb, const void *data, size_t bytes);
        void packMatrix(float *dst, const Matrix4 &m) const;
        void drawVB(RHIContext *ctx, const VertexBufferPtr &vb, uint32_t vertexCount);
        void render(RHIContext *ctx);

        CameraPtr mCamera {nullptr};

        ShaderVariantPtr mSceneVS {nullptr};
        ShaderVariantPtr mScenePS {nullptr};
        ShaderVariantPtr mWindowPS {nullptr};
        ShaderVariantPtr mCaseGlassPS {nullptr};
        TArray<ShaderPtr> mDummyShaders;

        VertexDeclarationPtr mSceneDecl {nullptr};
        VertexBufferPtr mOpaqueVB {nullptr};
        VertexBufferPtr mWindowVB {nullptr};
        VertexBufferPtr mCaseGlassVB {nullptr};
        uint32_t mOpaqueVertexCount {0};
        uint32_t mWindowVertexCount {0};
        uint32_t mCaseGlassVertexCount {0};

        ConstantBufferPtr mSceneCB {nullptr};

        RasterizerStatePtr mRasterizer {nullptr};
        DepthStencilStatePtr mOpaqueDepth {nullptr};
        DepthStencilStatePtr mGlassDepth {nullptr};
        BlendStatePtr mOpaqueBlend {nullptr};
        BlendStatePtr mWindowBlend {nullptr};
        BlendStatePtr mCaseGlassBlend {nullptr};

        RenderTexturePtr mColorTex {nullptr};
        RenderTexturePtr mDepthTex {nullptr};
        RenderTargetPtr mTarget {nullptr};

        bool mInitialized {false};
        bool mReady {false};
        bool mGLFamily {false};
        bool mDualSource {false};

        Real mElapsed {0.0f};
        Real mNextLog {0.0f};
        Vector3 mEye {-6.5f, 1.7f, -4.5f};
        Vector3 mLookAt {-1.0f, 1.6f, 3.0f};
    };
}


#endif  /*__OIT_BEHAVIOUR_H__*/
