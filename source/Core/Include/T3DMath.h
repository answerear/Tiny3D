

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
	};

	#include "T3DMath.inl"
}


#endif	/*__T3D_MATH_H__*/
