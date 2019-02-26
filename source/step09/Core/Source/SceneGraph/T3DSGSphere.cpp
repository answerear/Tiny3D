/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#include "SceneGraph/T3DSGSphere.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    struct SphereVertex
    {
        SphereVertex()
            : position(Vector3::ZERO)
            , color(Color4::WHITE.A8R8G8B8())
        {

        }

        Vector3     position;
        uint32_t    color;
    };

    //--------------------------------------------------------------------------

    SGSpherePtr SGSphere::create(const Vector3 &center, Real radius, 
        ID uID /* = E_NID_AUTOMATIC */)
    {
        SGSpherePtr sphere = new SGSphere(uID);
        sphere->release();

        if (sphere->init(center, radius) != T3D_OK)
        {
            sphere = nullptr;
        }

        return sphere;
    }

    //--------------------------------------------------------------------------

    SGSphere::SGSphere(ID uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
        , mCenter(Vector3::ZERO)
        , mRadius(REAL_ONE)
    {

    }

    //--------------------------------------------------------------------------

    SGSphere::~SGSphere()
    {

    }

    //--------------------------------------------------------------------------

    Node::Type SGSphere::getNodeType() const
    {
        return E_NT_SPHERE;
    }

    //--------------------------------------------------------------------------

    TResult SGSphere::init(const Vector3 &center, Real radius)
    {
        TResult ret = T3D_OK;

        mCenter = center;
        mRadius = radius;

        return ret;
    }

    //--------------------------------------------------------------------------

    MaterialPtr SGSphere::getMaterial() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VertexArrayObjectPtr SGSphere::getVertexArrayObject() const
    {
        return nullptr;
    }
}
