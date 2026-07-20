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

#ifndef __T3D_INTR_RAY_CAPSULE_H__
#define __T3D_INTR_RAY_CAPSULE_H__


#include "T3DMathPrerequisites.h"
#include "T3DRay.h"
#include "T3DCapsule.h"


namespace Tiny3D
{
    TSTRUCT()
    template <typename T>
    struct TIntrRayCapsule
    {
        TIntrRayCapsule();
        TIntrRayCapsule(const TRay<T> &ray, const TCapsule<T> &capsule);
        TIntrRayCapsule(const TRay<T> *ray, const TCapsule<T> *capsule);

        TPROPERTY(RTTRFuncName="ray", RTTRFuncType="getter")
        const TRay<T> *getRay() const
        {
            return mRay;
        }

        TPROPERTY(RTTRFuncName="capsule", RTTRFuncType="getter")
        const TCapsule<T> *getCapsule() const
        {
            return mCapsule;
        }

        TPROPERTY(RTTRFuncName="ray", RTTRFuncType="setter")
        void setRay(const TRay<T> *ray)
        {
            mRay = ray;
        }

        TPROPERTY(RTTRFuncName="capsule", RTTRFuncType="setter")
        void setCapsule(const TCapsule<T> *capsule)
        {
            mCapsule = capsule;
        }

        TFUNCTION()
        bool test();

        TFUNCTION()
        bool test(TVector3<T> &intersection);

        TFUNCTION()
        bool test(T &distance);

    protected:
        const TRay<T>       *mRay;
        const TCapsule<T>   *mCapsule;
    };
}


#include "T3DIntrRayCapsule.inl"


#endif  /*__T3D_INTR_RAY_CAPSULE_H__*/
