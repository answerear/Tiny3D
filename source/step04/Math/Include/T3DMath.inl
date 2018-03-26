

namespace Tiny3D
{
    ////////////////////////////////////////////////////////////////////////////

    inline Real Math::degreesToRadians(Real degrees)
    {
        return degrees * DEGREES_TO_RADIANS;
    }

    inline Real Math::radiansToDegrees(Real radians)
    {
        return radians * RADIANS_TO_DEGREES;
    }

    ////////////////////////////////////////////////////////////////////////////

    inline Real Radian::valueDegrees() const
    {
        return Math::radiansToDegrees(m_fRadian);
    }

    inline Real Degree::valueRadians() const
    {
        return Math::degreesToRadians(m_fDegree);
    }

    ////////////////////////////////////////////////////////////////////////////

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

    ////////////////////////////////////////////////////////////////////////////

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

    ////////////////////////////////////////////////////////////////////////////

    inline Real Math::sqr(Real value)
    {
        return value * value;
    }

    inline Real Math::sqrt(Real value)
    {
        return ::sqrt(value);
    }

    inline Real Math::invSqrt(Real value)
    {
        return Real(1.0) / ::sqrt(value);
    }

    inline Real Math::abs(Real value)
    { 
        return Real(fabs(value));
    }

    inline Degree Math::abs(const Degree &value)
    { 
        return Degree(fabs(value.valueDegrees())); 
    }

    inline Radian Math::abs(const Radian& value)
    { 
        return Radian(fabs(value.valueRadians()));
    }

    inline bool Math::realEqual(Real a, Real b, 
        Real tolerance /* = std::numeric_limits<Real>::epsilon() */)
    {
        if (fabs(b-a) <= tolerance)
            return true;
        else
            return false;
    }

    inline Real Math::sin(const Degree &degrees)
    {
        return Real(::sin(degrees.valueRadians()));
    }

    inline Real Math::sin(const Radian &radians)
    {
        return Real(::sin(radians.valueRadians()));
    }

    inline Real Math::cos(const Degree &degrees)
    {
        return Real(::cos(degrees.valueRadians()));
    }

    inline Real Math::cos(const Radian &radians)
    {
        return Real(::cos(radians.valueRadians()));
    }

    inline Real Math::tan(const Degree &degrees)
    {
        return Real(::tan(degrees.valueRadians()));
    }

    inline Real Math::tan(const Radian &radians)
    {
        return Real(::tan(radians.valueRadians()));
    }

    inline Radian Math::asin(Real fValue)
    {
        return Radian(::asin(fValue));
    }

    inline Radian Math::acos(Real fValue)
    {
        return Radian(::acos(fValue));
    }

    inline Radian Math::atan(Real fValue)
    {
        return Radian(::atan(fValue));
    }

    inline Radian Math::atan2(Real fY, Real fX)
    {
        return Radian(::atan2(fY, fX));
    }
}

