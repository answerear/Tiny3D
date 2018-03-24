

inline Degree::Degree(Real fDegree /* = 0.0 */)
    : m_fDegree(fDegree)
{

}

inline Degree::Degree(const Degree &other)
{
    m_fDegree = other.m_fDegree;
}

inline Degree &Degree::operator =(Real fDegree)
{
    m_fDegree = fDegree;
    return *this;
}

inline Degree &Degree::operator =(const Degree &other)
{
    m_fDegree = other.m_fDegree;
    return *this;
}

inline const Degree &Degree::operator +() const
{
    return *this;
}

inline Degree Degree::operator -() const
{
    return Degree(-m_fDegree);
}

inline bool Degree::operator ==(const Degree &other) const
{
    return (m_fDegree == other.m_fDegree);
}

inline bool Degree::operator !=(const Degree &other) const
{
    return (m_fDegree != other.m_fDegree);
}

inline bool Degree::operator <(const Degree &other) const
{
    return (m_fDegree < other.m_fDegree);
}

inline bool Degree::operator <=(const Degree &other) const
{
    return (m_fDegree <= other.m_fDegree);
}

inline bool Degree::operator >(const Degree &other) const
{
    return (m_fDegree > other.m_fDegree);
}

inline bool Degree::operator >=(const Degree &other) const
{
    return (m_fDegree >= other.m_fDegree);
}

inline Degree Degree::operator +(const Degree &other) const
{
    return Degree(m_fDegree + other.m_fDegree);
}

inline Degree Degree::operator -(const Degree &other) const
{
    return Degree(m_fDegree - other.m_fDegree);
}

inline Degree &Degree::operator +=(const Degree &other)
{
    m_fDegree += other.m_fDegree;
    return *this;
}

inline Degree &Degree::operator -=(const Degree &other)
{
    m_fDegree -= other.m_fDegree;
    return *this;
}

inline Degree Degree::operator *(Real fScalar) const
{
    return Degree(m_fDegree * fScalar);
}

inline Degree Degree::operator /(Real fScalar) const
{
    if (fScalar != 0.0)
        return Degree(m_fDegree / fScalar);
    return Degree();
}

inline Degree &Degree::operator *=(Real fScalar)
{
    m_fDegree *= fScalar;
    return *this;
}

inline Degree &Degree::operator /=(Real fScalar)
{
    if (fScalar != 0.0)
    {
        m_fDegree /= fScalar;
    }
    else
    {
        m_fDegree = 0.0;
    }
    return *this;
}

inline Real Degree::valueDegrees() const
{
    return m_fDegree;
}

