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


#include "Scene/T3DSceneNode.h"
#include "Scene/T3DSceneCamera.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SceneNode::SceneNode(ID uID /* = E_NID_AUTOMATIC */)
        : Node(uID)
        , mCameraMask(0)
        , mIsVisible(true)
        , mIsEnabled(true)
    {

    }

    //--------------------------------------------------------------------------

    SceneNode::~SceneNode()
    {

    }

    //--------------------------------------------------------------------------

    void SceneNode::updateTransform()
    {
        
    }

    //--------------------------------------------------------------------------

    void SceneNode::frustumCulling(BoundPtr bound, RenderQueuePtr queue)
    {
        
    }

    //--------------------------------------------------------------------------

    TResult SceneNode::cloneProperties(NodePtr node) const
    {
        TResult ret = Node::cloneProperties(node);

        if (ret == T3D_OK)
        {
            SceneNodePtr newNode = smart_pointer_cast<SceneNode>(node);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void SceneNode::setVisible(bool visible)
    {
        mIsVisible = visible;
    }

    //--------------------------------------------------------------------------

    void SceneNode::setEnabled(bool enabled)
    {
        mIsEnabled = enabled;
    }

    //--------------------------------------------------------------------------

    void SceneNode::visit()
    {
        // 先调用更新
        if (isEnabled())
        {
            updateTransform();

            // 再遍历子结点
            NodePtr node = getFirstChild();

            while (node != nullptr)
            {
                SceneNodePtr child = smart_pointer_cast<SceneNode>(node);
                child->visit();
                node = node->getNextSibling();
            }
        }
    }

    //--------------------------------------------------------------------------

    void SceneNode::setCameraMask(uint32_t mask)
    {
        if (mask != mCameraMask)
            mCameraMask = mask;

        NodePtr node = getFirstChild();

        while (node != nullptr)
        {
            SceneNodePtr child = smart_pointer_cast<SceneNode>(node);
            child->setCameraMask(mask);
            node = node->getNextSibling();
        }
    }
}
