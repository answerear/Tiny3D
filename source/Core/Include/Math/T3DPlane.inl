

namespace Tiny3D
{
    inline Plane::Plane()
    {
        mCoeff[0] = Real(0.0);
        mCoeff[1] = Real(1.0);
        mCoeff[2] = Real(0.0);
        mCoeff[3] = Real(0.0);
    }

    inline Plane::Plane(const Vector3 &rkNormal, Real fDistance)
    {
        mCoeff[0] = rkNormal[0];
        mCoeff[1] = rkNormal[1];
        mCoeff[2] = rkNormal[2];
        mCoeff[3] = fDistance;
    }

    inline Plane::Plane(const Vector3 &rkNormal, const Vector3 &rkPos)
    {
        mCoeff[0] = rkNormal[0];
        mCoeff[1] = rkNormal[1];
        mCoeff[2] = rkNormal[2];
        mCoeff[3] = - rkNormal.dot(rkPos);
    }

    inline Plane::Plane(const Vector3 &rkPos1, const Vector3 &rkPos2, const Vector3 &rkPos3)
    {
        Vector3 normal = (rkPos2 - rkPos1).cross(rkPos3 - rkPos1);
        normal.normalize();

        mCoeff[0] = normal[0];
        mCoeff[1] = normal[1];
        mCoeff[2] = normal[2];

        mCoeff[3] = -normal.dot(rkPos1);
    }

    inline Plane::Plane(const Plane &rkOther)
    {
        mCoeff[0] = rkOther.mCoeff[0];
        mCoeff[1] = rkOther.mCoeff[1];
        mCoeff[2] = rkOther.mCoeff[2];
        mCoeff[3] = rkOther.mCoeff[3];
    }

    inline Plane &Plane::operator =(const Plane &rkOther)
    {
        mCoeff[0] = rkOther.mCoeff[0];
        mCoeff[1] = rkOther.mCoeff[1];
        mCoeff[2] = rkOther.mCoeff[2];
        mCoeff[3] = rkOther.mCoeff[3];
        return *this;
    }

    inline bool Plane::operator ==(const Plane &rkOther) const
    {
        return (mCoeff[0] == rkOther.mCoeff[0] && mCoeff[1] == rkOther.mCoeff[1]
            && mCoeff[2] == rkOther.mCoeff[2] && mCoeff[3] == rkOther.mCoeff[3]);
    }

    inline bool Plane::operator !=(const Plane &rkOther) const
    {
        return !operator ==(rkOther);
    }

    inline const Real Plane::operator [](int32_t idx) const
    {
        T3D_ASSERT(idx >= 0 && idx < 4);
        return mCoeff[idx];
    }

    inline Real &Plane::operator [](int32_t idx)
    {
        T3D_ASSERT(idx >= 0 && idx < 4);
        return mCoeff[idx];
    }

    inline Real Plane::distanceToPoint(const Vector3 &rkPos) const
    {
        Vector3 normal(mCoeff[0], mCoeff[1], mCoeff[2]);
        return normal.dot(rkPos) + mCoeff[3];
    }

    inline Plane::Side Plane::sideForPoint(const Vector3 &rkPos) const
    {
        Real distance = distanceToPoint(rkPos);
        Plane::Side side = E_SIDE_NONE;

        if (distance < Real(0.0))
            side = E_SIDE_NEGATIVE;
        else if (distance > Real(0.0))
            side = E_SIDE_POSITIVE;

        return side;
    }

    inline Vector3 Plane::getNormal() const
    {
        return Vector3(mCoeff[0], mCoeff[1], mCoeff[2]);
    }

    inline Real Plane::getDistance() const
    {
        return mCoeff[3];
    }

    inline void Plane::normalize()
    {
        Real length = Math::Sqrt(mCoeff[0] * mCoeff[0] + mCoeff[1] * mCoeff[1] + mCoeff[2] * mCoeff[2]);
        mCoeff[0] /= length;
        mCoeff[1] /= length;
        mCoeff[2] /= length;
        mCoeff[3] /= length;
    }
}
