

#include "T3DBoneData.h"


namespace Tiny3D
{
    BoneDataPtr BoneData::create(const String &ID)
    {
        BoneDataPtr bone = new BoneData(ID);
        bone->release();
        return bone;
    }

    BoneData::BoneData(const String &ID)
        : mParent(nullptr)
        , mChild(nullptr)
        , mPrevSibling(nullptr)
        , mNextSibling(nullptr)
        , mID(ID)
        , mBoneTransform(false)
    {

    }

    BoneData::~BoneData()
    {
        mParent = nullptr;
        mChild = nullptr;
        mPrevSibling = nullptr;
        mNextSibling = nullptr;
    }
}

