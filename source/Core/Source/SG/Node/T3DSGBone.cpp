

#include "SG/Node/T3DSGBone.h"
#include "DataStruct/T3DBoneData.h"


namespace Tiny3D
{
    SGBonePtr SGBone::create(ObjectPtr data /* = nullptr */, uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGBonePtr bone = new SGBone(unID);

        if (bone != nullptr && bone->init(data))
        {
            bone->release();
        }
        else
        {
            T3D_SAFE_RELEASE(bone);
        }

        return bone;
    }

    SGBone::SGBone(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGTransformNode(unID)
        , mBoneData(nullptr)
    {

    }

    SGBone::~SGBone()
    {

    }

    bool SGBone::init(ObjectPtr data)
    {
        mBoneData = data;

        if (mBoneData != nullptr)
        {
            BoneDataPtr boneData = smart_pointer_cast<BoneData>(mBoneData);
            setName(boneData->mName);
        }

        return true;
    }

    uint16_t SGBone::getParentBone() const
    {
        uint16_t parent = 0xFFFF;
        BoneDataPtr boneData = smart_pointer_cast<BoneData>(mBoneData);
        if (boneData != nullptr)
        {
            parent = boneData->mParent;
        }
        return parent;
    }

    Node::Type SGBone::getNodeType() const
    {
        return E_NT_BONE;
    }

    NodePtr SGBone::clone() const
    {
        SGBonePtr bone = SGBone::create(mBoneData);
        if (bone != nullptr)
        {
            cloneProperties(bone);
        }
        return bone;
    }
}

