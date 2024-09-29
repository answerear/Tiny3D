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

#ifndef __T3D_MATH_LIB_H__
#define __T3D_MATH_LIB_H__


#include <T3DMathPrerequisites.h>

#include <T3DReal.h>
#include <T3DDegree.h>
#include <T3DRadian.h>
#include <T3DMath.h>
#include <T3DColor4.h>
#include <T3DColorRGB.h>
#include <T3DColorARGB.h>
#include <T3DColorRGBA.h>
#include <T3DRect.h>
#include <T3DVector2.h>
#include <T3DVector3.h>
#include <T3DVector4.h>
#include <T3DMatrix2.h>
#include <T3DMatrix3.h>
#include <T3DQuaternion.h>
#include <T3DMatrix4.h>
#include <T3DRay.h>
#include <T3DPlane.h>
#include <T3DTriangle.h>
#include <T3DSphere.h>
#include <T3DAabb.h>
#include <T3DObb.h>
#include <T3DFrustum.h>

#include <T3DIntrRayTriangle.h>
#include <T3DIntrRayPlane.h>
#include <T3DIntrRaySphere.h>
#include <T3DIntrRayAabb.h>
#include <T3DIntrRayObb.h>

#include <T3DIntrSphereTriangle.h>
#include <T3DIntrSpherePlane.h>
#include <T3DIntrSphereSphere.h>
#include <T3DIntrSphereObb.h>
#include <T3DIntrSphereAabb.h>

#include <T3DIntrAabbPlane.h>
#include <T3DIntrAabbAabb.h>
#include <T3DIntrAabbObb.h>

#include <T3DIntrObbPlane.h>
#include <T3DIntrObbObb.h>

#include <T3DIntrFrustumAabb.h>
#include <T3DIntrFrustumObb.h>
#include <T3DIntrFrustumSphere.h>

#include <T3DIntrPointSphere.h>
#include <T3DIntrPointAabb.h>
#include <T3DIntrPointObb.h>
#include <T3DIntrPointFrustum.h>


namespace Tiny3D
{
    #define __T3D_LOW_PRECISION_FLOAT__         0       // 32位单精度浮点数
    #define __T3D_HIGH_PRECISION_FLOAT__        1       // 64位双精度浮点数
    #define __T3D_LOW_PRECISION_FIX__           2       // 32位定点数
    #define __T3D_HIGH_PRECISION_FIX__          3       // 64位定点数

    #define __T3D_REAL_TYPE__   __T3D_LOW_PRECISION_FLOAT__ // 实数的精度类型

    #if __T3D_REAL_TYPE__ == __T3D_LOW_PRECISION_FIX__
        using Real = fix32_t;
    #elif __T3D_REAL_TYPE__ == __T3D_HIGH_PRECISION_FIX__
        using Real = fix64_t;
    #elif __T3D_REAL_TYPE__ == __T3D_LOW_PRECISION_FLOAT__
        using Real = float32_t;
    #elif __T3D_REAL_TYPE__ == __T3D_HIGH_PRECISION_FLOAT__
        using Real = float64_t;
    #endif


    using Degree = TDegree<Real>;
    using Radian = TRadian<Real>;
    using Math = TMath<Real>;

    using Size = TSize<size_t>;
    using RealSize = TSize<Real>;

    using Point = TPoint<size_t>;
    using RealPoint = TPoint<Real>;

    using Rect = TRect<size_t>;
    using RealRect = TRect<Real>;

    using Vector2 = TVector2<Real>;
    using Vector3 = TVector3<Real>;
    using Vector4 = TVector4<Real>;
    using Matrix2 = TMatrix2<Real>;
    using Matrix3 = TMatrix3<Real>;
    using Matrix4 = TMatrix4<Real>;
    using Quaternion = TQuaternion<Real>;

    using Ray = TRay<Real>;
    using Triangle = TTriangle<Real>;
    using Plane = TPlane<Real>;
    using Sphere = TSphere<Real>;
    using Aabb = TAabb<Real>;
    using Obb = TObb<Real>;
    using Frustum = TFrustum<Real>;

    using IntrRayTriangle = TIntrRayTriangle<Real>;
    using IntrRayPlane = TIntrRayPlane<Real>;
    using IntrRaySphere = TIntrRaySphere<Real>;
    using IntrRayAabb = TIntrRayAabb<Real>;
    using IntrRayObb = TIntrRayObb<Real>;

    using IntrSphereTriangle = TIntrSphereTriangle<Real>;
    using IntrSpherePlane = TIntrSpherePlane<Real>;
    using IntrSphereSphere = TIntrSphereSphere<Real>;
    using IntrSphereAabb = TIntrSphereAabb<Real>;
    using IntrSphereObb = TIntrSphereObb<Real>;

    using IntrAabbPlane = TIntrAabbPlane<Real>;
    using IntrAabbAabb = TIntrAabbAabb<Real>;
    using IntrAabbObb = TIntrAabbObb<Real>;

    using IntrObbPlane = TIntrObbPlane<Real>;
    using IntrObbObb = TIntrObbObb<Real>;

    using IntrFrustumSphere = TIntrFrustumSphere<Real>;
    using IntrFrustumAabb = TIntrFrustumAabb<Real>;
    using IntrFrustumObb = TIntrFrustumObb<Real>;

    using IntrPointSphere = TIntrPointSphere<Real>;
    using IntrPointAabb = TIntrPointAabb<Real>;
    using IntrPointObb = TIntrPointObb<Real>;
    using IntrPointFrustum = TIntrPointFrustum<Real>;


    #define REAL_ZERO           TReal<Real>::ZERO
    #define REAL_HALF           TReal<Real>::HALF
    #define REAL_ONE            TReal<Real>::ONE
    #define REAL_MINUS_ONE      TReal<Real>::MINUS_ONE
    #define REAL_INF            TReal<Real>::INF
    #define REAL_MINUS_INF      TReal<Real>::MINUS_INF
    #define REAL_EPSILON        TReal<Real>::EPSILON
}


#endif  /*__T3D_MATH_LIB_H__*/
