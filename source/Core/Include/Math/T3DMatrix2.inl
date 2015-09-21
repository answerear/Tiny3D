

namespace Tiny3D
{
	inline void Matrix2::makeZero()
	{
		m_afEntry[0] = Real(0.0);
		m_afEntry[1] = Real(0.0);
		m_afEntry[2] = Real(0.0);
		m_afEntry[3] = Real(0.0);
	}

	inline void Matrix2::makeIdentity()
	{
		m_afEntry[0] = Real(1.0);
		m_afEntry[1] = Real(0.0);
		m_afEntry[2] = Real(0.0);
		m_afEntry[3] = Real(1.0);
	}

	inline void Matrix2::makeDiagonal(Real fM00, Real fM11)
	{
		m_afEntry[0] = fM00;
		m_afEntry[1] = Real(0.0);
		m_afEntry[2] = Real(0.0);
		m_afEntry[3] = fM11;
	}

	inline void Matrix2::makeTensorProduct(const Vector2 &rkU, const Vector2 &rkV)
	{
		m_afEntry[0] = rkU[0] * rkV[0];
		m_afEntry[1] = rkU[0] * rkV[1];
		m_afEntry[2] = rkU[1] * rkV[0];
		m_afEntry[3] = rkU[1] * rkV[1];
	}

	inline void Matrix2::fromAngle(const Radian &radians)
	{
		m_afEntry[0] = Math::Cos(radians);
		m_afEntry[2] = Math::Sin(radians);
		m_afEntry[1] = -m_afEntry[2];
		m_afEntry[3] = m_afEntry[0];
	}

	inline Radian Matrix2::toAngle() const
	{
		return Math::ATan2(m_afEntry[2], m_afEntry[0]);
	}

	inline Matrix2::Matrix2(bool bZero /* = true */)
	{
		if (bZero)
		{
			makeZero();
		}
		else
		{
			makeIdentity();
		}
	}

	inline Matrix2::Matrix2(const Matrix2 &other)
	{
		memcpy(m_afEntry, other.m_afEntry, sizeof(m_afEntry));
	}

	inline Matrix2::Matrix2(Real fM00, Real fM01, Real fM10, Real fM11)
	{
		m_afEntry[0] = fM00;
		m_afEntry[1] = fM01;
		m_afEntry[2] = fM10;
		m_afEntry[3] = fM11;
	}

	inline Matrix2::Matrix2(Real afEntry[4], bool bRowMajor)
	{
		if (bRowMajor)
		{
			memcpy(m_afEntry, afEntry, sizeof(m_afEntry));
		}
		else
		{
			m_afEntry[0] = afEntry[0];
			m_afEntry[1] = afEntry[2];
			m_afEntry[2] = afEntry[1];
			m_afEntry[3] = afEntry[3];
		}
	}

	inline Matrix2::Matrix2(const Vector2 &rkU, const Vector2 &rkV, bool bColumns)
	{
		if (bColumns)
		{
			m_afEntry[0] = rkU[0];
			m_afEntry[1] = rkV[0];
			m_afEntry[2] = rkU[1];
			m_afEntry[3] = rkV[1];
		}
		else
		{
			m_afEntry[0] = rkU[0];
			m_afEntry[1] = rkU[1];
			m_afEntry[2] = rkV[0];
			m_afEntry[3] = rkV[1];
		}
	}

	inline Matrix2::Matrix2(const Vector2 *akV, bool bColumns)
	{
		if (bColumns)
		{
			m_afEntry[0] = akV[0][0];
			m_afEntry[1] = akV[1][0];
			m_afEntry[2] = akV[0][1];
			m_afEntry[3] = akV[1][1];
		}
		else
		{
			m_afEntry[0] = akV[0][0];
			m_afEntry[1] = akV[0][1];
			m_afEntry[2] = akV[1][0];
			m_afEntry[3] = akV[1][1];
		}
	}

	inline Matrix2::Matrix2(Real fM00, Real fM11)
	{
		makeDiagonal(fM00, fM11);
	}

	inline Matrix2::Matrix2(const Degree &degree)
	{
		fromAngle(degree);
	}

	inline Matrix2::Matrix2(const Vector2 &rkU, const Vector2 &rkV)
	{
		makeTensorProduct(rkU, rkV);
	}

	inline Matrix2::operator const Real *() const
	{
		return m_afEntry;
	}

	inline Matrix2::operator Real *()
	{
		return m_afEntry;
	}

	inline const Real *Matrix2::operator [](int32_t nRow) const
	{
		T3D_ASSERT(nRow >= 0 && nRow < 2);
		return &m_afEntry[nRow << 1];
	}

	inline Real *Matrix2::operator [](int32_t nRow)
	{
		T3D_ASSERT(nRow >= 0 && nRow < 2);
		return &m_afEntry[nRow << 1];
	}

	inline Real Matrix2::operator ()(int32_t nRow, int32_t nCol) const
	{
		T3D_ASSERT(nRow >= 0 && nRow < 2 && nCol >= 0 && nCol < 2);
		return m_afEntry[(nRow << 1) + nCol];
	}

	inline Real &Matrix2::operator ()(int32_t nRow, int32_t nCol)
	{
		T3D_ASSERT(nRow >= 0 && nRow < 2 && nCol >= 0 && nCol < 2);
		return m_afEntry[(nRow << 1) + nCol];
	}

	inline void Matrix2::setRow(int32_t nRow, const Vector2 &rkV)
	{
		T3D_ASSERT(nRow >= 0 && nRow < 2);
		int32_t i = nRow << 1;
		m_afEntry[i] = rkV[0];
		m_afEntry[i+1] = rkV[1];
	}

	inline Vector2 Matrix2::getRow(int32_t nRow) const
	{
		T3D_ASSERT(nRow >= 0 && nRow < 2);
		int32_t i = nRow << 1;
		return Vector2(m_afEntry[i], m_afEntry[i+1]);
	}

	inline void Matrix2::setColumn(int32_t nCol, const Vector2 &rkV)
	{
		T3D_ASSERT(nCol >= 0 && nCol < 2);
		m_afEntry[nCol] = rkV[0];
		m_afEntry[2+nCol] = rkV[1];
	}

	inline Vector2 Matrix2::getColumn(int32_t nCol) const
	{
		T3D_ASSERT(nCol >= 0 && nCol < 2);
		return Vector2(m_afEntry[nCol], m_afEntry[2+nCol]);
	}

	inline void Matrix2::getColumnMajor(Real *afColumns) const
	{
		afColumns[0] = m_afEntry[0];
		afColumns[1] = m_afEntry[2];
		afColumns[2] = m_afEntry[1];
		afColumns[3] = m_afEntry[3];
	}

	inline Matrix2 &Matrix2::operator =(const Matrix2 &other)
	{
		memcpy(m_afEntry, other.m_afEntry, sizeof(m_afEntry));
		return *this;
	}

	inline int32_t Matrix2::compareArrays(const Matrix2 &other) const
	{
		return memcmp(m_afEntry, other.m_afEntry, sizeof(m_afEntry));
	}

	inline bool Matrix2::operator ==(const Matrix2 &other) const
	{
		return (compareArrays(other) == 0);
	}

	inline bool Matrix2::operator !=(const Matrix2 &other) const
	{
		return (compareArrays(other) != 0);
	}

	inline bool Matrix2::operator <(const Matrix2 &other) const
	{
		return (compareArrays(other) < 0);
	}

	inline bool Matrix2::operator <=(const Matrix2 &other) const
	{
		return (compareArrays(other) <= 0);
	}

	inline bool Matrix2::operator >(const Matrix2 &other) const
	{
		return (compareArrays(other) > 0);
	}

	inline bool Matrix2::operator >=(const Matrix2 &other) const
	{
		return (compareArrays(other) >= 0);
	}

	inline Matrix2 Matrix2::operator +(const Matrix2 &other) const
	{
		return Matrix2(
			m_afEntry[0] + other.m_afEntry[0],
			m_afEntry[1] + other.m_afEntry[1],
			m_afEntry[2] + other.m_afEntry[2],
			m_afEntry[3] + other.m_afEntry[3]);
	}

	inline Matrix2 Matrix2::operator -(const Matrix2 &other) const
	{
		return Matrix2(
			m_afEntry[0] - other.m_afEntry[0],
			m_afEntry[1] - other.m_afEntry[1],
			m_afEntry[2] - other.m_afEntry[2],
			m_afEntry[3] - other.m_afEntry[3]);
	}

	inline Matrix2 Matrix2::operator *(const Matrix2 &other) const
	{
		return Matrix2(
			m_afEntry[0] * other.m_afEntry[0] + m_afEntry[1] * other.m_afEntry[2],
			m_afEntry[0] * other.m_afEntry[1] + m_afEntry[1] * other.m_afEntry[3],
			m_afEntry[2] * other.m_afEntry[0] + m_afEntry[2] * other.m_afEntry[2],
			m_afEntry[2] * other.m_afEntry[1] + m_afEntry[2] * other.m_afEntry[3]);
	}

	inline Matrix2 Matrix2::operator *(Real fScalar) const
	{
		return Matrix2(m_afEntry[0] * fScalar, m_afEntry[1] * fScalar, m_afEntry[2] * fScalar, m_afEntry[3] * fScalar);
	}

	inline Matrix2 Matrix2::operator /(Real fScalar) const
	{
		if (fScalar != 0.0)
			return Matrix2(m_afEntry[0] / fScalar, m_afEntry[1] / fScalar, m_afEntry[2] / fScalar, m_afEntry[3] / fScalar);
		return ZERO;
	}

	inline Matrix2 Matrix2::operator -() const
	{
		return Matrix2(-m_afEntry[0], -m_afEntry[1], -m_afEntry[2], -m_afEntry[3]);
	}

	inline Matrix2 &Matrix2::operator +=(const Matrix2 &other)
	{
		m_afEntry[0] += other.m_afEntry[0];
		m_afEntry[1] += other.m_afEntry[1];
		m_afEntry[2] += other.m_afEntry[2];
		m_afEntry[3] += other.m_afEntry[3];
		return *this;
	}

	inline Matrix2 &Matrix2::operator -=(const Matrix2 &other)
	{
		m_afEntry[0] -= other.m_afEntry[0];
		m_afEntry[1] -= other.m_afEntry[1];
		m_afEntry[2] -= other.m_afEntry[2];
		m_afEntry[3] -= other.m_afEntry[3];
		return *this;
	}

	inline Matrix2 &Matrix2::operator *=(Real fScalar)
	{
		m_afEntry[0] *= fScalar;
		m_afEntry[1] *= fScalar;
		m_afEntry[2] *= fScalar;
		m_afEntry[3] *= fScalar;
		return *this;
	}

	inline Matrix2 &Matrix2::operator /=(Real fScalar)
	{
		if (fScalar != 0.0)
		{
			m_afEntry[0] /= fScalar;
			m_afEntry[1] /= fScalar;
			m_afEntry[2] /= fScalar;
			m_afEntry[3] /= fScalar;
		}
		else
		{
			makeZero();
		}
		return *this;
	}

	inline Vector2 Matrix2::operator *(const Vector2 &rkV) const
	{
		return Vector2(m_afEntry[0] * rkV[0] + m_afEntry[1] * rkV[1], 
			m_afEntry[2] * rkV[0] + m_afEntry[3] * rkV[1]);
	}

	inline Matrix2 Matrix2::transpose() const
	{
		return Matrix2(m_afEntry[0], m_afEntry[2], m_afEntry[1], m_afEntry[3]);
	}

	inline Matrix2 Matrix2::transposeTimes(const Matrix2 &other) const
	{
		return Matrix2(
			m_afEntry[0] * other.m_afEntry[0] + m_afEntry[2] * other.m_afEntry[2],
			m_afEntry[0] * other.m_afEntry[1] + m_afEntry[2] * other.m_afEntry[3],
			m_afEntry[1] * other.m_afEntry[0] + m_afEntry[3] * other.m_afEntry[2],
			m_afEntry[1] * other.m_afEntry[1] + m_afEntry[3] * other.m_afEntry[3]);
	}

	inline Matrix2 Matrix2::timesTranspose(const Matrix2 &other) const
	{
		return Matrix2(
			m_afEntry[0] * other.m_afEntry[0] + m_afEntry[1] * other.m_afEntry[2],
			m_afEntry[2] * other.m_afEntry[0] + m_afEntry[2] * other.m_afEntry[2],
			m_afEntry[0] * other.m_afEntry[1] + m_afEntry[1] * other.m_afEntry[3],
			m_afEntry[2] * other.m_afEntry[1] + m_afEntry[2] * other.m_afEntry[3]);
	}

	inline Matrix2 Matrix2::inverse() const
	{
		Matrix2 matInverse;
		Real fDet = m_afEntry[0] * m_afEntry[3] - m_afEntry[1] * m_afEntry[2];
		if (fDet != 0.0)
		{
			matInverse.m_afEntry[0] = m_afEntry[3] / fDet;
			matInverse.m_afEntry[1] = -m_afEntry[1] / fDet;
			matInverse.m_afEntry[2] = -m_afEntry[2] / fDet;
			matInverse.m_afEntry[3] = m_afEntry[0] / fDet;
		}
		return matInverse;
	}

	inline Matrix2 Matrix2::adjoint() const
	{
		return Matrix2(m_afEntry[3], -m_afEntry[1], -m_afEntry[2], m_afEntry[0]);
	}

	inline Real Matrix2::determinant() const
	{
		return m_afEntry[0] * m_afEntry[3] - m_afEntry[1] * m_afEntry[2];
	}

	inline Real Matrix2::qform(const Vector2 &rkU, const Vector2 &rkV) const
	{
		return rkU.dot((*this) * rkV);
	}

	inline void Matrix2::orthonormalize()
	{
		// Algorithm uses Gram-Schmidt orthogonalization.  
		// If 'this' matrix is M = [m0|m1], then orthonormal output matrix is Q = [q0|q1],
		//
		//   q0 = m0/|m0|
		//   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
		//
		// where |V| indicates length of vector V and A*B indicates dot product of vectors A and B.
		
		// compute q0
		Real fLength = Math::Sqrt(m_afEntry[0] * m_afEntry[0] + m_afEntry[2] * m_afEntry[2]);
		m_afEntry[0] /= fLength;
		m_afEntry[2] /= fLength;

		// compute q1
		Real fDot0 = m_afEntry[0] * m_afEntry[1] + m_afEntry[2] * m_afEntry[3];
		m_afEntry[1] -= fDot0 * m_afEntry[0];
		m_afEntry[3] -= fDot0 * m_afEntry[2];

		fLength = Math::Sqrt(m_afEntry[1] * m_afEntry[1] + m_afEntry[3] * m_afEntry[3]);
		m_afEntry[1] /= fLength;
		m_afEntry[3] /= fLength;
	}


	inline Matrix2 operator *(Real fScalar, const Matrix2 &rkM)
	{
		return rkM * fScalar;
	}

	inline Vector2 operator *(const Vector2 &rkV, const Matrix2 &rkM)
	{
		return Vector2(rkV[0] * rkM[0][0] + rkV[1] * rkM[1][0], rkV[0] * rkM[0][1] + rkV[1] * rkM[1][1]);
	}
}
