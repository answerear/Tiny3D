﻿/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
 * Copyright (C) 2015-2019  Answer Wong
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


#include "Bound/T3DObbBound.h"
#include "SceneGraph/T3DSGBox.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ObbBoundPtr ObbBound::create(SGNode *node, ID uID /* = E_BID_AUTOMATIC */)
    {
        ObbBoundPtr bound = new ObbBound(node, uID);
        bound->release();
        return bound;
    }

    //--------------------------------------------------------------------------

    ObbBound::ObbBound(SGNode *node, ID uID /* = E_BID_AUTOMATIC */)
        : Bound(node, uID)
    {

    }

    //--------------------------------------------------------------------------

    ObbBound::~ObbBound()
    {

    }

    //--------------------------------------------------------------------------

    Bound::Type ObbBound::getType() const
    {
        return E_BT_OBB;
    }

    //--------------------------------------------------------------------------

    BoundPtr ObbBound::clone() const
    {
        ObbBoundPtr bound = ObbBound::create(getNode());
        cloneProperties(bound);
        return bound;
    }

    //--------------------------------------------------------------------------

    void ObbBound::cloneProperties(BoundPtr bound) const
    {
        Bound::cloneProperties(bound);

        ObbBoundPtr newBound = smart_pointer_cast<ObbBound>(bound);
        newBound->mObb = mObb;
        newBound->mOriginalObb = mOriginalObb;
        newBound->mRenderable = smart_pointer_cast<SGBox>(mRenderable->clone());
    }

    //--------------------------------------------------------------------------

    void ObbBound::setCenter(const Vector3 &center)
    {
        mObb.setCenter(center);
        mOriginalObb.setCenter(center);
    }

    //--------------------------------------------------------------------------

    void ObbBound::setAxis(const Vector3 &axis0, const Vector3 &axis1,
        const Vector3 &axis2)
    {
        mObb.setAxis(axis0, axis1, axis2);
        mOriginalObb.setAxis(axis0, axis1, axis2);
    }

    //--------------------------------------------------------------------------

    void ObbBound::setExtent(Real extent0, Real extent1, Real extent2)
    {
        mObb.setExtent(0, extent0);
        mObb.setExtent(1, extent1);
        mObb.setExtent(2, extent2);

        mOriginalObb.setExtent(0, extent0);
        mOriginalObb.setExtent(1, extent1);
        mOriginalObb.setExtent(2, extent2);
    }

    //--------------------------------------------------------------------------

    SGRenderablePtr ObbBound::getRenderable()
    {
        return mRenderable;
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

    void ObbBound::updateBound(const Transform &xform)
    {
        // 变换中点
        Vector3 center = xform.getAffineMatrix() * mOriginalObb.getCenter();

        // 变换3个轴
        Vector3 axis0 = xform.getAffineMatrix() 
            * (mOriginalObb.getAxis(0) * mOriginalObb.getExtent(0));
        Vector3 axis1 = xform.getAffineMatrix() 
            * (mOriginalObb.getAxis(1) * mOriginalObb.getExtent(1));
        Vector3 axis2 = xform.getAffineMatrix() 
            * (mOriginalObb.getAxis(2) * mOriginalObb.getExtent(2));

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
