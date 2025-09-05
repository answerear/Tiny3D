/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include "Resource/T3DSkinnedMesh.h"
#include "T3DErrorDef.h"
#include "Resource/T3DSkeletalAnimation.h"
#include "Resource/T3DAnimationManager.h"
#include "Animation/T3DAnimationClip.h"
#include "Animation/T3DAnimationTrack.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SkinnedMeshPtr SkinnedMesh::create(const String &name)
    {
        return T3D_NEW SkinnedMesh(name);
    }

    //--------------------------------------------------------------------------

    SkinnedMeshPtr SkinnedMesh::create(const String &name,
        VertexAttributes &&attributes, Vertices &&vertices,
        VertexStrides &&strides, VertexOffsets &&offsets, SubMeshes &&submeshes,
        SkeletalAnimation *skeletalAni, Bones &&bones)
    {
        return T3D_NEW SkinnedMesh(name, std::move(attributes), std::move(vertices), std::move(strides), std::move(offsets), std::move(submeshes), skeletalAni, std::move(bones));
    }

    //--------------------------------------------------------------------------

    SkinnedMesh::~SkinnedMesh()
    {

    }
    
    //--------------------------------------------------------------------------

    SkinnedMesh::SkinnedMesh(const String &name)
        : Mesh(name)
    {
        
    }

    //--------------------------------------------------------------------------

    SkinnedMesh::SkinnedMesh(const String &name, VertexAttributes &&attributes,
        Vertices &&vertices, VertexStrides &&strides, VertexOffsets &&offsets,
        SubMeshes &&submeshes, SkeletalAnimation *skeletalAni, Bones &&bones)
        : Mesh(name, std::move(attributes), std::move(vertices), std::move(strides), std::move(offsets), std::move(submeshes))
        , mSkeletalAniUUID(skeletalAni->getUUID())
        , mBones(std::move(bones))
        , mSkeletalAni(skeletalAni)
    {
        
    }
    
    //--------------------------------------------------------------------------

    Resource::Type SkinnedMesh::getType() const
    {
        return Type::kSkinnedMesh;
    }

    //--------------------------------------------------------------------------
    
    ResourcePtr SkinnedMesh::clone() const
    {
        SkinnedMeshPtr mesh = create(getName());
        mesh->cloneProperties(this);
        return mesh;
    }
    
    //--------------------------------------------------------------------------

    void SkinnedMesh::cloneProperties(const Resource *const src)
    {
        Mesh::cloneProperties(src);
        
        const SkinnedMesh *mesh = static_cast<const SkinnedMesh*>(src);
    }

    //--------------------------------------------------------------------------

    TResult SkinnedMesh::onCreate()
    {
        TResult ret = Mesh::onCreate();
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult SkinnedMesh::onLoad(Archive *archive)
    {
        TResult ret;

        do
        {
            ret = Mesh::onLoad(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            ret = generateRenderResource(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult SkinnedMesh::generateRenderResource(Archive *archive)
    {
        TResult ret = Mesh::generateRenderResource(archive);

        do
        {
            mSkeletalAni = T3D_ANIMATION_MGR.loadSkeletalAnimation(archive, mSkeletalAniUUID);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

#if defined (T3D_DEBUG)
    String SkinnedMesh::getDebugInfo() const
    {
        if (mIsBonesDirty)
        {
            populateBoneHierarchy();
            mIsBonesDirty = false;
        }

        String bones = getBoneDebugString(mJoints[mJointRootIdx], 0);

        std::stringstream ss;
        ss << std::endl << bones;

        ss << "Animation (" << mSkeletalAni->getName() << ") Data : " << std::endl;

        const AnimationClips &clips = mSkeletalAni->getAnimationClips();
        
        for (const auto &item : clips)
        {
            ss << "\tAnimation Clip (" << item.second->getName() << ") - Duration : " << item.second->getDuration() << std::endl;
            for (const auto &track : item.second->getTracks())
            {
                ss << "\t\tTrack : " << track.first << std::endl;
                ss << "\t\t\tTranslation :" << std::endl;
                for (const auto &kfTranslation : track.second->getTranslationTrack())
                {
                    const Vector3 &translation = kfTranslation->getTranslation();
                    ss << "\t\t\t\tTime : " << kfTranslation->getTime() << " - " << translation.getDebugString() << std::endl;
                }

                ss << "\t\t\tRotation :" << std::endl;
                for (const auto &kfOrienetation : track.second->getOrientationTrack())
                {
                    const Quaternion &orientation = kfOrienetation->getOrientation();
                    Matrix3 matR;
                    orientation.toRotationMatrix(matR);
                    Radian xAngle, yAngle, zAngle;
                    matR.toEulerAnglesZXY(zAngle, xAngle, yAngle);
                    ss << "\t\t\t\tTime : " << kfOrienetation->getTime() << " - (" << xAngle.valueDegrees() << ", " << yAngle.valueDegrees() << ", " << zAngle.valueDegrees() << ")" << std::endl;
                }
                ss << "\t\t\tScaling :" << std::endl;
                for (const auto &kfScaling : track.second->getScalingTrack())
                {
                    const Vector3 &scaling = kfScaling->getScaling();
                    ss << "\t\t\t\tTime : " << kfScaling->getTime() << " - " << scaling.getDebugString() << std::endl;
                }
            }
        }

        return ss.str();
    }

    void SkinnedMesh::populateBoneHierarchy() const
    {
        clearJoints();

        uint16_t index = 0;
        mJoints.resize(mBones.size(), nullptr);
        
        for (const auto &bone : mBones)
        {
            JointNode *node = T3D_NEW JointNode();
            node->joint = index;
            
            uint16_t parentIdx = bone->getParentIndex();
            if (parentIdx == BoneNode::kInvalidIndex)
            {
                mJointRootIdx = index;
            }

            mJoints[index] = node;
            index++;
        }

        index = 0;
        
        for (const auto &bone : mBones)
        {
            JointNode *node = mJoints[index];

            uint16_t parentIdx = bone->getParentIndex();
            if (parentIdx != BoneNode::kInvalidIndex)
            {
                mJoints[parentIdx]->children.emplace_back(index);
            }
            
            index++;
        }
    }

    void SkinnedMesh::clearJoints() const
    {
        for (auto &joint : mJoints)
        {
            T3D_SAFE_DELETE(joint);
        }

        mJoints.clear();
    }

    String SkinnedMesh::getBoneDebugString(JointNode *node, int32_t tabCount) const
    {
        std::stringstream ss;
        for (int32_t i = 0; i < tabCount; i++)
        {
            ss << "\t";
        }

        BoneNode *bone = mBones[node->joint];
        
        ss << "Bone - name : " << bone->getName();
        ss << " T : " << bone->getTranslation().getDebugString();
        const Quaternion &orientation = bone->getRotation();
        Matrix3 matR;
        orientation.toRotationMatrix(matR);
        Radian xAngle, yAngle, zAngle;
        matR.toEulerAnglesZXY(zAngle, xAngle, yAngle);
        ss << " R : (" << xAngle.valueDegrees() << ", " << yAngle.valueDegrees() << ", " << zAngle.valueDegrees() << ")";
        ss << " S : " << bone->getScaling().getDebugString();
        ss << std::endl;
        for (const auto child : node->children)
        {
            ss << getBoneDebugString(mJoints[child], tabCount + 1);
        }
        
        return ss.str();
    }

#endif
    
    //--------------------------------------------------------------------------
}
