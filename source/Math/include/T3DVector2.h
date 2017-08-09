/***************************************************************************************************
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
 **************************************************************************************************/

#ifndef __T3D_VECTOR_2_H__
#define __T3D_VECTOR_2_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"


namespace Tiny3D
{
    class T3D_MATH_API Vector2
    {
    public:
        /// Constructor
        Vector2();
        /// Create a Vector2 with X, Y.
        Vector2(Real x, Real y);
        /// Copy constructor
        Vector2(const Vector2 &other);

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
        /// Get Y.
        Real &y();

        /// Assignment
        Vector2 &operator =(const Vector2 &other);

        /// Comparison (equal to).
        bool operator ==(const Vector2 &other) const;
        /// Comparison (not equal to).
        bool operator !=(const Vector2 &other) const;
        /// Comparison (less than) for sorting only.
        bool operator <(const Vector2 &other) const;
        /// Comparison (less than or equal to) for sorting only.
        bool operator <=(const Vector2 &other) const;
        /// Comparison (greater than) for sorting only.
        bool operator >(const Vector2 &other) const;
        /// Comparison (greater than or equal to) for sorting only.
        bool operator >=(const Vector2 &other) const;

        /// Addition.
        Vector2 operator +(const Vector2 &other) const;
        /// Subtraction.
        Vector2 operator -(const Vector2 &other) const;
        /// Scalar multiplication.
        Vector2 operator *(Real fScalar) const;
        /// Scalar division.
        Vector2 operator /(Real fScalar) const;

        Vector2 operator *(const Vector2 &other) const;

        /// Negation.
        Vector2 operator -() const;

        /// Add and assign.
        Vector2 &operator +=(const Vector2 &other);
        /// Subtract and assign.
        Vector2 &operator -=(const Vector2 &other);
        /// Scalar multiply and assign.
        Vector2 &operator *=(Real fScalar);
        Vector2 &operator *=(const Vector2 &other);
        /// Scalar divide and assign.
        Vector2 &operator /=(Real fScalar);

        /// Get the length of vector, sqrt(x*x + y*y).
        Real length() const;
        /// Get the squared length of vector, x*x + y*y.
        Real squaredLength() const;

        /// Get the distance to another vector.
        Real distance(const Vector2 &other) const;
        /// Get the squared distance to another vector.
        Real squaredDistance(const Vector2 &other) const;

        /// Calculate dot product.
        Real dot(const Vector2 &other) const;

        /// Calculate cross product.
        Real cross(const Vector2 &other) const;

        /// Normalize the vector.
        Real normalize();

        /// Calculate the perpendicular vector (y,-x).
        Vector2 perp() const;

        static const Vector2 ZERO;
        static const Vector2 UNIT_X;
        static const Vector2 UNIT_Y;
        static const Vector2 NEGATIVE_UNIT_X;
        static const Vector2 NEGATIVE_UNIT_Y;

    private:
        int32_t compareArrays(const Vector2 &other) const;

    private:
        Real _x;
        Real _y;
    };
}


#include "T3DVector2.inl"


#endif  /*__T3D_VECTOR_2_H__*/
