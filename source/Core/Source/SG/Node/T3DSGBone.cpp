

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

    void SGBone::updateBone(const Matrix4 &matInverseRootParent)
    {
        bool isTransformDirty = isDirty();
        getLocalToWorldTransform();

        auto children = getChildren();
        auto itr = children.begin();

        while (itr != children.end())
        {
            auto child = *itr;
            T3D_ASSERT(child->getNodeType() == Node::E_NT_BONE);
            SGBonePtr bone = smart_pointer_cast<SGBone>(child);
            bone->updateBone(matInverseRootParent);
            ++itr;
        }

        if (isTransformDirty)
        {
            const Matrix4 &matCombine = getLocalToWorldTransform().getAffineMatrix();
            Matrix4 matLocal = getLocalTransform().getAffineMatrix();
            matCombine.printLog(getName() + " matCombine : ");
            matLocal.printLog(getName() + " matBone : ");
            BoneDataPtr boneData = smart_pointer_cast<BoneData>(mBoneData);
            boneData->mOffsetMatrix.printLog(getName() + " matVertex : ");
            mFinalMatrix = matInverseRootParent * matCombine * boneData->mOffsetMatrix;
            mFinalMatrix.printLog(getName() + " matFinal : ");
        }
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

