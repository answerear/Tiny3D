

namespace Tiny3D
{
	////////////////////////////////////////////////////////////////////////////////////////////////

	inline Real Math::DegreesToRadians(Real degrees)
	{
		return degrees * DEGREES_TO_RADIANS;
	}

	inline Real Math::RadiansToDegrees(Real radians)
	{
		return radians * RADIANS_TO_DEGREES;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	inline Real Radian::valueDegrees() const
	{
		return Math::RadiansToDegrees(m_fRadian);
	}

	inline Real Degree::valueRadians() const
	{
		return Math::DegreesToRadians(m_fDegree);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	inline Radian::Radian(const Degree &degree)
		: m_fRadian(degree.valueRadians())
	{

	}

	inline Radian &Radian::operator =(const Degree &degree)
	{
		m_fRadian = degree.valueRadians();
		return *this;
	}

	inline Radian Radian::operator +(const Degree &degree) const
	{
		return Radian(m_fRadian + degree.valueRadians());
	}

	inline Radian Radian::operator -(const Degree &degree) const
	{
		return Radian(m_fRadian - degree.valueRadians());
	}

	inline Radian &Radian::operator +=(const Degree &degree)
	{
		m_fRadian += degree.valueRadians();
		return *this;
	}

	inline Radian &Radian::operator -=(const Degree &degree)
	{
		m_fRadian -= degree.valueRadians();
		return *this;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	inline Degree::Degree(const Radian &radian)
		: m_fDegree(radian.valueDegrees())
	{

	}

	inline Degree &Degree::operator =(const Radian &radian)
	{
		m_fDegree = radian.valueDegrees();
		return *this;
	}

	inline Degree Degree::operator +(const Radian &radian) const
	{
		return Degree(m_fDegree + radian.valueDegrees());
	}

	inline Degree Degree::operator -(const Radian &radian) const
	{
		return Degree(m_fDegree - radian.valueDegrees());
	}

	inline Degree &Degree::operator +=(const Radian &radian)
	{
		m_fDegree += radian.valueDegrees();
		return *this;
	}

	inline Degree &Degree::operator -=(const Radian &radian)
	{
		m_fDegree -= radian.valueDegrees();
		return *this;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	inline Real Math::Sqr(Real fValue)
	{
		return fValue * fValue;
	}

	inline Real Math::Sqrt(Real fValue)
	{
		return sqrt(fValue);
	}

	inline Real Math::InvSqrt(Real fValue)
	{
		return Real(1.0) / sqrt(fValue);
	}

	inline Real Math::Abs(Real fValue)
	{ 
		return Real(fabs(fValue)); 
	}

	inline Degree Math::Abs(const Degree &dValue)
	{ 
		return Degree(fabs(dValue.valueDegrees())); 
	}

	inline Radian Math::Abs(const Radian& rValue)
	{ 
		return Radian(fabs(rValue.valueRadians())); 
	}

	inline bool Math::RealEqual(Real a, Real b, Real tolerance /* = std::numeric_limits<Real>::epsilon() */)
	{
		if (fabs(b-a) <= tolerance)
			return true;
		else
			return false;
	}

	inline Real Math::Sin(const Degree &degrees)
	{
		return Real(sin(degrees.valueRadians()));
	}

	inline Real Math::Sin(const Radian &radians)
	{
		return Real(sin(radians.valueRadians()));
	}

	inline Real Math::Cos(const Degree &degrees)
	{
		return Real(cos(degrees.valueRadians()));
	}

	inline Real Math::Cos(const Radian &radians)
	{
		return Real(cos(radians.valueRadians()));
	}

	inline Real Math::Tan(const Degree &degrees)
	{
		return Real(tan(degrees.valueRadians()));
	}

	inline Real Math::Tan(const Radian &radians)
	{
		return Real(tan(radians.valueRadians()));
	}

	inline Radian Math::ASin(Real fValue)
	{
		return Radian(asin(fValue));
	}

	inline Radian Math::ACos(Real fValue)
	{
		return Radian(acos(fValue));
	}

	inline Radian Math::ATan(Real fValue)
	{
		return Radian(atan(fValue));
	}

	inline Radian Math::ATan2(Real fY, Real fX)
	{
		return Radian(atan2(fY, fX));
	}
}

