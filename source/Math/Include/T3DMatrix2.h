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

#ifndef __T3D_MATRIX2_H__
#define __T3D_MATRIX2_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"
#include "T3DVector2.h"


namespace Tiny3D
{
    /// 行优先存储构成的2x2方阵.
    TSTRUCT()
    template <typename T>
    struct TMatrix2
    {
        TRTTI_FRIEND

    public:
        /// 构造一个零矩阵
        TMatrix2();
        /// 构造一个零矩阵或者单位矩阵.
        TMatrix2(bool isZero);
        /// 拷贝构造函数.
        TMatrix2(const TMatrix2 &other);
        /// 通过指定行列值构造矩阵.
        TMatrix2(T m00, T m01, T m10, T m11);
        /// 通过一个指定行优先或者列优先的数组指定矩阵值来构造对象.
        TMatrix2(T tuple[4], bool isRowMajor);
        /// 通过指定两个行向量或者列向量构造对象.
        TMatrix2(const TVector2<T> &rkU, const TVector2<T> &rkV, bool isColumns);
        /// 通过指定指定一个行向量或者列向量数组构造对象.
        TMatrix2(const TVector2<T> *akV, bool isColumns);
        /// 通过指定对角元素值构造对角矩阵.
        TMatrix2(T m00, T m11);
        /// 通过指定弧度构造一个旋转矩阵.
        TMatrix2(const TRadian<T> &rkRadians);
        /// 通过两个向量张量积构造矩阵 M = U * (V ^ T).
        TMatrix2(const TVector2<T> &rkU, const TVector2<T> &rkV);

        /// 构造零矩阵.
        TFUNCTION()
        void makeZero();

        /// 构造单位矩阵.
        TFUNCTION()
        void makeIdentity();

        /// 构造对角矩阵.
        TFUNCTION()
        void makeDiagonal(T m00, T m11);

        /// 通过两个向量张量积构造矩阵 M = U * (V ^ T).
        TFUNCTION()
        void makeTensorProduct(const TVector2<T> &rkU, const TVector2<T> &rkV);

        /// 通过一个弧度构造旋转矩阵.
        TFUNCTION()
        void fromAngle(const TRadian<T> &radians);

        /// 从矩阵提取出旋转弧度.
        TFUNCTION()
        TRadian<T> toAngle() const;

        /// 获取矩阵元素数组首地址.
        operator const T *() const;
        operator T *();

        /// 根据获取矩阵对应行的元素值.
        const T *operator [](int32_t row) const;
        T *operator [](int32_t row);

        /// 通过指定行和列获取对应的元素.
        T operator ()(int32_t row, int32_t col) const;
        T &operator ()(int32_t row, int32_t col);

        /// 设置矩阵指定行向量.
        TFUNCTION()
        void setRow(int32_t row, const TVector2<T> &rkV);

        /// 获取矩阵指定行向量.
        TFUNCTION()
        TVector2<T> getRow(int32_t row) const;

        /// 设置矩阵指定列向量.
        TFUNCTION()
        void setColumn(int32_t col, const TVector2<T> &rkV);

        /// 获取矩阵指定列向量.
        TFUNCTION()
        TVector2<T> getColumn(int32_t col) const;

        /// 按照列向量优先获取矩阵数据.
        TFUNCTION()
        void getColumnMajor(T *columns) const;

        /// 重载赋值运算符.
        TMatrix2 &operator =(const TMatrix2 &other);

        /// 重载相等运算符.
        bool operator ==(const TMatrix2 &other) const;
        /// 重载不等运算符.
        bool operator !=(const TMatrix2 &other) const;
        /// 重载小于运算符.
        bool operator <(const TMatrix2 &other) const;
        /// 重载小于等于运算符.
        bool operator <=(const TMatrix2 &other) const;
        /// 重载大于运算符.
        bool operator >(const TMatrix2 &other) const;
        /// 重载大于等于运算符.
        bool operator >=(const TMatrix2 &other) const;

        /// 重载加法运算符.
        TMatrix2 operator +(const TMatrix2 &other) const;
        /// 重载减法运算符.
        TMatrix2 operator -(const TMatrix2 &other) const;

        /// 重载乘法运算符，实现两个矩阵相乘.
        TMatrix2 operator *(const TMatrix2 &other) const;

        /// 重载乘法运算符，实现矩阵跟一个标量相乘 M1 = M * scalar.
        TMatrix2 operator *(T scalar) const;
        /// 重载除法运算符，实现矩阵跟一个标量相除 M1 = M / scalar.
        TMatrix2 operator /(T scalar) const;

        /// 重载取反运算符.
        TMatrix2 operator -() const;

        /// 重载加法赋值运算符.
        TMatrix2 &operator +=(const TMatrix2 &other);
        /// 重载减法赋值运算符.
        TMatrix2 &operator -=(const TMatrix2 &other);
        /// 重载乘法赋值运算符，实现矩阵跟一个标量相乘 M = M * scalar.
        TMatrix2 &operator *=(T scalar);
        /// 重载除法赋值运算符，实现矩阵跟一个标量相除 M = M / scalar.
        TMatrix2 &operator /=(T scalar);

        /// 重载乘法运算符，实现矩阵左乘向量 M = M * V.
        TVector2<T> operator *(const TVector2<T> &rkV) const;

        /// 矩阵转置 M = M ^ T.
        TFUNCTION()
        TMatrix2 transpose() const;

        /// 矩阵转置后乘以另外一个矩阵 M2 = (M ^ T) * M1.
        TFUNCTION()
        TMatrix2 transposeTimes(const TMatrix2 &other) const;

        /// 矩阵乘以另一个矩阵的转置矩阵 M2 = M * (M1 ^ T).
        TFUNCTION()
        TMatrix2 timesTranspose(const TMatrix2 &other) const;

        /// 计算矩阵的逆.
        TFUNCTION()
        TMatrix2 inverse() const;

        /// 计算矩阵的伴随矩阵.
        TFUNCTION()
        TMatrix2 adjoint() const;

        /// 计算矩阵行列式.
        TFUNCTION()
        T determinant() const;

        /// 二次型 ret = (U ^ T) * M * V.
        TFUNCTION()
        T qform(const TVector2<T> &rkU, const TVector2<T> &rkV) const;

        /// 矩阵标准正交化，使用 Gram-Schmidt 算法
        TFUNCTION()
        void orthonormalize();

        /// 特征分解
        TFUNCTION()
        void eigendecomposition(TMatrix2 &rRot, TMatrix2 &rDiag) const;

        static const TMatrix2 ZERO;      /// 零矩阵
        static const TMatrix2 IDENTITY;  /// 单位矩阵

    private:
        /// 比较矩阵
        int32_t compareArrays(const TMatrix2 &other) const;

    private:
        TPROPERTY(RTTRFuncName="data", RTTRFuncType="getter")
        Buffer getData() const;

        TPROPERTY(RTTRFuncName="data", RTTRFuncType="setter")
        void setData(Buffer data);
        
        T    mTuples[4];       /// 以行优先存储的矩阵元素
    };

    /// 重载乘法运算符，实现一个标量跟矩阵相乘
    template <typename T>
    TMatrix2<T> operator *(T scalar, const TMatrix2<T> &rkM);

    /// 重载乘法运算符，实现一个向量跟矩阵相乘
    template <typename T>
    TVector2<T> operator *(const TVector2<T> &rkV, const TMatrix2<T> &rkM);
}


#include "T3DMatrix2.inl"


#endif  /*__T3D_MATRIX2_H__*/
