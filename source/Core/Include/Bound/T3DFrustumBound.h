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


#ifndef __T3D_FRUSTUM_BOUND_H__
#define __T3D_FRUSTUM_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API FrustumBound : public Bound
    {
        TRTTI_ENABLE(Bound)
        TRTTI_FRIEND

    public:
        static FrustumBoundPtr create(const UUID &uuid = UUID::INVALID);

        ~FrustumBound() override;

        TFUNCTION("Description"="Bound Type")
        Type getType() const override;

        ComponentPtr clone() const override;

        TPROPERTY(RTTRFuncName="frustum", RTTRFuncType="getter", "Description"="Frustum")
        const Frustum &getFrustum() const { return mFrustum; }

        TPROPERTY(RTTRFuncName="originalFrustum", RTTRFuncType="getter", "Description"="Original Frustum")
        const Frustum& getOriginalFrustum() const { return mOriginalFrustum; }

        TFUNCTION()
        void setFrustumFace(Face face, const Plane &plane);

        TFUNCTION()
        void setFrustumFaces(Plane *plane, size_t planeCount);

        Renderable *getRenderable() override;

    protected:
        FrustumBound(const UUID &uuid = UUID::INVALID);

        bool testSphere(const Sphere &sphere) const override;

        bool testAabb(const Aabb &aabb) const override;

        bool testObb(const Obb &obb) const override;

        bool testFrustum(const Frustum &frustum) const override;

        TResult cloneProperties(const Component * const src) override;

        void onPositionChanged(const Vector3 &oldPos, const Vector3 &newPos) override;

        void onOrientationChanged(const Quaternion &oldOri, const Quaternion &newOri) override;

        void onScalingChanged(const Vector3 &oldScaling, const Vector3 &newScaling) override;

        void update();

    private:
        TPROPERTY(RTTRFuncName="frustum", RTTRFuncType="setter", "Description"="Frustum")
        void setFrustum(const Frustum &frustum) { mFrustum = frustum; }

        TPROPERTY(RTTRFuncName="originalFrustum", RTTRFuncType="setter", "Description"="Original Frustum")
        void setOriginalFrustum(const Frustum &frustum) { mOriginalFrustum = frustum; }

    private:
        /// 可变换的视锥体对象
        Frustum mFrustum;
        /// 不参与变换的原始视锥体
        Frustum mOriginalFrustum;
    };
}


#endif  /*__T3D_FRUSTUM_BOUND_H__*/
