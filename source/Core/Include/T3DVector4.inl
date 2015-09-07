

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

inline Real Vector4::operator [](int32_t i)
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

bool Vector4::operator ==(const Vector4 &other) const
{
	return (_x == other._x && _y == other._y && _z == other._z && _w == other._w);
}
