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

#include <algorithm>
#include "T3DErrorDef.h"
#include "Kernel/T3DAgent.h"
#include "Material/T3DPass.h"
#include "Material/T3DShaderVariantSet.h"
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
        return T3D_NEW Material(name);
    }

    //--------------------------------------------------------------------------

    MaterialPtr Material::create(const String &name, Shader *shader)
    {
        return T3D_NEW Material(name, shader);
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

        mTechInstances.clear();
        mCurTechnique = nullptr;

        for (const auto &techInst : material->mTechInstances)
        {
            if (techInst != nullptr)
            {
                TechniqueInstancePtr cloned = techInst->clone(this);
                mTechInstances.push_back(cloned);
                if (techInst == material->mCurTechnique)
                {
                    mCurTechnique = cloned;
                }
            }
        }

        if (mCurTechnique == nullptr && material->mCurTechnique != nullptr)
        {
            mCurTechnique = material->mCurTechnique->clone(this);
        }

        mLastEffectiveMaxLOD = material->mLastEffectiveMaxLOD;
    }

    //--------------------------------------------------------------------------

    uint32_t Material::computeEffectiveMaxLOD() const
    {
        uint32_t maxLOD = mShader != nullptr ? mShader->getMaxLOD() : UINT32_MAX;
        if (Agent::getInstancePtr() != nullptr)
        {
            const uint32_t globalMaxLOD = T3D_AGENT.getSettings().renderSettings.maxShaderLOD;
            maxLOD = std::min(maxLOD, globalMaxLOD);
        }
        return maxLOD;
    }

    //--------------------------------------------------------------------------

    TechniqueInstancePtr Material::getCurrentTechnique() const
    {
        const uint32_t effectiveMaxLOD = computeEffectiveMaxLOD();
        if (effectiveMaxLOD != mLastEffectiveMaxLOD)
        {
            mLastEffectiveMaxLOD = effectiveMaxLOD;
            const_cast<Material *>(this)->selectTechniqueByLOD(effectiveMaxLOD);
        }
        return mCurTechnique;
    }

    //--------------------------------------------------------------------------

    TechniqueInstancePtr Material::selectTechniqueByLOD(uint32_t maxLOD)
    {
        if (mTechInstances.empty())
        {
            return mCurTechnique;
        }

        const uint32_t effectiveMaxLOD = std::min(maxLOD, computeEffectiveMaxLOD());

        TechniqueInstancePtr bestPositive = nullptr;
        uint32_t bestPositiveLOD = 0;

        TechniqueInstancePtr firstUnrestricted = nullptr;

        TechniqueInstancePtr minLODInst = nullptr;
        uint32_t minLOD = UINT32_MAX;

        for (const auto &inst : mTechInstances)
        {
            if (inst == nullptr || inst->getTechnique() == nullptr)
            {
                continue;
            }

            const uint32_t lod = inst->getTechnique()->getLOD();
            if (lod == 0)
            {
                if (firstUnrestricted == nullptr)
                {
                    firstUnrestricted = inst;
                }
            }
            else
            {
                if (lod <= effectiveMaxLOD && lod > bestPositiveLOD)
                {
                    bestPositiveLOD = lod;
                    bestPositive = inst;
                }

                if (lod < minLOD)
                {
                    minLOD = lod;
                    minLODInst = inst;
                }
            }
        }

        if (bestPositive != nullptr)
        {
            mCurTechnique = bestPositive;
        }
        else if (firstUnrestricted != nullptr)
        {
            mCurTechnique = firstUnrestricted;
        }
        else if (minLODInst != nullptr)
        {
            mCurTechnique = minLODInst;
        }
        else
        {
            mCurTechnique = mTechInstances.front();
        }

        return mCurTechnique;
    }

    //--------------------------------------------------------------------------

    TResult Material::switchKeywords(const StringArray &enableKeys, const StringArray &disableKeys)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mCurTechnique == nullptr && mTechInstances.empty())
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Switch keyword failed ! Because current technique is null !");
                break;
            }

            for (const auto &techInst : mTechInstances)
            {
                if (techInst != nullptr)
                {
                    TResult r = techInst->switchKeywords(enableKeys, disableKeys);
                    if (T3D_FAILED(r))
                    {
                        ret = r;
                    }
                }
            }

            if (mTechInstances.empty() && mCurTechnique != nullptr)
            {
                ret = mCurTechnique->switchKeywords(enableKeys, disableKeys);
            }
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

        for (const auto &techInst : mTechInstances)
        {
            if (techInst != nullptr)
            {
                techInst->setTexture(name, uuid);
            }
        }

        if (mTechInstances.empty() && mCurTechnique != nullptr)
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
        auto applyToTechnique = [constValue](TechniqueInstance *techInst)
        {
            if (techInst == nullptr)
            {
                return;
            }

            switch (constValue->getDataType())
            {
            case ShaderConstantParam::DATA_TYPE::DT_FLOAT:
                techInst->setFloat(constValue->getName(), constValue->getFloat());
                break;
            case ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY:
                techInst->setFloatArray(constValue->getName(), constValue->getFloatArray());
                break;
            case ShaderConstantParam::DATA_TYPE::DT_BOOL:
                techInst->setBool(constValue->getName(), constValue->getBool());
                break;
            case ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY:
                techInst->setBoolArray(constValue->getName(), constValue->getBoolArray());
                break;
            case ShaderConstantParam::DATA_TYPE::DT_INTEGER:
                techInst->setInteger(constValue->getName(), constValue->getInteger());
                break;
            case ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY:
                techInst->setIntArray(constValue->getName(), constValue->getIntArray());
                break;
            case ShaderConstantParam::DATA_TYPE::DT_COLOR:
                techInst->setColor(constValue->getName(), constValue->getColor());
                break;
            case ShaderConstantParam::DATA_TYPE::DT_COLOR_ARRAY:
                techInst->setColorArray(constValue->getName(), constValue->getColorArray());
                break;
            case ShaderConstantParam::DATA_TYPE::DT_VECTOR4:
                techInst->setVector(constValue->getName(), constValue->getVector());
                break;
            case ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY:
                techInst->setVectorArray(constValue->getName(), constValue->getVectorArray());
                break;
            case ShaderConstantParam::DATA_TYPE::DT_MATRIX4:
                techInst->setMatrix(constValue->getName(), constValue->getMatrix());
                break;
            case ShaderConstantParam::DATA_TYPE::DT_MATRIX4_ARRAY:
                techInst->setMatrixArray(constValue->getName(), constValue->getMatrixArray());
                break;
            case ShaderConstantParam::DATA_TYPE::DT_STRUCT:
                techInst->setData(constValue->getName(), constValue->getData(), constValue->getDataSize());
                break;
            }
        };

        for (const auto &techInst : mTechInstances)
        {
            applyToTechnique(techInst.get());
        }

        if (mTechInstances.empty() && mCurTechnique != nullptr)
        {
            applyToTechnique(mCurTechnique.get());
        }
    }

    //--------------------------------------------------------------------------

    void Material::setShaderSamplerValue(Archive *archive, ShaderSamplerValue *samplerValue)
    {
        const UUID &texUUID = samplerValue->getTextureUUID();

        // 运行时纹理（如 shadowMap）在离线阶段没有对应资源，UUID 为无效值，跳过加载
        if (texUUID == UUID::INVALID)
        {
            return;
        }

        if (archive != nullptr)
        {
            TexturePtr texture = T3D_TEXTURE_MGR.loadTexture(archive, texUUID);
            T3D_ASSERT(texture);
        }

        for (const auto &techInst : mTechInstances)
        {
            if (techInst != nullptr)
            {
                techInst->setTexture(samplerValue->getName(), texUUID);
            }
        }

        if (mTechInstances.empty() && mCurTechnique != nullptr)
        {
            mCurTechnique->setTexture(samplerValue->getName(), texUUID);
        }
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

                auto initShaderParams = [this](const ShaderVariantSets &shaderVariants)
                {
                    for (const auto &kv : shaderVariants)
                    {
                        ShaderVariantSetPtr set = kv.second;
                        if (set == nullptr || set->empty())
                        {
                            continue;
                        }

                        // 材质参数（常量/采样器名称）与语言无关，取当前后端变体；
                        // 若当前后端变体缺失则退而取集合中任意一个变体即可。
                        ShaderVariantPtr variant = set->getActiveVariant();
                        if (variant == nullptr)
                        {
                            variant = set->getVariants().begin()->second;
                        }
                        if (variant == nullptr)
                        {
                            continue;
                        }

                        // 添加常量，并设置常量初始值
                        for (const auto &param : variant->getShaderConstantParams())
                        {
                            ShaderConstantValuePtr constValue = ShaderConstantValue::create(param.first, param.second->getDataType(), param.second->getDataSize());
                            uint8_t *data = T3D_POD_NEW_ARRAY(uint8_t, param.second->getDataSize());
                            memset(data, 0, param.second->getDataSize());
                            constValue->setData(data, param.second->getDataSize());
                            T3D_POD_SAFE_DELETE_ARRAY(data);

                            mConstantValues.emplace(constValue->getName(), constValue);
                        }

                        // 添加纹理采样器
                        for (const auto &param : variant->getShaderSamplerParams())
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

            mTechInstances.clear();
            for (const auto &tech : mShader->getTechniques())
            {
                if (tech != nullptr)
                {
                    mTechInstances.push_back(TechniqueInstance::create(this, tech));
                }
            }

            mLastEffectiveMaxLOD = computeEffectiveMaxLOD();
            selectTechniqueByLOD(mLastEffectiveMaxLOD);
            if (mCurTechnique == nullptr && mShader->getSupportTechnique() != nullptr)
            {
                mCurTechnique = TechniqueInstance::create(this, mShader->getSupportTechnique());
                mTechInstances.push_back(mCurTechnique);
            }

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
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Failed to load shader [uuid: %s] !", mShaderUUID.toString().c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }

            ret = init(false, archive);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
}
