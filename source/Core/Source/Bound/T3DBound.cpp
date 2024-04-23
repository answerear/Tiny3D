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
#include "Component/T3DTransform3D.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Bound::~Bound()
    {
    }

    //--------------------------------------------------------------------------

    bool Bound::test(Bound *bound) const
    {
        bool ret = false;

        switch (bound->getType())
        {
        case Type::SPHERE:
            {
                SphereBound *sphere = static_cast<SphereBound*>(bound);
                ret = testSphere(sphere->getSphere());
            }
            break;
        case Type::AABB:
            {
                AabbBound *aabb = static_cast<AabbBound*>(bound);
                ret = testAabb(aabb->getAlignAxisBox());
            }
            break;
        case Type::OBB:
            {
                ObbBound *obb = static_cast<ObbBound*>(bound);
                ret = testObb(obb->getObb());
            }
            break;
        case Type::FRUSTUM:
            {
                FrustumBound *frustum
                    = static_cast<FrustumBound*>(bound);
                ret = testFrustum(frustum->getFrustum());
            }
            break;
        default:
            break;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Bound::cloneProperties(const Component * const src)
    {
        TResult ret = Component::cloneProperties(src);

        if (ret == T3D_OK)
        {
            const Bound * const bound = static_cast<const Bound* const>(src);
            mGroupID = bound->mGroupID;
            mIsMovable = bound->mIsMovable;
            mIsCollisionSource = bound->mIsCollisionSource;
            mIsEnabled = bound->mIsEnabled;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void Bound::onStart()
    {
        GameObject *go = getGameObject();
        if (go != nullptr)
        {
            Transform3D *xform = go->getComponent<Transform3D>();
            xform->addPositionChangedCallback(this, [this](const Vector3 &oldPos, const Vector3 &newPos)
                {
                    onPositionChanged(oldPos, newPos);
                });

            xform->addOrientationChangedCallback(this, [this](const Quaternion &oldOri, const Quaternion &newOri)
                {
                    onOrientationChanged(oldOri, newOri);
                });

            xform->addScalingChangedCallback(this, [this](const Vector3 &oldScaling, const Vector3 &newScaling)
                {
                    onScalingChanged(oldScaling, newScaling);
                });
        }
    }

    //--------------------------------------------------------------------------
}
