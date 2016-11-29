

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

	inline Vector3 Vector3::operator *(Real fScalar) const
	{
		return Vector3(fScalar * _x, fScalar * _y, fScalar * _z);
	}

	inline Vector3 Vector3::operator /(Real fScalar) const
	{
		Vector3 result;

		if (fScalar != 0.0)
		{
			result._x = _x / fScalar;
			result._y = _y / fScalar;
			result._z = _z / fScalar;
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


	inline Vector3 &Vector3::operator /=(Real fScalar)
	{
		if (fScalar != 0.0)
		{
			_x /= fScalar;
			_y /= fScalar;
			_z /= fScalar;
		}
		else
		{
			_x = 0.0;
			_y = 0.0;
			_z = 0.0;
		}

		return *this;
	}

	inline Real Vector3::length() const
	{
		return Math::Sqrt(_x * _x + _y * _y + _z * _z);
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
		Real fLength = length();

		if (fLength > 0.0)
		{
			_x /= fLength;
			_y /= fLength;
			_z /= fLength;
		}
		else
		{
			fLength = 0.0;
			_x = 0.0;
			_y = 0.0;
			_z = 0.0;
		}

		return fLength;
	}

    inline Vector3 operator *(Real fScalar, const Vector3 &rkV)
    {
        return Vector3(fScalar * rkV.x(), fScalar * rkV.y(), fScalar * rkV.z());
    }
}
