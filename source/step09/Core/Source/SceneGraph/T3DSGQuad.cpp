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


#include "SceneGraph/T3DSGQuad.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SGQuadPtr SGQuad::create(const Quad &quad, const String &materialName,
        ID uID /* = E_NID_AUTOMATIC */)
    {
        SGQuadPtr q = new SGQuad(uID);
        q->release();
        return q;
    }

    //--------------------------------------------------------------------------

    SGQuad::SGQuad(ID uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
        , mMaterial(nullptr)
        , mVAO(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    SGQuad::~SGQuad()
    {

    }

    //--------------------------------------------------------------------------

    Node::Type SGQuad::getNodeType() const
    {
        return E_NT_QUAD;
    }

    //--------------------------------------------------------------------------

    TResult SGQuad::init(const Quad &quad, const String &materialName)
    {
        TResult ret = T3D_OK;
        return ret;
    }

    //--------------------------------------------------------------------------

    NodePtr SGQuad::clone() const
    {
        SGQuadPtr quad = new SGQuad();
        quad->release();

        if (cloneProperties(quad) != T3D_OK)
        {
            quad = nullptr;
        }

        return quad;
    }

    //--------------------------------------------------------------------------

    TResult SGQuad::cloneProperties(NodePtr node) const
    {
        TResult ret = SGRenderable::cloneProperties(node);

        if (ret == T3D_OK)
        {

        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void SGQuad::updateTransform()
    {

        SGRenderable::updateTransform();
    }

    //--------------------------------------------------------------------------

    void SGQuad::frustumCulling(BoundPtr bound, RenderQueuePtr queue)
    {
        
    }

    //--------------------------------------------------------------------------

    MaterialPtr SGQuad::getMaterial() const
    {
        return mMaterial;
    }

    //--------------------------------------------------------------------------

    VertexArrayObjectPtr SGQuad::getVertexArrayObject() const
    {
        return mVAO;
    }
}
