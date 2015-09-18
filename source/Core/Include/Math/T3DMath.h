

#ifndef __T3D_MATH_H__
#define __T3D_MATH_H__


#include "T3DEngineMacro.h"
#include <math.h>


namespace Tiny3D
{
	class Math
	{
	public:
		static float Sqr(Real fValue);
		static float Sqrt(Real fValue);

		static Real DegreesToRadians(Real degrees);
		static Real RadiansToDegrees(Real radians);

	public:
		static const Real POS_INFINITY;
		static const Real NEG_INFINITY;
		static const Real PI;
		static const Real TWO_PI;
		static const Real HALF_PI;
		static const Real DEGREES_TO_RADIANS;
		static const Real RADIANS_TO_DEGREES;
	};
}


#include "T3DMath.inl"


#endif	/*__T3D_MATH_H__*/
