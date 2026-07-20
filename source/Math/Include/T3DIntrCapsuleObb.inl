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


#include "T3DIntrCapsuleAabb.h"


namespace Tiny3D
{
    template <typename T>
    inline TIntrCapsuleObb<T>::TIntrCapsuleObb()
        : mCapsule(nullptr)
        , mBox(nullptr)
    {
    }

    template <typename T>
    inline TIntrCapsuleObb<T>::TIntrCapsuleObb(
        const TCapsule<T> &capsule, const TObb<T> &box)
        : mCapsule(&capsule)
        , mBox(&box)
    {
    }

    template <typename T>
    inline TIntrCapsuleObb<T>::TIntrCapsuleObb(
        const TCapsule<T> *capsule, const TObb<T> *box)
        : mCapsule(capsule)
        , mBox(box)
    {
    }

    template <typename T>
    bool TIntrCapsuleObb<T>::test()
    {
        if (mCapsule == nullptr || mBox == nullptr)
            return false;

        const TVector3<T> &center = mBox->getCenter();
        const TVector3<T> &axis0 = mBox->getAxis(0);
        const TVector3<T> &axis1 = mBox->getAxis(1);
        const TVector3<T> &axis2 = mBox->getAxis(2);
        T e0 = mBox->getExtent(0);
        T e1 = mBox->getExtent(1);
        T e2 = mBox->getExtent(2);

        auto toLocal = [&](const TVector3<T> &p) -> TVector3<T>
        {
            TVector3<T> d = p - center;
            return TVector3<T>(d.dot(axis0), d.dot(axis1), d.dot(axis2));
        };

        TCapsule<T> localCapsule(
            toLocal(mCapsule->getPoint0()),
            toLocal(mCapsule->getPoint1()),
            mCapsule->getRadius());

        TAabb<T> localAabb(-e0, e0, -e1, e1, -e2, e2);
        TIntrCapsuleAabb<T> intr(&localCapsule, &localAabb);
        return intr.test();
    }
}
