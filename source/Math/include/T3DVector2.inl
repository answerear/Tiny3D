

namespace Tiny3D
{
	inline Vector2::Vector2()
		: _x(0.0)
		, _y(0.0)
	{

	}

	inline Vector2::Vector2(Real x, Real y)
		: _x(x)
		, _y(y)
	{

	}

	inline Vector2::Vector2(const Vector2 &other)
		: _x(other._x)
		, _y(other._y)
	{

	}

	inline Vector2::operator const Real *() const
	{
		return &_x;
	}

	inline Vector2::operator Real *()
	{
		return &_x;
	}

	inline Real Vector2::operator [](int32_t i) const
	{
		T3D_ASSERT(i < 2);
		return *(&_x+i);
	}

	inline Real &Vector2::operator [](int32_t i)
	{
		T3D_ASSERT(i < 2);
		return *(&_x+i);
	}

	inline Real Vector2::x() const
	{
		return _x;
	}

	inline Real &Vector2::x()
	{
		return _x;
	}

	inline Real Vector2::y() const
	{
		return _y;
	}

	inline Real &Vector2::y()
	{
		return _y;
	}

	inline Vector2 &Vector2::operator =(const Vector2 &other)
	{
		_x = other._x;
		_y = other._y;
		return *this;
	}

	inline bool Vector2::operator ==(const Vector2 &other) const
	{
		return (_x == other._x && _y == other._y);
	}

	inline bool Vector2::operator !=(const Vector2 &other) const
	{
		return (_x != other._x || _y != other._y);
	}

	inline int32_t Vector2::compareArrays(const Vector2 &other) const
	{
		return memcmp(&_x, &other._x, 2*sizeof(Real));
	}

	inline bool Vector2::operator <(const Vector2 &other) const
	{
		return compareArrays(other) < 0;
	}

	inline bool Vector2::operator <=(const Vector2 &other) const
	{
		return compareArrays(other) <= 0;
	}

	inline bool Vector2::operator >(const Vector2 &other) const
	{
		return compareArrays(other) > 0;
	}

	inline bool Vector2::operator >=(const Vector2 &other) const
	{
		return compareArrays(other) >= 0;
	}

	inline Vector2 Vector2::operator +(const Vector2 &other) const
	{
		return Vector2(_x + other._x, _y + other._y);
	}

	inline Vector2 Vector2::operator -(const Vector2 &other) const
	{
		return Vector2(_x - other._x, _y - other._y);
	}

	inline Vector2 Vector2::operator *(Real fScalar) const
	{
		return Vector2(fScalar * _x, fScalar * _y);
	}

	inline Vector2 Vector2::operator /(Real fScalar) const
	{
		Vector2 result;

		if (fScalar != 0.0)
		{
			result._x = _x / fScalar;
			result._y = _y / fScalar;
		}

		return result;
	}

    inline Vector2 Vector2::operator *(const Vector2 &other) const
    {
        Vector2 v;
        v.x() = _x * other._x;
        v.y() = _y * other._y;
        return v;
    }

	inline Vector2 Vector2::operator -() const
	{
		return Vector2(-_x, -_y);
	}

	inline Vector2 &Vector2::operator +=(const Vector2 &other)
	{
		_x += other._x;
		_y += other._y;
		return *this;
	}

	inline Vector2 &Vector2::operator -=(const Vector2 &other)
	{
		_x -= other._x;
		_y -= other._y;
		return *this;
	}

	inline Vector2 &Vector2::operator *=(Real fScalar)
	{
		_x *= fScalar;
		_y *= fScalar;
		return *this;
	}

    inline Vector2 &Vector2::operator *=(const Vector2 &other)
    {
        _x *= other._x;
        _y *= other._y;
        return *this;
    }

	inline Vector2 &Vector2::operator /=(Real fScalar)
	{
		if (fScalar != 0.0)
		{
			_x = _x / fScalar;
			_y = _y / fScalar;
		}
		else
		{
			_x = 0.0;
			_y = 0.0;
		}

		return *this;
	}

	inline Real Vector2::length() const
	{
		return Math::Sqrt(_x * _x + _y * _y);
	}

	inline Real Vector2::squaredLength() const
	{
		return _x * _x + _y * _y;
	}

	inline Real Vector2::distance(const Vector2 &other) const
	{
		return (*this - other).length();
	}

	inline Real Vector2::squaredDistance(const Vector2 &other) const
	{
		return (*this - other).squaredLength();
	}

	inline Real Vector2::dot(const Vector2 &other) const
	{
		return (_x * other._x + _y * other._y);
	}

	inline Real Vector2::cross(const Vector2 &other) const
	{
		return (_x * other._y - _y * other._x);
	}

	inline Real Vector2::normalize()
	{
		Real fLength = length();

		if (fLength > 0.0)
		{
			_x /= fLength;
			_y /= fLength;
		}
		else
		{
			fLength = 0.0f;
			_x = 0.0;
			_y = 0.0;
		}

		return fLength;
	}

	inline Vector2 Vector2::perp() const
	{
		return Vector2(_x, -_y);
	}
}
