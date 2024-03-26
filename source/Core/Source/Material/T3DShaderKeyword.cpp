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


#include "Material/T3DShaderKeyword.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderKeyword ShaderKeyword::INVALID;
    
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
        mName = name;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool ShaderKeyword::operator!=(const ShaderKeyword &other) const
    {
        return mHashCode != other.mHashCode;
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


