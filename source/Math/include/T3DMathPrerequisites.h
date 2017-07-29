/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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


namespace Tiny3D
{
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


#endif    /*__T3D_MATH_PREREQUISITES_H__*/
