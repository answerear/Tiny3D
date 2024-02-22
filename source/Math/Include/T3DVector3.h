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

#ifndef __T3D_VECTOR3_H__
#define __T3D_VECTOR3_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"
#include "T3DReal.h"


namespace Tiny3D
{
    /// 3D向量类
    TSTRUCT()
    template <typename T>
    struct TVector3
    {
        TRTTI_FRIEND

    public:
        /// 默认构造函数
        TVector3();
        /// 通过传入 X, Y, Z 构造对象
        TVector3(T x, T y, T z);
        /// 拷贝构造函数
        TVector3(const TVector3 &other);

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

        /// 重载赋值运算符.
        TVector3 &operator =(const TVector3 &other);

        /// 重载相等运算符.
        bool operator ==(const TVector3 &other) const;
        /// 重载不等运算符.
        bool operator !=(const TVector3 &other) const;
        /// 重载小于运算符.
        bool operator <(const TVector3 &other) const;
        /// 重载小于等于运算符.
        bool operator <=(const TVector3 &other) const;
        /// 重载大于运算符.
        bool operator >(const TVector3 &other) const;
        /// 重载大于等于运算符.
        bool operator >=(const TVector3 &other) const;

        /// 重载加法运算符.
        TVector3 operator +(const TVector3 &other) const;
        /// 重载减法运算符.
        TVector3 operator -(const TVector3 &other) const;
        /// 重载乘法运算符，跟一个标量相乘.
        TVector3 operator *(T scalar) const;
        /// 重载除法运算符，跟一个标量相除.
        TVector3 operator /(T scalar) const;
        /// 重载乘法运算符，分别跟另一个向量的对应分量相乘
        /// x1 = x1 * x2, y1 = y1 * y2, z1 = z1 * z2;
        TVector3 operator *(const TVector3 &other) const;

        /// 重载取相反运算符.
        TVector3 operator -() const;
        /// 重载加法赋值运算符，跟另一个向量相加.
        TVector3 &operator +=(const TVector3 &other);
        /// 重载减法赋值运算符，跟另一个向量相减.
        TVector3 &operator -=(const TVector3 &other);
        /// 重载乘法赋值运算符，跟一个标量相乘.
        TVector3 &operator *=(T fScalar);
        /// 重载乘法赋值运算符，跟另一个向量对应分量分别相乘
        TVector3 &operator *=(const TVector3 &rkV);
        /// 重载除法赋值运算符，跟一个标量相除.
        TVector3 &operator /=(T scalar);

        /// 获取向量长度, sqrt(x * x + y * y + z * z).
        TFUNCTION("Description"="The length of the vector.")
        T length() const;
        /// 获取向量长度的平方值, x * x + y * y + z * z.
        TFUNCTION("Description"="The squared length of vector.")
        T length2() const;

        /// 获取两向量的距离.
        TFUNCTION("Description"="Calculate the distance between two vector.")
        T distance(const TVector3 &other) const;
        /// 获取两向量的距离平方.
        TFUNCTION("Description"="Calculate the squared distance between two vector.")
        T distance2(const TVector3 &other) const;

        /// 点积、点乘、内积.
        TFUNCTION("Description"="Calculate dot product.")
        T dot(const TVector3 &other) const;

        /// 外积、叉乘.
        TFUNCTION("Description"="Calculate cross product.")
        TVector3 cross(const TVector3 &other) const;

        /// 规范化.
        TFUNCTION("Description"="Normalize the vector")
        T normalize();

        static const TVector3 ZERO;              /// 零向量
        static const TVector3 UNIT_SCALE;        /// 单位缩放向量
        static const TVector3 UNIT_X;            /// X轴单位向量
        static const TVector3 UNIT_Y;            /// Y轴单位向量
        static const TVector3 UNIT_Z;            /// Z轴单位向量
        static const TVector3 NEGATIVE_UNIT_X;   /// X轴负方向单位向量
        static const TVector3 NEGATIVE_UNIT_Y;   /// Y轴负方向单位向量
        static const TVector3 NEGATIVE_UNIT_Z;   /// Z轴负方向单位向量

    private:
        /// 比较大小
        int32_t compareArrays(const TVector3 &other) const;

        TPROPERTY(RTTRFuncName="x", RTTRFuncType="setter", "Description"="x component")
        void setX(T x) { _x = x; }

        TPROPERTY(RTTRFuncName="y", RTTRFuncType="setter", "Description"="y component")
        void setY(T y) { _y = y; }

        TPROPERTY(RTTRFuncName="z", RTTRFuncType="setter", "Description"="z component")
        void setZ(T z) { _y = z; }
        
    private:
        T    _x;
        T    _y;
        T    _z;
    };

    /// 重载乘法运算符，标量乘以向量
    template <typename T>
    TVector3<T> operator *(T scalar, const TVector3<T> &rkV);
}


#include "T3DVector3.inl"


#endif  /*__T3D_VECTOR3_H__*/
