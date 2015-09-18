

#ifndef __T3D_MATRIX2_H__
#define __T3D_MATRIX2_H__


#include "T3DEngineMacro.h"
#include "T3DMath.h"
#include "T3DRadian.h"
#include "T3DDegree.h"


namespace Tiny3D
{
	class Matrix2
	{
	public:
		/// Create and initialize as zero matrix or identity matrix.
		Matrix2(bool bZero = true);
		/// Copy constructor.
		Matrix2(const Matrix2 &other);
		/// Create and initialize using rows and columns of numbers.
		Matrix2(Real fM00, Real fM01, Real fM02, Real fM03);
		/// Create a matrix from an array of numbers.
		Matrix2(Real afEntry[4], bool bRowMajor);
		/// Create matrix based on 2 Vector inputs.
		Matrix2(const Vector2 &rkU, const Vector2 &rkV, bool bColumns);
		/// Create a diagonal matrix.
		Matrix2(Real fM00, Real fM11);
		/// Create a rotation matrix.
		Matrix2(const Degree &degree);
		
		/// Make zero matrix.
		void makeZero();
		/// Make identity matrix.
		void makeIdentity();
		/// Make diagonal matrix.
		void makeDiagonal(Real fM00, Real fM11);
		/// Make rotation matrix.
		void fromAngle(const Degree &degree);
	};
}


#include "T3DMatrix2.h"


#endif	/*__T3D_MATRIX2_H__*/
