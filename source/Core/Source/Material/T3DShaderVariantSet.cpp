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


#include "Material/T3DShaderVariantSet.h"
#include "Kernel/T3DAgent.h"
#include "RHI/T3DRHIRenderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderVariantSetPtr ShaderVariantSet::create()
    {
        return T3D_NEW ShaderVariantSet();
    }

    //--------------------------------------------------------------------------

    ShaderVariantSet::~ShaderVariantSet()
    {
    }

    //--------------------------------------------------------------------------

    ShaderVariantPtr ShaderVariantSet::getVariant(SHADER_LANGUAGE lang) const
    {
        const auto itr = mVariants.find(lang);
        if (itr != mVariants.end())
        {
            return itr->second;
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------

    ShaderVariantPtr ShaderVariantSet::getActiveVariant() const
    {
        RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
        if (renderer == nullptr)
        {
            return nullptr;
        }
        return getVariant(renderer->getShadingLanguage());
    }

    //--------------------------------------------------------------------------

    void ShaderVariantSet::putVariant(SHADER_LANGUAGE lang, ShaderVariantPtr variant)
    {
        mVariants[lang] = variant;
    }

    //--------------------------------------------------------------------------

    void ShaderVariantSet::removeVariant(SHADER_LANGUAGE lang)
    {
        mVariants.erase(lang);
    }

    //--------------------------------------------------------------------------
}
