﻿/*******************************************************************************
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

#ifndef __T3D_VECTOR4_H__
#define __T3D_VECTOR4_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"


namespace Tiny3D
{
    /// 4D向量类
    class T3D_MATH_API Vector4
    {
    public:
        /// 默认构造函数
        Vector4();
        /// 通过 X, Y, Z, W 构造对象
        Vector4(Real x, Real y, Real z, Real w);
        /// 拷贝构造函数
        Vector4(const Vector4 &other);

        /// 返回成员变量的数组首地址.
        operator const Real *() const;
        operator Real *();

        /// 根据下标获取向量分量.
        Real operator [](int32_t i) const;
        Real &operator [](int32_t i);

        /// 获取x分量.
        Real x() const;
        Real &x();

        /// 获取y分量.
        Real y() const;
        Real &y();

        /// 获取z分量.
        Real z() const;
        Real &z();

        /// 获取w分量.
        Real w() const;
        Real &w();

        /// 重载赋值运算符
        Vector4 &operator =(const Vector4 &other);

        /// 重载相等运算符.
        bool operator ==(const Vector4 &other) const;
        /// 重载不等运算符.
        bool operator !=(const Vector4 &other) const;

        /// 重载加法运算符.
        Vector4 operator +(const Vector4 &other) const;
        /// 重载减法运算符.
        Vector4 operator -(const Vector4 &other) const;
        /// 重载乘法运算符，跟一个标量相乘.
        Vector4 operator *(Real scalar) const;
        /// 重载除法运算符，跟一个标量相除.
        Vector4 operator /(Real scalar) const;

        /// 重载取相反运算符.
        Vector4 operator -() const;

        /// 重载加法赋值运算符，跟另一个向量相加.
        Vector4 &operator +=(const Vector4 &other);
        /// 重载减法赋值运算符，跟另一个向量相减.
        Vector4 &operator -=(const Vector4 &other);
        /// 重载乘法赋值运算符，跟一个标量相乘.
        Vector4 &operator *=(Real scalar);
        /// 重载除法赋值运算符，跟一个标量相除.
        Vector4 &operator /=(Real scalar);

        /// 获取向量长度, sqrt(x*x + y*y + z*z + w*w).
        Real length() const;
        /// 获取向量长度的平方值, x*x + y*y + z*z + w*w.
        Real squaredLength() const;

        /// 获取两向量的距离.
        Real distance(const Vector4 &other) const;
        /// 获取两向量的距离平方.
        Real squaredDistance(const Vector4 &other) const;

        /// 点积、点乘、内积.
        Real dot(const Vector4 &other) const;

        /// 外积、叉乘.
        Vector4 cross(const Vector4 &other) const;

        /// 规范化.
        Real normalize();

        static const Vector4 ZERO;      /// 零向量

    private:
        Real    _x;
        Real    _y;
        Real    _z;
        Real    _w;
    };
}


#include "T3DVector4.inl"


#endif  /*__T3D_VECTOR4_H__*/
