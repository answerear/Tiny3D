

#ifndef __T3D_VECTOR_2_H__
#define __T3D_VECTOR_2_H__


#include "T3DEngineMacro.h"


namespace Tiny3D
{
	class T3D_ENGINE_API Vector2
	{
	public:
		/// Constructor
		Vector2();
		/// Create a Vector2 with X, Y.
		Vector2(float x, float y);
		/// Copy constructor
		Vector2(const Vector2 &other);

		/// Get array of members (constant).
		operator const float *() const;
		/// Get array of members.
		operator float *();

		/// Get a member (constant).
		float operator [](int32_t i) const;
		/// Get a member.
		float &operator [](int32_t i);

		/// Get X (constant).
		float x() const;
		/// Get X.
		float &x();

		/// Get Y (constant).
		float y() const;
		/// Get Y.
		float &y();

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
		Vector2 operator *(float fScalar) const;
		/// Scalar division.
		Vector2 operator /(float fScalar) const;

		/// Negation.
		Vector2 operator -() const;

		/// Add and assign.
		Vector2 &operator +=(const Vector2 &other);
		/// Subtract and assign.
		Vector2 &operator -=(const Vector2 &other);
		/// Scalar multiply and assign.
		Vector2 &operator *=(float fScalar);
		/// Scalar divide and assign.
		Vector2 &operator /=(float fScalar);

		/// Get the length of vector, sqrt(x*x + y*y). 
		float length() const;
		/// Get the squared length of vector, x*x + y*y. 
		float squaredLength() const;

		/// Get the distance to another vector.
		float distance() const;
		/// Get the squared distance to another vector.
		float squaredDistance() const;

		/// Calculate dot product.
		float dot(const Vector2 &other) const;

		/// Calculate cross product.
		float cross(const Vector2 &other) const;

		/// Normalize the vector.
		float normalize();

		/// Calculate the perpendicular vector (y,-x). 
		Vector2 perp();

		static const Vector2 ZERO;
		static const Vector2 UNIT_X;
		static const Vector2 UNIT_Y;
		static const Vector2 NEGATIVE_UNIT_X;
		static const Vector2 NEGATIVE_UNIT_Y;
		static const Vector2 UNIT_SCALE;

	private:
		float m_afTuple[2];
	};

	#include "T3DVector2.inl"
}


#endif	/*__T3D_VECTOR_2_H__*/
