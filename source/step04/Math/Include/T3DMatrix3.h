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

#ifndef __T3D_MATRIX3_H__
#define __T3D_MATRIX3_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"
#include "T3DVector3.h"


namespace Tiny3D
{
    /// 行优先存储构成的3x3方阵.
    class T3D_MATH_API Matrix3
    {
    public:
        /// 构造一个零矩阵或者单位矩阵.
        Matrix3(bool isZero = true);
        /// 拷贝构造函数.
        Matrix3(const Matrix3 &other);
        /// 通过指定行列值构造矩阵.
        Matrix3(Real m00, Real m01, Real m02, Real m10, Real m11, Real m12, 
            Real m20, Real m21, Real m22);
        /// 通过一个指定行优先或者列优先的数组指定矩阵值来构造对象.
        Matrix3(Real tuple[9], bool isRowMajor);
        /// 通过指定三个行向量或者列向量构造对象.
        Matrix3(const Vector3 &rkU, const Vector3 &rkV, const Vector3 &rkW, 
            bool isColumns);
        /// 通过指定指定一个行向量或者列向量数组构造对象.
        Matrix3(const Vector3 *akV, bool isColumns);
        /// 通过指定对角元素值构造对角矩阵.
        Matrix3(Real m00, Real m11, Real m22);
        /// 通过指定旋转轴和弧度值构造一个旋转矩阵.
        Matrix3(const Vector3 &rkAxis, const Radian &rkRadians);
        /// 通过两个向量张量积构造矩阵 M = U * (V ^ T).
        Matrix3(const Vector3 &rkU, const Vector3 &rkV);

        /// 构造零矩阵.
        void makeZero();
        /// 构造单位矩阵.
        void makeIdentity();
        /// 构造对角矩阵.
        void makeDiagonal(Real m00, Real m11, Real m22);
        /// 通过两个向量张量积构造矩阵 M = U * (V ^ T).
        void makeTensorProduct(const Vector3 &rkU, const Vector3 &rkV);

        /// 通过指定旋转轴和弧度值构造一个旋转矩阵.
        void fromAxisAngle(const Vector3 &rkAxis, const Radian &radians);
        /// 从矩阵中提取出旋转轴和旋转角弧度.
        void toAxisAngle(Vector3 &rAxis, Radian &rRadians) const;

        /// 获取矩阵元素数组首地址.
        operator const Real *() const;
        operator Real *();

        /// 根据下标获取矩阵对应元素值.
        const Real *operator [](int32_t row) const;
        Real *operator [](int32_t row);

        /// 通过指定行和列获取对应的元素.
        Real operator ()(int32_t row, int32_t col) const;
        Real &operator ()(int32_t row, int32_t col);

        /// 设置矩阵指定行向量.
        void setRow(int32_t row, const Vector3 &rkV);
        /// 获取矩阵指定行向量.
        Vector3 getRow(int32_t row) const;

        /// 设置矩阵指定列向量.
        void setColumn(int32_t col, const Vector3 &rkV);
        /// 获取矩阵指定列向量.
        Vector3 getColumn(int32_t col) const;

        /// 按照列向量优先获取矩阵数据.
        void getColumnMajor(Real *columns) const;

        /// 重载赋值运算符.
        Matrix3 &operator =(const Matrix3 &other);

        /// 重载相等运算符.
        bool operator ==(const Matrix3 &other) const;
        /// 重载不等运算符.
        bool operator !=(const Matrix3 &other) const;
        /// 重载小于运算符.
        bool operator <(const Matrix3 &other) const;
        /// 重载小于等于运算符.
        bool operator <=(const Matrix3 &other) const;
        /// 重载大于运算符.
        bool operator >(const Matrix3 &other) const;
        /// 重载大于等于运算符.
        bool operator >=(const Matrix3 &other) const;

        /// 重载加法运算符.
        Matrix3 operator +(const Matrix3 &other) const;
        /// 重载减法运算符.
        Matrix3 operator -(const Matrix3 &other) const;

        /// 重载乘法运算符，实现两个矩阵相乘.
        Matrix3 operator *(const Matrix3 &other) const;

        /// 重载乘法运算符，实现矩阵跟一个标量相乘 M1 = M * scalar.
        Matrix3 operator *(Real scalar) const;
        /// 重载除法运算符，实现矩阵跟一个标量相除 M1 = M / scalar.
        Matrix3 operator /(Real scalar) const;

        /// 重载取反运算符.
        Matrix3 operator -() const;

        /// 重载加法赋值运算符.
        Matrix3 &operator +=(const Matrix3 &other);
        /// 重载减法赋值运算符.
        Matrix3 &operator -=(const Matrix3 &other);
        /// 重载乘法赋值运算符，实现矩阵跟一个标量相乘 M = M * scalar.
        Matrix3 &operator *=(Real scalar);
        /// 重载除法赋值运算符，实现矩阵跟一个标量相除 M = M / scalar.
        Matrix3 &operator /=(Real scalar);

        /// 重载乘法运算符，实现矩阵左乘向量 M = M * V.
        Vector3 operator *(const Vector3 &rkV) const;

        /// 矩阵转置 M = (M ^ T).
        Matrix3 transpose() const;
        /// 矩阵转置后乘以另外一个矩阵 M2 = (M ^ T) * M1.
        Matrix3 transposeTimes(const Matrix3 &other) const;
        /// 矩阵乘以另一个矩阵的转置矩阵 M2 = M * (M1 ^ T).
        Matrix3 timesTranspose(const Matrix3 &other) const;

        /// 计算矩阵的逆.
        Matrix3 inverse() const;

        /// 计算矩阵的伴随矩阵.
        Matrix3 adjoint() const;

        /// 计算矩阵行列式.
        Real determinant() const;

        /// 二次型 ret = (U ^ T) * M * V.
        Real qform(const Vector3 &rkU, const Vector3 &rkV) const;

        /// 计算本矩阵跟对角矩阵相乘 M1 = M * D
        Matrix3 timesDiagonal(const Vector3 &rkDiag) const;

        /// 计算对角矩阵跟本矩阵相乘 M1 = D * M
        Matrix3 diagonalTimes(const Vector3 &rkDiag) const;

        /// 矩阵标准正交化，使用 Gram-Schmidt 算法
        void orthonormalize();

        /// 这个矩阵必须是正交矩阵. 它分解为 Yaw * Pitch * Roll 
        /// 其中 Yaw 是绕向上的向量(Y轴)旋转；Pitch 是绕向右的向量(X轴)旋转； 
        /// Roll 是绕向前的向量(Z轴)旋转.
        ///
        ///        +-                       -+
        ///        |    1       0       0    |
        /// R(x) = |    0     cos(x) -sin(x) |
        ///        |    0     sin(x)  cos(x) |
        ///        +-                       -+
        ///
        ///        +-                       -+
        ///        |  cos(y)    0     sin(y) |
        /// R(y) = |    0       1       0    |
        ///        | -sin(y)    0     cos(y) |
        ///        +-                       -+
        ///
        ///        +-                       -+
        ///        |  cos(z) -sin(z)    0    |
        /// R(z) = |  sin(z)  cos(z)    0    |
        ///        |    0       0       1    |
        ///        +-                       -+

        /// 先绕X轴旋转，再绕Y轴旋转，最后绕Z轴旋转获得一个欧拉角旋转矩阵
        void fromEulerAnglesXYZ(
            const Radian &rkPitch, 
            const Radian &rkYaw, 
            const Radian &rkRoll);
        /// 先绕X轴旋转，再绕Z轴旋转，最后绕Z轴旋转获得一个欧拉角旋转矩阵
        void fromEulerAnglesXZY(
            const Radian &rkPitch, 
            const Radian &rkRoll, 
            const Radian &rkYaw);
        /// 先绕Y轴旋转，再绕X轴旋转，最后绕Z轴旋转获得一个欧拉角旋转矩阵
        void fromEulerAnglesYXZ(
            const Radian &rkYaw, 
            const Radian &rkPitch, 
            const Radian &rkRoll);
        /// 先绕Y轴旋转，再绕Z轴旋转，最后绕X轴旋转获得一个欧拉角旋转矩阵
        void fromEulerAnglesYZX(
            const Radian &rkYaw, 
            const Radian &rkRoll, 
            const Radian &rkPitch);
        /// 先绕Z轴旋转，再绕X轴旋转，最后绕Y轴旋转获得一个欧拉角旋转矩阵
        void fromEulerAnglesZXY(
            const Radian &rkRoll, 
            const Radian &rkPitch, 
            const Radian &rkYaw);
        /// 先绕Z轴旋转，再绕Y轴旋转，最后绕X轴旋转获得一个欧拉角旋转矩阵
        void fromEulerAnglesZYX(
            const Radian &krRoll, 
            const Radian &rkYaw, 
            const Radian &rkPitch);

        /// 从一个欧拉角旋转矩阵获取出三个轴的旋转弧度
        bool toEulerAnglesXYZ(Radian &rPitch, Radian &rYaw, Radian &rRoll) const;
        bool toEulerAnglesXZY(Radian &rPitch, Radian &rRoll, Radian &rYaw) const;
        bool toEulerAnglesYXZ(Radian &rYaw, Radian &rPitch, Radian &rRoll) const;
        bool toEulerAnglesYZX(Radian &rYaw, Radian &rRoll, Radian &rPitch) const;
        bool toEulerAnglesZXY(Radian &rRoll, Radian &rPitch, Radian &rYaw) const;
        bool toEulerAnglesZYX(Radian &rRoll, Radian &rYaw, Radian &rPitch) const;

        /// 矩阵分解成一个旋转矩阵、一个缩放矩阵、一个切变矩阵
        /// 其中 
        ///   rQ 表示分解出来的旋转矩阵
        ///   rD 表示分解出来的缩放对角矩阵中对角线上元素构成的向量
        ///   rU 表示分解出来的切变矩阵中上三角矩阵的元素构成的向量
        void QDUDecomposition(Matrix3 &rQ, Vector3 &rD, Vector3 &rU) const;

    public:
        static const Matrix3 ZERO;
        static const Matrix3 IDENTITY;

    private:
        int32_t compareArrays(const Matrix3 &other) const;

    private:
        union
        {
            Real    mTuples[9];
            Real    m3x3[3][3];
        };
    };

    Matrix3 operator *(Real fScalar, const Matrix3 &rkM);

    Vector3 operator *(const Vector3 &rkV, const Matrix3 &rkM);
}


#include "T3DMatrix3.inl"


#endif  /*__T3D_MATRIX3_H__*/
