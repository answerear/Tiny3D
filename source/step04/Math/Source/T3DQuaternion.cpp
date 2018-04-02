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

#include "T3DQuaternion.h"


namespace Tiny3D
{
    const Quaternion Quaternion::IDENTITY(1.0, 0.0, 0.0, 0.0);
    const Quaternion Quaternion::ZERO(0.0, 0.0, 0.0, 0.0);


    void Quaternion::fromRotationMatrix(const Matrix3 &rkRot)
    {
        // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
        // article "Quaternion Calculus and Fast Animation".

        Real fTrace = rkRot[0][0] + rkRot[1][1] + rkRot[2][2];
        Real fRoot;

        if (fTrace > REAL_ZERO)
        {
            // |w| > 1/2, may as well choose w > 1/2
            fRoot = Math::sqrt(fTrace + REAL_ONE);  // 2w
            _w = REAL_HALF * fRoot;
            fRoot = REAL_HALF / fRoot;  // 1/(4w)
            _x = (rkRot[2][1] - rkRot[1][2]) * fRoot;
            _y = (rkRot[0][2] - rkRot[2][0]) * fRoot;
            _z = (rkRot[1][0] - rkRot[0][1]) * fRoot;
        }
        else
        {
            // |w| <= 1/2
            static size_t s_iNext[3] = {1, 2, 0};
            size_t i = 0;
            if (rkRot[1][1] > rkRot[0][0])
                i = 1;
            if (rkRot[2][2] > rkRot[i][i])
                i = 2;
            size_t j = s_iNext[i];
            size_t k = s_iNext[j];

            fRoot = Math::sqrt(rkRot[i][i] - rkRot[j][j] - rkRot[k][k] + REAL_ONE);
            Real* apkQuat[3] = {&_x, &_y, &_z};
            *apkQuat[i] = 0.5f * fRoot;
            fRoot = 0.5f / fRoot;
            _w = (rkRot[k][j] - rkRot[j][k]) * fRoot;
            *apkQuat[j] = (rkRot[j][i] + rkRot[i][j]) * fRoot;
            *apkQuat[k] = (rkRot[k][i] + rkRot[i][k]) * fRoot;
        }
    }

    void Quaternion::toAngleAxis(Radian &rAngle, Vector3 &rAxis) const
    {
        // The quaternion representing the rotation is
        //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

        Real fSqrLength = _x * _x + _y * _y + _z * _z;
        if (fSqrLength > REAL_ZERO)
        {
            rAngle = Real(2.0) * Math::acos(_w).valueRadians();
            Real fInvLength = REAL_ONE / Math::sqrt(fSqrLength);
            rAxis.x() = _x * fInvLength;
            rAxis.y() = _y * fInvLength;
            rAxis.z() = _z * fInvLength;
        }
        else
        {
            // angle is 0 (mod 2*pi), so any axis will do
            rAngle = Radian(0.0);
            rAxis.x() = REAL_ONE;
            rAxis.y() = REAL_ZERO;
            rAxis.z() = REAL_ZERO;
        }
    }

    void Quaternion::toRotationMatrix(Matrix3 &rRot) const
    {
        Real fTx  = _x + _x;
        Real fTy  = _y + _y;
        Real fTz  = _z + _z;
        Real fTwx = fTx * _w;
        Real fTwy = fTy * _w;
        Real fTwz = fTz * _w;
        Real fTxx = fTx * _x;
        Real fTxy = fTy * _x;
        Real fTxz = fTz * _x;
        Real fTyy = fTy * _y;
        Real fTyz = fTz * _y;
        Real fTzz = fTz * _z;

        rRot[0][0] = REAL_ONE - (fTyy + fTzz);
        rRot[0][1] = fTxy - fTwz;
        rRot[0][2] = fTxz + fTwy;
        rRot[1][0] = fTxy + fTwz;
        rRot[1][1] = REAL_ONE - (fTxx + fTzz);
        rRot[1][2] = fTyz - fTwx;
        rRot[2][0] = fTxz - fTwy;
        rRot[2][1] = fTyz + fTwx;
        rRot[2][2] = REAL_ONE - (fTxx + fTyy);
    }

    Quaternion &Quaternion::slerp(const Quaternion &rkP, const Quaternion &rkQ, 
        Real times, bool shortestPath, Real threshold /* = 1e-03 */)
    {
        Real fCos = rkP.dot(rkQ);
        Quaternion rkT;

        // Do we need to invert rotation?
        if (fCos < REAL_ZERO && shortestPath)
        {
            fCos = -fCos;
            rkT = -rkQ;
        }
        else
        {
            rkT = rkQ;
        }

        if (Math::abs(fCos) < 1 - threshold)
        {
            // Standard case (slerp)
            Real fSin = Math::sqrt(1 - Math::sqr(fCos));
            Radian fAngle = Math::atan2(fSin, fCos);
            Real fInvSin = REAL_ONE / fSin;
            Real fCoeff0 = Math::sin((REAL_ONE - times) * fAngle) * fInvSin;
            Real fCoeff1 = Math::sin(times * fAngle) * fInvSin;
            *this = fCoeff0 * rkP + fCoeff1 * rkT;
        }
        else
        {
            // There are two situations:
            // 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a 
            //    linear interpolation safely.
            // 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), 
            //    there are an infinite number of possibilities interpolation. 
            //    but we haven't have method to fix this case, so just use 
            //    linear interpolation here.
            Quaternion t = (1.0f - times) * rkP + times * rkT;
            // taking the complement requires renormalisation
            t.normalize();
            *this = t;
        }

        return *this;
    }
}
