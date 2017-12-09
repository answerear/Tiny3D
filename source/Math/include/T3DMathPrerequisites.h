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


#define __T3D_LOW_PRECISION_FIX__           0       // 32λ������
#define __T3D_HIGH_PRECISION_FIX__          1       // 64λ������
#define __T3D_LOW_PRECISION_FLOAT__         2       // 32λ�����ȸ�����
#define __T3D_HIGH_PRECISION_FLOAT__        3       // 64λ˫���ȸ�����

#define __T3D_REAL_TYPE__   __T3D_LOW_PRECISION_FLOAT__ // ʵ���ľ�������


namespace Tiny3D
{
    class fix32;
    class fix64;

    class Degree;
    class Radian;
    class Math;
    class Matrix2;
    class Matrix3;
    class Matrix4;
    class Quaternion;
    class Vector2;
    class Vector3;
    class Vector4;

    class Frustum;
    class Aabb;
    class Obb;
    class Sphere;
    class Plane;
    class Transform;
}

typedef float               float32_t;
typedef double              float64_t;

typedef Tiny3D::fix32		fix32_t;
typedef Tiny3D::fix64		fix64_t;

#if __T3D_REAL_TYPE__ == __T3D_LOW_PRECISION_FIX__
typedef fix32_t             Real;
#elif __T3D_REAL_TYPE__ == __T3D_HIGH_PRECISION_FIX__
typedef fix64_t             Real;
#elif __T3D_REAL_TYPE__ == __T3D_LOW_PRECISION_FLOAT__
typedef float32_t           Real;
#elif __T3D_REAL_TYPE__ == __T3D_HIGH_PRECISION_FLOAT__
typedef float64_t           Real;
#endif


#endif    /*__T3D_MATH_PREREQUISITES_H__*/
