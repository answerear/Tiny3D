

#ifndef __T3D_QUATERNION_H__
#define __T3D_QUATERNION_H__


#include "T3DEngineMacro.h"
#include "T3DMath.h"
#include "T3DMatrix3.h"


namespace Tiny3D
{
	class T3D_ENGINE_API Quaternion
	{
	public:
		/// Default constructor, initialize to identity rotation.
		Quaternion();
		/// Copy constructor.
		Quaternion(const Quaternion &other);
		/// Create and initialize from an explicit list of values.
		Quaternion(Real fW, Real fX, Real fY, Real fZ);
		/// Create a quaternion from a rotation matrix.
		Quaternion(const Matrix3 &rkRot);
		/// Create a quaternion from an angle and axis.
		Quaternion(const Radian &rkAngle, const Vector3 &rkAxis);
		/// Create a quaternion from 3 orthonormal local axis.
		Quaternion(const Vector3 &rkXAxis, const Vector3 &rkYAxis, const Vector3 &rkZAxis);
		/// Create a quaternion from 3 orthonormal local axis.
		Quaternion(const Vector3 *akAxis);
		/// Create a quaternion from 4 manual /w/x/y/z values.
		Quaternion(Real *pValues);

		/// Get array of quaternion members in w,x,y,z order (constant).
		operator const Real *() const;
		/// Get array of quaternion members in w,x,y,z order.
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

		/// Comparison (equal to).
		bool operator ==(const Quaternion &other) const;
		/// Comparison (not equal to).
		bool operator !=(const Quaternion &other) const;

		/// Assignment.
		Quaternion &operator =(const Quaternion &other);

		/// Addition.
		Quaternion operator +(const Quaternion &other) const;
		/// Subtraction.
		Quaternion operator -(const Quaternion &other) const;
		/// Multiplication.
		Quaternion operator *(const Quaternion &other) const;
		/// Scalar multiplication.
		Quaternion operator *(Real fScalar) const;
		/// Scalar division.
		Quaternion operator /(Real fScalar) const;

		/// Add and assign.
		Quaternion &operator +=(const Quaternion &other);
		/// Subtract and assign.
		Quaternion &operator -=(const Quaternion &other);
		/// Multiply and assign.
		Quaternion &operator *=(const Quaternion &other);
		/// Scalar multiply and assign.
		Quaternion &operator *=(Real fScalar);
		/// Scalar divide and assign.
		Quaternion &operator /=(Real fScalar);

		/// Negation.
		Quaternion operator -() const;

		/// Calculate dot product.
		Real dot(const Quaternion &other) const;

		/// Normalize the quaternion.
		Real normalize();

		/// Create quaternion from rotation angle and axis.
		void fromAngleAxis(const Radian &rkRadians, const Vector3 &rkAxis);
		/// Create a quaternion from 3 orthonormal local axis.
		void fromAxis(const Vector3 &rkXAxis, const Vector3 &rkYAxis, const Vector3 &rkZAxis);
		/// Create a quaternion from 3 orthonormal local axis.
		void fromAxis(const Vector3 *akAxis);
		/// Create a quaternion from a rotation matrix.
		void fromRotationMatrix(const Matrix3 &rkRot);

		/// Fills an angle (radians) around an axis (unit vector).
		void toAngleAxis(Radian &rAngle, Vector3 &rAxis) const;
		/// Fills an angle (degrees) around an axis (unit vector).
		void toAngleAxis(Degree &rAngle, Vector3 &rAxis) const;
		/// Gets the 3 orthonormal axis defining the quaternion.
		void toAngleAxis(Vector3 *aAxis) const;
		/// Gets a rotation matrix from this quaternion.
		void toRotationMatrix(Matrix3 &rRot) const;

		/// Calculate the local pitch element of this quaternion.
		Radian getPitch() const;
		/// Calculate the local roll element of this quaternion.
		Radian getRoll() const;
		/// Calculate the local yaw element of this quaternion.
		Radian getYaw() const;

		/// Inverses this quaternion.
		Quaternion inverse() const;

		/// Returns the X orthonormal axis defining the quaternion.
		Vector3 xAxis() const;
		/// Returns the Y orthonormal axis defining the quaternion.
		Vector3 yAxis() const;
		/// Returns the Z orthonormal axis defining the quaternion.
		Vector3 zAxis() const;

		/// Set this quaternion to the linear interpolation between two quaternions.
		Quaternion &lerp(const Quaternion &);

		/// Set this quaternion to the result of the spherical interpolation between two quaternions.
		Quaternion &slerp(const Quaternion &);

		static const Quaternion IDENTITY;
		static const Quaternion ZERO;

	private:
		Real	_w;
		Real	_x;
		Real	_y;
		Real	_z;
	};
}


#include "T3DQuaternion.inl"


#endif	/*__T3D_QUATERNION_H__*/
