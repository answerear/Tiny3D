

#ifndef __T3D_MATRIX2_H__
#define __T3D_MATRIX2_H__


#include "T3DPrerequisites.h"
#include "T3DMath.h"
#include "T3DVector2.h"


namespace Tiny3D
{
	class T3D_ENGINE_API Matrix2
	{
	public:
		/// Create and initialize as zero matrix or identity matrix.
		Matrix2(bool bZero = true);
		/// Copy constructor.
		Matrix2(const Matrix2 &other);
		/// Create and initialize using rows and columns of numbers.
		Matrix2(Real fM00, Real fM01, Real fM10, Real fM11);
		/// Create a matrix from an array of numbers.
		Matrix2(Real afEntry[4], bool bRowMajor);
		/// Create matrix based on 2 vector inputs.
		Matrix2(const Vector2 &rkU, const Vector2 &rkV, bool bColumns);
		/// Create matrix base on vector array.
		Matrix2(const Vector2 *akV, bool bColumns);
		/// Create a diagonal matrix.
		Matrix2(Real fM00, Real fM11);
		/// Create a rotation matrix.
		Matrix2(const Radian &rkRadians);
		/// Create a tensor product.
		Matrix2(const Vector2 &rkU, const Vector2 &rkV);
		
		/// Make zero matrix.
		void makeZero();
		/// Make identity matrix.
		void makeIdentity();
		/// Make diagonal matrix.
		void makeDiagonal(Real fM00, Real fM11);
		/// Make tensor product matrix (U * V ^ T)
		void makeTensorProduct(const Vector2 &rkU, const Vector2 &rkV);

		/// Make rotation matrix.
		void fromAngle(const Radian &radians);
		/// Get the rotation angle.
		Radian toAngle() const;

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
		void setRow(int32_t nRow, const Vector2 &rkV);
		/// Get a matrix row data in Vector2.
		Vector2 getRow(int32_t nRow) const;

		/// Set a matrix column data.
		void setColumn(int32_t nCol, const Vector2 &rkV);
		/// Get a matrix column data in Vector2.
		Vector2 getColumn(int32_t nCol) const;

		/// Get array of matrix members in column major.
		void getColumnMajor(Real *afColumns) const;

		/// Assignment.
		Matrix2 &operator =(const Matrix2 &other);

		/// Comparison (equal to).
		bool operator ==(const Matrix2 &other) const;
		/// Comparison (not equal to).
		bool operator !=(const Matrix2 &other) const;
		/// Comparison (less than).
		bool operator <(const Matrix2 &other) const;
		/// Comparison (less than or equal to).
		bool operator <=(const Matrix2 &other) const;
		/// COmparison (greater than).
		bool operator >(const Matrix2 &other) const;
		/// Comparison (greater than or equal to).
		bool operator >=(const Matrix2 &other) const;

		/// Addition.
		Matrix2 operator +(const Matrix2 &other) const;
		/// Subtraction.
		Matrix2 operator -(const Matrix2 &other) const;
		
		/// Dot product.
		Matrix2 operator *(const Matrix2 &other) const;
		
		/// Scalar multiplication.
		Matrix2 operator *(Real fScalar) const;
		/// Scalar division.
		Matrix2 operator /(Real fScalar) const;

		/// Negation.
		Matrix2 operator -() const;

		/// Add and assign.
		Matrix2 &operator +=(const Matrix2 &other);
		/// Subtract and assign.
		Matrix2 &operator -=(const Matrix2 &other);
		/// Scalar multiply and assign.
		Matrix2 &operator *=(Real fScalar);
		/// Scalar divide and assign.
		Matrix2 &operator /=(Real fScalar);

		/// Matrix times vector (M * V).
		Vector2 operator *(const Vector2 &rkV) const;

		/// Transpose (M ^ T).
		Matrix2 transpose() const;
		/// this ^ T * M.
		Matrix2 transposeTimes(const Matrix2 &other) const;
		/// this * M ^ T.
		Matrix2 timesTranspose(const Matrix2 &other) const;

		/// Inverse matrix.
		Matrix2 inverse() const;

		/// Adjoint matrix.
		Matrix2 adjoint() const;

		/// Calculate determinant.
		Real determinant() const;

		/// U ^ T * M * V.
		Real qform(const Vector2 &rkU, const Vector2 &rkV) const;

		/// Orthonormalize
		void orthonormalize();

	public:
		static const Matrix2 ZERO;
		static const Matrix2 IDENTITY;

	private:
		int32_t compareArrays(const Matrix2 &other) const;

	private:
		Real	m_afEntry[4];
	};

	Matrix2 operator *(Real fScalar, const Matrix2 &rkM);

	Vector2 operator *(const Vector2 &rkV, const Matrix2 &rkM);
}


#include "T3DMatrix2.inl"


#endif	/*__T3D_MATRIX2_H__*/
