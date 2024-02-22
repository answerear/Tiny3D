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

#ifndef __T3D_INTR_OBB_OBB_H__
#define __T3D_INTR_OBB_OBB_H__


#include "T3DMathPrerequisites.h"
#include "T3DObb.h"


namespace Tiny3D
{
    TSTRUCT()
    template <typename T>
    struct TIntrObbObb
    {
        TIntrObbObb();
        TIntrObbObb(const TObb<T> *box0, const TObb<T> *box1);
        TIntrObbObb(const TObb<T> &box0, const TObb<T> &box1);

        TFUNCTION()
        bool test();

        TPROPERTY(RTTRFuncName="obb0", RTTRFuncType="getter")
        const TObb<T> *getBox0() const
        {
            return mBox0;
        }

        TPROPERTY(RTTRFuncName="obb1", RTTRFuncType="getter")
        const TObb<T> *getBox1() const
        {
            return mBox1;
        }

        TPROPERTY(RTTRFuncName="obb0", RTTRFuncType="setter")
        void setBox0(const TObb<T> *box)
        {
            mBox0 = box;
        }

        TPROPERTY(RTTRFuncName="obb1", RTTRFuncType="setter")
        void setBox1(const TObb<T> *box)
        {
            mBox1 = box;
        }

    private:
        const TObb<T>   *mBox0;
        const TObb<T>   *mBox1;
    };
}


#include "T3DIntrObbObb.inl"


#endif  /*__T3D_INTR_OBB_OBB_H__*/
