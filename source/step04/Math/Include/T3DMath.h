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

#ifndef __T3D_MATH_H__
#define __T3D_MATH_H__


#include "T3DMathPrerequisites.h"
#include <math.h>
#include "T3DFix32.h"
#include "T3DFix64.h"
#include "T3DRadian.h"
#include "T3DDegree.h"


namespace Tiny3D
{
    class T3D_MATH_API Math
    {
    public:
        /// Square.
        static Real Sqr(Real fValue);
        /// Square root.
        static Real Sqrt(Real fValue);

        static Real InvSqrt(Real fValue);

        static Real Abs (Real fValue);
        static Degree Abs (const Degree& dValue);
        static Radian Abs (const Radian& rValue);

        /// Compare 2 reals, using tolerance for inaccuracies.
        static bool RealEqual(Real a, Real b, Real tolerance = std::numeric_limits<Real>::epsilon());

        /// Sine in degree.
        static Real Sin(const Degree &degrees);
        /// Sine in radian.
        static Real Sin(const Radian &radians);
        /// Cosine in degree.
        static Real Cos(const Degree &degrees);
        /// Cosine in radian.
        static Real Cos(const Radian &radians);
        /// Tangent in degree.
        static Real Tan(const Degree &degrees);
        /// Tangent in radians.
        static Real Tan(const Radian &radians);

        /// Arc-sine.
        static Radian ASin(Real fValue);
        /// Arc-cosine.
        static Radian ACos(Real fValue);
        /// Arc-tangent.
        static Radian ATan(Real fValue);
        /// Arc-tangent of y/x
        static Radian ATan2(Real fY, Real fX);

        static Real DegreesToRadians(Real degrees);
        static Real RadiansToDegrees(Real radians);

        static bool intersects(const Sphere &sphere1, const Sphere &sphere2);
        static bool intersects(const Sphere &sphere, const Aabb &box);
        static bool intersects(const Sphere &sphere, const Obb &box);
        static bool intersects(const Sphere &sphere, const Frustum &frustum);
        static bool intersects(const Sphere &sphere, const Plane &plane);
        static bool intersects(const Aabb &aabb1, const Aabb &aabb2);
        static bool intersects(const Aabb &aabb, const Obb &obb);
        static bool intersects(const Aabb &aabb, const Frustum &frustum);
        static bool intersects(const Aabb &box, const Plane &plane);
        static bool intersects(const Obb &box1, const Obb &box2);
        static bool intersects(const Obb &obb, const Frustum &frustum);
        static bool intersects(const Obb &box, const Plane &plane);

    public:
        static const Real POS_INFINITY;
        static const Real NEG_INFINITY;
        static const Real PI;
        static const Real TWO_PI;
        static const Real HALF_PI;
        static const Real DEGREES_TO_RADIANS;
        static const Real RADIANS_TO_DEGREES;
    };
}


#include "T3DMath.inl"


#endif  /*__T3D_MATH_H__*/
