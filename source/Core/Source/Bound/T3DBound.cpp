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


#include "Bound/T3DBound.h"
#include "Bound/T3DSphereBound.h"
#include "Bound/T3DAabbBound.h"
#include "Bound/T3DObbBound.h"
#include "Bound/T3DFrustumBound.h"
#include "Component/T3DRenderable.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Bound::Bound(ID uID)
        : Component(uID)
        , mGroupID(E_CID_INVALID)
        , mIsMovable(false)
        , mIsCollisionSource(false)
        , mIsEnabled(false)
        , mIsDirty(true)
    {
    }

    //--------------------------------------------------------------------------

    Bound::~Bound()
    {
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

    TResult Bound::cloneProperties(ComponentPtr newObj) const
    {
        TResult ret = Component::cloneProperties(newObj);

        if (ret == T3D_OK)
        {
            BoundPtr bound = smart_pointer_cast<Bound>(newObj);
            bound->mGroupID = mGroupID;
            bound->mIsMovable = mIsMovable;
            bound->mIsCollisionSource = mIsCollisionSource;
            bound->mIsEnabled = mIsEnabled;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void Bound::updateTransform(const Transform3D *xform)
    {
        mIsDirty = true;
    }
}

// //------------------------------------------------------------------------------
// //                                  RTTR
// //------------------------------------------------------------------------------
//
// RTTR_REGISTRATION
// {
//     using namespace rttr;
//
//     registration::class_<Tiny3D::Bound>("Tiny3D::Bound")
//         .enumeration<Tiny3D::Bound::Type>("Tiny3D::Bound::Type")
//         (
//             value("NONE", Tiny3D::Bound::Type::NONE),
//             value("SPHERE", Tiny3D::Bound::Type::SPHERE),
//             value("AABB", Tiny3D::Bound::Type::AABB),
//             value("OBB", Tiny3D::Bound::Type::OBB),
//             value("FRUSTUM", Tiny3D::Bound::Type::FRUSTUM)
//         )
//         .method("getType", &Tiny3D::Bound::getType)
//         .method("test", &Tiny3D::Bound::test)
//         .method("getRenderable", &Tiny3D::Bound::getRenderable)
//         .property("GroupID", &Tiny3D::Bound::getGroupID, &Tiny3D::Bound::setGroupID)
//         (
//             metadata(TXT_DESCRIPTION, "Group ID is used to divide different bounds.")
//         )
//         .property("CollsionSource", &Tiny3D::Bound::isCollisionSource, &Tiny3D::Bound::setCollisionSource)
//         (
//             metadata(TXT_DESCRIPTION, "The flag is used to mark the collision detection source.")
//         )
//         .property("Enabled", &Tiny3D::Bound::isEnabled, &Tiny3D::Bound::setEnabled)
//         (
//             metadata(TXT_DESCRIPTION, "The flag is used to the bounding volume is valid or not.")
//         );
// }

