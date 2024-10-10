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
    
    inline TransformNode *TransformNode::getFirstChild() const
    {
        return mFirstChild;
    }

    //--------------------------------------------------------------------------
    
    inline TransformNode *TransformNode::getLastChild() const
    {
        return mLastChild;
    }

    //--------------------------------------------------------------------------
    
    inline TransformNode *TransformNode::getPrevSibling() const
    {
        return mPrevSibling;
    }

    //--------------------------------------------------------------------------
    
    inline TransformNode *TransformNode::getNextSibling() const
    {
        return mNextSibling;
    }

    //--------------------------------------------------------------------------
    
    inline size_t TransformNode::getChildrenCount() const
    {
        return mChildrenCount;
    }

    //--------------------------------------------------------------------------
    
    inline TransformNode *TransformNode::getParent() const
    {
        return mParent;
    }

    //--------------------------------------------------------------------------
}
