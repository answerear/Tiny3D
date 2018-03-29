

namespace Tiny3D
{
    inline void Quaternion::fromAngleAxis(const Radian &rkRadians, const Vector3 &rkAxis)
    {
        Radian fHalfAngle(0.5f * rkRadians.valueRadians());
        Real fSin = Math::sin(fHalfAngle);
        _w = Math::cos(fHalfAngle);
        _x = fSin * rkAxis.x();
        _y = fSin * rkAxis.y();
        _z = fSin * rkAxis.z();
    }

    inline void Quaternion::fromAxis(const Vector3 &rkXAxis, const Vector3 &rkYAxis, const Vector3 &rkZAxis)
    {
        Matrix3 kRot;

        kRot[0][0] = rkXAxis.x();
        kRot[1][0] = rkXAxis.y();
        kRot[2][0] = rkXAxis.z();

        kRot[0][1] = rkYAxis.x();
        kRot[1][1] = rkYAxis.y();
        kRot[2][1] = rkYAxis.z();

        kRot[0][2] = rkZAxis.x();
        kRot[1][2] = rkZAxis.y();
        kRot[2][2] = rkZAxis.z();

        fromRotationMatrix(kRot);
    }

    inline void Quaternion::fromAxis(const Vector3 *akAxis)
    {
        Matrix3 kRot;

        kRot[0][0] = akAxis[0].x();
        kRot[1][0] = akAxis[0].y();
        kRot[2][0] = akAxis[0].z();

        kRot[0][1] = akAxis[1].x();
        kRot[1][1] = akAxis[1].y();
        kRot[2][1] = akAxis[1].z();

        kRot[0][2] = akAxis[2].x();
        kRot[1][2] = akAxis[2].y();
        kRot[2][2] = akAxis[2].z();

        fromRotationMatrix(kRot);
    }

    inline Quaternion::Quaternion()
        : _w(0.0)
        , _x(0.0)
        , _y(0.0)
        , _z(0.0)
    {

    }

    inline Quaternion::Quaternion(const Quaternion &other)
    {
        _w = other._w;
        _x = other._x;
        _y = other._y;
        _z = other._z;
    }

    inline Quaternion::Quaternion(Real fW, Real fX, Real fY, Real fZ)
        : _w(fW)
        , _x(fX)
        , _y(fY)
        , _z(fZ)
    {

    }

    inline Quaternion::Quaternion(const Matrix3 &rkRot)
    {
        fromRotationMatrix(rkRot);
    }

    inline Quaternion::Quaternion(const Radian &rkAngle, const Vector3 &rkAxis)
    {
        fromAngleAxis(rkAngle, rkAxis);
    }

    inline Quaternion::Quaternion(const Vector3 &rkXAxis, const Vector3 &rkYAxis, const Vector3 &rkZAxis)
    {
        fromAxis(rkXAxis, rkYAxis, rkZAxis);
    }

    inline Quaternion::Quaternion(const Vector3 *akAxis)
    {
        fromAxis(akAxis);
    }

    inline Quaternion::Quaternion(Real *pValues)
        : _w(pValues[0])
        , _x(pValues[1])
        , _y(pValues[2])
        , _z(pValues[3])
    {
    }

    inline Quaternion::operator const Real *() const
    {
        return &_w;
    }

    inline Quaternion::operator Real *()
    {
        return &_w;
    }

    inline Real Quaternion::operator [](int32_t i) const
    {
        T3D_ASSERT(i >= 0 && i < 4);
        return *(&_w + i);
    }

    inline Real &Quaternion::operator [](int32_t i)
    {
        T3D_ASSERT(i >= 0 && i < 4);
        return *(&_w + i);
    }

    inline Real Quaternion::x() const
    {
        return _x;
    }

    inline Real &Quaternion::x()
    {
        return _x;
    }

    inline Real Quaternion::y() const
    {
        return _y;
    }

    inline Real &Quaternion::y()
    {
        return _y;
    }

    inline Real Quaternion::z() const
    {
        return _z;
    }

    inline Real &Quaternion::z()
    {
        return _z;
    }

    inline Real Quaternion::w() const
    {
        return _w;
    }

    inline Real &Quaternion::w()
    {
        return _w;
    }

    inline bool Quaternion::operator ==(const Quaternion &other) const
    {
        return (_w == other._w && _x == other._x && _y == other._y && _z == other._z);
    }

    inline bool Quaternion::operator !=(const Quaternion &other) const
    {
        return !operator ==(other);
    }

    inline Quaternion &Quaternion::operator =(const Quaternion &other)
    {
        _w = other._w;
        _x = other._x;
        _y = other._y;
        _z = other._z;
        return *this;
    }

    inline Quaternion Quaternion::operator +(const Quaternion &other) const
    {
        return Quaternion(_w+other._w, _x+other._x, _y+other._y, _z+other._z);
    }

    inline Quaternion Quaternion::operator -(const Quaternion &other) const
    {
        return Quaternion(_w-other._w, _x-other._x, _y-other._y, _z-other._z);
    }

    inline Quaternion Quaternion::operator *(const Quaternion &other) const
    {
        return Quaternion
            (
            _w * other._w - _x * other._x - _y * other._y - _z * other._z,
            _w * other._x + _x * other._w + _y * other._z - _z * other._y,
            _w * other._y + _y * other._w + _z * other._x - _x * other._z,
            _w * other._z + _z * other._w + _x * other._y - _y * other._x
            );
    }

    inline Quaternion Quaternion::operator *(Real fScalar) const
    {
        return Quaternion(_w*fScalar, _x*fScalar, _y*fScalar, _z*fScalar);
    }

    inline Quaternion Quaternion::operator /(Real fScalar) const
    {
        Real fInvertScalar = 0.0f;
        if (fScalar != 0.0)
            fInvertScalar = 1.0f / fScalar;
        return Quaternion(_w * fInvertScalar, _x * fInvertScalar, _y * fInvertScalar, _z * fInvertScalar);
    }

    inline Quaternion &Quaternion::operator +=(const Quaternion &other)
    {
        _w += other._w;
        _x += other._x;
        _y += other._y;
        _z += other._z;
        return *this;
    }

    inline Quaternion &Quaternion::operator -=(const Quaternion &other)
    {
        _w -= other._w;
        _x -= other._x;
        _y -= other._y;
        _z -= other._z;
        return *this;
    }

    inline Quaternion &Quaternion::operator *=(const Quaternion &other)
    {
        Real w = _w * other._w - _x * other._x - _y * other._y - _z * other._z;
        Real x = _w * other._x + _x * other._w + _y * other._z - _z * other._y;
        Real y = _w * other._y + _y * other._w + _z * other._x - _x * other._z;
        Real z = _w * other._z + _z * other._w + _x * other._y - _y * other._x;
        _w = w, _x = x, _y = y, _z = z;
        return *this;
    }

    inline Quaternion &Quaternion::operator *=(Real fScalar)
    {
        _w *= fScalar;
        _x *= fScalar;
        _y *= fScalar;
        _z *= fScalar;
        return *this;
    }

    inline Quaternion &Quaternion::operator /=(Real fScalar)
    {
        Real fInvertScalar = 0.0f;
        if (fScalar != 0.0)
            fInvertScalar = 1.0f / fScalar;
        _w *= fInvertScalar;
        _x *= fInvertScalar;
        _y *= fInvertScalar;
        _z *= fInvertScalar;
        return *this;
    }

    inline Quaternion Quaternion::operator -() const
    {
        return Quaternion(-_w, -_x, -_y, -_z);
    }

    inline Vector3 Quaternion::operator* (const Vector3& v) const
    {
        // nVidia SDK implementation
        Vector3 uv, uuv;
        Vector3 qvec(_x, _y, _z);
        uv = qvec.cross(v);
        uuv = qvec.cross(uv);
        uv *= (2.0f * _w);
        uuv *= 2.0f;

        return v + uv + uuv;
    }

    inline Real Quaternion::dot(const Quaternion &other) const
    {
        return (_x * other._x + _y * other._y + _z * other._z + _w * other._w);
    }

    inline Real Quaternion::norm() const
    {
        return _w * _w + _x * _x + _y * _y + _z * _z;
    }

    inline Real Quaternion::normalize()
    {
        Real len = norm();
        Real factor = 1.0f / Math::sqrt(len);
        *this = *this * factor;
        return len;
    }

    inline Quaternion Quaternion::inverse() const
    {
        Real fNorm = _w * _w + _x * _x + _y * _y + _z * _z;
        if (fNorm > 0.0)
        {
            Real fInvNorm = 1.0f / fNorm;
            return Quaternion(_w * fInvNorm, -_x * fInvNorm, -_y * fInvNorm, -_z * fInvNorm);
        }
        else
        {
            // return an invalid result to flag the error
            return ZERO;
        }
    }

    inline void Quaternion::toAngleAxis(Degree &rDegree, Vector3 &rAxis) const
    {
        Radian rAngle;
        toAngleAxis(rAngle, rAxis);
        rDegree = rAngle;
    }

    inline Vector3 Quaternion::xAxis() const
    {
        //Real fTx  = 2.0*x;
        Real fTy  = 2.0f*_y;
        Real fTz  = 2.0f*_z;
        Real fTwy = fTy*_w;
        Real fTwz = fTz*_w;
        Real fTxy = fTy*_x;
        Real fTxz = fTz*_x;
        Real fTyy = fTy*_y;
        Real fTzz = fTz*_z;

        return Vector3(1.0f-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);
    }

    inline Vector3 Quaternion::yAxis() const
    {
        Real fTx  = 2.0f*_x;
        Real fTy  = 2.0f*_y;
        Real fTz  = 2.0f*_z;
        Real fTwx = fTx*_w;
        Real fTwz = fTz*_w;
        Real fTxx = fTx*_x;
        Real fTxy = fTy*_x;
        Real fTyz = fTz*_y;
        Real fTzz = fTz*_z;

        return Vector3(fTxy-fTwz, 1.0f-(fTxx+fTzz), fTyz+fTwx);
    }

    inline Vector3 Quaternion::zAxis() const
    {
        Real fTx  = 2.0f*_x;
        Real fTy  = 2.0f*_y;
        Real fTz  = 2.0f*_z;
        Real fTwx = fTx*_w;
        Real fTwy = fTy*_w;
        Real fTxx = fTx*_x;
        Real fTxz = fTz*_x;
        Real fTyy = fTy*_y;
        Real fTyz = fTz*_y;

        return Vector3(fTxz+fTwy, fTyz-fTwx, 1.0f-(fTxx+fTyy));
    }

    inline Quaternion &Quaternion::lerp(const Quaternion &rkP, const Quaternion &rkQ, Real fTimes)
    {
        Real fScale = 1.0f - fTimes;
        Quaternion q = (rkP * fScale) + (rkQ * fTimes);
        q.normalize();
        return (*this = q);
//         Real fCos = rkP.dot(rkQ);
//         *this = rkP + fTimes * (rkQ - rkP);
//         normalize();
//         return *this;
    }

    inline Quaternion operator *(Real fScalar, const Quaternion &rkQ)
    {
        return Quaternion(fScalar*rkQ.x(), fScalar*rkQ.y(), fScalar*rkQ.z(), fScalar*rkQ.w());
    }
}
