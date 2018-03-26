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
    inline Vector4::Vector4()
        : _x(0.0)
        , _y(0.0)
        , _z(0.0)
        , _w(0.0)
    {

    }

    inline Vector4::Vector4(Real x, Real y, Real z, Real w)
        : _x(x)
        , _y(y)
        , _z(z)
        , _w(w)
    {

    }

    inline Vector4::Vector4(const Vector4 &other)
        : _x(other._x)
        , _y(other._y)
        , _z(other._z)
        , _w(other._w)
    {

    }

    inline Vector4::operator const Real *() const
    {
        return &_x;
    }

    inline Vector4::operator Real *()
    {
        return &_x;
    }

    inline Real Vector4::operator [](int32_t i) const
    {
        T3D_ASSERT(i < 4);
        return *(&_x+i);
    }

    inline Real &Vector4::operator [](int32_t i)
    {
        T3D_ASSERT(i < 4);
        return *(&_x+i);
    }

    inline Real Vector4::x() const
    {
        return _x;
    }

    inline Real &Vector4::x()
    {
        return _x;
    }

    inline Real Vector4::y() const
    {
        return _y;
    }

    inline Real &Vector4::y()
    {
        return _y;
    }

    inline Real Vector4::z() const
    {
        return _z;
    }

    inline Real &Vector4::z()
    {
        return _z;
    }

    inline Real Vector4::w() const
    {
        return _w;
    }

    inline Real &Vector4::w()
    {
        return _w;
    }

    inline Vector4 &Vector4::operator =(const Vector4 &other)
    {
        _x = other._x;
        _y = other._y;
        _z = other._z;
        _w = other._w;
        return *this;
    }

    inline bool Vector4::operator ==(const Vector4 &other) const
    {
        return (_x == other._x && _y == other._y && _z == other._z && _w == other._w);
    }

    inline bool Vector4::operator !=(const Vector4 &other) const
    {
        return (_x != other._x || _y != other._y || _z != other._z || _w != other._w);
    }

    inline Vector4 Vector4::operator +(const Vector4 &other) const
    {
        return Vector4(_x + other._x, _y + other._y, _z + other._z, _w + other._w);
    }

    inline Vector4 Vector4::operator -(const Vector4 &other) const
    {
        return Vector4(_x - other._x, _y - other._y, _z - other._z, _w - other._w);
    }

    inline Vector4 Vector4::operator *(Real scalar) const
    {
        return Vector4(scalar * _x, scalar * _y, scalar * _z, scalar * _w);
    }

    inline Vector4 Vector4::operator /(Real scalar) const
    {
        Vector4 result;

        if (scalar != 0.0)
        {
            result._x = _x / scalar;
            result._y = _y / scalar;
            result._z = _z / scalar;
            result._w = _w / scalar;
        }

        return result;
    }

    inline Vector4 Vector4::operator -() const
    {
        return Vector4(-_x, -_y, -_z, -_w);
    }

    inline Vector4 &Vector4::operator +=(const Vector4 &other)
    {
        _x += other._x;
        _y += other._y;
        _z += other._z;
        _w += other._w;
        return *this;
    }

    inline Vector4 &Vector4::operator -=(const Vector4 &other)
    {
        _x -= other._x;
        _y -= other._y;
        _z -= other._z;
        _w -= other._w;
        return *this;
    }

    inline Vector4 &Vector4::operator *=(Real fScalar)
    {
        _x *= fScalar;
        _y *= fScalar;
        _z *= fScalar;
        _w *= fScalar;
        return *this;
    }

    inline Vector4 &Vector4::operator /=(Real fScalar)
    {
        if (fScalar != 0.0)
        {
            _x /= fScalar;
            _y /= fScalar;
            _z /= fScalar;
            _w /= fScalar;
        }
        else
        {
            _x = 0.0;
            _y = 0.0;
            _z = 0.0;
            _w = 0.0;
        }
        return *this;
    }

    inline Real Vector4::length() const
    {
        return Math::sqrt(_x * _x + _y * _y + _z * _z + _w * _w);
    }

    inline Real Vector4::squaredLength() const
    {
        return _x * _x + _y * _y + _z * _z + _w * _w;
    }

    inline Real Vector4::distance(const Vector4 &other) const
    {
        return (*this - other).length();
    }

    inline Real Vector4::squaredDistance(const Vector4 &other) const
    {
        return (*this - other).squaredLength();
    }

    inline Real Vector4::dot(const Vector4 &other) const
    {
        return (_x * other._x + _y * other._y + _z * other._z + _w * other._w);
    }

    inline Vector4 Vector4::cross(const Vector4 &other) const
    {
        return Vector4(_y * other._z - _z * other._y,
            _z * other._w - _w * other._z,
            _w * other._x - _x * other._w,
            _x * other._y - _y * other._x);
    }

    inline Real Vector4::normalize()
    {
        Real fLength = length();

        if (fLength > 0.0)
        {
            _x /= fLength;
            _y /= fLength;
            _z /= fLength;
            _w /= fLength;
        }
        else
        {
            fLength = 0.0;
            _x = 0.0;
            _y = 0.0;
            _z = 0.0;
            _w = 0.0;
        }

        return fLength;
    }
}
