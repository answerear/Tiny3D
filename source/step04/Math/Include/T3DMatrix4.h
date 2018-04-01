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

#ifndef __T3D_MATRIX4_H__
#define __T3D_MATRIX4_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"
#include "T3DMatrix3.h"
#include "T3DQuaternion.h"
#include "T3DVector4.h"


namespace Tiny3D
{
    class T3D_MATH_API Matrix4
    {
    public:
        /// 默认构造函数.
        Matrix4(bool isZero = false);
        /// 拷贝构造函数.
        Matrix4(const Matrix4 &other);
        /// 指定各行各列具体数值构造对象.
        Matrix4(Real m00, Real m01, Real m02, Real m03,
                Real m10, Real m11, Real m12, Real m13,
                Real m20, Real m21, Real m22, Real m23,
                Real m30, Real m31, Real m32, Real m33);
        /// 通过一个旋转矩阵或者缩放矩阵构造对象，其中平移部分为0.
        Matrix4(const Matrix3 &rkMat);
        /// 通过一个旋转或者缩放计算的四元数构造对象，其中平移部分为0.
        Matrix4(const Quaternion &rkRot);
        /// 通过位移向量构造对象.
        Matrix4(const Vector3 &rkPos);
        /// 构造对角矩阵.
        Matrix4(Real m00, Real m11, Real m22, Real m33);
        /// 通过三正交向量构造对象
        Matrix4(const Vector3 &X, const Vector3 &Y, const Vector3 &Z);

        /// 构造零矩阵.
        void makeZero();
        /// 构造单位矩阵.
        void makeIdentity();

        /// 返回成员变量的数组首地址.
        operator const Real *() const;
        operator Real *();

        /// 根据获取矩阵对应行的元素值.
        const Real *operator [](int32_t row) const;
        Real *operator [](int32_t row);

        /// 通过指定行和列获取对应的元素.
        Real operator ()(int32_t row, int32_t col) const;
        Real &operator ()(int32_t row, int32_t col);

        /// 重载赋值运算符，相同类型对象赋值.
        Matrix4 &operator =(const Matrix4 &other);
        /// 重载赋值运算符，从一个3x3方针赋值.
        void operator =(const Matrix3 &rkMat);

        /// 重载相等运算符.
        bool operator ==(const Matrix4 &other) const;
        /// 重载不等运算符.
        bool operator !=(const Matrix4 &other) const;
        /// 重载小于运算符.
        bool operator <(const Matrix4 &other) const;
        /// 重载小于等于运算符.
        bool operator <=(const Matrix4 &other) const;
        /// 重载大于运算符.
        bool operator >(const Matrix4 &other) const;
        /// 重载大于等于运算符.
        bool operator >=(const Matrix4 &other) const;

        /// 重载加法运算符.
        Matrix4 operator +(const Matrix4 &other) const;
        /// 重载减法运算符.
        Matrix4 operator -(const Matrix4 &other) const;

        /// 重载乘法运算符，实现两个矩阵相乘.
        Matrix4 operator *(const Matrix4 &other) const;

        /// 重载乘法运算符，实现矩阵跟一个标量相乘 M1 = M * scalar.
        Matrix4 operator *(Real scalar) const;
        /// 重载除法运算符，实现矩阵跟一个标量相除 M1 = M / scalar.
        Matrix4 operator /(Real scalar) const;

        /// 重载取反运算符.
        Matrix4 operator -() const;

        /// 重载加法赋值运算符.
        Matrix4 &operator +=(const Matrix4 &other);
        /// 重载减法赋值运算符.
        Matrix4 &operator -=(const Matrix4 &other);
        /// 重载乘法赋值运算符，实现矩阵跟一个标量相乘 M = M * scalar.
        Matrix4 &operator *=(Real scalar);
        /// 重载除法赋值运算符，实现矩阵跟一个标量相除 M = M / scalar.
        Matrix4 &operator /=(Real scalar);

        /// 重载乘法运算符，实现矩阵左乘向量 M = M * V.
        Vector4 operator *(const Vector4 &rkV) const;
        Vector3 operator *(const Vector3 &rkV) const;

        /// 矩阵转置 M = (M ^ T).
        Matrix4 transpose() const;

        /// 计算矩阵的逆.
        Matrix4 inverse() const;
        /// 计算仿射变换矩阵的逆.
        Matrix4 inverseAffine() const;

        /// 返回是否仿射变换矩阵判断.
        bool isAffine() const;

        ///  返回是否缩放变换判断.
        bool hasScale() const;

        /// 计算矩阵的伴随矩阵.
        Matrix4 adjoint() const;

        /// 计算矩阵行列式.
        Real determinant() const;

        /// 连接两个仿射变换矩阵.
        Matrix4 concatenateAffine(const Matrix4 &other) const;

        /// 分解成一个3-D向量表示的位移、一个3-D向量表示的缩放、
        /// 一个四元数表示的旋转.
        void decomposition(Vector3 &position, Vector3 &scale, 
            Quaternion &orientation) const;

        /// 提取出一个表示其旋转或者缩放的3x3矩阵.
        void extractMatrix(Matrix3 &rkMat) const;
        /// 提取出一个表示其旋转或者缩放的四元数.
        Quaternion extractQuaternion() const;
        /// 提取出一个3-D位移向量.
        Vector3 extractTranslation() const;

        /// 通过一个3-D位移向量、一个3-D缩放向量和一个旋转四元数构造矩阵.
        void makeTransform(const Vector3 &position, const Vector3 &scale, 
            const Quaternion &orientation);
        /// 通过一个3-D位移向量、一个3-D缩放向量和一个旋转四元数构造逆矩阵.
        void makeInverseTransform(const Vector3 &position, const Vector3 &scale, 
            const Quaternion &orientation);

        /// 通过指定的一个3-D向量构造位移矩阵.
        void makeTranslate(const Vector3 &position);
        /// 通过指定的3-D位移值构造位移矩阵.
        void makeTranslate(Real tx, Real ty, Real tz);

        /// 设置矩阵缩放值.
        void setScale(const Vector3 &scale);

        /// 设置矩阵位移部分.
        void setTranslate(const Vector3 &position);

        /// 变换一个3-D向量.
        Vector3 transformAffine(const Vector3 &v) const;
        /// 变换一个4-D向量.
        Vector4 transformAffine(const Vector4 &v) const;

        static const Matrix4 ZERO;      /// 零矩阵
        static const Matrix4 IDENTITY;  /// 单位矩阵

    protected:
        int32_t compareArrays(const Matrix4 &other) const;

    private:
        union
        {
            Real m4x4[4][4];
            Real mTuples[16];
        };
    };

    /// 重载乘法运算符，实现一个标量跟矩阵相乘
    Matrix4 operator *(Real scalar, const Matrix4 &rkM);

    /// 重载乘法运算符，实现一个向量跟矩阵相乘.
    Vector4 operator *(const Vector4 &rkV, const Matrix4 &rkM);
}


#include "T3DMatrix4.inl"


#endif  /*__T3D_MATRIX4_H__*/
