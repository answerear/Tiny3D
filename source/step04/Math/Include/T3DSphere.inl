/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


namespace Tiny3D
{
    template <typename T>
    inline TSphere<T>::TSphere()
        : mCenter(TReal<T>::ZERO, TReal<T>::ZERO)
        , mRadius(0)
    {
    }

    template <typename T>
    inline TSphere<T>::TSphere(const TVector3<T> &p0)
    {
        mRadius = TReal<T>::ZERO;
        mCenter = p0;
    }

    template <typename T>
    inline TSphere<T>::TSphere(const TVector3<T> &p0, const TVector3<T> &p1)
    {
        TVector3<T> a = p1 - p0;
        TVector3<T> o = TReal<T>::HALF * a;
        mRadius = o.normalize();
        mCenter = p0 + o;
    }

    template <typename T>
    inline TSphere<T>::TSphere(const TVector3<T> &p0, const TVector3<T> &p1, 
        const TVector3<T> &p2)
    {
        TVector3<T> a = p1 - p0;
        TVector3<T> b = p2 - p0;

        TVector3<T> n = a.cross(b);
        T denominator = 2 * n.dot(n);

        TVector3<T> o = (b.dot(b) * (n.cross(a))
            + (a.dot(a) * (b.cross(n)))) / denominator;

        mRadius = o.normalize();
        mCenter = p0 + o;
    }
    
    template <typename T>
    inline TSphere<T>::TSphere(const TVector3<T> &p0, const TVector3<T> &p1, 
        const TVector3<T> &p2, const TVector3<T> &p3)
    {
        TVector3<T> a = p1 - p0;
        TVector3<T> b = p2 - p0;
        TVector3<T> c = p3 - p0;

        TMatrix3<T> M(
            a.x(), a.y(), a.z(),
            b.x(), b.y(), b.z(),
            c.x(), c.y(), c.z());

        T denominator = 2 * M.determinant();

        TVector3<T> n = a.cross(b);

        TVector3<T> o = (c.dot(c) * n + (b.dot(b) * c.cross(a))
            + a.dot(a) * b.cross(c)) / denominator;

        mRadius = o.normalize();
        mCenter = p0 + o;
    }

    template <typename T>
    inline TSphere<T>::TSphere(const TVector3<T> &center, const T &radius)
        : mCenter(center)
        , mRadius(radius)
    {
    }

    template <typename T>
    inline TSphere<T>::TSphere(const TSphere &sphere)
        : mCenter(sphere.mCenter)
        , mRadius(sphere.mRadius)
    {
    }

    template <typename T>
    inline TSphere<T> &TSphere<T>::operator =(const TSphere &other)
    {
        mCenter = sphere.mCenter;
        mRadius = sphere.mRadius;
        return *this;
    }

    template <typename T>
    inline void TSphere<T>::build(TVector3<T> points[], size_t count, 
        BuildOption option /* = E_BUILD_WELZL */)
    {
        switch (option)
        {
        case E_BUILD_WELZL:
            buildByWelzl(points);
            break;
        case E_BUILD_RITTER:
            buildByRitter(points);
            break;
        case E_BUILD_AVERAGE:
            buildByAverage(points);
            break;
        default:
            break;
        }
    }

    template <typename T>
    inline const TVector3<T> &TSphere<T>::getCenter() const
    {
        return mCenter;
    }

    template <typename T>
    inline TVector3<T> &TSphere<T>::getCenter()
    {
        return mCenter;
    }

    template <typename T>
    inline const T &TSphere<T>::getRadius() const
    {
        return mRadius;
    }

    template <typename T>
    inline T &TSphere<T>::getRadius()
    {
        return mRadius;
    }

    template <typename T>
    inline void TSphere<T>::setCenter(const TVector3<T> &center)
    {
        mCenter = center;
    }

    template <typename T>
    inline void TSphere<T>::setRadius(const T &radius)
    {
        mRadius = radius;
    }

    template <typename T>
    inline bool TSphere<T>::contains(const TVector3<T> &point) const
    {
        return ((point - mCenter).length() <= mRadius);
    }

    template <typename T>
    void TSphere<T>::buildByWelzl(TVector3<T> points[], size_t count)
    {
        TVector3<T> **ptr = new TVector3<T>*[count];

        size_t i = 0;
        for (i = 0; i < count; ++i)
        {
            ptr[i] = &points[i];
        }

        *this = recurseMinSphere(ptr, count);

        delete []ptr;
    }

    template <typename T>
    TSphere<T> TSphere<T>::recurseMinSphere(
        TVector3<T> *points[], 
        size_t count, 
        size_t b)
    {
        TSphere<T> sphere;

        switch (b)
        {
        case 0:
            sphere = Sphere();
            break;
        case 1:
            sphere = Sphere(*points[-1]);
            break;
        case 2:
            sphere = Sphere(*points[-1], *points[-2]);
            break;
        case 3:
            sphere = Sphere(*points[-1], *points[-2], *points[-3]);
            break;
        case 4:
            sphere = Sphere(*points[-1], *points[-2], *points[-3], *points[-4]);
            return sphere;
            break;
        }

        size_t i = 0;
        for (i = 0; i < count; ++i)
        {
            T d = sphere.mCenter.distance2(*points[i]);
            if (d > TReal<T>::ZERO)
            {
                size_t j = 0;
                for (j = i; j > 0; --j)
                {
                    TVector3<T> *T = points[j];
                    points[j] = points[j - 1];
                    points[j - 1] = T;
                }

                sphere = recurseMinSphere(points + 1, i, b + 1);
            }
        }

        return sphere;
    }

    template <typename T>
    void TSphere<T>::buildByRitter(TVector3<T> points[], size_t count)
    {
        // 先找出在x，y，z三个轴方向上距离最大的点作为半径
        T maxX = TReal<T>::ZERO;
        T maxY = TReal<T>::ZERO;
        T maxZ = TReal<T>::ZERO;
        T minX = TReal<T>::MINUS_ONE;
        T minY = TReal<T>::MINUS_ONE;
        T minZ = TReal<T>::MINUS_ONE;
        size_t x0, y0, z0, x1, y1, z1;

        size_t i = 0;
        for (i = 0; i < count; ++i)
        {
            if (points[i].x > maxX)
            {
                maxX = points[i].x();
                x1 = i;
            }
                
            if (points[i].x < minX)
            {
                minX = points[i].x();
                x0 = i;
            }
                
            if (points[i].y > maxY)
            {
                maxY = points[i].y();
                y1 = i;
            }
                
            if (points[i].y < minY)
            {
                minY = points[i].y();
                y0 = i;
            }
                
            if (points[i].z > maxZ)
            {
                maxZ = points[i].z();
                z1 = i;
            }
                
            if (points[i].z < minZ)
            {
                minZ = points[i].z();
                z0 = i;
            }
        }

        T dx = maxX - minX;
        T dy = maxY - minY;
        T dz = maxZ - minZ;
        size_t max = x1, min = x0;
        T d;

        if (dz > dx && dz > dy)
        {
            max = z1;
            min = z0;
            d = dz;
        }
        else if (dy > dx && dy > dz)
        {
            max = y1;
            min = y0;
            d = dy;
        }

        TVector3<T> center(
            (points[max].x + points[min].x) * TReal<T>::HALF,
            (points[max].y + points[min].y) * TReal<T>::HALF,
            (points[max].z + points[min].z) * TReal<T>::HALF);

        T radius = TMath<T>::sqrt(d);

        // 遍历所有顶点来修正，以得到一个逼近的包围球
        for (i = 0; i < count; ++i)
        {
            T d2 = points[i].distance2(center);
            if (d2 > radius * radius)
            {
                d = TMath<T>::sqrt(d2);
                T newRadius = (d + radius) * TReal<T>::HALF;
                T l = (d - radius) * TReal<T>::HALF;
                radius = newRadius;
                center = center + (points[i] - center) * l;
            }
        }

        mCenter = center;
        mRadius = radius;
    }

    template <typename T>
    void TSphere<T>::buildByAverage(TVector3<T> points[], size_t count)
    {
        TVector3<T> center;
        T radius = TReal<T>::MINUS_ONE;

        if (count > 0)
        {
            center = TReal<T>::ZERO;
            size_t i = 0;
            for (i = 0; i < count; ++i)
                center += points[i];

            center /= T(count);

            for (i = 0; i < count; ++i)
            {
                T d2 = points[i].distance2(center);

                if (d2 > radius)
                    radius = d2;
            }

            radius = TMath<T>::sqrt(radius);
        }

        mCenter = center;
        mRadius = radius;
    }
}