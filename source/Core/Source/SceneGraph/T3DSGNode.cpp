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

#include "SceneGraph/T3DSGNode.h"
#include "Render/T3DRenderQueue.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    SGNode::SGNode(uint32_t unID /* = E_NID_AUTOMATIC */)
        : Node(unID)
        , mUserData(0)
        , mUserObject(nullptr)
        , mIsDirty(true)
    {

    }

    SGNode::~SGNode()
    {

    }

    void SGNode::updateTransform()
    {
        auto itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            const SGNodePtr &node = smart_pointer_cast<SGNode>(*itr);
            node->updateTransform();
            ++itr;
        }
    }

    void SGNode::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        auto itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            const SGNodePtr &node = smart_pointer_cast<SGNode>(*itr);
            node->frustumCulling(bound, queue);
            ++itr;
        }
    }

    void SGNode::setDirty(bool isDirty, bool recursive /* = false */)
    {
        mIsDirty = isDirty;

        if (recursive)
        {
            auto itr = mChildren.begin();

            while (itr != mChildren.end())
            {
                const SGNodePtr &node = smart_pointer_cast<SGNode>(*itr);
                node->setDirty(isDirty, recursive);
                ++itr;
            }
        }
    }

    void SGNode::cloneProperties(const NodePtr &node) const
    {
        Node::cloneProperties(node);

        const SGNodePtr &newNode = smart_pointer_cast<SGNode>(node);
        newNode->mUserData = mUserData;
        newNode->mUserObject = mUserObject;
    }

    void SGNode::setVisible(bool visible)
    {
        mIsVisible = visible;
    }
}
