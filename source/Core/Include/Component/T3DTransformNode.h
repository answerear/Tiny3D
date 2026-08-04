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


#define USE_GENARAL_MULTI_TREE


#if !defined (USE_GENARAL_MULTI_TREE)
namespace Tiny3D
{
    /**
     * \brief 场景树变换节点（链表式子节点实现，当前未启用）
     */
    TCLASS()
    class T3D_ENGINE_API TransformNode : public Component
    {
        friend class GameObject;
        
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND
        
    public:
        /// 析构
        ~TransformNode() override;

        /**
         * \brief DFS 遍历 active 子树并调用访问器
         * \tparam VisitAction : 访问回调类型
         * \tparam Args : 传递给回调的额外参数类型
         * \param [in] action : 访问回调
         * \param [in] args : 传递给回调的参数
         */
        template <typename VisitAction, typename ...Args>
        void visitActive(VisitAction &&action, Args &&...args);

        /**
         * \brief DFS 遍历整棵子树并调用访问器
         * \tparam VisitAction : 访问回调类型
         * \tparam Args : 传递给回调的额外参数类型
         * \param [in] action : 访问回调
         * \param [in] args : 传递给回调的参数
         */
        template <typename VisitAction, typename ...Args>
        void visitAll(VisitAction &&action, Args &&...args);

        /**
         * \brief 逆序 DFS 遍历 active 子树
         * \tparam VisitAction : 访问回调类型
         * \tparam Args : 传递给回调的额外参数类型
         * \param [in] action : 访问回调
         * \param [in] args : 传递给回调的参数
         */
        template <typename VisitAction, typename ...Args>
        void reverseVisitActive(VisitAction &&action, Args &&...args);

        /**
         * \brief 逆序 DFS 遍历整棵子树
         * \tparam VisitAction : 访问回调类型
         * \tparam Args : 传递给回调的额外参数类型
         * \param [in] action : 访问回调
         * \param [in] args : 传递给回调的参数
         */
        template <typename VisitAction, typename ...Args>
        void reverseVisitAll(VisitAction &&action, Args &&...args);

        /**
         * \brief 添加子节点到链表末尾
         * \param [in] node : 待添加节点，须无父节点
         * \return 成功返回 T3D_OK
         */
        virtual TResult addChild(TransformNode *node);

        /**
         * \brief 按指针移除子节点
         * \param [in] node : 待移除子节点，可为 nullptr
         * \return 成功返回 T3D_OK；node 为空返回 T3D_ERR_INVALID_POINTER
         */
        virtual TResult removeChild(TransformNode *node);

        /**
         * \brief 按 UUID 移除子节点
         * \param [in] nodeID : 子节点 UUID，不可为 INVALID
         * \return 成功返回 T3D_OK；无效 ID 返回 T3D_ERR_INVALID_ID
         */
        virtual TResult removeChild(const UUID &nodeID);

        /**
         * \brief 移除所有子节点并断开链表
         * \return 成功返回 T3D_OK
         */
        virtual TResult removeAllChildren();

        /**
         * \brief 从父节点移除自身
         * \return 成功返回 T3D_OK
         */
        TResult removeFromParent();

        /// 返回第一个子节点
        TransformNode *getFirstChild() const;

        /// 返回最后一个子节点
        TransformNode  *getLastChild() const;

        /**
         * \brief 按 UUID 查找直接子节点
         * \param [in] nodeID : 子节点 UUID
         * \return 找到返回子节点指针，否则 nullptr
         */
        TransformNode *getChild(const UUID &nodeID) const;

        /**
         * \brief 按 GameObject 名称查找直接子节点
         * \param [in] name : GameObject 名称
         * \return 找到返回子节点指针，否则 nullptr
         */
        TransformNode *getChild(const String &name) const;

        /// 返回前一个兄弟节点
        TransformNode *getPrevSibling() const;

        /// 返回后一个兄弟节点
        TransformNode *getNextSibling() const;

        /// 返回直接子节点数量
        size_t getChildrenCount() const;

        /// 返回父节点
        TransformNode *getParent() const;

    protected:
        /// 默认构造
        TransformNode() = default;
        
        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 组件唯一标识
         */
        TransformNode(const UUID &uuid);

        /**
         * \brief 克隆子树并挂接到当前节点
         * \param [in] src : 源组件
         * \return 成功返回 T3D_OK；src 为空返回 T3D_ERR_INVALID_POINTER
         */
        TResult cloneProperties(const Component * const src) override;

        /**
         * \brief 挂接父节点回调（默认空实现）
         * \param [in] parent : 新的父节点
         */
        virtual void onAttachParent(TransformNode *parent);

        /**
         * \brief 脱离父节点回调（默认空实现）
         * \param [in] parent : 原父节点
         */
        virtual void onDetachParent(TransformNode *parent);

        /**
         * \brief 销毁时从父节点移除自身
         */
        void onDestroy() override;

    private:
        using ChildrenUUID = TList<UUID>;

        /// 返回序列化的子节点 UUID 列表
        TPROPERTY(RTTRFuncName="Children", RTTRFuncType="getter")
        const ChildrenUUID &getChildrenUUID() const { return mChildrenUUID; }

        /**
         * \brief 设置序列化的子节点 UUID 列表
         * \param [in] childrenUUID : 子节点 UUID 列表
         */
        TPROPERTY(RTTRFuncName="Children", RTTRFuncType="setter")
        void setChildrenUUID(const ChildrenUUID &childrenUUID) { mChildrenUUID = childrenUUID; }

        /**
         * \brief 反序列化后按 mChildrenUUID 重建兄弟链表
         */
        void setupHierarchy();
        
    private:
        /// 子节点数量
        uint32_t            mChildrenCount {0};
        /// 父节点
        TransformNodePtr    mParent {nullptr};
        /// 第一个子节点
        TransformNodePtr    mFirstChild {nullptr};
        /// 最后一个子节点
        TransformNodePtr    mLastChild {nullptr};
        /// 前一个兄弟节点
        TransformNodePtr    mPrevSibling {nullptr};
        /// 后一个兄弟节点
        TransformNodePtr    mNextSibling {nullptr};

        /// 序列化的子节点 UUID 列表
        ChildrenUUID        mChildrenUUID {};
    };

}


#include "T3DTransformNode.inl"

#else

namespace Tiny3D
{
    /**
     * \brief 场景树变换节点基类，基于 TreeNode 管理父子层次与 DFS 遍历
     * \remarks onDestroy 时从父节点移除并递归 removeAllChildren；子树克隆由 GameObject::clone() 负责。
     */
    TCLASS()
    class T3D_ENGINE_API TransformNode : public Component, public TreeNode<UUID, TransformNode, TransformNodePtr, UUIDHash, UUIDEqual>
    {
        friend class GameObject;
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND
        
    public:
        /// 析构
        ~TransformNode() override;

        /**
         * \brief DFS 遍历 active 子树（过滤 GameObject 非 active 的节点）
         * \tparam VisitAction : 访问回调，签名为 void(int32_t depth, ...)
         * \tparam Args : 传递给回调的参数类型
         * \param [in] action : 访问回调
         * \param [in,out] args : 传递给回调的参数
         */
        template <typename VisitAction, typename ...Args>
        void visitActive(const VisitAction &action, Args &...args)
        {
            int32_t depth = 0;
            dfs_visit(depth, true,
                [](TransformNode *node) { return node->getGameObject()->isActive(); },
                action, args...);
        }
        
        /**
         * \brief DFS 遍历整棵子树
         * \tparam VisitAction : 访问回调
         * \tparam Args : 传递给回调的参数类型
         * \param [in] action : 访问回调
         * \param [in,out] args : 传递给回调的参数
         */
        template <typename VisitAction, typename ...Args>
        void visitAll(const VisitAction &action, Args &...args)
        {
            int32_t depth = 0;
            dfs_visit(depth, true, nullptr, action, args...);
        }

        /**
         * \brief 逆序 DFS 遍历 active 子树
         * \tparam VisitAction : 访问回调
         * \tparam Args : 传递给回调的参数类型
         * \param [in] action : 访问回调
         * \param [in,out] args : 传递给回调的参数
         */
        template <typename VisitAction, typename ...Args>
        void reverseVisitActive(const VisitAction &action, Args &...args)
        {
            int32_t depth = 0;
            dfs_visit(depth, false,
                [](TransformNode *node) { return node->getGameObject()->isActive(); },
                action, args...);
        }

        /**
         * \brief 逆序 DFS 遍历整棵子树
         * \tparam VisitAction : 访问回调
         * \tparam Args : 传递给回调的参数类型
         * \param [in] action : 访问回调
         * \param [in,out] args : 传递给回调的参数
         */
        template <typename VisitAction, typename ...Args>
        void reverseVisitAll(const VisitAction &action, Args &...args)
        {
            int32_t depth = 0;
            dfs_visit(depth, false, nullptr, action, args...);
        }

        /**
         * \brief 返回树节点键（即组件 UUID）
         * \return 组件 UUID 引用
         */
        const UUID &getKey() const override { return getUUID(); }
        
        /**
         * \brief 添加子节点并更新 mChildrenUUID
         * \param [in] node : 待添加子节点
         * \return 添加成功返回 true
         */
        bool addChild(TransformNode *node) override;

        /**
         * \brief 移除子节点并更新 mChildrenUUID
         * \param [in] node : 待移除子节点
         * \return 移除成功返回 true
         */
        bool removeChild(TransformNode *node) override;

        /**
         * \brief 按 UUID 移除子节点
         * \param [in] nodeID : 子节点 UUID
         * \return 被移除的子节点智能指针；未找到时行为取决于 TreeNode 基类
         */
        TransformNodePtr removeChild(const UUID &nodeID) override;

        /**
         * \brief 移除所有子节点并清空 mChildrenUUID
         */
        void removeAllChildren() override;

        /**
         * \brief 从父节点移除自身
         */
        void removeFromParent()
        {
            if (getParent() != nullptr)
            {
                getParent()->removeChild(this);
            }
        }

        /**
         * \brief 设置用户自定义数据指针
         * \param [in] data : 用户数据，不持有所有权
         */
        void setUserData(void *data) { mUserData = data; }

        /// 返回用户自定义数据指针
        void *getUserData() const { return mUserData; }
        
        /**
         * \brief DFS 遍历子树并输出层次结构文本
         * \param [in] outputLog : 是否写入 DEBUG 日志
         * \param [in] verbose : 是否输出组件类型列表
         * \return 层次结构字符串
         */
        virtual String printHierarchy(bool outputLog = true, bool verbose = false);
        
    protected:
        /// 默认构造
        TransformNode() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 组件唯一标识
         */
        TransformNode(const UUID &uuid);

        /**
         * \brief 克隆属性（TransformNode 基类无额外字段，子树由 GameObject::clone() 递归处理）
         * \param [in] src : 源组件，不可为 nullptr
         * \return 成功返回 T3D_OK；src 为空返回 T3D_ERR_INVALID_POINTER
         */
        TResult cloneProperties(const Component * const src) override;

        /**
         * \brief 挂接父节点回调（默认空实现，子类可覆写）
         * \param [in] parent : 新的父节点
         */
        virtual void onAttachParent(TransformNode *parent);

        /**
         * \brief 脱离父节点回调（默认空实现，子类可覆写）
         * \param [in] parent : 原父节点
         */
        virtual void onDetachParent(TransformNode *parent);

        /**
         * \brief 销毁时从父节点移除并递归 removeAllChildren
         */
        void onDestroy() override;
        
    private:
        using ChildrenUUID = TList<UUID>;

        /// 返回序列化的子节点 UUID 列表
        TPROPERTY(RTTRFuncName="Children", RTTRFuncType="getter")
        const ChildrenUUID &getChildrenUUID() const { return mChildrenUUID; }

        /**
         * \brief 设置序列化的子节点 UUID 列表
         * \param [in] childrenUUID : 子节点 UUID 列表
         */
        TPROPERTY(RTTRFuncName="Children", RTTRFuncType="setter")
        void setChildrenUUID(const ChildrenUUID &childrenUUID) { mChildrenUUID = childrenUUID; }

        /**
         * \brief 反序列化后按 mChildrenUUID 通过 msComponents 查找并挂接子节点
         */
        void setupHierarchy();

        /// 用户自定义数据指针（不持有所有权）
        void *mUserData {nullptr};

        /// 序列化的子节点 UUID 列表
        ChildrenUUID    mChildrenUUID {};
    };
}

#endif


#endif  /*__T3D_TRANSFORM_NODE_H__*/
