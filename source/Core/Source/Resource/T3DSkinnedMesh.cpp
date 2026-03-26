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
#include "Resource/T3DSkeleton.h"
#include "Resource/T3DSkeletonManager.h"
#include "Kernel/T3DTransform.h"
#include "Kernel/T3DGameObject.h"
#include "Component/T3DTransformNode.h"
#include "Component/T3DTransform3D.h"
#include "Component/T3DBone.h"


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
        Skeleton *skeleton, SkeletalAnimation *skeletalAni, 
        const Vector3 &position, const Quaternion &orientation, const Vector3 &scaling,
        const String &meshNodeName)
    {
        return T3D_NEW SkinnedMesh(name, std::move(attributes), std::move(vertices), std::move(strides), std::move(offsets), std::move(submeshes), skeleton, skeletalAni, position, orientation, scaling, meshNodeName);
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
        SubMeshes &&submeshes, Skeleton *skeleton, SkeletalAnimation *skeletalAni,
        const Vector3 &position, const Quaternion &orientation, const Vector3 &scaling,
        const String &meshNodeName)
        : Mesh(name, std::move(attributes), std::move(vertices), std::move(strides), std::move(offsets), std::move(submeshes), position, orientation, scaling, meshNodeName)
        , mSkeletonUUID(skeleton->getUUID())
        , mSkeletalAniUUID(skeletalAni->getUUID())
        , mSkeleton(skeleton)
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
        } while (false);
        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult SkinnedMesh::generateRenderResource(Archive *archive)
    {
        TResult ret = Mesh::generateRenderResource(archive);

        do
        {
            mSkeleton = T3D_SKELETON_MGR.loadSkeleton(archive, mSkeletonUUID);
            mSkeletalAni = T3D_ANIMATION_MGR.loadSkeletalAnimation(archive, mSkeletalAniUUID);

#if defined (T3D_DEBUG)
            mIsBonesDirty = true;

            if (mIsBonesDirty)
            {
                populateBoneHierarchy();
                mIsBonesDirty = false;

                String bones = getBoneDebugString(mJoints[mJointRootIdx], 0);

                T3D_LOG_DEBUG(LOG_TAG_RESOURCE, "Mesh (%s) Skeleton :\n%s", getName().c_str(), bones.c_str());
            }
#endif
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
                    // Matrix3 matR;
                    // orientation.toRotationMatrix(matR);
                    // Radian xAngle, yAngle, zAngle;
                    // matR.toEulerAnglesYXZ(yAngle, xAngle, zAngle);
                    Radian xAngle, yAngle, zAngle;
                    orientation.toEulerAnglesYXZ(yAngle, xAngle, zAngle);
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

        GameObjectPtr rootBoneGO = mSkeleton->getRootBoneGameObject();
        if (rootBoneGO == nullptr)
        {
            return;
        }

        // 先收集所有骨骼 GameObject，按 DFS 顺序建立索引
        TArray<GameObject*> allBoneGOs;
        TUnorderedMap<GameObject*, uint16_t> goToIndex;

        // DFS 遍历骨骼子树，收集所有骨骼节点
        std::function<void(GameObject*)> collectBones = [&](GameObject *go)
        {
            Bone *bone = go->getComponent<Bone>().get();
            if (bone == nullptr) return;

            uint16_t idx = static_cast<uint16_t>(allBoneGOs.size());
            goToIndex[go] = idx;
            allBoneGOs.push_back(go);

            TransformNode *node = go->getTransformNode();
            if (node == nullptr) return;

            for (auto itr = node->child_begin(); itr != node->child_end(); ++itr)
            {
                collectBones(itr->get()->getGameObject());
            }
        };
        collectBones(rootBoneGO.get());

        mJoints.resize(allBoneGOs.size(), nullptr);

        for (uint16_t i = 0; i < static_cast<uint16_t>(allBoneGOs.size()); i++)
        {
            JointNode *node = T3D_NEW JointNode();
            node->joint = i;
            mJoints[i] = node;

            TransformNode *tn = allBoneGOs[i]->getTransformNode();
            TransformNode *parentTN = (tn != nullptr) ? tn->getParent() : nullptr;
            if (parentTN == nullptr)
            {
                mJointRootIdx = i;
            }
            else
            {
                GameObject *parentGO = parentTN->getGameObject();
                auto it = goToIndex.find(parentGO);
                if (it != goToIndex.end())
                {
                    mJoints[it->second]->children.emplace_back(i);
                }
                else
                {
                    mJointRootIdx = i;
                }
            }
        }

        mAllBoneGOs = allBoneGOs;
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

        GameObject *boneGO = mAllBoneGOs[node->joint];
        Transform3D *transform3D = boneGO->getComponent<Transform3D>().get();

        ss << "Bone - name : " << boneGO->getName();
        if (transform3D != nullptr)
        {
            ss << " T : " << transform3D->getPosition().getDebugString();
            const Quaternion &orientation = transform3D->getOrientation();
            Radian xAngle, yAngle, zAngle;
            orientation.toEulerAnglesYXZ(yAngle, xAngle, zAngle);
            ss << " R : (" << xAngle.valueDegrees() << ", " << yAngle.valueDegrees() << ", " << zAngle.valueDegrees() << ")";
            ss << " S : " << transform3D->getScaling().getDebugString();
        }
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
