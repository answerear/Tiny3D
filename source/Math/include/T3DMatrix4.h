

#ifndef __T3D_MATRIX4_H__
#define __T3D_MATRIX4_H__


#include "T3DMathPrerequisites.h"
#include "T3DMath.h"
#include "T3DMatrix3.h"
#include "T3DQuaternion.h"
#include "T3DVector4.h"


namespace Tiny3D
{
    class T3D_MATH_API Matrix4
    {
    public:
        /// Default constructor.
        Matrix4(bool bZero = false);
        /// Copy constructor.
        Matrix4(const Matrix4 &other);
        /// Create and initialize using rows and columns of numbers.
        Matrix4(Real m00, Real m01, Real m02, Real m03,
                Real m10, Real m11, Real m12, Real m13,
                Real m20, Real m21, Real m22, Real m23,
                Real m30, Real m31, Real m32, Real m33);
        /// Create a standard 4x4 transformation matrix with a zero translation part from a
        /// rotation/scaling 3x3 matrix.
        Matrix4(const Matrix3 &rkMat);
        /// Create a standard 4x4 transformation matrix with a zero translation part from a
        /// rotation/scaling quaternion.
        Matrix4(const Quaternion &rkRot);

        /// Make zero matrix.
        void makeZero();
        /// Make identity matrix.
        void makeIdentity();

        /// Get array of matrix members in row major (constant).
        operator const Real *() const;
        /// Get array of matrix members in row major.
        operator Real *();

        /// Get array of matrix row members (constant).
        const Real *operator [](int32_t nRow) const;
        /// Get array of matrix row members.
        Real *operator [](int32_t nRow);

        /// Get a matrix member (constant).
        Real operator ()(int32_t nRow, int32_t nCol) const;
        /// Get a matrix member.
        Real &operator ()(int32_t nRow, int32_t nCol);

        /// Assignment.
        Matrix4 &operator =(const Matrix4 &other);
        /// Assignment from Matrix3
        void operator =(const Matrix3 &rkMat);

        /// Comparison (equal to).
        bool operator ==(const Matrix4 &other) const;
        /// Comparison (not equal to).
        bool operator !=(const Matrix4 &other) const;
        /// Comparison (less than).
        bool operator <(const Matrix4 &other) const;
        /// Comparison (less than or equal to).
        bool operator <=(const Matrix4 &other) const;
        /// COmparison (greater than).
        bool operator >(const Matrix4 &other) const;
        /// Comparison (greater than or equal to).
        bool operator >=(const Matrix4 &other) const;

        /// Addition.
        Matrix4 operator +(const Matrix4 &other) const;
        /// Subtraction.
        Matrix4 operator -(const Matrix4 &other) const;

        /// Dot product.
        Matrix4 operator *(const Matrix4 &other) const;

        /// Scalar multiplication.
        Matrix4 operator *(Real fScalar) const;
        /// Scalar division.
        Matrix4 operator /(Real fScalar) const;

        /// Negation.
        Matrix4 operator -() const;

        /// Add and assign.
        Matrix4 &operator +=(const Matrix4 &other);
        /// Subtract and assign.
        Matrix4 &operator -=(const Matrix4 &other);
        /// Scalar multiply and assign.
        Matrix4 &operator *=(Real fScalar);
        /// Scalar divide and assign.
        Matrix4 &operator /=(Real fScalar);

        /// Matrix times vector (M * V).
        Vector4 operator *(const Vector4 &rkV) const;
        Vector3 operator *(const Vector3 &rkV) const;

        /// Transpose (M ^ T).
        Matrix4 transpose() const;

        /// Inverse matrix.
        Matrix4 inverse() const;
        /// Inverse of the affine matrix.
        Matrix4 inverseAffine() const;

        /// Check whether or not the matrix is affine matrix.
        bool isAffine() const;

        ///  Determines if this matrix involves a scaling.
        bool hasScale() const;

        /// Adjoint matrix.
        Matrix4 adjoint() const;

        /// Calculate determinant.
        Real determinant() const;

        /// Concatenate two affine matrices.
        Matrix4 concatenateAffine(const Matrix4 &other) const;

        /// Decompose a Matrix4 to orientation / scale / position.
        void decomposition(Vector3 &position, Vector3 &scale, Quaternion &orientation) const;

        /// Extracts the rotation / scaling part of the matrix as a 3x3 matrix.
        void extractMatrix(Matrix3 &rkMat) const;
        /// Extracts the rotation / scaling part as a quaternion from the matrix.
        Quaternion extractQuaternion() const;
        /// Extracts the translation transformation part of the matrix.
        Vector3 extractTranslation() const;

        /// Builds a Matrix4 from orientation / scale / position.
        void makeTransform(const Vector3 &position, const Vector3 &scale, const Quaternion &orientation);
        /// Builds an inverse Matrix4 from orientation / scale / position.
        void makeInverseTransform(const Vector3 &position, const Vector3 &scale, const Quaternion &orientation);

        /// Builds a translate matrix by a Vector3.
        void makeTranslate(const Vector3 &position);
        /// Builds a translate matrix by 3 real values.
        void makeTranslate(Real tx, Real ty, Real tz);

        /// Sets the scale part of the matrix.
        void setScale(const Vector3 &scale);

        /// sets the translation transformation part of the matrix.
        void setTranslate(const Vector3 &position);

        /// 3-D Vector transformation specially for an affine matrix.
        Vector3 transformAffine(const Vector3 &v) const;
        /// 4-D Vector transformation specially for an affine matrix.
        Vector4 transformAffine(const Vector4 &v) const;

        static const Matrix4 ZERO;
        static const Matrix4 IDENTITY;

    protected:
        int32_t compareArrays(const Matrix4 &other) const;

    private:
        union
        {
            Real _m4x4[4][4];
            Real _m16[16];
        };
    };

    Matrix4 operator *(Real fScalar, const Matrix4 &rkM);

    Vector4 operator *(const Vector4 &rkV, const Matrix4 &rkM);
}


#include "T3DMatrix4.inl"


#endif  /*__T3D_MATRIX4_H__*/
