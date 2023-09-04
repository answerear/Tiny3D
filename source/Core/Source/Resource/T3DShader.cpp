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


#include "Resource/T3DShader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderConstantParam::ShaderConstantParam(const String &name, const void *data, uint32_t dataSize, uint32_t registerIdx, uint32_t registerNum, DATA_TYPE dataType)
        : mDataType(dataType)
        , mRegisterIndex(registerIdx)
        , mRegisterNum(registerNum)
        , mName(name)
    {
        if (data != nullptr && dataSize > 0)
        {
            mData = new uint8_t[dataSize];
            memcpy(mData, data, dataSize);
            mDataSize = dataSize;
        }
    }

    //--------------------------------------------------------------------------

    ShaderConstantParam::~ShaderConstantParam()
    {
        T3D_SAFE_DELETE_ARRAY(mData);
    }

    //--------------------------------------------------------------------------

    ShaderSamplerParam::ShaderSamplerParam(const String &name, uint32_t texType, uint32_t registerIdx, uint32_t registerNum)
    {
        
    }

    //--------------------------------------------------------------------------

    ShaderSamplerParam::~ShaderSamplerParam()
    {
        
    }

    //--------------------------------------------------------------------------

    ShaderKeyword::ShaderKeyword()
    {
        
    }

    //--------------------------------------------------------------------------

    ShaderKeyword::ShaderKeyword(const ShaderKeyword &other)
    {
        copy(other);
    }

    //--------------------------------------------------------------------------

    ShaderKeyword::ShaderKeyword(ShaderKeyword &&other) noexcept
    {
        move(std::move(other));
    }

    //--------------------------------------------------------------------------

    bool ShaderKeyword::hasKey(const String &key) const
    {
        auto itr = mKeys.find(key); // std::find(mKeys.begin(), mKeys.end(), key);
        return (itr != mKeys.end());
    }

    //--------------------------------------------------------------------------

    TResult ShaderKeyword::addKeyword(const String &keyword)
    {
        mKeys.insert(keyword);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ShaderKeyword::removeKeyword(const String &keyword)
    {
        mKeys.erase(keyword);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ShaderKeyword::generate()
    {
        String name;
        for (const auto &str : mKeys)
        {
            name += "_";
            name += str;
        }
        mHashCode = Hash::hash(name.c_str());
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool ShaderKeyword::operator==(const ShaderKeyword &other) const
    {
        return mHashCode == other.mHashCode;
    }

    //--------------------------------------------------------------------------

    bool ShaderKeyword::operator<(const ShaderKeyword &other) const
    {
        return mHashCode < other.mHashCode;
    }

    //--------------------------------------------------------------------------

    bool ShaderKeyword::operator>(const ShaderKeyword &other) const
    {
        return mHashCode > other.mHashCode;
    }

    //--------------------------------------------------------------------------

    ShaderKeyword &ShaderKeyword::operator=(const ShaderKeyword &other)
    {
        copy(other);
        return *this;
    }

    //--------------------------------------------------------------------------

    ShaderKeyword &ShaderKeyword::operator=(ShaderKeyword &&other) noexcept
    {
        move(std::move(other));
        return *this;
    }

    //--------------------------------------------------------------------------

    void ShaderKeyword::copy(const ShaderKeyword &other)
    {
        mKeys = other.mKeys;
        mHashCode = other.mHashCode;
    }

    //--------------------------------------------------------------------------

    void ShaderKeyword::move(ShaderKeyword &&other)
    {
        mKeys = std::move(other.mKeys);
        mHashCode = other.mHashCode;
    }

    //--------------------------------------------------------------------------

    ShaderVariantPtr ShaderVariant::create(ShaderKeyword &&keyword, const String &code)
    {
        return new ShaderVariant(std::move(keyword), code);
    }

    //--------------------------------------------------------------------------

    ShaderVariant::ShaderVariant(ShaderKeyword &&key, const String &code)
    {
        mShaderKeyword = new ShaderKeyword(std::move(key));
        setSourceCode(code.c_str(), code.length());
    }

    //--------------------------------------------------------------------------

    ShaderVariant::~ShaderVariant()
    {
        T3D_SAFE_DELETE(mShaderKeyword);
    }

    //--------------------------------------------------------------------------

    TResult ShaderVariant::setParam(const String &name, void *data)
    {
        if (data == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid parameter when call ShaderVariant::setParam()");
            return T3D_ERR_INVALID_PARAM;
        }

        for (auto param : mConstants)
        {
            if (name == param->getName())
            {
                void *dst = param->getData();
                if (data == nullptr)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid parameter data when call ShaderVariant::setParam()");
                    return T3D_ERR_INVALID_POINTER;
                }
                memcpy(dst, data, param->getSize());
                break;
            }
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ShaderVariant::setParam(const String &name, int32_t index)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    ShaderPtr Shader::create(const String &name)
    {
        return new Shader(name);
    }

    //--------------------------------------------------------------------------

    Shader::Shader(const String &name)
        : Resource(name)
    {
        
    }

    //--------------------------------------------------------------------------

    Shader::~Shader()
    {
        
    }
    
    //--------------------------------------------------------------------------

    Resource::Type Shader::getType() const
    {
        return Type::kShader;
    }

    //--------------------------------------------------------------------------
    
    TResult Shader::addShaderVariant(const ShaderKeyword &keyword, ShaderVariantPtr variant)
    {
        TResult ret = T3D_OK;

        auto itr = std::find(mKeywords.begin(), mKeywords.end(), keyword);
        if (itr != mKeywords.end())
        {
            T3D_LOG_WARNING(LOG_TAG_RESOURCE, "Add shader variant failed ! Keyword duplicated !");
            return T3D_ERR_DUPLICATED_ITEM;
        }

        mKeywords.push_back(keyword);
        auto rval = mVariants.insert(ShaderVariantsValue(keyword, variant));

        if (mCurrentKeyword == nullptr)
        {
            mCurrentKeyword = &(rval.first->first);
            mCurrentVariant = variant;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Shader::enableKeyword(const String &key)
    {
        TResult ret = T3D_OK;

        do 
        {
            bool found = false;
            for (const auto &keyword : mKeywords)
            {
                if (keyword.hasKey(key))
                {
                    found = true;
                    mCurrentKeyword = &keyword;
                    break;
                }
            }

            if (!found)
            {
                T3D_LOG_WARNING(LOG_TAG_RESOURCE,
                                "Enable keyword (%s) failed !", key.c_str());
                ret = T3D_ERR_NOT_FOUND;
                break;
            }

            auto itr = mVariants.find(*mCurrentKeyword);
            if (itr == mVariants.end())
            {
                T3D_LOG_WARNING(
                    LOG_TAG_RESOURCE,
                    "Could not find shader variant for keyword (%s) !",
                    key.c_str());
                ret = T3D_ERR_NOT_FOUND;
                break;
            }

            mCurrentVariant = itr->second;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Shader::disableKeyword(const String &keyword)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ResourcePtr Shader::clone() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
}


