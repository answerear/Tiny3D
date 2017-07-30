/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
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

#ifndef __MCONV_BOUND_H__
#define __MCONV_BOUND_H__


#include "mconv_node.h"


namespace mconv
{
    class SphereBound : public Node
    {
    public:
        SphereBound(const String &ID)
            : Node(ID)
            , mCenterX(0.0f)
            , mCenterY(0.0f)
            , mCenterZ(0.0f)
            , mRadius(1.0f)
        {

        }

        virtual ~SphereBound()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_BOUND_SPHERE;
        }

        float   mCenterX, mCenterY, mCenterZ;
        float   mRadius;
    };

    class AabbBound : public Node
    {
    public:
        AabbBound(const String &ID)
            : Node(ID)
            , mMinX(-1.0f)
            , mMaxX(1.0f)
            , mMinY(-1.0f)
            , mMaxY(1.0f)
            , mMinZ(-1.0f)
            , mMaxZ(1.0f)
        {

        }

        virtual ~AabbBound()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_BOUND_AABB;
        }

        float   mMinX;
        float   mMaxX;
        float   mMinY;
        float   mMaxY;
        float   mMinZ;
        float   mMaxZ;
    };
}

#endif
