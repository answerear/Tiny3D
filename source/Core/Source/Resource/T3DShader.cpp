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

    ShaderConstantParam::ShaderConstantParam(const String &name, const void *data, uint32_t dataSize, uint32_t registerIdx, uint32_t registerNum, DataType dataType)
    {
        
    }

    //--------------------------------------------------------------------------

    ShaderConstantParam::~ShaderConstantParam()
    {
        
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

    ShaderKeyword::ShaderKeyword(uint32_t count)
        : mKeys(count)
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
        return false;
    }

    //--------------------------------------------------------------------------

    TResult ShaderKeyword::setKeyword(const String &keyword, uint32_t index)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ShaderKeyword::generate()
    {
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
        
    }

    //--------------------------------------------------------------------------

    void ShaderKeyword::move(ShaderKeyword &&other)
    {
        
    }

    //--------------------------------------------------------------------------

    ShaderVariantPtr ShaderVariant::create(ShaderKeyword &&keyword, const String &code)
    {
        return new ShaderVariant(std::move(keyword), code.c_str());
    }

    //--------------------------------------------------------------------------

    ShaderVariant::ShaderVariant(ShaderKeyword &&key, const String &code)
    {
        
    }

    //--------------------------------------------------------------------------

    ShaderVariant::~ShaderVariant()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult ShaderVariant::setParam(const String &name, void *data)
    {
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

    TResult Shader::enableKeyword(const String &keyword)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Shader::disableKeyword(const String &keyword)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Shader::loadData(DataStream &stream)
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


