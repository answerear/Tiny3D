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

#ifndef __T3D_INTR_RAY_OBB_H__
#define __T3D_INTR_RAY_OBB_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"
#include "T3DObb.h"
#include "T3DRay.h"


namespace Tiny3D
{
    TSTRUCT()
    template <typename T>
    struct TIntrRayObb
    {
        TIntrRayObb();
        TIntrRayObb(const TRay<T> *ray, const TObb<T> *obb);
        TIntrRayObb(const TRay<T> &ray, const TObb<T> &obb);

        TFUNCTION()
        bool test();

        TFUNCTION()
        bool test(TVector3<T> &intersection);

        TFUNCTION()
        bool test(T &distance);

        TPROPERTY(RTTRFuncName="ray", RTTRFuncType="getter")
        const TRay<T> *getRay() const
        {
            return mRay;
        }

        TPROPERTY(RTTRFuncName="obb", RTTRFuncType="getter")
        const TAabb<T> *getObb() const
        {
            return mObb;
        }

        TPROPERTY(RTTRFuncName="ray", RTTRFuncType="setter")
        void setRay(const TRay<T> *ray)
        {
            mRay = ray;
        }

        TPROPERTY(RTTRFuncName="obb", RTTRFuncType="setter")
        void setObb(const TObb<T> *obb)
        {
            mObb = obb;
        }

    protected:
        const TRay<T>   *mRay;
        const TObb<T>   *mObb;
    };
}


#include "T3DIntrRayObb.inl"


#endif  /*__T3D_INTR_RAY_OBB_H__*/
