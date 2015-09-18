

#include "T3DMath.h"


namespace Tiny3D
{
	const Real Math::POS_INFINITY = std::numeric_limits<Real>::infinity();
	const Real Math::NEG_INFINITY = -std::numeric_limits<Real>::infinity();

	const Real Math::PI = Real(4.0 * atan(1.0));
	const Real Math::TWO_PI = Real(2.0 * PI);
	const Real Math::HALF_PI = Real(0.5 * PI);

	const Real Math::DEGREES_TO_RADIANS = PI / Real(180.0);
	const Real Math::RADIANS_TO_DEGREES = Real(180.0) / PI;
}
