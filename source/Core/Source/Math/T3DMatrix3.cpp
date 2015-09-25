

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

	Matrix3 Matrix3::adjoint() const
	{
		// This 3x3 matrix :
		//          +-        -+
		//          | A0 A1 A2 |
		//      A = | A3 A4 A5 |
		//          | A6 A7 A8 |
		//          +-        -+
		//
		// The adjoint of this 3x3 matrix : 
		// 
		//          +-                                  -+
		//          |  | A4 A5 |   | A1 A2 |   | A1 A2 | |
		//          | +| A7 A8 |  -| A7 A8 |  +| A4 A5 | |
		//          |                                    |
		// adj(A) = |  | A3 A5 |   | A0 A2 |   | A0 A2 | |
		//          | -| A6 A8 |  +| A6 A8 |  -| A3 A5 | |
		//          |                                    |
		//          |  | A3 A4 |   | A0 A1 |   | A0 A1 | |
		//          | +| A6 A7 |  -| A6 A7 |  +| A3 A4 | |
		//          +-                                  -+

		Matrix3 matAdjoint;
		Real fSum = 0.0;
		Real fValue = 0.0;

		fSum = m_afEntry[4] * m_afEntry[8];
		fValue = m_afEntry[5] * m_afEntry[7];
		fSum -= fValue;
		matAdjoint[0][0] = fSum;

		fSum = m_afEntry[1] * m_afEntry[8];
		fValue = m_afEntry[2] * m_afEntry[7];
		fSum -= fValue;
		matAdjoint[0][1] = -fSum;

		fSum = m_afEntry[1] * m_afEntry[5];
		fValue = m_afEntry[2] * m_afEntry[4];
		fSum -= fValue;
		matAdjoint[0][2] = fSum;

		fSum = m_afEntry[3] * m_afEntry[8];
		fValue = m_afEntry[5] * m_afEntry[6];
		fSum -= fValue;
		matAdjoint[1][0] = -fSum;

		fSum = m_afEntry[0] * m_afEntry[8];
		fValue = m_afEntry[2] * m_afEntry[6];
		fSum -= fValue;
		matAdjoint[1][1] = fSum;

		fSum = m_afEntry[0] * m_afEntry[5];
		fValue = m_afEntry[2] * m_afEntry[3];
		fSum -= fValue;
		matAdjoint[1][2] = -fSum;

		fSum = m_afEntry[3] * m_afEntry[7];
		fValue = m_afEntry[4] * m_afEntry[6];
		fSum -= fValue;
		matAdjoint[2][0] = fSum;

		fSum = m_afEntry[0] * m_afEntry[7];
		fValue = m_afEntry[1] * m_afEntry[6];
		fSum -= fValue;
		matAdjoint[2][1] = -fSum;

		fSum = m_afEntry[0] * m_afEntry[4];
		fValue = m_afEntry[1] * m_afEntry[3];
		fSum -= fValue;
		matAdjoint[2][2] = fSum;

		return matAdjoint;
	}

	void Matrix3::orthonormalize()
	{
		// Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
		// M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
		//
		//   q0 = m0/|m0|
		//   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
		//   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
		//
		// where |V| indicates length of vector V and A*B indicates dot
		// product of vectors A and B.

		// compute q0
		Real fLength = Math::Sqrt(m_afEntry[0]*m_afEntry[0] + m_afEntry[3]*m_afEntry[3] + m_afEntry[6]*m_afEntry[6]);
		
		m_afEntry[0] /= fLength;
		m_afEntry[3] /= fLength;
		m_afEntry[6] /= fLength;
		
		// compute q1
		Real fDot0 = m_afEntry[0]*m_afEntry[1] + m_afEntry[3]*m_afEntry[4] + m_afEntry[6]*m_afEntry[7];
		
		m_afEntry[1] -= fDot0*m_afEntry[0];
		m_afEntry[4] -= fDot0*m_afEntry[3];
		m_afEntry[7] -= fDot0*m_afEntry[6];
		
		fLength = Math::Sqrt(m_afEntry[1]*m_afEntry[1] + m_afEntry[4]*m_afEntry[4] + m_afEntry[7]*m_afEntry[7]);
		
		m_afEntry[1] /= fLength;
		m_afEntry[4] /= fLength;
		m_afEntry[7] /= fLength;
		
		// compute q2
		Real fDot1 = m_afEntry[1]*m_afEntry[2] + m_afEntry[4]*m_afEntry[5] + m_afEntry[7]*m_afEntry[8];
		
		fDot0 = m_afEntry[0]*m_afEntry[2] + m_afEntry[3]*m_afEntry[5] + m_afEntry[6]*m_afEntry[8];
		
		m_afEntry[2] -= fDot0*m_afEntry[0] + fDot1*m_afEntry[1];
		m_afEntry[5] -= fDot0*m_afEntry[3] + fDot1*m_afEntry[4];
		m_afEntry[8] -= fDot0*m_afEntry[6] + fDot1*m_afEntry[7];
		
		fLength = Math::Sqrt(m_afEntry[2]*m_afEntry[2] + m_afEntry[5]*m_afEntry[5] + m_afEntry[8]*m_afEntry[8]);
		
		m_afEntry[2] /= fLength;
		m_afEntry[5] /= fLength;
		m_afEntry[8] /= fLength;
	}

	bool Matrix3::toEulerAnglesXYZ(Radian &rPitch, Radian &rYaw, Radian &rRoll) const
	{
		//          +-                                      -+
		//          |  cy*cz          -cy*sz           sy    |
		// rot(A) = |  cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx |
		//          | -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy |
		//          +-                                      -+
		
		rYaw = Math::ASin(m_afEntry[2]);

		if (rYaw < Radian(Math::HALF_PI))
		{
			if (rYaw > Radian(-Math::HALF_PI))
			{
				rPitch = Math::ATan2(-m_afEntry[5], m_afEntry[8]);
				rRoll = Math::ATan2(-m_afEntry[1], m_afEntry[0]);
				return true;
			}
			else
			{
				// WARNING. Not unique
				Radian rRmY = Math::ATan2(m_afEntry[3], m_afEntry[4]);
				rRoll = Radian(0.0);
				rPitch = rRoll - rRmY;
				return false;
			}
		}
		else
		{
			Radian rRpY = Math::ATan2(m_afEntry[3], m_afEntry[4]);
			rRoll = Radian(0.0);
			rPitch = rRpY - rRoll;
			return false;
		}

		return true;
	}

	bool Matrix3::toEulerAnglesXZY(Radian &rPitch, Radian &rRoll, Radian &rYaw) const
	{
		//          +-                                      -+
		//	        |  cy*cz          -sz     cz*sy          |
		// rot(A) = |  sx*sy+cx*cy*sz  cx*cz -cy*sx+cx*sy*sz |
		//          | -cx*sy+cy*sx*sz  cz*sx  cx*cy+sx*sy*sz |
		//          +-                                      -+

		rRoll = Math::ASin(-m_afEntry[1]);

		if (rRoll < Radian(Math::HALF_PI))
		{
			if (rRoll > Radian(-Math::HALF_PI))
			{
				rPitch = Math::ATan2(m_afEntry[7], m_afEntry[4]);
				rYaw = Math::ATan2(m_afEntry[2], m_afEntry[0]);
			}
			else
			{
				// WARNING.  Not a unique solution.
				Radian fRmY = Math::ATan2(-m_afEntry[6], m_afEntry[8]);
				rYaw = Radian(0.0);  // any angle works
				rPitch = rYaw - fRmY;
				return false;
			}
		}
		else
		{
			// WARNING.  Not a unique solution.
			Radian fRpY = Math::ATan2(-m_afEntry[6], m_afEntry[8]);
			rYaw = Radian(0.0);  // any angle works
			rPitch = fRpY - rYaw;
			return false;
		}

		return true;
	}

	bool Matrix3::toEulerAnglesYXZ(Radian &rYaw, Radian &rPitch, Radian &rRoll) const
	{
		//          +-                                      -+
		//          |  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy |
		// rot(A) = |  cx*sz           cx*cz          -sx    |
		//          | -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy |
		//          +-                                      -+

		rPitch = Math::ASin(-m_afEntry[4]);

		if (rPitch < Radian(Math::HALF_PI))
		{
			if (rPitch > Radian(-Math::HALF_PI))
			{
				rYaw = Math::ATan2(m_afEntry[2], m_afEntry[8]);
				rRoll = Math::ATan2(m_afEntry[3], m_afEntry[4]);
			}
			else
			{
				// WARNING.  Not a unique solution.
				Radian fRmY = Math::ATan2(-m_afEntry[1], m_afEntry[0]);
				rRoll = Radian(0.0);  // any angle works
				rYaw = rRoll - fRmY;
				return false;
			}
		}
		else
		{
			// WARNING.  Not a unique solution.
			Radian fRpY = Math::ATan2(-m_afEntry[1], m_afEntry[0]);
			rRoll = Radian(0.0);  // any angle works
			rYaw = fRpY - rRoll;
			return false;
		}

		return true;
	}

	bool Matrix3::toEulerAnglesYZX(Radian &rYaw, Radian &rRoll, Radian &rPitch) const
	{
		//          +-                                      -+
		//          |  cy*cz  sx*sy-cx*cy*sz  cx*sy+cy*sx*sz |
		// rot(A) = |  sz     cx*cz          -cz*sx          |
		//          | -cz*sy  cy*sx+cx*sy*sz  cx*cy-sx*sy*sz |
		//          +-                                      -+

		rRoll = Math::ASin(m_afEntry[3]);

		if (rRoll < Radian(Math::HALF_PI))
		{
			if (rRoll > Radian(-Math::HALF_PI))
			{
				rYaw = Math::ATan2(-m_afEntry[6], m_afEntry[0]);
				rPitch = Math::ATan2(-m_afEntry[5], m_afEntry[4]);
				return true;
			}
			else
			{
				// WARNING.  Not a unique solution.
				Radian fRmY = Math::ATan2(m_afEntry[7], m_afEntry[8]);
				rPitch = Radian(0.0);  // any angle works
				rYaw = rPitch - fRmY;
				return false;
			}
		}
		else
		{
			// WARNING.  Not a unique solution.
			Radian fRpY = Math::ATan2(m_afEntry[7], m_afEntry[8]);
			rPitch = Radian(0.0);  // any angle works
			rYaw = fRpY - rPitch;
			return false;
		}

		return true;
	}

	bool Matrix3::toEulerAnglesZXY(Radian &rRoll, Radian &rPitch, Radian &rYaw) const
	{
		//          +-                                     -+
		//          | cy*cz-sx*sy*sz -cx*sz  cz*sy+cy*sx*sz |
		// rot(A) = | cz*sx*sy+cy*sz  cx*cz -cy*cz*sx+sy*sz |
		//          | -cx*sy          sx              cx*cy |
		//          +-                                     -+

		rPitch = Math::ASin(m_afEntry[7]);

		if (rPitch < Radian(Math::HALF_PI))
		{
			if (rPitch > Radian(-Math::HALF_PI))
			{
				rRoll = Math::ATan2(-m_afEntry[1], m_afEntry[4]);
				rYaw = Math::ATan2(-m_afEntry[6], m_afEntry[8]);
			}
			else
			{
				// WARNING.  Not a unique solution.
				Radian fRmY = Math::ATan2(m_afEntry[2], m_afEntry[0]);
				rYaw = Radian(0.0);  // any angle works
				rRoll = rYaw - fRmY;
				return false;
			}
		}
		else
		{
			// WARNING.  Not a unique solution.
			Radian fRpY = Math::ATan2(m_afEntry[2], m_afEntry[0]);
			rYaw = Radian(0.0);  // any angle works
			rRoll = fRpY - rYaw;
			return false;
		}

		return true;
	}

	bool Matrix3::toEulerAnglesZYX(Radian &rRoll, Radian &rYaw, Radian &rPitch) const
	{
		//          +-                                     -+
		//          | cy*cz  cz*sx*sy-cx*sz  cx*cz*sy+sx*sz |
		// rot(A) = | cy*sz  cx*cz+sx*sy*sz -cz*sx+cx*sy*sz |
		//          | -sy             cy*sx           cx*cy |
		//          +-                                     -+

		rYaw = Math::ASin(-m_afEntry[6]);
		if (rYaw < Radian(Math::HALF_PI))
		{
			if (rYaw > Radian(-Math::HALF_PI))
			{
				rRoll = Math::ATan2(m_afEntry[3], m_afEntry[0]);
				rPitch = Math::ATan2(m_afEntry[7], m_afEntry[8]);
			}
			else
			{
				// WARNING.  Not a unique solution.
				Radian fRmY = Math::ATan2(-m_afEntry[1], m_afEntry[2]);
				rPitch = Radian(0.0);  // any angle works
				rRoll = rPitch - fRmY;
				return false;
			}
		}
		else
		{
			// WARNING.  Not a unique solution.
			Radian fRpY = Math::ATan2(-m_afEntry[1], m_afEntry[2]);
			rPitch = Radian(0.0);  // any angle works
			rRoll = fRpY - rPitch;
			return false;
		}

		return true;
	}
}
