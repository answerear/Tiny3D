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


#ifndef __T3D_AABB_BOUND_H__
#define __T3D_AABB_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API AabbBound : public Bound
    {
         TRTTI_ENABLE(Bound)
         TRTTI_FRIEND

    public:
        static AabbBoundPtr create();

        ~AabbBound() override;

        TFUNCTION("Description"="Bound Type")
        Type getType() const override;

        ComponentPtr clone() const override;

        TPROPERTY(RTTRFuncName="AABB", RTTRFuncType="getter", "Description"="Aligned Axis Bounding Box")
        const Aabb &getAabb() const { return mAabb; }

        TPROPERTY(RTTRFuncName="originalAABB", RTTRFuncType="getter", "Description"="Original Aligned Axis Bounding Box")
        const Aabb& getOriginalAabb() const { return mOriginalAabb; }

        TFUNCTION()
        void setParams(Real minX, Real maxX, Real minY, Real maxY, Real minZ, Real maxZ)
        {
            mOriginalAabb.setParam(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ));
        }

        Renderable *getRenderable() override;
        
    protected:
        AabbBound() = default;

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
        TPROPERTY(RTTRFuncName="AABB", RTTRFuncType="setter", "Description"="Aligned Axis Bounding Box")
        void setAabb(const Aabb &aabb) { mAabb = aabb; }

        TPROPERTY(RTTRFuncName="originalAABB", RTTRFuncType="setter", "Description"="Original Aligned Axis Bounding Box")
        void setOriginalAabb(const Aabb &aabb) { mOriginalAabb = aabb; }

    private:
        /// 实时变换的AABB
        Aabb    mAabb {};
        /// 不参与变换的原始AABB
        Aabb    mOriginalAabb {};
    };
}

#endif  /*__T3D_AABB_BOUND_H__*/
