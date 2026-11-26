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


#include "HDRBloomTonemapBehaviour.h"
#include "HDRBloomComputeSources.h"
#include "Kernel/T3DAgent.h"
#include "Kernel/T3DPixelFormat.h"
#include "Material/T3DPass.h"
#include "Material/T3DShaderVariant.h"
#include "Material/T3DTechnique.h"
#include "Render/T3DForwardRenderPipeline.h"
#include "Render/T3DRenderConstant.h"
#include "Render/T3DRenderState.h"
#include "Render/T3DRenderTexture.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DShader.h"
#include "Resource/T3DShaderManager.h"
#include "Resource/T3DTextureManager.h"
#include "RHI/T3DRHIContext.h"
#include "RHI/T3DRHICapabilities.h"
#include "T3DErrorDef.h"

#define LOG_TAG_APP     "APP"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    HDRBloomTonemapBehaviour::HDRBloomTonemapBehaviour(const UUID &uuid)
        : CameraEffectBehaviour(uuid)
    {
    }

    //--------------------------------------------------------------------------

    HDRBloomTonemapBehaviour::~HDRBloomTonemapBehaviour() = default;

    //--------------------------------------------------------------------------

    void HDRBloomTonemapBehaviour::onAwake()
    {
        mPrefilterShader = buildShader("Prefilter", HDRShader::kBloomPrefilterPS);
        mDownsampleShader = buildShader("Downsample", HDRShader::kBloomDownsamplePS);
        mUpsampleShader = buildShader("Upsample", HDRShader::kBloomUpsamplePS);
        mTonemapShader = buildShader("Tonemap", HDRShader::kTonemapPS);

        if (mPrefilterShader == nullptr || mDownsampleShader == nullptr
            || mUpsampleShader == nullptr || mTonemapShader == nullptr)
        {
            T3D_LOG_WARNING(LOG_TAG_APP, "HDRBloomTonemap : shaders unavailable for this renderer, the effect will copy the image unchanged.");
            return;
        }

        mPrefilterMaterial = buildMaterial(mPrefilterShader, "Prefilter");
        mTonemapMaterial = buildMaterial(mTonemapShader, "Tonemap");
        mReady = (mPrefilterMaterial != nullptr && mTonemapMaterial != nullptr);
        for (uint32_t i = 0; i < kMaxLevels && mReady; ++i)
        {
            mDownsampleMaterials[i] = buildMaterial(mDownsampleShader, "Downsample" + std::to_string(i));
            mUpsampleMaterials[i] = buildMaterial(mUpsampleShader, "Upsample" + std::to_string(i));
            mReady = (mDownsampleMaterials[i] != nullptr && mUpsampleMaterials[i] != nullptr);
        }
    }

    //--------------------------------------------------------------------------

    void HDRBloomTonemapBehaviour::onDestroy()
    {
        releaseLevels();
        CameraEffectBehaviour::onDestroy();
    }

    //--------------------------------------------------------------------------

    ShaderPtr HDRBloomTonemapBehaviour::buildShader(const String &name, HDRShader pixelShader)
    {
        ShaderVariantPtr vshader = createHDRShaderVariant(HDRShader::kPostVS, SHADER_STAGE::kVertex);
        ShaderVariantPtr pshader = createHDRShaderVariant(pixelShader, SHADER_STAGE::kPixel);
        if (vshader == nullptr || pshader == nullptr)
        {
            return nullptr;
        }

        RenderStatePtr renderState = RenderState::create();
        BlendDesc blendDesc;
        renderState->setBlendDesc(blendDesc);
        DepthStencilDesc depthStencilDesc;
        depthStencilDesc.DepthTestEnable = false;
        depthStencilDesc.DepthWriteEnable = false;
        renderState->setDepthStencilDesc(depthStencilDesc);
        RasterizerDesc rasterizerDesc;
        rasterizerDesc.CullMode = CullingMode::kNone;
        renderState->setRasterizerDesc(rasterizerDesc);

        PassPtr pass = Pass::create("ForwardBase");
        if (T3D_FAILED(pass->addShaderVariant(vshader->getShaderKeyword(), vshader))
            || T3D_FAILED(pass->addShaderVariant(pshader->getShaderKeyword(), pshader)))
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "HDRBloomTonemap : failed to add shader variants for %s", name.c_str());
            return nullptr;
        }
        pass->addTag(ShaderLab::kBuiltinTagLightMode, ShaderLab::kBuiltinLightModeForwardBaseStr);
        pass->setRenderState(renderState);

        TechniquePtr tech = Technique::create("HDR-" + name + "-Technique");
        if (!tech->addPass(pass))
        {
            return nullptr;
        }

        ShaderPtr shader = T3D_SHADER_MGR.createShader("Hidden-HDR-" + name + "-" + getUUID().toString());
        if (shader == nullptr || !shader->addTechnique(tech))
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "HDRBloomTonemap : failed to create shader %s", name.c_str());
            return nullptr;
        }
        return shader;
    }

    //--------------------------------------------------------------------------

    MaterialPtr HDRBloomTonemapBehaviour::buildMaterial(Shader *shader, const String &name)
    {
        MaterialPtr material = T3D_MATERIAL_MGR.createMaterial("Hidden-HDR-" + name + "-" + getUUID().toString(), shader);
        if (material == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "HDRBloomTonemap : failed to create material %s", name.c_str());
            return nullptr;
        }

        StringArray enableKeywords;
        enableKeywords.push_back("");
        StringArray disableKeywords;
        material->switchKeywords(enableKeywords, disableKeywords);
        return material;
    }

    //--------------------------------------------------------------------------

    bool HDRBloomTonemapBehaviour::ensureLevels(RenderTexture *src)
    {
        const uint32_t width = src->getWidth();
        const uint32_t height = src->getHeight();
        const PixelFormat format = src->getPixelFormat();
        if (width == mSrcWidth && height == mSrcHeight && format == mSrcFormat)
        {
            return mLevelCount > 0;
        }

        releaseLevels();
        mSrcWidth = width;
        mSrcHeight = height;
        mSrcFormat = format;

        const RHICapabilities &caps = T3D_AGENT.getActiveRHIContext()->getCapabilities();
        const uint32_t required = kFormatSupportTexture | kFormatSupportRenderTarget | kFormatSupportLinearFilter;
        if (!caps.supportsFormat(format, required))
        {
            T3D_LOG_WARNING(LOG_TAG_APP, "HDRBloomTonemap : %s cannot be a linearly filtered render target here, bloom is disabled and the image is copied unchanged.",
                PixelFormatUtil::getName(format));
            return false;
        }

        // Linear filtering only matters where a level is read as _BloomTex;
        // drawFullscreen switches every _MainTex source back to point sampling itself.
        SamplerDesc samplerDesc;
        samplerDesc.MipFilter = FilterOptions::kNone;
        samplerDesc.AddressU = TextureAddressMode::kClamp;
        samplerDesc.AddressV = TextureAddressMode::kClamp;
        samplerDesc.AddressW = TextureAddressMode::kClamp;

        uint32_t levels = 0;
        for (uint32_t lw = width / 2, lh = height / 2; levels < kMaxLevels && lw >= 8 && lh >= 8; lw /= 2, lh /= 2)
        {
            ++levels;
        }

        if (levels >= 2 && ensureComputeChains(width / 2, height / 2, format, levels))
        {
            mDownChain->setSamplerDesc(samplerDesc);
            mUpChain->setSamplerDesc(samplerDesc);
            mLevelCount = levels;
            mUseCompute = true;
            T3D_LOG_INFO(LOG_TAG_APP, "HDRBloomTonemap : %u bloom mips in %s for a %ux%u source, downsample / upsample run as compute.",
                mLevelCount, PixelFormatUtil::getName(format), width, height);
            return true;
        }

        uint32_t w = width / 2;
        uint32_t h = height / 2;
        while (mLevelCount < kMaxLevels && w >= 8 && h >= 8)
        {
            const String suffix = std::to_string(mLevelCount) + "-" + getUUID().toString();
            mLevels[mLevelCount] = T3D_TEXTURE_MGR.createRenderTexture("HDR-BloomDown-" + suffix, w, h, format, 1, 1, 0, true, kCPUNone);
            mUpsampled[mLevelCount] = T3D_TEXTURE_MGR.createRenderTexture("HDR-BloomUp-" + suffix, w, h, format, 1, 1, 0, true, kCPUNone);
            if (mLevels[mLevelCount] == nullptr || mUpsampled[mLevelCount] == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_APP, "HDRBloomTonemap : failed to create %ux%u %s bloom level", w, h, PixelFormatUtil::getName(format));
                ++mLevelCount;
                releaseLevels();
                return false;
            }
            mLevels[mLevelCount]->setSamplerDesc(samplerDesc);
            mUpsampled[mLevelCount]->setSamplerDesc(samplerDesc);
            ++mLevelCount;
            w /= 2;
            h /= 2;
        }

        if (mLevelCount < 2)
        {
            T3D_LOG_WARNING(LOG_TAG_APP, "HDRBloomTonemap : %ux%u is too small for bloom, copying the image unchanged.", width, height);
            releaseLevels();
            return false;
        }

        T3D_LOG_INFO(LOG_TAG_APP, "HDRBloomTonemap : %u bloom levels in %s for a %ux%u source.",
            mLevelCount, PixelFormatUtil::getName(format), width, height);
        return true;
    }

    //--------------------------------------------------------------------------

    bool HDRBloomTonemapBehaviour::ensureComputeChains(uint32_t width, uint32_t height, PixelFormat format, uint32_t levels)
    {
        RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
        const RHICapabilities &caps = ctx->getCapabilities();
        if (!caps.supportsCompute || !caps.supportsUnorderedAccess || !caps.supportsTextureSubresourceViews
            || !caps.supportsFormat(format, kFormatSupportUnorderedAccess))
        {
            return false;
        }

        if (mKernelFormat != format)
        {
            mDownsampleKernel = createHDRBloomKernel(HDRBloomKernel::kDownsample, format, mKernelShaders[0]);
            mUpsampleKernel = createHDRBloomKernel(HDRBloomKernel::kUpsample, format, mKernelShaders[1]);
            mKernelFormat = format;
        }
        if (mDownsampleKernel == nullptr || mUpsampleKernel == nullptr)
        {
            return false;
        }

        const String suffix = getUUID().toString();
        mDownChain = T3D_TEXTURE_MGR.createRenderTexture("HDR-BloomDownChain-" + suffix, TEXTURE_TYPE::TT_2D,
            width, height, 1, format, levels, true, kCPUNone, kGPUUnorderedAccess);
        mUpChain = T3D_TEXTURE_MGR.createRenderTexture("HDR-BloomUpChain-" + suffix, TEXTURE_TYPE::TT_2D,
            width, height, 1, format, levels, true, kCPUNone, kGPUUnorderedAccess);
        if (mDownChain == nullptr || mUpChain == nullptr)
        {
            T3D_LOG_WARNING(LOG_TAG_APP, "HDRBloomTonemap : failed to create %ux%u %s mip chains, using pixel shader levels.",
                width, height, PixelFormatUtil::getName(format));
            releaseLevels();
            return false;
        }
        return true;
    }

    //--------------------------------------------------------------------------

    void HDRBloomTonemapBehaviour::releaseLevels()
    {
        if (mDownChain != nullptr)
        {
            T3D_TEXTURE_MGR.unload(mDownChain);
            mDownChain = nullptr;
        }
        if (mUpChain != nullptr)
        {
            T3D_TEXTURE_MGR.unload(mUpChain);
            mUpChain = nullptr;
        }
        mUseCompute = false;

        for (uint32_t i = 0; i < mLevelCount; ++i)
        {
            if (mLevels[i] != nullptr)
            {
                T3D_TEXTURE_MGR.unload(mLevels[i]);
                mLevels[i] = nullptr;
            }
            if (mUpsampled[i] != nullptr)
            {
                T3D_TEXTURE_MGR.unload(mUpsampled[i]);
                mUpsampled[i] = nullptr;
            }
        }
        mLevelCount = 0;
    }

    //--------------------------------------------------------------------------

    void HDRBloomTonemapBehaviour::setPassParams(Material *material, RenderTexture *dst, const Vector4 &params)
    {
        material->setVector("hdr_PostParams", params);
        material->setVector("hdr_TexelParams", Vector4(REAL_ONE / Real(dst->getWidth()), REAL_ONE / Real(dst->getHeight()), REAL_ZERO, REAL_ZERO));
    }

    //--------------------------------------------------------------------------

    void HDRBloomTonemapBehaviour::bindBloom(Material *material, RenderTexture *bloom)
    {
        material->setTexture("_BloomTex", bloom->getUUID());
    }

    //--------------------------------------------------------------------------

    void HDRBloomTonemapBehaviour::onRenderImage(RHIContext *ctx,
                                                 RenderTexture *src,
                                                 RenderTexture *dst,
                                                 RenderTexture * /*depth*/)
    {
        if (!mReady || !ensureLevels(src))
        {
            if (!mWarnedFallback)
            {
                T3D_LOG_WARNING(LOG_TAG_APP, "HDRBloomTonemap : falling back to a plain copy, no bloom or tone mapping.");
                mWarnedFallback = true;
            }
            blitCopy(ctx, src, dst);
            return;
        }

        ForwardRenderPipeline &pipeline = static_cast<ForwardRenderPipeline &>(T3D_RENDER_PIPELINE);

        if (mUseCompute)
        {
            // Bright pass writes mip 0 (the default render target subresource), compute fills the rest
            setPassParams(mPrefilterMaterial, mDownChain, Vector4(mThreshold, 0.5f, REAL_ZERO, REAL_ZERO));
            pipeline.drawFullscreen(ctx, mPrefilterMaterial, src, mDownChain);

            runComputeChain(ctx);

            // The tone map samples mUpChain with MipFilter none, i.e. only mip 0
            setPassParams(mTonemapMaterial, dst, Vector4(mExposure, mBloomIntensity, REAL_ZERO, REAL_ZERO));
            bindBloom(mTonemapMaterial, mUpChain);
            pipeline.drawFullscreen(ctx, mTonemapMaterial, src, dst);
            return;
        }

        // Bright pass : scene -> level 0
        setPassParams(mPrefilterMaterial, mLevels[0], Vector4(mThreshold, 0.5f, REAL_ZERO, REAL_ZERO));
        pipeline.drawFullscreen(ctx, mPrefilterMaterial, src, mLevels[0]);

        // Downsample : level i - 1 -> level i
        for (uint32_t i = 1; i < mLevelCount; ++i)
        {
            setPassParams(mDownsampleMaterials[i], mLevels[i], Vector4::ZERO);
            pipeline.drawFullscreen(ctx, mDownsampleMaterials[i], mLevels[i - 1], mLevels[i]);
        }

        // Upsample : level i + blurred coarser result -> upsampled i
        RenderTexture *coarser = mLevels[mLevelCount - 1];
        for (int32_t i = static_cast<int32_t>(mLevelCount) - 2; i >= 0; --i)
        {
            Material *material = mUpsampleMaterials[i];
            setPassParams(material, mUpsampled[i], Vector4::ZERO);
            bindBloom(material, coarser);
            pipeline.drawFullscreen(ctx, material, mLevels[i], mUpsampled[i]);
            coarser = mUpsampled[i];
        }

        // Composite and tone map
        setPassParams(mTonemapMaterial, dst, Vector4(mExposure, mBloomIntensity, REAL_ZERO, REAL_ZERO));
        bindBloom(mTonemapMaterial, coarser);
        pipeline.drawFullscreen(ctx, mTonemapMaterial, src, dst);
    }

    //--------------------------------------------------------------------------

    TResult HDRBloomTonemapBehaviour::dispatchKernel(RHIContext *ctx, ShaderVariant *kernel, const PixelBuffers &srvs,
        const TextureSubresources &srvSubs, RenderTexture *uav, uint32_t uavMip)
    {
        TResult ret = ctx->setComputeShader(kernel);
        if (T3D_SUCCEEDED(ret))
        {
            ret = ctx->setCSPixelBuffers(0, srvs, srvSubs);
        }

        UnorderedAccessBuffers uavs;
        uavs.push_back(uav->getPixelBuffer());
        TextureSubresources uavSubs;
        uavSubs.push_back(TextureSubresource::mip(uavMip));
        if (T3D_SUCCEEDED(ret))
        {
            ret = ctx->setCSUnorderedAccessBuffers(0, uavs, UAVInitialCounts(), uavSubs);
        }

        if (T3D_SUCCEEDED(ret))
        {
            uint32_t gx = 8, gy = 8, gz = 1;
            kernel->getThreadGroupSize(gx, gy, gz);
            gx = gx == 0 ? 8 : gx;
            gy = gy == 0 ? 8 : gy;
            const uint32_t w = TextureSubresource::getMipSize(uav->getWidth(), uavMip);
            const uint32_t h = TextureSubresource::getMipSize(uav->getHeight(), uavMip);
            ret = ctx->dispatch((w + gx - 1) / gx, (h + gy - 1) / gy, 1);
        }

        // The next dispatch reads this mip as an SRV : clear the slots instead of a plain
        // uavBarrier, which rebinds the UAV and keeps the hazard alive on D3D11
        ctx->uavBarrier(uavs);
        ctx->setCSUnorderedAccessBuffers(0, UnorderedAccessBuffers(1, nullptr));
        ctx->setCSPixelBuffers(0, PixelBuffers(srvs.size(), nullptr));
        return ret;
    }

    //--------------------------------------------------------------------------

    void HDRBloomTonemapBehaviour::runComputeChain(RHIContext *ctx)
    {
        PixelBuffer *down = mDownChain->getPixelBuffer();
        PixelBuffer *up = mUpChain->getPixelBuffer();

        // Downsample : down mip i - 1 -> down mip i, read and write views never overlap
        for (uint32_t i = 1; i < mLevelCount; ++i)
        {
            PixelBuffers srvs;
            srvs.push_back(down);
            TextureSubresources subs;
            subs.push_back(TextureSubresource::mip(i - 1));
            if (T3D_FAILED(dispatchKernel(ctx, mDownsampleKernel.get(), srvs, subs, mDownChain, i)))
            {
                break;
            }
        }

        // Upsample : down mip i + blurred coarser level -> up mip i
        for (int32_t i = static_cast<int32_t>(mLevelCount) - 2; i >= 0; --i)
        {
            const bool coarsestStep = (i == static_cast<int32_t>(mLevelCount) - 2);
            PixelBuffers srvs;
            srvs.push_back(down);
            srvs.push_back(coarsestStep ? down : up);
            TextureSubresources subs;
            subs.push_back(TextureSubresource::mip(i));
            subs.push_back(TextureSubresource::mip(i + 1));
            if (T3D_FAILED(dispatchKernel(ctx, mUpsampleKernel.get(), srvs, subs, mUpChain, i)))
            {
                break;
            }
        }

        ctx->setComputeShader(nullptr);
    }

    //--------------------------------------------------------------------------
}
