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
#include "Resource/T3DShaderManager.h"
#include "Material/T3DTechniqueInstance.h"


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

    TResult Material::switchKeywords(const StringArray &enableKeys, const StringArray &disableKeys)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mCurTechnique == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Switch keyword failed ! Because current technique is null !");
                break;
            }

            ret = mCurTechnique->switchKeywords(enableKeys, disableKeys);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    bool Material::isKeywordEnable(const String &keyword) const
    {
        return false;
    }

    //--------------------------------------------------------------------------

    void Material::setTexture(const String &name, Texture *value)
    {
        auto itr = mSamplers.find(name);
        if (itr != mSamplers.end())
        {
            // TODO
            // itr->second->setTextureType(value);
        }
    }

    //--------------------------------------------------------------------------

    Texture *Material::getTexture(const String &name) const
    {
        // TextureState *texState;
        Texture *texture = nullptr;
        auto itr = mSamplers.find(name);
        if (itr != mSamplers.end())
        {
            // TODO
            // texState = itr->second->getTextureState();
        }
        return texture;
    }

    //--------------------------------------------------------------------------

    bool Material::hasTexture(const String &name) const
    {
        const auto itr = mSamplers.find(name);
        return (itr != mSamplers.end());
    }

    //--------------------------------------------------------------------------

    bool Material::onPostLoad()
    {
        TResult ret = T3D_OK;

        do
        {
            ArchivePtr archive;
            mShader = T3D_SHADER_MGR.loadShader(archive, mShaderName);
            if (mShader == nullptr)
            {
                // 加载 shader 失败
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Load shader failed !");
                break;
            }
            
            if (T3D_FAILED(mShader->compile()))
            {
                // 编译 shader 失败
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Compile shader failed !");
                break;
            }

            mCurTechnique = TechniqueInstance::create(mShader->getSupportTechnique());

            // 清除 constant 、sampler数据
            mConstants.clear();
            mSamplers.clear();

            // 复制一份 constant 数据
            for (auto value : mShader->getConstantParams())
            {
                ShaderConstantParamPtr param = value.second->clone();
                mConstants.emplace(value.first, param);
            }

            // 复制一份 sampler 数据
            for (auto value : mShader->getSamplerParams())
            {
                ShaderSamplerParamPtr param = value.second->clone();
                mSamplers.emplace(value.first, param);
            }
        } while (false);
        
        return true;
    }

    //--------------------------------------------------------------------------
}
