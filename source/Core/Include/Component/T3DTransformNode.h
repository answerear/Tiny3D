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


#ifndef __T3D_TRANSFORM_NODE_H__
#define __T3D_TRANSFORM_NODE_H__


#include "Component/T3DComponent.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API TransformNode : public Component
    {
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND
        
    public:
        ~TransformNode() override;

        template <typename VisitAction, typename ...Args>
        void visitActive(VisitAction &&action, Args &&...args);
        
        template <typename VisitAction, typename  ...Args>
        void visitVisible(VisitAction &&action, Args &&...args);

        template <typename VisitAction, typename ...Args>
        void visitAll(VisitAction &&action, Args &&...args);

        template <typename VisitAction, typename ...Args>
        void reverseVisitActive(VisitAction &&action, Args &&...args);

        template <typename VisitAction, typename ...Args>
        void reverseVisitVisible(VisitAction &&action, Args &&...args);

        template <typename VisitAction, typename ...Args>
        void reverseVisitAll(VisitAction &&action, Args &&...args);

        virtual TResult addChild(TransformNodePtr node);

        virtual TResult removeChild(TransformNodePtr node);

        virtual TResult removeChild(const UUID &nodeID);

        virtual TResult removeAllChildren();

        TResult removeFromParent();

        TransformNodePtr getFirstChild() const;

        TransformNodePtr getLastChild() const;

        TransformNodePtr getChild(const UUID &nodeID) const;

        TransformNodePtr getChild(const String &name) const;

        TransformNodePtr getPrevSibling() const;

        TransformNodePtr getNextSibling() const;

        size_t getChildrenCount() const;

        TransformNodePtr getParent() const;

    protected:
        TransformNode(const UUID &uuid = UUID::INVALID);

        TResult cloneProperties(const Component * const src) override;

        virtual void onAttachParent(TransformNodePtr parent);

        virtual void onDetachParent(TransformNodePtr parent);

        void onDestroy() override;

    private:
        /// 子结点数量
        uint32_t            mChildrenCount {0};
        /// 父结点
        TransformNodePtr    mParent {nullptr};
        /// 第一个子结点
        TransformNodePtr    mFirstChild {nullptr};
        /// 最后一个子结点
        TransformNodePtr    mLastChild {nullptr};
        /// 前一个兄弟结点
        TransformNodePtr    mPrevSibling {nullptr};
        /// 后一个兄弟结点
        TransformNodePtr    mNextSibling {nullptr};
    };
}


#include "T3DTransformNode.inl"


#endif  /*__T3D_TRANSFORM_NODE_H__*/
