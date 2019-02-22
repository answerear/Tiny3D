﻿/*******************************************************************************
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


#include "Kernel/T3DNode.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Node::Node(ID uID /* = E_NID_AUTOMATIC */)
        : mID(E_NID_INVALID)
        , mName()
        , mParent(nullptr)
    {
        if (E_NID_AUTOMATIC == mID)
        {
            mID = makeGlobalID();
        }
        else
        {
            mID = uID;
        }
    }

    Node::~Node()
    {
        removeAllChildren(true);
    }

    //--------------------------------------------------------------------------

    TResult Node::addChild(NodePtr node)
    {
        T3D_ASSERT(node->getParent() == nullptr);
        mChildren.push_back(node);
        node->mParent = this;
        node->onAttachParent(this);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Node::removeChild(NodePtr node, bool cleanup)
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

            auto itr = mChildren.begin();

            while (itr != mChildren.end())
            {
                NodePtr child = *itr;

                if (child == node)
                {
                    if (cleanup)
                    {
                        child->removeAllChildren(cleanup);
                    }

                    child->onDetachParent(this);
                    child->mParent = nullptr;
                    mChildren.erase(itr);
                    break;
                }

                ++itr;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Node::removeChild(uint32_t nodeID, bool cleanup)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (nodeID == E_NID_INVALID)
            {
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Invalid node ID !");
                ret = T3D_ERR_INVALID_ID;
                break;
            }

            auto itr = mChildren.begin();

            while (itr != mChildren.end())
            {
                NodePtr child = *itr;

                if (child != nullptr && child->getNodeID() == nodeID)
                {
                    if (cleanup)
                    {
                        child->removeAllChildren(cleanup);
                    }

                    child->onDetachParent(this);
                    child->mParent = nullptr;
                    mChildren.erase(itr);
                    break;
                }
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Node::removeAllChildren(bool cleanup)
    {
        TResult ret = T3D_OK;

        auto itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            NodePtr &child = *itr;

            if (cleanup)
            {
                child->removeAllChildren(cleanup);
            }

            child->onDetachParent(this);
            child->mParent = nullptr;

            ++itr;
        }

        mChildren.clear();

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Node::removeFromParent(bool cleanup)
    {
        TResult ret = T3D_OK;

        if (mParent != nullptr)
        {
            mParent->removeChild(this, cleanup);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    NodePtr Node::getChild(ID nodeID) const
    {
        NodePtr child = nullptr;

        auto itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            NodePtr node = *itr;
            if (node->getNodeID() == nodeID)
            {
                child = node;
                break;
            }

            ++itr;
        }

        return child;
    }

    //--------------------------------------------------------------------------

    NodePtr Node::getChild(const String &name) const
    {
        NodePtr child = nullptr;

        auto itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            NodePtr node = *itr;
            if (node->getName() == name)
            {
                child = node;
                break;
            }

            ++itr;
        }

        return child;
    }

    //--------------------------------------------------------------------------

    TResult Node::cloneProperties(NodePtr node) const
    {
        TResult ret = T3D_OK;

        do 
        {
            if (node == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Invalid pointer !");
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            // 克隆结点名称
            node->mName = mName;

            // 克隆子结点属性
            auto itr = node->mChildren.begin();

            while (itr != node->mChildren.end())
            {
                NodePtr &child = *itr;
                NodePtr newChild = child->clone();
                newChild->cloneProperties(child);
                node->addChild(newChild);
                ++itr;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    void Node::onAttachParent(NodePtr parent)
    {

    }

    //--------------------------------------------------------------------------

    void Node::onDetachParent(NodePtr parent)
    {

    }

    //--------------------------------------------------------------------------

    ID Node::makeGlobalID() const
    {
        static ID uID = 0;
        return ++uID;
    }
}

