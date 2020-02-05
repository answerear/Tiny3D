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


#include "Bound/T3DFrustumBound.h"
#include "Scene/T3DSceneBox.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    FrustumBoundPtr FrustumBound::create(ID uID /* = E_BID_AUTOMATIC */)
    {
        FrustumBoundPtr bound = new FrustumBound(uID);
        bound->release();
        return bound;
    }

    //--------------------------------------------------------------------------

    FrustumBound::FrustumBound(ID uID /* = E_BID_AUTOMATIC */)
        : Bound(uID)
    {

    }

    //--------------------------------------------------------------------------

    FrustumBound::~FrustumBound()
    {

    }

    //--------------------------------------------------------------------------

    Bound::Type FrustumBound::getType() const
    {
        return Type::FRUSTUM;
    }

    //--------------------------------------------------------------------------

    BoundPtr FrustumBound::clone() const
    {
        FrustumBoundPtr bound = FrustumBound::create();
        cloneProperties(bound);
        return bound;
    }

    //--------------------------------------------------------------------------

    void FrustumBound::cloneProperties(BoundPtr bound) const
    {
        Bound::cloneProperties(bound);

        FrustumBoundPtr newBound = smart_pointer_cast<FrustumBound>(bound);
        newBound->mFrustum = mFrustum;
        newBound->mOriginalFrustum = mOriginalFrustum;
        newBound->mRenderable = smart_pointer_cast<SceneBox>(mRenderable->clone());
    }

    //--------------------------------------------------------------------------

    void FrustumBound::setFrustumFace(Frustum::Face face, const Plane &plane)
    {
        mFrustum.setFace(face, plane);
        mOriginalFrustum.setFace(face, plane);
    }

    //--------------------------------------------------------------------------

    void FrustumBound::setFrustumFaces(Plane *plane, size_t planeCount)
    {
        T3D_ASSERT(planeCount <= Frustum::E_MAX_FACE);

        size_t i = 0;
        for (i = 0; i < planeCount; ++i)
        {
            mFrustum.setFace((Frustum::Face)i, plane[i]);
            mOriginalFrustum.setFace((Frustum::Face)i, plane[i]);
        }
    }

    //--------------------------------------------------------------------------

    SceneRenderablePtr FrustumBound::getRenderable()
    {
        return mRenderable;
    }

    //--------------------------------------------------------------------------

    bool FrustumBound::testSphere(const Sphere &sphere) const
    {
        IntrFrustumSphere intr(&mFrustum, &sphere);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool FrustumBound::testAabb(const Aabb &aabb) const
    {
        IntrFrustumAabb intr(&mFrustum, &aabb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool FrustumBound::testObb(const Obb &obb) const
    {
        IntrFrustumObb intr(&mFrustum, &obb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool FrustumBound::testFrustum(const Frustum &frustum) const
    {
        // 暂时不具备这种检测能力，而且实际应用中应该也没有这样的需求
        return false;
    }

    //--------------------------------------------------------------------------

    void FrustumBound::updateBound(const Transform &xform)
    {
        // 这里就不做变换了，引擎直接通过相机重建frustum
    }
}

