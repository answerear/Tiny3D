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


#include "Bound/T3DObbBound.h"
#include "Component/T3DTransform3D.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ObbBoundPtr ObbBound::create()
    {
        return new ObbBound();
    }

    //--------------------------------------------------------------------------

    ObbBound::~ObbBound()
    {

    }

    //--------------------------------------------------------------------------

    Bound::Type ObbBound::getType() const
    {
        return Type::OBB;
    }

    //--------------------------------------------------------------------------

    ComponentPtr ObbBound::clone() const
    {
        ObbBoundPtr newObj = new ObbBound();
        if (T3D_FAILED(newObj->cloneProperties(this)))
        {
            newObj = nullptr;
        }
        return newObj;
    }

    //--------------------------------------------------------------------------

    TResult ObbBound::cloneProperties(const Component * const src)
    {
        TResult ret = Bound::cloneProperties(src);

        if (ret == T3D_OK)
        {
            const ObbBound *const newBound = static_cast<const ObbBound* const>(src);
            mObb = newBound->mObb;
            mOriginalObb = newBound->mOriginalObb;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    Renderable *ObbBound::getRenderable()
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    bool ObbBound::testSphere(const Sphere &sphere) const
    {
        IntrSphereObb intr(&sphere, &mObb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool ObbBound::testAabb(const Aabb &aabb) const
    {
        IntrAabbObb intr(&aabb, &mObb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool ObbBound::testObb(const Obb &obb) const
    {
        IntrObbObb intr(&mObb, &obb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool ObbBound::testFrustum(const Frustum &frustum) const
    {
        IntrFrustumObb intr(&frustum, &mObb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    void ObbBound::onPositionChanged(const Vector3 &oldPos, const Vector3 &newPos)
    {
        update();
    }
    
    //--------------------------------------------------------------------------

    void ObbBound::onOrientationChanged(const Quaternion &oldOri, const Quaternion &newOri)
    {
        update();
    }

    //--------------------------------------------------------------------------

    void ObbBound::onScalingChanged(const Vector3 &oldScaling, const Vector3 &newScaling)
    {
        update();
    }
    
    //--------------------------------------------------------------------------

    void ObbBound::update()
    {
        GameObject *go = getGameObject();
        T3D_ASSERT(go != nullptr, "Game object must be not nullptr !");
        Transform3D *xform = go->getComponent<Transform3D>();
        if (xform != nullptr)
        {
            const Transform &transform = xform->getLocalToWorldTransform();

            // 变换中点
            Vector3 center = transform.getAffineMatrix() * mOriginalObb.getCenter();

            // 变换3个轴
            Vector3 axis0 = transform.getAffineMatrix() * (mOriginalObb.getAxis(0) * mOriginalObb.getExtent(0));
            Vector3 axis1 = transform.getAffineMatrix() * (mOriginalObb.getAxis(1) * mOriginalObb.getExtent(1));
            Vector3 axis2 = transform.getAffineMatrix() * (mOriginalObb.getAxis(2) * mOriginalObb.getExtent(2));

            // 计算三个轴上的长度
            Real extent0 = axis0.length();
            Real extent1 = axis1.length();
            Real extent2 = axis2.length();

            axis0 /= extent0;
            axis1 /= extent1;
            axis2 /= extent2;

            mObb.setCenter(center);
            mObb.setAxis(axis0, axis1, axis2);
            mObb.setExtent(0, extent0);
            mObb.setExtent(1, extent1);
            mObb.setExtent(2, extent2);
        }
    }
}


