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

#ifndef __T3D_INTR_POINT_SPHERE_H__
#define __T3D_INTR_POINT_SPHERE_H__


#include "T3DMathPrerequisites.h"
#include "T3DSphere.h"
#include "T3DVector3.h"


namespace Tiny3D
{
    TSTRUCT()
    template <typename T>
    struct TIntrPointSphere
    {
        TIntrPointSphere();
        TIntrPointSphere(const TVector3<T> *point, const TSphere<T> *sphere);
        TIntrPointSphere(const TVector3<T> &point, const TSphere<T> &sphere);

        TFUNCTION()
        bool test();

        TPROPERTY(RTTRFuncName="point", RTTRFuncType="getter")
        const TVector3<T> *getPoint() const
        {
            return mPoint;
        }

        TPROPERTY(RTTRFuncName="sphere", RTTRFuncType="getter")
        const TSphere<T> *getSphere() const
        {
            return mSphere;
        }

        TPROPERTY(RTTRFuncName="point", RTTRFuncType="setter")
        void setPoint(const TVector3<T> *point)
        {
            mPoint = point;
        }

        TPROPERTY(RTTRFuncName="sphere", RTTRFuncType="setter")
        void setSphere(const TSphere<T> *sphere)
        {
            mSphere = sphere;
        }

    private:
        const TVector3<T>   *mPoint;
        const TSphere<T>    *mSphere;
    };
}


#include "T3DIntrPointSphere.inl"


#endif  /*__T3D_INTR_POINT_SPHERE_H__*/
