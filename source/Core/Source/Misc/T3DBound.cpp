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

#include "Bound/T3DBound.h"
#include "Bound/T3DSphereBound.h"
#include "Bound/T3DAabbBound.h"
#include "Bound/T3DObbBound.h"
#include "SceneGraph/T3DSGNode.h"
#include "T3DTransform.h"
#include "SceneGraph/T3DSGRenderable.h"


namespace Tiny3D
{
    Bound::Bound(uint32_t unID, SGNode *node)
        : mNode(node)
        , mID(unID)
        , mGroupID(0)
        , mMovable(false)
        , mCollisionSource(false)
        , mEnable(true)
    {

    }

    Bound::~Bound()
    {
        mNode = nullptr;
    }

    void Bound::setTransform(const Transform &transform)
    {
        updateBound(transform);
    }

    bool Bound::test(const BoundPtr &bound) const
    {
        bool result = false;

        switch (bound->getType())
        {
        case E_BT_SPHERE:
            {
                result = testSphere((const SphereBoundPtr &)bound);
            }
            break;
        case E_BT_AABB:
            {
                result = testAabb((const AabbBoundPtr &)bound);
            }
            break;
        case E_BT_OBB:
            {
                result = testObb((const ObbBoundPtr &)bound);
            }
            break;
        case E_BT_FRUSTUM:
            {
                result = testFrustum((const FrustumBoundPtr &)bound);
            }
            break;
        default:
            break;
        }

        return result;
    }

    void Bound::cloneProperties(const BoundPtr &bound) const
    {
        bound->mSphere = mSphere;
        bound->mNode = mNode;
        bound->mID = mID;
        bound->mGroupID = mGroupID;
        bound->mMovable = mMovable;
        bound->mCollisionSource = mCollisionSource;
        bound->mEnable = mEnable;
    }
}
