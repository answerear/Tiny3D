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

#ifndef __T3D_VECTOR4_H__
#define __T3D_VECTOR4_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"


namespace Tiny3D
{
    class T3D_MATH_API Vector4
    {
    public:
        /// Default constructor
        Vector4();
        /// Create a Vector4 with X, Y, Z, W
        Vector4(Real x, Real y, Real z, Real w);
        /// Copy constructor
        Vector4(const Vector4 &other);

        /// Get array of members (constant).
        operator const Real *() const;
        /// Get array of members.
        operator Real *();

        /// Get a member (constant).
        Real operator [](int32_t i) const;
        /// Get a member.
        Real &operator [](int32_t i);

        /// Get X (constant).
        Real x() const;
        /// Get X.
        Real &x();

        /// Get Y (constant).
        Real y() const;
        /// Get Y
        Real &y();

        /// Get Z (constant).
        Real z() const;
        /// Get Z
        Real &z();

        /// Get W (constant).
        Real w() const;
        /// Get W
        Real &w();

        /// Assignment
        Vector4 &operator =(const Vector4 &other);

        /// Comparison (equal to).
        bool operator ==(const Vector4 &other) const;
        /// Comparison (not equal to).
        bool operator !=(const Vector4 &other) const;

        /// Addition.
        Vector4 operator +(const Vector4 &other) const;
        /// Subtraction.
        Vector4 operator -(const Vector4 &other) const;
        /// Scalar multiplication.
        Vector4 operator *(Real fScalar) const;
        /// Scalar division.
        Vector4 operator /(Real fScalar) const;

        /// Negation.
        Vector4 operator -() const;

        /// Add and assign
        Vector4 &operator +=(const Vector4 &other);
        /// Subtract and assign
        Vector4 &operator -=(const Vector4 &other);
        /// Scalar multiply and assign.
        Vector4 &operator *=(Real fScalar);
        /// Scalar divide and assign.
        Vector4 &operator /=(Real fScalar);

        /// Get the length of vector, sqrt(x*x + y*y + z*z + w*w).
        Real length() const;
        /// Get the squared length of vector, x*x + y*y + z*z + w*w.
        Real squaredLength() const;

        /// Get the distance to another vector.
        Real distance(const Vector4 &other) const;
        /// Get the squared distance to another vector.
        Real squaredDistance(const Vector4 &other) const;

        /// Calculate dot product.
        Real dot(const Vector4 &other) const;

        /// Calculate cross product in right-handed rule.
        Vector4 cross(const Vector4 &other) const;

        /// Normalize the vector.
        Real normalize();

        static const Vector4 ZERO;

    private:
        Real    _x;
        Real    _y;
        Real    _z;
        Real    _w;
    };
}


#include "T3DVector4.inl"


#endif  /*__T3D_VECTOR4_H__*/
