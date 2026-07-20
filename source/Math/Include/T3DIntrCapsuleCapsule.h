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

#ifndef __T3D_INTR_CAPSULE_CAPSULE_H__
#define __T3D_INTR_CAPSULE_CAPSULE_H__


#include "T3DMathPrerequisites.h"
#include "T3DCapsule.h"


namespace Tiny3D
{
    TSTRUCT()
    template <typename T>
    struct TIntrCapsuleCapsule
    {
        TIntrCapsuleCapsule();
        TIntrCapsuleCapsule(const TCapsule<T> *c0, const TCapsule<T> *c1);
        TIntrCapsuleCapsule(const TCapsule<T> &c0, const TCapsule<T> &c1);

        TFUNCTION()
        bool test();

        TPROPERTY(RTTRFuncName="capsule0", RTTRFuncType="getter")
        const TCapsule<T> *getCapsule0() const
        {
            return mCapsule0;
        }

        TPROPERTY(RTTRFuncName="capsule1", RTTRFuncType="getter")
        const TCapsule<T> *getCapsule1() const
        {
            return mCapsule1;
        }

        TPROPERTY(RTTRFuncName="capsule0", RTTRFuncType="setter")
        void setCapsule0(const TCapsule<T> *capsule)
        {
            mCapsule0 = capsule;
        }

        TPROPERTY(RTTRFuncName="capsule1", RTTRFuncType="setter")
        void setCapsule1(const TCapsule<T> *capsule)
        {
            mCapsule1 = capsule;
        }

    private:
        const TCapsule<T>   *mCapsule0;
        const TCapsule<T>   *mCapsule1;
    };
}


#include "T3DIntrCapsuleCapsule.inl"


#endif  /*__T3D_INTR_CAPSULE_CAPSULE_H__*/
