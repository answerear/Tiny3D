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
    TSTRUCT()
    template <typename T>
    struct TMath
    {
        /// 求平方.
        TFUNCTION()
        static T sqr(T value);
        
        /// 求平方根.
        TFUNCTION()
        static T sqrt(T value);
        
        /// 求平方根倒数.
        TFUNCTION()
        static T invSqrt(T value);

        /// 求绝对值
        TFUNCTION()
        static T abs(T value);

        TFUNCTION()
        static TDegree<T> abs(const TDegree<T>& value);
        
        TFUNCTION()
        static TRadian<T> abs(const TRadian<T>& value);

        /// 根据误差值判断两个实数是否相等.
        TFUNCTION()
        static bool realEqual(T a, T b, 
            T tolerance = std::numeric_limits<T>::epsilon());

        /// 通过角度求正弦值.
        TFUNCTION()
        static T sin(const TDegree<T> &degrees);
        
        /// 通过弧度求正弦值.
        TFUNCTION()
        static T sin(const TRadian<T> &radians);
        
        /// 通过角度求余弦值.
        TFUNCTION()
        static T cos(const TDegree<T> &degrees);
        
        /// 通过弧度求余弦值.
        TFUNCTION()
        static T cos(const TRadian<T> &radians);
        
        /// 通过角度求正切值.
        TFUNCTION()
        static T tan(const TDegree<T> &degrees);
        
        /// 通过弧度求正切值.
        TFUNCTION()
        static T tan(const TRadian<T> &radians);

        /// 求反正弦.
        TFUNCTION()
        static TRadian<T> asin(T value);
        
        /// 求反余弦.
        TFUNCTION()
        static TRadian<T> acos(T value);
        
        /// 求反正切.
        TFUNCTION()
        static TRadian<T> atan(T value);
        
        /// 通过 y/x 求反正切
        TFUNCTION()
        static TRadian<T> atan2(T y, T x);

        TFUNCTION()
        static T degreesToRadians(T degrees);

        TFUNCTION()
        static T radiansToDegrees(T radians);

        TFUNCTION()
        static T max(T a, T b);

        TFUNCTION()
        static T min(T a, T b);

        static const T POS_INFINITY;
        static const T NEG_INFINITY;
        static const T PI;
        static const T TWO_PI;
        static const T HALF_PI;
        static const T DEGREES_TO_RADIANS;
        static const T RADIANS_TO_DEGREES;
    };
}


#include "T3DMath.inl"


#endif  /*__T3D_MATH_H__*/
