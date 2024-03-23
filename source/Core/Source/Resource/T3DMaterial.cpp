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

    MaterialPtr Material::create(const String &name, Shader *shader)
    {
        return new Material(name, shader);
    }
    
    //--------------------------------------------------------------------------

    Material::Material(const String &name)
        : Resource(name)
    {
        
    }

    //--------------------------------------------------------------------------

    Material::Material(const String &name, Shader *shader)
        : Resource(name)
        , mShader(shader)
    {
        mShaderName = shader->getName();
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

    TResult Material::init()
    {
        TResult ret;

        do
        {
            ret = mShader->compile();
            if (T3D_FAILED(ret))
            {
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

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Material::onCreate()
    {
        TResult ret;

        do
        {
            ret = Resource::onCreate();
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = init();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Material::onLoad()
    {
        TResult ret = T3D_OK;

        do
        {
            ret = Resource::onLoad();
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            ArchivePtr archive;
            mShader = T3D_SHADER_MGR.loadShader(archive, mShaderName);
            if (mShader == nullptr)
            {
                // 加载 shader 失败
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Load shader failed !");
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            ret = init();
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
}
