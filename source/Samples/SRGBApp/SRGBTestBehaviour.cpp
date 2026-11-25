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


#include "SRGBTestBehaviour.h"
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

#include <cmath>
#include <cstdlib>

#define LOG_TAG_APP     "APP"


namespace Tiny3D
{
    namespace
    {
        /// Mode 0 of the pattern shader writes column c as c / 255, so the check targets are 256 wide
        const uint32_t kCheckWidth = 256;
        const uint32_t kCheckHeight = 4;
        const uint32_t kCheckerSize = 64;
        const uint32_t kAtlasHeight = 8;
        const uint32_t kStoreWidth = 512;
        const uint32_t kStoreHeight = 4;
        /// Linear peak of the dark ramp in row 2; an 8-bit UNORM target keeps only about 20 levels of it
        const Real kDarkPeak = 0.08f;

        const Real kModeValues = 0.0f;
        const Real kModeDarkRamp = 1.0f;
        const Real kModeSample = 2.0f;

        Real srgbToLinear(Real c)
        {
            return (c <= 0.04045f) ? c / 12.92f : std::pow((c + 0.055f) / 1.055f, 2.4f);
        }

        Real linearToSrgb(Real v)
        {
            v = std::max(0.0f, std::min(1.0f, v));
            return (v <= 0.0031308f) ? v * 12.92f : 1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f;
        }

        uint8_t toByte(Real v)
        {
            return static_cast<uint8_t>(std::lround(std::max(0.0f, std::min(1.0f, v)) * 255.0f));
        }

        bool isBGRA(PixelFormat format)
        {
            return format == PixelFormat::E_PF_B8G8R8A8 || format == PixelFormat::E_PF_B8G8R8A8_SRGB;
        }

        /// Colours of the eight bars in row 1, as sRGB bytes
        const uint8_t kBars[8][3] =
        {
            { 230,  60,  50 }, {  60, 180,  75 }, {  40,  90, 220 }, { 250, 200,  40 },
            {  70, 200, 210 }, { 200,  70, 180 }, { 128, 128, 128 }, { 255, 140,  20 },
        };
    }

    //--------------------------------------------------------------------------

    SRGBTestBehaviour::SRGBTestBehaviour(const UUID &uuid)
        : CameraEffectBehaviour(uuid)
    {
    }

    //--------------------------------------------------------------------------

    SRGBTestBehaviour::~SRGBTestBehaviour() = default;

    //--------------------------------------------------------------------------

    void SRGBTestBehaviour::onAwake()
    {
        mPatternShader = buildShader("Pattern", SRGBShader::kPatternPS);
        mCompositeShader = buildShader("Composite", SRGBShader::kCompositePS);
        if (mPatternShader == nullptr || mCompositeShader == nullptr)
        {
            T3D_LOG_WARNING(LOG_TAG_APP, "SRGBApp : shaders unavailable for this renderer, the camera image is copied unchanged.");
            return;
        }

        buildChart();
        buildChecks();
    }

    //--------------------------------------------------------------------------

    void SRGBTestBehaviour::onDestroy()
    {
        for (Check &check : mChecks)
        {
            RenderTexture *rt = (check.material != nullptr) ? static_cast<RenderTexture *>(check.target.get()) : nullptr;
            if (rt != nullptr)
            {
                T3D_TEXTURE_MGR.unload(rt);
            }
        }
        mChecks.clear();

        for (RenderTexturePtr &rt : mStoreTargets)
        {
            if (rt != nullptr)
            {
                T3D_TEXTURE_MGR.unload(rt);
                rt = nullptr;
            }
        }
        mTextures.clear();
        CameraEffectBehaviour::onDestroy();
    }

    //--------------------------------------------------------------------------

    ShaderPtr SRGBTestBehaviour::buildShader(const String &name, SRGBShader pixelShader)
    {
        ShaderVariantPtr vshader = createSRGBShaderVariant(SRGBShader::kPostVS, SHADER_STAGE::kVertex);
        ShaderVariantPtr pshader = createSRGBShaderVariant(pixelShader, SHADER_STAGE::kPixel);
        if (vshader == nullptr || pshader == nullptr)
        {
            return nullptr;
        }

        RenderStatePtr renderState = RenderState::create();
        renderState->setBlendDesc(BlendDesc());
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
            T3D_LOG_ERROR(LOG_TAG_APP, "SRGBApp : failed to add shader variants for %s", name.c_str());
            return nullptr;
        }
        pass->addTag(ShaderLab::kBuiltinTagLightMode, ShaderLab::kBuiltinLightModeForwardBaseStr);
        pass->setRenderState(renderState);

        TechniquePtr tech = Technique::create("SRGB-" + name + "-Technique");
        if (!tech->addPass(pass))
        {
            return nullptr;
        }

        ShaderPtr shader = T3D_SHADER_MGR.createShader("Hidden-SRGB-" + name + "-" + getUUID().toString());
        if (shader == nullptr || !shader->addTechnique(tech))
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "SRGBApp : failed to create shader %s", name.c_str());
            return nullptr;
        }
        return shader;
    }

    //--------------------------------------------------------------------------

    MaterialPtr SRGBTestBehaviour::buildMaterial(Shader *shader, const String &name)
    {
        MaterialPtr material = T3D_MATERIAL_MGR.createMaterial("Hidden-SRGB-" + name + "-" + getUUID().toString(), shader);
        if (material == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "SRGBApp : failed to create material %s", name.c_str());
            return nullptr;
        }

        StringArray enableKeywords;
        enableKeywords.push_back("");
        StringArray disableKeywords;
        material->switchKeywords(enableKeywords, disableKeywords);
        return material;
    }

    //--------------------------------------------------------------------------

    MaterialPtr SRGBTestBehaviour::buildPatternMaterial(const String &name, const Vector4 &params, Texture *source)
    {
        MaterialPtr material = buildMaterial(mPatternShader, name);
        if (material == nullptr)
        {
            return nullptr;
        }

        material->setVector("srgb_PassParams", params);
        // The shader samples _SrcTex in every mode, so it always needs something bound
        Texture *bound = (source != nullptr) ? source : (mTextures.empty() ? nullptr : mTextures.front().get());
        if (bound != nullptr)
        {
            material->setTexture("_SrcTex", bound->getUUID());
        }
        return material;
    }

    //--------------------------------------------------------------------------

    Texture2DPtr SRGBTestBehaviour::buildTexture(const String &name, PixelFormat format, uint32_t width, uint32_t height,
                                                 const TArray<uint8_t> &rgba, bool linearFilter, uint32_t cpuAccess)
    {
        const RHICapabilities &caps = T3D_AGENT.getActiveRHIContext()->getCapabilities();
        const uint32_t required = kFormatSupportTexture | (linearFilter ? kFormatSupportLinearFilter : 0);
        if (!caps.supportsFormat(format, required))
        {
            T3D_LOG_WARNING(LOG_TAG_APP, "SRGBApp : %s cannot be a %stexture on this renderer.",
                PixelFormatUtil::getName(format), linearFilter ? "linearly filtered " : "");
            return nullptr;
        }

        Buffer data;
        data.DataSize = rgba.size();
        data.Data = T3D_POD_NEW_ARRAY(uint8_t, data.DataSize);
        const bool swap = isBGRA(format);
        for (size_t i = 0; i < rgba.size(); i += 4)
        {
            data.Data[i + 0] = swap ? rgba[i + 2] : rgba[i + 0];
            data.Data[i + 1] = rgba[i + 1];
            data.Data[i + 2] = swap ? rgba[i + 0] : rgba[i + 2];
            data.Data[i + 3] = rgba[i + 3];
        }

        Texture2DPtr texture = T3D_TEXTURE_MGR.createTexture2D(name + "-" + getUUID().toString(), width, height, format, data,
            1, 1, 0, UUID::INVALID, cpuAccess);
        if (texture == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_APP, "SRGBApp : failed to create %s texture %s", PixelFormatUtil::getName(format), name.c_str());
            return nullptr;
        }

        SamplerDesc samplerDesc;
        samplerDesc.MinFilter = linearFilter ? FilterOptions::kLinear : FilterOptions::kPoint;
        samplerDesc.MagFilter = samplerDesc.MinFilter;
        samplerDesc.MipFilter = FilterOptions::kNone;
        const TextureAddressMode address = linearFilter ? TextureAddressMode::kWrap : TextureAddressMode::kClamp;
        samplerDesc.AddressU = address;
        samplerDesc.AddressV = address;
        samplerDesc.AddressW = address;
        texture->setSamplerDesc(samplerDesc);
        mTextures.push_back(texture);
        return texture;
    }

    //--------------------------------------------------------------------------

    RenderTexturePtr SRGBTestBehaviour::buildTarget(const String &name, PixelFormat format, uint32_t width, uint32_t height,
                                                    uint32_t gpuAccess, String &skipReason)
    {
        const RHICapabilities &caps = T3D_AGENT.getActiveRHIContext()->getCapabilities();
        uint32_t required = kFormatSupportTexture | kFormatSupportRenderTarget;
        if ((gpuAccess & kGPUUnorderedAccess) != 0)
        {
            required |= kFormatSupportUnorderedAccess;
        }
        if (!caps.supportsFormat(format, required))
        {
            skipReason = String(PixelFormatUtil::getName(format)) + " is not reported as a "
                + (((gpuAccess & kGPUUnorderedAccess) != 0) ? "UAV-capable " : "") + "render target";
            return nullptr;
        }

        RenderTexturePtr rt = T3D_TEXTURE_MGR.createRenderTexture(name + "-" + getUUID().toString(), width, height, format,
            1, 1, 0, true, kCPURead, gpuAccess);
        if (rt == nullptr)
        {
            skipReason = String("failed to create the ") + PixelFormatUtil::getName(format) + " render target";
            return nullptr;
        }

        SamplerDesc samplerDesc;
        samplerDesc.MinFilter = FilterOptions::kPoint;
        samplerDesc.MagFilter = FilterOptions::kPoint;
        samplerDesc.MipFilter = FilterOptions::kNone;
        samplerDesc.AddressU = TextureAddressMode::kClamp;
        samplerDesc.AddressV = TextureAddressMode::kClamp;
        samplerDesc.AddressW = TextureAddressMode::kClamp;
        rt->setSamplerDesc(samplerDesc);
        return rt;
    }

    //--------------------------------------------------------------------------

    void SRGBTestBehaviour::buildChart()
    {
        // Atlas : rows 0-3 gray ramp 0..255, rows 4-7 eight 32 texel wide colour bars
        TArray<uint8_t> atlas(kCheckWidth * kAtlasHeight * 4);
        for (uint32_t y = 0; y < kAtlasHeight; ++y)
        {
            for (uint32_t x = 0; x < kCheckWidth; ++x)
            {
                uint8_t *p = &atlas[(y * kCheckWidth + x) * 4];
                if (y < kAtlasHeight / 2)
                {
                    p[0] = p[1] = p[2] = static_cast<uint8_t>(x);
                }
                else
                {
                    const uint8_t *bar = kBars[x / (kCheckWidth / 8)];
                    p[0] = bar[0];
                    p[1] = bar[1];
                    p[2] = bar[2];
                }
                p[3] = 255;
            }
        }

        TArray<uint8_t> checker(kCheckerSize * kCheckerSize * 4);
        for (uint32_t y = 0; y < kCheckerSize; ++y)
        {
            for (uint32_t x = 0; x < kCheckerSize; ++x)
            {
                uint8_t *p = &checker[(y * kCheckerSize + x) * 4];
                p[0] = p[1] = p[2] = ((x + y) % 2 != 0) ? 255 : 0;
                p[3] = 255;
            }
        }

        Texture2DPtr atlasUnorm = buildTexture("AtlasUnorm", PixelFormat::E_PF_R8G8B8A8, kCheckWidth, kAtlasHeight, atlas, false, kCPUNone);
        Texture2DPtr atlasRgba = buildTexture("AtlasRgbaSrgb", PixelFormat::E_PF_R8G8B8A8_SRGB, kCheckWidth, kAtlasHeight, atlas, false, kCPUNone);
        Texture2DPtr atlasBgra = buildTexture("AtlasBgraSrgb", PixelFormat::E_PF_B8G8R8A8_SRGB, kCheckWidth, kAtlasHeight, atlas, false, kCPUNone);
        Texture2DPtr checkerUnorm = buildTexture("CheckerUnorm", PixelFormat::E_PF_R8G8B8A8, kCheckerSize, kCheckerSize, checker, true, kCPUNone);
        Texture2DPtr checkerSrgb = buildTexture("CheckerSrgb", PixelFormat::E_PF_R8G8B8A8_SRGB, kCheckerSize, kCheckerSize, checker, true, kCPUNone);

        const PixelFormat storeFormats[2] = { PixelFormat::E_PF_R8G8B8A8, PixelFormat::E_PF_R8G8B8A8_SRGB };
        bool storesReady = true;
        for (uint32_t i = 0; i < 2; ++i)
        {
            String reason;
            mStoreTargets[i] = buildTarget(String("Store") + std::to_string(i), storeFormats[i], kStoreWidth, kStoreHeight, kGPUNone, reason);
            if (mStoreTargets[i] == nullptr)
            {
                T3D_LOG_WARNING(LOG_TAG_APP, "SRGBApp : %s", reason.c_str());
                storesReady = false;
                continue;
            }
            mStoreMaterials[i] = buildPatternMaterial(String("Store") + std::to_string(i), Vector4(kModeDarkRamp, kDarkPeak, 0.0f, 0.0f), nullptr);
            storesReady = storesReady && (mStoreMaterials[i] != nullptr);
        }

        mCompositeMaterial = buildMaterial(mCompositeShader, "Composite");
        mReady = atlasUnorm != nullptr && atlasRgba != nullptr && atlasBgra != nullptr
            && checkerUnorm != nullptr && checkerSrgb != nullptr && storesReady && mCompositeMaterial != nullptr;
        if (!mReady)
        {
            T3D_LOG_WARNING(LOG_TAG_APP, "SRGBApp : the test chart needs sRGB textures and render targets, the camera image is copied unchanged.");
            return;
        }

        mCompositeMaterial->setTexture("_AtlasUnorm", atlasUnorm->getUUID());
        mCompositeMaterial->setTexture("_AtlasRgbaSrgb", atlasRgba->getUUID());
        mCompositeMaterial->setTexture("_AtlasBgraSrgb", atlasBgra->getUUID());
        mCompositeMaterial->setTexture("_StoreUnorm", mStoreTargets[0]->getUUID());
        mCompositeMaterial->setTexture("_StoreSrgb", mStoreTargets[1]->getUUID());
        mCompositeMaterial->setTexture("_CheckerUnorm", checkerUnorm->getUUID());
        mCompositeMaterial->setTexture("_CheckerSrgb", checkerSrgb->getUUID());
    }

    //--------------------------------------------------------------------------

    void SRGBTestBehaviour::buildChecks()
    {
        if (!mReady)
        {
            return;
        }

        const size_t pixelCount = static_cast<size_t>(kCheckWidth) * kCheckHeight;

        // Distinct values per channel, so a red / blue swap cannot pass
        TArray<uint8_t> ramp(pixelCount * 4);
        for (size_t i = 0; i < pixelCount; ++i)
        {
            const uint32_t c = static_cast<uint32_t>(i % kCheckWidth);
            ramp[i * 4 + 0] = static_cast<uint8_t>(c);
            ramp[i * 4 + 1] = static_cast<uint8_t>(255 - c);
            ramp[i * 4 + 2] = static_cast<uint8_t>((c * 5) & 0xFF);
            ramp[i * 4 + 3] = 255;
        }

        TArray<uint8_t> decoded(pixelCount * 4);
        for (size_t i = 0; i < ramp.size(); ++i)
        {
            decoded[i] = ((i % 4) == 3) ? ramp[i] : toByte(srgbToLinear(static_cast<Real>(ramp[i]) / 255.0f));
        }

        // Pattern mode 0 : column c writes v = c / 255 as (v, 1 - v, v / 4), alpha is never encoded
        TArray<uint8_t> encoded(pixelCount * 4);
        for (size_t i = 0; i < pixelCount; ++i)
        {
            const Real v = static_cast<Real>(i % kCheckWidth) / 255.0f;
            encoded[i * 4 + 0] = toByte(linearToSrgb(v));
            encoded[i * 4 + 1] = toByte(linearToSrgb(1.0f - v));
            encoded[i * 4 + 2] = toByte(linearToSrgb(v * 0.25f));
            encoded[i * 4 + 3] = 255;
        }

        // addCheck hands out references into mChecks, they must survive later additions
        mChecks.reserve(16);
        auto addCheck = [this](const String &name, uint32_t width, uint32_t height, int32_t tolerance) -> Check &
        {
            T3D_ASSERT(mChecks.size() < mChecks.capacity());
            mChecks.emplace_back();
            Check &check = mChecks.back();
            check.name = name;
            check.width = width;
            check.height = height;
            check.tolerance = tolerance;
            return check;
        };

        const PixelFormat srgbFormats[2] = { PixelFormat::E_PF_R8G8B8A8_SRGB, PixelFormat::E_PF_B8G8R8A8_SRGB };
        for (PixelFormat format : srgbFormats)
        {
            const String formatName = PixelFormatUtil::getName(format);
            Texture2DPtr source = buildTexture("CheckSource" + formatName, format, kCheckWidth, kCheckHeight, ramp, false, kCPURead);

            Check &raw = addCheck("sRGB texture stores the uploaded bytes unchanged (" + formatName + ")", kCheckWidth, kCheckHeight, 0);
            raw.target = source;
            raw.format = format;
            raw.expected = ramp;
            if (source == nullptr)
            {
                raw.skipReason = formatName + " is not available as a texture";
            }

            Check &decode = addCheck("Sampling decodes sRGB to linear (" + formatName + " -> R8G8B8A8 UNORM)", kCheckWidth, kCheckHeight, 1);
            decode.expected = decoded;
            decode.format = PixelFormat::E_PF_R8G8B8A8;
            if (source == nullptr)
            {
                decode.skipReason = raw.skipReason;
                continue;
            }
            decode.target = buildTarget("CheckDecode" + formatName, PixelFormat::E_PF_R8G8B8A8, kCheckWidth, kCheckHeight, kGPUNone, decode.skipReason);
            if (decode.target != nullptr)
            {
                decode.material = buildPatternMaterial("CheckDecode" + formatName, Vector4(kModeSample, 0.0f, 0.0f, 0.0f), source);
            }
        }

        for (PixelFormat format : srgbFormats)
        {
            const String formatName = PixelFormatUtil::getName(format);
            Check &encode = addCheck("Render target encodes linear to sRGB (" + formatName + ")", kCheckWidth, kCheckHeight, 1);
            encode.expected = encoded;
            encode.format = format;
            encode.target = buildTarget("CheckEncode" + formatName, format, kCheckWidth, kCheckHeight, kGPUNone, encode.skipReason);
            if (encode.target != nullptr)
            {
                encode.material = buildPatternMaterial("CheckEncode" + formatName, Vector4(kModeValues, 1.0f / 255.0f, 0.0f, 0.0f), nullptr);
            }
        }

        {
            // D3D11 creates this one typeless and puts sRGB views on it, the UAV view stays linear
            Check &uav = addCheck("UAV-capable sRGB render target still encodes (R8G8B8A8_SRGB + kGPUUnorderedAccess)", kCheckWidth, kCheckHeight, 1);
            uav.expected = encoded;
            uav.format = PixelFormat::E_PF_R8G8B8A8_SRGB;
            uav.target = buildTarget("CheckEncodeUAV", PixelFormat::E_PF_R8G8B8A8_SRGB, kCheckWidth, kCheckHeight, kGPUUnorderedAccess, uav.skipReason);
            if (uav.target != nullptr)
            {
                uav.material = buildPatternMaterial("CheckEncodeUAV", Vector4(kModeValues, 1.0f / 255.0f, 0.0f, 0.0f), nullptr);
            }
        }

        {
            // Every pixel samples a texel corner of the black / white checker : decoding before
            // filtering gives linear 0.5 (byte 128), filtering the raw bytes first would give 54
            TArray<uint8_t> checker(kCheckerSize * kCheckerSize * 4);
            for (uint32_t y = 0; y < kCheckerSize; ++y)
            {
                for (uint32_t x = 0; x < kCheckerSize; ++x)
                {
                    uint8_t *p = &checker[(y * kCheckerSize + x) * 4];
                    p[0] = p[1] = p[2] = ((x + y) % 2 != 0) ? 255 : 0;
                    p[3] = 255;
                }
            }
            Texture2DPtr source = buildTexture("CheckFilterSource", PixelFormat::E_PF_R8G8B8A8_SRGB, kCheckerSize, kCheckerSize, checker, true, kCPUNone);

            Check &filter = addCheck("Bilinear filtering averages sRGB texels in linear space", kCheckerSize, kCheckerSize, 2);
            filter.format = PixelFormat::E_PF_R8G8B8A8;
            filter.expected.assign(static_cast<size_t>(kCheckerSize) * kCheckerSize * 4, 128);
            for (size_t i = 3; i < filter.expected.size(); i += 4)
            {
                filter.expected[i] = 255;
            }
            if (source == nullptr)
            {
                filter.skipReason = "R8G8B8A8_SRGB cannot be linearly filtered";
            }
            else
            {
                filter.target = buildTarget("CheckFilter", PixelFormat::E_PF_R8G8B8A8, kCheckerSize, kCheckerSize, kGPUNone, filter.skipReason);
                if (filter.target != nullptr)
                {
                    const Real half = 0.5f / static_cast<Real>(kCheckerSize);
                    filter.material = buildPatternMaterial("CheckFilter", Vector4(kModeSample, 0.0f, half, half), source);
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    void SRGBTestBehaviour::onRenderImage(RHIContext *ctx,
                                          RenderTexture *src,
                                          RenderTexture *dst,
                                          RenderTexture * /*depth*/)
    {
        if (!mReady)
        {
            blitCopy(ctx, src, dst);
            return;
        }

        ForwardRenderPipeline &pipeline = static_cast<ForwardRenderPipeline &>(T3D_RENDER_PIPELINE);

        for (uint32_t i = 0; i < 2; ++i)
        {
            pipeline.drawFullscreen(ctx, mStoreMaterials[i], src, mStoreTargets[i]);
        }

        for (Check &check : mChecks)
        {
            if (check.material != nullptr)
            {
                pipeline.drawFullscreen(ctx, check.material, src, static_cast<RenderTexture *>(check.target.get()));
            }
        }

        pipeline.drawFullscreen(ctx, mCompositeMaterial, src, dst);
        ++mFramesRendered;
    }

    //--------------------------------------------------------------------------

    void SRGBTestBehaviour::requestReadback()
    {
        if (!mReady || mReadbackPending || mVerified)
        {
            return;
        }

        RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
        if (ctx == nullptr || !ctx->getCapabilities().supportsReadback)
        {
            T3D_LOG_WARNING(LOG_TAG_APP, "SRGBApp : readback is not supported by this renderer, only the chart can be checked.");
            mVerified = true;
            return;
        }

        for (Check &check : mChecks)
        {
            if (check.target != nullptr && check.skipReason.empty())
            {
                check.handle = check.target->map(ReadbackRegion());
            }
        }
        mReadbackPending = true;
    }

    //--------------------------------------------------------------------------

    void SRGBTestBehaviour::verifyReadback()
    {
        if (!mReadbackPending)
        {
            return;
        }
        mReadbackPending = false;
        mVerified = true;

        uint32_t passed = 0, failed = 0, skipped = 0;
        for (Check &check : mChecks)
        {
            if (!check.skipReason.empty() || check.target == nullptr)
            {
                ++skipped;
                T3D_LOG_INFO(LOG_TAG_APP, "SRGBApp : SKIP %s : %s", check.name.c_str(), check.skipReason.c_str());
                continue;
            }

            if (!check.handle.isValid())
            {
                ++failed;
                T3D_LOG_ERROR(LOG_TAG_APP, "SRGBApp : FAIL %s : map returned an invalid handle", check.name.c_str());
                continue;
            }

            Buffer data;
            const TResult ret = check.target->unmap(check.handle, data);
            const size_t expectedSize = check.expected.size();
            if (T3D_FAILED(ret) || data.DataSize != expectedSize)
            {
                ++failed;
                T3D_LOG_ERROR(LOG_TAG_APP, "SRGBApp : FAIL %s : unmap ret=%d size=%zu (expect %zu)",
                    check.name.c_str(), ret, data.DataSize, expectedSize);
                data.release();
                continue;
            }

            const bool swap = isBGRA(check.format);
            int32_t maxDiff = 0;
            size_t firstBad = expectedSize;
            for (size_t i = 0; i < expectedSize; i += 4)
            {
                const uint8_t got[4] =
                {
                    swap ? data.Data[i + 2] : data.Data[i + 0],
                    data.Data[i + 1],
                    swap ? data.Data[i + 0] : data.Data[i + 2],
                    data.Data[i + 3],
                };
                for (size_t c = 0; c < 4; ++c)
                {
                    const int32_t diff = std::abs(static_cast<int32_t>(got[c]) - static_cast<int32_t>(check.expected[i + c]));
                    maxDiff = std::max(maxDiff, diff);
                    if (diff > check.tolerance && firstBad == expectedSize)
                    {
                        firstBad = i;
                    }
                }
            }

            if (firstBad == expectedSize)
            {
                ++passed;
                T3D_LOG_INFO(LOG_TAG_APP, "SRGBApp : PASS %s (max diff %d, tolerance %d)", check.name.c_str(), maxDiff, check.tolerance);
            }
            else
            {
                ++failed;
                const size_t pixel = firstBad / 4;
                const uint8_t *e = &check.expected[firstBad];
                const uint8_t *g = data.Data + firstBad;
                T3D_LOG_ERROR(LOG_TAG_APP, "SRGBApp : FAIL %s : pixel (%zu, %zu) stored bytes (%u, %u, %u, %u), expected RGBA (%u, %u, %u, %u), max diff %d",
                    check.name.c_str(), pixel % check.width, pixel / check.width, g[0], g[1], g[2], g[3], e[0], e[1], e[2], e[3], maxDiff);
            }
            data.release();
        }

        T3D_LOG_INFO(LOG_TAG_APP, "SRGBApp : readback checks done, %u passed, %u failed, %u skipped.", passed, failed, skipped);
    }
}
