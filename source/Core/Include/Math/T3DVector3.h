

#ifndef __T3D_VECTOR3_H__
#define __T3D_VECTOR3_H__


#include "T3DPrerequisites.h"
#include "T3DMath.h"


namespace Tiny3D
{
	class T3D_ENGINE_API Vector3
	{
	public:
		/// Default constructor
		Vector3();
		/// Create a Vector3 with X, Y, Z
		Vector3(Real x, Real y, Real z);
		/// Copy constructor
		Vector3(const Vector3 &other);

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

		/// Get Z (constant).
		Real z() const;
		/// Get Z.
		Real &z();

		/// Assignment.
		Vector3 &operator =(const Vector3 &other);

		/// Comparison (equal to).
		bool operator ==(const Vector3 &other) const;
		/// Comparison (not equal to).
		bool operator !=(const Vector3 &other) const;
		/// Comparison (less than) for sorting only.
		bool operator <(const Vector3 &other) const;
		/// Comparison (less than or equal to) for sorting only.
		bool operator <=(const Vector3 &other) const;
		/// Comparison (greater than) for sorting only.
		bool operator >(const Vector3 &other) const;
		/// Comparison (greater than or equal to) for sorting only.
		bool operator >=(const Vector3 &other) const;

		/// Addition.
		Vector3 operator +(const Vector3 &other) const;
		/// Subtraction.
		Vector3 operator -(const Vector3 &other) const;
		/// Scalar multiplication.
		Vector3 operator *(Real fScalar) const;
		/// Scalar division.
		Vector3 operator /(Real fScalar) const;

		/// Negation.
		Vector3 operator -() const;
		/// Add and assign.
		Vector3 &operator +=(const Vector3 &other);
		/// Subtract and assign.
		Vector3 &operator -=(const Vector3 &other);
		/// Scalar multiply and assign.
		Vector3 &operator *=(Real fScalar);
		Vector3 &operator *=(const Vector3 &rkV);
		/// Scalar divide and assign.
		Vector3 &operator /=(Real fScalar);

		/// Get the length of vector, sqrt(x*x + y*y + z*z). 
		Real length() const;
		/// Get the squared length of vector, x*x + y*y + z*z. 
		Real squaredLength() const;

		/// Get the distance to another vector.
		Real distance(const Vector3 &other) const;
		/// Get the squared distance to another vector.
		Real squaredDistance(const Vector3 &other) const;

		/// Calculate dot product.
		Real dot(const Vector3 &other) const;

		/// Calculate cross product in right-handed rule. 
		Vector3 cross(const Vector3 &other) const;

		/// Normalize the vector.
		Real normalize();

		static const Vector3 ZERO;
		static const Vector3 UNIT_X;
		static const Vector3 UNIT_Y;
		static const Vector3 UNIT_Z;
		static const Vector3 NEGATIVE_UNIT_X;
		static const Vector3 NEGATIVE_UNIT_Y;
		static const Vector3 NEGATIVE_UNIT_Z;

	private:
		int32_t compareArrays(const Vector3 &other) const;

	private:
		Real	_x;
		Real	_y;
		Real	_z;
	};
}


#include "T3DVector3.inl"


#endif	/*__T3D_VECTOR3_H__*/
