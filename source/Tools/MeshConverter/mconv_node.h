

#ifndef __MCONV_NODE_H__
#define __MCONV_NODE_H__


#include "mconv_prerequisites.h"


namespace mconv
{
    typedef std::list<Node*>        Nodes;
    typedef Nodes::iterator         NodesItr;
    typedef Nodes::const_iterator   NodesConstItr;

    class Node
    {
    public:
        Node(const String &ID = "Node")
            : mID(ID)
            , mParent(nullptr)
        {

        }

        virtual ~Node()
        {
            removeAllChildren();
        }

        const String &getID() const
        {
            return mID;
        }

        size_t getChildrenCount() const
        {
            return mChildren.size();
        }

        void addChild(Node *node)
        {
            T3D_ASSERT(node->getParent() == nullptr);
            mChildren.push_back(node);
            node->mParent = this;
        }

        void removeChild(const String &ID, bool destroy = true)
        {
            NodesConstItr itr = mChildren.begin();
            while (itr != mChildren.end())
            {
                Node *child = *itr;
                if (child->getID() == ID)
                {
                    child->mParent = nullptr;
                    mChildren.erase(itr);
                    if (destroy)
                    {
                        delete child;
                    }
                    break;
                }

                ++itr;
            }
        }

        void removeChild(Node *node, bool destroy = true)
        {
            NodesItr itr = mChildren.begin();
            while (itr != mChildren.end())
            {
                Node *child = *itr;
                if (node == child)
                {
                    mChildren.erase(itr);
                    if (destroy)
                    {
                        delete child;
                    }
                    break;
                }
                ++itr;
            }
        }

        void removeAllChildren(bool destroy = true)
        {
            while (mChildren.empty())
            {
                Node *child = mChildren.front();
                child->mParent = nullptr;
                mChildren.pop_front();
                if (destroy)
                {
                    delete child;
                }
            }
        }

        bool getChild(const String &ID, Node *&node)
        {
            bool result = false;

            NodesConstItr itr = mChildren.begin();
            while (itr != mChildren.end())
            {
                Node *child = *itr;
                if (child->getID() == ID)
                {
                    node = child;
                    break;
                }

                ++itr;
            }

            return result;
        }

        Node *getParent()
        {
            return mParent;
        }

        void removeFromParent(bool destroy = true)
        {
            if (mParent != nullptr)
            {
                mParent->removeChild(this);
            }
        }

    protected:
        String  mID;
        Node    *mParent;
        Nodes   mChildren;
    };
}


#endif  /*__MCONV_NODE_H__*/
