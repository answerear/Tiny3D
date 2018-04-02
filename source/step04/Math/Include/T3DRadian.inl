

inline Radian::Radian(Real fRadian)
        : mRadian(fRadian)
{

}

inline Radian::Radian(const Radian &other)
    : mRadian(other.mRadian)
{

}

inline Radian &Radian::operator =(Real fRadian)
{
    mRadian = fRadian;
    return *this;
}

inline Radian &Radian::operator =(const Radian &other)
{
    mRadian = other.mRadian;
    return *this;
}

inline const Radian &Radian::operator +() const
{
    return *this;
}

inline Radian Radian::operator -() const
{
    return Radian(-mRadian);
}

inline bool Radian::operator ==(const Radian &other) const
{
    return (mRadian == other.mRadian);
}

inline bool Radian::operator !=(const Radian &other) const
{
    return (mRadian != other.mRadian);
}

inline bool Radian::operator <(const Radian &other) const
{
    return (mRadian < other.mRadian);
}

inline bool Radian::operator <=(const Radian &other) const
{
    return (mRadian <= other.mRadian);
}

inline bool Radian::operator >(const Radian &other) const
{
    return (mRadian > other.mRadian);
}

inline bool Radian::operator >=(const Radian &other) const
{
    return (mRadian >= other.mRadian);
}

inline Radian Radian::operator +(const Radian &other) const
{
    return Radian(mRadian + other.mRadian);
}

inline Radian Radian::operator -(const Radian &other) const
{
    return Radian(mRadian - other.mRadian);
}

inline Radian &Radian::operator +=(const Radian &other)
{
    mRadian += other.mRadian;
    return *this;
}

inline Radian &Radian::operator -=(const Radian &other)
{
    mRadian -= other.mRadian;
    return *this;
}

inline Radian Radian::operator *(Real scalar) const
{
    return Radian(mRadian * scalar);
}

inline Radian Radian::operator /(Real scalar) const
{
    if (scalar != REAL_ZERO)
        return Radian(mRadian / scalar);
    return Radian();
}

inline Radian &Radian::operator *=(Real scalar)
{
    mRadian *= scalar;
    return *this;
}

inline Radian &Radian::operator /=(Real scalar)
{
    if (scalar != Real(0.0))
    {
        mRadian /= scalar;
    }
    else
    {
        mRadian = REAL_ZERO;
    }

    return *this;
}

inline Real Radian::valueRadians() const
{
    return mRadian;
}


