

#ifndef __T3D_VECTOR4_H__
#define __T3D_VECTOR4_H__


#include "T3DPrerequisites.h"
#include "T3DMath.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Vector4
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
