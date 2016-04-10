

namespace Tiny3D
{
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

    inline bool SGNode::isDirty() const
    {
        return mIsDirty;
    }
}
