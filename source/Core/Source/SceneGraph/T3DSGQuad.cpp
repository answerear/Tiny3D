/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
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

#include "SceneGraph/T3DSGQuad.h"
#include "T3DTypedef.h"
#include "Render/T3DRenderQueue.h"


namespace Tiny3D
{
    SGQuadPtr SGQuad::create(uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGQuadPtr quad = new SGQuad(uID);
        if (quad != nullptr)
        {
            quad->release();
        }
        return quad;
    }

    SGQuad::SGQuad(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
    {

    }

    SGQuad::~SGQuad()
    {

    }

    NodePtr SGQuad::clone() const
    {
        SGQuadPtr quad = SGQuad::create();
        if (quad != nullptr)
        {
            cloneProperties(quad);
        }

        return quad;
    }

    void SGQuad::cloneProperties(const NodePtr &node) const
    {
        SGRenderable::cloneProperties(node);
    }

    void SGQuad::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        if (isVisible())
        {
            queue->addRenderable(RenderQueue::E_GRPID_SOLID, this);
        }
    }

    Node::Type SGQuad::getNodeType() const
    {
        return E_NT_QUAD;
    }

    MaterialPtr SGQuad::getMaterial() const
    {
        return nullptr;
    }

    Renderer::PrimitiveType SGQuad::getPrimitiveType() const
    {
        return Renderer::E_PT_TRIANGLE_LIST;
    }

    VertexDataPtr SGQuad::getVertexData() const
    {
        return mVertexData;
    }

    IndexDataPtr SGQuad::getIndexData() const
    {
        return nullptr;
    }

    bool SGQuad::isIndicesUsed() const
    {
        return false;
    }
}
