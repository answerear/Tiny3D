

namespace Tiny3D
{
    inline void Matrix4::makeIdentity()
    {
        m4x4[0][0] = REAL_ONE;
        m4x4[0][1] = REAL_ZERO;
        m4x4[0][2] = REAL_ZERO;
        m4x4[0][3] = REAL_ZERO;
        m4x4[1][0] = REAL_ZERO;
        m4x4[1][1] = REAL_ONE;
        m4x4[1][2] = REAL_ZERO;
        m4x4[1][3] = REAL_ZERO;
        m4x4[2][0] = REAL_ZERO;
        m4x4[2][1] = REAL_ZERO;
        m4x4[2][2] = REAL_ONE;
        m4x4[2][3] = REAL_ZERO;
        m4x4[3][0] = REAL_ZERO;
        m4x4[3][1] = REAL_ZERO;
        m4x4[3][2] = REAL_ZERO;
        m4x4[3][3] = REAL_ONE;
    }

    inline void Matrix4::makeZero()
    {
        m4x4[0][0] = m4x4[0][1] = m4x4[0][2] = m4x4[0][3] = REAL_ZERO;
        m4x4[1][0] = m4x4[1][1] = m4x4[1][2] = m4x4[1][3] = REAL_ZERO;
        m4x4[2][0] = m4x4[2][1] = m4x4[2][2] = m4x4[2][3] = REAL_ZERO;
        m4x4[3][0] = m4x4[3][1] = m4x4[3][2] = m4x4[3][3] = REAL_ZERO;
    }

    inline Matrix4 &Matrix4::operator =(const Matrix4 &other)
    {
        memcpy(mTuples, other.mTuples, sizeof(mTuples));
        return *this;
    }

    inline void Matrix4::operator =(const Matrix3 &rkMat)
    {
        m4x4[0][0] = rkMat[0][0], m4x4[0][1] = rkMat[0][1], m4x4[0][2] = rkMat[0][2];
        m4x4[1][0] = rkMat[1][0], m4x4[1][1] = rkMat[1][1], m4x4[1][2] = rkMat[1][2];
        m4x4[2][0] = rkMat[2][0], m4x4[2][1] = rkMat[2][1], m4x4[2][2] = rkMat[2][2];
    }

    inline Matrix4::Matrix4(bool isZero /* = true */)
    {
        if (isZero)
            makeZero();
        else
            makeIdentity();
    }

    inline Matrix4::Matrix4(const Matrix4 &other)
    {
        memcpy(mTuples, other.mTuples, sizeof(mTuples));
    }

    inline Matrix4::Matrix4(
        Real m00, Real m01, Real m02, Real m03, 
        Real m10, Real m11, Real m12, Real m13, 
        Real other0, Real other1, Real other2, Real other3, 
        Real m30, Real m31, Real m32, Real m33)
    {
        m4x4[0][0] = m00;
        m4x4[0][1] = m01;
        m4x4[0][2] = m02;
        m4x4[0][3] = m03;
        m4x4[1][0] = m10;
        m4x4[1][1] = m11;
        m4x4[1][2] = m12;
        m4x4[1][3] = m13;
        m4x4[2][0] = other0;
        m4x4[2][1] = other1;
        m4x4[2][2] = other2;
        m4x4[2][3] = other3;
        m4x4[3][0] = m30;
        m4x4[3][1] = m31;
        m4x4[3][2] = m32;
        m4x4[3][3] = m33;
    }

    inline Matrix4::Matrix4(const Matrix3 &rkMat)
    {
        operator =(rkMat);
        m4x4[0][3] = m4x4[1][3] = m4x4[2][3] = REAL_ZERO;
        m4x4[3][3] = REAL_ONE;
        m4x4[3][0] = m4x4[3][1] = m4x4[3][2] = REAL_ZERO;
    }

    inline Matrix4::Matrix4(const Quaternion &rkRot)
    {
        Matrix3 mat;
        rkRot.toRotationMatrix(mat);
        operator =(mat);
        m4x4[0][3] = m4x4[1][3] = m4x4[2][3] = REAL_ZERO;
        m4x4[3][3] = REAL_ONE;
        m4x4[3][0] = m4x4[3][1] = m4x4[3][2] = REAL_ZERO;
    }

    inline Matrix4::Matrix4(const Vector3 &rkPos)
    {
        m4x4[0][0] = REAL_ONE,  m4x4[0][1] = REAL_ZERO, m4x4[0][2] = REAL_ZERO, m4x4[0][3] = rkPos.x();
        m4x4[1][0] = REAL_ZERO, m4x4[1][1] = REAL_ONE,  m4x4[1][2] = REAL_ZERO, m4x4[1][3] = rkPos.y();
        m4x4[2][0] = REAL_ZERO, m4x4[2][1] = REAL_ZERO, m4x4[2][2] = REAL_ONE,  m4x4[2][3] = rkPos.z();
        m4x4[3][0] = REAL_ZERO, m4x4[3][1] = REAL_ZERO, m4x4[3][2] = REAL_ZERO, m4x4[3][3] = REAL_ONE;
    }

    inline Matrix4::Matrix4(Real m00, Real m11, Real m22, Real m33)
    {
        m4x4[0][0] = m00,       m4x4[0][1] = REAL_ZERO, m4x4[0][2] = REAL_ZERO, m4x4[0][3] = REAL_ZERO;
        m4x4[1][0] = REAL_ZERO, m4x4[1][1] = m11,       m4x4[1][2] = REAL_ZERO, m4x4[1][3] = REAL_ZERO;
        m4x4[2][0] = REAL_ZERO, m4x4[2][1] = REAL_ZERO, m4x4[2][2] = m22,       m4x4[2][3] = REAL_ZERO;
        m4x4[3][0] = REAL_ZERO, m4x4[3][1] = REAL_ZERO, m4x4[3][2] = REAL_ZERO, m4x4[3][3] = m33;
    }

    inline Matrix4::Matrix4(const Vector3 &X, const Vector3 &Y, const Vector3 &Z)
    {
        m4x4[0][0] = X[0],      m4x4[0][1] = Y[0],      m4x4[0][2] = Z[0],      m4x4[0][3] = REAL_ZERO;
        m4x4[1][0] = X[1],      m4x4[1][1] = Y[1],      m4x4[1][2] = Z[1],      m4x4[1][3] = REAL_ZERO;
        m4x4[2][0] = X[2],      m4x4[2][1] = Y[2],      m4x4[2][2] = Z[2],      m4x4[2][3] = REAL_ZERO;
        m4x4[3][0] = REAL_ZERO, m4x4[3][1] = REAL_ZERO, m4x4[3][2] = REAL_ZERO, m4x4[3][3] = REAL_ONE;
    }

    inline Matrix4::operator const Real *() const
    {
        return mTuples;
    }

    inline Matrix4::operator Real *()
    {
        return mTuples;
    }

    inline const Real *Matrix4::operator [](int32_t row) const
    {
        T3D_ASSERT(row >= 0 && row < 4);
        return m4x4[row];
    }

    inline Real *Matrix4::operator [](int32_t row)
    {
        T3D_ASSERT(row >= 0 && row < 4);
        return m4x4[row];
    }

    inline Real Matrix4::operator ()(int32_t row, int32_t col) const
    {
        return m4x4[row][col];
    }

    inline Real &Matrix4::operator ()(int32_t row, int32_t col)
    {
        return m4x4[row][col];
    }

    inline int32_t Matrix4::compareArrays(const Matrix4 &other) const
    {
        return memcmp(mTuples, other.mTuples, sizeof(mTuples));
    }

    inline bool Matrix4::operator ==(const Matrix4 &other) const
    {
        return compareArrays(other) == 0;
    }

    inline bool Matrix4::operator !=(const Matrix4 &other) const
    {
        return compareArrays(other) != 0;
    }

    inline bool Matrix4::operator <(const Matrix4 &other) const
    {
        return compareArrays(other) < 0;
    }

    inline bool Matrix4::operator <=(const Matrix4 &other) const
    {
        return compareArrays(other) <= 0;
    }

    inline bool Matrix4::operator >(const Matrix4 &other) const
    {
        return compareArrays(other) > 0;
    }

    inline bool Matrix4::operator >=(const Matrix4 &other) const
    {
        return compareArrays(other) >= 0;
    }

    inline Matrix4 Matrix4::operator +(const Matrix4 &other) const
    {
        return Matrix4(
            m4x4[0][0] + other.m4x4[0][0], m4x4[0][1] + other.m4x4[0][1], m4x4[0][2] + other.m4x4[0][2], m4x4[0][3] + other.m4x4[0][3],
            m4x4[1][0] + other.m4x4[1][0], m4x4[1][1] + other.m4x4[1][1], m4x4[1][2] + other.m4x4[1][2], m4x4[1][3] + other.m4x4[1][3],
            m4x4[2][0] + other.m4x4[2][0], m4x4[2][1] + other.m4x4[2][1], m4x4[2][2] + other.m4x4[2][2], m4x4[2][3] + other.m4x4[2][3],
            m4x4[3][0] + other.m4x4[3][0], m4x4[3][1] + other.m4x4[3][1], m4x4[3][2] + other.m4x4[3][2], m4x4[3][3] + other.m4x4[3][3]
        );
    }

    inline Matrix4 Matrix4::operator -(const Matrix4 &other) const
    {
        return Matrix4(
            m4x4[0][0] - other.m4x4[0][0], m4x4[0][1] - other.m4x4[0][1], m4x4[0][2] - other.m4x4[0][2], m4x4[0][3] - other.m4x4[0][3],
            m4x4[1][0] - other.m4x4[1][0], m4x4[1][1] - other.m4x4[1][1], m4x4[1][2] - other.m4x4[1][2], m4x4[1][3] - other.m4x4[1][3],
            m4x4[2][0] - other.m4x4[2][0], m4x4[2][1] - other.m4x4[2][1], m4x4[2][2] - other.m4x4[2][2], m4x4[2][3] - other.m4x4[2][3],
            m4x4[3][0] - other.m4x4[3][0], m4x4[3][1] - other.m4x4[3][1], m4x4[3][2] - other.m4x4[3][2], m4x4[3][3] - other.m4x4[3][3]
        );
    }

    inline Matrix4 Matrix4::operator *(const Matrix4 &other) const
    {
        return Matrix4(
            m4x4[0][0] * other.m4x4[0][0] + m4x4[0][1] * other.m4x4[1][0] + m4x4[0][2] * other.m4x4[2][0] + m4x4[0][3] * other.m4x4[3][0],
            m4x4[0][0] * other.m4x4[0][1] + m4x4[0][1] * other.m4x4[1][1] + m4x4[0][2] * other.m4x4[2][1] + m4x4[0][3] * other.m4x4[3][1],
            m4x4[0][0] * other.m4x4[0][2] + m4x4[0][1] * other.m4x4[1][2] + m4x4[0][2] * other.m4x4[2][2] + m4x4[0][3] * other.m4x4[3][2],
            m4x4[0][0] * other.m4x4[0][3] + m4x4[0][1] * other.m4x4[1][3] + m4x4[0][2] * other.m4x4[2][3] + m4x4[0][3] * other.m4x4[3][3],

            m4x4[1][0] * other.m4x4[0][0] + m4x4[1][1] * other.m4x4[1][0] + m4x4[1][2] * other.m4x4[2][0] + m4x4[1][3] * other.m4x4[3][0],
            m4x4[1][0] * other.m4x4[0][1] + m4x4[1][1] * other.m4x4[1][1] + m4x4[1][2] * other.m4x4[2][1] + m4x4[1][3] * other.m4x4[3][1],
            m4x4[1][0] * other.m4x4[0][2] + m4x4[1][1] * other.m4x4[1][2] + m4x4[1][2] * other.m4x4[2][2] + m4x4[1][3] * other.m4x4[3][2],
            m4x4[1][0] * other.m4x4[0][3] + m4x4[1][1] * other.m4x4[1][3] + m4x4[1][2] * other.m4x4[2][3] + m4x4[1][3] * other.m4x4[3][3],

            m4x4[2][0] * other.m4x4[0][0] + m4x4[2][1] * other.m4x4[1][0] + m4x4[2][2] * other.m4x4[2][0] + m4x4[2][3] * other.m4x4[3][0],
            m4x4[2][0] * other.m4x4[0][1] + m4x4[2][1] * other.m4x4[1][1] + m4x4[2][2] * other.m4x4[2][1] + m4x4[2][3] * other.m4x4[3][1],
            m4x4[2][0] * other.m4x4[0][2] + m4x4[2][1] * other.m4x4[1][2] + m4x4[2][2] * other.m4x4[2][2] + m4x4[2][3] * other.m4x4[3][2],
            m4x4[2][0] * other.m4x4[0][3] + m4x4[2][1] * other.m4x4[1][3] + m4x4[2][2] * other.m4x4[2][3] + m4x4[2][3] * other.m4x4[3][3],

            m4x4[3][0] * other.m4x4[0][0] + m4x4[3][1] * other.m4x4[1][0] + m4x4[3][2] * other.m4x4[2][0] + m4x4[3][3] * other.m4x4[3][0],
            m4x4[3][0] * other.m4x4[0][1] + m4x4[3][1] * other.m4x4[1][1] + m4x4[3][2] * other.m4x4[2][1] + m4x4[3][3] * other.m4x4[3][1],
            m4x4[3][0] * other.m4x4[0][2] + m4x4[3][1] * other.m4x4[1][2] + m4x4[3][2] * other.m4x4[2][2] + m4x4[3][3] * other.m4x4[3][2],
            m4x4[3][0] * other.m4x4[0][3] + m4x4[3][1] * other.m4x4[1][3] + m4x4[3][2] * other.m4x4[2][3] + m4x4[3][3] * other.m4x4[3][3]
            );
    }

    inline Matrix4 Matrix4::operator *(Real scalar) const
    {
        return Matrix4(
            m4x4[0][0] * scalar, m4x4[0][1] * scalar, m4x4[0][2] * scalar, m4x4[0][3] * scalar,
            m4x4[1][0] * scalar, m4x4[1][1] * scalar, m4x4[1][2] * scalar, m4x4[1][3] * scalar,
            m4x4[2][0] * scalar, m4x4[2][1] * scalar, m4x4[2][2] * scalar, m4x4[2][3] * scalar,
            m4x4[3][0] * scalar, m4x4[3][1] * scalar, m4x4[3][2] * scalar, m4x4[3][3] * scalar
            );
    }

    inline Matrix4 Matrix4::operator /(Real scalar) const
    {
        Real fInvertScalar = 0.0;
        if (scalar != REAL_ZERO)
            fInvertScalar = REAL_ONE / scalar;
        return Matrix4(
            m4x4[0][0] * fInvertScalar, m4x4[0][1] * fInvertScalar, m4x4[0][2] * fInvertScalar, m4x4[0][3] * fInvertScalar,
            m4x4[1][0] * fInvertScalar, m4x4[1][1] * fInvertScalar, m4x4[1][2] * fInvertScalar, m4x4[1][3] * fInvertScalar,
            m4x4[2][0] * fInvertScalar, m4x4[2][1] * fInvertScalar, m4x4[2][2] * fInvertScalar, m4x4[2][3] * fInvertScalar,
            m4x4[3][0] * fInvertScalar, m4x4[3][1] * fInvertScalar, m4x4[3][2] * fInvertScalar, m4x4[3][3] * fInvertScalar
            );
    }

    inline Matrix4 Matrix4::operator -() const
    {
        return Matrix4(
            -m4x4[0][0], -m4x4[0][1], -m4x4[0][2], -m4x4[0][3],
            -m4x4[1][0], -m4x4[1][1], -m4x4[1][2], -m4x4[1][3],
            -m4x4[2][0], -m4x4[2][1], -m4x4[2][2], -m4x4[2][3],
            -m4x4[3][0], -m4x4[3][1], -m4x4[3][2], -m4x4[3][3]
            );
    }

    inline Matrix4 &Matrix4::operator +=(const Matrix4 &other)
    {
        m4x4[0][0] += other.m4x4[0][0];
        m4x4[0][1] += other.m4x4[0][1];
        m4x4[0][2] += other.m4x4[0][2];
        m4x4[0][3] += other.m4x4[0][3];

        m4x4[1][0] += other.m4x4[1][0];
        m4x4[1][1] += other.m4x4[1][1];
        m4x4[1][2] += other.m4x4[1][2];
        m4x4[1][3] += other.m4x4[1][3];

        m4x4[2][0] += other.m4x4[2][0];
        m4x4[2][1] += other.m4x4[2][1];
        m4x4[2][2] += other.m4x4[2][2];
        m4x4[2][3] += other.m4x4[2][3];

        m4x4[3][0] += other.m4x4[3][0];
        m4x4[3][1] += other.m4x4[3][1];
        m4x4[3][2] += other.m4x4[3][2];
        m4x4[3][3] += other.m4x4[3][3];

        return *this;
    }

    inline Matrix4 &Matrix4::operator -=(const Matrix4 &other)
    {
        m4x4[0][0] -= other.m4x4[0][0];
        m4x4[0][1] -= other.m4x4[0][1];
        m4x4[0][2] -= other.m4x4[0][2];
        m4x4[0][3] -= other.m4x4[0][3];

        m4x4[1][0] -= other.m4x4[1][0];
        m4x4[1][1] -= other.m4x4[1][1];
        m4x4[1][2] -= other.m4x4[1][2];
        m4x4[1][3] -= other.m4x4[1][3];

        m4x4[2][0] -= other.m4x4[2][0];
        m4x4[2][1] -= other.m4x4[2][1];
        m4x4[2][2] -= other.m4x4[2][2];
        m4x4[2][3] -= other.m4x4[2][3];

        m4x4[3][0] -= other.m4x4[3][0];
        m4x4[3][1] -= other.m4x4[3][1];
        m4x4[3][2] -= other.m4x4[3][2];
        m4x4[3][3] -= other.m4x4[3][3];

        return *this;
    }

    inline Matrix4 &Matrix4::operator *=(Real scalar)
    {
        m4x4[0][0] *= scalar, m4x4[0][1] *= scalar, m4x4[0][2] *= scalar, m4x4[0][3] *= scalar;
        m4x4[1][0] *= scalar, m4x4[1][1] *= scalar, m4x4[1][2] *= scalar, m4x4[1][3] *= scalar;
        m4x4[2][0] *= scalar, m4x4[2][1] *= scalar, m4x4[2][2] *= scalar, m4x4[2][3] *= scalar;
        m4x4[3][0] *= scalar, m4x4[3][1] *= scalar, m4x4[3][2] *= scalar, m4x4[3][3] *= scalar;

        return *this;
    }

    inline Matrix4 &Matrix4::operator /=(Real scalar)
    {
        Real fInvertScalar = 0.0;
        if (scalar != REAL_ZERO)
            fInvertScalar = REAL_ONE / scalar;
        
        m4x4[0][0] *= fInvertScalar, m4x4[0][1] *= fInvertScalar, m4x4[0][2] *= fInvertScalar, m4x4[0][3] *= fInvertScalar;
        m4x4[1][0] *= fInvertScalar, m4x4[1][1] *= fInvertScalar, m4x4[1][2] *= fInvertScalar, m4x4[1][3] *= fInvertScalar;
        m4x4[2][0] *= fInvertScalar, m4x4[2][1] *= fInvertScalar, m4x4[2][2] *= fInvertScalar, m4x4[2][3] *= fInvertScalar;
        m4x4[3][0] *= fInvertScalar, m4x4[3][1] *= fInvertScalar, m4x4[3][2] *= fInvertScalar, m4x4[3][3] *= fInvertScalar;

        return *this;
    }

    inline Vector4 Matrix4::operator *(const Vector4 &rkV) const
    {
        return Vector4(
            m4x4[0][0] * rkV.x() + m4x4[0][1] * rkV.y() + m4x4[0][2] * rkV.z() + m4x4[0][3] * rkV.w(), 
            m4x4[1][0] * rkV.x() + m4x4[1][1] * rkV.y() + m4x4[1][2] * rkV.z() + m4x4[1][3] * rkV.w(),
            m4x4[2][0] * rkV.x() + m4x4[2][1] * rkV.y() + m4x4[2][2] * rkV.z() + m4x4[2][3] * rkV.w(),
            m4x4[3][0] * rkV.x() + m4x4[3][1] * rkV.y() + m4x4[3][2] * rkV.z() + m4x4[3][3] * rkV.w()
            );
    }

    inline Vector3 Matrix4::operator *(const Vector3 &rkV) const
    {
        Real w = m4x4[3][0] * rkV.x() + m4x4[3][1] * rkV.y() + m4x4[3][2] * rkV.z() + m4x4[3][3] * Real(1.0);

        if (w != Real(0.0))
        {
            return Vector3(
                (m4x4[0][0] * rkV.x() + m4x4[0][1] * rkV.y() + m4x4[0][2] * rkV.z() + m4x4[0][3] * Real(1.0)) / w, 
                (m4x4[1][0] * rkV.x() + m4x4[1][1] * rkV.y() + m4x4[1][2] * rkV.z() + m4x4[1][3] * Real(1.0)) / w,
                (m4x4[2][0] * rkV.x() + m4x4[2][1] * rkV.y() + m4x4[2][2] * rkV.z() + m4x4[2][3] * Real(1.0)) / w
                );
        }
        
        return Vector3(
            m4x4[0][0] * rkV.x() + m4x4[0][1] * rkV.y() + m4x4[0][2] * rkV.z() + m4x4[0][3] * Real(1.0),
            m4x4[1][0] * rkV.x() + m4x4[1][1] * rkV.y() + m4x4[1][2] * rkV.z() + m4x4[1][3] * Real(1.0),
            m4x4[2][0] * rkV.x() + m4x4[2][1] * rkV.y() + m4x4[2][2] * rkV.z() + m4x4[2][3] * Real(1.0)
            );
    }

    inline Matrix4 Matrix4::transpose() const
    {
        return Matrix4(
            m4x4[0][0], m4x4[1][0], m4x4[2][0], m4x4[3][0],
            m4x4[0][1], m4x4[1][1], m4x4[2][1], m4x4[3][1],
            m4x4[0][2], m4x4[1][2], m4x4[2][2], m4x4[3][2],
            m4x4[0][3], m4x4[1][3], m4x4[2][3], m4x4[3][3]
            );
    }
    inline bool Matrix4::isAffine() const
    {
        return m4x4[3][0] == REAL_ZERO && m4x4[3][1] == REAL_ZERO && m4x4[3][2] == REAL_ZERO && m4x4[3][3] == REAL_ONE;
    }

    inline bool Matrix4::hasScale() const
    {
        Real t = m4x4[0][0] * m4x4[0][0] + m4x4[1][0] * m4x4[1][0] + m4x4[2][0] * m4x4[2][0];
        if (!Math::realEqual(t, REAL_ONE, Real(1e-04)))
            return true;
        t = m4x4[0][1] * m4x4[0][1] + m4x4[1][1] * m4x4[1][1] + m4x4[2][1] * m4x4[2][1];
        if (!Math::realEqual(t, REAL_ONE, Real(1e-04)))
            return true;
        t = m4x4[0][2] * m4x4[0][2] + m4x4[1][2] * m4x4[1][2] + m4x4[2][2] * m4x4[2][2];
        if (!Math::realEqual(t, REAL_ONE, Real(1e-04)))
            return true;

        return false;
    }

    inline Matrix4 Matrix4::concatenateAffine(const Matrix4 &other) const
    {
        T3D_ASSERT(isAffine() && other.isAffine());

        return Matrix4(
            m4x4[0][0] * other.m4x4[0][0] + m4x4[0][1] * other.m4x4[1][0] + m4x4[0][2] * other.m4x4[2][0],
            m4x4[0][0] * other.m4x4[0][1] + m4x4[0][1] * other.m4x4[1][1] + m4x4[0][2] * other.m4x4[2][1],
            m4x4[0][0] * other.m4x4[0][2] + m4x4[0][1] * other.m4x4[1][2] + m4x4[0][2] * other.m4x4[2][2],
            m4x4[0][0] * other.m4x4[0][3] + m4x4[0][1] * other.m4x4[1][3] + m4x4[0][2] * other.m4x4[2][3] + m4x4[0][3],

            m4x4[1][0] * other.m4x4[0][0] + m4x4[1][1] * other.m4x4[1][0] + m4x4[1][2] * other.m4x4[2][0],
            m4x4[1][0] * other.m4x4[0][1] + m4x4[1][1] * other.m4x4[1][1] + m4x4[1][2] * other.m4x4[2][1],
            m4x4[1][0] * other.m4x4[0][2] + m4x4[1][1] * other.m4x4[1][2] + m4x4[1][2] * other.m4x4[2][2],
            m4x4[1][0] * other.m4x4[0][3] + m4x4[1][1] * other.m4x4[1][3] + m4x4[1][2] * other.m4x4[2][3] + m4x4[1][3],

            m4x4[2][0] * other.m4x4[0][0] + m4x4[2][1] * other.m4x4[1][0] + m4x4[2][2] * other.m4x4[2][0],
            m4x4[2][0] * other.m4x4[0][1] + m4x4[2][1] * other.m4x4[1][1] + m4x4[2][2] * other.m4x4[2][1],
            m4x4[2][0] * other.m4x4[0][2] + m4x4[2][1] * other.m4x4[1][2] + m4x4[2][2] * other.m4x4[2][2],
            m4x4[2][0] * other.m4x4[0][3] + m4x4[2][1] * other.m4x4[1][3] + m4x4[2][2] * other.m4x4[2][3] + m4x4[2][3],

            REAL_ZERO, REAL_ZERO, REAL_ZERO, REAL_ONE);
    }

    inline void Matrix4::extractMatrix(Matrix3 &m3x3) const
    {
        m3x3[0][0] = m4x4[0][0];
        m3x3[0][1] = m4x4[0][1];
        m3x3[0][2] = m4x4[0][2];
        m3x3[1][0] = m4x4[1][0];
        m3x3[1][1] = m4x4[1][1];
        m3x3[1][2] = m4x4[1][2];
        m3x3[2][0] = m4x4[2][0];
        m3x3[2][1] = m4x4[2][1];
        m3x3[2][2] = m4x4[2][2];
    }

    inline Quaternion Matrix4::extractQuaternion() const
    {
        Matrix3 m3x3;
        extractMatrix(m3x3);
        return Quaternion(m3x3);
    }

    inline Vector3 Matrix4::extractTranslation() const
    {
        return Vector3(m4x4[0][3], m4x4[1][3], m4x4[2][3]);
    }

    inline void Matrix4::decomposition(Vector3 &position, Vector3 &scale, Quaternion &orientation) const
    {
        T3D_ASSERT(isAffine());

        Matrix3 m3x3;
        extractMatrix(m3x3);

        Matrix3 matQ;
        Vector3 vecU;
        m3x3.QDUDecomposition(matQ, scale, vecU); 

        orientation = Quaternion( matQ );
        position = Vector3(m4x4[0][3], m4x4[1][3], m4x4[2][3]);
    }

    inline void Matrix4::makeTranslate(const Vector3 &position)
    {
        m4x4[0][0] = REAL_ONE, m4x4[0][1] = REAL_ZERO, m4x4[0][2] = REAL_ZERO, m4x4[0][3] = position.x();
        m4x4[1][0] = REAL_ZERO, m4x4[1][1] = REAL_ONE, m4x4[1][2] = REAL_ZERO, m4x4[1][3] = position.y();
        m4x4[2][0] = REAL_ZERO, m4x4[2][1] = REAL_ZERO, m4x4[2][2] = REAL_ONE, m4x4[2][3] = position.z();
        m4x4[3][0] = REAL_ZERO, m4x4[3][1] = REAL_ZERO, m4x4[3][2] = REAL_ZERO, m4x4[3][3] = REAL_ONE;
    }

    inline void Matrix4::makeTranslate(Real tx, Real ty, Real tz)
    {
        m4x4[0][0] = REAL_ONE, m4x4[0][1] = REAL_ZERO, m4x4[0][2] = REAL_ZERO, m4x4[0][3] = tx;
        m4x4[1][0] = REAL_ZERO, m4x4[1][1] = REAL_ONE, m4x4[1][2] = REAL_ZERO, m4x4[1][3] = ty;
        m4x4[2][0] = REAL_ZERO, m4x4[2][1] = REAL_ZERO, m4x4[2][2] = REAL_ONE, m4x4[2][3] = tz;
        m4x4[3][0] = REAL_ZERO, m4x4[3][1] = REAL_ZERO, m4x4[3][2] = REAL_ZERO, m4x4[3][3] = REAL_ONE;
    }

    inline void Matrix4::setScale(const Vector3 &scale)
    {
        m4x4[0][0] = scale.x();
        m4x4[1][1] = scale.y();
        m4x4[2][2] = scale.z();
    }

    inline void Matrix4::setTranslate(const Vector3 &position)
    {
        m4x4[0][3] = position.x();
        m4x4[1][3] = position.y();
        m4x4[2][3] = position.z();
    }

    inline Vector3 Matrix4::transformAffine(const Vector3 &v) const
    {
        T3D_ASSERT(isAffine());

        return Vector3(
            m4x4[0][0] * v.x() + m4x4[0][1] * v.y() + m4x4[0][2] * v.z() + m4x4[0][3], 
            m4x4[1][0] * v.x() + m4x4[1][1] * v.y() + m4x4[1][2] * v.z() + m4x4[1][3],
            m4x4[2][0] * v.x() + m4x4[2][1] * v.y() + m4x4[2][2] * v.z() + m4x4[2][3]);
    }

    inline Vector4 Matrix4::transformAffine(const Vector4 &v) const
    {
        T3D_ASSERT(isAffine());

        return Vector4(
            m4x4[0][0] * v.x() + m4x4[0][1] * v.y() + m4x4[0][2] * v.z() + m4x4[0][3] * v.w(), 
            m4x4[1][0] * v.x() + m4x4[1][1] * v.y() + m4x4[1][2] * v.z() + m4x4[1][3] * v.w(),
            m4x4[2][0] * v.x() + m4x4[2][1] * v.y() + m4x4[2][2] * v.z() + m4x4[2][3] * v.w(),
            v.w());
    }

    inline Matrix4 operator *(Real scalar, const Matrix4 &rkM)
    {
        return Matrix4(
            scalar * rkM[0][0], scalar * rkM[0][1], scalar * rkM[0][2], scalar * rkM[0][3],
            scalar * rkM[1][0], scalar * rkM[1][1], scalar * rkM[1][2], scalar * rkM[1][3],
            scalar * rkM[2][0], scalar * rkM[2][1], scalar * rkM[2][2], scalar * rkM[2][3],
            scalar * rkM[3][0], scalar * rkM[3][1], scalar * rkM[3][2], scalar * rkM[3][3]
            );
    }

    inline Vector4 operator *(const Vector4 &rkV, const Matrix4 &rkM)
    {
        return Vector4(
            rkV.x() * rkM[0][0] + rkV.y() * rkM[1][0] + rkV.z() * rkM[2][0] + rkV.w() * rkM[3][0],
            rkV.x() * rkM[0][1] + rkV.y() * rkM[1][1] + rkV.z() * rkM[2][1] + rkV.w() * rkM[3][1],
            rkV.x() * rkM[0][2] + rkV.y() * rkM[1][2] + rkV.z() * rkM[2][2] + rkV.w() * rkM[3][2],
            rkV.x() * rkM[0][3] + rkV.y() * rkM[1][3] + rkV.z() * rkM[2][3] + rkV.w() * rkM[3][3]
            );
    }
}
