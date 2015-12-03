

#ifndef __T3D_TRANSFORM_H__
#define __T3D_TRANSFORM_H__


#include "T3DPrerequisites.h"
#include "Math/T3DVector3.h"
#include "Math/T3DQuaternion.h"
#include "Math/T3DMatrix4.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Transform
    {
    public:
        Transform();
        Transform(const Vector3 &rkTranslate, const Vector3 &rkScale, const Quaternion &rkOrientation);
        Transform(const Transform &rkOther);

        Transform &operator =(const Transform &rkOther);

        void setTranslate(const Vector3 &rkTranslate);
        const Vector3 &getTranslate() const;
        Vector3 &getTranslate();

        void setScale(const Vector3 &rkScale);
        const Vector3 &getScale() const;
        Vector3 &getScale();

        void setOrientation(const Quaternion &rkOrientation);
        const Quaternion &getOrientation() const;
        Quaternion &getOrientation();

        void update();

        const Matrix4 &getAffineMatrix() const;

    private:
        Vector3 mTranslate;
        Vector3 mScale;
        Quaternion  mOrientation;

        Matrix4 mAffineMatrix;
    };

    inline Transform::Transform()
        : mTranslate()
        , mScale(Real(1.0), Real(1.0), Real(1.0))
        , mOrientation(Real(0.0), Real(0.0), Real(1.0), Real(0.0))
    {
        mAffineMatrix.makeTransform(mTranslate, mScale, mOrientation);
    }

    inline Transform::Transform(const Vector3 &rkTranslate, const Vector3 &rkScale, const Quaternion &rkOrientation)
        : mTranslate(rkTranslate)
        , mScale(rkScale)
        , mOrientation(rkOrientation)
    {

    }

    inline Transform::Transform(const Transform &rkOther)
    {
        mTranslate = rkOther.mTranslate;
        mOrientation = rkOther.mOrientation;
        mScale = rkOther.mScale;
        mAffineMatrix = rkOther.mAffineMatrix;
    }

    inline Transform &Transform::operator =(const Transform &rkOther)
    {
        mTranslate = rkOther.mTranslate;
        mOrientation = rkOther.mOrientation;
        mScale = rkOther.mScale;
        mAffineMatrix = rkOther.mAffineMatrix;
        return *this;
    }

    inline void Transform::setTranslate(const Vector3 &rkTranslate)
    {
        mTranslate = rkTranslate;
    }

    inline const Vector3 &Transform::getTranslate() const
    {
        return mTranslate;
    }

    inline Vector3 &Transform::getTranslate()
    {
        return mTranslate;
    }

    inline void Transform::setScale(const Vector3 &rkScale)
    {
        mScale = rkScale;
    }

    inline const Vector3 &Transform::getScale() const
    {
        return mScale;
    }

    inline Vector3 &Transform::getScale()
    {
        return mScale;
    }

    inline void Transform::setOrientation(const Quaternion &rkOrientation)
    {
        mOrientation = rkOrientation;
    }

    inline const Quaternion &Transform::getOrientation() const
    {
        return mOrientation;
    }

    inline Quaternion &Transform::getOrientation()
    {
        return mOrientation;
    }

    inline const Matrix4 &Transform::getAffineMatrix() const
    {
        return mAffineMatrix;
    }

    inline void Transform::update()
    {
        mAffineMatrix.makeTransform(mTranslate, mScale, mOrientation);
    }
}


#endif  /*__T3D_TRANSFORM_H__*/
