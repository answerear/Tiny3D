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

        if (fTrace > 0.0)
        {
            // |w| > 1/2, may as well choose w > 1/2
            fRoot = Math::sqrt(fTrace + 1.0f);  // 2w
            _w = 0.5f * fRoot;
            fRoot = 0.5f / fRoot;  // 1/(4w)
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

            fRoot = Math::sqrt(rkRot[i][i] - rkRot[j][j] - rkRot[k][k] + 1.0f);
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
        if (fSqrLength > 0.0)
        {
            rAngle = Real(2.0) * Math::acos(_w).valueRadians();
            Real fInvLength = Real(1.0) / Math::sqrt(fSqrLength);
            rAxis.x() = _x * fInvLength;
            rAxis.y() = _y * fInvLength;
            rAxis.z() = _z * fInvLength;
        }
        else
        {
            // angle is 0 (mod 2*pi), so any axis will do
            rAngle = Radian(0.0);
            rAxis.x() = 1.0;
            rAxis.y() = 0.0;
            rAxis.z() = 0.0;
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

        rRot[0][0] = 1.0f - (fTyy + fTzz);
        rRot[0][1] = fTxy - fTwz;
        rRot[0][2] = fTxz + fTwy;
        rRot[1][0] = fTxy + fTwz;
        rRot[1][1] = 1.0f - (fTxx + fTzz);
        rRot[1][2] = fTyz - fTwx;
        rRot[2][0] = fTxz - fTwy;
        rRot[2][1] = fTyz + fTwx;
        rRot[2][2] = 1.0f - (fTxx + fTyy);
    }

    Radian Quaternion::getPitch(bool reprojectAxis /* = true */) const
    {
        if (reprojectAxis)
        {
            // pitch = atan2(localy.z, localy.y)
            // pick parts of yAxis() implementation that we need
            Real fTx  = 2.0f * _x;
            //          Real fTy  = 2.0f*y;
            Real fTz  = 2.0f * _z;
            Real fTwx = fTx * _w;
            Real fTxx = fTx * _x;
            Real fTyz = fTz * _y;
            Real fTzz = fTz * _z;

            // Vector3(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
            return Radian(Math::atan2(fTyz+fTwx, 1.0f-(fTxx+fTzz)));
        }
        else
        {
            // internal version
            return Radian(Math::atan2(2*(_y*_z + _w*_x), _w*_w - _x*_x - _y*_y + _z*_z));
        }
    }

    Radian Quaternion::getYaw(bool reprojectAxis) const
    {
        if (reprojectAxis)
        {
            // yaw = atan2(localz.x, localz.z)
            // pick parts of zAxis() implementation that we need
            Real fTx  = 2.0f * _x;
            Real fTy  = 2.0f * _y;
            Real fTz  = 2.0f * _z;
            Real fTwy = fTy * _w;
            Real fTxx = fTx * _x;
            Real fTxz = fTz * _x;
            Real fTyy = fTy * _y;

            // Vector3(fTxz+fTwy, fTyz-fTwx, 1.0-(fTxx+fTyy));

            return Radian(Math::atan2(fTxz+fTwy, 1.0f-(fTxx+fTyy)));
        }
        else
        {
            // internal version
            return Radian(Math::asin(-2*(_x*_z - _w*_y)));
        }
    }

    Radian Quaternion::getRoll(bool reprojectAxis) const
    {
        if (reprojectAxis)
        {
            // roll = atan2(localx.y, localx.x)
            // pick parts of xAxis() implementation that we need
            //          Real fTx  = 2.0*x;
            Real fTy  = 2.0f * _y;
            Real fTz  = 2.0f * _z;
            Real fTwz = fTz * _w;
            Real fTxy = fTy * _x;
            Real fTyy = fTy * _y;
            Real fTzz = fTz * _z;

            // Vector3(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);

            return Radian(Math::atan2(fTxy+fTwz, 1.0f-(fTyy+fTzz)));
        }
        else
        {
            return Radian(Math::atan2(2*(_x*_y + _w*_z), _w*_w + _x*_x - _y*_y - _z*_z));
        }
    }

    Quaternion &Quaternion::slerp(const Quaternion &rkP, const Quaternion &rkQ, Real fTimes, bool shortestPath, Real fThreshold /* = 1e-03 */)
    {
        Real fCos = rkP.dot(rkQ);
        Quaternion rkT;

        // Do we need to invert rotation?
        if (fCos < 0.0f && shortestPath)
        {
            fCos = -fCos;
            rkT = -rkQ;
        }
        else
        {
            rkT = rkQ;
        }

        if (Math::abs(fCos) < 1 - fThreshold)
        {
            // Standard case (slerp)
            Real fSin = Math::sqrt(1 - Math::sqr(fCos));
            Radian fAngle = Math::atan2(fSin, fCos);
            Real fInvSin = 1.0f / fSin;
            Real fCoeff0 = Math::sin((1.0f - fTimes) * fAngle) * fInvSin;
            Real fCoeff1 = Math::sin(fTimes * fAngle) * fInvSin;
            *this = fCoeff0 * rkP + fCoeff1 * rkT;
        }
        else
        {
            // There are two situations:
            // 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
            //    interpolation safely.
            // 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
            //    are an infinite number of possibilities interpolation. but we haven't
            //    have method to fix this case, so just use linear interpolation here.
            Quaternion t = (1.0f - fTimes) * rkP + fTimes * rkT;
            // taking the complement requires renormalisation
            t.normalize();
            *this = t;
        }

        return *this;
    }
}
