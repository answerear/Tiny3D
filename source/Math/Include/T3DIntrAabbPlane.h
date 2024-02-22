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

#ifndef __T3D_INTR_AABB_PLANE_H__
#define __T3D_INTR_AABB_PLANE_H__


#include "T3DMathPrerequisites.h"
#include "T3DAabb.h"
#include "T3DPlane.h"


namespace Tiny3D
{
    TSTRUCT()
    template <typename T>
    struct TIntrAabbPlane
    {
        TIntrAabbPlane();
        TIntrAabbPlane(const TAabb<T> *box, const TPlane<T> *plane);
        TIntrAabbPlane(const TAabb<T> &box, const TPlane<T> &plane);

        TFUNCTION()
        int32_t test();

        TPROPERTY(RTTRFuncName="aabb", RTTRFuncType="getter")
        const TAabb<T> *getBox() const
        {
            return mBox;
        }

        TPROPERTY(RTTRFuncName="plane", RTTRFuncType="getter")
        const TAabb<T> *getPlane() const
        {
            return mBox;
        }

        TPROPERTY(RTTRFuncName="aabb", RTTRFuncType="setter")
        void setBox(const TAabb<T> *box)
        {
            mBox = box;
        }

        TPROPERTY(RTTRFuncName="plane", RTTRFuncType="setter")
        void setPlane(const TPlane<T> *plane)
        {
            mPlane = plane;
        }

    private:
        const TAabb<T>  *mBox;
        const TPlane<T> *mPlane;
    };
}


#include "T3DIntrAabbPlane.inl"


#endif  /*__T3D_INTR_AABB_PLANE_H__*/
