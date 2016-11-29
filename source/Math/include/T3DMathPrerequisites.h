

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
