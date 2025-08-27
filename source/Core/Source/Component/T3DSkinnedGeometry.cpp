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
#include "Animation/T3DAnimationPlayer.h"
#include "Resource/T3DSkinnedMesh.h"
#include "T3DErrorDef.h"
#include "Component/T3DTransform3D.h"


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
        Geometry::onDestroy();
    }

    //--------------------------------------------------------------------------

    ComponentPtr SkinnedGeometry::clone() const
    {
        SkinnedGeometryPtr geometry = new SkinnedGeometry();
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
        GameObject *root = getGameObject();

        SkinnedMesh *skinnedMesh = static_cast<SkinnedMesh *>(getMeshObject());
        if (skinnedMesh == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_COMPONENT, "SkinnedGeometry::populateAllChildren failed. Skinned Mesh is nullptr !");
            return T3D_ERR_RES_INVALID_OBJECT;
        }

        mBoneGameObjects.clear();
        mBoneGameObjects.resize(skinnedMesh->getBones().size(), nullptr);

        mAllBones.clear();

        // 先构建所有 game object 对象
        for (size_t i = 0; i < skinnedMesh->getBones().size(); i++)
        {
            const auto &bone = skinnedMesh->getBones()[i];
            GameObjectPtr go = GameObject::create(bone->getName());
            Transform3DPtr xform = smart_pointer_cast<Transform3D>(go->addComponent<Transform3D>());
            xform->setPosition(bone->getTranslation());
            xform->setOrientation(bone->getRotation());
            xform->setScaling(bone->getScaling());
            mBoneGameObjects[i] = go;
            mAllBones.emplace(bone->getName(), go);
        }

        // 根据骨骼层级结构来构建 game object 层级结构
        for (size_t i = 0; i < skinnedMesh->getBones().size(); i++)
        {
            const auto &bone = skinnedMesh->getBones()[i];
            GameObjectPtr go = mBoneGameObjects[i];
            Transform3D *node = static_cast<Transform3D *>(go->getTransformNode());
            uint16_t parentIdx = bone->getParentIndex();
            if (parentIdx != BoneNode::kInvalidIndex)
            {
                GameObjectPtr parentGO = mBoneGameObjects[parentIdx];
                Transform3D *parentNode = static_cast<Transform3D *>(parentGO->getTransformNode());
                parentNode->addChild(node);
            }
            else
            {
                root->getTransformNode()->addChild(node);
            }
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool SkinnedGeometry::play(const String &clipName)
    {
        T3D_ASSERT(mAnimationPlayer != nullptr);

        mAnimationPlayer->playClip(clipName, false, false);

        return true;
    }

    //--------------------------------------------------------------------------
}
