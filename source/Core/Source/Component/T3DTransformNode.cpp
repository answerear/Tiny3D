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


#include "Component/T3DTransformNode.h"

#include "Component/T3DRenderable.h"
#include "Kernel/T3DGameObject.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    TransformNode::TransformNode(const UUID& uuid /* = UUID::INVALID */)
        : Component(uuid)
    {
    }

    TransformNode::~TransformNode()
    {
        // removeAllChildren();
    }

    //--------------------------------------------------------------------------
    
    TResult TransformNode::addChild(TransformNodePtr node)
    {
        T3D_ASSERT(node->getParent() == nullptr, "parent node is nullptr !");
        
        if (mFirstChild == nullptr)
        {
            // 没有子结点
            mLastChild = mFirstChild = node;
        }
        else
        {
            // 有子结点，直接插入子结点链表末尾
            node->mPrevSibling = mLastChild;
            node->mNextSibling = nullptr;
            mLastChild->mNextSibling = node;
            mLastChild = node;
        }

        node->mParent = this;
        mChildrenCount++;
        node->onAttachParent(this);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult TransformNode::removeChild(TransformNodePtr node)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (node == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Invalid node pointer !");
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            TransformNode *child = mFirstChild;

            while (child != nullptr)
            {
                if (child == node)
                {
                    // 找到要删除的，先断开链表前后关系
                    child->onDetachParent(this);
                    child->mParent = nullptr;
                    mChildrenCount--;

                    if (child->mPrevSibling != nullptr)
                        child->mPrevSibling->mNextSibling = child->mNextSibling;
                    if (child->mNextSibling != nullptr)
                        child->mNextSibling->mPrevSibling = child->mPrevSibling;

                    break;
                }

                child = child->mNextSibling;
            }

            if (mChildrenCount == 0)
            {
                mFirstChild = nullptr;
                mLastChild = nullptr;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult TransformNode::removeChild(const UUID &nodeID)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (nodeID == UUID::INVALID)
            {
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Invalid node ID !");
                ret = T3D_ERR_INVALID_ID;
                break;
            }

            TransformNode *child = mFirstChild;

            while (child != nullptr)
            {
                if (child->getUUID() == nodeID)
                {
                    // 找到要删除的，先断开链表前后关系
                    child->onDetachParent(this);
                    child->mParent = nullptr;
                    mChildrenCount--;

                    if (child->mPrevSibling != nullptr)
                        child->mPrevSibling->mNextSibling = child->mNextSibling;
                    if (child->mNextSibling != nullptr)
                        child->mNextSibling->mPrevSibling = child->mPrevSibling;

                    break;
                }

                child = child->mNextSibling;
            }

            if (mChildrenCount == 0)
            {
                mFirstChild = nullptr;
                mLastChild = nullptr;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult TransformNode::removeAllChildren()
    {
        TResult ret = T3D_OK;

        TransformNode *child = mFirstChild;

        while (child != nullptr)
        {
            child->onDetachParent(this);
            child->mParent = nullptr;
            if (child->mPrevSibling != nullptr)
                child->mPrevSibling->mNextSibling = nullptr;
            child->mPrevSibling = nullptr;
            if (child->mNextSibling != nullptr)
                child->mNextSibling->mPrevSibling = nullptr;
            child = child->mNextSibling;
        }

        mFirstChild = mLastChild = nullptr;
        mChildrenCount = 0;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult TransformNode::removeFromParent()
    {
        TResult ret = T3D_OK;

        if (mParent != nullptr)
        {
            mParent->removeChild(this);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TransformNodePtr TransformNode::getChild(const UUID &nodeID) const
    {
        TransformNode *child = nullptr;
        TransformNode *temp = mFirstChild;

        while (temp != nullptr)
        {
            if (temp->getUUID() == nodeID)
            {
                child = temp;
                break;
            }

            temp = temp->mNextSibling;
        }

        return child;
    }

    //--------------------------------------------------------------------------

    TransformNodePtr TransformNode::getChild(const String &name) const
    {
        TransformNode *child = nullptr;
        TransformNode *temp = mFirstChild;

        while (temp != nullptr)
        {
            if (temp->getGameObject()->getName() == name)
            {
                child = temp;
                break;
            }

            temp = temp->mNextSibling;
        }

        return child;
    }

    //--------------------------------------------------------------------------

    TResult TransformNode::cloneProperties(const Component * const src)
    {
        TResult ret = T3D_OK;

        do 
        {
            const TransformNode * const node = static_cast<const TransformNode* const>(src);
            if (node == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Invalid pointer !");
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            // 克隆子结点属性
            TransformNode *child = mFirstChild;

            while (child != nullptr)
            {
                TransformNodePtr newChild = smart_pointer_cast<TransformNode>(child->clone());
                child->cloneProperties(newChild);
                addChild(newChild);
                child = child->mNextSibling;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void TransformNode::onAttachParent(TransformNodePtr parent)
    {

    }

    //--------------------------------------------------------------------------

    void TransformNode::onDetachParent(TransformNodePtr parent)
    {

    }

    //--------------------------------------------------------------------------

    void TransformNode::onDestroy()
    {
        TransformNode *parent = getParent();
        if (parent != nullptr)
        {
            parent->removeChild(this);
        }
        
        Component::onDestroy();
    }

    //--------------------------------------------------------------------------
}


