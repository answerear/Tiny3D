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


#ifndef __T3D_NATIVE_TYPE_WRAPPER_H__
#define __T3D_NATIVE_TYPE_WRAPPER_H__


#include "T3DUtilsPrerequisites.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    class T3D_UTILS_API Int8
    {
    public:
        Int8(int8_t val = 0) 
            : mValue(val) 
        {}

        operator int8_t& () { return mValue; }

    protected:
        int8_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_UTILS_API UInt8
    {
    public:
        UInt8(uint8_t val = 0)
            : mValue(val)
        {}

        operator uint8_t& () { return mValue; }

    protected:
        uint8_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_UTILS_API Int16
    {
    public:
        Int16(int16_t val = 0)
            : mValue(val)
        {}

        operator int16_t& () { return mValue; }

    protected:
        int16_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_UTILS_API UInt16
    {
    public:
        UInt16(uint16_t val = 0)
            : mValue(val)
        {}

        operator uint16_t& () { return mValue; }

    protected:
        uint16_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_UTILS_API Int32
    {
    public:
        Int32(int32_t val = 0)
            : mValue(val)
        {}

        operator int32_t& () { return mValue; }

    protected:
        int32_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_UTILS_API UInt32
    {
    public:
        UInt32(uint32_t val = 0)
            : mValue(val)
        {}

        operator uint32_t& () { return mValue; }

    protected:
        uint32_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_UTILS_API Int64
    {
    public:
        Int64(int64_t val = 0)
            : mValue(val)
        {}

        operator int64_t& () { return mValue; }

    protected:
        int64_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_UTILS_API UInt64
    {
    public:
        UInt64(uint64_t val = 0)
            : mValue(val)
        {}

        operator uint64_t& () { return mValue; }

    protected:
        uint64_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_UTILS_API Float32
    {
    public:
        Float32(float32_t val = 0.0f)
            : mValue(val)
        {}

        operator float32_t& () { return mValue; }

    protected:
        float32_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_UTILS_API Float64
    {
    public:
        Float64(float64_t val = 0.0)
            : mValue(val)
        {}

        operator float64_t& () { return mValue; }

    protected:
        float64_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_UTILS_API Boolean
    {
    public:
        Boolean(bool val = false)
            : mValue(val)
        {}

        operator bool& () { return mValue; }

    protected:
        bool mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_UTILS_API Char
    {
    public:
        Char(char val = 0)
            : mValue(val)
        {}

        operator char& () { return mValue; }

    protected:
        char mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_UTILS_API WChar
    {
    public:
        WChar(wchar_t val = 0)
            : mValue(val)
        {}

        operator wchar_t& () { return mValue; }

    protected:
        wchar_t mValue;
    };

    //--------------------------------------------------------------------------

//     class T3D_UTILS_API String
//     {
//     public:
//     };
}


#endif  /*__T3D_NATIVE_TYPE_WRAPPER_H__*/
