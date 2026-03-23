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

#include "Resource/T3DPrefab.h"
#include "Component/T3DComponent.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    PrefabPtr Prefab::create(const String &name)
    {
        return T3D_NEW Prefab(name);
    }

    //--------------------------------------------------------------------------

    Prefab::~Prefab()
    {
    }

    //--------------------------------------------------------------------------

    Prefab::Prefab(const String &name)
        : Resource(name)
    {
        
    }

    //--------------------------------------------------------------------------

    Resource::Type Prefab::getType() const
    {
        return Type::kPrefab;
    }

    //--------------------------------------------------------------------------

    GameObjectPtr Prefab::instantiate() const
    {
        if (mRootGameObject == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                "Prefab [%s] instantiate: mRootGameObject is nullptr, cannot instantiate.",
                getName().c_str());
            return nullptr;
        }

        return mRootGameObject->clone();
    }

    //--------------------------------------------------------------------------
    
    ResourcePtr Prefab::clone() const
    {
        PrefabPtr prefab = create(getName());
        prefab->cloneProperties(this);
        return prefab;
    }
    
    //--------------------------------------------------------------------------

    void Prefab::cloneProperties(const Resource *const src)
    {
        Resource::cloneProperties(src);

        const Prefab *prefab = static_cast<const Prefab*>(src);
        mGameObjects = prefab->mGameObjects;
        mRootGameObjectUUID = prefab->mRootGameObjectUUID;
        mRootGameObject = prefab->mRootGameObject;
    }

    //--------------------------------------------------------------------------

    TResult Prefab::onLoad(Archive *archive)
    {
        TResult ret = Resource::onLoad(archive);

        if (T3D_SUCCEEDED(ret))
        {
            for (auto component : mNeedToLoadResourceComponents)
            {
                component->onLoadResource(archive);
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void Prefab::onPreSave()
    {
        // 序列化前，将预制体子树收集到扁平表
        if (mRootGameObject == nullptr)
        {
            T3D_LOG_WARNING(LOG_TAG_RESOURCE,
                "Prefab [%s] onPreSave: mRootGameObject is nullptr, skip collecting hierarchy.",
                getName().c_str());
            return;
        }

        mGameObjects.clear();
        GameObject::collectHierarchy(mRootGameObject, mGameObjects);
        mRootGameObjectUUID = mRootGameObject->getUUID();
    }

    //--------------------------------------------------------------------------

    void Prefab::onPostLoad()
    {
        // GameObject 没有重写 onPostLoad()，ReadValue 末尾调用的是 Object::onPostLoad()（空实现）。
        // setupHierarchy() 不会被自动触发，必须在这里手动调用，与 Scene::onPostLoad() 的做法一致。
        for (const auto &item : mGameObjects)
        {
            item.second->setupHierarchy();
        }

        // 通过根节点 UUID 找到根节点并赋值
        const auto it = mGameObjects.find(mRootGameObjectUUID);
        if (it != mGameObjects.end())
        {
            mRootGameObject = it->second;
        }
        else
        {
            T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                "Prefab [%s] onPostLoad: RootGameObjectUUID [%s] not found in GameObjects.",
                getName().c_str(), mRootGameObjectUUID.toString().c_str());
            mRootGameObject = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    void Prefab::onAddComponentForLoadingResource(Component *component)
    {
        mNeedToLoadResourceComponents.emplace(component);
    }

    //--------------------------------------------------------------------------
}
