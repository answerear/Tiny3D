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

#include "Bound/T3DSphereBound.h"
#include "Bound/T3DAabbBound.h"
#include "Bound/T3DObbBound.h"
#include "Bound/T3DFrustumBound.h"
#include "T3DMath.h"
#include "SceneGraph/T3DSGNode.h"
#include "SceneGraph/T3DSGBox.h"


namespace Tiny3D
{
    FrustumBoundPtr FrustumBound::create(uint32_t unID, SGNode *node)
    {
        FrustumBoundPtr bound = new FrustumBound(unID, node);
        bound->release();
        return bound;
    }

    FrustumBound::FrustumBound(uint32_t unID, SGNode *node)
        : Bound(unID, node)
        , mRenderable(nullptr)
    {

    }

    FrustumBound::~FrustumBound()
    {
        mRenderable = nullptr;
    }

    Bound::Type FrustumBound::getType() const
    {
        return E_BT_OBB;
    }

    SGRenderablePtr FrustumBound::getRenderable()
    {
        return mRenderable;
    }

    BoundPtr FrustumBound::clone() const
    {
        FrustumBoundPtr bound = new FrustumBound(getID(), getNode());
        bound->release();
        cloneProperties(bound);
        return bound;
    }

    void FrustumBound::setParam(const Plane *plane, size_t planeCount)
    {
        size_t i = 0;
        for (i = 0; i < planeCount; ++i)
        {
            mFrustum.setFace((Frustum::Face)i, plane[i]);
        }
    }

    bool FrustumBound::testSphere(const SphereBoundPtr &bound) const
    {
        return Math::intersects(bound->getSphere(), mFrustum);
    }

    bool FrustumBound::testAabb(const AabbBoundPtr &bound) const
    {
        return Math::intersects(bound->getAlignAxisBox(), mFrustum);
    }

    bool FrustumBound::testObb(const ObbBoundPtr &bound) const
    {
        return Math::intersects(bound->getObb(), mFrustum);
    }

    bool FrustumBound::testFrustum(const FrustumBoundPtr &bound) const
    {
        return false;//Math::intersects(mFrustum, bound->getFrustum());
    }

    void FrustumBound::updateBound(const Transform &transform)
    {

    }

    void FrustumBound::cloneProperties(const BoundPtr &bound) const
    {
        Bound::cloneProperties(bound);

        const FrustumBoundPtr &newBound = (const FrustumBoundPtr &)bound;
        newBound->mFrustum = mFrustum;
    }
}
