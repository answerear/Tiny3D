

namespace Tiny3D
{
	inline void Matrix4::makeIdentity()
	{
		_m4x4[0][0] = 1.0;
		_m4x4[0][1] = 0.0;
		_m4x4[0][2] = 0.0;
		_m4x4[0][3] = 0.0;
		_m4x4[1][0] = 0.0;
		_m4x4[1][1] = 1.0;
		_m4x4[1][2] = 0.0;
		_m4x4[1][3] = 0.0;
		_m4x4[2][0] = 0.0;
		_m4x4[2][1] = 0.0;
		_m4x4[2][2] = 1.0;
		_m4x4[2][3] = 0.0;
		_m4x4[3][0] = 0.0;
		_m4x4[3][1] = 0.0;
		_m4x4[3][2] = 0.0;
		_m4x4[3][3] = 1.0;
	}

	inline void Matrix4::makeZero()
	{
		_m4x4[0][0] = _m4x4[0][1] = _m4x4[0][2] = _m4x4[0][3] = 0.0;
		_m4x4[1][0] = _m4x4[1][1] = _m4x4[1][2] = _m4x4[1][3] = 0.0;
		_m4x4[2][0] = _m4x4[2][1] = _m4x4[2][2] = _m4x4[2][3] = 0.0;
		_m4x4[3][0] = _m4x4[3][1] = _m4x4[3][2] = _m4x4[3][3] = 0.0;
	}

	inline Matrix4 &Matrix4::operator =(const Matrix4 &other)
	{
		memcpy(_m16, other._m16, sizeof(_m16));
		return *this;
	}

	inline void Matrix4::operator =(const Matrix3 &rkMat)
	{
		_m4x4[0][0] = rkMat[0][0], _m4x4[0][1] = rkMat[0][1], _m4x4[0][2] = rkMat[0][2];
		_m4x4[1][0] = rkMat[1][0], _m4x4[1][1] = rkMat[1][1], _m4x4[1][2] = rkMat[1][2];
		_m4x4[2][0] = rkMat[2][0], _m4x4[2][1] = rkMat[2][1], _m4x4[2][2] = rkMat[2][2];
	}

	inline Matrix4::Matrix4(bool bZero /* = true */)
	{
		if (bZero)
			makeZero();
		else
			makeIdentity();
	}

	inline Matrix4::Matrix4(const Matrix4 &other)
	{
		memcpy(_m16, other._m16, sizeof(_m16));
	}

	inline Matrix4::Matrix4(
		Real m00, Real m01, Real m02, Real m03, 
		Real m10, Real m11, Real m12, Real m13, 
		Real other0, Real other1, Real other2, Real other3, 
		Real m30, Real m31, Real m32, Real m33)
	{
		_m4x4[0][0] = m00;
		_m4x4[0][1] = m01;
		_m4x4[0][2] = m02;
		_m4x4[0][3] = m03;
		_m4x4[1][0] = m10;
		_m4x4[1][1] = m11;
		_m4x4[1][2] = m12;
		_m4x4[1][3] = m13;
		_m4x4[2][0] = other0;
		_m4x4[2][1] = other1;
		_m4x4[2][2] = other2;
		_m4x4[2][3] = other3;
		_m4x4[3][0] = m30;
		_m4x4[3][1] = m31;
		_m4x4[3][2] = m32;
		_m4x4[3][3] = m33;
	}

	inline Matrix4::Matrix4(const Matrix3 &rkMat)
	{
		operator =(rkMat);
		_m4x4[0][3] = _m4x4[1][3] = _m4x4[2][3] = 0.0;
		_m4x4[3][3] = 1.0;
        _m4x4[3][0] = _m4x4[3][1] = _m4x4[3][2] = 0.0;
	}

	inline Matrix4::Matrix4(const Quaternion &rkRot)
	{
		Matrix3 mat;
		rkRot.toRotationMatrix(mat);
		operator =(mat);
		_m4x4[0][3] = _m4x4[1][3] = _m4x4[2][3] = 0.0;
		_m4x4[3][3] = 1.0;
        _m4x4[3][0] = _m4x4[3][1] = _m4x4[3][2] = 0.0;
	}

    inline Matrix4::Matrix4(const Vector3 &rkPos)
    {
        _m4x4[0][0] = 1.0, _m4x4[0][1] = 0.0, _m4x4[0][2] = 0.0, _m4x4[0][3] = rkPos.x();
        _m4x4[1][0] = 0.0, _m4x4[1][1] = 1.0, _m4x4[1][2] = 0.0, _m4x4[1][3] = rkPos.y();
        _m4x4[2][0] = 0.0, _m4x4[2][1] = 0.0, _m4x4[2][2] = 1.0, _m4x4[2][3] = rkPos.z();
        _m4x4[3][0] = 0.0, _m4x4[3][1] = 0.0, _m4x4[3][2] = 0.0, _m4x4[3][3] = 1.0;
    }

    inline Matrix4::Matrix4(Real m00, Real m11, Real m22, Real m33)
    {
        _m4x4[0][0] = m00, _m4x4[0][1] = 0.0, _m4x4[0][2] = 0.0, _m4x4[0][3] = 0.0;
        _m4x4[1][0] = 0.0, _m4x4[1][1] = m11, _m4x4[1][2] = 0.0, _m4x4[1][3] = 0.0;
        _m4x4[2][0] = 0.0, _m4x4[2][1] = 0.0, _m4x4[2][2] = m22, _m4x4[2][3] = 0.0;
        _m4x4[3][0] = 0.0, _m4x4[3][1] = 0.0, _m4x4[3][2] = 0.0, _m4x4[3][3] = m33;
    }

	inline Matrix4::operator const Real *() const
	{
		return _m16;
	}

	inline Matrix4::operator Real *()
	{
		return _m16;
	}

	inline const Real *Matrix4::operator [](int32_t nRow) const
	{
		T3D_ASSERT(nRow >= 0 && nRow < 4);
		return _m4x4[nRow];
	}

	inline Real *Matrix4::operator [](int32_t nRow)
	{
		T3D_ASSERT(nRow >= 0 && nRow < 4);
		return _m4x4[nRow];
	}

	inline Real Matrix4::operator ()(int32_t nRow, int32_t nCol) const
	{
		return _m4x4[nRow][nCol];
	}

	inline Real &Matrix4::operator ()(int32_t nRow, int32_t nCol)
	{
		return _m4x4[nRow][nCol];
	}

	inline int32_t Matrix4::compareArrays(const Matrix4 &other) const
	{
		return memcmp(_m16, other._m16, sizeof(_m16));
	}

	inline bool Matrix4::operator ==(const Matrix4 &other) const
	{
		return compareArrays(other) == 0;
	}

	inline bool Matrix4::operator !=(const Matrix4 &other) const
	{
		return compareArrays(other) != 0;
	}

	inline bool Matrix4::operator <(const Matrix4 &other) const
	{
		return compareArrays(other) < 0;
	}

	inline bool Matrix4::operator <=(const Matrix4 &other) const
	{
		return compareArrays(other) <= 0;
	}

	inline bool Matrix4::operator >(const Matrix4 &other) const
	{
		return compareArrays(other) > 0;
	}

	inline bool Matrix4::operator >=(const Matrix4 &other) const
	{
		return compareArrays(other) >= 0;
	}

	inline Matrix4 Matrix4::operator +(const Matrix4 &other) const
	{
		return Matrix4(
			_m4x4[0][0] + other._m4x4[0][0], _m4x4[0][1] + other._m4x4[0][1], _m4x4[0][2] + other._m4x4[0][2], _m4x4[0][3] + other._m4x4[0][3],
			_m4x4[1][0] + other._m4x4[1][0], _m4x4[1][1] + other._m4x4[1][1], _m4x4[1][2] + other._m4x4[1][2], _m4x4[1][3] + other._m4x4[1][3],
			_m4x4[2][0] + other._m4x4[2][0], _m4x4[2][1] + other._m4x4[2][1], _m4x4[2][2] + other._m4x4[2][2], _m4x4[2][3] + other._m4x4[2][3],
			_m4x4[3][0] + other._m4x4[3][0], _m4x4[3][1] + other._m4x4[3][1], _m4x4[3][2] + other._m4x4[3][2], _m4x4[3][3] + other._m4x4[3][3]);
	}

	inline Matrix4 Matrix4::operator -(const Matrix4 &other) const
	{
		return Matrix4(
			_m4x4[0][0] - other._m4x4[0][0], _m4x4[0][1] - other._m4x4[0][1], _m4x4[0][2] - other._m4x4[0][2], _m4x4[0][3] - other._m4x4[0][3],
			_m4x4[1][0] - other._m4x4[1][0], _m4x4[1][1] - other._m4x4[1][1], _m4x4[1][2] - other._m4x4[1][2], _m4x4[1][3] - other._m4x4[1][3],
			_m4x4[2][0] - other._m4x4[2][0], _m4x4[2][1] - other._m4x4[2][1], _m4x4[2][2] - other._m4x4[2][2], _m4x4[2][3] - other._m4x4[2][3],
			_m4x4[3][0] - other._m4x4[3][0], _m4x4[3][1] - other._m4x4[3][1], _m4x4[3][2] - other._m4x4[3][2], _m4x4[3][3] - other._m4x4[3][3]
		);
	}

	inline Matrix4 Matrix4::operator *(const Matrix4 &other) const
	{
		return Matrix4(
			_m4x4[0][0] * other._m4x4[0][0] + _m4x4[0][1] * other._m4x4[1][0] + _m4x4[0][2] * other._m4x4[2][0] + _m4x4[0][3] * other._m4x4[3][0],
			_m4x4[0][0] * other._m4x4[0][1] + _m4x4[0][1] * other._m4x4[1][1] + _m4x4[0][2] * other._m4x4[2][1] + _m4x4[0][3] * other._m4x4[3][1],
			_m4x4[0][0] * other._m4x4[0][2] + _m4x4[0][1] * other._m4x4[1][2] + _m4x4[0][2] * other._m4x4[2][2] + _m4x4[0][3] * other._m4x4[3][2],
			_m4x4[0][0] * other._m4x4[0][3] + _m4x4[0][1] * other._m4x4[1][3] + _m4x4[0][2] * other._m4x4[2][3] + _m4x4[0][3] * other._m4x4[3][3],

			_m4x4[1][0] * other._m4x4[0][0] + _m4x4[1][1] * other._m4x4[1][0] + _m4x4[1][2] * other._m4x4[2][0] + _m4x4[1][3] * other._m4x4[3][0],
			_m4x4[1][0] * other._m4x4[0][1] + _m4x4[1][1] * other._m4x4[1][1] + _m4x4[1][2] * other._m4x4[2][1] + _m4x4[1][3] * other._m4x4[3][1],
			_m4x4[1][0] * other._m4x4[0][2] + _m4x4[1][1] * other._m4x4[1][2] + _m4x4[1][2] * other._m4x4[2][2] + _m4x4[1][3] * other._m4x4[3][2],
			_m4x4[1][0] * other._m4x4[0][3] + _m4x4[1][1] * other._m4x4[1][3] + _m4x4[1][2] * other._m4x4[2][3] + _m4x4[1][3] * other._m4x4[3][3],

			_m4x4[2][0] * other._m4x4[0][0] + _m4x4[2][1] * other._m4x4[1][0] + _m4x4[2][2] * other._m4x4[2][0] + _m4x4[2][3] * other._m4x4[3][0],
			_m4x4[2][0] * other._m4x4[0][1] + _m4x4[2][1] * other._m4x4[1][1] + _m4x4[2][2] * other._m4x4[2][1] + _m4x4[2][3] * other._m4x4[3][1],
			_m4x4[2][0] * other._m4x4[0][2] + _m4x4[2][1] * other._m4x4[1][2] + _m4x4[2][2] * other._m4x4[2][2] + _m4x4[2][3] * other._m4x4[3][2],
			_m4x4[2][0] * other._m4x4[0][3] + _m4x4[2][1] * other._m4x4[1][3] + _m4x4[2][2] * other._m4x4[2][3] + _m4x4[2][3] * other._m4x4[3][3],

			_m4x4[3][0] * other._m4x4[0][0] + _m4x4[3][1] * other._m4x4[1][0] + _m4x4[3][2] * other._m4x4[2][0] + _m4x4[3][3] * other._m4x4[3][0],
			_m4x4[3][0] * other._m4x4[0][1] + _m4x4[3][1] * other._m4x4[1][1] + _m4x4[3][2] * other._m4x4[2][1] + _m4x4[3][3] * other._m4x4[3][1],
			_m4x4[3][0] * other._m4x4[0][2] + _m4x4[3][1] * other._m4x4[1][2] + _m4x4[3][2] * other._m4x4[2][2] + _m4x4[3][3] * other._m4x4[3][2],
			_m4x4[3][0] * other._m4x4[0][3] + _m4x4[3][1] * other._m4x4[1][3] + _m4x4[3][2] * other._m4x4[2][3] + _m4x4[3][3] * other._m4x4[3][3]
			);
	}

	inline Matrix4 Matrix4::operator *(Real fScalar) const
	{
		return Matrix4(
			_m4x4[0][0] * fScalar, _m4x4[0][1] * fScalar, _m4x4[0][2] * fScalar, _m4x4[0][3] * fScalar,
			_m4x4[1][0] * fScalar, _m4x4[1][1] * fScalar, _m4x4[1][2] * fScalar, _m4x4[1][3] * fScalar,
			_m4x4[2][0] * fScalar, _m4x4[2][1] * fScalar, _m4x4[2][2] * fScalar, _m4x4[2][3] * fScalar,
			_m4x4[3][0] * fScalar, _m4x4[3][1] * fScalar, _m4x4[3][2] * fScalar, _m4x4[3][3] * fScalar
			);
	}

	inline Matrix4 Matrix4::operator /(Real fScalar) const
	{
		Real fInvertScalar = 0.0;
		if (fScalar != 0.0)
			fInvertScalar = Real(1.0) / fScalar;
		return Matrix4(
			_m4x4[0][0] * fInvertScalar, _m4x4[0][1] * fInvertScalar, _m4x4[0][2] * fInvertScalar, _m4x4[0][3] * fInvertScalar,
			_m4x4[1][0] * fInvertScalar, _m4x4[1][1] * fInvertScalar, _m4x4[1][2] * fInvertScalar, _m4x4[1][3] * fInvertScalar,
			_m4x4[2][0] * fInvertScalar, _m4x4[2][1] * fInvertScalar, _m4x4[2][2] * fInvertScalar, _m4x4[2][3] * fInvertScalar,
			_m4x4[3][0] * fInvertScalar, _m4x4[3][1] * fInvertScalar, _m4x4[3][2] * fInvertScalar, _m4x4[3][3] * fInvertScalar
			);
	}

	inline Matrix4 Matrix4::operator -() const
	{
		return Matrix4(
			-_m4x4[0][0], -_m4x4[0][1], -_m4x4[0][2], -_m4x4[0][3],
			-_m4x4[1][0], -_m4x4[1][1], -_m4x4[1][2], -_m4x4[1][3],
			-_m4x4[2][0], -_m4x4[2][1], -_m4x4[2][2], -_m4x4[2][3],
			-_m4x4[3][0], -_m4x4[3][1], -_m4x4[3][2], -_m4x4[3][3]
			);
	}

	inline Matrix4 &Matrix4::operator +=(const Matrix4 &other)
	{
		_m4x4[0][0] += other._m4x4[0][0];
		_m4x4[0][1] += other._m4x4[0][1];
		_m4x4[0][2] += other._m4x4[0][2];
		_m4x4[0][3] += other._m4x4[0][3];

		_m4x4[1][0] += other._m4x4[1][0];
		_m4x4[1][1] += other._m4x4[1][1];
		_m4x4[1][2] += other._m4x4[1][2];
		_m4x4[1][3] += other._m4x4[1][3];

		_m4x4[2][0] += other._m4x4[2][0];
		_m4x4[2][1] += other._m4x4[2][1];
		_m4x4[2][2] += other._m4x4[2][2];
		_m4x4[2][3] += other._m4x4[2][3];

		_m4x4[3][0] += other._m4x4[3][0];
		_m4x4[3][1] += other._m4x4[3][1];
		_m4x4[3][2] += other._m4x4[3][2];
		_m4x4[3][3] += other._m4x4[3][3];

		return *this;
	}

	inline Matrix4 &Matrix4::operator -=(const Matrix4 &other)
	{
		_m4x4[0][0] -= other._m4x4[0][0];
		_m4x4[0][1] -= other._m4x4[0][1];
		_m4x4[0][2] -= other._m4x4[0][2];
		_m4x4[0][3] -= other._m4x4[0][3];

		_m4x4[1][0] -= other._m4x4[1][0];
		_m4x4[1][1] -= other._m4x4[1][1];
		_m4x4[1][2] -= other._m4x4[1][2];
		_m4x4[1][3] -= other._m4x4[1][3];

		_m4x4[2][0] -= other._m4x4[2][0];
		_m4x4[2][1] -= other._m4x4[2][1];
		_m4x4[2][2] -= other._m4x4[2][2];
		_m4x4[2][3] -= other._m4x4[2][3];

		_m4x4[3][0] -= other._m4x4[3][0];
		_m4x4[3][1] -= other._m4x4[3][1];
		_m4x4[3][2] -= other._m4x4[3][2];
		_m4x4[3][3] -= other._m4x4[3][3];

		return *this;
	}

	inline Matrix4 &Matrix4::operator *=(Real fScalar)
	{
		_m4x4[0][0] *= fScalar, _m4x4[0][1] *= fScalar, _m4x4[0][2] *= fScalar, _m4x4[0][3] *= fScalar;
		_m4x4[1][0] *= fScalar, _m4x4[1][1] *= fScalar, _m4x4[1][2] *= fScalar, _m4x4[1][3] *= fScalar;
		_m4x4[2][0] *= fScalar, _m4x4[2][1] *= fScalar, _m4x4[2][2] *= fScalar, _m4x4[2][3] *= fScalar;
		_m4x4[3][0] *= fScalar, _m4x4[3][1] *= fScalar, _m4x4[3][2] *= fScalar, _m4x4[3][3] *= fScalar;

		return *this;
	}

	inline Matrix4 &Matrix4::operator /=(Real fScalar)
	{
		Real fInvertScalar = 0.0;
		if (fScalar != 0.0)
			fInvertScalar = Real(1.0) / fScalar;
		
		_m4x4[0][0] *= fInvertScalar, _m4x4[0][1] *= fInvertScalar, _m4x4[0][2] *= fInvertScalar, _m4x4[0][3] *= fInvertScalar;
		_m4x4[1][0] *= fInvertScalar, _m4x4[1][1] *= fInvertScalar, _m4x4[1][2] *= fInvertScalar, _m4x4[1][3] *= fInvertScalar;
		_m4x4[2][0] *= fInvertScalar, _m4x4[2][1] *= fInvertScalar, _m4x4[2][2] *= fInvertScalar, _m4x4[2][3] *= fInvertScalar;
		_m4x4[3][0] *= fInvertScalar, _m4x4[3][1] *= fInvertScalar, _m4x4[3][2] *= fInvertScalar, _m4x4[3][3] *= fInvertScalar;

		return *this;
	}

	inline Vector4 Matrix4::operator *(const Vector4 &rkV) const
	{
		return Vector4(
			_m4x4[0][0] * rkV.x() + _m4x4[0][1] * rkV.y() + _m4x4[0][2] * rkV.z() + _m4x4[0][3] * rkV.w(), 
			_m4x4[1][0] * rkV.x() + _m4x4[1][1] * rkV.y() + _m4x4[1][2] * rkV.z() + _m4x4[1][3] * rkV.w(),
			_m4x4[2][0] * rkV.x() + _m4x4[2][1] * rkV.y() + _m4x4[2][2] * rkV.z() + _m4x4[2][3] * rkV.w(),
			_m4x4[3][0] * rkV.x() + _m4x4[3][1] * rkV.y() + _m4x4[3][2] * rkV.z() + _m4x4[3][3] * rkV.w()
			);
	}

    inline Vector3 Matrix4::operator *(const Vector3 &rkV) const
    {
        Real w = _m4x4[3][0] * rkV.x() + _m4x4[3][1] * rkV.y() + _m4x4[3][2] * rkV.z() + _m4x4[3][3] * Real(1.0);

        if (w != Real(0.0))
        {
            return Vector3(
                (_m4x4[0][0] * rkV.x() + _m4x4[0][1] * rkV.y() + _m4x4[0][2] * rkV.z() + _m4x4[0][3] * Real(1.0)) / w, 
                (_m4x4[1][0] * rkV.x() + _m4x4[1][1] * rkV.y() + _m4x4[1][2] * rkV.z() + _m4x4[1][3] * Real(1.0)) / w,
                (_m4x4[2][0] * rkV.x() + _m4x4[2][1] * rkV.y() + _m4x4[2][2] * rkV.z() + _m4x4[2][3] * Real(1.0)) / w
                );
        }
        
        return Vector3(
            _m4x4[0][0] * rkV.x() + _m4x4[0][1] * rkV.y() + _m4x4[0][2] * rkV.z() + _m4x4[0][3] * Real(1.0),
            _m4x4[1][0] * rkV.x() + _m4x4[1][1] * rkV.y() + _m4x4[1][2] * rkV.z() + _m4x4[1][3] * Real(1.0),
            _m4x4[2][0] * rkV.x() + _m4x4[2][1] * rkV.y() + _m4x4[2][2] * rkV.z() + _m4x4[2][3] * Real(1.0)
            );
    }

	inline Matrix4 Matrix4::transpose() const
	{
		return Matrix4(
			_m4x4[0][0], _m4x4[1][0], _m4x4[2][0], _m4x4[3][0],
			_m4x4[0][1], _m4x4[1][1], _m4x4[2][1], _m4x4[3][1],
			_m4x4[0][2], _m4x4[1][2], _m4x4[2][2], _m4x4[3][2],
			_m4x4[0][3], _m4x4[1][3], _m4x4[2][3], _m4x4[3][3]
			);
	}
	inline bool Matrix4::isAffine() const
	{
		return _m4x4[3][0] == 0.0 && _m4x4[3][1] == 0.0 && _m4x4[3][2] == 0.0 && _m4x4[3][3] == 1.0;
	}

	inline bool Matrix4::hasScale() const
	{
		Real t = _m4x4[0][0] * _m4x4[0][0] + _m4x4[1][0] * _m4x4[1][0] + _m4x4[2][0] * _m4x4[2][0];
		if (!Math::RealEqual(t, 1.0, (Real)1e-04))
			return true;
		t = _m4x4[0][1] * _m4x4[0][1] + _m4x4[1][1] * _m4x4[1][1] + _m4x4[2][1] * _m4x4[2][1];
		if (!Math::RealEqual(t, 1.0, (Real)1e-04))
			return true;
		t = _m4x4[0][2] * _m4x4[0][2] + _m4x4[1][2] * _m4x4[1][2] + _m4x4[2][2] * _m4x4[2][2];
		if (!Math::RealEqual(t, 1.0, (Real)1e-04))
			return true;

		return false;
	}

	inline Matrix4 Matrix4::concatenateAffine(const Matrix4 &other) const
	{
		T3D_ASSERT(isAffine() && other.isAffine());

		return Matrix4(
			_m4x4[0][0] * other._m4x4[0][0] + _m4x4[0][1] * other._m4x4[1][0] + _m4x4[0][2] * other._m4x4[2][0],
			_m4x4[0][0] * other._m4x4[0][1] + _m4x4[0][1] * other._m4x4[1][1] + _m4x4[0][2] * other._m4x4[2][1],
			_m4x4[0][0] * other._m4x4[0][2] + _m4x4[0][1] * other._m4x4[1][2] + _m4x4[0][2] * other._m4x4[2][2],
			_m4x4[0][0] * other._m4x4[0][3] + _m4x4[0][1] * other._m4x4[1][3] + _m4x4[0][2] * other._m4x4[2][3] + _m4x4[0][3],

			_m4x4[1][0] * other._m4x4[0][0] + _m4x4[1][1] * other._m4x4[1][0] + _m4x4[1][2] * other._m4x4[2][0],
			_m4x4[1][0] * other._m4x4[0][1] + _m4x4[1][1] * other._m4x4[1][1] + _m4x4[1][2] * other._m4x4[2][1],
			_m4x4[1][0] * other._m4x4[0][2] + _m4x4[1][1] * other._m4x4[1][2] + _m4x4[1][2] * other._m4x4[2][2],
			_m4x4[1][0] * other._m4x4[0][3] + _m4x4[1][1] * other._m4x4[1][3] + _m4x4[1][2] * other._m4x4[2][3] + _m4x4[1][3],

			_m4x4[2][0] * other._m4x4[0][0] + _m4x4[2][1] * other._m4x4[1][0] + _m4x4[2][2] * other._m4x4[2][0],
			_m4x4[2][0] * other._m4x4[0][1] + _m4x4[2][1] * other._m4x4[1][1] + _m4x4[2][2] * other._m4x4[2][1],
			_m4x4[2][0] * other._m4x4[0][2] + _m4x4[2][1] * other._m4x4[1][2] + _m4x4[2][2] * other._m4x4[2][2],
			_m4x4[2][0] * other._m4x4[0][3] + _m4x4[2][1] * other._m4x4[1][3] + _m4x4[2][2] * other._m4x4[2][3] + _m4x4[2][3],

			0.0, 0.0, 0.0, 1.0);
	}

	inline void Matrix4::extractMatrix(Matrix3 &m3x3) const
	{
		m3x3[0][0] = _m4x4[0][0];
		m3x3[0][1] = _m4x4[0][1];
		m3x3[0][2] = _m4x4[0][2];
		m3x3[1][0] = _m4x4[1][0];
		m3x3[1][1] = _m4x4[1][1];
		m3x3[1][2] = _m4x4[1][2];
		m3x3[2][0] = _m4x4[2][0];
		m3x3[2][1] = _m4x4[2][1];
		m3x3[2][2] = _m4x4[2][2];
	}

	inline Quaternion Matrix4::extractQuaternion() const
	{
		Matrix3 m3x3;
		extractMatrix(m3x3);
		return Quaternion(m3x3);
	}

	inline Vector3 Matrix4::extractTranslation() const
	{
		return Vector3(_m4x4[0][3], _m4x4[1][3], _m4x4[2][3]);
	}

	inline void Matrix4::decomposition(Vector3 &position, Vector3 &scale, Quaternion &orientation) const
	{
		T3D_ASSERT(isAffine());

		Matrix3 m3x3;
		extractMatrix(m3x3);

		Matrix3 matQ;
		Vector3 vecU;
		m3x3.QDUDecomposition(matQ, scale, vecU); 

		orientation = Quaternion( matQ );
		position = Vector3(_m4x4[0][3], _m4x4[1][3], _m4x4[2][3]);
	}

	inline void Matrix4::makeTranslate(const Vector3 &position)
	{
		_m4x4[0][0] = 1.0, _m4x4[0][1] = 0.0, _m4x4[0][2] = 0.0, _m4x4[0][3] = position.x();
		_m4x4[1][0] = 0.0, _m4x4[1][1] = 1.0, _m4x4[1][2] = 0.0, _m4x4[1][3] = position.y();
		_m4x4[2][0] = 0.0, _m4x4[2][1] = 0.0, _m4x4[2][2] = 1.0, _m4x4[2][3] = position.z();
		_m4x4[3][0] = 0.0, _m4x4[3][1] = 0.0, _m4x4[3][2] = 0.0, _m4x4[3][3] = 1.0;
	}

	inline void Matrix4::makeTranslate(Real tx, Real ty, Real tz)
	{
		_m4x4[0][0] = 1.0, _m4x4[0][1] = 0.0, _m4x4[0][2] = 0.0, _m4x4[0][3] = tx;
		_m4x4[1][0] = 0.0, _m4x4[1][1] = 1.0, _m4x4[1][2] = 0.0, _m4x4[1][3] = ty;
		_m4x4[2][0] = 0.0, _m4x4[2][1] = 0.0, _m4x4[2][2] = 1.0, _m4x4[2][3] = tz;
		_m4x4[3][0] = 0.0, _m4x4[3][1] = 0.0, _m4x4[3][2] = 0.0, _m4x4[3][3] = 1.0;
	}

	inline void Matrix4::setScale(const Vector3 &scale)
	{
		_m4x4[0][0] = scale.x();
		_m4x4[1][1] = scale.y();
		_m4x4[2][2] = scale.z();
	}

	inline void Matrix4::setTranslate(const Vector3 &position)
	{
		_m4x4[0][3] = position.x();
		_m4x4[1][3] = position.y();
		_m4x4[2][3] = position.z();
	}

	inline Vector3 Matrix4::transformAffine(const Vector3 &v) const
	{
		T3D_ASSERT(isAffine());

		return Vector3(
			_m4x4[0][0] * v.x() + _m4x4[0][1] * v.y() + _m4x4[0][2] * v.z() + _m4x4[0][3], 
			_m4x4[1][0] * v.x() + _m4x4[1][1] * v.y() + _m4x4[1][2] * v.z() + _m4x4[1][3],
			_m4x4[2][0] * v.x() + _m4x4[2][1] * v.y() + _m4x4[2][2] * v.z() + _m4x4[2][3]);
	}

	inline Vector4 Matrix4::transformAffine(const Vector4 &v) const
	{
		T3D_ASSERT(isAffine());

		return Vector4(
			_m4x4[0][0] * v.x() + _m4x4[0][1] * v.y() + _m4x4[0][2] * v.z() + _m4x4[0][3] * v.w(), 
			_m4x4[1][0] * v.x() + _m4x4[1][1] * v.y() + _m4x4[1][2] * v.z() + _m4x4[1][3] * v.w(),
			_m4x4[2][0] * v.x() + _m4x4[2][1] * v.y() + _m4x4[2][2] * v.z() + _m4x4[2][3] * v.w(),
			v.w());
	}


	inline Matrix4 operator *(Real fScalar, const Matrix4 &rkM)
	{
		return Matrix4(
			fScalar * rkM[0][0], fScalar * rkM[0][1], fScalar * rkM[0][2], fScalar * rkM[0][3],
			fScalar * rkM[1][0], fScalar * rkM[1][1], fScalar * rkM[1][2], fScalar * rkM[1][3],
			fScalar * rkM[2][0], fScalar * rkM[2][1], fScalar * rkM[2][2], fScalar * rkM[2][3],
			fScalar * rkM[3][0], fScalar * rkM[3][1], fScalar * rkM[3][2], fScalar * rkM[3][3]
			);
	}

	inline Vector4 operator *(const Vector4 &rkV, const Matrix4 &rkM)
	{
		return Vector4(
			rkV.x() * rkM[0][0] + rkV.y() * rkM[1][0] + rkV.z() * rkM[2][0] + rkV.w() * rkM[3][0],
			rkV.x() * rkM[0][1] + rkV.y() * rkM[1][1] + rkV.z() * rkM[2][1] + rkV.w() * rkM[3][1],
			rkV.x() * rkM[0][2] + rkV.y() * rkM[1][2] + rkV.z() * rkM[2][2] + rkV.w() * rkM[3][2],
			rkV.x() * rkM[0][3] + rkV.y() * rkM[1][3] + rkV.z() * rkM[2][3] + rkV.w() * rkM[3][3]
			);
	}
}
