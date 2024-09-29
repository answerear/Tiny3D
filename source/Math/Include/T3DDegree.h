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

#ifndef __T3D_DEGREE_H__
#define __T3D_DEGREE_H__


#include "T3DMathPrerequisites.h"


namespace Tiny3D
{
    // template <typename T> struct TRadian;
    
    /// 角度类，用于表示角度
    TSTRUCT()
    template <typename T>
    struct TDegree
    {
        TRTTI_FRIEND

    public:
        explicit TDegree();

        /// 指定实数角度值的构造函数
        explicit TDegree(T degree);
        /// 指定弧度对象的构造函数
        // TDegree(const TRadian<T> &radian);
        /// 拷贝构造函数
        TDegree(const TDegree &other);

        /// 重载赋值操作符，从一个实数角度赋值
        TDegree &operator =(T degree);
        /// 重载赋值操作符，从另外一个角度对象赋值
        TDegree &operator =(const TDegree &other);
        /// 重载赋值操作符，从一个弧度对象赋值
        // TDegree &operator =(const TRadian<T> &radian);

        /// 重载取正数操作符
        const TDegree &operator +() const;
        /// 重载取反操作符
        TDegree operator -() const;

        /// 重载相等操作符
        bool operator ==(const TDegree &other) const;
        /// 重载不等操作符
        bool operator !=(const TDegree &other) const;
        /// 重载小于操作符
        bool operator <(const TDegree &other) const;
        /// 重载小于等于操作符
        bool operator <=(const TDegree &other) const;
        /// 重载大于操作符
        bool operator >(const TDegree &other) const;
        /// 重载大于等于操作符
        bool operator >=(const TDegree &other) const;

        /// 重载加法操作符，跟另一个角度对象相加
        TDegree operator +(const TDegree &other) const;
        /// 重载减法操作符，跟另一个角度对象相减
        TDegree operator -(const TDegree &other) const;

        /// 重载加法操作符，跟另一个弧度对象相加
        // TDegree operator +(const TRadian<T> &radian) const;
        /// 重载减法操作符，跟另一个弧度对象相减
        // TDegree operator -(const TRadian<T> &radian) const;

        /// 重载加法赋值操作符，跟另一个角度对象相加
        TDegree &operator +=(const TDegree &other);
        /// 重载减法赋值操作符，跟另一个角度对象相减
        TDegree &operator -=(const TDegree &other);

        /// 重载加法赋值操作符，跟另一个弧度对象相加
        // TDegree &operator +=(const TRadian<T> &radian);
        /// 重载减法赋值操作符，跟另一个弧度对象相减
        // TDegree &operator -=(const TRadian<T> &radian);

        /// 重载乘法操作符，跟一个标量实数相乘
        TDegree operator *(T scalar) const;
        /// 重载除法操作符，跟一个标量实数相除
        TDegree operator /(T scalar) const;

        /// 重载乘法赋值操作符，跟一个标量实数相乘
        TDegree &operator *=(T scalar);
        /// 重载除法赋值操作符，跟一个标量实数相除
        TDegree &operator /=(T scalar);

        /// 获取真实的角度值，返回一个实数值
        TPROPERTY(RTTRFuncName="value", RTTRFuncType="getter", "Description"="A real value")
        T valueDegrees() const;
        
        /// 获取真实的弧度制，返回一个实数值
        TFUNCTION()
        T valueRadians() const;

    private:
        TPROPERTY(RTTRFuncName="value", RTTRFuncType="setter", "Description"="A real value")
        void setValue(T degree) { mDegree = degree; }
        
        T    mDegree;
    };

    #include "T3DDegree.inl"

    /// 重载标量和角度乘法操作符
    template <typename T>
    inline TDegree<T> operator *(T scalar, const TDegree<T> &degree)
    {
        return TDegree<T>(scalar * degree.valueDegrees());
    }
}


#endif  /*__T3D_DEGREE_H__*/
