

#include "T3DMatrix3.h"


namespace Tiny3D
{
	const Matrix3 Matrix3::ZERO(Real(0.0), Real(0.0), Real(0.0), Real(0.0), Real(0.0), Real(0.0), Real(0.0), Real(0.0), Real(0.0));
	const Matrix3 Matrix3::IDENTITY(Real(1.0), Real(0.0), Real(0.0), Real(0.0), Real(1.0), Real(0.0), Real(0.0), Real(0.0), Real(1.0));

	void Matrix3::toAxisAngle(Vector3 &rAxis, Radian &rRadians) const
	{
		// Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
		// The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
		// I is the identity and
		//
		//       +-        -+
		//   P = |  0 -z +y |
		//       | +z  0 -x |
		//       | -y +x  0 |
		//       +-        -+
		//
		// If A > 0, R represents a counterclockwise rotation about the axis in
		// the sense of looking from the tip of the axis vector towards the
		// origin.  Some algebra will show that
		//
		//   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
		//
		// In the event that A = pi, R-R^t = 0 which prevents us from extracting
		// the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
		// P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
		// z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
		// it does not matter which sign you choose on the square roots.

		Real fTrace = m_afEntry[0] + m_afEntry[4] + m_afEntry[8];
		Real fCos = 0.5f * (fTrace - 1.0f);
		rRadians = Math::ACos(fCos);  // in [0,PI]

		if (rRadians > Radian(0.0))
		{
			if (rRadians < Radian(Math::PI))
			{
				rAxis.x() = m_afEntry[7] - m_afEntry[5];
				rAxis.y() = m_afEntry[2] - m_afEntry[6];
				rAxis.z() = m_afEntry[3] - m_afEntry[1];
				rAxis.normalize();
			}
			else
			{
				// angle is PI
				float fHalfInverse;
				if (m_afEntry[0] >= m_afEntry[4])
				{
					// r00 >= r11
					if (m_afEntry[0] >= m_afEntry[8])
					{
						// r00 is maximum diagonal term
						rAxis.x() = 0.5f * Math::Sqrt(m_afEntry[0] - m_afEntry[4] - m_afEntry[8] + 1.0f);
						fHalfInverse = 0.5f / rAxis.x();
						rAxis.y() = fHalfInverse * m_afEntry[1];
						rAxis.z() = fHalfInverse * m_afEntry[2];
					}
					else
					{
						// r22 is maximum diagonal term
						rAxis.z() = 0.5f * Math::Sqrt(m_afEntry[8] - m_afEntry[0] - m_afEntry[4] + 1.0f);
						fHalfInverse = 0.5f / rAxis.z();
						rAxis.x() = fHalfInverse * m_afEntry[2];
						rAxis.y() = fHalfInverse * m_afEntry[5];
					}
				}
				else
				{
					// r11 > r00
					if ( m_afEntry[4] >= m_afEntry[8] )
					{
						// r11 is maximum diagonal term
						rAxis.y() = 0.5f * Math::Sqrt(m_afEntry[4] - m_afEntry[0] - m_afEntry[8] + 1.0f);
						fHalfInverse  = 0.5f / rAxis.y();
						rAxis.x() = fHalfInverse * m_afEntry[1];
						rAxis.z() = fHalfInverse * m_afEntry[5];
					}
					else
					{
						// r22 is maximum diagonal term
						rAxis.z() = 0.5f * Math::Sqrt(m_afEntry[8] - m_afEntry[0] - m_afEntry[4] + 1.0f);
						fHalfInverse = 0.5f / rAxis.z();
						rAxis.x() = fHalfInverse * m_afEntry[2];
						rAxis.y() = fHalfInverse * m_afEntry[5];
					}
				}
			}
		}
		else
		{
			// The angle is 0 and the matrix is the identity.  Any axis will
			// work, so just use the x-axis.
			rAxis.x() = 1.0;
			rAxis.y() = 0.0;
			rAxis.z() = 0.0;
		}
	}
}
