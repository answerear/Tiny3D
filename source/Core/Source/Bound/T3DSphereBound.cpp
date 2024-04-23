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


#include "Bound/T3DSphereBound.h"
#include "Component/T3DTransform3D.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SphereBoundPtr SphereBound::create()
    {
        return new SphereBound();
    }

    //--------------------------------------------------------------------------
    
    SphereBound::~SphereBound()
    {
    }

    //--------------------------------------------------------------------------

    Bound::Type SphereBound::getType() const
    {
        return Type::SPHERE;
    }

    //--------------------------------------------------------------------------

    ComponentPtr SphereBound::clone() const
    {
        SphereBoundPtr newObj = new SphereBound();
        if (T3D_FAILED(newObj->cloneProperties(this)))
        {
            newObj = nullptr;
        }
        return newObj;
    }

    //--------------------------------------------------------------------------

    TResult SphereBound::cloneProperties(const Component * const src)
    {
        TResult ret = Bound::cloneProperties(src);

        if (ret == T3D_OK)
        {
            const SphereBound * const sphereBound = static_cast<const SphereBound* const>(src);
            mOriginalSphere = sphereBound->mOriginalSphere;
            mSphere = sphereBound->mSphere;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    Renderable *SphereBound::getRenderable()
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    bool SphereBound::testSphere(const Sphere &sphere) const
    {
        IntrSphereSphere intr(&mSphere, &sphere);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool SphereBound::testAabb(const Aabb &aabb) const
    {
        IntrSphereAabb intr(&mSphere, &aabb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool SphereBound::testObb(const Obb &obb) const
    {
        IntrSphereObb intr(&mSphere, &obb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool SphereBound::testFrustum(const Frustum &frustum) const
    {
        IntrFrustumSphere intr(&frustum, &mSphere);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    void SphereBound::onPositionChanged(const Vector3 &oldPos, const Vector3 &newPos)
    {
        update();
    }

    
    //--------------------------------------------------------------------------

    void SphereBound::onOrientationChanged(const Quaternion &oldOri, const Quaternion &newOri)
    {
        update();
    }

    //--------------------------------------------------------------------------

    void SphereBound::onScalingChanged(const Vector3 &oldScaling, const Vector3 &newScaling)
    {
        update();
    }

    //--------------------------------------------------------------------------

    void SphereBound::update()
    {
        GameObject *go = getGameObject();
        T3D_ASSERT(go != nullptr, "Game object must be not nullptr !");
        Transform3D *xform = go->getComponent<Transform3D>();
        if (xform != nullptr)
        {
            const Transform &transform = xform->getLocalToWorldTransform();

            // 更新变换后用于碰撞检测的球体
            const Matrix4 &M = transform.getAffineMatrix();
            Vector3 center = M * mOriginalSphere.getCenter();

            const Vector3 &S = transform.getScaling();
            Real factor = std::max(std::max(S.x(), S.y()), S.z());
            Real radius = factor * mOriginalSphere.getRadius();

            mSphere.setCenter(center);
            mSphere.setRadius(radius);

            // 原始球体，只更新球心位置，不更新半径大小
            center = M * mOriginalSphere.getCenter();
            mOriginalSphere.setCenter(center);
        }
    }
}
