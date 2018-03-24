

inline Radian::Radian(Real fRadian)
        : m_fRadian(fRadian)
{

}

inline Radian::Radian(const Radian &other)
    : m_fRadian(other.m_fRadian)
{

}

inline Radian &Radian::operator =(Real fRadian)
{
    m_fRadian = fRadian;
    return *this;
}

inline Radian &Radian::operator =(const Radian &other)
{
    m_fRadian = other.m_fRadian;
    return *this;
}

inline const Radian &Radian::operator +() const
{
    return *this;
}

inline Radian Radian::operator -() const
{
    return Radian(-m_fRadian);
}

inline bool Radian::operator ==(const Radian &other) const
{
    return (m_fRadian == other.m_fRadian);
}

inline bool Radian::operator !=(const Radian &other) const
{
    return (m_fRadian != other.m_fRadian);
}

inline bool Radian::operator <(const Radian &other) const
{
    return (m_fRadian < other.m_fRadian);
}

inline bool Radian::operator <=(const Radian &other) const
{
    return (m_fRadian <= other.m_fRadian);
}

inline bool Radian::operator >(const Radian &other) const
{
    return (m_fRadian > other.m_fRadian);
}

inline bool Radian::operator >=(const Radian &other) const
{
    return (m_fRadian >= other.m_fRadian);
}

inline Radian Radian::operator +(const Radian &other) const
{
    return Radian(m_fRadian + other.m_fRadian);
}

inline Radian Radian::operator -(const Radian &other) const
{
    return Radian(m_fRadian - other.m_fRadian);
}

inline Radian &Radian::operator +=(const Radian &other)
{
    m_fRadian += other.m_fRadian;
    return *this;
}

inline Radian &Radian::operator -=(const Radian &other)
{
    m_fRadian -= other.m_fRadian;
    return *this;
}

inline Radian Radian::operator *(Real fScalar) const
{
    return Radian(m_fRadian * fScalar);
}

inline Radian Radian::operator /(Real fScalar) const
{
    if (fScalar != 0.0)
        return Radian(m_fRadian / fScalar);
    return Radian();
}

inline Radian &Radian::operator *=(Real fScalar)
{
    m_fRadian *= fScalar;
    return *this;
}

inline Radian &Radian::operator /=(Real fScalar)
{
    if (fScalar != 0.0)
    {
        m_fRadian /= fScalar;
    }
    else
    {
        m_fRadian = 0.0;
    }

    return *this;
}

inline Real Radian::valueRadians() const
{
    return m_fRadian;
}


