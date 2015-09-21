

namespace Tiny3D
{
	void Matrix3::makeZero()
	{
		m_afEntry[0] = m_afEntry[1] = m_afEntry[2] = Real(0.0);
		m_afEntry[3] = m_afEntry[4] = m_afEntry[5] = Real(0.0);
		m_afEntry[6] = m_afEntry[7] = m_afEntry[8] = Real(0.0);
	}

	void Matrix3::makeIdentity()
	{
		m_afEntry[0] = Real(1.0), m_afEntry[1] = Real(0.0), m_afEntry[2] = Real(0.0);
		m_afEntry[3] = Real(0.0), m_afEntry[4] = Real(1.0), m_afEntry[5] = Real(0.0);
		m_afEntry[6] = Real(0.0), m_afEntry[7] = Real(0.0), m_afEntry[8] = Real(1.0);
	}

	void Matrix3::makeDiagonal(Real fM00, Real fM11, Real fM22)
	{
		m_afEntry[0] = fM00, m_afEntry[1] = Real(0.0), m_afEntry[2] = Real(0.0);
		m_afEntry[3] = Real(0.0), m_afEntry[4] = fM11, m_afEntry[5] = Real(0.0);
		m_afEntry[6] = Real(0.0), m_afEntry[7] = Real(0.0), m_afEntry[8] = fM11;
	}

	void Matrix3::makeTensorProduct(const Vector3 &rkU, const Vector3 &rkV)
	{

	}

	Matrix3::Matrix3(bool bZero /* = true */)
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

	Matrix3::Matrix3(Real fM00, Real fM01, Real fM02, Real fM10, Real fM11, Real fM12, Real fM20, Real fM21, Real fM22)
	{

	}
}
