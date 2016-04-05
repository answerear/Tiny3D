

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
        enum Type
        {
            E_TYPE_NONE = 0,
            E_TYPE_ACTION,
            E_TYPE_ANIMATION,
            E_TYPE_BONE,
            E_TYPE_CAMERA,
            E_TYPE_LIGHT,
            E_TYPE_MATERIAL,
            E_TYPE_MATERIALS,
            E_TYPE_MESH,
            E_TYPE_MODEL,
            E_TYPE_SCENE,
            E_TYPE_SKELETON,
            E_TYPE_SKIN,
            E_TYPE_SUBMESH,
            E_TYPE_SUBMESHES,
            E_TYPE_TEXTURE,
            E_TYPE_TEXTURES,
        };

        Node(const String &ID = "Node")
            : mID(ID)
            , mParent(nullptr)
            , mSource(nullptr)
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

        virtual Type getNodeType() const = 0;

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

        void removeAllChildren(bool destroy = true)
        {
            while (!mChildren.empty())
            {
                Node *child = mChildren.front();
                child->mParent = nullptr;
                mChildren.pop_front();
                if (destroy)
                {
                    child->removeAllChildren(true);
                    delete child;
                }
            }
        }

        bool getChild(const String &ID, Node *&node) const
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

        Node *getChild(int index) const
        {
            int i = 0;
            Node *pChild = nullptr;
            NodesConstItr itr = mChildren.begin();

            while (itr != mChildren.end())
            {
                if (i == index)
                {
                    pChild = *itr;
                    break;
                }

                ++i;
                ++itr;
            }

            return pChild;
        }

        Node *getParent()
        {
            return mParent;
        }

        void removeFromParent(bool destroy = true)
        {
            if (mParent != nullptr)
            {
                mParent->removeChild(this, destroy);
            }
        }

    protected:
        String  mID;
        Node    *mParent;
        Nodes   mChildren;

        void    *mSource;
    };
}


#endif  /*__MCONV_NODE_H__*/
