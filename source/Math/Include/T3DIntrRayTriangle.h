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

#ifndef __T3D_INTR_RAY_TRIANGLE_H__
#define __T3D_INTR_RAY_TRIANGLE_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"
#include "T3DRay.h"
#include "T3DTriangle.h"


namespace Tiny3D
{
    TSTRUCT()
    template <typename T>
    struct TIntrRayTriangle
    {
        TIntrRayTriangle();
        TIntrRayTriangle(const TRay<T> *ray, const TTriangle<T> *triangle);
        TIntrRayTriangle(const TRay<T> &ray, const TTriangle<T> &triangle);

        TFUNCTION()
        bool test();

        TFUNCTION()
        bool test(TVector3<T> &intersection);

        TPROPERTY(RTTRFuncName="ray", RTTRFuncType="getter")
        const TRay<T> *getRay() const
        {
            return mRay;
        }

        TPROPERTY(RTTRFuncName="triangle", RTTRFuncType="getter")
        const TTriangle<T> *getTriangle() const
        {
            return mTriangle;
        }

        TPROPERTY(RTTRFuncName="ray", RTTRFuncType="setter")
        void setRay(const TRay<T> *ray)
        {
            mRay = ray;
        }

        TPROPERTY(RTTRFuncName="triangle", RTTRFuncType="setter")
        void setTriangle(const TTriangle<T> *triangle)
        {
            mTriangle = triangle;
        }

    private:
        const TRay<T>       *mRay;
        const TTriangle<T>  *mTriangle;
    };
}


#include "T3DIntrRayTriangle.inl"


#endif  /*__T3D_INTR_RAY_TRIANGLE_H__*/
