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
    namespace IntrCapsuleDetail
    {
        template <typename T>
        inline T clamp01(T value)
        {
            if (value < TReal<T>::ZERO)
                return TReal<T>::ZERO;
            if (value > TReal<T>::ONE)
                return TReal<T>::ONE;
            return value;
        }

        /// Squared distance between segments [p1,q1] and [p2,q2].
        template <typename T>
        inline T sqrDistanceSegmentSegment(
            const TVector3<T> &p1, const TVector3<T> &q1,
            const TVector3<T> &p2, const TVector3<T> &q2)
        {
            TVector3<T> d1 = q1 - p1;
            TVector3<T> d2 = q2 - p2;
            TVector3<T> r = p1 - p2;
            T a = d1.dot(d1);
            T e = d2.dot(d2);
            T f = d2.dot(r);
            T s, t;

            if (a <= TReal<T>::EPSILON && e <= TReal<T>::EPSILON)
            {
                return r.dot(r);
            }

            if (a <= TReal<T>::EPSILON)
            {
                s = TReal<T>::ZERO;
                t = clamp01(f / e);
            }
            else
            {
                T c = d1.dot(r);
                if (e <= TReal<T>::EPSILON)
                {
                    t = TReal<T>::ZERO;
                    s = clamp01(-c / a);
                }
                else
                {
                    T b = d1.dot(d2);
                    T denom = a * e - b * b;
                    if (denom != TReal<T>::ZERO)
                    {
                        s = clamp01((b * f - c * e) / denom);
                    }
                    else
                    {
                        s = TReal<T>::ZERO;
                    }

                    t = (b * s + f) / e;
                    if (t < TReal<T>::ZERO)
                    {
                        t = TReal<T>::ZERO;
                        s = clamp01(-c / a);
                    }
                    else if (t > TReal<T>::ONE)
                    {
                        t = TReal<T>::ONE;
                        s = clamp01((b - c) / a);
                    }
                }
            }

            TVector3<T> c1 = p1 + d1 * s;
            TVector3<T> c2 = p2 + d2 * t;
            return (c1 - c2).length2();
        }

        template <typename T>
        inline T sqrDistancePointSegment(
            const TVector3<T> &point,
            const TVector3<T> &p0,
            const TVector3<T> &p1)
        {
            TVector3<T> d = p1 - p0;
            T denom = d.length2();
            T t = TReal<T>::ZERO;
            if (denom > TReal<T>::ZERO)
            {
                t = clamp01((point - p0).dot(d) / denom);
            }
            TVector3<T> closest = p0 + d * t;
            return (point - closest).length2();
        }

    }

    template <typename T>
    inline TIntrCapsuleCapsule<T>::TIntrCapsuleCapsule()
        : mCapsule0(nullptr)
        , mCapsule1(nullptr)
    {
    }

    template <typename T>
    inline TIntrCapsuleCapsule<T>::TIntrCapsuleCapsule(
        const TCapsule<T> &c0, const TCapsule<T> &c1)
        : mCapsule0(&c0)
        , mCapsule1(&c1)
    {
    }

    template <typename T>
    inline TIntrCapsuleCapsule<T>::TIntrCapsuleCapsule(
        const TCapsule<T> *c0, const TCapsule<T> *c1)
        : mCapsule0(c0)
        , mCapsule1(c1)
    {
    }

    template <typename T>
    bool TIntrCapsuleCapsule<T>::test()
    {
        if (mCapsule0 == nullptr || mCapsule1 == nullptr)
            return false;

        T r = mCapsule0->getRadius() + mCapsule1->getRadius();
        T r2 = r * r;
        T d2 = IntrCapsuleDetail::sqrDistanceSegmentSegment(
            mCapsule0->getPoint0(), mCapsule0->getPoint1(),
            mCapsule1->getPoint0(), mCapsule1->getPoint1());
        return d2 <= r2;
    }
}
