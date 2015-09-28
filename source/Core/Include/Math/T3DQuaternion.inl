

namespace Tiny3D
{
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

	inline Quaternion &Quaternion::Quaternion(const Quaternion &other)
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
}
