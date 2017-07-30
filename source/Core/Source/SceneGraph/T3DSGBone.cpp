/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "SceneGraph/T3DSGBone.h"
#include "Misc/T3DBoneData.h"


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

    void SGBone::updateTransform()
    {
        bool isTransformDirty = isDirty();

        SGTransformNode::updateTransform();

        if (isTransformDirty)
        {
            const Matrix4 &matCombine = getLocalToWorldTransform().getAffineMatrix();
            Matrix4 matLocal = getLocalTransform().getAffineMatrix();
//             matCombine.printLog(getName() + " matCombine : ");
//             matLocal.printLog(getName() + " matBone : ");
            BoneDataPtr boneData = smart_pointer_cast<BoneData>(mBoneData);
//             boneData->mOffsetMatrix.printLog(getName() + " matVertex : ");
//             boneData->mLocalMatrix.printLog(getName() + "matBindPose : ");
            mFinalMatrix = matCombine * boneData->mOffsetMatrix;
//             mFinalMatrix.printLog(getName() + " matFinal : ");
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

