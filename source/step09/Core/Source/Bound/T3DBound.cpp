/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#include "Bound/T3DBound.h"
#include "Memory/T3DSmartPtr.h"
#include "Bound/T3DSphereBound.h"
#include "Bound/T3DAabbBound.h"
#include "Bound/T3DObbBound.h"
#include "Bound/T3DFrustumBound.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Bound::Bound(SceneNode *node, ID uID)
        : mNode(node)
        , mID(E_BID_INVALID)
        , mGroupID(E_BID_INVALID)
        , mIsMovable(false)
        , mIsCollisionSource(false)
        , mIsEnabled(false)
    {
        if (uID == E_BID_AUTOMATIC)
        {
            // 自动生成ID
            mID = makeGlobalID();
        }
        else
        {
            mID = uID;
        }
    }

    //--------------------------------------------------------------------------

    Bound::~Bound()
    {
        mNode = nullptr;
    }

    //--------------------------------------------------------------------------

    bool Bound::test(BoundPtr bound) const
    {
        bool ret = false;

        switch (bound->getType())
        {
        case Type::SPHERE:
            {
                SphereBoundPtr sphere = smart_pointer_cast<SphereBound>(bound);
                ret = testSphere(sphere->getSphere());
            }
            break;
        case Type::AABB:
            {
                AabbBoundPtr aabb = smart_pointer_cast<AabbBound>(bound);
                ret = testAabb(aabb->getAlignAxisBox());
            }
            break;
        case Type::OBB:
            {
                ObbBoundPtr obb = smart_pointer_cast<ObbBound>(bound);
                ret = testObb(obb->getObb());
            }
            break;
        case Type::FRUSTUM:
            {
                FrustumBoundPtr frustum
                    = smart_pointer_cast<FrustumBound>(bound);
                ret = testFrustum(frustum->getFrustum());
            }
            break;
        default:
            break;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void Bound::cloneProperties(BoundPtr bound) const
    {
        bound->mSphere = mSphere;
        bound->mNode = mNode;
        bound->mID = mID;
        bound->mGroupID = mGroupID;
        bound->mIsMovable = mIsMovable;
        bound->mIsCollisionSource = mIsCollisionSource;
        bound->mIsEnabled = mIsEnabled;
    }

    //--------------------------------------------------------------------------

    ID Bound::makeGlobalID() const
    {
        static ID uID = 0;
        return ++uID;
    }
}
