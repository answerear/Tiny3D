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


#ifndef __T3D_SPHERE_BOUND_H__
#define __T3D_SPHERE_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API SphereBound : public Bound
    {
        TRTTI_ENABLE(Bound)
        TRTTI_FRIEND

    public:
        static SphereBoundPtr create(const UUID &uuid = UUID::INVALID);

        ~SphereBound() override;

        TFUNCTION()
        Type getType() const override;

        ComponentPtr clone() const override;

        TFUNCTION()
        void setParams(const Vector3 &center, Real radius)
        {
            mOriginalSphere.setCenter(center);
            mOriginalSphere.setRadius(radius);

            mSphere.setCenter(center);
            mSphere.setRadius(radius);
        }

        TPROPERTY(RTTRFuncName="sphere", RTTRFuncType="getter", "Description"="Sphere")
        const Sphere &getSphere() const { return mSphere; }

        TPROPERTY(RTTRFuncName="originalSphere", RTTRFuncType="getter", "Description"="OriginalSphere")
        const Sphere& getOriginalSphere() const { return mOriginalSphere; }

        Renderable *getRenderable() override;

    protected:
        SphereBound(const UUID &uuid = UUID::INVALID);

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
        TPROPERTY(RTTRFuncName="sphere", RTTRFuncType="setter", "Description"="Sphere")
        void setSphere(const Sphere &sphere) { mSphere = sphere; }

        TPROPERTY(RTTRFuncName="originalSphere", RTTRFuncType="setter", "Description"="OriginalSphere")
        void setOriginalSphere(const Sphere &sphere) { mOriginalSphere = sphere; }

    private:
        /// 原始球体
        Sphere    mOriginalSphere {};
        /// 用于快速检测相交性的球体
        Sphere    mSphere {};
    };
}

#endif  /*__T3D_SPHERE_BOUND_H__*/
