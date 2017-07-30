/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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
    ObbBoundPtr ObbBound::create(uint32_t unID, SGNode *node)
    {
        ObbBoundPtr bound = new ObbBound(unID, node);
        bound->release();
        return bound;
    }

    ObbBound::ObbBound(uint32_t unID, SGNode *node)
        : Bound(unID, node)
    {

    }

    ObbBound::~ObbBound()
    {

    }

    Bound::Type ObbBound::getType() const
    {
        return E_BT_OBB;
    }

    SGRenderablePtr ObbBound::getRenderable()
    {
        return mRenderable;
    }

    BoundPtr ObbBound::clone() const
    {
        ObbBoundPtr bound = new ObbBound(getID(), getNode());
        bound->release();
        cloneProperties(bound);
        return bound;
    }

    bool ObbBound::testSphere(const SphereBoundPtr &bound) const
    {
        return Math::intersects(bound->getSphere(), mObb);
    }

    bool ObbBound::testAabb(const AabbBoundPtr &bound) const
    {
        return Math::intersects(bound->getAlignAxisBox(), mObb);
    }

    bool ObbBound::testObb(const ObbBoundPtr &bound) const
    {
        return Math::intersects(mObb, bound->getObb());
    }

    bool ObbBound::testFrustum(const FrustumBoundPtr &bound) const
    {
        return Math::intersects(mObb, bound->getFrustum());
    }

    void ObbBound::updateBound(const Transform &transform)
    {

    }

    void ObbBound::cloneProperties(const BoundPtr &bound) const
    {
        Bound::cloneProperties(bound);

        const ObbBoundPtr &newBound = (const ObbBoundPtr &)bound;
        newBound->mObb = mObb;
        newBound->mOriginalObb = mOriginalObb;
        newBound->mRenderable = smart_pointer_cast<SGBox>(mRenderable->clone());

    }
}
