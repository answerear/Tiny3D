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

#ifndef __T3D_FRUSTUM_BOUND_H__
#define __T3D_FRUSTUM_BOUND_H__


#include "Bound/T3DBound.h"
#include "T3DFrustum.h"


namespace Tiny3D
{
    class T3D_ENGINE_API FrustumBound : public Bound
    {
    public:
        static FrustumBoundPtr create(uint32_t unID, SGNode *node);

        virtual ~FrustumBound();

        virtual Type getType() const override;
        virtual SGRenderablePtr getRenderable() override;
        virtual BoundPtr clone() const override;

        const Frustum &getFrustum() const   { return mFrustum; }
        void setFrustum(Frustum::Face face, const Plane &plane) { mFrustum.setFace(face, plane); }

        void setParam(const Plane *plane, size_t planeCount);

    protected:
        FrustumBound(uint32_t unID, SGNode *node);

        virtual bool testSphere(const SphereBoundPtr &bound) const override;
        virtual bool testAabb(const AabbBoundPtr &bound) const override;
        virtual bool testObb(const ObbBoundPtr &bound) const override;
        virtual bool testFrustum(const FrustumBoundPtr &bound) const override;

        virtual void updateBound(const Transform &transform) override;

        virtual void cloneProperties(const BoundPtr &bound) const;

        Frustum     mFrustum;
        SGBoxPtr    mRenderable;
    };
}


#endif  /*__T3D_FRUSTUM_BOUND_H__*/
