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


#include "Material/T3DShaderSamplerParam.h"
#include "Kernel/T3DAgent.h"
#include "Resource/T3DTextureManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderSamplerParamPtr ShaderSamplerParam::create(const String &name)
    {
        return new ShaderSamplerParam(name);
    }
    
    //--------------------------------------------------------------------------

    ShaderSamplerParam::ShaderSamplerParam(const String &name)
        : mName(name)
    {
        // ArchivePtr archive;
        // mTexture = T3D_TEXTURE_MGR.loadTexture(archive, mName);
    }

    //--------------------------------------------------------------------------

    ShaderSamplerParam::~ShaderSamplerParam()
    {
        
    }

    //--------------------------------------------------------------------------

    ShaderSamplerParamPtr ShaderSamplerParam::clone() const
    {
        ShaderSamplerParamPtr sampler = new ShaderSamplerParam();
        sampler->mName = mName;
        sampler->mTexType = mTexType;
        sampler->mTexBinding = mTexBinding;
        sampler->mSamplerBinding = mSamplerBinding;
        return sampler;
    }

    //--------------------------------------------------------------------------

    void ShaderSamplerParam::onPostLoad()
    {
        // ArchivePtr archive;
        // mTexture = T3D_TEXTURE_MGR.loadTexture(archive, mName);
    }

    //--------------------------------------------------------------------------

    ShaderSamplerValuePtr ShaderSamplerValue::create(const String &name)
    {
        return new ShaderSamplerValue(name);
    }

    //--------------------------------------------------------------------------

    ShaderSamplerValue::ShaderSamplerValue(const String &name)
        : mName(name)
    {
        
    }

    //--------------------------------------------------------------------------

    ShaderSamplerValue::~ShaderSamplerValue()
    {
        
    }

    //--------------------------------------------------------------------------

    ShaderSamplerValuePtr ShaderSamplerValue::clone() const
    {
        ShaderSamplerValuePtr sampler = create(mName);

        if (sampler != nullptr)
        {
            sampler->mTextureUUID = mTextureUUID;
            sampler->mTexture = mTexture;
        }
        
        return sampler;
    }
    //--------------------------------------------------------------------------
}


