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


#include "T3DMatrix4.h"
#include "T3DIntrAabbPlane.h"


namespace Tiny3D
{
    template <typename T>
    inline TIntrObbPlane<T>::TIntrObbPlane()
        : mBox(nullptr)
        , mPlane(nullptr)
    {

    }

    template <typename T>
    inline TIntrObbPlane<T>::TIntrObbPlane(
        const TObb<T> &box,
        const TPlane<T> &plane)
        : mBox(&box)
        , mPlane(&plane)
    {

    }

    template <typename T>
    inline TIntrObbPlane<T>::TIntrObbPlane(
        const TObb<T> *box,
        const TPlane<T> *plane)
        : mBox(box)
        , mPlane(plane)
    {

    }

    template <typename T>
    int32_t TIntrObbPlane<T>::test()
    {
        if (mBox == nullptr || mPlane == nullptr)
            return -1;

        // 先把平面变换到OBB空间

        // 构造变换矩阵
        TMatrix3<T> m(mBox->getAxis(0), mBox->getAxis(1), mBox->getAxis(2), true);
        m = m.inverse();

        // 构造变换后平面
        TVector3<T> n = m * mPlane->getNormal();
        n.normalize();
        TPlane<T> plane(n, mPlane->getDistance());

        // 构造一个AABB
        T minX = -mBox->getExtent(0) * TReal<T>::HALF;
        T maxX = mBox->getExtent(0) * TReal<T>::HALF;
        T minY = -mBox->getExtent(1) * TReal<T>::HALF;
        T maxY = mBox->getExtent(1) * TReal<T>::HALF;
        T minZ = -mBox->getExtent(2) * TReal<T>::HALF;
        T maxZ = mBox->getExtent(2) * TReal<T>::HALF;

        TAabb<T> aabb(minX, maxX, minY, maxY, minZ, maxZ);

        // 构造一个平面和AABB检测器对象
        TIntrAabbPlane<T> intr(aabb, plane);

        return intr.test();
    }
}

