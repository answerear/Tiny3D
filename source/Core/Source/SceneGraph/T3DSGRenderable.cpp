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

#include "SceneGraph/T3DSGRenderable.h"
#include "SceneGraph/T3DSGTransformNode.h"


namespace Tiny3D
{
    SGRenderable::SGRenderable(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGNode(unID)
    {

    }

    SGRenderable::~SGRenderable()
    {
    }

    const Matrix4 &SGRenderable::getWorldMatrix() const
    {
        NodePtr node = getParent();
        while (node != nullptr && node->getNodeType() != E_NT_TRANSFORM)
        {
            node = node->getParent();
        }

        if (node != nullptr)
        {
            T3D_ASSERT(node->getNodeType() == E_NT_TRANSFORM);
            SGTransformNodePtr parent = smart_pointer_cast<SGTransformNode>(node);
            return parent->getLocalToWorldTransform().getAffineMatrix();
        }

        return Matrix4::IDENTITY;
    }
}
