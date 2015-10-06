

#ifndef __T3D_MATRIX3_H__
#define __T3D_MATRIX3_H__


#include "T3DPrerequisites.h"
#include "T3DMath.h"
#include "T3DVector3.h"


namespace Tiny3D
{
	class T3D_ENGINE_API Matrix3
	{
	public:
		/// Create and initialize as zero matrix or identity matrix.
		Matrix3(bool bZero = true);
		/// Copy constructor.
		Matrix3(const Matrix3 &other);
		/// Create and initialize using rows and columns of numbers.
		Matrix3(Real fM00, Real fM01, Real fM02, Real fM10, Real fM11, Real fM12, Real fM20, Real fM21, Real fM22);
		/// Create a matrix from an array of numbers.
		Matrix3(Real afEntry[9], bool bRowMajor);
		/// Create matrix based on 3 vector inputs.
		Matrix3(const Vector3 &rkU, const Vector3 &rkV, const Vector3 &rkW, bool bColumns);
		/// Create matrix base on Vector array.
		Matrix3(const Vector3 *akV, bool bColumns);
		/// Create a diagonal matrix.
		Matrix3(Real fM00, Real fM11, Real fM22);
		/// Create a rotation matrix.
		Matrix3(const Vector3 &rkAxis, const Radian &rkRadians);
		/// Create a tensor product (U * V ^ T).
		Matrix3(const Vector3 &rkU, const Vector3 &rkV);

		/// Make zero matrix.
		void makeZero();
		/// Make identity matrix.
		void makeIdentity();
		/// Make diagonal matrix.
		void makeDiagonal(Real fM00, Real fM11, Real fM22);
		/// Make tensor product matrix (U * V ^ T)
		void makeTensorProduct(const Vector3 &rkU, const Vector3 &rkV);

		/// Make rotation matrix.
		void fromAxisAngle(const Vector3 &rkAxis, const Radian &radians);
		/// Get the rotation angle.
		void toAxisAngle(Vector3 &rAxis, Radian &rRadians) const;

		/// Get array of matrix members in row major (constant).
		operator const Real *() const;
		/// Get array of matrix members in row major.
		operator Real *();

		/// Get array of matrix row members (constant).
		const Real *operator [](int32_t nRow) const;
		/// Get array of matrix row members.
		Real *operator [](int32_t nRow);

		/// Get a matrix member (constant).
		Real operator ()(int32_t nRow, int32_t nCol) const;
		/// Get a matrix member.
		Real &operator ()(int32_t nRow, int32_t nCol);

		/// Set a matrix row data.
		void setRow(int32_t nRow, const Vector3 &rkV);
		/// Get a matrix row data in Vector3.
		Vector3 getRow(int32_t nRow) const;

		/// Set a matrix column data.
		void setColumn(int32_t nCol, const Vector3 &rkV);
		/// Get a matrix column data in Vector3.
		Vector3 getColumn(int32_t nCol) const;

		/// Get array of matrix members in column major.
		void getColumnMajor(Real *afColumns) const;

		/// Assignment.
		Matrix3 &operator =(const Matrix3 &other);

		/// Comparison (equal to).
		bool operator ==(const Matrix3 &other) const;
		/// Comparison (not equal to).
		bool operator !=(const Matrix3 &other) const;
		/// Comparison (less than).
		bool operator <(const Matrix3 &other) const;
		/// Comparison (less than or equal to).
		bool operator <=(const Matrix3 &other) const;
		/// COmparison (greater than).
		bool operator >(const Matrix3 &other) const;
		/// Comparison (greater than or equal to).
		bool operator >=(const Matrix3 &other) const;

		/// Addition.
		Matrix3 operator +(const Matrix3 &other) const;
		/// Subtraction.
		Matrix3 operator -(const Matrix3 &other) const;

		/// Dot product.
		Matrix3 operator *(const Matrix3 &other) const;

		/// Scalar multiplication.
		Matrix3 operator *(Real fScalar) const;
		/// Scalar division.
		Matrix3 operator /(Real fScalar) const;

		/// Negation.
		Matrix3 operator -() const;

		/// Add and assign.
		Matrix3 &operator +=(const Matrix3 &other);
		/// Subtract and assign.
		Matrix3 &operator -=(const Matrix3 &other);
		/// Scalar multiply and assign.
		Matrix3 &operator *=(Real fScalar);
		/// Scalar divide and assign.
		Matrix3 &operator /=(Real fScalar);

		/// Matrix times vector (M * V).
		Vector3 operator *(const Vector3 &rkV) const;

		/// Transpose (M ^ T).
		Matrix3 transpose() const;
		/// this ^ T * M.
		Matrix3 transposeTimes(const Matrix3 &other) const;
		/// this * M ^ T.
		Matrix3 timesTranspose(const Matrix3 &other) const;

		/// Inverse matrix.
		Matrix3 inverse() const;

		/// Adjoint matrix.
		Matrix3 adjoint() const;

		/// Calculate determinant.
		Real determinant() const;

		/// U ^ T * M * V.
		Real qform(const Vector3 &rkU, const Vector3 &rkV) const;

		/// M * D
		Matrix3 timesDiagonal(const Vector3 &rkDiag) const;

		/// D * M
		Matrix3 diagonalTimes(const Vector3 &rkDiag) const;

		/// Orthonormalize
		void orthonormalize();

		/// The matrix must be orthonormal.  The decomposition is yaw*pitch*roll where yaw is 
		/// rotation about the Up vector, pitch is rotation about the Right axis, and roll is 
		/// rotation about the Direction axis.
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
		///
		void fromEulerAnglesXYZ(const Radian &rkPitch, const Radian &rkYaw, const Radian &rkRoll);
		void fromEulerAnglesXZY(const Radian &rkPitch, const Radian &rkRoll, const Radian &rkYaw);
		void fromEulerAnglesYXZ(const Radian &rkYaw, const Radian &rkPitch, const Radian &rkRoll);
		void fromEulerAnglesYZX(const Radian &rkYaw, const Radian &rkRoll, const Radian &rkPitch);
		void fromEulerAnglesZXY(const Radian &rkRoll, const Radian &rkPitch, const Radian &rkYaw);
		void fromEulerAnglesZYX(const Radian &krRoll, const Radian &rkYaw, const Radian &rkPitch);

		bool toEulerAnglesXYZ(Radian &rPitch, Radian &rYaw, Radian &rRoll) const;
		bool toEulerAnglesXZY(Radian &rPitch, Radian &rRoll, Radian &rYaw) const;
		bool toEulerAnglesYXZ(Radian &rYaw, Radian &rPitch, Radian &rRoll) const;
		bool toEulerAnglesYZX(Radian &rYaw, Radian &rRoll, Radian &rPitch) const;
		bool toEulerAnglesZXY(Radian &rRoll, Radian &rPitch, Radian &rYaw) const;
		bool toEulerAnglesZYX(Radian &rRoll, Radian &rYaw, Radian &rPitch) const;

		void QDUDecomposition(Matrix3 &kQ, Vector3 &kD, Vector3 &kU) const;

	public:
		static const Matrix3 ZERO;
		static const Matrix3 IDENTITY;

	private:
		int32_t compareArrays(const Matrix3 &other) const;

	private:
		union
		{
			Real	m_afEntry[9];
			Real	_m3x3[3][3];
		};
	};

	Matrix3 operator *(Real fScalar, const Matrix3 &rkM);

	Vector3 operator *(const Vector3 &rkV, const Matrix3 &rkM);
}


#include "T3DMatrix3.inl"


#endif	/*__T3D_MATRIX3_H__*/
