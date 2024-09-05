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

    ShaderSamplerParamPtr ShaderSamplerParam::create(const String &name, const String &texName, TEXTURE_TYPE texType)
    {
        return new ShaderSamplerParam(name, texName, texType);
    }
    
    //--------------------------------------------------------------------------

    ShaderSamplerParamPtr ShaderSamplerParam::create(const String &name, TEXTURE_TYPE texType, TexturePtr texture)
    {
        return new ShaderSamplerParam(name, texType, texture);
    }

    //--------------------------------------------------------------------------

    ShaderSamplerParam::ShaderSamplerParam(const String &name, const String &texName, TEXTURE_TYPE texType)
        : mName(name)
        , mTexName(texName)
        , mTexType(texType)
    {
        // ArchivePtr archive;
        // mTexture = T3D_TEXTURE_MGR.loadTexture(archive, mName);
    }

    //--------------------------------------------------------------------------

    ShaderSamplerParam::ShaderSamplerParam(const String &name, TEXTURE_TYPE texType, TexturePtr texture)
        : mName(name)
        , mTexType(texType)
        , mTexture(texture)
    {
        
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
        sampler->mTexName = mTexName;
        sampler->mTexType = mTexType;
        sampler->mTexture = mTexture;
        return sampler;
    }

    //--------------------------------------------------------------------------

    void ShaderSamplerParam::onPostLoad()
    {
        // ArchivePtr archive;
        // mTexture = T3D_TEXTURE_MGR.loadTexture(archive, mName);
    }

    //--------------------------------------------------------------------------
}


