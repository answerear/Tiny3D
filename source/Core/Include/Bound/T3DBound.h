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

#ifndef __T3D_BOUND_H__
#define __T3D_BOUND_H__


#include "Misc/T3DObject.h"
#include "T3DSphere.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Bound : public Object
    {
    public:
        enum Type
        {
            E_BT_NONE = 0,
            E_BT_SPHERE,
            E_BT_AABB,
            E_BT_OBB,
            E_BT_FRUSTUM,
        };

        virtual ~Bound();

        virtual Type getType() const = 0;

        uint32_t getID() const;

        void setGroupID(uint32_t groupID);
        uint32_t getGroupID() const;

        const Sphere &getSphere() const;

        virtual bool test(const BoundPtr &bound) const;

        SGNodePtr getNode() const;
        SGNodePtr getNode();

        void setCollisionSource(bool isSource);
        bool isCollisionSource() const;

        void setEnable(bool enable);
        bool isEnable() const;

        virtual SGRenderablePtr getRenderable() = 0;

        virtual void setTransform(const Transform &transform);

        virtual BoundPtr clone() const = 0;

    protected:
        Bound(uint32_t unID, SGNode *node);

        virtual bool testSphere(const SphereBoundPtr &bound) const = 0;
        virtual bool testAabb(const AabbBoundPtr &bound) const = 0;
        virtual bool testObb(const ObbBoundPtr &bound) const = 0;
        virtual bool testFrustum(const FrustumBoundPtr &bound) const = 0;

        virtual void updateBound(const Transform &transform) = 0;

        virtual void cloneProperties(const BoundPtr &bound) const;

    protected:
        Sphere  mSphere;

    private:
        SGNode      *mNode;

        uint32_t    mID;
        uint32_t    mGroupID;
        bool        mMovable;
        bool        mCollisionSource;
        bool        mEnable;
    };
}


#include "T3DBound.inl"


#endif  /*__T3D_BOUND_H__*/
