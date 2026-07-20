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


#include "T3DIntrCapsuleCapsule.h"


namespace Tiny3D
{
    namespace IntrCapsuleTriangleDetail
    {
        template <typename T>
        inline TVector3<T> closestPointOnTriangle(
            const TVector3<T> &p,
            const TVector3<T> &a,
            const TVector3<T> &b,
            const TVector3<T> &c)
        {
            TVector3<T> ab = b - a;
            TVector3<T> ac = c - a;
            TVector3<T> ap = p - a;

            T d1 = ab.dot(ap);
            T d2 = ac.dot(ap);
            if (d1 <= TReal<T>::ZERO && d2 <= TReal<T>::ZERO)
                return a;

            TVector3<T> bp = p - b;
            T d3 = ab.dot(bp);
            T d4 = ac.dot(bp);
            if (d3 >= TReal<T>::ZERO && d4 <= d3)
                return b;

            T vc = d1 * d4 - d3 * d2;
            if (vc <= TReal<T>::ZERO && d1 >= TReal<T>::ZERO && d3 <= TReal<T>::ZERO)
            {
                T v = d1 / (d1 - d3);
                return a + ab * v;
            }

            TVector3<T> cp = p - c;
            T d5 = ab.dot(cp);
            T d6 = ac.dot(cp);
            if (d6 >= TReal<T>::ZERO && d5 <= d6)
                return c;

            T vb = d5 * d2 - d1 * d6;
            if (vb <= TReal<T>::ZERO && d2 >= TReal<T>::ZERO && d6 <= TReal<T>::ZERO)
            {
                T w = d2 / (d2 - d6);
                return a + ac * w;
            }

            T va = d3 * d6 - d5 * d4;
            if (va <= TReal<T>::ZERO && (d4 - d3) >= TReal<T>::ZERO
                && (d5 - d6) >= TReal<T>::ZERO)
            {
                T w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
                return b + (c - b) * w;
            }

            T denom = TReal<T>::ONE / (va + vb + vc);
            T v = vb * denom;
            T w = vc * denom;
            return a + ab * v + ac * w;
        }

        template <typename T>
        inline T sqrDistanceSegmentTriangle(
            const TVector3<T> &p0, const TVector3<T> &p1,
            const TVector3<T> &a, const TVector3<T> &b, const TVector3<T> &c)
        {
            T d0 = (closestPointOnTriangle(p0, a, b, c) - p0).length2();
            T d1 = (closestPointOnTriangle(p1, a, b, c) - p1).length2();
            T best = (d0 < d1) ? d0 : d1;

            T e0 = IntrCapsuleDetail::sqrDistanceSegmentSegment(p0, p1, a, b);
            T e1 = IntrCapsuleDetail::sqrDistanceSegmentSegment(p0, p1, b, c);
            T e2 = IntrCapsuleDetail::sqrDistanceSegmentSegment(p0, p1, c, a);
            if (e0 < best) best = e0;
            if (e1 < best) best = e1;
            if (e2 < best) best = e2;

            TVector3<T> ab = b - a;
            TVector3<T> ac = c - a;
            TVector3<T> n = ab.cross(ac);
            T nLen2 = n.length2();
            if (nLen2 > TReal<T>::EPSILON)
            {
                TVector3<T> d = p1 - p0;
                T denom = n.dot(d);
                if (denom != TReal<T>::ZERO)
                {
                    T t = n.dot(a - p0) / denom;
                    if (t >= TReal<T>::ZERO && t <= TReal<T>::ONE)
                    {
                        TVector3<T> p = p0 + d * t;
                        TVector3<T> q = closestPointOnTriangle(p, a, b, c);
                        T dp = (q - p).length2();
                        if (dp < best)
                            best = dp;
                    }
                }
            }

            return best;
        }
    }

    template <typename T>
    inline TIntrCapsuleTriangle<T>::TIntrCapsuleTriangle()
        : mCapsule(nullptr)
        , mTriangle(nullptr)
    {
    }

    template <typename T>
    inline TIntrCapsuleTriangle<T>::TIntrCapsuleTriangle(
        const TCapsule<T> &capsule, const TTriangle<T> &triangle)
        : mCapsule(&capsule)
        , mTriangle(&triangle)
    {
    }

    template <typename T>
    inline TIntrCapsuleTriangle<T>::TIntrCapsuleTriangle(
        const TCapsule<T> *capsule, const TTriangle<T> *triangle)
        : mCapsule(capsule)
        , mTriangle(triangle)
    {
    }

    template <typename T>
    bool TIntrCapsuleTriangle<T>::test()
    {
        if (mCapsule == nullptr || mTriangle == nullptr)
            return false;

        const TVector3<T> &a = (*mTriangle)[0];
        const TVector3<T> &b = (*mTriangle)[1];
        const TVector3<T> &c = (*mTriangle)[2];

        T r = mCapsule->getRadius();
        T r2 = r * r;
        T d2 = IntrCapsuleTriangleDetail::sqrDistanceSegmentTriangle(
            mCapsule->getPoint0(), mCapsule->getPoint1(), a, b, c);
        return d2 <= r2;
    }
}
