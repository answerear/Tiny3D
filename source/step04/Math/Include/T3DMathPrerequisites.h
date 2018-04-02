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

#ifndef __T3D_MATH_PREREQUISITES_H__
#define __T3D_MATH_PREREQUISITES_H__


#include "T3DMacro.h"
#include <stdarg.h>


#if defined T3DMATH_EXPORT
    #define T3D_MATH_API        T3D_EXPORT_API
#else
    #define T3D_MATH_API        T3D_IMPORT_API
#endif


#include "T3DPlatform.h"
#include "T3DLog.h"


#define __T3D_LOW_PRECISION_FIX__           0       // 32位定点数
#define __T3D_HIGH_PRECISION_FIX__          1       // 64位定点数
#define __T3D_LOW_PRECISION_FLOAT__         2       // 32位单精度浮点数
#define __T3D_HIGH_PRECISION_FLOAT__        3       // 64位双精度浮点数

#define __T3D_REAL_TYPE__   __T3D_LOW_PRECISION_FLOAT__ // 实数的精度类型


namespace Tiny3D
{
    class fix32;
    class fix64;

    class Degree;
    class Radian;

    class Vector2;
    class Vector3;
    class Vector4;

    class Quaternion;

    class Matrix2;
    class Matrix3;
    class Matrix4;
}

typedef float               float32_t;
typedef double              float64_t;

typedef Tiny3D::fix32		fix32_t;
typedef Tiny3D::fix64		fix64_t;

#if __T3D_REAL_TYPE__ == __T3D_LOW_PRECISION_FIX__
typedef fix32_t             Real;
#define REAL_ZERO           fix32_t::ZERO
#define REAL_ONE            fix32_t::ONE
#define REAL_HALF           fix32_t::HALF
#elif __T3D_REAL_TYPE__ == __T3D_HIGH_PRECISION_FIX__
typedef fix64_t             Real;
#define REAL_ZERO           fix64_t::ZERO
#define REAL_ONE            fix64_t::ONE
#define REAL_HALF           fix64_t::HALF
#elif __T3D_REAL_TYPE__ == __T3D_LOW_PRECISION_FLOAT__
typedef float32_t           Real;
#define REAL_ZERO           0.0f
#define REAL_ONE            1.0f
#define REAL_HALF           0.5f
#elif __T3D_REAL_TYPE__ == __T3D_HIGH_PRECISION_FLOAT__
typedef float64_t           Real;
#define REAL_ZERO           0.0
#define REAL_ONE            1.0
#define REAL_HALF           0.5
#endif


#endif    /*__T3D_MATH_PREREQUISITES_H__*/
