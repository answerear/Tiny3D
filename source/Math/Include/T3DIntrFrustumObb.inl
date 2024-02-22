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


#include "T3DIntrObbPlane.h"


namespace Tiny3D
{
    template <typename T>
    inline TIntrFrustumObb<T>::TIntrFrustumObb()
        : mFrustum(nullptr)
        , mBox(nullptr)
    {

    }

    template <typename T>
    inline TIntrFrustumObb<T>::TIntrFrustumObb(
        const TFrustum<T> &frustum,
        const TObb<T> &box)
        : mFrustum(&frustum)
        , mBox(&box)
    {

    }

    template <typename T>
    inline TIntrFrustumObb<T>::TIntrFrustumObb(
        const TFrustum<T> *frustum,
        const TObb<T> *box)
        : mFrustum(frustum)
        , mBox(box)
    {

    }

    template <typename T>
    inline bool TIntrFrustumObb<T>::test()
    {
        if (mFrustum == nullptr || mBox == nullptr)
            return false;

        TIntrObbPlane<T> intr;
        int32_t result;

        intr.setBox(mBox);

        intr.setPlane(&mFrustum->getFace(E_FACE_NEAR));
        result = intr.test();
        if (result < 0)
            return false;

        intr.setPlane(&mFrustum->getFace(E_FACE_FAR));
        result = intr.test();
        if (result < 0)
            return false;

        intr.setPlane(&mFrustum->getFace(E_FACE_LEFT));
        result = intr.test();
        if (result < 0)
            return false;

        intr.setPlane(&mFrustum->getFace(E_FACE_RIGHT));
        result = intr.test();
        if (result < 0)
            return false;

        intr.setPlane(&mFrustum->getFace(E_FACE_TOP));
        result = intr.test();
        if (result < 0)
            return false;

        intr.setPlane(&mFrustum->getFace(E_FACE_BOTTOM));
        result = intr.test();
        if (result < 0)
            return false;

        return true;
    }
}

