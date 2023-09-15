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

    PassPtr Pass::create(const String &name)
    {
        return new Pass(name);
    }

    //--------------------------------------------------------------------------

    Pass::Pass(const String &name)
        : mName(name)
    {
        
    }

    //--------------------------------------------------------------------------

    PassPtr Pass::clone() const
    {
        PassPtr pass = create(getName());
        pass->mShaderName = mShaderName;
        pass->mTags = mTags;
        pass->mRenderState = mRenderState;
        pass->mShader = mShader;
        return pass;
    }
    
    //--------------------------------------------------------------------------
    
    bool Pass::addTag(const String &key, const String &value)
    {
        auto rval = mTags.emplace(key, value);
        return rval.second;
    }
    
    //--------------------------------------------------------------------------

    void Pass::removeTag(const String &key)
    {
        mTags.erase(key);
    }
    
    //--------------------------------------------------------------------------

    bool Pass::getTag(const String &key, String &value) const
    {
        bool ret = false;
        const auto itr = mTags.find(key);
        if (itr != mTags.end())
        {
            value = itr->second;
            ret = true;
        }
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TechniquePtr Technique::create(const String &name)
    {
        return new Technique(name);
    }

    //--------------------------------------------------------------------------

    Technique::Technique(const String &name)
        : mName(name)
    {
        
    }

    //--------------------------------------------------------------------------

    TechniquePtr Technique::clone() const
    {
        TechniquePtr tech = create(getName());
        tech->mLOD = mLOD;
        tech->mTags = mTags;
        for (auto pass : mPasses)
        {
            tech->mPasses.emplace_back(pass->clone());
        }
        return tech;
    }

    //--------------------------------------------------------------------------

    bool Technique::addTag(const String &key, const String &value)
    {
        auto rval = mTags.emplace(key, value);
        return rval.second;
    }
    
    //--------------------------------------------------------------------------

    void Technique::removeTag(const String &key)
    {
        mTags.erase(key);
    }
    
    //--------------------------------------------------------------------------

    bool Technique::getTag(const String &key, String &value) const
    {
        bool ret = false;
        const auto itr = mTags.find(key);
        if (itr != mTags.end())
        {
            value = itr->second;
            ret = true;
        }
        return ret;
    }
    
    //--------------------------------------------------------------------------

    bool Technique::addPass(PassPtr pass)
    {
        bool found = false;
        for (const auto p : mPasses)
        {
            if (p->getName() == pass->getName())
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            mPasses.emplace_back(pass);
        }

        return !found;
    }
    
    //--------------------------------------------------------------------------

    void Technique::removePass(const String &name)
    {
        for (auto itr = mPasses.begin(); itr != mPasses.end(); ++itr)
        {
            if ((*itr)->getName() == name)
            {
                mPasses.erase(itr);
                break;
            }
        }
    }
    
    //--------------------------------------------------------------------------

    bool Technique::getPass(const String &name, PassPtr &pass) const
    {
        bool ret = false;
        for (const auto p : mPasses)
        {
            if (p->getName() == name)
            {
                pass = p;
                ret = true;
                break;
            }
        }
        return ret;
    }
    
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
        clearConstantData();
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
        mTechniques.clear();
        for (auto tech : material->mTechniques)
        {
            mTechniques.emplace_back(tech);
        }
    }

    //--------------------------------------------------------------------------

    bool Material::addTechnique(TechniquePtr tech)
    {
        bool found = false;
        for (const auto t : mTechniques)
        {
            if (t->getName() == tech->getName())
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            mTechniques.push_back(tech);
        }

        return !found;
    }

    //--------------------------------------------------------------------------

    void Material::removeTechnique(const String &name)
    {
        for (auto itr = mTechniques.begin(); itr != mTechniques.end(); ++itr)
        {
            if ((*itr)->getName() == name)
            {
                mTechniques.erase(itr);
                break;
            }
        }
    }

    //--------------------------------------------------------------------------

    bool Material::getTechnique(const String &name, TechniquePtr &tech) const
    {
        bool found = false;
        
        for (const auto t : mTechniques)
        {
            if (t->getName() == name)
            {
                tech = t;
                found = true;
                break;
            }
        }

        return found;
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

    void Material::clearConstantData()
    {
        for (auto val : mConstantData)
        {
            val.second.release();
        }
    }

    //--------------------------------------------------------------------------

    void Material::setInteger(const String &name, int32_t value)
    {
        setValue<int32_t>(name, &value);
    }

    //--------------------------------------------------------------------------

    int32_t Material::getInteger(const String &name) const
    {
        return getValue<int32_t>(name);
    }
    
    //--------------------------------------------------------------------------

    bool Material::hasInteger(const String &name) const
    {
        return hasValue<int32_t>(name);
    }

    //--------------------------------------------------------------------------

    void Material::setFloat(const String &name, float32_t value)
    {
        setValue<float32_t>(name, &value);
    }

    //--------------------------------------------------------------------------

    float32_t Material::getFloat(const String &name) const
    {
        return getValue<float32_t>(name);
    }

    //--------------------------------------------------------------------------

    bool Material::hasFloat(const String &name) const
    {
        return hasValue<float32_t>(name);
    }

    //--------------------------------------------------------------------------

    void Material::setFloatArray(const String &name, const FloatArray &values)
    {
        setValue<float32_t>(name, &values[0]);
    }

    //--------------------------------------------------------------------------

    FloatArray Material::getFloatArray(const String &name) const
    {
        return getValues<float_t>(name);
    }

    //--------------------------------------------------------------------------

    void Material::setColor(const String &name, const ColorRGBA &value)
    {
        setValue<ColorRGBA>(name, &value);
    }

    //--------------------------------------------------------------------------

    ColorRGBA Material::getColor(const String &name) const
    {
        return getValue<ColorRGBA>(name);
    }

    //--------------------------------------------------------------------------

    bool Material::hasColor(const String &name) const
    {
        return hasValue<ColorRGBA>(name);
    }

    //--------------------------------------------------------------------------

    void Material::setColorArray(const String &name, const ColorArray &values)
    {
        setValue<uint8_t>(name, &values[0]);
    }

    //--------------------------------------------------------------------------

    ColorArray Material::getColorArray(const String &name) const
    {
        return getValues<ColorRGBA>(name);
    }

    //--------------------------------------------------------------------------

    void Material::setVector(const String &name, const Vector4 &value)
    {
        setValue<Vector4>(name, &value);
    }

    //--------------------------------------------------------------------------

    Vector4 Material::getVector(const String &name) const
    {
        return getValue<Vector4>(name);
    }

    //--------------------------------------------------------------------------

    bool Material::hasVector(const String &name) const
    {
        return hasValue<Vector4>(name);
    }

    //--------------------------------------------------------------------------

    void Material::setVectorArray(const String &name, const Vector4Array &values)
    {
        setValue<Vector4>(name, &values[0]);
    }

    //--------------------------------------------------------------------------

    Vector4Array Material::getVectorArray(const String &name) const
    {
        return getValues<Vector4>(name);
    }
    
    //--------------------------------------------------------------------------

    void Material::setMatrix(const String &name, const Matrix4 &value)
    {
        setValue<Matrix4>(name, &value);
    }

    //--------------------------------------------------------------------------

    Matrix4 Material::getMatrix(const String &name) const
    {
        return getValue<Matrix4>(name);
    }
    
    //--------------------------------------------------------------------------

    bool Material::hasMatrix(const String &name) const
    {
        return hasValue<Matrix4>(name);
    }
    
    //--------------------------------------------------------------------------

    void Material::setMatrixArray(const String &name, const Matrix4Array &values)
    {
        setValue<Matrix4>(name, &values[0]);
    }

    //--------------------------------------------------------------------------

    Matrix4Array Material::getMatrixArray(const String &name) const
    {
        return getValues<Matrix4>(name);
    }
    
    //--------------------------------------------------------------------------

    void Material::setTexture(const String &name, const TextureState &value)
    {
        auto itr = mTextureData.find(name);
        if (itr != mTextureData.end())
        {
            itr->second = value;
        }
        else
        {
            mTextureData.emplace(name, value);
        }
    }

    //--------------------------------------------------------------------------

    TextureState Material::getTexture(const String &name) const
    {
        TextureState tex;
        auto itr = mTextureData.find(name);
        if (itr != mTextureData.end())
        {
            tex = itr->second;
        }
        return tex;
    }

    //--------------------------------------------------------------------------

    bool Material::hasTexture(const String &name) const
    {
        const auto itr = mTextureData.find(name);
        return (itr != mTextureData.end());
    }

    //--------------------------------------------------------------------------
}
