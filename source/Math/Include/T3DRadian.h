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

#ifndef __T3D_RADIAN_H__
#define __T3D_RADIAN_H__


#include "T3DMathPrerequisites.h"
#include "T3DReal.h"


namespace Tiny3D
{
    /// 弧度类，用于表示弧度
    TSTRUCT()
    template <typename T>
    struct TRadian
    {
        TRTTI_FRIEND

    public:
        /// 默认构造函数
        explicit TRadian();
        /// 指定实数弧度制的构造函数.
        explicit TRadian(T radian);
        /// 指定角度对象的构造函数
        TRadian(const TDegree<T> &degree);
        /// 拷贝构造函数
        TRadian(const TRadian &other);

        /// 重载赋值操作符，从一个实数弧度赋值
        TRadian &operator =(T radian);
        /// 重载赋值操作符，从一个弧度对象赋值
        TRadian &operator =(const TRadian &other);
        /// 重载赋值操作符，从一个角度对象赋值
        TRadian &operator =(const TDegree<T> &degree);

        /// 重载取正数操作符
        const TRadian &operator +() const;
        /// 重载取反操作符
        TRadian operator -() const;

        /// 重载相等操作符
        bool operator ==(const TRadian &other) const;
        /// 重载不等操作符
        bool operator !=(const TRadian &other) const;
        /// 重载小于操作符
        bool operator <(const TRadian &other) const;
        /// 重载小于等于操作符
        bool operator <=(const TRadian &other) const;
        /// 重载大于操作符
        bool operator >(const TRadian &other) const;
        /// 重载大于等于操作符
        bool operator >=(const TRadian &other) const;

        /// 重载加法操作符，跟另一个弧度对象相加
        TRadian operator +(const TRadian &other) const;
        /// 重载减法操作符，跟另一个弧度对象相减
        TRadian operator -(const TRadian &other) const;

        /// 重载加法操作符，跟另一个角度对象相加
        TRadian operator +(const TDegree<T> &degree) const;
        /// 重载减法操作符，跟另一个角度对象相减
        TRadian operator -(const TDegree<T> &degree) const;

        /// 重载加法赋值操作符，跟另一个弧度对象相加
        TRadian &operator +=(const TRadian &other);
        /// 重载减法赋值操作符，跟另一个弧度对象相减
        TRadian &operator -=(const TRadian &other);

        /// 重载加法赋值操作符，跟另一个角度对象相加
        TRadian &operator +=(const TDegree<T> &degree);
        /// 重载减法赋值操作符，跟另一个角度对象相减
        TRadian &operator -=(const TDegree<T> &degree);

        /// 重载乘法操作符，跟一个标量相乘
        TRadian operator *(T scalar) const;
        /// 重载除法操作符，跟一个标量相除
        TRadian operator /(T scalar) const;

        /// 重载乘法赋值操作符，跟一个标量相乘
        TRadian &operator *=(T scalar);
        /// 重载除法赋值操作符，跟一个标量相除
        TRadian &operator /=(T scalar);

        /// 获取真实的角度值，返回一个实数值
        TFUNCTION()
        T valueDegrees() const;
        
        /// 获取真实的弧度制，返回一个实数值
        TPROPERTY(RTTRFuncName="value", RTTRFuncType="getter", "Description"="A real value")
        T valueRadians() const;

    private:
        TPROPERTY(RTTRFuncName="value", RTTRFuncType="setter", "Description"="A real value")
        void setValue(T radian) { mRadian = radian; }
        
        T    mRadian;
    };

    #include "T3DRadian.inl"

    /// 重载标量和弧度乘法操作符
    template <typename T>
    inline TRadian<T> operator *(T scalar, const TRadian<T> &radians)
    {
        return TRadian<T>(scalar * radians.valueRadians());
    }
}


#endif  /*__T3D_RADIAN_H__*/