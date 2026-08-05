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
#include "Component/T3DPrefabInstance.h"
#include "Component/T3DTransformNode.h"
#include "Component/T3DTransform3D.h"
#include "Resource/T3DScene.h"
#include "Resource/T3DAssetManager.h"
#include "Resource/T3DPrefabManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    PrefabPtr Prefab::create(const String &name)
    {
        return T3D_NEW Prefab(name);
    }

    //--------------------------------------------------------------------------

    PrefabPtr Prefab::create(const String &name, GameObjectPtr root)
    {
        return T3D_NEW Prefab(name, root);
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

    Prefab::Prefab(const String &name, GameObjectPtr root)
        : Resource(name)
        , mRootGameObject(root)
    {
        if (root != nullptr)
        {
            mRootGameObjectUUID = root->getUUID();
        }
    }

    //--------------------------------------------------------------------------

    Resource::Type Prefab::getType() const
    {
        return Type::kPrefab;
    }

    //--------------------------------------------------------------------------

    void Prefab::setRootGameObject(GameObjectPtr root)
    {
        mRootGameObject = root;
        if (root != nullptr)
        {
            mRootGameObjectUUID = root->getUUID();
        }
        else
        {
            mRootGameObjectUUID = UUID::INVALID;
        }
    }

    //--------------------------------------------------------------------------

    void Prefab::setupPrefabInstanceLink(GameObject *instanceRoot,
        const GameObject::TemplateInstanceMap &map) const
    {
        if (instanceRoot == nullptr)
        {
            return;
        }

        PrefabInstancePtr link = instanceRoot->getComponent<PrefabInstance>();
        if (link == nullptr)
        {
            link = instanceRoot->addComponent<PrefabInstance>();
        }

        if (link != nullptr)
        {
            link->setSourcePrefabUUID(getUUID());
            PrefabInstance::TemplateToInstanceMap instMap;
            for (const auto &item : map)
            {
                instMap[item.first] = item.second;
            }
            // PrefabInstance 自身不在模板映射中（后挂）
            link->setTemplateToInstanceMap(instMap);
        }
    }

    //--------------------------------------------------------------------------

    GameObjectPtr Prefab::instantiateInternal(bool linked) const
    {
        if (mRootGameObject == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                "Prefab [%s] instantiate: mRootGameObject is nullptr, cannot instantiate.",
                getName().c_str());
            return nullptr;
        }

        GameObject::TemplateInstanceMap map;
        GameObjectPtr root = mRootGameObject->clone(linked ? &map : nullptr);
        if (root == nullptr)
        {
            return nullptr;
        }

        if (linked)
        {
            setupPrefabInstanceLink(root.get(), map);
        }

        return root;
    }

    //--------------------------------------------------------------------------

    GameObjectPtr Prefab::instantiate() const
    {
        return instantiateInternal(true);
    }

    //--------------------------------------------------------------------------

    GameObjectPtr Prefab::instantiateUnlinked() const
    {
        return instantiateInternal(false);
    }

    //--------------------------------------------------------------------------

    GameObjectPtr Prefab::instantiate(Scene *scene, TransformNode *parent) const
    {
        if (scene == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                "Prefab [%s] instantiate: scene is nullptr.", getName().c_str());
            return nullptr;
        }

        GameObjectPtr root = instantiateInternal(true);
        if (root == nullptr)
        {
            return nullptr;
        }

        GameObjects objs;
        GameObject::collectHierarchy(root.get(), objs);
        for (const auto &item : objs)
        {
            scene->addGameObject(item.second.get());
        }

        TransformNode *node = root->getTransformNode();
        TransformNode *attachParent = parent;
        if (attachParent == nullptr)
        {
            attachParent = scene->getRootTransform();
        }

        if (node != nullptr && attachParent != nullptr)
        {
            attachParent->addChild(node);
        }

        scene->awakeHierarchy(root.get());
        return root;
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
        mBasePrefabUUID = prefab->mBasePrefabUUID;
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

    TResult Prefab::onUnload()
    {
        // GameObject 与其组件、TransformNode 的父子之间都是智能指针互持，不显式
        // 销毁就会留下引用环，与 Scene::onUnload 的处理保持一致。
        if (mRootGameObject != nullptr)
        {
            GameObject::destroy(mRootGameObject);
            mRootGameObject = nullptr;
        }

        // 扁平表里可能残留不在根子树上的节点（如加载出错的资产），一并销毁
        for (const auto &item : mGameObjects)
        {
            GameObject::destroy(item.second.get());
        }

        mGameObjects.clear();
        mNeedToLoadResourceComponents.clear();

        return Resource::onUnload();
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

        // Variant：若有基 Prefab，先从基实例化再应用本资产内嵌的差异（一期简化：仅记录 base UUID，
        // 完整 Variant 合并在二期由编辑器工作流完善；加载时保证 base 可解析）。
        if (isVariant() && mRootGameObject != nullptr)
        {
            // 先查缓存，未命中才读档；FileSystem 这类档案不支持 UUID 寻址
            Resource *base = T3D_PREFAB_MGR.getResource(mBasePrefabUUID);
            PrefabPtr baseHold;
            if (base == nullptr)
            {
                baseHold = T3D_ASSET_MGR.loadPrefab(mBasePrefabUUID);
                base = baseHold.get();
            }

            if (base == nullptr)
            {
                T3D_LOG_WARNING(LOG_TAG_RESOURCE,
                    "Prefab Variant [%s] base UUID [%s] not found.",
                    getName().c_str(), mBasePrefabUUID.toString().c_str());
            }
        }
    }

    //--------------------------------------------------------------------------

    void Prefab::onAddComponentForLoadingResource(Component *component)
    {
        mNeedToLoadResourceComponents.emplace(component);
    }

    //--------------------------------------------------------------------------
}