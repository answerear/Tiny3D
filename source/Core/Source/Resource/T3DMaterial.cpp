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

#include "T3DErrorDef.h"
#include "Material/T3DPass.h"
#include "Resource/T3DShader.h"
#include "Resource/T3DShaderManager.h"
#include "Material/T3DTechniqueInstance.h"
#include "Resource/T3DTextureManager.h"
#include "Material/T3DPassInstance.h"


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
        , mShaderUUID(shader->getUUID())
        , mShader(shader)
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

        // constant values
        for (const auto &item : material->getConstantValues())
        {
            ShaderConstantValuePtr constValue = item.second->clone();
            mConstantValues.emplace(item.first, constValue);
        }

        // texture sampler values
        for (const auto &item : material->getSamplerValues())
        {
            ShaderSamplerValuePtr samplerValue = item.second->clone();
            mSamplerValues.emplace(item.first, samplerValue);
        }

        // shader uuid
        mShaderUUID = material->getShaderUUID();

        // shader
        mShader = material->getShader();

        if (material->getCurrentTechnique() != nullptr)
        {
            mCurTechnique = material->getCurrentTechnique()->clone(this);
        }
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

    void Material::setTexture(const String &name, const UUID &uuid)
    {
        const auto itr = mSamplerValues.find(name);
        if (itr != mSamplerValues.end())
        {
            itr->second->setTextureUUID(uuid);
        }

        if (mCurTechnique != nullptr)
        {
            mCurTechnique->setTexture(name, uuid);
        }
    }

    //--------------------------------------------------------------------------

    const UUID &Material::getTexture(const String &name) const
    {
        const auto itr = mSamplerValues.find(name);
        if (itr != mSamplerValues.end())
        {
            return itr->second->getTextureUUID();
        }
        return UUID::INVALID;
    }

    //--------------------------------------------------------------------------

    bool Material::hasTexture(const String &name) const
    {
        const auto itr = mSamplerValues.find(name);
        return (itr != mSamplerValues.end());
    }

    //--------------------------------------------------------------------------

    void Material::setShaderConstantValue(ShaderConstantValue *constValue)
    {
        T3D_ASSERT(mCurTechnique != nullptr);
        
        switch (constValue->getDataType())
        {
        case ShaderConstantParam::DATA_TYPE::DT_FLOAT:
            {
                /// 浮点数
                mCurTechnique->setFloat(constValue->getName(), constValue->getFloat());
            }
            break;
        case ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY:
            {
                /// 浮点数组
                mCurTechnique->setFloatArray(constValue->getName(), constValue->getFloatArray());
            }
            break;
        case ShaderConstantParam::DATA_TYPE::DT_BOOL:
            {
                /// 布尔值
                mCurTechnique->setBool(constValue->getName(), constValue->getBool());
            }
            break;
        case ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY:
            {
                /// 布尔数组
                mCurTechnique->setBoolArray(constValue->getName(), constValue->getBoolArray());
            }
            break;
        case ShaderConstantParam::DATA_TYPE::DT_INTEGER:
            {
                /// 整型
                mCurTechnique->setInteger(constValue->getName(), constValue->getInteger());
            }
            break;
        case ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY:
            {
                /// 整型数组
                mCurTechnique->setIntArray(constValue->getName(), constValue->getIntArray());
            }
            break;
        case ShaderConstantParam::DATA_TYPE::DT_COLOR:
            {
                /// 颜色
                mCurTechnique->setColor(constValue->getName(), constValue->getColor());
            }
            break;
        case ShaderConstantParam::DATA_TYPE::DT_COLOR_ARRAY:
            {
                /// 颜色数组
                mCurTechnique->setColorArray(constValue->getName(), constValue->getColorArray());
            }
            break;
        case ShaderConstantParam::DATA_TYPE::DT_VECTOR4:
            {
                /// 4D 向量
                mCurTechnique->setVector(constValue->getName(), constValue->getVector());
            }
            break;
        case ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY:
            {
                /// 4D 向量数组
                mCurTechnique->setVectorArray(constValue->getName(), constValue->getVectorArray());
            }
            break;
        case ShaderConstantParam::DATA_TYPE::DT_MATRIX4:
            {
                /// 4D 方阵
                mCurTechnique->setMatrix(constValue->getName(), constValue->getMatrix());
            }
            break;
        case ShaderConstantParam::DATA_TYPE::DT_MATRIX4_ARRAY:
            {
                /// 4D 方阵数组
                mCurTechnique->setMatrixArray(constValue->getName(), constValue->getMatrixArray());
            }
            break;
        case ShaderConstantParam::DATA_TYPE::DT_STRUCT:
            {
                /// 结构体
                mCurTechnique->setData(constValue->getName(), constValue->getData(), constValue->getDataSize());
            }
            break;
        }
    }

    //--------------------------------------------------------------------------

    void Material::setShaderSamplerValue(Archive *archive, ShaderSamplerValue *samplerValue)
    {
        if (archive != nullptr)
        {
            TexturePtr texture = T3D_TEXTURE_MGR.loadTexture(archive, samplerValue->getTextureUUID());
            T3D_ASSERT(texture);
        }

        T3D_ASSERT(mCurTechnique != nullptr);
        mCurTechnique->setTexture(samplerValue->getName(), samplerValue->getTextureUUID());
    }

    //--------------------------------------------------------------------------

    TResult Material::init(bool shouldReflect, Archive *archive)
    {
        TResult ret;

        do
        {
            ret = mShader->compile();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Failed to compile shader (%s) ! ERROR [%d]", mShader->getName().c_str(), ret);
                break;
            }

            if (shouldReflect)
            {
                ret = mShader->reflect();
                if (T3D_FAILED(ret))
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Failed to reflect shader (%s) ! ERROR [%d]", mShader->getName().c_str(), ret);
                    break;
                }

                auto initShaderParams = [this](const ShaderVariants &shaderVariants)
                {
                    for (const auto &shaderVariant : shaderVariants)
                    {
                        // 添加常量，并设置常量初始值
                        for (const auto &param : shaderVariant.second->getShaderConstantParams())
                        {
                            ShaderConstantValuePtr constValue = ShaderConstantValue::create(param.first, param.second->getDataType());
                            uint8_t *data = new uint8_t[param.second->getDataSize()];
                            memset(data, 0, param.second->getDataSize());
                            constValue->setData(data, param.second->getDataSize());
                            T3D_SAFE_DELETE_ARRAY(data);

                            mConstantValues.emplace(constValue->getName(), constValue);
                        }

                        // 添加纹理采样器
                        for (const auto &param : shaderVariant.second->getShaderSamplerParams())
                        {
                            ShaderSamplerValuePtr samplerValue = ShaderSamplerValue::create(param.first);
                            mSamplerValues.emplace(samplerValue->getName(), samplerValue);
                        }
                    }
                };
                
                for (const auto &tech : mShader->getTechniques())
                {
                    for (const auto &pass : tech->getPasses())
                    {
                        initShaderParams(pass->getVertexShaders());
                        initShaderParams(pass->getPixelShaders());
                        initShaderParams(pass->getDomainShaders());
                        initShaderParams(pass->getHullShaders());
                        initShaderParams(pass->getGeometryShaders());
                    }
                }
            }

            mCurTechnique = TechniqueInstance::create(this, mShader->getSupportTechnique());

            // 设置 shader constant buffer 值
            for (const auto &item : mConstantValues)
            {
                setShaderConstantValue(item.second);
            }

            // 档案对象非空，则需要加载纹理资源
            for (const auto &item : mSamplerValues)
            {
                setShaderSamplerValue(archive, item.second);
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

            ret = init(true, nullptr);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Material::onLoad(Archive *archive)
    {
        TResult ret = T3D_OK;

        do
        {
            ret = Resource::onLoad(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }

            mShader = T3D_SHADER_MGR.loadShader(archive, mShaderUUID);
            if (mShader == nullptr)
            {
                // 加载 shader 失败
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Load shader (%s) failed !", mShaderUUID.toString().c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }

            ret = init(false, archive);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    

    //--------------------------------------------------------------------------
}
