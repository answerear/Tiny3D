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


#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "T3DMacro.h"
#include "Reflection/T3DReflection.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    class T3D_PLATFORM_API Int8
    {
        T3D_DECLARE_CLASS();

    public:
        Int8(int8_t val = 0)
            : mValue(val)
        {}

        operator int8_t&() { return mValue; }

    protected:
        int8_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_PLATFORM_API UInt8
    {
        T3D_DECLARE_CLASS();

    public:
        UInt8(uint8_t val = 0)
            : mValue(val)
        {}

        operator uint8_t&() { return mValue; }

    protected:
        uint8_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_PLATFORM_API Int16
    {
        T3D_DECLARE_CLASS();

    public:
        Int16(int16_t val = 0)
            : mValue(val)
        {}

        operator int16_t&() { return mValue; }
    protected:
        int16_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_PLATFORM_API UInt16
    {
        T3D_DECLARE_CLASS();

    public:
        UInt16(uint16_t val = 0)
            : mValue(val)
        {}

        operator uint16_t&() { return mValue; }

    protected:
        uint16_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_PLATFORM_API Int32
    {
        T3D_DECLARE_CLASS();

    public:
        Int32(int32_t val = 0)
            : mValue(val)
        {}

        operator int32_t&() { return mValue; }

    protected:
        int32_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_PLATFORM_API UInt32
    {
        T3D_DECLARE_CLASS();

    public:
        UInt32(uint32_t val = 0)
            : mValue(val)
        {}

        operator uint32_t&() { return mValue; }

    protected:
        uint32_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_PLATFORM_API Int64
    {
        T3D_DECLARE_CLASS();

    public:
        Int64(int64_t val = 0)
            : mValue(val)
        {}

        operator int64_t&() { return mValue; }

    protected:
        int64_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_PLATFORM_API UInt64
    {
        T3D_DECLARE_CLASS();

    public:
        UInt64(uint64_t val = 0)
            : mValue(val)
        {}

        operator uint64_t&() { return mValue; }

    protected:
        uint64_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_PLATFORM_API Float32
    {
        T3D_DECLARE_CLASS();

    public:
        Float32(float32_t val = 0.0f)
            : mValue(val)
        {}

        operator float32_t&() { return mValue; }

    protected:
        float32_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_PLATFORM_API Float64
    {
        T3D_DECLARE_CLASS();

    public:
        Float64(float64_t val = 0.0)
            : mValue(val)
        {}

        operator float64_t&() { return mValue; }

    protected:
        float64_t mValue;
    };

    //--------------------------------------------------------------------------

    class T3D_PLATFORM_API Boolean
    {
        T3D_DECLARE_CLASS();

    public:
        Boolean(bool val = false)
            : mValue(val)
        {}

        operator bool&() { return mValue; }

    protected:
        bool mValue;
    };
}


#endif  /*__T3D_NATIVE_TYPE_WRAPPER_H__*/
