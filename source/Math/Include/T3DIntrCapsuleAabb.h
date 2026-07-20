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

#ifndef __T3D_INTR_CAPSULE_AABB_H__
#define __T3D_INTR_CAPSULE_AABB_H__


#include "T3DMathPrerequisites.h"
#include "T3DCapsule.h"
#include "T3DAabb.h"


namespace Tiny3D
{
    TSTRUCT()
    template <typename T>
    struct TIntrCapsuleAabb
    {
        TIntrCapsuleAabb();
        TIntrCapsuleAabb(const TCapsule<T> *capsule, const TAabb<T> *box);
        TIntrCapsuleAabb(const TCapsule<T> &capsule, const TAabb<T> &box);

        TFUNCTION()
        bool test();

        TPROPERTY(RTTRFuncName="capsule", RTTRFuncType="getter")
        const TCapsule<T> *getCapsule() const
        {
            return mCapsule;
        }

        TPROPERTY(RTTRFuncName="box", RTTRFuncType="getter")
        const TAabb<T> *getBox() const
        {
            return mBox;
        }

        TPROPERTY(RTTRFuncName="capsule", RTTRFuncType="setter")
        void setCapsule(const TCapsule<T> *capsule)
        {
            mCapsule = capsule;
        }

        TPROPERTY(RTTRFuncName="box", RTTRFuncType="setter")
        void setBox(const TAabb<T> *box)
        {
            mBox = box;
        }

    private:
        const TCapsule<T>   *mCapsule;
        const TAabb<T>      *mBox;
    };
}


#include "T3DIntrCapsuleAabb.inl"


#endif  /*__T3D_INTR_CAPSULE_AABB_H__*/
