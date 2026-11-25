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

#ifndef __HDR_BLOOM_TONEMAP_BEHAVIOUR_H__
#define __HDR_BLOOM_TONEMAP_BEHAVIOUR_H__


#include "Behaviour/T3DCameraEffectBehaviour.h"
#include "HDRShaderSources.h"


namespace Tiny3D
{
    /**
     * \brief Bloom and ACES tone mapping for an HDR camera
     * \remarks
     *   Chain: bright pass to half resolution, box downsample, tent upsample back,
     *   then composite with the scene and tone map into the destination.
     *   Each draw has its own material so per-pass constants never overwrite each other.
     *   Bloom levels keep the source format; when that format cannot be linearly
     *   filtered as a render target, the effect falls back to a plain copy.
     */
    TCLASS()
    class HDRBloomTonemapBehaviour : public CameraEffectBehaviour
    {
        TRTTI_ENABLE(CameraEffectBehaviour)
        TRTTI_FRIEND

    public:
        ~HDRBloomTonemapBehaviour() override;

        TPROPERTY(RTTRFuncName="Exposure", RTTRFuncType="getter")
        Real getExposure() const { return mExposure; }

        TPROPERTY(RTTRFuncName="Exposure", RTTRFuncType="setter")
        void setExposure(Real exposure) { mExposure = exposure; }

        TPROPERTY(RTTRFuncName="Threshold", RTTRFuncType="getter")
        Real getThreshold() const { return mThreshold; }

        TPROPERTY(RTTRFuncName="Threshold", RTTRFuncType="setter")
        void setThreshold(Real threshold) { mThreshold = threshold; }

        TPROPERTY(RTTRFuncName="BloomIntensity", RTTRFuncType="getter")
        Real getBloomIntensity() const { return mBloomIntensity; }

        TPROPERTY(RTTRFuncName="BloomIntensity", RTTRFuncType="setter")
        void setBloomIntensity(Real intensity) { mBloomIntensity = intensity; }

        void onRenderImage(RHIContext *ctx,
                           RenderTexture *src,
                           RenderTexture *dst,
                           RenderTexture *depth) override;

    protected:
        HDRBloomTonemapBehaviour() = default;
        explicit HDRBloomTonemapBehaviour(const UUID &uuid);

        void onAwake() override;
        void onDestroy() override;

    private:
        static constexpr uint32_t kMaxLevels = 6;

        ShaderPtr buildShader(const String &name, HDRShader pixelShader);
        MaterialPtr buildMaterial(Shader *shader, const String &name);
        bool ensureLevels(RenderTexture *src);
        void releaseLevels();
        void setPassParams(Material *material, RenderTexture *dst, const Vector4 &params);
        void bindBloom(Material *material, RenderTexture *bloom);

        ShaderPtr mPrefilterShader {nullptr};
        ShaderPtr mDownsampleShader {nullptr};
        ShaderPtr mUpsampleShader {nullptr};
        ShaderPtr mTonemapShader {nullptr};

        MaterialPtr mPrefilterMaterial {nullptr};
        MaterialPtr mDownsampleMaterials[kMaxLevels] {};
        MaterialPtr mUpsampleMaterials[kMaxLevels] {};
        MaterialPtr mTonemapMaterial {nullptr};

        /// mLevels[0] is the half resolution bright pass, each next level halves again
        RenderTexturePtr mLevels[kMaxLevels] {};
        /// mUpsampled[i] = mLevels[i] + blurred coarser level, same size as mLevels[i]
        RenderTexturePtr mUpsampled[kMaxLevels] {};
        uint32_t mLevelCount {0};
        uint32_t mSrcWidth {0};
        uint32_t mSrcHeight {0};
        PixelFormat mSrcFormat {PixelFormat::E_PF_UNKNOWN};

        bool mReady {false};
        bool mWarnedFallback {false};

        Real mExposure {1.0f};
        Real mThreshold {1.0f};
        Real mBloomIntensity {0.6f};
    };
}


#endif  /*__HDR_BLOOM_TONEMAP_BEHAVIOUR_H__*/
