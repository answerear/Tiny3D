/*******************************************************************************
* This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
* Copyright (C) 2015-2017  Answer Wong
* For latest info, see https://github.com/asnwerear/Tiny3D
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

namespace Tiny3D
{
    inline Vector3::Vector3()
        : _x(0.0)
        , _y(0.0)
        , _z(0.0)
    {

    }

    inline Vector3::Vector3(Real x, Real y, Real z)
        : _x(x)
        , _y(y)
        , _z(z)
    {

    }

    inline Vector3::Vector3(const Vector3 &other)
        : _x(other._x)
        , _y(other._y)
        , _z(other._z)
    {

    }

    inline Vector3::operator const Real *() const
    {
        return &_x;
    }

    inline Vector3::operator Real *()
    {
        return &_x;
    }

    inline Real Vector3::operator [](int32_t i) const
    {
        T3D_ASSERT(i < 3);
        return *(&_x+i);
    }

    inline Real &Vector3::operator [](int32_t i)
    {
        T3D_ASSERT(i < 3);
        return *(&_x+i);
    }

    inline Real Vector3::x() const
    {
        return _x;
    }

    inline Real &Vector3::x()
    {
        return _x;
    }

    inline Real Vector3::y() const
    {
        return _y;
    }

    inline Real &Vector3::y()
    {
        return _y;
    }

    inline Real Vector3::z() const
    {
        return _z;
    }

    inline Real &Vector3::z()
    {
        return _z;
    }

    inline Vector3 &Vector3::operator =(const Vector3 &other)
    {
        _x = other._x;
        _y = other._y;
        _z = other._z;
        return *this;
    }

    inline bool Vector3::operator ==(const Vector3 &other) const
    {
        return (_x == other._x && _y == other._y && _z == other._z);
    }

    inline bool Vector3::operator !=(const Vector3 &other) const
    {
        return (_x != other._x || _y != other._y || _z != other._z);
    }

    inline int32_t Vector3::compareArrays(const Vector3 &other) const
    {
        return memcmp(&_x, &other._x, 3*sizeof(Real));
    }

    inline bool Vector3::operator <(const Vector3 &other) const
    {
        return compareArrays(other) < 0;
    }

    inline bool Vector3::operator <=(const Vector3 &other) const
    {
        return compareArrays(other) <= 0;
    }

    inline bool Vector3::operator >(const Vector3 &other) const
    {
        return compareArrays(other) > 0;
    }

    inline bool Vector3::operator >=(const Vector3 &other) const
    {
        return compareArrays(other) >= 0;
    }

    inline Vector3 Vector3::operator +(const Vector3 &other) const
    {
        return Vector3(_x + other._x, _y + other._y, _z + other._z);
    }

    inline Vector3 Vector3::operator -(const Vector3 &other) const
    {
        return Vector3(_x - other._x, _y - other._y, _z - other._z);
    }

    inline Vector3 Vector3::operator *(Real scalar) const
    {
        return Vector3(scalar * _x, scalar * _y, scalar * _z);
    }

    inline Vector3 Vector3::operator /(Real scalar) const
    {
        Vector3 result;

        if (scalar != REAL_ZERO)
        {
            result._x = _x / scalar;
            result._y = _y / scalar;
            result._z = _z / scalar;
        }

        return result;
    }

    inline Vector3 Vector3::operator *(const Vector3 &other) const
    {
        Vector3 v;
        v.x() = _x * other._x;
        v.y() = _y * other._y;
        v.z() = _z * other._z;
        return v;
    }

    inline Vector3 Vector3::operator -() const
    {
        return Vector3(-_x, -_y, -_z);
    }

    inline Vector3 &Vector3::operator +=(const Vector3 &other)
    {
        _x += other._x;
        _y += other._y;
        _z += other._z;
        return *this;
    }

    inline Vector3 &Vector3::operator -=(const Vector3 &other)
    {
        _x -= other._x;
        _y -= other._y;
        _z -= other._z;
        return *this;
    }

    inline Vector3 &Vector3::operator *=(Real fScalar)
    {
        _x *= fScalar;
        _y *= fScalar;
        _z *= fScalar;
        return *this;
    }

    inline Vector3 &Vector3::operator *=( const Vector3 &rkVector)
    {
        _x *= rkVector._x;
        _y *= rkVector._y;
        _z *= rkVector._z;
        return *this;
    }


    inline Vector3 &Vector3::operator /=(Real scalar)
    {
        if (scalar != REAL_ZERO)
        {
            _x /= scalar;
            _y /= scalar;
            _z /= scalar;
        }
        else
        {
            _x = REAL_ZERO;
            _y = REAL_ZERO;
            _z = REAL_ZERO;
        }

        return *this;
    }

    inline Real Vector3::length() const
    {
        return Math::sqrt(_x * _x + _y * _y + _z * _z);
    }

    inline Real Vector3::squaredLength() const
    {
        return (_x * _x + _y * _y + _z * _z);
    }

    inline Real Vector3::distance(const Vector3 &other) const
    {
        return (*this - other).length();
    }

    inline Real Vector3::squaredDistance(const Vector3 &other) const
    {
        return (*this - other).squaredLength();
    }

    inline Real Vector3::dot(const Vector3 &other) const
    {
        return (_x * other._x + _y * other._y + _z * other._z);
    }

    inline Vector3 Vector3::cross(const Vector3 &other) const
    {
        return Vector3(_y * other._z - _z * other._y,
            _z * other._x - _x * other._z,
            _x * other._y - _y * other._x);
    }

    inline Real Vector3::normalize()
    {
        Real len = length();

        if (len > REAL_ZERO)
        {
            _x /= len;
            _y /= len;
            _z /= len;
        }
        else
        {
            len = REAL_ZERO;
            _x = REAL_ZERO;
            _y = REAL_ZERO;
            _z = REAL_ZERO;
        }

        return len;
    }

    inline Vector3 operator *(Real scalar, const Vector3 &rkV)
    {
        return Vector3(scalar * rkV.x(), scalar * rkV.y(), scalar * rkV.z());
    }
}
