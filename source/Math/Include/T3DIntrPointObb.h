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

#ifndef __T3D_INTR_POINT_OBB_H__
#define __T3D_INTR_POINT_OBB_H__


#include "T3DMathPrerequisites.h"
#include "T3DVector3.h"
#include "T3DObb.h"


namespace Tiny3D
{
    TSTRUCT()
    template <typename T>
    struct TIntrPointObb
    {
        TIntrPointObb();
        TIntrPointObb(const TVector3<T> *point, const TObb<T> *box);
        TIntrPointObb(const TVector3<T> &point, const TObb<T> &box);

        TFUNCTION()
        bool test();

        TPROPERTY(RTTRFuncName="point", RTTRFuncType="getter")
        const TVector3<T> *getPoint() const
        {
            return mPoint;
        }

        TPROPERTY(RTTRFuncName="obb", RTTRFuncType="getter")
        const TObb<T> *getBox() const
        {
            return mBox;
        }

        TPROPERTY(RTTRFuncName="point", RTTRFuncType="setter")
        void setPoint(const TVector3<T> *point)
        {
            mPoint = point;
        }

        TPROPERTY(RTTRFuncName="obb", RTTRFuncType="setter")
        void setBox(const TObb<T> *box)
        {
            mBox = box;
        }

    private:
        const TVector3<T>   *mPoint;
        const TObb<T>       *mBox;
    };
}


#include "T3DIntrPointObb.inl"


#endif  /*__T3D_INTR_POINT_OBB_H__*/
