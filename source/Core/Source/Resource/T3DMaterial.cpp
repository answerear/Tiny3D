/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "Resource/T3DMaterial.h"
#include "Resource/T3DShader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    MaterialPtr Material::create(const String &name)
    {
        return new Material(name);
    }

    //--------------------------------------------------------------------------

    Material::Material(const String &name)
        : Resource(name)
    {
        
    }

    //--------------------------------------------------------------------------

    Material::~Material()
    {
        
    }

    //--------------------------------------------------------------------------

    Resource::Type Material::getType() const
    {
        return Type::kMaterial;
    }

    //--------------------------------------------------------------------------
    
    ResourcePtr Material::clone() const
    {
        MaterialPtr material = create(getName());
        material->cloneProperties(this);
        return material;
    }
    
    //--------------------------------------------------------------------------

    void Material::cloneProperties(const Resource *const src)
    {
        Resource::cloneProperties(src);
        const Material *material = static_cast<const Material*>(src);
    }

    //--------------------------------------------------------------------------

    TResult Material::enableKeyword(const String &keyword)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Material::disableKeyword(const String &keyword)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool Material::isKeywordEnable(const String &keyword) const
    {
        return false;
    }

    //--------------------------------------------------------------------------

    void Material::setTexture(const String &name, const TextureState &value)
    {
        auto itr = mSamplers.find(name);
        if (itr != mSamplers.end())
        {
            itr->second->setTexture(value);
        }
    }

    //--------------------------------------------------------------------------

    TextureState Material::getTexture(const String &name) const
    {
        TextureState tex;
        auto itr = mSamplers.find(name);
        if (itr != mSamplers.end())
        {
            tex = itr->second->getTexture();
        }
        return tex;
    }

    //--------------------------------------------------------------------------

    bool Material::hasTexture(const String &name) const
    {
        const auto itr = mSamplers.find(name);
        return (itr != mSamplers.end());
    }

    //--------------------------------------------------------------------------

    void Material::setShader(ShaderPtr shader)
    {
        mShader = shader;

        // 清除 constant 数据
        mConstants.clear();

        // 复制一份 constant 数据
        for (auto value : mShader->getConstantParams())
        {
            ShaderConstantParamPtr param = value.second->clone();
            mConstants.emplace(value.first, param);
        }

        // 清除 sampler 数据
        mSamplers.clear();
        
        // 复制一份 sampler 数据
        for (auto value : mShader->getSamplerParams())
        {
            ShaderSamplerParamPtr param = value.second->clone();
            mSamplers.emplace(value.first, param);
        }
    }

    //--------------------------------------------------------------------------
}
