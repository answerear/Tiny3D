

#ifndef __T3D_MATRIX3_H__
#define __T3D_MATRIX3_H__


#include "T3DEngineMacro.h"
#include "T3DMath.h"
#include "T3DVector3.h"


namespace Tiny3D
{
	class T3D_ENGINE_API Matrix3
	{
	public:
		Matrix3(bool bZero = true);
		Matrix3(Real fM00, Real fM01, Real fM02, Real fM10, Real fM11, Real fM12, Real fM20, Real fM21, Real fM22);

	public:
		static const Matrix3 ZERO;
		static const Matrix3 IDENTITY;

	private:
		Real	m_afEntry[9];
	};
}


#include "T3DMatrix3.inl"


#endif	/*__T3D_MATRIX3_H__*/
