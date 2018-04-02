

namespace Tiny3D
{
    inline void Matrix3::makeZero()
    {
        mTuples[0] = mTuples[1] = mTuples[2] = Real(0.0);
        mTuples[3] = mTuples[4] = mTuples[5] = Real(0.0);
        mTuples[6] = mTuples[7] = mTuples[8] = Real(0.0);
    }

    inline void Matrix3::makeIdentity()
    {
        mTuples[0] = Real(1.0), mTuples[1] = Real(0.0), mTuples[2] = Real(0.0);
        mTuples[3] = Real(0.0), mTuples[4] = Real(1.0), mTuples[5] = Real(0.0);
        mTuples[6] = Real(0.0), mTuples[7] = Real(0.0), mTuples[8] = Real(1.0);
    }

    inline void Matrix3::makeDiagonal(Real m00, Real m11, Real m22)
    {
        mTuples[0] = m00, mTuples[1] = Real(0.0), mTuples[2] = Real(0.0);
        mTuples[3] = Real(0.0), mTuples[4] = m11, mTuples[5] = Real(0.0);
        mTuples[6] = Real(0.0), mTuples[7] = Real(0.0), mTuples[8] = m22;
    }

    inline void Matrix3::makeTensorProduct(const Vector3 &rkU, 
        const Vector3 &rkV)
    {
        mTuples[0] = rkU[0] * rkV[0];
        mTuples[1] = rkU[0] * rkV[1];
        mTuples[2] = rkU[0] * rkV[2];
        mTuples[3] = rkU[1] * rkV[0];
        mTuples[4] = rkU[1] * rkV[1];
        mTuples[5] = rkU[1] * rkV[2];
        mTuples[6] = rkU[2] * rkV[0];
        mTuples[7] = rkU[2] * rkV[1];
        mTuples[8] = rkU[2] * rkV[2];
    }

    inline void Matrix3::fromAxisAngle(const Vector3 &rkAxis, 
        const Radian &radians)
    {
        Real fCos = Math::cos(radians);
        Real fSin = Math::sin(radians);
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

        mTuples[0] = fX2 * fOneMinusCos + fCos;
        mTuples[1] = fXYM - fZSin;
        mTuples[2] = fXZM + fYSin;
        mTuples[3] = fXYM + fZSin;
        mTuples[4] = fY2 * fOneMinusCos + fCos;
        mTuples[5] = fYZM - fXSin;
        mTuples[6] = fXZM - fYSin;
        mTuples[7] = fYZM + fXSin;
        mTuples[8] = fZ2 * fOneMinusCos + fCos;
    }

    inline Matrix3::Matrix3(bool isZero /* = true */)
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

    inline Matrix3::Matrix3(const Matrix3 &other)
    {
        memcpy(mTuples, other.mTuples, sizeof(mTuples));
    }

    inline Matrix3::Matrix3(
        Real m00, Real m01, Real m02, 
        Real m10, Real m11, Real m12, 
        Real m20, Real m21, Real m22)
    {
        mTuples[0] = m00;
        mTuples[1] = m01;
        mTuples[2] = m02;
        mTuples[3] = m10;
        mTuples[4] = m11;
        mTuples[5] = m12;
        mTuples[6] = m20;
        mTuples[7] = m21;
        mTuples[8] = m22;
    }

    inline Matrix3::Matrix3(Real tuples[9], bool isRowMajor)
    {
        if (isRowMajor)
        {
            memcpy(mTuples, tuples, sizeof(mTuples));
        }
        else
        {
            mTuples[0] = tuples[0];
            mTuples[1] = tuples[3];
            mTuples[2] = tuples[6];
            mTuples[3] = tuples[1];
            mTuples[4] = tuples[4];
            mTuples[5] = tuples[7];
            mTuples[6] = tuples[2];
            mTuples[7] = tuples[5];
            mTuples[8] = tuples[8];
        }
    }

    inline Matrix3::Matrix3(const Vector3 &rkU, const Vector3 &rkV, 
        const Vector3 &rkW, bool isColumns)
    {
        if (isColumns)
        {
            mTuples[0] = rkU[0];
            mTuples[1] = rkV[0];
            mTuples[2] = rkW[0];
            mTuples[3] = rkU[1];
            mTuples[4] = rkV[1];
            mTuples[5] = rkW[1];
            mTuples[6] = rkU[2];
            mTuples[7] = rkV[2];
            mTuples[8] = rkW[2];
        }
        else
        {
            mTuples[0] = rkU[0];
            mTuples[1] = rkU[1];
            mTuples[2] = rkU[2];
            mTuples[3] = rkV[0];
            mTuples[4] = rkV[1];
            mTuples[5] = rkV[2];
            mTuples[6] = rkW[0];
            mTuples[7] = rkW[1];
            mTuples[8] = rkW[2];
        }
    }

    inline Matrix3::Matrix3(const Vector3 *akV, bool isColumns)
    {
        if (isColumns)
        {
            mTuples[0] = akV[0][0];
            mTuples[1] = akV[1][0];
            mTuples[2] = akV[2][0];
            mTuples[3] = akV[0][1];
            mTuples[4] = akV[1][1];
            mTuples[5] = akV[2][1];
            mTuples[6] = akV[0][2];
            mTuples[7] = akV[1][2];
            mTuples[8] = akV[2][2];
        }
        else
        {
            mTuples[0] = akV[0][0];
            mTuples[1] = akV[0][1];
            mTuples[2] = akV[0][2];
            mTuples[3] = akV[1][0];
            mTuples[4] = akV[1][1];
            mTuples[5] = akV[1][2];
            mTuples[6] = akV[2][0];
            mTuples[7] = akV[2][1];
            mTuples[8] = akV[2][2];
        }
    }

    inline Matrix3::Matrix3(Real m00, Real m11, Real m22)
    {
        makeDiagonal(m00, m11, m22);
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
        return mTuples;
    }

    inline Matrix3::operator Real *()
    {
        return mTuples;
    }

    inline const Real *Matrix3::operator [](int32_t nRow) const
    {
        T3D_ASSERT(nRow >= 0 && nRow < 3);
        return &mTuples[nRow * 3];
    }

    inline Real *Matrix3::operator [](int32_t nRow)
    {
        T3D_ASSERT(nRow >= 0 && nRow < 3);
        return &mTuples[nRow * 3];
    }

    inline Real Matrix3::operator ()(int32_t nRow, int32_t nCol) const
    {
        T3D_ASSERT(nRow >= 0 && nRow < 3 && nCol >=0 && nCol < 3);
        return mTuples[nRow * 3 + nCol];
    }

    inline Real &Matrix3::operator ()(int32_t nRow, int32_t nCol)
    {
        T3D_ASSERT(nRow >= 0 && nRow < 3 && nCol >= 0 && nCol < 3);
        return mTuples[nRow * 3 + nCol];
    }

    inline void Matrix3::setRow(int32_t nRow, const Vector3 &rkV)
    {
        T3D_ASSERT(nRow >= 0 && nRow < 3);
        int32_t i = nRow * 3;
        mTuples[i] = rkV[0];
        mTuples[i+1] = rkV[1];
        mTuples[i+2] = rkV[2];
    }

    inline Vector3 Matrix3::getRow(int32_t nRow) const
    {
        T3D_ASSERT(nRow >= 0 && nRow < 3);
        int32_t i = nRow * 3;
        return Vector3(mTuples[i], mTuples[i+1], mTuples[i+2]);
    }

    inline void Matrix3::setColumn(int32_t nCol, const Vector3 &rkV)
    {
        T3D_ASSERT(nCol >= 0 && nCol < 3);
        mTuples[nCol] = rkV[0];
        mTuples[nCol+3] = rkV[1];
        mTuples[nCol+6] = rkV[2];
    }

    inline Vector3 Matrix3::getColumn(int32_t nCol) const
    {
        T3D_ASSERT(nCol >= 0 && nCol < 3);
        return Vector3(mTuples[nCol], mTuples[nCol+3], mTuples[nCol+6]);
    }

    inline void Matrix3::getColumnMajor(Real *afColumns) const
    {
        afColumns[0] = mTuples[0];
        afColumns[1] = mTuples[3];
        afColumns[2] = mTuples[6];
        afColumns[3] = mTuples[1];
        afColumns[4] = mTuples[4];
        afColumns[5] = mTuples[7];
        afColumns[6] = mTuples[2];
        afColumns[7] = mTuples[5];
        afColumns[8] = mTuples[8];
    }

    inline Matrix3 &Matrix3::operator =(const Matrix3 &other)
    {
        memcpy(mTuples, other.mTuples, sizeof(mTuples));
        return *this;
    }

    inline int32_t Matrix3::compareArrays(const Matrix3 &other) const
    {
        return memcmp(mTuples, other.mTuples, sizeof(mTuples));
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
            mTuples[0] + other.mTuples[0],
            mTuples[1] + other.mTuples[1],
            mTuples[2] + other.mTuples[2],
            mTuples[3] + other.mTuples[3],
            mTuples[4] + other.mTuples[4],
            mTuples[5] + other.mTuples[5],
            mTuples[6] + other.mTuples[6],
            mTuples[7] + other.mTuples[7],
            mTuples[8] + other.mTuples[8]);
    }

    inline Matrix3 Matrix3::operator -(const Matrix3 &other) const
    {
        return Matrix3(
            mTuples[0] - other.mTuples[0],
            mTuples[1] - other.mTuples[1],
            mTuples[2] - other.mTuples[2],
            mTuples[3] - other.mTuples[3],
            mTuples[4] - other.mTuples[4],
            mTuples[5] - other.mTuples[5],
            mTuples[6] - other.mTuples[6],
            mTuples[7] - other.mTuples[7],
            mTuples[8] - other.mTuples[8]);
    }

    inline Matrix3 Matrix3::operator *(const Matrix3 &other) const
    {
        return Matrix3(
            mTuples[0] * other.mTuples[0] + mTuples[1] * other.mTuples[3] + mTuples[2] * other.mTuples[6],
            mTuples[0] * other.mTuples[1] + mTuples[1] * other.mTuples[4] + mTuples[2] * other.mTuples[7],
            mTuples[0] * other.mTuples[2] + mTuples[1] * other.mTuples[5] + mTuples[2] * other.mTuples[8],
            mTuples[3] * other.mTuples[0] + mTuples[4] * other.mTuples[3] + mTuples[5] * other.mTuples[6],
            mTuples[3] * other.mTuples[1] + mTuples[4] * other.mTuples[4] + mTuples[5] * other.mTuples[7],
            mTuples[3] * other.mTuples[2] + mTuples[4] * other.mTuples[5] + mTuples[5] * other.mTuples[8],
            mTuples[6] * other.mTuples[0] + mTuples[7] * other.mTuples[3] + mTuples[8] * other.mTuples[6],
            mTuples[6] * other.mTuples[1] + mTuples[7] * other.mTuples[4] + mTuples[8] * other.mTuples[7],
            mTuples[6] * other.mTuples[2] + mTuples[7] * other.mTuples[5] + mTuples[8] * other.mTuples[8]
            );
    }

    inline Matrix3 Matrix3::operator *(Real scalar) const
    {
        return Matrix3(
            mTuples[0] * scalar, mTuples[1] * scalar, mTuples[2] * scalar,
            mTuples[3] * scalar, mTuples[4] * scalar, mTuples[5] * scalar,
            mTuples[6] * scalar, mTuples[7] * scalar, mTuples[8] * scalar);
    }

    inline Matrix3 Matrix3::operator /(Real scalar) const
    {
        if (scalar != REAL_ZERO)
            return Matrix3(
                mTuples[0] / scalar, mTuples[1] / scalar, mTuples[2] / scalar, 
                mTuples[3] / scalar, mTuples[4] / scalar, mTuples[5] / scalar,
                mTuples[6] / scalar, mTuples[7] / scalar, mTuples[8] / scalar);
        return ZERO;
    }

    inline Matrix3 Matrix3::operator -() const
    {
        return Matrix3(
            -mTuples[0], -mTuples[1], -mTuples[2],
            -mTuples[3], -mTuples[4], -mTuples[5],
            -mTuples[6], -mTuples[7], -mTuples[8]);
    }

    inline Matrix3 &Matrix3::operator +=(const Matrix3 &other)
    {
        mTuples[0] += other.mTuples[0];
        mTuples[1] += other.mTuples[1];
        mTuples[2] += other.mTuples[2];
        mTuples[3] += other.mTuples[3];
        mTuples[4] += other.mTuples[4];
        mTuples[5] += other.mTuples[5];
        mTuples[6] += other.mTuples[6];
        mTuples[7] += other.mTuples[7];
        mTuples[8] += other.mTuples[8];
        return *this;
    }

    inline Matrix3 &Matrix3::operator -=(const Matrix3 &other)
    {
        mTuples[0] -= other.mTuples[0];
        mTuples[1] -= other.mTuples[1];
        mTuples[2] -= other.mTuples[2];
        mTuples[3] -= other.mTuples[3];
        mTuples[4] -= other.mTuples[4];
        mTuples[5] -= other.mTuples[5];
        mTuples[6] -= other.mTuples[6];
        mTuples[7] -= other.mTuples[7];
        mTuples[8] -= other.mTuples[8];
        return *this;
    }

    inline Matrix3 &Matrix3::operator *=(Real scalar)
    {
        mTuples[0] *= scalar;
        mTuples[1] *= scalar;
        mTuples[2] *= scalar;
        mTuples[3] *= scalar;
        mTuples[4] *= scalar;
        mTuples[5] *= scalar;
        mTuples[6] *= scalar;
        mTuples[7] *= scalar;
        mTuples[8] *= scalar;
        return *this;
    }

    inline Matrix3 &Matrix3::operator /=(Real scalar)
    {
        if (scalar != REAL_ZERO)
        {
            mTuples[0] /= scalar;
            mTuples[1] /= scalar;
            mTuples[2] /= scalar;
            mTuples[3] /= scalar;
            mTuples[4] /= scalar;
            mTuples[5] /= scalar;
            mTuples[6] /= scalar;
            mTuples[7] /= scalar;
            mTuples[8] /= scalar;
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
            mTuples[0] * rkV[0] + mTuples[1] * rkV[1] + mTuples[2] * rkV[2], 
            mTuples[3] * rkV[0] + mTuples[4] * rkV[1] + mTuples[5] * rkV[2],
            mTuples[6] * rkV[0] + mTuples[7] * rkV[1] + mTuples[8] * rkV[2]);
    }

    inline Matrix3 Matrix3::transpose() const
    {
        return Matrix3(
            mTuples[0], mTuples[3], mTuples[6],
            mTuples[1], mTuples[4], mTuples[7],
            mTuples[2], mTuples[5], mTuples[8]);
    }

    inline Matrix3 Matrix3::transposeTimes(const Matrix3 &other) const
    {
        return Matrix3(
            mTuples[0] * other.mTuples[0] + mTuples[3] * other.mTuples[3] + mTuples[6] * other.mTuples[6], 
            mTuples[0] * other.mTuples[1] + mTuples[3] * other.mTuples[4] + mTuples[6] * other.mTuples[7], 
            mTuples[0] * other.mTuples[2] + mTuples[3] * other.mTuples[5] + mTuples[6] * other.mTuples[8], 
            mTuples[1] * other.mTuples[0] + mTuples[4] * other.mTuples[3] + mTuples[7] * other.mTuples[6], 
            mTuples[1] * other.mTuples[1] + mTuples[4] * other.mTuples[4] + mTuples[7] * other.mTuples[7], 
            mTuples[1] * other.mTuples[2] + mTuples[4] * other.mTuples[5] + mTuples[7] * other.mTuples[8], 
            mTuples[2] * other.mTuples[0] + mTuples[5] * other.mTuples[3] + mTuples[8] * other.mTuples[6], 
            mTuples[2] * other.mTuples[1] + mTuples[5] * other.mTuples[4] + mTuples[8] * other.mTuples[7], 
            mTuples[2] * other.mTuples[2] + mTuples[5] * other.mTuples[5] + mTuples[8] * other.mTuples[8]);
    }

    inline Matrix3 Matrix3::timesTranspose(const Matrix3 &other) const
    {
        return Matrix3(
            mTuples[0] * other.mTuples[0] + mTuples[1] * other.mTuples[1] + mTuples[2] * other.mTuples[2],
            mTuples[0] * other.mTuples[3] + mTuples[1] * other.mTuples[4] + mTuples[2] * other.mTuples[5],
            mTuples[0] * other.mTuples[6] + mTuples[1] * other.mTuples[7] + mTuples[2] * other.mTuples[8],
            mTuples[3] * other.mTuples[0] + mTuples[4] * other.mTuples[1] + mTuples[5] * other.mTuples[2],
            mTuples[3] * other.mTuples[3] + mTuples[4] * other.mTuples[4] + mTuples[5] * other.mTuples[5],
            mTuples[3] * other.mTuples[6] + mTuples[4] * other.mTuples[7] + mTuples[5] * other.mTuples[8],
            mTuples[6] * other.mTuples[0] + mTuples[7] * other.mTuples[1] + mTuples[8] * other.mTuples[2],
            mTuples[6] * other.mTuples[3] + mTuples[7] * other.mTuples[4] + mTuples[8] * other.mTuples[5],
            mTuples[6] * other.mTuples[6] + mTuples[7] * other.mTuples[7] + mTuples[8] * other.mTuples[8]);
    }

    inline Matrix3 Matrix3::inverse() const
    {
        Matrix3 matInverse;
        Real fDet = mTuples[0] * mTuples[3] - mTuples[1] * mTuples[2];
        if (fDet != REAL_ZERO)
        {
            matInverse.mTuples[0] = mTuples[3] / fDet;
            matInverse.mTuples[1] = -mTuples[1] / fDet;
            matInverse.mTuples[2] = -mTuples[2] / fDet;
            matInverse.mTuples[3] = mTuples[0] / fDet;
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

        Real fCofactor00 = mTuples[4] * mTuples[8] - mTuples[5] * mTuples[7];
        Real fCofactor01 = mTuples[5] * mTuples[6] - mTuples[3] * mTuples[8];
        Real fCofactor02 = mTuples[3] * mTuples[7] - mTuples[4] * mTuples[6];

        return mTuples[0] * fCofactor00 + mTuples[1] * fCofactor01 + mTuples[2] * fCofactor02;
    }

    inline Real Matrix3::qform(const Vector3 &rkU, const Vector3 &rkV) const
    {
        return rkU.dot((*this) * rkV);
    }

    inline Matrix3 Matrix3::timesDiagonal(const Vector3 &rkDiag) const
    {
        return Matrix3(
            mTuples[0] * rkDiag[0], mTuples[1] * rkDiag[1], mTuples[2] * rkDiag[2],
            mTuples[3] * rkDiag[0], mTuples[4] * rkDiag[1], mTuples[5] * rkDiag[2],
            mTuples[6] * rkDiag[0], mTuples[7] * rkDiag[1], mTuples[8] * rkDiag[2]);
    }

    inline Matrix3 Matrix3::diagonalTimes(const Vector3 &rkDiag) const
    {
        return Matrix3(
            rkDiag[0] * mTuples[0], rkDiag[0] * mTuples[1], rkDiag[0] * mTuples[2],
            rkDiag[1] * mTuples[3], rkDiag[1] * mTuples[4], rkDiag[1] * mTuples[5],
            rkDiag[2] * mTuples[6], rkDiag[2] * mTuples[7], rkDiag[2] * mTuples[8]);
    }

    inline void Matrix3::fromEulerAnglesXYZ(const Radian &rkPitch, const Radian &rkYaw, const Radian &rkRoll)
    {
        Real fCos, fSin;

        fCos = Math::cos(rkPitch);
        fSin = Math::sin(rkPitch);
        Matrix3 matX(
            1.0, 0.0, 0.0, 
            0.0, fCos, -fSin, 
            0.0, fSin, fCos);

        fCos = Math::cos(rkYaw);
        fSin = Math::sin(rkYaw);
        Matrix3 matY(
            fCos, 0.0, fSin, 
            0.0, 1.0, 0.0, 
            -fSin, 0.0, fCos);

        fCos = Math::cos(rkRoll);
        fSin = Math::sin(rkRoll);
        Matrix3 matZ(
            fCos, -fSin, 0.0, 
            fSin, fCos, 0.0, 
            0.0, 0.0, 1.0);

        *this = matX * (matY * matZ);
    }

    inline void Matrix3::fromEulerAnglesXZY(const Radian &rkPitch, const Radian &rkRoll, const Radian &rkYaw)
    {
        Real fCos, fSin;

        fCos = Math::cos(rkPitch);
        fSin = Math::sin(rkPitch);
        Matrix3 matX(
            1.0, 0.0, 0.0,
            0.0, fCos, -fSin,
            0.0, fSin, fCos);

        fCos = Math::cos(rkRoll);
        fSin = Math::sin(rkRoll);
        Matrix3 matZ(
            fCos, -fSin, 0.0,
            fSin, fCos, 0.0,
            0.0, 0.0, 1.0);

        fCos = Math::cos(rkYaw);
        fSin = Math::sin(rkYaw);
        Matrix3 matY(
            fCos, 0.0, fSin,
            0.0, 1.0, 0.0,
            -fSin, 0.0, fCos);

        *this = matX * (matZ * matY);
    }

    inline void Matrix3::fromEulerAnglesYXZ(const Radian &rkYaw, const Radian &rkPitch, const Radian &rkRoll)
    {
        Real fCos, fSin;

        fCos = Math::cos(rkYaw);
        fSin = Math::sin(rkYaw);
        Matrix3 matY(
            fCos, 0.0, fSin,
            0.0, 1.0, 0.0,
            -fSin, 0.0, fCos);

        fCos = Math::cos(rkPitch);
        fSin = Math::sin(rkPitch);
        Matrix3 matX(
            1.0, 0.0, 0.0,
            0.0, fCos, -fSin,
            0.0, fSin, fCos);

        fCos = Math::cos(rkRoll);
        fSin = Math::sin(rkRoll);
        Matrix3 matZ(
            fCos, -fSin, 0.0,
            fSin, fCos, 0.0,
            0.0, 0.0, 1.0);

        *this = matY * (matX * matZ);
    }

    inline void Matrix3::fromEulerAnglesYZX(const Radian &rkYaw, const Radian &rkRoll, const Radian &rkPitch)
    {
        Real fCos, fSin;

        fCos = Math::cos(rkYaw);
        fSin = Math::sin(rkYaw);
        Matrix3 matY(
            fCos, 0.0, fSin,
            0.0, 1.0, 0.0,
            -fSin, 0.0, fCos);

        fCos = Math::cos(rkRoll);
        fSin = Math::sin(rkRoll);
        Matrix3 matZ(
            fCos, -fSin, 0.0,
            fSin, fCos, 0.0,
            0.0, 0.0, 1.0);

        fCos = Math::cos(rkPitch);
        fSin = Math::sin(rkPitch);
        Matrix3 matX(
            1.0, 0.0, 0.0,
            0.0, fCos, -fSin,
            0.0, fSin, fCos);

        *this = matY * (matZ * matX);
    }

    inline void Matrix3::fromEulerAnglesZXY(const Radian &rkRoll, const Radian &rkPitch, const Radian &rkYaw)
    {
        Real fCos, fSin;

        fCos = Math::cos(rkRoll);
        fSin = Math::sin(rkRoll);
        Matrix3 matZ(
            fCos, -fSin, 0.0,
            fSin, fCos, 0.0,
            0.0, 0.0, 1.0);

        fCos = Math::cos(rkPitch);
        fSin = Math::sin(rkPitch);
        Matrix3 matX(
            1.0, 0.0, 0.0,
            0.0, fCos, -fSin,
            0.0, fSin, fCos);

        fCos = Math::cos(rkYaw);
        fSin = Math::sin(rkYaw);
        Matrix3 matY(
            fCos, 0.0, fSin,
            0.0, 1.0, 0.0,
            -fSin, 0.0, fCos);

        *this = matZ * (matX * matY);
    }

    inline void Matrix3::fromEulerAnglesZYX(const Radian &rkRoll, const Radian &rkYaw, const Radian &rkPitch)
    {
        Real fCos, fSin;

        fCos = Math::cos(rkRoll);
        fSin = Math::sin(rkRoll);
        Matrix3 matZ(
            fCos, -fSin, 0.0,
            fSin, fCos, 0.0,
            0.0, 0.0, 1.0);

        fCos = Math::cos(rkYaw);
        fSin = Math::sin(rkYaw);
        Matrix3 matY(
            fCos, 0.0, fSin,
            0.0, 1.0, 0.0,
            -fSin, 0.0, fCos);

        fCos = Math::cos(rkPitch);
        fSin = Math::sin(rkPitch);
        Matrix3 matX(
            1.0, 0.0, 0.0,
            0.0, fCos, -fSin,
            0.0, fSin, fCos);

        *this = matZ * (matY * matX);
    }

    inline Matrix3 operator *(Real scalar, const Matrix3 &rkM)
    {
        return rkM * scalar;
    }

    inline Vector3 operator *(const Vector3 &rkV, const Matrix3 &rkM)
    {
        return Vector3(
            rkV[0] * rkM[0][0] + rkV[1] * rkM[1][0] + rkV[2] * rkM[2][0], 
            rkV[0] * rkM[0][1] + rkV[1] * rkM[1][1] + rkV[2] * rkM[2][1],
            rkV[0] * rkM[0][2] + rkV[1] * rkM[1][2] + rkV[2] * rkM[2][2]);
    }
}
