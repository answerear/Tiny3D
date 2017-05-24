
#include "T3DTransform.h"


namespace Tiny3D
{
    void Transform::applyTransform(const Transform &rkTransform, const Vector3 &rkTranslation, const Quaternion &rkOrientation, const Vector3 &rkScale)
    {
//                 Quaternion R = transform.getOrientation() * mOrientation;
//                 Vector3 S = transform.getScale() * mScale;
//                 Vector3 T = transform.getOrientation() * mPosition;
//                 T = T * transform.getScale();
//                 T = transform.getTranslate() + T;
//                 mWorldTransform.setTranslate(T);
//                 mWorldTransform.setOrientation(R);
//                 mWorldTransform.setScale(S);
//                 mWorldTransform.update();
        mOrientation = rkTransform.getOrientation() * rkOrientation;
        mScale = rkTransform.getScale() * rkScale;
        mTranslate = rkTransform.getOrientation() * rkTranslation;
        mTranslate = mTranslate * rkTransform.getScale();
        mTranslate = rkTransform.getTranslate() + mTranslate;
        makeAffineMatrix();
//         const Matrix4 &ParentM = rkTransform.getAffineMatrix();
//         Matrix4 T(rkTranslation), S(rkScale[0], rkScale[1], rkScale[2], 1.0f), R(rkOrientation);
//         Matrix4 LocalM = T * R * S;
//         mAffineMatrix = ParentM * LocalM;
//         mAffineMatrix.decomposition(mTranslate, mScale, mOrientation);
    }

    void Transform::applyTransform(const Transform &rkTransform1, const Transform &rkTransform2)
    {
        applyTransform(rkTransform1, rkTransform2.getTranslate(), rkTransform2.getOrientation(), rkTransform2.getScale());
    }
}
