/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

namespace Tiny3D
{
    template <typename T>
    inline TVector4<T>::TVector4()
        : _x(0.0)
        , _y(0.0)
        , _z(0.0)
        , _w(0.0)
    {

    }

    template <typename T>
    inline TVector4<T>::TVector4(T x, T y, T z, T w)
        : _x(x)
        , _y(y)
        , _z(z)
        , _w(w)
    {

    }

    template <typename T>
    inline TVector4<T>::TVector4(const TVector3<T> &v, T w)
        : _x(v.x())
        , _y(v.y())
        , _z(v.z())
        , _w(w)
    {

    }

    template <typename T>
    inline TVector4<T>::TVector4(const TVector4 &other)
        : _x(other._x)
        , _y(other._y)
        , _z(other._z)
        , _w(other._w)
    {

    }

    template <typename T>
    inline TVector4<T>::operator const T *() const
    {
        return &_x;
    }

    template <typename T>
    inline TVector4<T>::operator T *()
    {
        return &_x;
    }

    template <typename T>
    inline T TVector4<T>::operator [](int32_t i) const
    {
        T3D_ASSERT(i < 4, "The i is out of bound !");
        return *(&_x+i);
    }

    template <typename T>
    inline T &TVector4<T>::operator [](int32_t i)
    {
        T3D_ASSERT(i < 4, "The i is out of bound !");
        return *(&_x+i);
    }

    template <typename T>
    inline T TVector4<T>::x() const
    {
        return _x;
    }

    template <typename T>
    inline T &TVector4<T>::x()
    {
        return _x;
    }

    template <typename T>
    inline T TVector4<T>::y() const
    {
        return _y;
    }

    template <typename T>
    inline T &TVector4<T>::y()
    {
        return _y;
    }

    template <typename T>
    inline T TVector4<T>::z() const
    {
        return _z;
    }

    template <typename T>
    inline T &TVector4<T>::z()
    {
        return _z;
    }

    template <typename T>
    inline T TVector4<T>::w() const
    {
        return _w;
    }

    template <typename T>
    inline T &TVector4<T>::w()
    {
        return _w;
    }

    template <typename T>
    inline TVector4<T> &TVector4<T>::operator =(const TVector4 &other)
    {
        _x = other._x;
        _y = other._y;
        _z = other._z;
        _w = other._w;
        return *this;
    }

    template <typename T>
    inline bool TVector4<T>::operator ==(const TVector4 &other) const
    {
        return (_x == other._x && _y == other._y && _z == other._z && _w == other._w);
    }

    template <typename T>
    inline bool TVector4<T>::operator !=(const TVector4 &other) const
    {
        return (_x != other._x || _y != other._y || _z != other._z || _w != other._w);
    }

    template <typename T>
    inline TVector4<T> TVector4<T>::operator +(const TVector4 &other) const
    {
        return TVector4(_x + other._x, _y + other._y, _z + other._z, _w + other._w);
    }

    template <typename T>
    inline TVector4<T> TVector4<T>::operator -(const TVector4 &other) const
    {
        return TVector4(_x - other._x, _y - other._y, _z - other._z, _w - other._w);
    }

    template <typename T>
    inline TVector4<T> TVector4<T>::operator *(T scalar) const
    {
        return TVector4(scalar * _x, scalar * _y, scalar * _z, scalar * _w);
    }

    template <typename T>
    inline TVector4<T> TVector4<T>::operator /(T scalar) const
    {
        TVector4 result;

        if (scalar != TReal<T>::ZERO)
        {
            result._x = _x / scalar;
            result._y = _y / scalar;
            result._z = _z / scalar;
            result._w = _w / scalar;
        }

        return result;
    }

    template <typename T>
    inline TVector4<T> TVector4<T>::operator -() const
    {
        return TVector4(-_x, -_y, -_z, -_w);
    }

    template <typename T>
    inline TVector4<T> &TVector4<T>::operator +=(const TVector4 &other)
    {
        _x += other._x;
        _y += other._y;
        _z += other._z;
        _w += other._w;
        return *this;
    }

    template <typename T>
    inline TVector4<T> &TVector4<T>::operator -=(const TVector4 &other)
    {
        _x -= other._x;
        _y -= other._y;
        _z -= other._z;
        _w -= other._w;
        return *this;
    }

    template <typename T>
    inline TVector4<T> &TVector4<T>::operator *=(T fScalar)
    {
        _x *= fScalar;
        _y *= fScalar;
        _z *= fScalar;
        _w *= fScalar;
        return *this;
    }

    template <typename T>
    inline TVector4<T> &TVector4<T>::operator /=(T fScalar)
    {
        if (fScalar != TReal<T>::ZERO)
        {
            _x /= fScalar;
            _y /= fScalar;
            _z /= fScalar;
            _w /= fScalar;
        }
        else
        {
            _x = TReal<T>::ZERO;
            _y = TReal<T>::ZERO;
            _z = TReal<T>::ZERO;
            _w = TReal<T>::ZERO;
        }
        return *this;
    }

    template <typename T>
    inline T TVector4<T>::length() const
    {
        return TMath<T>::sqrt(_x * _x + _y * _y + _z * _z + _w * _w);
    }

    template <typename T>
    inline T TVector4<T>::length2() const
    {
        return _x * _x + _y * _y + _z * _z + _w * _w;
    }

    template <typename T>
    inline T TVector4<T>::distance(const TVector4 &other) const
    {
        return (*this - other).length();
    }

    template <typename T>
    inline T TVector4<T>::distance2(const TVector4 &other) const
    {
        return (*this - other).length2();
    }

    template <typename T>
    inline T TVector4<T>::dot(const TVector4 &other) const
    {
        return (_x * other._x + _y * other._y + _z * other._z + _w * other._w);
    }

    template <typename T>
    inline TVector4<T> TVector4<T>::cross(const TVector4 &other) const
    {
        return TVector4(_y * other._z - _z * other._y,
            _z * other._w - _w * other._z,
            _w * other._x - _x * other._w,
            _x * other._y - _y * other._x);
    }

    template <typename T>
    inline T TVector4<T>::normalize()
    {
        T fLength = length();

        if (fLength > TReal<T>::ZERO)
        {
            _x /= fLength;
            _y /= fLength;
            _z /= fLength;
            _w /= fLength;
        }
        else
        {
            fLength = TReal<T>::ZERO;
            _x = TReal<T>::ZERO;
            _y = TReal<T>::ZERO;
            _z = TReal<T>::ZERO;
            _w = TReal<T>::ZERO;
        }

        return fLength;
    }

    template <typename T>
    const TVector4<T> TVector4<T>::ZERO(0.0, 0.0, 0.0, 0.0);
}
