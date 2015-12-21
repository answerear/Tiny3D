

namespace Tiny3D
{
    inline uint32_t SGNode::getNodeID() const
    {
        return mID;
    }

    inline void SGNode::setName(const String &name)
    {
        mName = name;
    }

    inline const String &SGNode::getName() const
    {
        return mName;
    }

    inline void SGNode::setUserData(long_t data)
    {
        mUserData = data;
    }

    inline long_t SGNode::getUserData() const
    {
        return mUserData;
    }

    inline void SGNode::setUserObject(const ObjectPtr &rkObject)
    {
        mUserObject = rkObject;
    }

    inline const ObjectPtr &SGNode::getUserObject() const
    {
        return mUserObject;
    }

    inline ObjectPtr SGNode::getUserObject()
    {
        return mUserObject;
    }

    inline const SGChildren &SGNode::getChildren() const
    {
        return mChildren;
    }

    inline const SGNodePtr &SGNode::getParent() const
    {
        return mParent;
    }

    inline SGNodePtr SGNode::getParent()
    {
        return mParent;
    }

    inline bool SGNode::isDirty() const
    {
        return mIsDirty;
    }
}
