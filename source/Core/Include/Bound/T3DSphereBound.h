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

#ifndef __T3D_SPHERE_BOUND_H__
#define __T3D_SPHERE_BOUND_H__


#include "Bound/T3DBound.h"
#include "T3DSphere.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SphereBound : public Bound
    {
    public:
        static SphereBoundPtr create(uint32_t unID, SGNode *node);

        virtual ~SphereBound();

        virtual Type getType() const override;
        virtual BoundPtr clone() const override;

        void setParams(const Vector3 &rkCenter, Real radius);

        const Sphere &getOriginalSphere() const { return mOriginalSphere; }

        virtual SGRenderablePtr getRenderable() override;

    protected:
        SphereBound(uint32_t unID, SGNode *node);

        virtual bool testSphere(const SphereBoundPtr &bound) const override;
        virtual bool testAabb(const AabbBoundPtr &bound) const override;
        virtual bool testObb(const ObbBoundPtr &bound) const override;
        virtual bool testFrustum(const FrustumBoundPtr &bound) const override;

        virtual void updateBound(const Transform &transform) override;

        virtual void cloneProperties(const BoundPtr &bound) const override;

    private:
        Sphere          mOriginalSphere;
        SGSpherePtr     mRenderable;
    };
}


#endif  /*__T3D_SPHERE_BOUND_H__*/
