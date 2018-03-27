/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

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

        Real fTrace = mTuples[0] + mTuples[4] + mTuples[8];
        Real fCos = 0.5f * (fTrace - 1.0f);
        rRadians = Math::acos(fCos);  // in [0,PI]

        if (rRadians > Radian(0.0))
        {
            if (rRadians < Radian(Math::PI))
            {
                rAxis.x() = mTuples[7] - mTuples[5];
                rAxis.y() = mTuples[2] - mTuples[6];
                rAxis.z() = mTuples[3] - mTuples[1];
                rAxis.normalize();
            }
            else
            {
                // angle is PI
                float fHalfInverse;
                if (mTuples[0] >= mTuples[4])
                {
                    // r00 >= r11
                    if (mTuples[0] >= mTuples[8])
                    {
                        // r00 is maximum diagonal term
                        rAxis.x() = 0.5f * Math::sqrt(mTuples[0] - mTuples[4] - mTuples[8] + 1.0f);
                        fHalfInverse = 0.5f / rAxis.x();
                        rAxis.y() = fHalfInverse * mTuples[1];
                        rAxis.z() = fHalfInverse * mTuples[2];
                    }
                    else
                    {
                        // r22 is maximum diagonal term
                        rAxis.z() = 0.5f * Math::sqrt(mTuples[8] - mTuples[0] - mTuples[4] + 1.0f);
                        fHalfInverse = 0.5f / rAxis.z();
                        rAxis.x() = fHalfInverse * mTuples[2];
                        rAxis.y() = fHalfInverse * mTuples[5];
                    }
                }
                else
                {
                    // r11 > r00
                    if ( mTuples[4] >= mTuples[8] )
                    {
                        // r11 is maximum diagonal term
                        rAxis.y() = 0.5f * Math::sqrt(mTuples[4] - mTuples[0] - mTuples[8] + 1.0f);
                        fHalfInverse  = 0.5f / rAxis.y();
                        rAxis.x() = fHalfInverse * mTuples[1];
                        rAxis.z() = fHalfInverse * mTuples[5];
                    }
                    else
                    {
                        // r22 is maximum diagonal term
                        rAxis.z() = 0.5f * Math::sqrt(mTuples[8] - mTuples[0] - mTuples[4] + 1.0f);
                        fHalfInverse = 0.5f / rAxis.z();
                        rAxis.x() = fHalfInverse * mTuples[2];
                        rAxis.y() = fHalfInverse * mTuples[5];
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

        fSum = mTuples[4] * mTuples[8];
        fValue = mTuples[5] * mTuples[7];
        fSum -= fValue;
        matAdjoint[0][0] = fSum;

        fSum = mTuples[1] * mTuples[8];
        fValue = mTuples[2] * mTuples[7];
        fSum -= fValue;
        matAdjoint[0][1] = -fSum;

        fSum = mTuples[1] * mTuples[5];
        fValue = mTuples[2] * mTuples[4];
        fSum -= fValue;
        matAdjoint[0][2] = fSum;

        fSum = mTuples[3] * mTuples[8];
        fValue = mTuples[5] * mTuples[6];
        fSum -= fValue;
        matAdjoint[1][0] = -fSum;

        fSum = mTuples[0] * mTuples[8];
        fValue = mTuples[2] * mTuples[6];
        fSum -= fValue;
        matAdjoint[1][1] = fSum;

        fSum = mTuples[0] * mTuples[5];
        fValue = mTuples[2] * mTuples[3];
        fSum -= fValue;
        matAdjoint[1][2] = -fSum;

        fSum = mTuples[3] * mTuples[7];
        fValue = mTuples[4] * mTuples[6];
        fSum -= fValue;
        matAdjoint[2][0] = fSum;

        fSum = mTuples[0] * mTuples[7];
        fValue = mTuples[1] * mTuples[6];
        fSum -= fValue;
        matAdjoint[2][1] = -fSum;

        fSum = mTuples[0] * mTuples[4];
        fValue = mTuples[1] * mTuples[3];
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
        Real fLength = Math::sqrt(mTuples[0]*mTuples[0] + mTuples[3]*mTuples[3] + mTuples[6]*mTuples[6]);

        mTuples[0] /= fLength;
        mTuples[3] /= fLength;
        mTuples[6] /= fLength;

        // compute q1
        Real fDot0 = mTuples[0]*mTuples[1] + mTuples[3]*mTuples[4] + mTuples[6]*mTuples[7];

        mTuples[1] -= fDot0*mTuples[0];
        mTuples[4] -= fDot0*mTuples[3];
        mTuples[7] -= fDot0*mTuples[6];

        fLength = Math::sqrt(mTuples[1]*mTuples[1] + mTuples[4]*mTuples[4] + mTuples[7]*mTuples[7]);

        mTuples[1] /= fLength;
        mTuples[4] /= fLength;
        mTuples[7] /= fLength;

        // compute q2
        Real fDot1 = mTuples[1]*mTuples[2] + mTuples[4]*mTuples[5] + mTuples[7]*mTuples[8];

        fDot0 = mTuples[0]*mTuples[2] + mTuples[3]*mTuples[5] + mTuples[6]*mTuples[8];

        mTuples[2] -= fDot0*mTuples[0] + fDot1*mTuples[1];
        mTuples[5] -= fDot0*mTuples[3] + fDot1*mTuples[4];
        mTuples[8] -= fDot0*mTuples[6] + fDot1*mTuples[7];

        fLength = Math::sqrt(mTuples[2]*mTuples[2] + mTuples[5]*mTuples[5] + mTuples[8]*mTuples[8]);

        mTuples[2] /= fLength;
        mTuples[5] /= fLength;
        mTuples[8] /= fLength;
    }

    bool Matrix3::toEulerAnglesXYZ(Radian &rPitch, Radian &rYaw, Radian &rRoll) const
    {
        //          +-                                      -+
        //          |  cy*cz          -cy*sz           sy    |
        // rot(A) = |  cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx |
        //          | -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy |
        //          +-                                      -+

        rYaw = Math::asin(mTuples[2]);

        if (rYaw < Radian(Math::HALF_PI))
        {
            if (rYaw > Radian(-Math::HALF_PI))
            {
                rPitch = Math::atan2(-mTuples[5], mTuples[8]);
                rRoll = Math::atan2(-mTuples[1], mTuples[0]);
                return true;
            }
            else
            {
                // WARNING. Not unique
                Radian rRmY = Math::atan2(mTuples[3], mTuples[4]);
                rRoll = Radian(0.0);
                rPitch = rRoll - rRmY;
                return false;
            }
        }
        else
        {
            Radian rRpY = Math::atan2(mTuples[3], mTuples[4]);
            rRoll = Radian(0.0);
            rPitch = rRpY - rRoll;
            return false;
        }

        return true;
    }

    bool Matrix3::toEulerAnglesXZY(Radian &rPitch, Radian &rRoll, Radian &rYaw) const
    {
        //          +-                                      -+
        //          |  cy*cz          -sz     cz*sy          |
        // rot(A) = |  sx*sy+cx*cy*sz  cx*cz -cy*sx+cx*sy*sz |
        //          | -cx*sy+cy*sx*sz  cz*sx  cx*cy+sx*sy*sz |
        //          +-                                      -+

        rRoll = Math::asin(-mTuples[1]);

        if (rRoll < Radian(Math::HALF_PI))
        {
            if (rRoll > Radian(-Math::HALF_PI))
            {
                rPitch = Math::atan2(mTuples[7], mTuples[4]);
                rYaw = Math::atan2(mTuples[2], mTuples[0]);
            }
            else
            {
                // WARNING.  Not a unique solution.
                Radian fRmY = Math::atan2(-mTuples[6], mTuples[8]);
                rYaw = Radian(0.0);  // any angle works
                rPitch = rYaw - fRmY;
                return false;
            }
        }
        else
        {
            // WARNING.  Not a unique solution.
            Radian fRpY = Math::atan2(-mTuples[6], mTuples[8]);
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

        rPitch = Math::asin(-mTuples[4]);

        if (rPitch < Radian(Math::HALF_PI))
        {
            if (rPitch > Radian(-Math::HALF_PI))
            {
                rYaw = Math::atan2(mTuples[2], mTuples[8]);
                rRoll = Math::atan2(mTuples[3], mTuples[4]);
            }
            else
            {
                // WARNING.  Not a unique solution.
                Radian fRmY = Math::atan2(-mTuples[1], mTuples[0]);
                rRoll = Radian(0.0);  // any angle works
                rYaw = rRoll - fRmY;
                return false;
            }
        }
        else
        {
            // WARNING.  Not a unique solution.
            Radian fRpY = Math::atan2(-mTuples[1], mTuples[0]);
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

        rRoll = Math::asin(mTuples[3]);

        if (rRoll < Radian(Math::HALF_PI))
        {
            if (rRoll > Radian(-Math::HALF_PI))
            {
                rYaw = Math::atan2(-mTuples[6], mTuples[0]);
                rPitch = Math::atan2(-mTuples[5], mTuples[4]);
                return true;
            }
            else
            {
                // WARNING.  Not a unique solution.
                Radian fRmY = Math::atan2(mTuples[7], mTuples[8]);
                rPitch = Radian(0.0);  // any angle works
                rYaw = rPitch - fRmY;
                return false;
            }
        }
        else
        {
            // WARNING.  Not a unique solution.
            Radian fRpY = Math::atan2(mTuples[7], mTuples[8]);
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

        rPitch = Math::asin(mTuples[7]);

        if (rPitch < Radian(Math::HALF_PI))
        {
            if (rPitch > Radian(-Math::HALF_PI))
            {
                rRoll = Math::atan2(-mTuples[1], mTuples[4]);
                rYaw = Math::atan2(-mTuples[6], mTuples[8]);
            }
            else
            {
                // WARNING.  Not a unique solution.
                Radian fRmY = Math::atan2(mTuples[2], mTuples[0]);
                rYaw = Radian(0.0);  // any angle works
                rRoll = rYaw - fRmY;
                return false;
            }
        }
        else
        {
            // WARNING.  Not a unique solution.
            Radian fRpY = Math::atan2(mTuples[2], mTuples[0]);
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

        rYaw = Math::asin(-mTuples[6]);
        if (rYaw < Radian(Math::HALF_PI))
        {
            if (rYaw > Radian(-Math::HALF_PI))
            {
                rRoll = Math::atan2(mTuples[3], mTuples[0]);
                rPitch = Math::atan2(mTuples[7], mTuples[8]);
            }
            else
            {
                // WARNING.  Not a unique solution.
                Radian fRmY = Math::atan2(-mTuples[1], mTuples[2]);
                rPitch = Radian(0.0);  // any angle works
                rRoll = rPitch - fRmY;
                return false;
            }
        }
        else
        {
            // WARNING.  Not a unique solution.
            Radian fRpY = Math::atan2(-mTuples[1], mTuples[2]);
            rPitch = Radian(0.0);  // any angle works
            rRoll = fRpY - rPitch;
            return false;
        }

        return true;
    }

    void Matrix3::QDUDecomposition (Matrix3& kQ,
                                    Vector3& kD, Vector3& kU) const
    {
        // Factor M = QR = QDU where Q is orthogonal, D is diagonal,
        // and U is upper triangular with ones on its diagonal.  Algorithm uses
        // Gram-Schmidt orthogonalization (the QR algorithm).
        //
        // If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
        //
        //   q0 = m0/|m0|
        //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
        //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
        //
        // where |V| indicates length of vector V and A*B indicates dot
        // product of vectors A and B.  The matrix R has entries
        //
        //   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
        //   r10 = 0      r11 = q1*m1  r12 = q1*m2
        //   r20 = 0      r21 = 0      r22 = q2*m2
        //
        // so D = diag(r00,r11,r22) and U has entries u01 = r01/r00,
        // u02 = r02/r00, and u12 = r12/r11.

        // Q = rotation
        // D = scaling
        // U = shear

        // D stores the three diagonal entries r00, r11, r22
        // U stores the entries U[0] = u01, U[1] = u02, U[2] = u12

        // build orthogonal matrix Q
        Real fInvLength = Math::invSqrt(m3x3[0][0]*m3x3[0][0] + m3x3[1][0]*m3x3[1][0] + m3x3[2][0]*m3x3[2][0]);

        kQ[0][0] = m3x3[0][0]*fInvLength;
        kQ[1][0] = m3x3[1][0]*fInvLength;
        kQ[2][0] = m3x3[2][0]*fInvLength;

        Real fDot = kQ[0][0]*m3x3[0][1] + kQ[1][0]*m3x3[1][1] +
                    kQ[2][0]*m3x3[2][1];
        kQ[0][1] = m3x3[0][1]-fDot*kQ[0][0];
        kQ[1][1] = m3x3[1][1]-fDot*kQ[1][0];
        kQ[2][1] = m3x3[2][1]-fDot*kQ[2][0];
        fInvLength = Math::invSqrt(kQ[0][1]*kQ[0][1] + kQ[1][1]*kQ[1][1] + kQ[2][1]*kQ[2][1]);

        kQ[0][1] *= fInvLength;
        kQ[1][1] *= fInvLength;
        kQ[2][1] *= fInvLength;

        fDot = kQ[0][0]*m3x3[0][2] + kQ[1][0]*m3x3[1][2] +
               kQ[2][0]*m3x3[2][2];
        kQ[0][2] = m3x3[0][2]-fDot*kQ[0][0];
        kQ[1][2] = m3x3[1][2]-fDot*kQ[1][0];
        kQ[2][2] = m3x3[2][2]-fDot*kQ[2][0];
        fDot = kQ[0][1]*m3x3[0][2] + kQ[1][1]*m3x3[1][2] +
               kQ[2][1]*m3x3[2][2];
        kQ[0][2] -= fDot*kQ[0][1];
        kQ[1][2] -= fDot*kQ[1][1];
        kQ[2][2] -= fDot*kQ[2][1];
        fInvLength = Math::invSqrt(kQ[0][2]*kQ[0][2] + kQ[1][2]*kQ[1][2] + kQ[2][2]*kQ[2][2]);

        kQ[0][2] *= fInvLength;
        kQ[1][2] *= fInvLength;
        kQ[2][2] *= fInvLength;

        // guarantee that orthogonal matrix has determinant 1 (no reflections)
        Real fDet = kQ[0][0]*kQ[1][1]*kQ[2][2] + kQ[0][1]*kQ[1][2]*kQ[2][0] +
                    kQ[0][2]*kQ[1][0]*kQ[2][1] - kQ[0][2]*kQ[1][1]*kQ[2][0] -
                    kQ[0][1]*kQ[1][0]*kQ[2][2] - kQ[0][0]*kQ[1][2]*kQ[2][1];

        if ( fDet < 0.0 )
        {
            for (size_t iRow = 0; iRow < 3; iRow++)
                for (size_t iCol = 0; iCol < 3; iCol++)
                    kQ[iRow][iCol] = -kQ[iRow][iCol];
        }

        // build "right" matrix R
        Matrix3 kR;
        kR[0][0] = kQ[0][0]*m3x3[0][0] + kQ[1][0]*m3x3[1][0] +
                   kQ[2][0]*m3x3[2][0];
        kR[0][1] = kQ[0][0]*m3x3[0][1] + kQ[1][0]*m3x3[1][1] +
                   kQ[2][0]*m3x3[2][1];
        kR[1][1] = kQ[0][1]*m3x3[0][1] + kQ[1][1]*m3x3[1][1] +
                   kQ[2][1]*m3x3[2][1];
        kR[0][2] = kQ[0][0]*m3x3[0][2] + kQ[1][0]*m3x3[1][2] +
                   kQ[2][0]*m3x3[2][2];
        kR[1][2] = kQ[0][1]*m3x3[0][2] + kQ[1][1]*m3x3[1][2] +
                   kQ[2][1]*m3x3[2][2];
        kR[2][2] = kQ[0][2]*m3x3[0][2] + kQ[1][2]*m3x3[1][2] +
                   kQ[2][2]*m3x3[2][2];

        // the scaling component
        kD[0] = kR[0][0];
        kD[1] = kR[1][1];
        kD[2] = kR[2][2];

        // the shear component
        Real fInvD0 = 1.0f/kD[0];
        kU[0] = kR[0][1]*fInvD0;
        kU[1] = kR[0][2]*fInvD0;
        kU[2] = kR[1][2]/kD[1];
    }
}
