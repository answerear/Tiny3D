/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#ifndef __T3D_TYPE_H__
#define __T3D_TYPE_H__


#include <string>
#include <memory>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <algorithm>

#define __T3D_LOW_PRECISION_FIX__           0       // 32位定点数
#define __T3D_HIGH_PRECISION_FIX__          1       // 64位定点数
#define __T3D_LOW_PRECISION_FLOAT__         2       // 32位单精度浮点数
#define __T3D_HIGH_PRECISION_FLOAT__        3       // 64位双精度浮点数

#define __T3D_REAL_TYPE__   __T3D_LOW_PRECISION_FLOAT__ // 实数的精度类型

namespace Tiny3D
{
	class fix32;
	class fix64;
}


typedef signed char         char_t;
typedef unsigned char       uchar_t;
typedef signed short        short_t;
typedef unsigned short      ushort_t;
typedef signed int          int_t;
typedef unsigned int        uint_t;
typedef signed long         long_t;
typedef unsigned long       ulong_t;

typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef signed short        int16_t;
typedef unsigned short      uint16_t;
typedef signed int          int32_t;
typedef unsigned int        uint32_t;

typedef signed long long    int64_t;
typedef unsigned long long  uint64_t;

typedef float               float32_t;
typedef double              float64_t;

typedef Tiny3D::fix32		fix32_t;
typedef Tiny3D::fix64		fix64_t;

typedef std::string         String;
typedef std::wstring        WString;

typedef std::string         UTF8String;
typedef std::u16string      UTF16String;
typedef std::u32string      UTF32String;

typedef void*               THandle;

#if __T3D_REAL_TYPE__ == __T3D_LOW_PRECISION_FIX__
typedef fix32_t             real_t;
#elif __T3D_REAL_TYPE__ == __T3D_HIGH_PRECISION_FIX__
typedef fix64_t             real_t;
#elif __T3D_REAL_TYPE__ == __T3D_LOW_PRECISION_FLOAT__
typedef float32_t           real_t;
#elif __T3D_REAL_TYPE__ == __T3D_HIGH_PRECISION_FLOAT__
typedef float64_t           real_t;
#endif



#endif  /*__T3D_TYPE_H__*/