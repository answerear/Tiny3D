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

#ifndef __SRGB_TEST_BEHAVIOUR_H__
#define __SRGB_TEST_BEHAVIOUR_H__


#include "Behaviour/T3DCameraEffectBehaviour.h"
#include "SRGBShaderSources.h"


namespace Tiny3D
{
    /**
     * \brief Replaces the camera image with the sRGB test chart and runs the readback checks
     * \remarks
     *   The chart has four rows, each putting a reference next to the path under test :
     *   sRGB texture decoding, R8G8B8A8 / B8G8R8A8 channel order, 8-bit render target
     *   precision and bilinear filtering in linear space. The camera buffer and the back
     *   buffer are UNORM, so the composite encodes linear -> sRGB in the shader.
     *   The readback checks compare the bytes the GPU actually stored or produced
     *   against values computed on the CPU and log PASS / FAIL for each of them.
     */
    TCLASS()
    class SRGBTestBehaviour : public CameraEffectBehaviour
    {
        TRTTI_ENABLE(CameraEffectBehaviour)
        TRTTI_FRIEND

    public:
        ~SRGBTestBehaviour() override;

        void onRenderImage(RHIContext *ctx,
                           RenderTexture *src,
                           RenderTexture *dst,
                           RenderTexture *depth) override;

        /**
         * \brief Record the readback copies of every check target
         * \remarks Call from Application::onRender once the check passes have run at least once
         */
        void requestReadback();

        /**
         * \brief Consume the readbacks, compare and log the results
         * \remarks Call from Application::onPostRender in the frame requestReadback ran
         */
        void verifyReadback();

        bool isReadbackPending() const { return mReadbackPending; }
        bool isVerified() const { return mVerified; }
        uint32_t getFramesRendered() const { return mFramesRendered; }

    protected:
        SRGBTestBehaviour() = default;
        explicit SRGBTestBehaviour(const UUID &uuid);

        void onAwake() override;
        void onDestroy() override;

    private:
        struct Check
        {
            String name;
            /// Texture2D for the raw-storage checks, RenderTexture written by material otherwise
            TexturePtr target {nullptr};
            /// Pattern pass that fills target every frame, nullptr when target is only read back
            MaterialPtr material {nullptr};
            PixelFormat format {PixelFormat::E_PF_UNKNOWN};
            uint32_t width {0};
            uint32_t height {0};
            /// RGBA per pixel whatever the target's byte order
            TArray<uint8_t> expected;
            int32_t tolerance {0};
            String skipReason;
            ReadbackHandle handle {};
        };

        ShaderPtr buildShader(const String &name, SRGBShader pixelShader);
        MaterialPtr buildMaterial(Shader *shader, const String &name);
        MaterialPtr buildPatternMaterial(const String &name, const Vector4 &params, Texture *source);

        Texture2DPtr buildTexture(const String &name, PixelFormat format, uint32_t width, uint32_t height,
                                  const TArray<uint8_t> &rgba, bool linearFilter, uint32_t cpuAccess);
        RenderTexturePtr buildTarget(const String &name, PixelFormat format, uint32_t width, uint32_t height,
                                     uint32_t gpuAccess, String &skipReason);

        void buildChart();
        void buildChecks();

        ShaderPtr mPatternShader {nullptr};
        ShaderPtr mCompositeShader {nullptr};
        MaterialPtr mCompositeMaterial {nullptr};

        TArray<TexturePtr> mTextures;
        MaterialPtr mStoreMaterials[2] {};
        RenderTexturePtr mStoreTargets[2] {};
        TArray<Check> mChecks;

        bool mReady {false};
        bool mReadbackPending {false};
        bool mVerified {false};
        uint32_t mFramesRendered {0};
    };

    T3D_DECLARE_SMART_PTR(SRGBTestBehaviour);
}


#endif  /*__SRGB_TEST_BEHAVIOUR_H__*/
