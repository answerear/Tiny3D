/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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

#ifndef __T3D_TRANSFORM_H__
#define __T3D_TRANSFORM_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    TSTRUCT()
    struct T3D_ENGINE_API Transform
    {
    public:
        Transform();

        Transform(const Vector3 &translation, const Vector3 &scaling, const Quaternion &orientation);

        Transform(const Transform &other);

        Transform &operator =(const Transform &other);

        TPROPERTY(RTTRFuncName="Translation", RTTRFuncType="setter")
        void setTranslation(const Vector3 &translation);

        TPROPERTY(RTTRFuncName="Translation", RTTRFuncType="getter")
        const Vector3 &getTranslation() const;

        Vector3 &getTranslation();

        TPROPERTY(RTTRFuncName="Scaling", RTTRFuncType="setter")
        void setScaling(const Vector3 &scaling);

        TPROPERTY(RTTRFuncName="Scaling", RTTRFuncType="getter")
        const Vector3 &getScaling() const;

        Vector3 &getScaling();

        TPROPERTY(RTTRFuncName="Orientation", RTTRFuncType="setter")
        void setOrientation(const Quaternion &orientation);

        TPROPERTY(RTTRFuncName="Orientation", RTTRFuncType="getter")
        const Quaternion &getOrientation() const;

        Quaternion &getOrientation();

        void applyTransform(const Transform &xform1, const Transform &xform2);

        void applyTransform(const Transform &xform, const Vector3 &translation, const Quaternion &orientation, const Vector3 &scaling);

        const Matrix4 &getAffineMatrix() const;

        void update();

    private:
        void makeAffineMatrix();

    private:
        /// 平移
        Vector3     mTranslation {};
        /// 缩放
        Vector3     mScaling {};
        /// 朝向
        Quaternion  mOrientation {};

        /// 根据 TRS 构造的仿射变黄
        Matrix4     mAffineMatrix {};
    };
}


#include "T3DTransform.inl"


#endif    /*__T3D_TRANSFORM_H__*/
