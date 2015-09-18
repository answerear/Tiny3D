

namespace Tiny3D
{
	inline float Math::Sqr(Real fValue)
	{
		return fValue * fValue;
	}

	inline float Math::Sqrt(Real fValue)
	{
		return sqrt(fValue);
	}

	inline Real Math::DegreesToRadians(Real degrees)
	{
		return degrees * DEGREES_TO_RADIANS;
	}

	inline Real Math::RadiansToDegrees(Real radians)
	{
		return radians * RADIANS_TO_DEGREES;
	}
}

