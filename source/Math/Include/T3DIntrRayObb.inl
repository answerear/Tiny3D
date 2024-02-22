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
#include "T3DIntrRayAabb.h"


namespace Tiny3D
{
    template <typename T>
    inline TIntrRayObb<T>::TIntrRayObb()
        : mRay(nullptr)
        , mObb(nullptr)
    {

    }

    template <typename T>
    inline TIntrRayObb<T>::TIntrRayObb(const TRay<T> *ray, const TObb<T> *obb)
        : mRay(ray)
        , mObb(obb)
    {

    }

    template <typename T>
    inline TIntrRayObb<T>::TIntrRayObb(const TRay<T> &ray, const TObb<T> &obb)
        : mRay(&ray)
        , mObb(&obb)
    {

    }

    template <typename T>
    bool TIntrRayObb<T>::test()
    {
        // 先把射线变换到OBB空间

        // 构造变换矩阵
        TMatrix3<T> m(mObb->getAxis(0), mObb->getAxis(1), mObb->getAxis(2), true);
        m = m.inverse();

        // 构造变换后射线
        TVector3<T> origin = m * mRay->getOrigin();
        TVector3<T> dir = m * mRay->getDirection();

        TRay<T> ray(origin, dir);

        // 构造一个AABB
        T minX = mObb->getCenter().x() - mObb->getExtent(0) * TReal<T>::HALF;
        T maxX = mObb->getCenter().x() + mObb->getExtent(0) * TReal<T>::HALF;
        T minY = mObb->getCenter().y() - mObb->getExtent(1) * TReal<T>::HALF;
        T maxY = mObb->getCenter().y() + mObb->getExtent(1) * TReal<T>::HALF;
        T minZ = mObb->getCenter().z() - mObb->getExtent(2) * TReal<T>::HALF;
        T maxZ = mObb->getCenter().z() + mObb->getExtent(2) * TReal<T>::HALF;

        TAabb<T> aabb(minX, maxX, minY, maxY, minZ, maxZ);

        // 构造一个射线和AABB检测器对象
        TIntrRayAabb<T> intr(ray, aabb);

        return intr.test();
    }

    template <typename T>
    bool TIntrRayObb<T>::test(TVector3<T> &intersection)
    {
        // 先把射线变换到OBB空间

        // 构造变换矩阵
        TMatrix3<T> m(mObb->getAxis(0), mObb->getAxis(1), mObb->getAxis(2), true);
        m = m.inverse();

        // 构造变换后射线
        T origin = m * mRay->getOrigin();
        T dir = m * mRay->getDirection();

        TRay<T> ray(origin, dir);

        // 构造一个AABB
        T minX = -mObb->getExtent(0) * TReal<T>::HALF;
        T maxX = mObb->getExtent(0) * TReal<T>::HALF;
        T minY = -mObb->getExtent(1) * TReal<T>::HALF;
        T maxY = mObb->getExtent(1) * TReal<T>::HALF;
        T minZ = -mObb->getExtent(2) * TReal<T>::HALF;
        T maxZ = mObb->getExtent(2) * TReal<T>::HALF;

        TAabb<T> aabb(minX, maxX, minY, maxY, minZ, maxZ);

        // 构造一个射线和AABB检测器对象
        TIntrRayAabb<T> intr(ray, aabb);

        return intr.test(intersection);
    }

    template <typename T>
    bool TIntrRayObb<T>::test(T &distance)
    {
        // 先把射线变换到OBB空间

        // 构造变换矩阵
        TMatrix3<T> m(mObb->getAxis(0), mObb->getAxis(1), mObb->getAxis(2), true);
        m = m.inverse();

        // 构造变换后射线
        T origin = m * mRay->getOrigin();
        T dir = m * mRay->getDirection();

        TRay<T> ray(origin, dir);

        // 构造一个AABB
        T minX = -mObb->getExtent(0) * TReal<T>::HALF;
        T maxX = mObb->getExtent(0) * TReal<T>::HALF;
        T minY = -mObb->getExtent(1) * TReal<T>::HALF;
        T maxY = mObb->getExtent(1) * TReal<T>::HALF;
        T minZ = -mObb->getExtent(2) * TReal<T>::HALF;
        T maxZ = mObb->getExtent(2) * TReal<T>::HALF;

        TAabb<T> aabb(minX, maxX, minY, maxY, minZ, maxZ);

        // 构造一个射线和AABB检测器对象
        TIntrRayAabb<T> intr(ray, aabb);

        return intr.test(distance);
    }
}

