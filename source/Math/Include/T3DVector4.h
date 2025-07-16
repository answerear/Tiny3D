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

#ifndef __T3D_VECTOR4_H__
#define __T3D_VECTOR4_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"
#include "T3DVector3.h"


namespace Tiny3D
{
    /// 4D向量类
    TSTRUCT()
    template <typename T>
    struct TVector4
    {
        TRTTI_FRIEND

    public:
        /// 默认构造函数
        TVector4();
        /// 通过 X, Y, Z, W 构造对象
        TVector4(T x, T y, T z, T w);
        /// 通过一个三维向量和w值构造对象
        TVector4(const TVector3<T> &v, T w);
        /// 拷贝构造函数
        TVector4(const TVector4 &other);

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

        /// 获取z分量.
        TPROPERTY(RTTRFuncName="z", RTTRFuncType="getter", "Description"="z component")
        T z() const;
        T &z();

        /// 获取w分量.
        TPROPERTY(RTTRFuncName="w", RTTRFuncType="getter", "Description"="w component")
        T w() const;
        T &w();

        /// 重载赋值运算符
        TVector4 &operator =(const TVector4 &other);

        /// 重载相等运算符.
        bool operator ==(const TVector4 &other) const;
        /// 重载不等运算符.
        bool operator !=(const TVector4 &other) const;

        /// 重载加法运算符.
        TVector4 operator +(const TVector4 &other) const;
        /// 重载减法运算符.
        TVector4 operator -(const TVector4 &other) const;
        /// 重载乘法运算符，跟一个标量相乘.
        TVector4 operator *(T scalar) const;
        /// 重载除法运算符，跟一个标量相除.
        TVector4 operator /(T scalar) const;

        /// 重载取相反运算符.
        TVector4 operator -() const;

        /// 重载加法赋值运算符，跟另一个向量相加.
        TVector4 &operator +=(const TVector4 &other);
        /// 重载减法赋值运算符，跟另一个向量相减.
        TVector4 &operator -=(const TVector4 &other);
        /// 重载乘法赋值运算符，跟一个标量相乘.
        TVector4 &operator *=(T scalar);
        /// 重载除法赋值运算符，跟一个标量相除.
        TVector4 &operator /=(T scalar);

        /// 获取向量长度, sqrt(x*x + y*y + z*z + w*w).
        TFUNCTION("Description"="The length of the vector.")
        T length() const;
        /// 获取向量长度的平方值, x*x + y*y + z*z + w*w.
        TFUNCTION("Description"="The squared length of vector.")
        T length2() const;

        /// 获取两向量的距离.
        TFUNCTION("Description"="Calculate the distance between two vector.")
        T distance(const TVector4 &other) const;
        /// 获取两向量的距离平方.
        TFUNCTION("Description"="Calculate the squared distance between two vector.")
        T distance2(const TVector4 &other) const;

        /// 点积、点乘、内积.
        TFUNCTION("Description"="Calculate dot product.")
        T dot(const TVector4 &other) const;

        /// 外积、叉乘.
        TFUNCTION("Description"="Calculate cross product.")
        TVector4 cross(const TVector4 &other) const;

        /// 规范化.
        TFUNCTION("Description"="Normalize the vector")
        T normalize();

        static const TVector4 ZERO;      /// 零向量

    private:
        TPROPERTY(RTTRFuncName="x", RTTRFuncType="setter", "Description"="x component")
        void setX(T x) { _x = x; }

        TPROPERTY(RTTRFuncName="y", RTTRFuncType="setter", "Description"="y component")
        void setY(T y) { _y = y; }

        TPROPERTY(RTTRFuncName="z", RTTRFuncType="setter", "Description"="z component")
        void setZ(T z) { _y = z; }

        TPROPERTY(RTTRFuncName="w", RTTRFuncType="setter", "Description"="w component")
        void setW(T w) { _w = w; }
        
        T    _x;
        T    _y;
        T    _z;
        T    _w;
    };
}


#include "T3DVector4.inl"


#endif  /*__T3D_VECTOR4_H__*/
