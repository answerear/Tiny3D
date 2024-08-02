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
    inline TPlane<T>::TPlane()
    {
        mCoeff[0] = TReal<T>::ZERO;
        mCoeff[1] = TReal<T>::ONE;
        mCoeff[2] = TReal<T>::ZERO;
        mCoeff[3] = TReal<T>::ZERO;
    }

    template <typename T>
    inline TPlane<T>::TPlane(const TVector3<T> &rkNormal, T fDistance)
    {
        mCoeff[0] = rkNormal[0];
        mCoeff[1] = rkNormal[1];
        mCoeff[2] = rkNormal[2];
        mCoeff[3] = fDistance;
    }

    template <typename T>
    inline TPlane<T>::TPlane(const TVector3<T> &rkNormal, const TVector3<T> &rkPos)
    {
        mCoeff[0] = rkNormal[0];
        mCoeff[1] = rkNormal[1];
        mCoeff[2] = rkNormal[2];
        mCoeff[3] = -rkNormal.dot(rkPos);
    }

    template <typename T>
    inline TPlane<T>::TPlane(const TVector3<T> &rkPos1, const TVector3<T> &rkPos2, const TVector3<T> &rkPos3)
    {
        TVector3<T> normal = (rkPos2 - rkPos1).cross(rkPos3 - rkPos2);
        normal.normalize();

        mCoeff[0] = normal[0];
        mCoeff[1] = normal[1];
        mCoeff[2] = normal[2];

        mCoeff[3] = -normal.dot(rkPos1);
    }

    template <typename T>
    inline TPlane<T>::TPlane(const TPlane<T> &rkOther)
    {
        mCoeff[0] = rkOther.mCoeff[0];
        mCoeff[1] = rkOther.mCoeff[1];
        mCoeff[2] = rkOther.mCoeff[2];
        mCoeff[3] = rkOther.mCoeff[3];
    }

    template <typename T>
    inline TPlane<T> &TPlane<T>::operator =(const TPlane<T> &rkOther)
    {
        mCoeff[0] = rkOther.mCoeff[0];
        mCoeff[1] = rkOther.mCoeff[1];
        mCoeff[2] = rkOther.mCoeff[2];
        mCoeff[3] = rkOther.mCoeff[3];
        return *this;
    }

    template <typename T>
    inline bool TPlane<T>::operator ==(const TPlane<T> &rkOther) const
    {
        return (mCoeff[0] == rkOther.mCoeff[0] 
            && mCoeff[1] == rkOther.mCoeff[1]
            && mCoeff[2] == rkOther.mCoeff[2] 
            && mCoeff[3] == rkOther.mCoeff[3]);
    }

    template <typename T>
    inline bool TPlane<T>::operator !=(const TPlane<T> &rkOther) const
    {
        return !operator ==(rkOther);
    }

    template <typename T>
    inline const T TPlane<T>::operator [](int32_t idx) const
    {
        T3D_ASSERT(idx >= 0 && idx < 4);
        return mCoeff[idx];
    }

    template <typename T>
    inline T &TPlane<T>::operator [](int32_t idx)
    {
        T3D_ASSERT(idx >= 0 && idx < 4);
        return mCoeff[idx];
    }

    template <typename T>
    inline T TPlane<T>::distanceToPoint(const TVector3<T> &rkPos) const
    {
        TVector3<T> normal(mCoeff[0], mCoeff[1], mCoeff[2]);
        return normal.dot(rkPos) + mCoeff[3];
    }

    template <typename T>
    inline T TPlane<T>::fastDistanceToPoint(const TVector3<T> &rkPos) const
    {
        TVector3<T> normal(mCoeff[0], mCoeff[1], mCoeff[2]);
        return normal.dot(rkPos) + mCoeff[3];
    }

    template <typename T>
    inline Side TPlane<T>::sideForPoint(
        const TVector3<T> &rkPos) const
    {
        T distance = distanceToPoint(rkPos);
        Side side = Side::kIntersect;

        if (distance < TReal<T>::ZERO)
            side = Side::kNegative;
        else if (distance > TReal<T>::ZERO)
            side = Side::kPositive;

        return side;
    }

    template <typename T>
    inline Side TPlane<T>::fastSideForPoint(
        const TVector3<T> &rkPos) const
    {
        T distance = fastDistanceToPoint(rkPos);
        Side side = Side::kIntersect;

        if (distance < TReal<T>::ZERO)
            side = Side::kNegative;
        else if (distance > TReal<T>::ZERO)
            side = Side::kPositive;

        return side;
    }

    template <typename T>
    inline TVector3<T> TPlane<T>::getNormal() const
    {
        return TVector3<T>(mCoeff[0], mCoeff[1], mCoeff[2]);
    }

    template <typename T>
    inline T TPlane<T>::getDistance() const
    {
        return mCoeff[3];
    }

    template <typename T>
    inline void TPlane<T>::normalize()
    {
        T length = TMath<T>::sqrt(
            mCoeff[0] * mCoeff[0] 
            + mCoeff[1] * mCoeff[1] 
            + mCoeff[2] * mCoeff[2]);
        mCoeff[0] /= length;
        mCoeff[1] /= length;
        mCoeff[2] /= length;
        mCoeff[3] /= length;
    }
}
