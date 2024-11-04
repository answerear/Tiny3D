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

#ifndef __T3D_TREE_H__
#define __T3D_TREE_H__


#include "T3DUtilsPrerequisites.h"


namespace Tiny3D
{
    template<typename KEY>
    class TreeNodeKey
    {
    public:
        virtual const KEY &getKey() const = 0;
    };

    template<typename KEY, typename VALUE, typename POINTER_TYPE = VALUE *, typename HASHER = std::hash<KEY>, typename KEY_EQ = std::equal_to<KEY>>
    class TreeNode : public TreeNodeKey<KEY>
    {
    public:
        using key_t = KEY;
        using value_t = VALUE;
        using pointer_t = value_t *;
        using smart_pointer_t = POINTER_TYPE;

        using Action = TFunction<void(pointer_t)>;
        using QueryAction = TFunction<bool(pointer_t)>;

        using Children = TList<smart_pointer_t>;
        using ChildrenItr = typename Children::iterator;
        using ChildrenMap = TUnorderedMap<key_t, pointer_t, HASHER, KEY_EQ>;

        TreeNode() = default;

        virtual ~TreeNode()
        {
            mChildren.clear();
            mChildrenMap.clear();
        }

        TreeNode(const TreeNode &) = delete;

        TreeNode &operator=(const TreeNode &) = delete;

        TreeNode(TreeNode &&) = delete;

        TreeNode &operator=(TreeNode &&) = delete;

        virtual bool addChild(pointer_t node, const Action &action)
        {
            if (node == nullptr)
            {
                return false;
            }
            
            if (node->mParent != nullptr)
            {
                return false;
            }
            
            auto rval = mChildrenMap.emplace(node->getKey(), node);
            if (rval.second)
            {
                mChildren.emplace_back(node);
                node->mParent = static_cast<pointer_t>(this);
                node->mSelfItr = std::prev(mChildren.end());
                mIsDirty = true;
                if (action != nullptr)
                {
                    action(node);
                }
            }
            
            return rval.second;
        }

        virtual bool addChild(pointer_t node)
        {
            return TreeNode::addChild(node, nullptr);
        }

        virtual bool insertAfterChild(pointer_t prevNode, pointer_t node, const Action &action)
        {
            if (node == nullptr)
            {
                return false;
            }

            auto it = mChildrenMap.find(prevNode->getKey());
            if (it == mChildrenMap.end())
            {
                return false;
            }

            auto rval = mChildrenMap.emplace(node->getKey(), node);
            if (rval.second)
            {
                if (prevNode == nullptr)
                {
                    // 插入头
                    mChildren.emplace_front(node);
                    node->mSelfItr = mChildren.begin();
                }
                else
                {
                    node->mSelfItr = mChildren.insert(std::next(prevNode->mSelfItr), node);
                }
                
                if (action != nullptr)
                {
                    action(node);
                }
            }
            
            return rval.second;
        }

        virtual bool insertAfterChild(pointer_t prevNode, pointer_t node)
        {
            return insertAfterChild(prevNode, node, nullptr);
        }

        virtual bool removeChild(pointer_t node, const Action &action)
        {
            bool ret = false;
            if (node != nullptr)
            {
                auto itr = mChildrenMap.find(node->getKey());
                if (itr == mChildrenMap.end())
                {
                    if (action != nullptr)
                    {
                        action(node);
                    }
                    
                    node->mParent = nullptr;
                    mChildren.remove(node);
                    mChildrenMap.erase(itr);
                    mIsDirty = true;
                    ret = true;
                }
            }
            return ret;
        }

        virtual bool removeChild(pointer_t node)
        {
            return TreeNode::removeChild(node, nullptr);
        }

        virtual smart_pointer_t removeChild(const key_t &key, const Action &action)
        {
            auto itr = mChildrenMap.find(key);
            if (itr != mChildrenMap.end())
            {
                if (action != nullptr)
                {
                    action(static_cast<pointer_t>(this));
                }
                
                smart_pointer_t node = itr->second;
                mChildren.remove(node);
                mChildrenMap.erase(itr);
                mIsDirty = true;
                return node;
            }
            return nullptr;
        }

        virtual smart_pointer_t removeChild(const key_t &key)
        {
            return TreeNode::removeChild(key, nullptr);
        }
        
        // void removeFromParent()
        // {
        //     if (getParent() != nullptr)
        //     {
        //         getParent->removeChild(this);
        //     }
        // }
        
        virtual void removeAllChildren(const Action &removeAction, const Action &deleteAction = nullptr)
        {
            auto itr = mChildren.rbegin();
            while (itr != mChildren.rend())
            {
                auto itrCur = itr++;

                if (removeAction != nullptr)
                {
                    removeAction(*itrCur);
                }

                (*itrCur)->mParent = nullptr;
                
                if (deleteAction != nullptr)
                {
                    deleteAction(*itrCur);
                }
            }

            mSelfItr = mChildren.end();
            mChildrenMap.clear();
            mChildren.clear();
            mIsDirty = true;
        }

        virtual void removeAllChildren()
        {
            return TreeNode::removeAllChildren([this](pointer_t node){ removeChild(node); }, [](pointer_t node) { delete node;});
        }

        pointer_t getParent() const { return mParent; }

        pointer_t getChild(const key_t &key) const
        {
            auto itr = mChildrenMap.find(key);
            if (itr != mChildrenMap.end())
            {
                return itr->second;
            }
            return nullptr;
        }

        const Children &getChildren() const { return mChildren; }

        size_t getChildrenCount() const { return mChildren.size(); }

        size_t getTotalCount() const
        {
            if (mIsDirty)
            {
                size_t mTotalCount = 1;
                for (auto child : mChildren)
                {
                    mTotalCount += child->getTotalCount();
                }
                mIsDirty = false;
            }
            return mTotalCount;
        }

        template<typename VisitAction, typename... Args>
        void dfs_visit(bool selfFirst, const QueryAction &query, const VisitAction &action, Args &&... args)
        {
            if (query == nullptr || query(static_cast<pointer_t>(this)))
            {
                if (selfFirst)
                {
                    action(static_cast<pointer_t>(this), std::forward<Args>(args)...);
                }

                for (auto &child : mChildren)
                {
                    child->dfs_visit(selfFirst, query, action, std::forward<Args>(args)...);
                }

                if (!selfFirst)
                {
                    action(static_cast<pointer_t>(this), std::forward<Args>(args)...);
                }
            }
        }

        template<typename VisitAction, typename... Args>
        void bfs_visit(bool selfFirst, const QueryAction &query, const VisitAction &action, Args &&... args)
        {
            if (query == nullptr || query(static_cast<pointer_t>(this)))
            {
                std::queue<pointer_t> queue;
                queue.push(static_cast<pointer_t>(this));

                while (!queue.empty())
                {
                    pointer_t currentNode = queue.front();
                    queue.pop();

                    if (selfFirst)
                    {
                        action(currentNode, query, std::forward<Args>(args)...);
                    }

                    for (auto &child : currentNode->children)
                    {
                        queue.push(child);
                    }

                    if (!selfFirst)
                    {
                        action(currentNode, std::forward<Args>(args)...);
                    }
                }
            }
        }

        using child_iterator = typename std::list<smart_pointer_t>::iterator;

        using const_child_iterator = typename std::list<smart_pointer_t>::const_iterator;
        
        child_iterator child_begin()
        {
            return mChildren.begin();
        }

        const_child_iterator child_begin() const
        {
            return mChildren.begin();
        }

        child_iterator child_end()
        {
            return mChildren.end();
        }

        const_child_iterator child_end() const
        {
            return mChildren.end();
        }
        
        class dfs_iterator
        {
        public:
            dfs_iterator(TreeNode *root)
            {
                if (root != nullptr)
                {
                    mStack.push(root);
                }
            }

            ~dfs_iterator() = default;

            dfs_iterator(const dfs_iterator &other)
                : mStack(other.mStack)
            {
            }

            dfs_iterator &operator =(const dfs_iterator &other)
            {
                if (this != &other)
                {
                    mStack = other.mStack;
                }
                return *this;
            }

            dfs_iterator(dfs_iterator &&other) = default;

            dfs_iterator &operator =(dfs_iterator &&other) = default;

            TreeNode *operator *()
            {
                return mStack.top();
            }

            dfs_iterator &operator ++()
            {
                if (mStack.empty())
                {
                    return *this;
                }
                TreeNode *current = mStack.top();
                mStack.pop();
                for (auto itr = current->mChildren.begin(); itr != current->mChildren.end(); ++itr)
                {
                    mStack.push(*itr);
                }
                return *this;
            }

            bool operator !=(const dfs_iterator &other) const
            {
                return (!mStack.empty() || !other.mStack.empty());
            }

            bool operator ==(const dfs_iterator &other) const
            {
                return (mStack.empty() && other.mStack.mepty()) || (mStack.top() == other.mStack.top());
            }

        private:
            TStack<TreeNode *> mStack {};
        };

        class bfs_iterator
        {
        public:
            bfs_iterator(TreeNode *root)
            {
                if (root != nullptr)
                {
                    mQueue.push(root);
                }
            }

            ~bfs_iterator()
            {
                // mQueue.clear();
                while (!mQueue.empty())
                {
                    mQueue.pop();
                }
            }

            bfs_iterator(const bfs_iterator &other)
                : mQueue(other.mQueue)
            {
            }

            bfs_iterator &operator =(const bfs_iterator &other)
            {
                if (this != &other)
                {
                    mQueue = other.mQueue;
                }
                return *this;
            }

            bfs_iterator(bfs_iterator &&other) = delete;

            bfs_iterator &operator =(bfs_iterator &&other) = delete;

            TreeNode *operator *()
            {
                return mQueue.front();
            }

            bfs_iterator &operator ++()
            {
                if (mQueue.empty())
                {
                    return *this;
                }

                TreeNode *current = mQueue.front();
                mQueue.pop();
                for (auto child : current->mChildren)
                {
                    mQueue.push(child);
                }
                return *this;
            }

            bool operator !=(const bfs_iterator &other) const
            {
                return !mQueue.empty() || !other.mQueue.empty();
            }

            bool operator ==(const bfs_iterator &other) const
            {
                return (mQueue.empty() && other.mQueue.empty()) || (mQueue.front() == other.mQueue.front());
            }

        private:
            TQueue<TreeNode *> mQueue {};
        };

        dfs_iterator dfs_begin()
        {
            return dfs_iterator(this);
        }

        dfs_iterator dfs_end()
        {
            return dfs_iterator(nullptr);
        }

        bfs_iterator bfs_begin()
        {
            return bfs_iterator(this);
        }

        bfs_iterator bfs_end()
        {
            return bfs_iterator(nullptr);
        }

        using iterator = bfs_iterator;

        iterator begin()
        {
            return bfs_begin();
        }

        iterator end()
        {
            return bfs_end();
        }

    protected:
        smart_pointer_t mParent {nullptr};
        Children mChildren {};
        ChildrenMap mChildrenMap {};
        ChildrenItr mSelfItr {};
        mutable bool mIsDirty {false};
        mutable size_t mTotalCount {0};
    };
}


#endif  /*__T3D_TREE_H__*/