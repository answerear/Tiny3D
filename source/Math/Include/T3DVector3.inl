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
    inline TVector3<T>::TVector3()
        : _x(0.0)
        , _y(0.0)
        , _z(0.0)
    {

    }

    template <typename T>
    inline TVector3<T>::TVector3(T x, T y, T z)
        : _x(x)
        , _y(y)
        , _z(z)
    {

    }

    template <typename T>
    inline TVector3<T>::TVector3(const TVector3 &other)
        : _x(other._x)
        , _y(other._y)
        , _z(other._z)
    {

    }

    template <typename T>
    inline TVector3<T>::operator const T *() const
    {
        return &_x;
    }

    template <typename T>
    inline TVector3<T>::operator T *()
    {
        return &_x;
    }

    template <typename T>
    inline T TVector3<T>::operator [](int32_t i) const
    {
        T3D_ASSERT(i < 3);
        return *(&_x+i);
    }

    template <typename T>
    inline T &TVector3<T>::operator [](int32_t i)
    {
        T3D_ASSERT(i < 3);
        return *(&_x+i);
    }

    template <typename T>
    inline T TVector3<T>::x() const
    {
        return _x;
    }

    template <typename T>
    inline T &TVector3<T>::x()
    {
        return _x;
    }

    template <typename T>
    inline T TVector3<T>::y() const
    {
        return _y;
    }

    template <typename T>
    inline T &TVector3<T>::y()
    {
        return _y;
    }

    template <typename T>
    inline T TVector3<T>::z() const
    {
        return _z;
    }

    template <typename T>
    inline T &TVector3<T>::z()
    {
        return _z;
    }

    template <typename T>
    inline TVector3<T> &TVector3<T>::operator =(const TVector3 &other)
    {
        _x = other._x;
        _y = other._y;
        _z = other._z;
        return *this;
    }

    template <typename T>
    inline bool TVector3<T>::operator ==(const TVector3 &other) const
    {
        return (_x == other._x && _y == other._y && _z == other._z);
    }

    template <typename T>
    inline bool TVector3<T>::operator !=(const TVector3 &other) const
    {
        return (_x != other._x || _y != other._y || _z != other._z);
    }

    template <typename T>
    inline int32_t TVector3<T>::compareArrays(const TVector3 &other) const
    {
        return memcmp(&_x, &other._x, 3*sizeof(T));
    }

    template <typename T>
    inline bool TVector3<T>::operator <(const TVector3 &other) const
    {
        return compareArrays(other) < 0;
    }

    template <typename T>
    inline bool TVector3<T>::operator <=(const TVector3 &other) const
    {
        return compareArrays(other) <= 0;
    }

    template <typename T>
    inline bool TVector3<T>::operator >(const TVector3 &other) const
    {
        return compareArrays(other) > 0;
    }

    template <typename T>
    inline bool TVector3<T>::operator >=(const TVector3 &other) const
    {
        return compareArrays(other) >= 0;
    }

    template <typename T>
    inline TVector3<T> TVector3<T>::operator +(const TVector3 &other) const
    {
        return TVector3(_x + other._x, _y + other._y, _z + other._z);
    }

    template <typename T>
    inline TVector3<T> TVector3<T>::operator -(const TVector3 &other) const
    {
        return TVector3(_x - other._x, _y - other._y, _z - other._z);
    }

    template <typename T>
    inline TVector3<T> TVector3<T>::operator *(T scalar) const
    {
        return TVector3(scalar * _x, scalar * _y, scalar * _z);
    }

    template <typename T>
    inline TVector3<T> TVector3<T>::operator /(T scalar) const
    {
        TVector3 result;

        if (scalar != TReal<T>::ZERO)
        {
            result._x = _x / scalar;
            result._y = _y / scalar;
            result._z = _z / scalar;
        }

        return result;
    }

    template <typename T>
    inline TVector3<T> TVector3<T>::operator *(const TVector3 &other) const
    {
        TVector3 v;
        v.x() = _x * other._x;
        v.y() = _y * other._y;
        v.z() = _z * other._z;
        return v;
    }

    template <typename T>
    inline TVector3<T> TVector3<T>::operator -() const
    {
        return TVector3(-_x, -_y, -_z);
    }

    template <typename T>
    inline TVector3<T> &TVector3<T>::operator +=(const TVector3 &other)
    {
        _x += other._x;
        _y += other._y;
        _z += other._z;
        return *this;
    }

    template <typename T>
    inline TVector3<T> &TVector3<T>::operator -=(const TVector3 &other)
    {
        _x -= other._x;
        _y -= other._y;
        _z -= other._z;
        return *this;
    }

    template <typename T>
    inline TVector3<T> &TVector3<T>::operator *=(T scalar)
    {
        _x *= scalar;
        _y *= scalar;
        _z *= scalar;
        return *this;
    }

    template <typename T>
    inline TVector3<T> &TVector3<T>::operator *=( const TVector3 &rkVector)
    {
        _x *= rkVector._x;
        _y *= rkVector._y;
        _z *= rkVector._z;
        return *this;
    }

    template <typename T>
    inline TVector3<T> &TVector3<T>::operator /=(T scalar)
    {
        if (scalar != TReal<T>::ZERO)
        {
            _x /= scalar;
            _y /= scalar;
            _z /= scalar;
        }
        else
        {
            _x = TReal<T>::ZERO;
            _y = TReal<T>::ZERO;
            _z = TReal<T>::ZERO;
        }

        return *this;
    }

    template <typename T>
    inline T TVector3<T>::length() const
    {
        return TMath<T>::sqrt(_x * _x + _y * _y + _z * _z);
    }

    template <typename T>
    inline T TVector3<T>::length2() const
    {
        return (_x * _x + _y * _y + _z * _z);
    }

    template <typename T>
    inline T TVector3<T>::distance(const TVector3 &other) const
    {
        return (*this - other).length();
    }

    template <typename T>
    inline T TVector3<T>::distance2(const TVector3 &other) const
    {
        return (*this - other).length2();
    }

    template <typename T>
    inline T TVector3<T>::dot(const TVector3 &other) const
    {
        return (_x * other._x + _y * other._y + _z * other._z);
    }

    template <typename T>
    inline TVector3<T> TVector3<T>::cross(const TVector3 &other) const
    {
        return TVector3(_y * other._z - _z * other._y,
            _z * other._x - _x * other._z,
            _x * other._y - _y * other._x);
    }

    template <typename T>
    inline T TVector3<T>::normalize()
    {
        T len = length();

        if (len > TReal<T>::ZERO)
        {
            _x /= len;
            _y /= len;
            _z /= len;
        }
        else
        {
            len = TReal<T>::ZERO;
            _x = TReal<T>::ZERO;
            _y = TReal<T>::ZERO;
            _z = TReal<T>::ZERO;
        }

        return len;
    }

    template <typename T>
    inline TVector3<T> &TVector3<T>::lerp(const TVector3 &start, const TVector3 &end, T t)
    {
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        *this = start * (1.0f - t) + end * t;
        return *this;
    }

    template <typename T>
    inline String TVector3<T>::getDebugString() const
    {
        std::stringstream ss;
        ss << "(" << _x << ", " << _y << ", " << _z << ")";
        return ss.str();
    }
    
    template <typename T>
    inline TVector3<T> operator *(T scalar, const TVector3<T> &rkV)
    {
        return TVector3<T>(scalar * rkV.x(), scalar * rkV.y(), scalar * rkV.z());
    }

    template <typename T>
    const TVector3<T> TVector3<T>::ZERO(0.0, 0.0, 0.0);

    template <typename T>
    const TVector3<T> TVector3<T>::UNIT_SCALE(1.0, 1.0, 1.0);

    template <typename T>
    const TVector3<T> TVector3<T>::UNIT_X(1.0, 0.0, 0.0);

    template <typename T>
    const TVector3<T> TVector3<T>::UNIT_Y(0.0, 1.0, 0.0);

    template <typename T>
    const TVector3<T> TVector3<T>::UNIT_Z(0.0, 0.0, 1.0);

    template <typename T>
    const TVector3<T> TVector3<T>::NEGATIVE_UNIT_X(-1.0, 0.0, 0.0);

    template <typename T>
    const TVector3<T> TVector3<T>::NEGATIVE_UNIT_Y(0.0, -1.0, 0.0);

    template <typename T>
    const TVector3<T> TVector3<T>::NEGATIVE_UNIT_Z(0.0, 0.0, -1.0);

    template <typename T>
    const TVector3<T> TVector3<T>::RIGHT(1.0f, 0.0f, 0.0f);

    template <typename T>
    const TVector3<T> TVector3<T>::UP(0.0f, 1.0f, 0.0f);

    template <typename T>
    const TVector3<T> TVector3<T>::FORWARD(0.0, 0.0, 1.0);
}
