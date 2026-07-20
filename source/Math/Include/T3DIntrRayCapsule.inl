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


#include "T3DMath.h"
#include "T3DIntrCapsuleCapsule.h"


namespace Tiny3D
{
    namespace IntrRayCapsuleDetail
    {
        template <typename T>
        inline bool closestRaySegment(
            const TVector3<T> &origin,
            const TVector3<T> &dir,
            const TVector3<T> &p0,
            const TVector3<T> &p1,
            T &rayT,
            T &segT,
            T &dist2)
        {
            TVector3<T> d = p1 - p0;
            TVector3<T> r = origin - p0;
            T a = dir.dot(dir);
            T b = dir.dot(d);
            T c = d.dot(d);
            T e = dir.dot(r);
            T f = d.dot(r);
            T denom = a * c - b * b;

            if (denom > TReal<T>::EPSILON)
            {
                rayT = (b * f - c * e) / denom;
                segT = (a * f - b * e) / denom;
            }
            else
            {
                rayT = TReal<T>::ZERO;
                segT = (c > TReal<T>::EPSILON) ? (f / c) : TReal<T>::ZERO;
            }

            if (rayT < TReal<T>::ZERO)
                rayT = TReal<T>::ZERO;

            segT = IntrCapsuleDetail::clamp01(segT);

            TVector3<T> closestRay = origin + dir * rayT;
            TVector3<T> closestSeg = p0 + d * segT;
            dist2 = (closestRay - closestSeg).length2();
            return true;
        }
    }

    template <typename T>
    inline TIntrRayCapsule<T>::TIntrRayCapsule()
        : mRay(nullptr)
        , mCapsule(nullptr)
    {
    }

    template <typename T>
    inline TIntrRayCapsule<T>::TIntrRayCapsule(
        const TRay<T> &ray, const TCapsule<T> &capsule)
        : mRay(&ray)
        , mCapsule(&capsule)
    {
    }

    template <typename T>
    inline TIntrRayCapsule<T>::TIntrRayCapsule(
        const TRay<T> *ray, const TCapsule<T> *capsule)
        : mRay(ray)
        , mCapsule(capsule)
    {
    }

    template <typename T>
    bool TIntrRayCapsule<T>::test()
    {
        if (mRay == nullptr || mCapsule == nullptr)
            return false;

        TVector3<T> dir = mRay->getDirection();
        dir.normalize();

        T rayT = TReal<T>::ZERO;
        T segT = TReal<T>::ZERO;
        T dist2 = TReal<T>::ZERO;
        IntrRayCapsuleDetail::closestRaySegment(
            mRay->getOrigin(), dir,
            mCapsule->getPoint0(), mCapsule->getPoint1(),
            rayT, segT, dist2);

        T r = mCapsule->getRadius();
        return dist2 <= r * r;
    }

    template <typename T>
    bool TIntrRayCapsule<T>::test(TVector3<T> &intersection)
    {
        if (mRay == nullptr || mCapsule == nullptr)
            return false;

        TVector3<T> dir = mRay->getDirection();
        dir.normalize();

        T rayT = TReal<T>::ZERO;
        T segT = TReal<T>::ZERO;
        T dist2 = TReal<T>::ZERO;
        IntrRayCapsuleDetail::closestRaySegment(
            mRay->getOrigin(), dir,
            mCapsule->getPoint0(), mCapsule->getPoint1(),
            rayT, segT, dist2);

        T r = mCapsule->getRadius();
        T r2 = r * r;
        if (dist2 > r2)
            return false;

        // Approximate entry point along the ray toward the capsule axis.
        T offset = TReal<T>::ZERO;
        if (r2 > dist2)
        {
            offset = TMath<T>::sqrt(r2 - dist2);
        }
        T tHit = rayT - offset;
        if (tHit < TReal<T>::ZERO)
            tHit = TReal<T>::ZERO;

        intersection = mRay->getOrigin() + dir * tHit;
        return true;
    }

    template <typename T>
    bool TIntrRayCapsule<T>::test(T &distance)
    {
        TVector3<T> intersection;
        if (!test(intersection))
            return false;

        distance = (intersection - mRay->getOrigin()).length();
        return true;
    }
}
