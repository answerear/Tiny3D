

#include "SG/Node/T3DSGNode.h"


namespace Tiny3D
{
    SGNode::SGNode(uint32_t unID /* = E_NID_AUTOMATIC */)
        : mID(E_NID_INVALID)
        , mName("")
        , mUserData(0)
        , mUserObject(nullptr)
        , mParent(nullptr)
    {
        if (E_NID_AUTOMATIC == unID)
        {
            mID = makeGlobalID();
        }
        else
        {
            mID = unID;
        }
    }

    SGNode::~SGNode()
    {
        removeAllChildren();
    }

    void SGNode::addChild(const SGNodePtr &node)
    {
        T3D_ASSERT(node->getParent() == nullptr);
        mChildren.push_back(node);
        node->mParent = this;
        node->onAttachParent(this);
    }

    void SGNode::removeChild(const SGNodePtr &node)
    {
        if (node != nullptr)
        {
            SGChildrenItr itr = mChildren.begin();

            while (itr != mChildren.end())
            {
                SGNodePtr &child = *itr;

                if (child == node)
                {
                    child->onDetachParent(this);
                    child->mParent = nullptr;
                    mChildren.erase(itr);
                    break;
                }

                ++itr;
            }
        }
    }

    void SGNode::removeChild(uint32_t nodeID)
    {
        SGChildrenItr itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            SGNodePtr &child = *itr;

            if (child != nullptr && child->getNodeID() == nodeID)
            {
                child->onDetachParent(this);
                child->mParent = nullptr;
                mChildren.erase(itr);
                break;
            }

            ++itr;
        }
    }

    void SGNode::removeAllChildren()
    {
        SGChildrenItr itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            SGNodePtr &child = *itr;

            child->onDetachParent(this);
            child->mParent = nullptr;

            ++itr;
        }

        mChildren.clear();
    }

    void SGNode::removeFromParent()
    {
        if (mParent != nullptr)
        {
            mParent->removeChild(this);
        }
    }

    const SGNodePtr &SGNode::getChild(uint32_t nodeID) const
    {
        SGChildrenConstItr itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            const SGNodePtr &node = *itr;
            if (node->getNodeID() == nodeID)
            {
                return node;
                break;
            }
            ++itr;
        }

        return SGNodePtr::NULL_PTR;
    }

    SGNodePtr SGNode::getChild(uint32_t nodeID)
    {
        SGNodePtr child;
        SGChildrenItr itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            SGNodePtr &node = *itr;
            if (node->getNodeID() == nodeID)
            {
                child = node;
                break;
            }
            ++itr;
        }

        return child;
    }

    const SGNodePtr &SGNode::getChild(const String &name) const
    {
        SGChildrenConstItr itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            const SGNodePtr &node = *itr;
            if (node->getName() == name)
            {
                return node;
                break;
            }
            ++itr;
        }

        return SGNodePtr::NULL_PTR;
    }

    SGNodePtr SGNode::getChild(const String &name)
    {
        SGNodePtr child;
        SGChildrenItr itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            SGNodePtr &node = *itr;
            if (node->getName() == name)
            {
                child = node;
                break;
            }
            ++itr;
        }

        return child;
    }

    void SGNode::updateTransform()
    {
        SGChildrenItr itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            SGNodePtr &node = *itr;
            node->updateTransform();
            ++itr;
        }
    }

    void SGNode::frustumCulling(const FrustumBound &bound, const RenderQueuePtr &queue)
    {
        SGChildrenItr itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            SGNodePtr &node = *itr;
            node->frustumCulling(bound, queue);
            ++itr;
        }
    }

    void SGNode::setDirty(bool isDirty, bool recursive /* = false */)
    {
        mIsDirty = isDirty;

        if (recursive)
        {
            SGChildrenItr itr = mChildren.begin();

            while (itr != mChildren.end())
            {
                SGNodePtr &node = *itr;
                node->setDirty(isDirty, recursive);
                ++itr;
            }
        }
    }

    void SGNode::cloneProperties(SGNode *node) const
    {
        node->mName = mName;
        node->mUserData = mUserData;
        node->mUserObject = mUserObject;

        SGChildrenItr itr = node->mChildren.begin();
        while (itr != node->mChildren.end())
        {
            SGNodePtr &child = *itr;
            SGNodePtr newChild = child->clone();
            newChild->cloneProperties(child);
            node->addChild(newChild);
            ++itr;
        }
    }

    void SGNode::onAttachParent(const SGNodePtr &parent)
    {

    }

    void SGNode::onDetachParent(const SGNodePtr &parent)
    {

    }

    uint32_t SGNode::makeGlobalID() const
    {
        static uint32_t unID = 0;
        return ++unID;
    }
}
