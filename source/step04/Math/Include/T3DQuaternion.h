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

#ifndef __T3D_QUATERNION_H__
#define __T3D_QUATERNION_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"
#include "T3DMatrix3.h"


namespace Tiny3D
{
    class T3D_MATH_API Quaternion
    {
    public:
        /// 默认构造函数，构造一个单位四元数(1,0,0,0)对象.
        Quaternion();
        /// 拷贝构造函数.
        Quaternion(const Quaternion &other);
        /// 指定四元数四个值构造对象.
        Quaternion(Real w, Real x, Real y, Real z);
        /// 通过一个旋转矩阵构造一个四元数对象.
        Quaternion(const Matrix3 &rkRot);
        /// 指定旋转轴和旋转弧度构造一个四元数对象.
        Quaternion(const Radian &rkAngle, const Vector3 &rkAxis);
        /// 通过指定3个正交轴构造一个四元数对象.
        Quaternion(
            const Vector3 &rkXAxis, 
            const Vector3 &rkYAxis, 
            const Vector3 &rkZAxis);
        /// 通过指定3个正交向量数组首地址构造一个四元数对象(XYZ顺序).
        Quaternion(const Vector3 *akAxis);
        /// 通过指定4个元素的数组首地址构造一个四元数对象(w, x, y, z).
        Quaternion(Real *values);

        /// 获取四元数元素数值数组首地址，按照w, x, y, z顺序返回.
        operator const Real *() const;
        operator Real *();

        /// 根据下标获取四元数元素数值.
        Real operator [](int32_t i) const;
        Real &operator [](int32_t i);

        /// 获取X元素值.
        Real x() const;
        Real &x();

        /// 获取Y元素值.
        Real y() const;
        Real &y();

        /// 获取Z元素值.
        Real z() const;
        Real &z();

        /// 获取W元素值.
        Real w() const;
        Real &w();

        /// 重载相等运算符.
        bool operator ==(const Quaternion &other) const;
        /// 重载不等运算符.
        bool operator !=(const Quaternion &other) const;

        /// 重载赋值运算符.
        Quaternion &operator =(const Quaternion &other);

        /// 重载加法运算符.
        Quaternion operator +(const Quaternion &other) const;
        /// 重载减法运算符.
        Quaternion operator -(const Quaternion &other) const;
        /// 重载乘法运算符，实现跟另一个四元数的叉乘.
        Quaternion operator *(const Quaternion &other) const;
        /// 重载乘法运算符，实现跟一个标量相乘.
        Quaternion operator *(Real scalar) const;
        /// 重载除法运算符，实现跟一个标量相除.
        Quaternion operator /(Real scalar) const;

        /// 重载加法赋值运算符.
        Quaternion &operator +=(const Quaternion &other);
        /// 重载减法赋值运算符.
        Quaternion &operator -=(const Quaternion &other);
        /// 重载乘法赋值运算符，实现跟另一个四元数的叉乘.
        Quaternion &operator *=(const Quaternion &other);
        /// 重载乘法赋值运算符，实现跟一个标量相乘.
        Quaternion &operator *=(Real scalar);
        /// 重载除法赋值运算符，实现跟一个标量相乘.
        Quaternion &operator /=(Real scalar);

        /// 重载取反运算符.
        Quaternion operator -() const;

        /// 通过四元数旋转一个向量.
        Vector3 operator* (const Vector3 &rkVector) const;

        /// 计算四元数点积.
        Real dot(const Quaternion &other) const;

        /// 计算四元数的范数的平方.
        Real norm() const;
        /// 规范化四元数，返回四元数长度.
        Real normalize();

        /// 计算四元数的逆.
        Quaternion inverse() const;

        /// 根据指定旋转弧度和旋转轴构造一个四元数.
        void fromAngleAxis(const Radian &rkRadians, const Vector3 &rkAxis);
        /// 根据指定三个正交向量构造一个四元数.
        void fromAxis(
            const Vector3 &rkXAxis, 
            const Vector3 &rkYAxis, 
            const Vector3 &rkZAxis);
        /// 根据指定三个正交向量数组首地址构造一个四元数.
        void fromAxis(const Vector3 *akAxis);
        /// 根据一个旋转矩阵构造一个四元数.
        void fromRotationMatrix(const Matrix3 &rkRot);

        /// 把四元数转成一个旋转角(弧度)和一个单位向量表示的旋转轴.
        void toAngleAxis(Radian &rAngle, Vector3 &rAxis) const;
        /// 把四元数转成一个旋转角(角度)和一个单位向量表示的旋转轴.
        void toAngleAxis(Degree &rAngle, Vector3 &rAxis) const;
        /// 把四元数转成三个正交单位向量.
        void toAngleAxis(Vector3 *aAxis) const;
        /// 把四元数转成一个旋转矩阵.
        void toRotationMatrix(Matrix3 &rRot) const;

        /// 获取绕X轴旋转(俯仰角)的角度(单位：弧度).
        Radian getPitch(bool reprojectAxis = true) const;
        /// 获取绕Z轴旋转(翻滚角)的角度(单位：弧度).
        Radian getRoll(bool reprojectAxis = true) const;
        /// 获取绕Y轴旋转(偏航角)的角度(单位：弧度).
        Radian getYaw(bool reprojectAxis = true) const;

        /// 获取X正交轴.
        Vector3 xAxis() const;
        /// 获取Y正交轴.
        Vector3 yAxis() const;
        /// 获取Z正交轴.
        Vector3 zAxis() const;

        /// 两个四元数球面线性插值构造一个四元数.
        Quaternion &slerp(const Quaternion &rkP, const Quaternion &rkQ, 
            Real times, bool shortestPath, Real threshold = 1e-03);

        static const Quaternion IDENTITY;       /// 单位四元数
        static const Quaternion ZERO;           /// 零四元数

    private:
        Real    _w;
        Real    _x;
        Real    _y;
        Real    _z;
    };

    /// 重载乘法运算符，实现一个标量跟四元数相乘
    Quaternion operator *(Real scalar, const Quaternion &rkQ);
}


#include "T3DQuaternion.inl"


#endif  /*__T3D_QUATERNION_H__*/
