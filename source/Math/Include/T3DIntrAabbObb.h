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

#ifndef __T3D_INTR_AABB_OBB_H__
#define __T3D_INTR_AABB_OBB_H__


#include "T3DMathPrerequisites.h"
#include "T3DAabb.h"
#include "T3DObb.h"


namespace Tiny3D
{
    TSTRUCT()
    template <typename T>
    struct TIntrAabbObb
    {
        TIntrAabbObb();
        TIntrAabbObb(const TAabb<T> *aabb, const TObb<T> *obb);
        TIntrAabbObb(const TAabb<T> &aabb, const TObb<T> &obb);

        TFUNCTION()
        bool test();

        TPROPERTY(RTTRFuncName="aabb", RTTRFuncType="getter")
        const TAabb<T> *getAabb() const
        {
            return mAabb;
        }

        TPROPERTY(RTTRFuncName="obb", RTTRFuncType="getter")
        const TObb<T> *getObb() const
        {
            return mObb;
        }

        TPROPERTY(RTTRFuncName="aabb", RTTRFuncType="setter")
        void setAabb(const TAabb<T> *aabb)
        {
            mAabb = aabb;
        }

        TPROPERTY(RTTRFuncName="obb", RTTRFuncType="setter")
        void setObb(const TObb<T> *obb)
        {
            mObb = obb;
        }

    private:
        const TAabb<T>  *mAabb;
        const TObb<T>   *mObb;
    };
}


#include "T3DIntrAabbObb.inl"


#endif  /*__T3D_INTR_AABB_OBB_H__*/
