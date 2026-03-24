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


#include "Component/T3DSkinnedGeometry.h"
#include "Resource/T3DSkinnedMesh.h"
#include "T3DErrorDef.h"
#include "Component/T3DTransform3D.h"
#include "Component/T3DBone.h"
#include "Resource/T3DSkeleton.h"
#include "Kernel/T3DGameObject.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SkinnedGeometry::SkinnedGeometry(const UUID &uuid)
        : Geometry(uuid)
    {
        mAnimationPlayer = AnimationPlayer::create(this);
    }

    //--------------------------------------------------------------------------

    SkinnedGeometry::~SkinnedGeometry()
    {
        mAnimationPlayer = nullptr;
    }

    //--------------------------------------------------------------------------

    void SkinnedGeometry::onStart()
    {
        Geometry::onStart();
    }

    //--------------------------------------------------------------------------

    void SkinnedGeometry::onUpdate()
    {
        T3D_ASSERT(mAnimationPlayer != nullptr);
        mAnimationPlayer->updateAnimation();
    }

    //--------------------------------------------------------------------------

    void SkinnedGeometry::onDestroy()
    {
        // 显式置空，触发骨骼根节点 GameObject 的销毁链：
        // TransformNode::onDestroy() 会自动从场景树摘除并递归销毁整个骨骼子树
        mRootBoneGameObject = nullptr;
        mAllBones.clear();
        mBoneGameObjects.clear();

        Geometry::onDestroy();
    }

    //--------------------------------------------------------------------------

    ComponentPtr SkinnedGeometry::clone() const
    {
        SkinnedGeometryPtr geometry = T3D_NEW SkinnedGeometry();
        TResult ret = geometry->cloneProperties(this);
        if (T3D_FAILED(ret))
        {
            geometry = nullptr;
        }
        return geometry;
    }

    //--------------------------------------------------------------------------

    TResult SkinnedGeometry::cloneProperties(const Component *const src)
    {
        TResult ret;

        do
        {
            ret = Geometry::cloneProperties(src);
            if (T3D_FAILED(ret))
            {
                break;
            }

            const SkinnedGeometry * const other = static_cast<const SkinnedGeometry * const>(src);
            mDefaultClipName = other->mDefaultClipName;
            mIsGPUSkinning = other->mIsGPUSkinning;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void SkinnedGeometry::onPostLoad()
    {
        Geometry::onPostLoad();
    }

    //--------------------------------------------------------------------------

    void SkinnedGeometry::onLoadResource(Archive *archive)
    {
        Geometry::onLoadResource(archive);

        populateAllChildren();
    }

    //--------------------------------------------------------------------------

    void SkinnedGeometry::generateRenderMaterial()
    {
        Geometry::generateRenderMaterial();
    }

    //--------------------------------------------------------------------------

    TResult SkinnedGeometry::populateAllChildren()
    {
        // 若已有旧骨骼子树（如热重载场景），先清理，防止旧节点残留
        if (mRootBoneGameObject != nullptr)
        {
            mRootBoneGameObject = nullptr;  // 触发旧骨骼子树的自动销毁和场景树摘除
            mAllBones.clear();
            mBoneGameObjects.clear();
        }

        GameObject *root = getGameObject();

        SkinnedMesh *skinnedMesh = static_cast<SkinnedMesh *>(getMeshObject());
        if (skinnedMesh == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_COMPONENT, "SkinnedGeometry::populateAllChildren failed. Skinned Mesh is nullptr !");
            return T3D_ERR_RES_INVALID_OBJECT;
        }

        Skeleton *skeleton = skinnedMesh->getSkeleton();
        if (skeleton == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_COMPONENT, "SkinnedGeometry::populateAllChildren failed. Skeleton is nullptr !");
            return T3D_ERR_RES_INVALID_OBJECT;
        }

        GameObjectPtr templateRootBone = skeleton->getRootBoneGameObject();
        if (templateRootBone == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_COMPONENT, "SkinnedGeometry::populateAllChildren failed. Root bone GameObject is nullptr !");
            return T3D_ERR_RES_INVALID_OBJECT;
        }

        // 克隆骨骼根节点的整个子树，作为该 SkinnedGeometry 实例独占的运行时骨骼副本
        mRootBoneGameObject = templateRootBone->clone();
        if (mRootBoneGameObject == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_COMPONENT, "SkinnedGeometry::populateAllChildren failed. Clone root bone GameObject failed !");
            return T3D_ERR_RES_INVALID_OBJECT;
        }

        T3D_LOG_DEBUG(LOG_TAG_COMPONENT, "SkinnedGeometry::populateAllChildren. Template root bone hierarchy:");
        templateRootBone->getTransformNode()->printHierarchy();
        
        T3D_LOG_DEBUG(LOG_TAG_COMPONENT, "SkinnedGeometry::populateAllChildren. Cloned root bone hierarchy:");
        mRootBoneGameObject->getTransformNode()->printHierarchy();
        
        mBoneGameObjects.clear();
        mAllBones.clear();

        // 获取 TransformNode，若失败则清理已克隆的骨骼子树后返回错误
        TransformNode *myNode = root != nullptr ? root->getTransformNode() : nullptr;
        if (myNode == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_COMPONENT, "SkinnedGeometry::populateAllChildren failed. TransformNode of root GameObject is nullptr !");
            mRootBoneGameObject = nullptr;
            mAllBones.clear();
            mBoneGameObjects.clear();
            return T3D_ERR_RES_INVALID_OBJECT;
        }

        // 第一步：遍历克隆子树，按骨骼名称建立 mAllBones 哈希表
        std::function<void(GameObject*)> collectClonedBones = [&](GameObject *go)
        {
            BonePtr bone = go->getComponent<Bone>();
            if (bone != nullptr)
            {
                mAllBones.emplace(go->getName(), go);
            }

            TransformNode *node = go->getTransformNode();
            if (node == nullptr) return;

            for (auto itr = node->child_begin(); itr != node->child_end(); ++itr)
            {
                collectClonedBones(itr->get()->getGameObject());
            }
        };
        collectClonedBones(mRootBoneGameObject.get());

        // 第二步：按模板骨骼子树的 DFS 顺序，填充 mBoneGameObjects 数组（保证索引与模板一致）
        std::function<void(GameObject*)> buildIndexByTemplate = [&](GameObject *go)
        {
            BonePtr bone = go->getComponent<Bone>();
            if (bone != nullptr)
            {
                auto it = mAllBones.find(go->getName());
                if (it != mAllBones.end())
                {
                    mBoneGameObjects.push_back(it->second);
                }
            }

            TransformNode *node = go->getTransformNode();
            if (node == nullptr) return;

            for (auto itr = node->child_begin(); itr != node->child_end(); ++itr)
            {
                buildIndexByTemplate(itr->get()->getGameObject());
            }
        };
        buildIndexByTemplate(templateRootBone.get());

        // 将克隆出的骨骼根节点作为兄弟节点挂接到 SkinnedGeometry 所在 GameObject 的同级父节点下
        if (myNode->getParent() != nullptr)
        {
            TransformNode *rootBoneNode = mRootBoneGameObject->getTransformNode();
            if (rootBoneNode != nullptr)
            {
                myNode->getParent()->addChild(rootBoneNode);
            }
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool SkinnedGeometry::play(const String &clipName, bool isLoop)
    {
        T3D_ASSERT(mAnimationPlayer != nullptr);

        mPlaybackID = mAnimationPlayer->playClip(clipName, false, isLoop, mIsGPUSkinning);

        return true;
    }

    //--------------------------------------------------------------------------
    
    bool SkinnedGeometry::stop()
    {
        T3D_ASSERT(mAnimationPlayer != nullptr);
        mAnimationPlayer->stopPlayback(mPlaybackID);
        mPlaybackID = AnimationPlayer::INVALID_ID;
        return true;
    }
    
    //--------------------------------------------------------------------------
}
