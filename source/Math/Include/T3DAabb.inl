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
    inline TAabb<T>::TAabb()
        : mMinX(TReal<T>::ZERO)
        , mMaxX(TReal<T>::ZERO)
        , mMinY(TReal<T>::ZERO)
        , mMaxY(TReal<T>::ZERO)
        , mMinZ(TReal<T>::ZERO)
        , mMaxZ(TReal<T>::ZERO)
    {

    }

    template <typename T>
    inline TAabb<T>::TAabb(T fXMin, T fXMax, T fYMin, T fYMax, T fZMin, T fZMax)
        : mMinX(fXMin)
        , mMaxX(fXMax)
        , mMinY(fYMin)
        , mMaxY(fYMax)
        , mMinZ(fZMin)
        , mMaxZ(fZMax)
    {

    }

    template <typename T>
    void TAabb<T>::build(const TVector3<T> points[], size_t count)
    {
        mMaxX = TReal<T>::MINUS_INF;
        mMaxY = TReal<T>::MINUS_INF;
        mMaxZ = TReal<T>::MINUS_INF;
        mMinX = TReal<T>::INF;
        mMinY = TReal<T>::INF;
        mMinZ = TReal<T>::INF;

        size_t i = 0;
        for (i = 0; i < count; ++i)
        {
            if (points[i].x() > mMaxX)
                mMaxX = points[i].x();
            if (points[i].x() < mMinX)
                mMinX = points[i].x();
            if (points[i].y() > mMaxY)
                mMaxY = points[i].y();
            if (points[i].y() < mMinY)
                mMinY = points[i].y();
            if (points[i].z() > mMaxZ)
                mMaxZ = points[i].z();
            if (points[i].z() < mMinZ)
                mMinZ = points[i].z();
        }

        TVector3<T> center(
            (mMinX + mMaxX) * TReal<T>::HALF,
            (mMinY + mMaxY) * TReal<T>::HALF,
            (mMinZ + mMaxZ) * TReal<T>::HALF
            );
        mSphere.setCenter(center);
            
        TVector3<T> D(mMaxX - mMinX, mMaxY - mMinY, mMaxZ - mMinZ);
        mSphere.setRadius(D.length() * TReal<T>::HALF);
    }

    template <typename T>
    inline T TAabb<T>::getWidth() const
    {
        return mMaxX - mMinX;
    }

    template <typename T>
    inline T TAabb<T>::getHeight() const
    {
        return mMaxY - mMinY;
    }

    template <typename T>
    inline T TAabb<T>::getDepth() const
    {
        return mMaxZ - mMinZ;
    }

    template <typename T>
    inline T TAabb<T>::getMinX() const
    {
        return mMinX;
    }

    template <typename T>
    inline T TAabb<T>::getMaxX() const
    {
        return mMaxX;
    }

    template <typename T>
    inline T TAabb<T>::getMinY() const
    {
        return mMinY;
    }

    template <typename T>
    inline T TAabb<T>::getMaxY() const
    {
        return mMaxY;
    }

    template <typename T>
    inline T TAabb<T>::getMinZ() const
    {
        return mMinZ;
    }

    template <typename T>
    inline T TAabb<T>::getMaxZ() const
    {
        return mMaxZ;
    }

    template <typename T>
    inline const TVector3<T> &TAabb<T>::getCenter() const
    {
        return mSphere.getCenter();
    }

    template <typename T>
    inline T TAabb<T>::getRadius() const
    {
        return mSphere.getRadius();
    }

    template <typename T>
    inline const TSphere<T> &TAabb<T>::getSphere() const
    {
        return mSphere;
    }

    template <typename T>
    inline void TAabb<T>::setWidth(T fWidth)
    {
        T ex = fWidth * TReal<T>::HALF;
        mMinX = mSphere.getCenter().x() - ex;
        mMaxX = mSphere.getCenter().x() + ex;
    }

    template <typename T>
    inline void TAabb<T>::setHeight(T fHeight)
    {
        T ex = fHeight * TReal<T>::HALF;
        mMinY = mSphere.getCenter().y() - ex;
        mMaxY = mSphere.getCenter().y() + ex;
    }

    template <typename T>
    inline void TAabb<T>::setDepth(T fDepth)
    {
        T ex = fDepth * TReal<T>::HALF;
        mMinZ = mSphere.getCenter().z() - ex;
        mMaxZ = mSphere.getCenter().z() + ex;
    }

    template <typename T>
    inline void TAabb<T>::setMinX(T x)
    {
        mMinX = x;
        mSphere.getCenter().x() = (mMaxX + mMinX) * TReal<T>::HALF;
        T fWidth = mMaxX - mMinX;
        T fHeight = mMaxY - mMinY;
        T fDepth = mMaxZ - mMinZ;
        mSphere.getRadius() = TMath<T>::sqrt(fWidth * fWidth + fHeight * fHeight
            + fDepth * fDepth) * TReal<T>::HALF;
    }

    template <typename T>
    inline void TAabb<T>::setMaxX(T x)
    {
        mMaxX = x;
        mSphere.getCenter().x() = (mMaxX + mMinX) * TReal<T>::HALF;
        T fWidth = mMaxX - mMinX;
        T fHeight = mMaxY - mMinY;
        T fDepth = mMaxZ - mMinZ;
        mSphere.getRadius() = TMath<T>::sqrt(fWidth * fWidth + fHeight * fHeight
            + fDepth * fDepth) * TReal<T>::HALF;
    }

    template <typename T>
    inline void TAabb<T>::setMinY(T y)
    {
        mMinY = y;
        mSphere.getCenter().y() = (mMaxY + mMinY) * TReal<T>::HALF;
        T fWidth = mMaxX - mMinX;
        T fHeight = mMaxY - mMinY;
        T fDepth = mMaxZ - mMinZ;
        mSphere.getRadius() = TMath<T>::sqrt(fWidth * fWidth + fHeight * fHeight
            + fDepth * fDepth) * TReal<T>::HALF;
    }

    template <typename T>
    inline void TAabb<T>::setMaxY(T y)
    {
        mMaxY = y;
        mSphere.getCenter().y() = (mMaxY + mMinY) * TReal<T>::HALF;
        T fWidth = mMaxX - mMinX;
        T fHeight = mMaxY - mMinY;
        T fDepth = mMaxZ - mMinZ;
        mSphere.getRadius() = TMath<T>::sqrt(fWidth * fWidth + fHeight * fHeight
            + fDepth * fDepth) * TReal<T>::HALF;
    }

    template <typename T>
    inline void TAabb<T>::setMinZ(T z)
    {
        mMinZ = z;
        mSphere.getCenter().z() = (mMaxZ + mMinZ) * TReal<T>::HALF;
        T fWidth = mMaxX - mMinX;
        T fHeight = mMaxY - mMinY;
        T fDepth = mMaxZ - mMinZ;
        mSphere.getRadius() = TMath<T>::sqrt(fWidth * fWidth + fHeight * fHeight
            + fDepth * fDepth) * TReal<T>::HALF;
    }

    template <typename T>
    inline void TAabb<T>::setMaxZ(T z)
    {
        mMaxZ = z;
        mSphere.getCenter().z() = (mMaxZ + mMinZ) * TReal<T>::HALF;
        T fWidth = mMaxX - mMinX;
        T fHeight = mMaxY - mMinY;
        T fDepth = mMaxZ - mMinZ;
        mSphere.getRadius() = TMath<T>::sqrt(fWidth * fWidth + fHeight * fHeight
            + fDepth * fDepth) * TReal<T>::HALF;
    }

    template <typename T>
    inline void TAabb<T>::setParam(const TVector3<T> &vMin, const TVector3<T> &vMax)
    {
        mMinX = vMin.x();
        mMinY = vMin.y();
        mMinZ = vMin.z();
        mMaxX = vMax.x();
        mMaxY = vMax.y();
        mMaxZ = vMax.z();

        TVector3<T> temp = vMax - vMin;
        TVector3<T> center = temp * TReal<T>::HALF + vMin;
        T radius = temp.length();
        mSphere.setCenter(center);
        mSphere.setRadius(radius);
    }

    template<typename T>
    String TAabb<T>::getDebugString() const
    {
        std::stringstream ss;
        ss << "X: [" << mMinX << ", " << mMaxX << "] ";
        ss << "Y: [" << mMinY << ", " << mMaxY << "] ";
        ss << "Z: [" << mMinZ << ", " << mMaxZ << "]";
        return ss.str();
    }
}

