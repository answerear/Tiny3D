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

#ifndef __T3D_DEGREE_H__
#define __T3D_DEGREE_H__


#include "T3DMathPrerequisites.h"


namespace Tiny3D
{
    /// 角度类，用于表示角度
    class T3D_MATH_API Degree
    {
    public:
        /// 指定实数角度值的构造函数
        explicit Degree(Real fDegree = 0.0);
        /// 指定弧度对象的构造函数
        Degree(const Radian &radian);
        /// 拷贝构造函数
        Degree(const Degree &other);

        /// 重载赋值操作符，从一个实数角度赋值
        Degree &operator =(Real fDegree);
        /// 重载赋值操作符，从另外一个角度对象赋值
        Degree &operator =(const Degree &other);
        /// 重载赋值操作符，从一个弧度对象赋值
        Degree &operator =(const Radian &radian);

        /// 重载取正数操作符
        const Degree &operator +() const;
        /// 重载取反操作符
        Degree operator -() const;

        /// 重载相等操作符
        bool operator ==(const Degree &other) const;
        /// 重载不等操作符
        bool operator !=(const Degree &other) const;
        /// 重载小于操作符
        bool operator <(const Degree &other) const;
        /// 重载小于等于操作符
        bool operator <=(const Degree &other) const;
        /// 重载大于操作符
        bool operator >(const Degree &other) const;
        /// 重载大于等于操作符
        bool operator >=(const Degree &other) const;

        /// 重载加法操作符，跟另一个角度对象相加
        Degree operator +(const Degree &other) const;
        /// 重载减法操作符，跟另一个角度对象相减
        Degree operator -(const Degree &other) const;

        /// 重载加法操作符，跟另一个弧度对象相加
        Degree operator +(const Radian &radian) const;
        /// 重载减法操作符，跟另一个弧度对象相减
        Degree operator -(const Radian &radian) const;

        /// 重载加法赋值操作符，跟另一个角度对象相加
        Degree &operator +=(const Degree &other);
        /// 重载减法赋值操作符，跟另一个角度对象相减
        Degree &operator -=(const Degree &other);

        /// 重载加法赋值操作符，跟另一个弧度对象相加
        Degree &operator +=(const Radian &radian);
        /// 重载减法赋值操作符，跟另一个弧度对象相减
        Degree &operator -=(const Radian &radian);

        /// 重载乘法操作符，跟一个标量实数相乘
        Degree operator *(Real fScalar) const;
        /// 重载除法操作符，跟一个标量实数相除
        Degree operator /(Real fScalar) const;

        /// 重载乘法赋值操作符，跟一个标量实数相乘
        Degree &operator *=(Real fScalar);
        /// 重载除法赋值操作符，跟一个标量实数相除
        Degree &operator /=(Real fScalar);

        /// 获取真实的角度值，返回一个实数值
        Real valueDegrees() const;
        /// 获取真实的弧度制，返回一个实数值
        Real valueRadians() const;

    private:
        Real    m_fDegree;
    };

    #include "T3DDegree.inl"

    /// 重载标量和角度乘法操作符
    inline Degree operator *(Real fScalar, const Degree &degree)
    {
        return Degree(fScalar * degree.valueDegrees());
    }
}


#endif  /*__T3D_DEGREE_H__*/
