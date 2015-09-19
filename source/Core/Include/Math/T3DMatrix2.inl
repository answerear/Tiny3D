

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
		return &m_afEntry[nRow * 2];
	}

	inline Real *Matrix2::operator [](int32_t nRow)
	{
		T3D_ASSERT(nRow >= 0 && nRow < 2);
		return &m_afEntry[nRow * 2];
	}

	inline Real Matrix2::operator ()(int32_t nRow, int32_t nCol) const
	{
		T3D_ASSERT(nRow >= 0 && nRow < 2 && nCol >= 0 && nCol < 2);
		return m_afEntry[nRow * 2 + nCol];
	}

	inline Real Matrix2::operator ()()(int32_t nRow, int32_t nCol)
	{
		T3D_ASSERT(nRow >= 0 && nRow < 2 && nCol >= 0 && nCol < 2);
		return m_afEntry[nRow * 2 + nCol];
	}

	inline void Matrix2::setRow(int32_t nRow, const Vector2 &rkV)
	{
		T3D_ASSERT(nRow >= 0 && nRow < 2);
		int32_t i = nRow * 2;
		m_afEntry[i] = rkV[0];
		m_afEntry[i+1] = rkV[1];
	}

	inline Vector2 Matrix2::getRow(int32_t nRow) const
	{
		T3D_ASSERT(nRow >= 0 && nRow < 2);
		int32_t i = nRow * 2;
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
		
	}
}
