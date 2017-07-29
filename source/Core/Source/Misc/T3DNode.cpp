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

#include "Misc/T3DNode.h"


namespace Tiny3D
{
    Node::Node(uint32_t uID /* = E_NID_AUTOMATIC */)
        : mID(E_NID_INVALID)
        , mName("")
        , mParent(nullptr)
    {
        if (E_NID_AUTOMATIC == uID)
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

    void Node::addChild(const NodePtr &node)
    {
        T3D_ASSERT(node->getParent() == nullptr);
        mChildren.push_back(node);
        node->mParent = this;
        node->onAttachParent(this);
    }

    void Node::removeChild(const NodePtr &node, bool cleanup)
    {
        if (node != nullptr)
        {
            auto itr = mChildren.begin();

            while (itr != mChildren.end())
            {
                NodePtr &child = *itr;

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
        }
    }

    void Node::removeChild(uint32_t nodeID, bool cleanup)
    {
        auto itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            NodePtr &child = *itr;

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

            ++itr;
        }
    }

    void Node::removeAllChildren(bool cleanup)
    {
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
    }

    void Node::removeFromParent(bool cleanup)
    {
        if (mParent != nullptr)
        {
            mParent->removeChild(this, cleanup);
        }
    }

    const NodePtr &Node::getChild(uint32_t nodeID) const
    {
        auto itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            const NodePtr &node = *itr;
            if (node->getNodeID() == nodeID)
            {
                return node;
                break;
            }
            ++itr;
        }

        return NodePtr::NULL_PTR;
    }

    NodePtr Node::getChild(uint32_t nodeID)
    {
        NodePtr child;
        auto itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            NodePtr &node = *itr;
            if (node->getNodeID() == nodeID)
            {
                child = node;
                break;
            }
            ++itr;
        }

        return child;
    }

    const NodePtr &Node::getChild(const String &name) const
    {
        auto itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            const NodePtr &node = *itr;
            if (node->getName() == name)
            {
                return node;
                break;
            }
            ++itr;
        }

        return NodePtr::NULL_PTR;
    }

    NodePtr Node::getChild(const String &name)
    {
        NodePtr child;
        auto itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            NodePtr &node = *itr;
            if (node->getName() == name)
            {
                child = node;
                break;
            }
            ++itr;
        }

        return child;
    }

    void Node::cloneProperties(const NodePtr &node) const
    {
        node->mName = mName;

        auto itr = node->mChildren.begin();
        while (itr != node->mChildren.end())
        {
            NodePtr &child = *itr;
            NodePtr newChild = child->clone();
            newChild->cloneProperties(child);
            node->addChild(newChild);
            ++itr;
        }
    }

    void Node::onAttachParent(const NodePtr &parent)
    {

    }

    void Node::onDetachParent(const NodePtr &parent)
    {

    }

    uint32_t Node::makeGlobalID() const
    {
        static uint32_t unID = 0;
        return ++unID;
    }
}
