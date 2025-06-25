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

#ifndef __T3D_MATRIX4_H__
#define __T3D_MATRIX4_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"
#include "T3DMatrix3.h"
#include "T3DQuaternion.h"
#include "T3DVector4.h"


namespace Tiny3D
{
    TSTRUCT()
    template <typename T>
    struct TMatrix4
    {
        TRTTI_FRIEND

    public:
        /// 构造一个零矩阵
        TMatrix4();
        /// 默认构造函数.
        TMatrix4(bool isZero);
        /// 拷贝构造函数.
        TMatrix4(const TMatrix4& other);
        /// 指定各行各列具体数值构造对象.
        TMatrix4(T m00, T m01, T m02, T m03,
            T m10, T m11, T m12, T m13,
            T m20, T m21, T m22, T m23,
            T m30, T m31, T m32, T m33);
        /// 通过一个旋转矩阵或者缩放矩阵构造对象，其中平移部分为0.
        TMatrix4(const TMatrix3<T>& rkMat);
        /// 通过一个旋转或者缩放计算的四元数构造对象，其中平移部分为0.
        TMatrix4(const TQuaternion<T>& rkRot);
        /// 通过位移向量构造对象.
        TMatrix4(const TVector3<T>& rkPos);
        /// 构造对角矩阵.
        TMatrix4(T m00, T m11, T m22, T m33);
        /// 通过三正交向量构造对象
        TMatrix4(const TVector3<T>& X, const TVector3<T>& Y, const TVector3<T>& Z);

        /// 构造零矩阵.
        TFUNCTION()
        void makeZero();

        /// 构造单位矩阵.
        TFUNCTION()
        void makeIdentity();

        TFUNCTION()
        void make(T m00, T m01, T m02, T m03,
            T m10, T m11, T m12, T m13,
            T m20, T m21, T m22, T m23,
            T m30, T m31, T m32, T m33);

        /// 返回成员变量的数组首地址.
        //operator const T *() const;
        //operator T *();

        /// 根据获取矩阵对应行的元素值.
        const T* operator [](size_t row) const;
        T* operator [](size_t row);

        /// 通过指定行和列获取对应的元素.
        T operator ()(size_t row, size_t col) const;
        T& operator ()(size_t row, size_t col);

        /// 重载赋值运算符，相同类型对象赋值.
        TMatrix4& operator =(const TMatrix4& other);
        /// 重载赋值运算符，从一个3x3方针赋值.
        TMatrix4& operator =(const TMatrix3<T>& rkMat);

        /// 重载相等运算符.
        bool operator ==(const TMatrix4& other) const;
        /// 重载不等运算符.
        bool operator !=(const TMatrix4& other) const;
        /// 重载小于运算符.
        bool operator <(const TMatrix4& other) const;
        /// 重载小于等于运算符.
        bool operator <=(const TMatrix4& other) const;
        /// 重载大于运算符.
        bool operator >(const TMatrix4& other) const;
        /// 重载大于等于运算符.
        bool operator >=(const TMatrix4& other) const;

        /// 重载加法运算符.
        TMatrix4 operator +(const TMatrix4& other) const;
        /// 重载减法运算符.
        TMatrix4 operator -(const TMatrix4& other) const;

        /// 重载乘法运算符，实现两个矩阵相乘.
        TMatrix4 operator *(const TMatrix4& other) const;

        /// 重载乘法运算符，实现矩阵跟一个标量相乘 M1 = M * scalar.
        TMatrix4 operator *(T scalar) const;
        /// 重载除法运算符，实现矩阵跟一个标量相除 M1 = M / scalar.
        TMatrix4 operator /(T scalar) const;

        /// 重载取反运算符.
        TMatrix4 operator -() const;

        /// 重载加法赋值运算符.
        TMatrix4& operator +=(const TMatrix4& other);
        /// 重载减法赋值运算符.
        TMatrix4& operator -=(const TMatrix4& other);
        /// 重载乘法赋值运算符，实现矩阵跟一个标量相乘 M = M * scalar.
        TMatrix4& operator *=(T scalar);
        /// 重载除法赋值运算符，实现矩阵跟一个标量相除 M = M / scalar.
        TMatrix4& operator /=(T scalar);

        /// 重载乘法运算符，实现矩阵左乘向量 M = M * V.
        TVector4<T> operator *(const TVector4<T>& rkV) const;
        TVector3<T> operator *(const TVector3<T>& rkV) const;

        /// 矩阵转置 M = (M ^ T).
        TFUNCTION()
        TMatrix4 transpose() const;

        /// 计算矩阵的逆.
        TFUNCTION()
        TMatrix4 inverse() const;

        /// 计算仿射变换矩阵的逆.
        TFUNCTION()
        TMatrix4 inverseAffine() const;

        /// 返回是否仿射变换矩阵判断.
        TFUNCTION()
        bool isAffine() const;

        ///  返回是否缩放变换判断.
        TFUNCTION()
        bool hasScale() const;

        /// 计算矩阵的伴随矩阵.
        TFUNCTION()
        TMatrix4 adjoint() const;

        /// 计算矩阵行列式.
        TFUNCTION()
        T determinant() const;

        /// 连接两个仿射变换矩阵.
        TFUNCTION()
        TMatrix4 concatenateAffine(const TMatrix4& other) const;

        /// 分解成一个3-D向量表示的位移、一个3-D向量表示的缩放、
        /// 一个四元数表示的旋转.
        TFUNCTION()
        void decomposition(TVector3<T>& position, TVector3<T>& scale,
                TQuaternion<T>& orientation) const;

        /// 提取出一个表示其旋转或者缩放的3x3矩阵.
        TFUNCTION()
        void extractMatrix(TMatrix3<T>& rkMat) const;

        /// 提取出一个表示其旋转或者缩放的四元数.
        TFUNCTION()
        TQuaternion<T> extractQuaternion() const;

        /// 提取出一个3-D位移向量.
        TFUNCTION()
        TVector3<T> extractTranslation() const;

        /// 通过一个3-D位移向量、一个3-D缩放向量和一个旋转四元数构造矩阵.
        TFUNCTION()
        void makeTransform(const TVector3<T>& position,
                const TVector3<T>& scale, const TQuaternion<T>& orientation);

        /// 通过一个3-D位移向量、一个3-D缩放向量和一个旋转四元数构造逆矩阵.
        TFUNCTION()
        void makeInverseTransform(const TVector3<T>& position,
                const TVector3<T>& scale, const TQuaternion<T>& orientation);

        /// 通过指定的一个3-D向量构造位移矩阵.
        TFUNCTION()
        void makeTranslate(const TVector3<T>& position);

        /// 通过指定的3-D位移值构造位移矩阵.
        TFUNCTION()
        void makeTranslate(T tx, T ty, T tz);

        /// 设置矩阵缩放值.
        TFUNCTION()
        void setScale(const TVector3<T>& scale);

        /// 设置矩阵位移部分.
        TFUNCTION()
        void setTranslate(const TVector3<T>& position);

        /// 变换一个3-D向量.
        TFUNCTION()
        TVector3<T> transformAffine(const TVector3<T>& v) const;

        /// 变换一个4-D向量.
        TFUNCTION()
        TVector4<T> transformAffine(const TVector4<T>& v) const;

        /// 左手系透视投影变换.
        TFUNCTION()
        void perspective_LH(const TRadian<T> &fovY, T aspect, T zNear, T zFar);

        /// 右手系透视投影变换.
        TFUNCTION()
        void perspective_RH(const TRadian<T> &fovY, T aspect, T zNear, T zFar);

        /// 左手系正交投影变换.
        TFUNCTION()
        void orthographic_LH(T width, T height, T zNear, T zFar);

        /// 右手系正交投影变换.
        TFUNCTION()
        void orthographic_RH(T width, T height, T zNear, T zFar);

        /// 左手系观察变换.
        TFUNCTION()
        void lookAt_LH(const TVector3<T> &eye, const TVector3<T> &at, const TVector3<T> &up);

        /// 右手系观察变换.
        TFUNCTION()
        void lookAt_RH(const TVector3<T> &eye, const TVector3<T> &at, const TVector3<T> &up);

        static const TMatrix4 ZERO;      /// 零矩阵
        static const TMatrix4 IDENTITY;  /// 单位矩阵

    protected:
        int32_t compareArrays(const TMatrix4& other) const;

    private:
        TPROPERTY(RTTRFuncName = "data", RTTRFuncType = "getter")
        Buffer getData() const;

        TPROPERTY(RTTRFuncName = "data", RTTRFuncType = "setter")
        void setData(Buffer data);

        union
        {
            T m4x4[4][4];
            T mTuples[16];
        };
    };

    /// 重载乘法运算符，实现一个标量跟矩阵相乘
    template <typename T>
    TMatrix4<T> operator *(T scalar, const TMatrix4<T>& rkM);

    /// 重载乘法运算符，实现一个向量跟矩阵相乘.
    template <typename T>
    TVector4<T> operator *(const TVector4<T>& rkV, const TMatrix4<T>& rkM);
}


#include "T3DMatrix4.inl"


#endif  /*__T3D_MATRIX4_H__*/
