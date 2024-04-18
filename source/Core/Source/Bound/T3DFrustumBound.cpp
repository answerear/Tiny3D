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
#include "Component/T3DTransform3D.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    FrustumBoundPtr FrustumBound::create(const UUID &uuid)
    {
        return new FrustumBound(uuid);
    }

    //--------------------------------------------------------------------------

    FrustumBound::FrustumBound(const UUID &uuid)
        : Bound(uuid)
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

    ComponentPtr FrustumBound::clone() const
    {
        FrustumBoundPtr newObj = new FrustumBound();
        if (T3D_FAILED(newObj->cloneProperties(this)))
        {
            newObj = nullptr;
        }
        return newObj;
    }

    //--------------------------------------------------------------------------

    TResult FrustumBound::cloneProperties(const Component * const src)
    {
        TResult ret = Bound::cloneProperties(src);

        if (ret == T3D_OK)
        {
            const FrustumBound *const newBound = static_cast<const FrustumBound* const>(src);
            mFrustum = newBound->mFrustum;
            mOriginalFrustum = newBound->mOriginalFrustum;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void FrustumBound::setFrustumFace(Face face, const Plane &plane)
    {
        mFrustum.setFace(face, plane);
        mOriginalFrustum.setFace(face, plane);
    }

    //--------------------------------------------------------------------------

    void FrustumBound::setFrustumFaces(Plane *plane, size_t planeCount)
    {
        T3D_ASSERT(planeCount <= E_MAX_FACE, "Frustum must be 6 faces !");

        for (size_t i = 0; i < planeCount; ++i)
        {
            mFrustum.setFace((Face)i, plane[i]);
            mOriginalFrustum.setFace((Face)i, plane[i]);
        }
    }

    //--------------------------------------------------------------------------

    Renderable *FrustumBound::getRenderable()
    {
        return nullptr;
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

    void FrustumBound::onPositionChanged(const Vector3 &oldPos, const Vector3 &newPos)
    {
        update();
    }
    
    //--------------------------------------------------------------------------

    void FrustumBound::onOrientationChanged(const Quaternion &oldOri, const Quaternion &newOri)
    {
        update();
    }

    //--------------------------------------------------------------------------

    void FrustumBound::onScalingChanged(const Vector3 &oldScaling, const Vector3 &newScaling)
    {
        update();
    }
    
    //--------------------------------------------------------------------------
    
    void FrustumBound::update()
    {
        // 这里就不做变换了，引擎直接通过相机重建frustum
    }

    //--------------------------------------------------------------------------
}

