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
        /// 求平方.
        static Real sqr(Real value);
        /// 求平方根.
        static Real sqrt(Real value);
        /// 求平方根倒数.
        static Real invSqrt(Real value);

        /// 求绝对值
        static Real abs(Real value);
        static Degree abs(const Degree& value);
        static Radian abs(const Radian& value);

        /// 根据误差值判断两个实数是否相等.
        static bool realEqual(Real a, Real b, 
            Real tolerance = std::numeric_limits<Real>::epsilon());

        /// 通过角度求正弦值.
        static Real sin(const Degree &degrees);
        /// 通过弧度求正弦值.
        static Real sin(const Radian &radians);
        /// 通过角度求余弦值.
        static Real cos(const Degree &degrees);
        /// 通过弧度求余弦值.
        static Real cos(const Radian &radians);
        /// 通过角度求正切值.
        static Real tan(const Degree &degrees);
        /// 通过弧度求正切值.
        static Real tan(const Radian &radians);

        /// 求反正弦.
        static Radian asin(Real value);
        /// 求反余弦.
        static Radian acos(Real value);
        /// 求反正切.
        static Radian atan(Real value);
        /// 通过 y/x 求反正切
        static Radian atan2(Real fY, Real fX);

        static Real degreesToRadians(Real degrees);
        static Real radiansToDegrees(Real radians);

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
