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


#include "GrayscaleEffectBehaviour.h"
#include "LogCameraBehaviour.h"
#include "PostProcessShaderSources.h"
#include "Kernel/T3DAgent.h"
#include "Material/T3DPass.h"
#include "Material/T3DShaderKeyword.h"
#include "Material/T3DShaderVariant.h"
#include "Material/T3DTechnique.h"
#include "Render/T3DForwardRenderPipeline.h"
#include "Render/T3DRenderConstant.h"
#include "Render/T3DRenderState.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DShader.h"
#include "Resource/T3DShaderManager.h"
#include "RHI/T3DRHIRenderer.h"
#include "T3DErrorDef.h"

#define LOG_TAG_APP     "APP"


namespace Tiny3D
{

    //--------------------------------------------------------------------------

    GrayscaleEffectBehaviour::GrayscaleEffectBehaviour(const UUID &uuid)
        : CameraEffectBehaviour(uuid)
    {
    }

    //--------------------------------------------------------------------------

    void GrayscaleEffectBehaviour::onAwake()
    {
        mMaterial = buildMaterial();
    }

    //--------------------------------------------------------------------------

    MaterialPtr GrayscaleEffectBehaviour::buildMaterial()
    {
        RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
        if (renderer == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "GrayscaleEffectBehaviour: no active RHI renderer");
            return nullptr;
        }

        const SHADER_LANGUAGE shadingLang = renderer->getShadingLanguage();
        const char *vsCode = nullptr;
        size_t vsSize = 0;
        const char *psCode = nullptr;
        size_t psSize = 0;
        if (!getGrayscaleEffectShader(vsCode, vsSize, psCode, psSize))
        {
            T3D_LOG_WARNING(LOG_TAG_RENDER,
                "GrayscaleEffectBehaviour: no shader variant for current renderer, fallback to blit copy");
            return nullptr;
        }

        ShaderKeyword vkeyword;
        vkeyword.addKeyword("");
        vkeyword.generate();
        ShaderKeyword pkeyword(vkeyword);

        ShaderVariantPtr vshader = ShaderVariant::create(std::move(vkeyword), vsCode, vsSize);
        vshader->setShaderStage(SHADER_STAGE::kVertex);
        vshader->setLanguage(shadingLang);

        ShaderVariantPtr pshader = ShaderVariant::create(std::move(pkeyword), psCode, psSize);
        pshader->setShaderStage(SHADER_STAGE::kPixel);
        pshader->setLanguage(shadingLang);

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
        TResult ret = pass->addShaderVariant(vshader->getShaderKeyword(), vshader);
        if (T3D_FAILED(ret))
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "GrayscaleEffectBehaviour: failed to add VS variant");
            return nullptr;
        }

        pass->addTag(ShaderLab::kBuiltinTagLightMode, ShaderLab::kBuiltinLightModeForwardBaseStr);
        ret = pass->addShaderVariant(pshader->getShaderKeyword(), pshader);
        if (T3D_FAILED(ret))
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "GrayscaleEffectBehaviour: failed to add PS variant");
            return nullptr;
        }
        pass->setRenderState(renderState);

        TechniquePtr tech = Technique::create("Grayscale-Technique");
        if (!tech->addPass(pass))
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "GrayscaleEffectBehaviour: failed to add pass");
            return nullptr;
        }
        tech->addTag(ShaderLab::kBuiltinTagQueue, ShaderLab::kBuiltinQueueOverlayStr);

        const String uniqueSuffix = getUUID().toString();
        ShaderPtr shader = T3D_SHADER_MGR.createShader(String("Hidden-Grayscale-") + uniqueSuffix);
        if (shader == nullptr || !shader->addTechnique(tech))
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "GrayscaleEffectBehaviour: failed to create shader");
            return nullptr;
        }

        MaterialPtr material = T3D_MATERIAL_MGR.createMaterial(String("Hidden-Grayscale-") + uniqueSuffix, shader.get());
        if (material == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "GrayscaleEffectBehaviour: failed to create material");
            return nullptr;
        }

        StringArray enableKeywords;
        enableKeywords.push_back("");
        StringArray disableKeywords;
        material->switchKeywords(enableKeywords, disableKeywords);
        return material;
    }

    //--------------------------------------------------------------------------

    void GrayscaleEffectBehaviour::logRenderImage()
    {
        GameObject *go = getGameObject();
        if (go == nullptr)
        {
            return;
        }

        auto logCam = go->getComponent<LogCameraBehaviour>();
        if (logCam != nullptr && logCam->shouldLog())
        {
            T3D_LOG_INFO(LOG_TAG_APP, "[Effect] GrayscaleEffectBehaviour::onRenderImage this=%p", this);
        }
    }

    //--------------------------------------------------------------------------

    void GrayscaleEffectBehaviour::onRenderImage(RHIContext *ctx,
                                                RenderTexture *src,
                                                RenderTexture *dst,
                                                RenderTexture * /*depth*/)
    {
        logRenderImage();

        if (mMaterial == nullptr)
        {
            T3D_LOG_WARNING(LOG_TAG_RENDER,
                "GrayscaleEffectBehaviour: no material, blit copy this=%p", this);
            blitCopy(ctx, src, dst);
            return;
        }

        static_cast<ForwardRenderPipeline &>(T3D_RENDER_PIPELINE).drawFullscreen(
            ctx, mMaterial.get(), src, dst);
    }

    //--------------------------------------------------------------------------
}
