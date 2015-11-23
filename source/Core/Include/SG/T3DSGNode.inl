

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

    inline void SGNode::setUserObject(Object *pObject)
    {
        T3D_SAFE_RELEASE(mUserObject);
        mUserObject = pObject;
        T3D_SAFE_ACQUIRE(mUserObject);
    }

    inline const SGChildren &SGNode::getChildren() const
    {
        return mChildren;
    }

    inline SGNode *SGNode::getParent()
    {
        return mParent;
    }
}
