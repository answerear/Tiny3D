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

#ifndef __T3D_INTR_SHPERE_SPHERE_H__
#define __T3D_INTR_SHPERE_SPHERE_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"
#include "T3DSphere.h"


namespace Tiny3D
{
    TSTRUCT()
    template <typename T>
    struct TIntrSphereSphere
    {
        TIntrSphereSphere();
        TIntrSphereSphere(const TSphere<T> *s0, const TSphere<T> *s1);
        TIntrSphereSphere(const TSphere<T> &s0, const TSphere<T> &s1);

        TFUNCTION()
        bool test();

        TPROPERTY(RTTRFuncName="sphere0", RTTRFuncType="getter")
        const TSphere<T> *getSphere0() const
        {
            return mSphere0;
        }

        TPROPERTY(RTTRFuncName="sphere1", RTTRFuncType="getter")
        const TSphere<T> *getSphere1() const
        {
            return mSphere1;
        }

        TPROPERTY(RTTRFuncName="sphere0", RTTRFuncType="setter")
        void setSphere0(const TSphere<T> *sphere)
        {
            mSphere0 = sphere;
        }

        TPROPERTY(RTTRFuncName="sphere1", RTTRFuncType="setter")
        void setSphere1(const TSphere<T> *sphere)
        {
            mSphere1 = sphere;
        }

    private:
        const TSphere<T>    *mSphere0;
        const TSphere<T>    *mSphere1;
    };
}


#include "T3DIntrSphereSphere.inl"


#endif  /*__T3D_INTR_SHPERE_SPHERE_H__*/
