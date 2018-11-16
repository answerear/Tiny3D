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
    inline TIntrRayTriangle<T>::TIntrRayTriangle()
        : mRay(nullptr)
        , mTriangle(nullptr)
        , mResult()
    {

    }

    template <typename T>
    inline TIntrRayTriangle<T>::TIntrRayTriangle(
        const TRay<T> *ray, 
        const TTriangle<T> *triangle)
        : mRay(ray)
        , mTriangle(triangle)
        , mResult()
    {

    }

    template <typename T>
    inline TIntrRayTriangle<T>::TIntrRayTriangle(
        const TRay<T> &ray, 
        const TTriangle<T> &triangle)
        : mRay(&ray)
        , mTriangle(&triangle)
        , mResult()
    {

    }

    template <typename T>
    bool TIntrRayTriangle<T>::test()
    {
        if (mRay == nullptr || mTriangle == nullptr)
            return false;

        // 这里分两步检测
        // 1. 计算射线和包含该三角形的平面的交点
        // 2. 通过计算交点的重心坐标，来判断它是否在三角形中
        
        // 计算逆时针的变向量
        TVector3<T> e1 = mTriangle[1] - mTriangle[0];
        TVector3<T> e2 = mTriangle[2] - mTriangle[1];

        // 计算表面法向量
        TVector3<T> n = e1.cross(e2);

        // 计算倾斜角，表示靠近三角形 “正面” 的程度
        float dot = n.dot(mRay->getDirection());

        // 检查射线平行于三角形或未指向三角形正面的情况
        if (dot >= TReal<T>::ZERO)
        {
            // 射线平行于三角形或者射线跟三角形法线同向
            return false;
        }

        // 计算平面方程的 d 值，在右边使用带 d 的平面方程
        // Ax + By + Cz = d
        float d = n.dot(mTriangle[0]);

        // 计算和包含三角形的平面的参数交点
        float t = d - n.dot(mRay->getOrigin());

        if (t >= TReal<T>::ZERO)
        {
            // 射线起点在多边形的背面
            return false;
        }

        // t是射线起点到平面的距离，dot是射线长度在平面法线上的投影
        if (t < dot)
        {
            // 射线不够长......
            return false;
        }

        // 现在计算射线和平面的交点
        t /= dot;

        // 计算 3D 交点
        TVector3<T> p = mRay->getOrigin() + mRay->getDirection() * t;

        // 找到主要轴，选择投影平面，这里取巧，选择3个轴绝对值最大的轴抛弃掉
        T u0, u1, u2;
        T v0, v1, v2;

        if (Math<T>::abs(n.x()) > Math<T>::abs(n.y()))
        {
            if (Math<T>::abs(n.x()) > Math<T>::abs(n.z()))
            {
                u0 = p.y() - mTriangle[0].y();
                u1 = mTriangle[1].y() - mTriangle[0].y();
                u2 = mTriangle[2].y() - mTriangle[0].y();

                v0 = p.z() - mTriangle[0].z();
                v1 = mTriangle[1].z() - mTriangle[0].z();
                v2 = mTriangle[2].z() - mTriangle[0].z();
            }
            else
            {
                u0 = p.x() - mTriangle[0].x();
                u1 = mTriangle[1].x() - mTriangle[0].x();
                u2 = mTriangle[2].x() - mTriangle[0].x();

                v0 = p.y() - mTriangle[0].y();
                v1 = mTriangle[1].y() - mTriangle[0].y();
                v2 = mTriangle[2].y() - mTriangle[0].y();
            }
        }
        else
        {
            if (Math<T>::abs(n.y()) > Math<T>::abs(n.z))
            {
                u0 = p.x() - mTriangle[0].x();
                u1 = mTriangle[1].x() - mTriangle[0].x();
                u2 = mTriangle[2].x() - mTriangle[0].x();

                v0 = p.z() - mTriangle[0].z();
                v1 = mTriangle[1].z() - mTriangle[0].z();
                v2 = mTriangle[2].z() - mTriangle[0].z();
            }
            else
            {
                u0 = p.x() - mTriangle[0].x();
                u1 = mTriangle[1].x() - mTriangle[0].x();
                u2 = mTriangle[2].x() - mTriangle[0].x();

                v0 = p.y() - mTriangle[0].y();
                v1 = mTriangle[1].y() - mTriangle[0].y();
                v2 = mTriangle[2].y() - mTriangle[0].y();
            }
        }

        T temp = u1 * v2 - v1 * u2;
        if (temp == TReal<T>::ZERO)
        {
            return false;
        }

        temp = TReal<T>::ONE / temp;

        // 计算重心坐标，每一步都检查边界条件
        T alpha = (u0 * v2 - v0 * u2) * temp;
        if (alpha < TReal<T>::ZERO)
        {
            return false;
        }

        T beta = (u1 * v0 - v1 * u0) * temp;
        if (beta < TReal<T>::ZERO)
        {
            return false;
        }

        T gamma = TReal<T>::ONE - alpha - beta;
        if (gamma < TReal<T>::ZERO)
        {
            return false;
        }

        // 把交点保存起来
        mResult = p;

        return true;
    }
}

