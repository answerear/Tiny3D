

#include "SG/T3DSGNode.h"


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
    }

    void SGNode::addChild(SGNode *node)
    {
        T3D_SAFE_ACQUIRE(node);
        mChildren.push_back(node);
        node->mParent = this;
        node->onAttachParent(this);
    }

    void SGNode::removeChild(SGNode *node)
    {
        if (node != nullptr)
        {
            SGChildrenItr itr = mChildren.begin();

            while (itr != mChildren.end())
            {
                SGNode *&child = *itr;

                if (child == node)
                {
                    child->onDetachParent(this);
                    child->mParent = nullptr;
                    child->release();
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
            SGNode *&child = *itr;

            if (child != nullptr && child->getNodeID() == nodeID)
            {
                child->onDetachParent(this);
                child->mParent = nullptr;
                child->release();
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
            SGNode *&child = *itr;

            child->onDetachParent(this);
            child->mParent = nullptr;
            child->release();

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

    SGNode *SGNode::getChild(uint32_t nodeID)
    {
        SGNode *child = nullptr;
        SGChildrenItr itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            SGNode *&node = *itr;
            if (child->getNodeID() == nodeID)
            {
                child = node;
            }
            ++itr;
        }

        return child;
    }

    SGNode *SGNode::getChild(const String &name)
    {
        SGNode *child = nullptr;
        SGChildrenItr itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            SGNode *&node = *itr;
            if (child->getName() == name)
            {
                child = node;
            }
            ++itr;
        }

        return child;
    }

    void SGNode::update()
    {
        SGChildrenItr itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            SGNode *&node = *itr;
            node->update();
            ++itr;
        }
    }

    void SGNode::render()
    {
        SGChildrenItr itr = mChildren.begin();

        while (itr != mChildren.end())
        {
            SGNode *&node = *itr;
            node->render();
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
                SGNode *&node = *itr;
                node->setDirty(isDirty, recursive);
                ++itr;
            }
        }
    }

    void SGNode::cloneProperties(SGNode *node)
    {
        mName = node->mName;
        mUserData = node->mUserData;
        mUserObject = node->mUserObject;
        T3D_SAFE_ACQUIRE(mUserObject);

        SGChildrenItr itr = node->mChildren.begin();
        while (itr != node->mChildren.end())
        {
            SGNode *child = *itr;
            SGNode *newChild = child->clone();
            newChild->cloneProperties(child);
            node->addChild(newChild);
            node->autorelease();
            ++itr;
        }
    }

    void SGNode::onAttachParent(SGNode *parent)
    {

    }

    void SGNode::onDetachParent(SGNode *parent)
    {

    }
}
