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


namespace Tiny3D
{
    namespace IntrCapsuleAabbDetail
    {
        template <typename T>
        inline T sqrDistancePointAabb(
            const TVector3<T> &point, const TAabb<T> &box)
        {
            T dist2 = TReal<T>::ZERO;

            if (point.x() < box.getMinX())
            {
                T d = box.getMinX() - point.x();
                dist2 += d * d;
            }
            else if (point.x() > box.getMaxX())
            {
                T d = point.x() - box.getMaxX();
                dist2 += d * d;
            }

            if (point.y() < box.getMinY())
            {
                T d = box.getMinY() - point.y();
                dist2 += d * d;
            }
            else if (point.y() > box.getMaxY())
            {
                T d = point.y() - box.getMaxY();
                dist2 += d * d;
            }

            if (point.z() < box.getMinZ())
            {
                T d = box.getMinZ() - point.z();
                dist2 += d * d;
            }
            else if (point.z() > box.getMaxZ())
            {
                T d = point.z() - box.getMaxZ();
                dist2 += d * d;
            }

            return dist2;
        }

        template <typename T>
        inline T sqrDistanceSegmentAabb(
            const TVector3<T> &p0, const TVector3<T> &p1, const TAabb<T> &box)
        {
            TVector3<T> d = p1 - p0;
            T lo = TReal<T>::ZERO;
            T hi = TReal<T>::ONE;
            for (int32_t i = 0; i < 24; ++i)
            {
                T m1 = lo + (hi - lo) / T(3);
                T m2 = hi - (hi - lo) / T(3);
                T f1 = sqrDistancePointAabb(p0 + d * m1, box);
                T f2 = sqrDistancePointAabb(p0 + d * m2, box);
                if (f1 < f2)
                {
                    hi = m2;
                }
                else
                {
                    lo = m1;
                }
            }

            T t = (lo + hi) * TReal<T>::HALF;
            return sqrDistancePointAabb(p0 + d * t, box);
        }
    }

    template <typename T>
    inline TIntrCapsuleAabb<T>::TIntrCapsuleAabb()
        : mCapsule(nullptr)
        , mBox(nullptr)
    {
    }

    template <typename T>
    inline TIntrCapsuleAabb<T>::TIntrCapsuleAabb(
        const TCapsule<T> &capsule, const TAabb<T> &box)
        : mCapsule(&capsule)
        , mBox(&box)
    {
    }

    template <typename T>
    inline TIntrCapsuleAabb<T>::TIntrCapsuleAabb(
        const TCapsule<T> *capsule, const TAabb<T> *box)
        : mCapsule(capsule)
        , mBox(box)
    {
    }

    template <typename T>
    bool TIntrCapsuleAabb<T>::test()
    {
        if (mCapsule == nullptr || mBox == nullptr)
            return false;

        T r = mCapsule->getRadius();
        T r2 = r * r;
        T d2 = IntrCapsuleAabbDetail::sqrDistanceSegmentAabb(
            mCapsule->getPoint0(), mCapsule->getPoint1(), *mBox);
        return d2 <= r2;
    }
}
