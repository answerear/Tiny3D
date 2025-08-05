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

#ifndef __T3D_VECTOR_2_H__
#define __T3D_VECTOR_2_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"


namespace Tiny3D
{
    /// 2D向量类
    TSTRUCT()
    template <typename T>
    struct TVector2
    {
        TRTTI_FRIEND

    public:
        /// 默认构造函数
        TVector2();
        /// 通过x, y构造对象.
        TVector2(T x, T y);
        /// 拷贝构造
        TVector2(const TVector2 &other);

        /// 返回成员变量的数组首地址.
        operator const T *() const;
        operator T *();

        /// 根据下标获取向量分量.
        T operator [](int32_t i) const;
        T &operator [](int32_t i);

        /// 获取x分量.
        TPROPERTY(RTTRFuncName="x", RTTRFuncType="getter", "Description"="x component")
        T x() const;
        T &x();

        /// 获取y分量.
        TPROPERTY(RTTRFuncName="y", RTTRFuncType="getter", "Description"="y component")
        T y() const;
        T &y();

        /// 重载赋值运算符
        TVector2 &operator =(const TVector2 &other);

        /// 重载相等运算符.
        bool operator ==(const TVector2 &other) const;
        /// 重载不等运算符.
        bool operator !=(const TVector2 &other) const;
        /// 重载小于运算符.
        bool operator <(const TVector2 &other) const;
        /// 重载小于等于运算符.
        bool operator <=(const TVector2 &other) const;
        /// 重载大于运算符.
        bool operator >(const TVector2 &other) const;
        /// 重载大于等于运算符.
        bool operator >=(const TVector2 &other) const;

        /// 重载加法运算符.
        TVector2 operator +(const TVector2 &other) const;
        /// 重载减法运算符.
        TVector2 operator -(const TVector2 &other) const;
        /// 重载乘法运算符，跟一个标量相乘.
        TVector2 operator *(T scalar) const;
        /// 重载除法运算符，跟一个标量相除.
        TVector2 operator /(T scalar) const;
        /// 重载乘法运算符，分别跟另一个向量的对应分量相乘
        /// x1 = x1 * x2, y1 = y1 * y2
        TVector2 operator *(const TVector2 &other) const;

        /// 重载取相反运算符.
        TVector2 operator -() const;

        /// 重载加法赋值运算符，跟另一个向量相加.
        TVector2 &operator +=(const TVector2 &other);
        /// 重载减法赋值运算符，跟另一个向量相减.
        TVector2 &operator -=(const TVector2 &other);
        /// 重载乘法赋值运算符，跟一个标量相乘.
        TVector2 &operator *=(T scalar);
        /// 重载乘法赋值运算符，跟另一个向量对应分量分别相乘
        TVector2 &operator *=(const TVector2 &other);
        /// 重载除法赋值运算符，跟一个标量相除.
        TVector2 &operator /=(T fScalar);

        /// 获取向量长度, sqrt(x*x + y*y).
        TFUNCTION("Description"="The length of the vector.")
        T length() const;
        /// 获取向量长度的平方值, x*x + y*y.
        TFUNCTION("Description"="The squared length of vector.")
        T length2() const;

        /// 获取两向量的距离.
        TFUNCTION("Description"="Calculate the distance between two vector.")
        T distance(const TVector2 &other) const;
        /// 获取两向量的距离平方.
        TFUNCTION("Description"="Calculate the squared distance between two vector.")
        T distance2(const TVector2 &other) const;

        /// 点积、点乘、内积.
        TFUNCTION("Description"="Calculate dot product.")
        T dot(const TVector2 &other) const;

        /// 外积、叉乘.
        TFUNCTION("Description"="Calculate cross product.")
        T cross(const TVector2 &other) const;

        /// 规范化.
        TFUNCTION("Description"="Normalize the vector")
        T normalize();

        /// 计算垂直向量 (y,-x).
        TFUNCTION("Description"="Calculate the perpendicular vector (y,-x).")
        TVector2 perp() const;

        String getDebugString() const;

        static const TVector2 ZERO;              /// 零向量
        static const TVector2 UNIT_X;            /// X轴单位向量
        static const TVector2 UNIT_Y;            /// Y轴单位向量
        static const TVector2 NEGATIVE_UNIT_X;   /// X轴负方向单位向量
        static const TVector2 NEGATIVE_UNIT_Y;   /// Y轴负方向单位向量

    private:
        /// 比较大小
        int32_t compareArrays(const TVector2 &other) const;

        TPROPERTY(RTTRFuncName="x", RTTRFuncType="setter", "Description"="x component")
        void setX(T x) { _x = x; }

        TPROPERTY(RTTRFuncName="y", RTTRFuncType="setter", "Description"="y component")
        void setY(T y) { _y = y; }
        
    private:
        T _x;    /// x分量
        T _y;    /// y分量
    };
}


#include "T3DVector2.inl"


#endif  /*__T3D_VECTOR_2_H__*/
