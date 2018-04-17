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

#ifndef __T3D_SPHERE_H__
#define __T3D_SPHERE_H__


#include "T3DMathPrerequisites.h"
#include "T3DVector3.h"


namespace Tiny3D
{
    /// 球类
    template <typename T>
    class TSphere
    {
        /// 默认构造函数
        TSphere()
            : mCenter(T(0), T(0))
            , mRadius(0)
        {
        }

        /// 初始化原点和半径的构造函数
        TSphere(const TVector3<T> &center, const T &radius)
            : mCenter(center)
            , mRadius(radius)
        {
        }

        /// 拷贝构造函数
        TSphere(const TSphere &sphere)
            : mCenter(sphere.mCenter)
            , mRadius(sphere.mRadius)
        {
        }

        /// 重载赋值运算符
        TSphere &operator =(const TSphere &sphere)
        {
            mCenter = sphere.mCenter;
            mRadius = sphere.mRadius;
            return *this;
        }
        
        /// 获取球心
        const TVector3<T> &getCenter() const
        {
            return mCenter;
        }

        TVector3<T> &getCenter()
        {
            return mCenter;
        }

        /// 获取半径
        const T &getRadius() const
        {
            return mRadius;
        }

        T &getRadius()
        {
            return mRadius;
        }

        /// 设置球心
        void setCenter(const TVector3<T> &center)
        {
            mCenter = center;
        }

        /// 设置半径
        void setRadius(const T &radius)
        {
            mRadius = radius;
        }

        /// 检测指定点是否在球内部
        bool contains(const TVector3<T> &point) const;

    private:
        TVector3<T> mCenter;    /// 球心
        T           mRadius;    /// 半径
    };
}

#include "T3DSphere.inl"

#endif    /*__T3D_SPHERE_H__*/
