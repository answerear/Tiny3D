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
    inline TIntrRaySphere<T>::TIntrRaySphere()
        : mRay(nullptr)
        , mSphere(nullptr)
    {

    }

    template <typename T>
    inline TIntrRaySphere<T>::TIntrRaySphere(
        const TRay<T> &ray, 
        const TSphere<T> &sphere)
        : mRay(&ray)
        , mSphere(&sphere)
    {

    }

    template <typename T>
    inline TIntrRaySphere<T>::TIntrRaySphere(
        const TRay<T> *ray, 
        const TSphere<T> *sphere)
        : mRay(ray)
        , mSphere(sphere)
    {

    }

    template <typename T>
    bool TIntrRaySphere<T>::test()
    {
        if (mRay == nullptr || mSphere == nullptr)
            return false;

        // 计算射线起点到球心的向量
        TVector3<T> OC = mSphere->getCenter() - mRay->getOrigin();

        // 计算oc射线方向上的投影
        TVector3<T> dir = mRay->getDirection();
        dir.normalize();
        T proj = OC.dot(dir);

        if (proj < TReal<T>::ZERO)
        {
            return false;
        }

        T OC2 = OC.dot(OC);

        // 计算出球心到射线的距离
        T d = OC2 - proj * proj;

        T radiusSqr = mSphere->getRadius() * mSphere->getRadius();

        if (d > radiusSqr)
        {
            // 球心到射线的距离大于半径，则射线跟球没有相交
            return false;
        }

        return true;
    }

    template <typename T>
    bool TIntrRaySphere<T>::test(TVector3<T> &intersection)
    {
        if (mRay == nullptr || mSphere == nullptr)
            return false;

        // 计算射线起点到球心的向量
        TVector3<T> OC = mSphere->getCenter() - mRay->getOrigin();

        // 计算oc射线方向上的投影
        TVector3<T> dir = mRay->getDirection();
        dir.normalize();
        T proj = OC.dot(dir);

        if (proj < TReal<T>::ZERO)
        {
            return false;
        }

        T OC2 = OC.dot(OC);

        // 计算出球心到射线的距离
        T d2 = OC2 - proj * proj;

        T radiusSqr = mSphere->getRadius() * mSphere->getRadius();

        if (d2 > radiusSqr)
        {
            // 球心到射线的距离大于半径，则射线跟球没有相交
            return false;
        }

        T t0, t1;

        if (mSphere->getRadius() - d2 < TReal<T>::EPSILON)
        {
            t0 = t1 = proj;
        }
        else
        {
            T d = TMath<T>::sqrt(d2);
            t0 = proj - d;
            t1 = proj + d;
        }

        intersection = mRay->getOrigin() + t0 * dir;

        return true;
    }

    template <typename T>
    bool TIntrRaySphere<T>::test(T &distance)
    {
        // 计算射线起点到球心的向量
        TVector3<T> OC = mSphere->getCenter() - mRay->getOrigin();

        // 计算oc射线方向上的投影
        TVector3<T> dir = mRay->getDirection();
        dir.normalize();
        T proj = OC.dot(dir);

        if (proj < TReal<T>::ZERO)
        {
            return false;
        }

        T OC2 = OC.dot(OC);

        // 计算出球心到射线的距离
        T d2 = OC2 - proj * proj;

        T radiusSqr = mSphere->getRadius() * mSphere->getRadius();

        if (d2 > radiusSqr)
        {
            // 球心到射线的距离大于半径，则射线跟球没有相交
            return false;
        }

        distance = TMath<T>::sqrt(d2);

        return true;
    }
}

