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


#ifndef __T3D_OBB_BOUND_H__
#define __T3D_OBB_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API ObbBound : public Bound
    {
        TRTTI_ENABLE(Bound)
        TRTTI_FRIEND

    public:
        static ObbBoundPtr create();

        ~ObbBound() override;

        TFUNCTION("Description"="Bound Type")
        Type getType() const override;

        ComponentPtr clone() const override;

        TPROPERTY(RTTRFuncName="OBB", RTTRFuncType="getter", "Description"="Oriented Bounding Box")
        const Obb &getObb() const { return mObb; }

        TPROPERTY(RTTRFuncName="originalOBB", RTTRFuncType="getter", "Description"="Original Oriented Bounding Box")
        const Obb& getOriginalObb() const { return mOriginalObb; }

        TFUNCTION()
        void setCenter(const Vector3 &center)
        {
            mObb.setCenter(center);
            mOriginalObb.setCenter(center);
        }

        TFUNCTION()
        void setAxis(const Vector3 &axis0, const Vector3 &axis1, const Vector3 &axis2)
        {
            mObb.setAxis(axis0, axis1, axis2);
            mOriginalObb.setAxis(axis0, axis1, axis2);
        }

        TFUNCTION()
        void setExtent(Real extent0, Real extent1, Real extent2)
        {
            mObb.setExtent(0, extent0);
            mObb.setExtent(1, extent1);
            mObb.setExtent(2, extent2);

            mOriginalObb.setExtent(0, extent0);
            mOriginalObb.setExtent(1, extent1);
            mOriginalObb.setExtent(2, extent2);
        }

        Renderable *getRenderable() override;

    protected:
        ObbBound() = default;

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
        TPROPERTY(RTTRFuncName="OBB", RTTRFuncType="setter", "Description"="Oriented Bounding Box")
        void setObb(const Obb &obb) { mObb = obb; }

        TPROPERTY(RTTRFuncName="originalOBB", RTTRFuncType="setter", "Description"="Original Oriented Bounding Box")
        void setOriginalObb(const Obb &obb) { mOriginalObb = obb; }
        
    private:
        /// 实时变换的OBB
        Obb mObb {};
        /// 不参与变换的原始OBB
        Obb mOriginalObb {};
    };
}


#endif  /*__T3D_OBB_BOUND_H__*/
