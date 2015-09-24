

namespace Tiny3D
{
	inline void Matrix3::makeZero()
	{
		m_afEntry[0] = m_afEntry[1] = m_afEntry[2] = Real(0.0);
		m_afEntry[3] = m_afEntry[4] = m_afEntry[5] = Real(0.0);
		m_afEntry[6] = m_afEntry[7] = m_afEntry[8] = Real(0.0);
	}

	inline void Matrix3::makeIdentity()
	{
		m_afEntry[0] = Real(1.0), m_afEntry[1] = Real(0.0), m_afEntry[2] = Real(0.0);
		m_afEntry[3] = Real(0.0), m_afEntry[4] = Real(1.0), m_afEntry[5] = Real(0.0);
		m_afEntry[6] = Real(0.0), m_afEntry[7] = Real(0.0), m_afEntry[8] = Real(1.0);
	}

	inline void Matrix3::makeDiagonal(Real fM00, Real fM11, Real fM22)
	{
		m_afEntry[0] = fM00, m_afEntry[1] = Real(0.0), m_afEntry[2] = Real(0.0);
		m_afEntry[3] = Real(0.0), m_afEntry[4] = fM11, m_afEntry[5] = Real(0.0);
		m_afEntry[6] = Real(0.0), m_afEntry[7] = Real(0.0), m_afEntry[8] = fM11;
	}

	inline void Matrix3::makeTensorProduct(const Vector3 &rkU, const Vector3 &rkV)
	{
		m_afEntry[0] = rkU[0] * rkV[0];
		m_afEntry[1] = rkU[0] * rkV[1];
		m_afEntry[2] = rkU[0] * rkV[2];
		m_afEntry[3] = rkU[1] * rkV[0];
		m_afEntry[4] = rkU[1] * rkV[1];
		m_afEntry[5] = rkU[1] * rkV[2];
		m_afEntry[6] = rkU[2] * rkV[0];
		m_afEntry[7] = rkU[2] * rkV[1];
		m_afEntry[8] = rkU[2] * rkV[2];
	}

	inline void Matrix3::fromAxisAngle(const Vector3 &rkAxis, const Radian &radians)
	{
		Real fCos = Math::Cos(radians);
		Real fSin = Math::Sin(radians);
		Real fOneMinusCos = Real(1.0) - fCos;
		Real fX2 = rkAxis[0] * rkAxis[0];
		Real fY2 = rkAxis[1] * rkAxis[1];
		Real fZ2 = rkAxis[2] * rkAxis[2];
		Real fXYM = rkAxis[0] * rkAxis[1] * fOneMinusCos;
		Real fXZM = rkAxis[0] * rkAxis[2] * fOneMinusCos;
		Real fYZM = rkAxis[1] * rkAxis[2] * fOneMinusCos;
		Real fXSin = rkAxis[0] * fSin;
		Real fYSin = rkAxis[1] * fSin;
		Real fZSin = rkAxis[2] * fSin;

		m_afEntry[0] = fX2 * fOneMinusCos + fCos;
		m_afEntry[1] = fXYM - fZSin;
		m_afEntry[2] = fXZM+fYSin;
		m_afEntry[3] = fXYM+fZSin;
		m_afEntry[4] = fY2*fOneMinusCos+fCos;
		m_afEntry[5] = fYZM-fXSin;
		m_afEntry[6] = fXZM-fYSin;
		m_afEntry[7] = fYZM+fXSin;
		m_afEntry[8] = fZ2*fOneMinusCos+fCos;
	}

	inline Matrix3::Matrix3(bool bZero /* = true */)
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

	inline Matrix3::Matrix3(const Matrix3 &other)
	{
		memcpy(m_afEntry, other.m_afEntry, sizeof(m_afEntry));
	}

	inline Matrix3::Matrix3(Real fM00, Real fM01, Real fM02, Real fM10, Real fM11, Real fM12, Real fM20, Real fM21, Real fM22)
	{
		m_afEntry[0] = fM00;
		m_afEntry[1] = fM01;
		m_afEntry[2] = fM02;
		m_afEntry[3] = fM10;
		m_afEntry[4] = fM11;
		m_afEntry[5] = fM12;
		m_afEntry[6] = fM20;
		m_afEntry[7] = fM21;
		m_afEntry[8] = fM22;
	}

	inline Matrix3::Matrix3(Real afEntry[9], bool bRowMajor)
	{
		if (bRowMajor)
		{
			memcpy(m_afEntry, afEntry, sizeof(m_afEntry));
		}
		else
		{
			m_afEntry[0] = afEntry[0];
			m_afEntry[1] = afEntry[3];
			m_afEntry[2] = afEntry[6];
			m_afEntry[3] = afEntry[1];
			m_afEntry[4] = afEntry[4];
			m_afEntry[5] = afEntry[7];
			m_afEntry[6] = afEntry[2];
			m_afEntry[7] = afEntry[5];
			m_afEntry[8] = afEntry[8];
		}
	}

	inline Matrix3::Matrix3(const Vector3 &rkU, const Vector3 &rkV, const Vector3 &rkW, bool bColumns)
	{
		if (bColumns)
		{
			m_afEntry[0] = rkU[0];
			m_afEntry[1] = rkV[0];
			m_afEntry[2] = rkW[0];
			m_afEntry[3] = rkU[1];
			m_afEntry[4] = rkV[1];
			m_afEntry[5] = rkW[1];
			m_afEntry[6] = rkU[2];
			m_afEntry[7] = rkV[2];
			m_afEntry[8] = rkW[2];
		}
		else
		{
			m_afEntry[0] = rkU[0];
			m_afEntry[1] = rkU[1];
			m_afEntry[2] = rkU[2];
			m_afEntry[3] = rkV[0];
			m_afEntry[4] = rkV[1];
			m_afEntry[5] = rkV[2];
			m_afEntry[6] = rkW[0];
			m_afEntry[7] = rkW[1];
			m_afEntry[8] = rkW[2];
		}
	}

	inline Matrix3::Matrix3(const Vector3 *akV, bool bColumns)
	{
		if (bColumns)
		{
			m_afEntry[0] = akV[0][0];
			m_afEntry[1] = akV[1][0];
			m_afEntry[2] = akV[2][0];
			m_afEntry[3] = akV[0][1];
			m_afEntry[4] = akV[1][1];
			m_afEntry[5] = akV[2][1];
			m_afEntry[6] = akV[0][2];
			m_afEntry[7] = akV[1][2];
			m_afEntry[8] = akV[2][2];
		}
		else
		{
			m_afEntry[0] = akV[0][0];
			m_afEntry[1] = akV[0][1];
			m_afEntry[2] = akV[0][2];
			m_afEntry[3] = akV[1][0];
			m_afEntry[4] = akV[1][1];
			m_afEntry[5] = akV[1][2];
			m_afEntry[6] = akV[2][0];
			m_afEntry[7] = akV[2][1];
			m_afEntry[8] = akV[2][2];
		}
	}

	inline Matrix3::Matrix3(Real fM00, Real fM11, Real fM22)
	{
		makeDiagonal(fM00, fM11, fM22);
	}

	inline Matrix3::Matrix3(const Vector3 &rkAxis, const Radian &rkRadians)
	{
		fromAxisAngle(rkAxis, rkRadians);
	}

	inline Matrix3::Matrix3(const Vector3 &rkU, const Vector3 &rkV)
	{
		makeTensorProduct(rkU, rkV);
	}

	inline Matrix3::operator const Real *() const
	{
		return m_afEntry;
	}

	inline Matrix3::operator Real *()
	{
		return m_afEntry;
	}

	inline const Real *Matrix3::operator [](int32_t nRow) const
	{
		T3D_ASSERT(nRow >= 0 && nRow < 3);
		return &m_afEntry[nRow * 3];
	}

	inline Real *Matrix3::operator [](int32_t nRow)
	{
		T3D_ASSERT(nRow >= 0 && nRow < 3);
		return &m_afEntry[nRow * 3];
	}

	inline Real Matrix3::operator ()(int32_t nRow, int32_t nCol) const
	{
		T3D_ASSERT(nRow >= 0 && nRow < 3 && nCol >=0 && nCol < 3);
		return m_afEntry[nRow * 3 + nCol];
	}

	inline Real &Matrix3::operator ()(int32_t nRow, int32_t nCol)
	{
		T3D_ASSERT(nRow >= 0 && nRow < 3 && nCol >= 0 && nCol < 3);
		return m_afEntry[nRow * 3 + nCol];
	}

	inline void Matrix3::setRow(int32_t nRow, const Vector3 &rkV)
	{
		T3D_ASSERT(nRow >= 0 && nRow < 3);
		int32_t i = nRow * 3;
		m_afEntry[i] = rkV[0];
		m_afEntry[i+1] = rkV[1];
		m_afEntry[i+2] = rkV[2];
	}

	inline Vector3 Matrix3::getRow(int32_t nRow) const
	{
		T3D_ASSERT(nRow >= 0 && nRow < 3);
		int32_t i = nRow * 3;
		return Vector3(m_afEntry[i], m_afEntry[i+1], m_afEntry[i+2]);
	}

	inline void Matrix3::setColumn(int32_t nCol, const Vector3 &rkV)
	{
		T3D_ASSERT(nCol >= 0 && nCol < 3);
		m_afEntry[nCol] = rkV[0];
		m_afEntry[nCol+3] = rkV[1];
		m_afEntry[nCol+6] = rkV[2];
	}

	inline Vector3 Matrix3::getColumn(int32_t nCol) const
	{
		T3D_ASSERT(nCol >= 0 && nCol < 3);
		return Vector3(m_afEntry[nCol], m_afEntry[nCol+3], m_afEntry[nCol+6]);
	}

	inline void Matrix3::getColumnMajor(Real *afColumns) const
	{
		afColumns[0] = m_afEntry[0];
		afColumns[1] = m_afEntry[3];
		afColumns[2] = m_afEntry[6];
		afColumns[3] = m_afEntry[1];
		afColumns[4] = m_afEntry[4];
		afColumns[5] = m_afEntry[7];
		afColumns[6] = m_afEntry[2];
		afColumns[7] = m_afEntry[5];
		afColumns[8] = m_afEntry[8];
	}

	inline Matrix3 &Matrix3::operator =(const Matrix3 &other)
	{
		memcpy(m_afEntry, other.m_afEntry, sizeof(m_afEntry));
		return *this;
	}

	inline int32_t Matrix3::compareArrays(const Matrix3 &other) const
	{
		return memcmp(m_afEntry, other.m_afEntry, sizeof(m_afEntry));
	}

	inline bool Matrix3::operator ==(const Matrix3 &other) const
	{
		return (compareArrays(other) == 0);
	}

	inline bool Matrix3::operator !=(const Matrix3 &other) const
	{
		return (compareArrays(other) != 0);
	}

	inline bool Matrix3::operator <(const Matrix3 &other) const
	{
		return (compareArrays(other) < 0);
	}

	inline bool Matrix3::operator <=(const Matrix3 &other) const
	{
		return (compareArrays(other) <= 0);
	}

	inline bool Matrix3::operator >(const Matrix3 &other) const
	{
		return (compareArrays(other) > 0);
	}

	inline bool Matrix3::operator >=(const Matrix3 &other) const
	{
		return (compareArrays(other) >= 0);
	}

	inline Matrix3 Matrix3::operator +(const Matrix3 &other) const
	{
		return Matrix3(
			m_afEntry[0] + other.m_afEntry[0],
			m_afEntry[1] + other.m_afEntry[1],
			m_afEntry[2] + other.m_afEntry[2],
			m_afEntry[3] + other.m_afEntry[3],
			m_afEntry[4] + other.m_afEntry[4],
			m_afEntry[5] + other.m_afEntry[5],
			m_afEntry[6] + other.m_afEntry[6],
			m_afEntry[7] + other.m_afEntry[7],
			m_afEntry[8] + other.m_afEntry[8]);
	}

	inline Matrix3 Matrix3::operator -(const Matrix3 &other) const
	{
		return Matrix3(
			m_afEntry[0] - other.m_afEntry[0],
			m_afEntry[1] - other.m_afEntry[1],
			m_afEntry[2] - other.m_afEntry[2],
			m_afEntry[3] - other.m_afEntry[3],
			m_afEntry[4] - other.m_afEntry[4],
			m_afEntry[5] - other.m_afEntry[5],
			m_afEntry[6] - other.m_afEntry[6],
			m_afEntry[7] - other.m_afEntry[7],
			m_afEntry[8] - other.m_afEntry[8]);
	}

	inline Matrix3 Matrix3::operator *(const Matrix3 &other) const
	{
		return Matrix3(
			m_afEntry[0] * other.m_afEntry[0] + m_afEntry[1] * other.m_afEntry[3] + m_afEntry[2] * other.m_afEntry[6],
			m_afEntry[0] * other.m_afEntry[1] + m_afEntry[1] * other.m_afEntry[4] + m_afEntry[2] * other.m_afEntry[7],
			m_afEntry[0] * other.m_afEntry[2] + m_afEntry[1] * other.m_afEntry[5] + m_afEntry[2] * other.m_afEntry[8],
			m_afEntry[3] * other.m_afEntry[0] + m_afEntry[4] * other.m_afEntry[3] + m_afEntry[5] * other.m_afEntry[6],
			m_afEntry[3] * other.m_afEntry[1] + m_afEntry[4] * other.m_afEntry[4] + m_afEntry[5] * other.m_afEntry[7],
			m_afEntry[3] * other.m_afEntry[2] + m_afEntry[4] * other.m_afEntry[5] + m_afEntry[5] * other.m_afEntry[8],
			m_afEntry[6] * other.m_afEntry[0] + m_afEntry[7] * other.m_afEntry[3] + m_afEntry[8] * other.m_afEntry[6],
			m_afEntry[6] * other.m_afEntry[1] + m_afEntry[7] * other.m_afEntry[4] + m_afEntry[8] * other.m_afEntry[7],
			m_afEntry[6] * other.m_afEntry[2] + m_afEntry[7] * other.m_afEntry[5] + m_afEntry[8] * other.m_afEntry[8]
			);
	}

	inline Matrix3 Matrix3::operator *(Real fScalar) const
	{
		return Matrix3(
			m_afEntry[0] * fScalar, m_afEntry[1] * fScalar, m_afEntry[2] * fScalar,
			m_afEntry[3] * fScalar, m_afEntry[4] * fScalar, m_afEntry[5] * fScalar,
			m_afEntry[6] * fScalar, m_afEntry[7] * fScalar, m_afEntry[8] * fScalar);
	}

	inline Matrix3 Matrix3::operator /(Real fScalar) const
	{
		if (fScalar != 0.0)
			return Matrix3(
				m_afEntry[0] / fScalar, m_afEntry[1] / fScalar, m_afEntry[2] / fScalar, 
				m_afEntry[3] / fScalar, m_afEntry[4] / fScalar, m_afEntry[5] / fScalar,
				m_afEntry[6] / fScalar, m_afEntry[7] / fScalar, m_afEntry[8] / fScalar);
		return ZERO;
	}

	inline Matrix3 Matrix3::operator -() const
	{
		return Matrix3(
			-m_afEntry[0], -m_afEntry[1], -m_afEntry[2],
			-m_afEntry[3], -m_afEntry[4], -m_afEntry[5],
			-m_afEntry[6], -m_afEntry[7], -m_afEntry[8]);
	}

	inline Matrix3 &Matrix3::operator +=(const Matrix3 &other)
	{
		m_afEntry[0] += other.m_afEntry[0];
		m_afEntry[1] += other.m_afEntry[1];
		m_afEntry[2] += other.m_afEntry[2];
		m_afEntry[3] += other.m_afEntry[3];
		m_afEntry[4] += other.m_afEntry[4];
		m_afEntry[5] += other.m_afEntry[5];
		m_afEntry[6] += other.m_afEntry[6];
		m_afEntry[7] += other.m_afEntry[7];
		m_afEntry[8] += other.m_afEntry[8];
		return *this;
	}

	inline Matrix3 &Matrix3::operator -=(const Matrix3 &other)
	{
		m_afEntry[0] -= other.m_afEntry[0];
		m_afEntry[1] -= other.m_afEntry[1];
		m_afEntry[2] -= other.m_afEntry[2];
		m_afEntry[3] -= other.m_afEntry[3];
		m_afEntry[4] -= other.m_afEntry[4];
		m_afEntry[5] -= other.m_afEntry[5];
		m_afEntry[6] -= other.m_afEntry[6];
		m_afEntry[7] -= other.m_afEntry[7];
		m_afEntry[8] -= other.m_afEntry[8];
		return *this;
	}

	inline Matrix3 &Matrix3::operator *=(Real fScalar)
	{
		m_afEntry[0] *= fScalar;
		m_afEntry[1] *= fScalar;
		m_afEntry[2] *= fScalar;
		m_afEntry[3] *= fScalar;
		m_afEntry[4] *= fScalar;
		m_afEntry[5] *= fScalar;
		m_afEntry[6] *= fScalar;
		m_afEntry[7] *= fScalar;
		m_afEntry[8] *= fScalar;
		return *this;
	}

	inline Matrix3 &Matrix3::operator /=(Real fScalar)
	{
		if (fScalar != 0.0)
		{
			m_afEntry[0] /= fScalar;
			m_afEntry[1] /= fScalar;
			m_afEntry[2] /= fScalar;
			m_afEntry[3] /= fScalar;
			m_afEntry[4] /= fScalar;
			m_afEntry[5] /= fScalar;
			m_afEntry[6] /= fScalar;
			m_afEntry[7] /= fScalar;
			m_afEntry[8] /= fScalar;
		}
		else
		{
			makeZero();
		}
		return *this;
	}

	inline Vector3 Matrix3::operator *(const Vector3 &rkV) const
	{
		return Vector3(
			m_afEntry[0] * rkV[0] + m_afEntry[1] * rkV[1] + m_afEntry[2] * rkV[2], 
			m_afEntry[3] * rkV[0] + m_afEntry[4] * rkV[1] + m_afEntry[5] * rkV[2],
			m_afEntry[6] * rkV[0] + m_afEntry[7] * rkV[1] + m_afEntry[8] * rkV[2]);
	}

	inline Matrix3 Matrix3::transpose() const
	{
		return Matrix3(
			m_afEntry[0], m_afEntry[3], m_afEntry[6],
			m_afEntry[1], m_afEntry[4], m_afEntry[7],
			m_afEntry[2], m_afEntry[5], m_afEntry[8]);
	}

	inline Matrix3 Matrix3::transposeTimes(const Matrix3 &other) const
	{
		return Matrix3(
			m_afEntry[0] * other.m_afEntry[0] + m_afEntry[3] * other.m_afEntry[3] + m_afEntry[6] * other.m_afEntry[6], 
			m_afEntry[0] * other.m_afEntry[1] + m_afEntry[3] * other.m_afEntry[4] + m_afEntry[6] * other.m_afEntry[7], 
			m_afEntry[0] * other.m_afEntry[2] + m_afEntry[3] * other.m_afEntry[5] + m_afEntry[6] * other.m_afEntry[8], 
			m_afEntry[1] * other.m_afEntry[0] + m_afEntry[4] * other.m_afEntry[3] + m_afEntry[7] * other.m_afEntry[6], 
			m_afEntry[1] * other.m_afEntry[1] + m_afEntry[4] * other.m_afEntry[4] + m_afEntry[7] * other.m_afEntry[7], 
			m_afEntry[1] * other.m_afEntry[2] + m_afEntry[4] * other.m_afEntry[5] + m_afEntry[7] * other.m_afEntry[8], 
			m_afEntry[2] * other.m_afEntry[0] + m_afEntry[5] * other.m_afEntry[3] + m_afEntry[8] * other.m_afEntry[6], 
			m_afEntry[2] * other.m_afEntry[1] + m_afEntry[5] * other.m_afEntry[4] + m_afEntry[8] * other.m_afEntry[7], 
			m_afEntry[2] * other.m_afEntry[2] + m_afEntry[5] * other.m_afEntry[5] + m_afEntry[8] * other.m_afEntry[8]);
	}

	inline Matrix3 Matrix3::timesTranspose(const Matrix3 &other) const
	{
		return Matrix3(
			m_afEntry[0] * other.m_afEntry[0] + m_afEntry[1] * other.m_afEntry[1] + m_afEntry[2] * other.m_afEntry[2],
			m_afEntry[0] * other.m_afEntry[3] + m_afEntry[1] * other.m_afEntry[4] + m_afEntry[2] * other.m_afEntry[5],
			m_afEntry[0] * other.m_afEntry[6] + m_afEntry[1] * other.m_afEntry[7] + m_afEntry[2] * other.m_afEntry[8],
			m_afEntry[3] * other.m_afEntry[0] + m_afEntry[4] * other.m_afEntry[1] + m_afEntry[5] * other.m_afEntry[2],
			m_afEntry[3] * other.m_afEntry[3] + m_afEntry[4] * other.m_afEntry[4] + m_afEntry[5] * other.m_afEntry[5],
			m_afEntry[3] * other.m_afEntry[6] + m_afEntry[4] * other.m_afEntry[7] + m_afEntry[5] * other.m_afEntry[8],
			m_afEntry[6] * other.m_afEntry[0] + m_afEntry[7] * other.m_afEntry[1] + m_afEntry[8] * other.m_afEntry[2],
			m_afEntry[6] * other.m_afEntry[3] + m_afEntry[7] * other.m_afEntry[4] + m_afEntry[8] * other.m_afEntry[5],
			m_afEntry[6] * other.m_afEntry[6] + m_afEntry[7] * other.m_afEntry[7] + m_afEntry[8] * other.m_afEntry[8]);
	}

	inline Matrix3 Matrix3::inverse() const
	{
		Matrix3 matInverse;
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

	inline Real Matrix3::determinant() const
	{
		// This 3x3 matrix : 
		//        +-       -+
		//    A = | A0 A1 A2|
		//        | A3 A4 A5|
		//        | A6 A7 A8|
		//        +-       -+
		// 
		// det(A) = A0 * A4 * A8 + A1 * A5 * A6 + A2 * A3 * A7 - A2 * A4 * A6 - A1 * A3 * A8 - A0 * A5 * A7
		// equal to
		// det(A) = A0 * (A4 * A8 - A5 * A7) + A1 * (A5 * A6 - A3 * A8) + A2 * (A3 * A7 - A4 * A6)

		Real fCofactor00 = m_afEntry[4] * m_afEntry[8] - m_afEntry[5] * m_afEntry[7];
		Real fCofactor01 = m_afEntry[5] * m_afEntry[6] - m_afEntry[3] * m_afEntry[8];
		Real fCofactor02 = m_afEntry[3] * m_afEntry[7] - m_afEntry[4] * m_afEntry[6];

		return m_afEntry[0] * fCofactor00 + m_afEntry[1] * fCofactor01 + m_afEntry[2] * fCofactor02;
	}

	inline Real Matrix3::qform(const Vector3 &rkU, const Vector3 &rkV) const
	{
		return rkU.dot((*this) * rkV);
	}

	inline Matrix3 Matrix3::timesDiagonal(const Vector3 &rkDiag) const
	{
		return Matrix3(
			m_afEntry[0] * rkDiag[0], m_afEntry[1] * rkDiag[1], m_afEntry[2] * rkDiag[2],
			m_afEntry[3] * rkDiag[0], m_afEntry[4] * rkDiag[1], m_afEntry[5] * rkDiag[2],
			m_afEntry[6] * rkDiag[0], m_afEntry[7] * rkDiag[1], m_afEntry[8] * rkDiag[2]);
	}

	inline Matrix3 Matrix3::diagonalTimes(const Vector3 &rkDiag) const
	{
		return Matrix3(
			rkDiag[0] * m_afEntry[0], rkDiag[0] * m_afEntry[1], rkDiag[0] * m_afEntry[2],
			rkDiag[1] * m_afEntry[3], rkDiag[1] * m_afEntry[4], rkDiag[1] * m_afEntry[5],
			rkDiag[2] * m_afEntry[6], rkDiag[2] * m_afEntry[7], rkDiag[2] * m_afEntry[8]);
	}

	inline void Matrix3::fromEulerAnglesXYZ(const Radian &rkPitch, const Radian &rkYaw, const Radian &rkRoll)
	{
		Real fCos, fSin;

		fCos = Math::Cos(rkPitch);
		fSin = Math::Sin(rkPitch);
		Matrix3 matX(
			1.0, 0.0, 0.0, 
			0.0, fCos, -fSin, 
			0.0, fSin, fCos);

		fCos = Math::Cos(rkYaw);
		fSin = Math::Sin(rkYaw);
		Matrix3 matY(
			fCos, 0.0, fSin, 
			0.0, 1.0, 0.0, 
			-fSin, 0.0, fCos);

		fCos = Math::Cos(rkRoll);
		fSin = Math::Sin(rkRoll);
		Matrix3 matZ(
			fCos, -fSin, 0.0, 
			fSin, fCos, 0.0, 
			0.0, 0.0, 1.0);

		*this = matX * (matY * matZ);
	}

	inline void Matrix3::fromEulerAnglesXZY(const Radian &rkPitch, const Radian &rkRoll, const Radian &rkYaw)
	{
		Real fCos, fSin;

		fCos = Math::Cos(rkPitch);
		fSin = Math::Sin(rkPitch);
		Matrix3 matX(
			1.0, 0.0, 0.0,
			0.0, fCos, -fSin,
			0.0, fSin, fCos);

		fCos = Math::Cos(rkRoll);
		fSin = Math::Sin(rkRoll);
		Matrix3 matZ(
			fCos, -fSin, 0.0,
			fSin, fCos, 0.0,
			0.0, 0.0, 1.0);

		fCos = Math::Cos(rkYaw);
		fSin = Math::Sin(rkYaw);
		Matrix3 matY(
			fCos, 0.0, fSin,
			0.0, 1.0, 0.0,
			-fSin, 0.0, fCos);

		*this = matX * (matZ * matY);
	}

	inline void Matrix3::fromEulerAnglesYXZ(const Radian &rkYaw, const Radian &rkPitch, const Radian &rkRoll)
	{
		Real fCos, fSin;

		fCos = Math::Cos(rkYaw);
		fSin = Math::Sin(rkYaw);
		Matrix3 matY(
			fCos, 0.0, fSin,
			0.0, 1.0, 0.0,
			-fSin, 0.0, fCos);

		fCos = Math::Cos(rkPitch);
		fSin = Math::Sin(rkPitch);
		Matrix3 matX(
			1.0, 0.0, 0.0,
			0.0, fCos, -fSin,
			0.0, fSin, fCos);

		fCos = Math::Cos(rkRoll);
		fSin = Math::Sin(rkRoll);
		Matrix3 matZ(
			fCos, -fSin, 0.0,
			fSin, fCos, 0.0,
			0.0, 0.0, 1.0);

		*this = matY * (matX * matZ);
	}

	inline void Matrix3::fromEulerAnglesYZX(const Radian &rkYaw, const Radian &rkRoll, const Radian &rkPitch)
	{
		Real fCos, fSin;

		fCos = Math::Cos(rkYaw);
		fSin = Math::Sin(rkYaw);
		Matrix3 matY(
			fCos, 0.0, fSin,
			0.0, 1.0, 0.0,
			-fSin, 0.0, fCos);

		fCos = Math::Cos(rkRoll);
		fSin = Math::Sin(rkRoll);
		Matrix3 matZ(
			fCos, -fSin, 0.0,
			fSin, fCos, 0.0,
			0.0, 0.0, 1.0);

		fCos = Math::Cos(rkPitch);
		fSin = Math::Sin(rkPitch);
		Matrix3 matX(
			1.0, 0.0, 0.0,
			0.0, fCos, -fSin,
			0.0, fSin, fCos);

		*this = matY * (matZ * matX);
	}

	inline void Matrix3::fromEulerAnglesZXY(const Radian &rkRoll, const Radian &rkPitch, const Radian &rkYaw)
	{
		Real fCos, fSin;

		fCos = Math::Cos(rkRoll);
		fSin = Math::Sin(rkRoll);
		Matrix3 matZ(
			fCos, -fSin, 0.0,
			fSin, fCos, 0.0,
			0.0, 0.0, 1.0);

		fCos = Math::Cos(rkPitch);
		fSin = Math::Sin(rkPitch);
		Matrix3 matX(
			1.0, 0.0, 0.0,
			0.0, fCos, -fSin,
			0.0, fSin, fCos);

		fCos = Math::Cos(rkYaw);
		fSin = Math::Sin(rkYaw);
		Matrix3 matY(
			fCos, 0.0, fSin,
			0.0, 1.0, 0.0,
			-fSin, 0.0, fCos);

		*this = matZ * (matX * matY);
	}

	inline void Matrix3::fromEulerAnglesZYX(const Radian &krRoll, const Radian &rkYaw, const Radian &rkPitch)
	{
		Real fCos, fSin;

		fCos = Math::Cos(rkRoll);
		fSin = Math::Sin(rkRoll);
		Matrix3 matZ(
			fCos, -fSin, 0.0,
			fSin, fCos, 0.0,
			0.0, 0.0, 1.0);

		fCos = Math::Cos(rkYaw);
		fSin = Math::Sin(rkYaw);
		Matrix3 matY(
			fCos, 0.0, fSin,
			0.0, 1.0, 0.0,
			-fSin, 0.0, fCos);

		fCos = Math::Cos(rkPitch);
		fSin = Math::Sin(rkPitch);
		Matrix3 matX(
			1.0, 0.0, 0.0,
			0.0, fCos, -fSin,
			0.0, fSin, fCos);

		*this = matZ * (matY * matX);
	}

	inline Matrix3 operator *(Real fScalar, const Matrix3 &rkM)
	{
		return rkM * fScalar;
	}

	inline Vector3 operator *(const Vector3 &rkV, const Matrix3 &rkM)
	{
		return Vector3(
			rkV[0] * rkM[0][0] + rkV[1] * rkM[1][0] + rkV[2] * rkM[2][0], 
			rkV[0] * rkM[0][1] + rkV[1] * rkM[1][1] + rkV[2] * rkM[2][1],
			rkV[0] * rkM[0][2] + rkV[1] * rkM[1][2] + rkV[2] * rkM[2][2]);
	}
}
