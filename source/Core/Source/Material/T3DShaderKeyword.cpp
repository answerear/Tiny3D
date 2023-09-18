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


#include "Material/T3DShaderKeyword.h"


namespace Tiny3D
{
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
}


