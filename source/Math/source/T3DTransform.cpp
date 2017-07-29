/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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
