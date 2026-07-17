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
    template <typename T>
    inline TCapsule<T>::TCapsule()
        : mPoint0(TReal<T>::ZERO, TReal<T>::ZERO, TReal<T>::ZERO)
        , mPoint1(TReal<T>::ZERO, TReal<T>::ZERO, TReal<T>::ZERO)
        , mRadius(TReal<T>::ZERO)
    {
    }

    template <typename T>
    inline TCapsule<T>::TCapsule(const TVector3<T> &p0, const TVector3<T> &p1,
        const T &radius)
        : mPoint0(p0)
        , mPoint1(p1)
        , mRadius(radius)
    {
    }

    template <typename T>
    inline TCapsule<T>::TCapsule(const TVector3<T> &center,
        const T &cylinderHalfHeight, const T &radius, const TVector3<T> &axis)
        : mPoint0(center - axis * cylinderHalfHeight)
        , mPoint1(center + axis * cylinderHalfHeight)
        , mRadius(radius)
    {
    }

    template <typename T>
    inline TCapsule<T>::TCapsule(const TCapsule &other)
        : mPoint0(other.mPoint0)
        , mPoint1(other.mPoint1)
        , mRadius(other.mRadius)
    {
    }

    template <typename T>
    inline TCapsule<T> &TCapsule<T>::operator =(const TCapsule &other)
    {
        mPoint0 = other.mPoint0;
        mPoint1 = other.mPoint1;
        mRadius = other.mRadius;
        return *this;
    }

    template <typename T>
    inline const TVector3<T> &TCapsule<T>::getPoint0() const
    {
        return mPoint0;
    }

    template <typename T>
    inline TVector3<T> &TCapsule<T>::getPoint0()
    {
        return mPoint0;
    }

    template <typename T>
    inline const TVector3<T> &TCapsule<T>::getPoint1() const
    {
        return mPoint1;
    }

    template <typename T>
    inline TVector3<T> &TCapsule<T>::getPoint1()
    {
        return mPoint1;
    }

    template <typename T>
    inline const T &TCapsule<T>::getRadius() const
    {
        return mRadius;
    }

    template <typename T>
    inline T &TCapsule<T>::getRadius()
    {
        return mRadius;
    }

    template <typename T>
    inline void TCapsule<T>::setPoint0(const TVector3<T> &p0)
    {
        mPoint0 = p0;
    }

    template <typename T>
    inline void TCapsule<T>::setPoint1(const TVector3<T> &p1)
    {
        mPoint1 = p1;
    }

    template <typename T>
    inline void TCapsule<T>::setRadius(const T &radius)
    {
        mRadius = radius;
    }

    template <typename T>
    inline TVector3<T> TCapsule<T>::getCenter() const
    {
        return TReal<T>::HALF * (mPoint0 + mPoint1);
    }

    template <typename T>
    inline TVector3<T> TCapsule<T>::getAxis() const
    {
        TVector3<T> axis = mPoint1 - mPoint0;
        T len = axis.normalize();
        if (len == TReal<T>::ZERO)
        {
            return TVector3<T>::UNIT_Y;
        }
        return axis;
    }

    template <typename T>
    inline T TCapsule<T>::getSegmentLength() const
    {
        return (mPoint1 - mPoint0).length();
    }

    template <typename T>
    inline T TCapsule<T>::getHeight() const
    {
        return getSegmentLength() + mRadius + mRadius;
    }

    template <typename T>
    inline bool TCapsule<T>::contains(const TVector3<T> &point) const
    {
        TVector3<T> d = mPoint1 - mPoint0;
        T denom = d.length2();
        T t = TReal<T>::ZERO;

        if (denom > TReal<T>::ZERO)
        {
            t = (point - mPoint0).dot(d) / denom;
            if (t < TReal<T>::ZERO)
            {
                t = TReal<T>::ZERO;
            }
            else if (t > TReal<T>::ONE)
            {
                t = TReal<T>::ONE;
            }
        }

        TVector3<T> closest = mPoint0 + d * t;
        return ((point - closest).length2() <= mRadius * mRadius);
    }

    template <typename T>
    inline String TCapsule<T>::getDebugString() const
    {
        std::stringstream ss;
        ss << "Point0 : " << mPoint0.getDebugString()
            << " Point1 : " << mPoint1.getDebugString()
            << " Radius : " << mRadius;
        return ss.str();
    }
}