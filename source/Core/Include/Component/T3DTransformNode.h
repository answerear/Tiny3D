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
