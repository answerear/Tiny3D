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

namespace Tiny3D
{
    inline void Matrix2::makeZero()
    {
        mTuples[0] = Real(0.0);
        mTuples[1] = Real(0.0);
        mTuples[2] = Real(0.0);
        mTuples[3] = Real(0.0);
    }

    inline void Matrix2::makeIdentity()
    {
        mTuples[0] = Real(1.0);
        mTuples[1] = Real(0.0);
        mTuples[2] = Real(0.0);
        mTuples[3] = Real(1.0);
    }

    inline void Matrix2::makeDiagonal(Real m00, Real m11)
    {
        mTuples[0] = m00;
        mTuples[1] = Real(0.0);
        mTuples[2] = Real(0.0);
        mTuples[3] = m11;
    }

    inline void Matrix2::makeTensorProduct(const Vector2 &rkU, const Vector2 &rkV)
    {
        mTuples[0] = rkU[0] * rkV[0];
        mTuples[1] = rkU[0] * rkV[1];
        mTuples[2] = rkU[1] * rkV[0];
        mTuples[3] = rkU[1] * rkV[1];
    }

    inline void Matrix2::fromAngle(const Radian &radians)
    {
        mTuples[0] = Math::cos(radians);
        mTuples[2] = Math::sin(radians);
        mTuples[1] = -mTuples[2];
        mTuples[3] = mTuples[0];
    }

    inline Radian Matrix2::toAngle() const
    {
        return Math::atan2(mTuples[2], mTuples[0]);
    }

    inline Matrix2::Matrix2(bool isZero /* = true */)
    {
        if (isZero)
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
        memcpy(mTuples, other.mTuples, sizeof(mTuples));
    }

    inline Matrix2::Matrix2(Real m00, Real m01, Real m10, Real m11)
    {
        mTuples[0] = m00;
        mTuples[1] = m01;
        mTuples[2] = m10;
        mTuples[3] = m11;
    }

    inline Matrix2::Matrix2(Real tuple[4], bool isRowMajor)
    {
        if (isRowMajor)
        {
            memcpy(mTuples, tuple, sizeof(mTuples));
        }
        else
        {
            mTuples[0] = tuple[0];
            mTuples[1] = tuple[2];
            mTuples[2] = tuple[1];
            mTuples[3] = tuple[3];
        }
    }

    inline Matrix2::Matrix2(const Vector2 &rkU, const Vector2 &rkV, bool isColumns)
    {
        if (isColumns)
        {
            mTuples[0] = rkU[0];
            mTuples[1] = rkV[0];
            mTuples[2] = rkU[1];
            mTuples[3] = rkV[1];
        }
        else
        {
            mTuples[0] = rkU[0];
            mTuples[1] = rkU[1];
            mTuples[2] = rkV[0];
            mTuples[3] = rkV[1];
        }
    }

    inline Matrix2::Matrix2(const Vector2 *akV, bool isColumns)
    {
        if (isColumns)
        {
            mTuples[0] = akV[0][0];
            mTuples[1] = akV[1][0];
            mTuples[2] = akV[0][1];
            mTuples[3] = akV[1][1];
        }
        else
        {
            mTuples[0] = akV[0][0];
            mTuples[1] = akV[0][1];
            mTuples[2] = akV[1][0];
            mTuples[3] = akV[1][1];
        }
    }

    inline Matrix2::Matrix2(Real m00, Real m11)
    {
        makeDiagonal(m00, m11);
    }

    inline Matrix2::Matrix2(const Radian &rkRadians)
    {
        fromAngle(rkRadians);
    }

    inline Matrix2::Matrix2(const Vector2 &rkU, const Vector2 &rkV)
    {
        makeTensorProduct(rkU, rkV);
    }

    inline Matrix2::operator const Real *() const
    {
        return mTuples;
    }

    inline Matrix2::operator Real *()
    {
        return mTuples;
    }

    inline const Real *Matrix2::operator [](int32_t row) const
    {
        T3D_ASSERT(row >= 0 && row < 2);
        return &mTuples[row << 1];
    }

    inline Real *Matrix2::operator [](int32_t row)
    {
        T3D_ASSERT(row >= 0 && row < 2);
        return &mTuples[row << 1];
    }

    inline Real Matrix2::operator ()(int32_t row, int32_t col) const
    {
        T3D_ASSERT(row >= 0 && row < 2 && col >= 0 && col < 2);
        return mTuples[(row << 1) + col];
    }

    inline Real &Matrix2::operator ()(int32_t row, int32_t col)
    {
        T3D_ASSERT(row >= 0 && row < 2 && col >= 0 && col < 2);
        return mTuples[(row << 1) + col];
    }

    inline void Matrix2::setRow(int32_t row, const Vector2 &rkV)
    {
        T3D_ASSERT(row >= 0 && row < 2);
        int32_t i = row << 1;
        mTuples[i] = rkV[0];
        mTuples[i+1] = rkV[1];
    }

    inline Vector2 Matrix2::getRow(int32_t row) const
    {
        T3D_ASSERT(row >= 0 && row < 2);
        int32_t i = row << 1;
        return Vector2(mTuples[i], mTuples[i+1]);
    }

    inline void Matrix2::setColumn(int32_t col, const Vector2 &rkV)
    {
        T3D_ASSERT(col >= 0 && col < 2);
        mTuples[col] = rkV[0];
        mTuples[2+ col] = rkV[1];
    }

    inline Vector2 Matrix2::getColumn(int32_t col) const
    {
        T3D_ASSERT(col >= 0 && col < 2);
        return Vector2(mTuples[col], mTuples[2+ col]);
    }

    inline void Matrix2::getColumnMajor(Real *columns) const
    {
        columns[0] = mTuples[0];
        columns[1] = mTuples[2];
        columns[2] = mTuples[1];
        columns[3] = mTuples[3];
    }

    inline Matrix2 &Matrix2::operator =(const Matrix2 &other)
    {
        memcpy(mTuples, other.mTuples, sizeof(mTuples));
        return *this;
    }

    inline int32_t Matrix2::compareArrays(const Matrix2 &other) const
    {
        return memcmp(mTuples, other.mTuples, sizeof(mTuples));
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
            mTuples[0] + other.mTuples[0],
            mTuples[1] + other.mTuples[1],
            mTuples[2] + other.mTuples[2],
            mTuples[3] + other.mTuples[3]);
    }

    inline Matrix2 Matrix2::operator -(const Matrix2 &other) const
    {
        return Matrix2(
            mTuples[0] - other.mTuples[0],
            mTuples[1] - other.mTuples[1],
            mTuples[2] - other.mTuples[2],
            mTuples[3] - other.mTuples[3]);
    }

    inline Matrix2 Matrix2::operator *(const Matrix2 &other) const
    {
        return Matrix2(
            mTuples[0] * other.mTuples[0] + mTuples[1] * other.mTuples[2],
            mTuples[0] * other.mTuples[1] + mTuples[1] * other.mTuples[3],
            mTuples[2] * other.mTuples[0] + mTuples[2] * other.mTuples[2],
            mTuples[2] * other.mTuples[1] + mTuples[2] * other.mTuples[3]);
    }

    inline Matrix2 Matrix2::operator *(Real scalar) const
    {
        return Matrix2(mTuples[0] * scalar, mTuples[1] * scalar, 
            mTuples[2] * scalar, mTuples[3] * scalar);
    }

    inline Matrix2 Matrix2::operator /(Real scalar) const
    {
        if (scalar != 0.0)
            return Matrix2(mTuples[0] / scalar, mTuples[1] / scalar,
                mTuples[2] / scalar, mTuples[3] / scalar);
        return ZERO;
    }

    inline Matrix2 Matrix2::operator -() const
    {
        return Matrix2(-mTuples[0], -mTuples[1], -mTuples[2], -mTuples[3]);
    }

    inline Matrix2 &Matrix2::operator +=(const Matrix2 &other)
    {
        mTuples[0] += other.mTuples[0];
        mTuples[1] += other.mTuples[1];
        mTuples[2] += other.mTuples[2];
        mTuples[3] += other.mTuples[3];
        return *this;
    }

    inline Matrix2 &Matrix2::operator -=(const Matrix2 &other)
    {
        mTuples[0] -= other.mTuples[0];
        mTuples[1] -= other.mTuples[1];
        mTuples[2] -= other.mTuples[2];
        mTuples[3] -= other.mTuples[3];
        return *this;
    }

    inline Matrix2 &Matrix2::operator *=(Real fScalar)
    {
        mTuples[0] *= fScalar;
        mTuples[1] *= fScalar;
        mTuples[2] *= fScalar;
        mTuples[3] *= fScalar;
        return *this;
    }

    inline Matrix2 &Matrix2::operator /=(Real scalar)
    {
        if (scalar != 0.0)
        {
            mTuples[0] /= scalar;
            mTuples[1] /= scalar;
            mTuples[2] /= scalar;
            mTuples[3] /= scalar;
        }
        else
        {
            makeZero();
        }
        return *this;
    }

    inline Vector2 Matrix2::operator *(const Vector2 &rkV) const
    {
        return Vector2(mTuples[0] * rkV[0] + mTuples[1] * rkV[1], 
            mTuples[2] * rkV[0] + mTuples[3] * rkV[1]);
    }

    inline Matrix2 Matrix2::transpose() const
    {
        return Matrix2(mTuples[0], mTuples[2], mTuples[1], mTuples[3]);
    }

    inline Matrix2 Matrix2::transposeTimes(const Matrix2 &other) const
    {
        return Matrix2(
            mTuples[0] * other.mTuples[0] + mTuples[2] * other.mTuples[2],
            mTuples[0] * other.mTuples[1] + mTuples[2] * other.mTuples[3],
            mTuples[1] * other.mTuples[0] + mTuples[3] * other.mTuples[2],
            mTuples[1] * other.mTuples[1] + mTuples[3] * other.mTuples[3]);
    }

    inline Matrix2 Matrix2::timesTranspose(const Matrix2 &other) const
    {
        return Matrix2(
            mTuples[0] * other.mTuples[0] + mTuples[1] * other.mTuples[2],
            mTuples[2] * other.mTuples[0] + mTuples[2] * other.mTuples[2],
            mTuples[0] * other.mTuples[1] + mTuples[1] * other.mTuples[3],
            mTuples[2] * other.mTuples[1] + mTuples[2] * other.mTuples[3]);
    }

    inline Matrix2 Matrix2::inverse() const
    {
        Matrix2 mat;
        Real det = mTuples[0] * mTuples[3] - mTuples[1] * mTuples[2];
        if (det != 0.0)
        {
            mat.mTuples[0] = mTuples[3] / det;
            mat.mTuples[1] = -mTuples[1] / det;
            mat.mTuples[2] = -mTuples[2] / det;
            mat.mTuples[3] = mTuples[0] / det;
        }
        return mat;
    }

    inline Matrix2 Matrix2::adjoint() const
    {
        return Matrix2(mTuples[3], -mTuples[1], -mTuples[2], mTuples[0]);
    }

    inline Real Matrix2::determinant() const
    {
        return mTuples[0] * mTuples[3] - mTuples[1] * mTuples[2];
    }

    inline Real Matrix2::qform(const Vector2 &rkU, const Vector2 &rkV) const
    {
        return rkU.dot((*this) * rkV);
    }

    inline void Matrix2::orthonormalize()
    {
        // Algorithm uses Gram-Schmidt orthogonalization.  
        // If 'this' matrix is M = [m0|m1], then orthonormal output matrix is 
        // Q = [q0|q1],
        //
        //   q0 = m0/|m0|
        //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
        //
        // where |V| indicates length of vector V and A*B indicates dot product 
        // of vectors A and B.
        
        // compute q0
        Real len = Math::sqrt(mTuples[0] * mTuples[0] + mTuples[2] * mTuples[2]);
        mTuples[0] /= len;
        mTuples[2] /= len;

        // compute q1
        Real dot0 = mTuples[0] * mTuples[1] + mTuples[2] * mTuples[3];
        mTuples[1] -= dot0 * mTuples[0];
        mTuples[3] -= dot0 * mTuples[2];

        len = Math::sqrt(mTuples[1] * mTuples[1] + mTuples[3] * mTuples[3]);
        mTuples[1] /= len;
        mTuples[3] /= len;
    }

    inline void Matrix2::eigendecomposition(Matrix2 &rRot, Matrix2 &rDiag) const
    {
        Real trace = mTuples[0] + mTuples[3];
        Real diff = mTuples[0] - mTuples[3];
        Real discr = Math::sqrt(diff * diff + ((mTuples[1] * mTuples[1]) * 4));
        Real val0 = (trace - discr) / 2;
        Real val1 = (trace + discr) / 2;
        rDiag.makeDiagonal(val0, val1);
        
        Vector2 kRow0(mTuples[0] - val0, mTuples[1]);
        Vector2 kRow1(mTuples[1], mTuples[3] - val0);
        Real len0 = kRow0.length();
        Real len1 = kRow1.length();
        
        if (len0 > len1)
        {
            if (len0 > Real(0.0))
            {
                rRot.mTuples[0] = kRow0[1];
                rRot.mTuples[1] = kRow0[0];
                rRot.mTuples[2] = -kRow0[0];
                rRot.mTuples[3] = kRow0[1];
            }
            else
            {
                rRot.makeIdentity();
            }
        }
        else
        {
            if (len1 > Real(0.0))
            {
                rRot.mTuples[0] = kRow1[1];
                rRot.mTuples[1] = kRow1[0];
                rRot.mTuples[2] = -kRow1[0];
                rRot.mTuples[3] = kRow1[1];
            }
            else
            {
                rRot.makeIdentity();
            }
        }
    }

    inline Matrix2 operator *(Real scalar, const Matrix2 &rkM)
    {
        return rkM * scalar;
    }

    inline Vector2 operator *(const Vector2 &rkV, const Matrix2 &rkM)
    {
        return Vector2(rkV[0] * rkM[0][0] + rkV[1] * rkM[1][0], 
            rkV[0] * rkM[0][1] + rkV[1] * rkM[1][1]);
    }
}
