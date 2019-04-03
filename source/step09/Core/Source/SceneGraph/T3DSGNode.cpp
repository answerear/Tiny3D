/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#include "SceneGraph/T3DSGNode.h"
#include "SceneGraph/T3DSGCamera.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SGNode::SGNode(ID uID /* = E_NID_AUTOMATIC */)
        : Node(uID)
        , mCameraMask(0)
        , mUserData(nullptr)
        , mUserObject(nullptr)
        , mIsVisible(true)
        , mIsEnabled(true)
    {

    }

    //--------------------------------------------------------------------------

    SGNode::~SGNode()
    {

    }

    //--------------------------------------------------------------------------

    void SGNode::updateTransform()
    {
        
    }

    //--------------------------------------------------------------------------

    void SGNode::frustumCulling(BoundPtr bound, RenderQueuePtr queue)
    {
        
    }

    //--------------------------------------------------------------------------

    TResult SGNode::cloneProperties(NodePtr node) const
    {
        TResult ret = Node::cloneProperties(node);

        if (ret == T3D_OK)
        {
            SGNodePtr newNode = smart_pointer_cast<SGNode>(node);
            newNode->mUserData = mUserData;
            newNode->mUserObject = mUserObject;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void SGNode::setVisible(bool visible)
    {
        mIsVisible = visible;
    }

    //--------------------------------------------------------------------------

    void SGNode::setEnabled(bool enabled)
    {
        mIsEnabled = enabled;
    }

    //--------------------------------------------------------------------------

    void SGNode::visit()
    {
        // 先调用更新
        if (isEnabled())
        {
            updateTransform();

            // 再遍历子结点
            NodePtr node = getFirstChild();

            while (node != nullptr)
            {
                SGNodePtr child = smart_pointer_cast<SGNode>(node);
                child->visit();
                node = node->getNextSibling();
            }
        }
    }

    //--------------------------------------------------------------------------

    void SGNode::setCameraMask(uint32_t mask)
    {
        if (mask != mCameraMask)
            mCameraMask = mask;

        NodePtr node = getFirstChild();

        while (node != nullptr)
        {
            SGNodePtr child = smart_pointer_cast<SGNode>(node);
            child->setCameraMask(mask);
            node = node->getNextSibling();
        }
    }
}
