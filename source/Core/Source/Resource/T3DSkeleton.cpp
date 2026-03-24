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

#include "Resource/T3DSkeleton.h"
#include "T3DErrorDef.h"
#include "Component/T3DTransformNode.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SkeletonPtr Skeleton::create(const String &name)
    {
        return T3D_NEW Skeleton(name);
    }

    //--------------------------------------------------------------------------

    SkeletonPtr Skeleton::create(const String &name, GameObjectPtr rootBoneGameObject)
    {
        return T3D_NEW Skeleton(name, rootBoneGameObject);
    }

    //--------------------------------------------------------------------------

    Skeleton::~Skeleton()
    {

    }
    
    //--------------------------------------------------------------------------

    Skeleton::Skeleton(const String &name)
        : Resource(name)
    {
        
    }

    //--------------------------------------------------------------------------

    Skeleton::Skeleton(const String &name, GameObjectPtr rootBoneGameObject)
        : Resource(name)
        , mRootBoneGameObject(rootBoneGameObject)
    {
        
    }
    
    //--------------------------------------------------------------------------

    Resource::Type Skeleton::getType() const
    {
        return Type::kSkeleton;
    }

    //--------------------------------------------------------------------------
    
    ResourcePtr Skeleton::clone() const
    {
        SkeletonPtr skel = create(getName());
        skel->cloneProperties(this);
        return skel;
    }
    
    //--------------------------------------------------------------------------

    void Skeleton::cloneProperties(const Resource *const src)
    {
        Resource::cloneProperties(src);
        
        const Skeleton *skel = static_cast<const Skeleton*>(src);
        mRootBoneGameObject = skel->mRootBoneGameObject;
        mBoneGameObjects = skel->mBoneGameObjects;
        mRootBoneUUID = skel->mRootBoneUUID;
    }

    //--------------------------------------------------------------------------

    TResult Skeleton::onCreate()
    {
        TResult ret = Resource::onCreate();
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Skeleton::onLoad(Archive *archive)
    {
        TResult ret;

        do
        {
            ret = Resource::onLoad(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Skeleton::onUnload()
    {
        TResult ret;

        do
        {
            ret = Resource::onUnload();
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            mBoneGameObjects.clear();
            GameObject::destroy(mRootBoneGameObject);
            mRootBoneGameObject = nullptr;
        } while (false);
        
        return ret;
    }
    //--------------------------------------------------------------------------

    void Skeleton::onPreSave()
    {
        // 序列化前，将骨骼子树收集到扁平表
        if (mRootBoneGameObject == nullptr)
        {
            T3D_LOG_WARNING(LOG_TAG_RESOURCE,
                "Skeleton [%s] onPreSave: mRootBoneGameObject is nullptr, skip collecting hierarchy.",
                getName().c_str());
            return;
        }

        mBoneGameObjects.clear();
        GameObject::collectHierarchy(mRootBoneGameObject, mBoneGameObjects);
        mRootBoneUUID = mRootBoneGameObject->getUUID();
    }

    //--------------------------------------------------------------------------

    void Skeleton::onPostLoad()
    {
        // GameObject 没有重写 onPostLoad()，ReadValue 末尾调用的是 Object::onPostLoad()（空实现）。
        // setupHierarchy() 不会被自动触发，必须在这里手动调用，与 Scene::onPostLoad() 的做法一致。
        for (const auto &item : mBoneGameObjects)
        {
            item.second->setupHierarchy();
        }

        // 通过根节点 UUID 找到根节点并赋值
        const auto it = mBoneGameObjects.find(mRootBoneUUID);
        if (it != mBoneGameObjects.end())
        {
            mRootBoneGameObject = it->second;
            mRootBoneGameObject->getTransformNode()->printHierarchy();
        }
        else
        {
            T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                "Skeleton [%s] onPostLoad: RootBoneUUID [%s] not found in BoneGameObjects.",
                getName().c_str(), mRootBoneUUID.toString().c_str());
            mRootBoneGameObject = nullptr;
        }
    }

    //--------------------------------------------------------------------------
}
