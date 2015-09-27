

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
		/// Create and initialize from an explicit list of values.
		Quaternion(Real fW, Real fX, Real fY, Real fZ);
		/// Create a quaternion from a rotation matrix.
		Quaternion(const Matrix3 &matRot);
		/// Create a quaternion from an angle and axis.
		Quaternion(const Radian &rfAngle, const Vector3 &rkAxis);
		/// Create a quaternion from 3 orthonormal local axis.
		Quaternion(const Vector3 &rkXAxis, const Vector3 &rkYAxis, const Vector3 &rkZAxis);

	};
}


#endif	/*__T3D_QUATERNION_H__*/
