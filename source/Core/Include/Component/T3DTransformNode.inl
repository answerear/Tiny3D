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


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    
    template <typename VisitAction, typename ...Args>
    void TransformNode::visitActive(VisitAction &&action, Args &&...args)
    {
        if (getGameObject()->isActive())
        {
            // 更新自己
            action(this, std::forward<Args>(args)...);

            // 遍历子结点
            TransformNodePtr node = getFirstChild();

            while (node != nullptr)
            {
                TransformNodePtr child = smart_pointer_cast<TransformNode>(node);
                child->visitActive(action, std::forward<Args>(args)...);
                node = node->getNextSibling();
            }
        }
    }

    //--------------------------------------------------------------------------
    
    template <typename VisitAction, typename  ...Args>
    void TransformNode::visitVisible(VisitAction &&action, Args &&...args)
    {
        if (getGameObject()->isVisible())
        {
            // 更新自己
            action(this, std::forward<Args>(args)...);

            // 遍历子结点
            TransformNodePtr node = getFirstChild();

            while (node != nullptr)
            {
                TransformNodePtr child = smart_pointer_cast<TransformNode>(node);
                child->visitVisible(action, std::forward<Args>(args)...);
                node = node->getNextSibling();
            }
        }
    }

    //--------------------------------------------------------------------------
    
    template <typename VisitAction, typename ...Args>
    void TransformNode::visitAll(VisitAction &&action, Args &&...args)
    {
        // 更新自己
        action(this, std::forward<Args>(args)...);

        // 遍历子结点
        TransformNodePtr node = getFirstChild();

        while (node != nullptr)
        {
            TransformNodePtr child = smart_pointer_cast<TransformNode>(node);
            child->visitAll(action, std::forward<Args>(args)...);
            node = node->getNextSibling();
        }
    }

    //--------------------------------------------------------------------------

    template <typename VisitAction, typename ...Args>
    void TransformNode::reverseVisitActive(VisitAction &&action, Args &&...args)
    {
        if (getGameObject()->isActive())
        {
            // 遍历子结点
            TransformNodePtr node = getLastChild();

            while (node != nullptr)
            {
                TransformNodePtr child = smart_pointer_cast<TransformNode>(node);
                child->visitActive(action, std::forward<Args>(args)...);
                node = node->getPrevSibling();
            }

            // 更新自己
            action(this, std::forward<Args>(args)...);
        }
    }

    template <typename VisitAction, typename ...Args>
    void TransformNode::reverseVisitVisible(VisitAction &&action, Args &&...args)
    {
        if (getGameObject()->isVisible())
        {
            // 遍历子结点
            TransformNodePtr node = getLastChild();

            while (node != nullptr)
            {
                TransformNodePtr child = smart_pointer_cast<TransformNode>(node);
                child->visitVisible(action, std::forward<Args>(args)...);
                node = node->getPrevSibling();
            }

            // 更新自己
            action(this, std::forward<Args>(args)...);
        }
    }

    template <typename VisitAction, typename ...Args>
    void TransformNode::reverseVisitAll(VisitAction &&action, Args &&...args)
    {
        // 遍历子结点
        TransformNodePtr node = getLastChild();

        while (node != nullptr)
        {
            TransformNodePtr child = smart_pointer_cast<TransformNode>(node);
            child->visitAll(action, std::forward<Args>(args)...);
            node = node->getPrevSibling();
        }

        // 更新自己
        action(this, std::forward<Args>(args)...);
    }
    
    //--------------------------------------------------------------------------
    
    inline TransformNodePtr TransformNode::getFirstChild() const
    {
        return mFirstChild;
    }

    //--------------------------------------------------------------------------
    
    inline TransformNodePtr TransformNode::getLastChild() const
    {
        return mLastChild;
    }

    //--------------------------------------------------------------------------
    
    inline TransformNodePtr TransformNode::getPrevSibling() const
    {
        return mPrevSibling;
    }

    //--------------------------------------------------------------------------
    
    inline TransformNodePtr TransformNode::getNextSibling() const
    {
        return mNextSibling;
    }

    //--------------------------------------------------------------------------
    
    inline size_t TransformNode::getChildrenCount() const
    {
        return mChildrenCount;
    }

    //--------------------------------------------------------------------------
    
    inline TransformNodePtr TransformNode::getParent() const
    {
        return mParent;
    }

    //--------------------------------------------------------------------------
}
