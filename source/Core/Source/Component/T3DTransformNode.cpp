/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "Component/T3DTransformNode.h"
#include "Kernel/T3DGameObject.h"
#include "Render/T3DRenderPipeline.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    TransformNode::TransformNode(const UUID& uuid)
        : Component(uuid)
    {
    }

    TransformNode::~TransformNode()
    {
        // removeAllChildren();
    }

    //--------------------------------------------------------------------------
    
    TResult TransformNode::addChild(TransformNode *node)
    {
        T3D_ASSERT(node->getParent() == nullptr);
        
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
        mChildrenUUID.emplace_back(node->getUUID());
        T3D_ASSERT(mChildrenCount == mChildrenUUID.size());
        node->onAttachParent(this);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult TransformNode::removeChild(TransformNode *node)
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

            ret = removeChild(node->getUUID());
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

            auto it = std::find(mChildrenUUID.begin(), mChildrenUUID.end(), nodeID);
            if (it != mChildrenUUID.end())
            {
                mChildrenUUID.erase(it);
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

            T3D_ASSERT(mChildrenCount == mChildrenUUID.size());
            
            if (mChildrenCount == 0)
            {
                mFirstChild = nullptr;
                mLastChild = nullptr;
            }
        } while (false);

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

        mChildrenUUID.clear();

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

    TransformNode *TransformNode::getChild(const UUID &nodeID) const
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

    TransformNode *TransformNode::getChild(const String &name) const
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

    void TransformNode::onAttachParent(TransformNode *parent)
    {

    }

    //--------------------------------------------------------------------------

    void TransformNode::onDetachParent(TransformNode *parent)
    {

    }

    //--------------------------------------------------------------------------

    void TransformNode::onDestroy()
    {
        TransformNodePtr parent = getParent();
        if (parent != nullptr)
        {
            parent->removeChild(this);
        }
        
        Component::onDestroy();
    }

    //--------------------------------------------------------------------------

    void TransformNode::setupHierarchy()
    {
        size_t i = 0;
        size_t count = mChildrenUUID.size();
        TransformNode *prev = nullptr, *next = nullptr;
        
        for (auto itr = mChildrenUUID.begin(); itr != mChildrenUUID.end(); ++itr, ++i)
        {
            const UUID &uuid = *itr;
            T3D_ASSERT(uuid != UUID::INVALID);
            
            if (i == 0)
            {
                // 第一个
                const auto it = msComponents.find(uuid);
                if (it != msComponents.end())
                {
                    mFirstChild = it->second;
                    mFirstChild->mParent = this;
                    mFirstChild->mPrevSibling = nullptr;
                    mFirstChild->mNextSibling = nullptr;
                    prev = mFirstChild;
                }
            }
            else if (i == count - 1)
            {
                // 最后一个
                const auto it = msComponents.find(uuid);
                if (it != msComponents.end())
                {
                    mLastChild = it->second;
                    mLastChild->mParent = this;
                    mLastChild->mPrevSibling = prev;
                    mLastChild->mNextSibling = nullptr;
                }
            }
            else
            {
                const auto it = msComponents.find(uuid);
                if (it != msComponents.end())
                {
                    TransformNode *node = static_cast<TransformNode*>(it->second.get());
                    T3D_ASSERT(node->mPrevSibling == nullptr);
                    node->mPrevSibling = prev;
                    T3D_ASSERT(node->mNextSibling == nullptr);
                    prev->mNextSibling = node;
                }
            }
        }
    }

    //--------------------------------------------------------------------------
}


