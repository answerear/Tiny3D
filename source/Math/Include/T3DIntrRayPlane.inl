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
    inline TIntrRayPlane<T>::TIntrRayPlane()
        : mRay(nullptr)
        , mPlane(nullptr)
    {

    }

    template <typename T>
    inline TIntrRayPlane<T>::TIntrRayPlane(
        const TRay<T> &ray, 
        const TPlane<T> &plane)
        : mRay(&ray)
        , mPlane(&plane)
    {

    }

    template <typename T>
    inline TIntrRayPlane<T>::TIntrRayPlane(
        const TRay<T> *ray,
        const TPlane<T> *plane)
        : mRay(ray)
        , mPlane(plane)
    {

    }

    template <typename T>
    bool TIntrRayPlane<T>::test()
    {
        if (mRay == nullptr || mPlane == nullptr)
            return false;

        T dot = mRay->getDirection().dot(mPlane->getNormal());

        if (dot >= TReal<T>::ZERO)
        {
            // 没有交点
            return false;
        }

        return true;
    }

    template <typename T>
    bool TIntrRayPlane<T>::test(TVector3<T> &intersection)
    {
        if (mRay == nullptr || mPlane == nullptr)
            return false;

        T dot = mRay->getDirection().dot(mPlane->getNormal());

        if (dot >= TReal<T>::ZERO)
        {
            // 没有交点
            return false;
        }

        // 求交点
        T t = (mPlane[3] - mRay->getOrigin().dot(mPlane->getNormal())) / dot;

        intersection = mRay->getOrigin() + t * mRay->getDirection();

        return true;
    }

    template <typename T>
    bool TIntrRayPlane<T>::test(T &distance)
    {
        if (mRay == nullptr || mPlane == nullptr)
            return false;

        T dot = mRay->getDirection().dot(mPlane->getNormal());

        if (dot >= TReal<T>::ZERO)
        {
            // 没有交点
            return false;
        }

        // 求交点
        T t = (mPlane[3] - mRay->getOrigin().dot(mPlane->getNormal())) / dot;

        TVector3<T> point = mRay->getOrigin() + t * mRay->getDirection();

        distance = (point - mRay->getOrigin()).length();

        return true;
    }
}

