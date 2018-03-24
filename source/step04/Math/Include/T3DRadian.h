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

#ifndef __T3D_RADIAN_H__
#define __T3D_RADIAN_H__


#include "T3DMathPrerequisites.h"


namespace Tiny3D
{
    /// 弧度类，用于表示弧度
    class T3D_MATH_API Radian
    {
    public:
        /// 指定实数弧度制的构造函数.
        explicit Radian(Real fRadian = 0.0);
        /// 指定角度对象的构造函数
        Radian(const Degree &degree);
        /// 拷贝构造函数
        Radian(const Radian &other);

        /// 重载赋值操作符，从一个实数弧度赋值
        Radian &operator =(Real fRadian);
        /// 重载赋值操作符，从一个弧度对象赋值
        Radian &operator =(const Radian &other);
        /// 重载赋值操作符，从一个角度对象赋值
        Radian &operator =(const Degree &degree);

        /// 重载取正数操作符
        const Radian &operator +() const;
        /// 重载取反操作符
        Radian operator -() const;

        /// 重载相等操作符
        bool operator ==(const Radian &other) const;
        /// 重载不等操作符
        bool operator !=(const Radian &other) const;
        /// 重载小于操作符
        bool operator <(const Radian &other) const;
        /// 重载小于等于操作符
        bool operator <=(const Radian &other) const;
        /// 重载大于操作符
        bool operator >(const Radian &other) const;
        /// 重载大于等于操作符
        bool operator >=(const Radian &other) const;

        /// 重载加法操作符，跟另一个弧度对象相加
        Radian operator +(const Radian &other) const;
        /// 重载减法操作符，跟另一个弧度对象相减
        Radian operator -(const Radian &other) const;

        /// 重载加法操作符，跟另一个角度对象相加
        Radian operator +(const Degree &degree) const;
        /// 重载减法操作符，跟另一个角度对象相减
        Radian operator -(const Degree &degree) const;

        /// 重载加法赋值操作符，跟另一个弧度对象相加
        Radian &operator +=(const Radian &other);
        /// 重载减法赋值操作符，跟另一个弧度对象相减
        Radian &operator -=(const Radian &other);

        /// 重载加法赋值操作符，跟另一个角度对象相加
        Radian &operator +=(const Degree &degree);
        /// 重载减法赋值操作符，跟另一个角度对象相减
        Radian &operator -=(const Degree &degree);

        /// 重载乘法操作符，跟一个标量相乘
        Radian operator *(Real fScalar) const;
        /// 重载除法操作符，跟一个标量相除
        Radian operator /(Real fScalar) const;

        /// 重载乘法赋值操作符，跟一个标量相乘
        Radian &operator *=(Real fScalar);
        /// 重载除法赋值操作符，跟一个标量相除
        Radian &operator /=(Real fScalar);

        /// 获取真实的角度值，返回一个实数值
        Real valueDegrees() const;
        /// 获取真实的弧度制，返回一个实数值
        Real valueRadians() const;

    private:
        Real    m_fRadian;
    };

    #include "T3DRadian.inl"

    /// 重载标量和弧度乘法操作符
    inline Radian operator *(Real fScalar, const Radian &radians)
    {
        return Radian(fScalar * radians.valueRadians());
    }
}


#endif  /*__T3D_RADIAN_H__*/