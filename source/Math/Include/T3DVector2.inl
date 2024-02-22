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
    inline TVector2<T>::TVector2()
        : _x(0.0)
        , _y(0.0)
    {

    }

    template <typename T>
    inline TVector2<T>::TVector2(T x, T y)
        : _x(x)
        , _y(y)
    {

    }

    template <typename T>
    inline TVector2<T>::TVector2(const TVector2 &other)
        : _x(other._x)
        , _y(other._y)
    {

    }

    template <typename T>
    inline TVector2<T>::operator const T *() const
    {
        return &_x;
    }

    template <typename T>
    inline TVector2<T>::operator T *()
    {
        return &_x;
    }

    template <typename T>
    inline T TVector2<T>::operator [](int32_t i) const
    {
        T3D_ASSERT(i < 2, "The i is out of bound !");
        return *(&_x+i);
    }

    template <typename T>
    inline T &TVector2<T>::operator [](int32_t i)
    {
        T3D_ASSERT(i < 2, "The i is out of bound !");
        return *(&_x+i);
    }

    template <typename T>
    inline T TVector2<T>::x() const
    {
        return _x;
    }

    template <typename T>
    inline T &TVector2<T>::x()
    {
        return _x;
    }

    template <typename T>
    inline T TVector2<T>::y() const
    {
        return _y;
    }

    template <typename T>
    inline T &TVector2<T>::y()
    {
        return _y;
    }

    template <typename T>
    inline TVector2<T> &TVector2<T>::operator =(const TVector2 &other)
    {
        _x = other._x;
        _y = other._y;
        return *this;
    }

    template <typename T>
    inline bool TVector2<T>::operator ==(const TVector2 &other) const
    {
        return (_x == other._x && _y == other._y);
    }

    template <typename T>
    inline bool TVector2<T>::operator !=(const TVector2 &other) const
    {
        return (_x != other._x || _y != other._y);
    }

    template <typename T>
    inline int32_t TVector2<T>::compareArrays(const TVector2 &other) const
    {
        return memcmp(&_x, &other._x, 2*sizeof(T));
    }

    template <typename T>
    inline bool TVector2<T>::operator <(const TVector2 &other) const
    {
        return compareArrays(other) < 0;
    }

    template <typename T>
    inline bool TVector2<T>::operator <=(const TVector2 &other) const
    {
        return compareArrays(other) <= 0;
    }

    template <typename T>
    inline bool TVector2<T>::operator >(const TVector2 &other) const
    {
        return compareArrays(other) > 0;
    }

    template <typename T>
    inline bool TVector2<T>::operator >=(const TVector2 &other) const
    {
        return compareArrays(other) >= 0;
    }

    template <typename T>
    inline TVector2<T> TVector2<T>::operator +(const TVector2 &other) const
    {
        return TVector2(_x + other._x, _y + other._y);
    }

    template <typename T>
    inline TVector2<T> TVector2<T>::operator -(const TVector2 &other) const
    {
        return TVector2(_x - other._x, _y - other._y);
    }

    template <typename T>
    inline TVector2<T> TVector2<T>::operator *(T scalar) const
    {
        return TVector2(scalar * _x, scalar * _y);
    }

    template <typename T>
    inline TVector2<T> TVector2<T>::operator /(T scalar) const
    {
        TVector2 result;

        if (scalar != TReal<T>::ZERO)
        {
            result._x = _x / scalar;
            result._y = _y / scalar;
        }

        return result;
    }

    template <typename T>
    inline TVector2<T> TVector2<T>::operator *(const TVector2 &other) const
    {
        TVector2 v;
        v.x() = _x * other._x;
        v.y() = _y * other._y;
        return v;
    }

    template <typename T>
    inline TVector2<T> TVector2<T>::operator -() const
    {
        return TVector2(-_x, -_y);
    }

    template <typename T>
    inline TVector2<T> &TVector2<T>::operator +=(const TVector2 &other)
    {
        _x += other._x;
        _y += other._y;
        return *this;
    }

    template <typename T>
    inline TVector2<T> &TVector2<T>::operator -=(const TVector2 &other)
    {
        _x -= other._x;
        _y -= other._y;
        return *this;
    }

    template <typename T>
    inline TVector2<T> &TVector2<T>::operator *=(T scalar)
    {
        _x *= scalar;
        _y *= scalar;
        return *this;
    }

    template <typename T>
    inline TVector2<T> &TVector2<T>::operator *=(const TVector2 &other)
    {
        _x *= other._x;
        _y *= other._y;
        return *this;
    }

    template <typename T>
    inline TVector2<T> &TVector2<T>::operator /=(T scalar)
    {
        if (scalar != TReal<T>::ZERO)
        {
            _x = _x / scalar;
            _y = _y / scalar;
        }
        else
        {
            _x = TReal<T>::ZERO;
            _y = TReal<T>::ZERO;
        }

        return *this;
    }

    template <typename T>
    inline T TVector2<T>::length() const
    {
        return TMath<T>::sqrt(_x * _x + _y * _y);
    }

    template <typename T>
    inline T TVector2<T>::length2() const
    {
        return _x * _x + _y * _y;
    }

    template <typename T>
    inline T TVector2<T>::distance(const TVector2 &other) const
    {
        return (*this - other).length();
    }

    template <typename T>
    inline T TVector2<T>::distance2(const TVector2 &other) const
    {
        return (*this - other).length2();
    }

    template <typename T>
    inline T TVector2<T>::dot(const TVector2 &other) const
    {
        return (_x * other._x + _y * other._y);
    }

    template <typename T>
    inline T TVector2<T>::cross(const TVector2 &other) const
    {
        return (_x * other._y - _y * other._x);
    }

    template <typename T>
    inline T TVector2<T>::normalize()
    {
        T len = length();

        if (len > TReal<T>::ZERO)
        {
            _x /= len;
            _y /= len;
        }
        else
        {
            len = TReal<T>::ZERO;
            _x = TReal<T>::ZERO;
            _y = TReal<T>::ZERO;
        }

        return len;
    }

    template <typename T>
    inline TVector2<T> TVector2<T>::perp() const
    {
        return TVector2(_x, -_y);
    }

    template <typename T>
    const TVector2<T> TVector2<T>::ZERO(0.0, 0.0);

    template <typename T>
    const TVector2<T> TVector2<T>::UNIT_X(1.0, 0.0);

    template <typename T>
    const TVector2<T> TVector2<T>::UNIT_Y(0.0, 1.0);

    template <typename T>
    const TVector2<T> TVector2<T>::NEGATIVE_UNIT_X(-1.0, 0.0);

    template <typename T>
    const TVector2<T> TVector2<T>::NEGATIVE_UNIT_Y(0.0, -1.0);
}
