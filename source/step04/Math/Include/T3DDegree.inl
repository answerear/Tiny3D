

inline Degree::Degree(Real fDegree /* = 0.0 */)
    : mDegree(fDegree)
{

}

inline Degree::Degree(const Degree &other)
{
    mDegree = other.mDegree;
}

inline Degree &Degree::operator =(Real fDegree)
{
    mDegree = fDegree;
    return *this;
}

inline Degree &Degree::operator =(const Degree &other)
{
    mDegree = other.mDegree;
    return *this;
}

inline const Degree &Degree::operator +() const
{
    return *this;
}

inline Degree Degree::operator -() const
{
    return Degree(-mDegree);
}

inline bool Degree::operator ==(const Degree &other) const
{
    return (mDegree == other.mDegree);
}

inline bool Degree::operator !=(const Degree &other) const
{
    return (mDegree != other.mDegree);
}

inline bool Degree::operator <(const Degree &other) const
{
    return (mDegree < other.mDegree);
}

inline bool Degree::operator <=(const Degree &other) const
{
    return (mDegree <= other.mDegree);
}

inline bool Degree::operator >(const Degree &other) const
{
    return (mDegree > other.mDegree);
}

inline bool Degree::operator >=(const Degree &other) const
{
    return (mDegree >= other.mDegree);
}

inline Degree Degree::operator +(const Degree &other) const
{
    return Degree(mDegree + other.mDegree);
}

inline Degree Degree::operator -(const Degree &other) const
{
    return Degree(mDegree - other.mDegree);
}

inline Degree &Degree::operator +=(const Degree &other)
{
    mDegree += other.mDegree;
    return *this;
}

inline Degree &Degree::operator -=(const Degree &other)
{
    mDegree -= other.mDegree;
    return *this;
}

inline Degree Degree::operator *(Real scalar) const
{
    return Degree(mDegree * scalar);
}

inline Degree Degree::operator /(Real scalar) const
{
    if (scalar != REAL_ZERO)
        return Degree(mDegree / scalar);
    return Degree();
}

inline Degree &Degree::operator *=(Real scalar)
{
    mDegree *= scalar;
    return *this;
}

inline Degree &Degree::operator /=(Real scalar)
{
    if (scalar != REAL_ZERO)
    {
        mDegree /= scalar;
    }
    else
    {
        mDegree = REAL_ZERO;
    }
    return *this;
}

inline Real Degree::valueDegrees() const
{
    return mDegree;
}

