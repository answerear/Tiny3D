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

#include "Component/T3DPrefabInstance.h"
#include "Component/T3DPrefabOverrideUtil.h"
#include "Component/T3DTransformNode.h"
#include "Kernel/T3DGameObject.h"
#include "Resource/T3DPrefab.h"
#include "Resource/T3DPrefabManager.h"
#include "Resource/T3DAssetManager.h"
#include "Resource/T3DScene.h"
#include "Resource/T3DSceneManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    PrefabInstancePtr PrefabInstance::create()
    {
        return T3D_NEW PrefabInstance(UUID::generate());
    }

    //--------------------------------------------------------------------------

    PrefabInstance::PrefabInstance(const UUID &uuid)
        : Component(uuid)
    {
    }

    //--------------------------------------------------------------------------

    ComponentPtr PrefabInstance::clone() const
    {
        PrefabInstancePtr inst = create();
        if (inst != nullptr)
        {
            TResult ret = inst->cloneProperties(this);
            if (T3D_FAILED(ret))
            {
                inst = nullptr;
            }
        }
        return inst;
    }

    //--------------------------------------------------------------------------

    TResult PrefabInstance::cloneProperties(const Component * const src)
    {
        TResult ret = Component::cloneProperties(src);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        const PrefabInstance *srcInst = static_cast<const PrefabInstance *>(src);
        mSourcePrefabUUID = srcInst->mSourcePrefabUUID;
        mModifications = srcInst->mModifications;
        mMappedTemplateUUIDs = srcInst->mMappedTemplateUUIDs;
        mMappedInstanceUUIDs = srcInst->mMappedInstanceUUIDs;
        // 运行时 Object* 映射不拷贝，由 instantiate / rebuild 重新建立
        mTemplateToInstance.clear();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    Object *PrefabInstance::resolveInstanceObject(const UUID &templateUUID) const
    {
        const auto it = mTemplateToInstance.find(templateUUID);
        return it != mTemplateToInstance.end() ? it->second : nullptr;
    }

    //--------------------------------------------------------------------------

    void PrefabInstance::setTemplateToInstanceMap(const TemplateToInstanceMap &map)
    {
        mTemplateToInstance = map;
        mMappedTemplateUUIDs.clear();
        mMappedInstanceUUIDs.clear();
        mMappedTemplateUUIDs.reserve(map.size());
        mMappedInstanceUUIDs.reserve(map.size());

        for (const auto &item : map)
        {
            mMappedTemplateUUIDs.push_back(item.first);
            Object *obj = item.second;
            UUID instanceUUID = UUID::INVALID;
            if (GameObject *go = dynamic_cast<GameObject*>(obj))
            {
                instanceUUID = go->getUUID();
            }
            else if (Component *comp = dynamic_cast<Component*>(obj))
            {
                instanceUUID = comp->getUUID();
            }
            mMappedInstanceUUIDs.push_back(instanceUUID);
        }
    }

    //--------------------------------------------------------------------------

    void PrefabInstance::clearTemplateToInstanceMap()
    {
        mTemplateToInstance.clear();
        mMappedTemplateUUIDs.clear();
        mMappedInstanceUUIDs.clear();
    }

    //--------------------------------------------------------------------------

    void PrefabInstance::rebuildRuntimeMapFromSerializedUUIDs()
    {
        mTemplateToInstance.clear();
        if (mGameObject == nullptr)
        {
            return;
        }

        GameObjects objs;
        GameObject::collectHierarchy(mGameObject, objs);

        const size_t count = (mMappedTemplateUUIDs.size() < mMappedInstanceUUIDs.size())
            ? mMappedTemplateUUIDs.size() : mMappedInstanceUUIDs.size();
        for (size_t i = 0; i < count; ++i)
        {
            const UUID &templateUUID = mMappedTemplateUUIDs[i];
            const UUID &instanceUUID = mMappedInstanceUUIDs[i];

            Object *obj = nullptr;
            const auto goIt = objs.find(instanceUUID);
            if (goIt != objs.end())
            {
                obj = goIt->second.get();
            }
            else
            {
                for (const auto &item : objs)
                {
                    for (const auto &citem : item.second->getAllComponents())
                    {
                        if (citem.second != nullptr && citem.second->getUUID() == instanceUUID)
                        {
                            obj = citem.second.get();
                            break;
                        }
                    }
                    if (obj != nullptr)
                    {
                        break;
                    }
                }
            }

            if (obj != nullptr)
            {
                mTemplateToInstance[templateUUID] = obj;
            }
        }
    }

    //--------------------------------------------------------------------------

    void PrefabInstance::recordOverride(const PropertyModification &mod)
    {
        clearOverride(mod.getTargetObjectUUID(), mod.getPropertyPath());
        mModifications.push_back(mod);
    }

    //--------------------------------------------------------------------------

    void PrefabInstance::clearOverride(const UUID &targetObjectUUID, const String &propertyPath)
    {
        for (auto it = mModifications.begin(); it != mModifications.end(); )
        {
            if (it->getTargetObjectUUID() == targetObjectUUID
                && it->getPropertyPath() == propertyPath)
            {
                it = mModifications.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    //--------------------------------------------------------------------------

    TResult PrefabInstance::applyOverridesToInstance()
    {
        if (mTemplateToInstance.empty())
        {
            rebuildRuntimeMapFromSerializedUUIDs();
        }
        return PrefabOverrideUtil::applyModifications(this, mModifications);
    }

    //--------------------------------------------------------------------------

    Prefab *PrefabInstance::resolveSourcePrefab(PrefabPtr &holder) const
    {
        holder = nullptr;

        if (mSourcePrefabUUID == UUID::INVALID)
        {
            return nullptr;
        }

        // 优先走缓存，避免在不支持 UUID 寻址的档案上做无谓读档
        Resource *res = T3D_PREFAB_MGR.getResource(mSourcePrefabUUID);
        if (res != nullptr && res->getType() == Resource::Type::kPrefab)
        {
            return static_cast<Prefab*>(res);
        }

        holder = T3D_ASSET_MGR.loadPrefab(mSourcePrefabUUID);
        return holder.get();
    }

    //--------------------------------------------------------------------------

    TResult PrefabInstance::recordOverridesFromSource()
    {
        PrefabPtr prefabHold;
        Prefab *prefab = resolveSourcePrefab(prefabHold);

        if (prefab == nullptr)
        {
            return T3D_ERR_NOT_FOUND;
        }

        if (mTemplateToInstance.empty())
        {
            rebuildRuntimeMapFromSerializedUUIDs();
        }

        PropertyModifications mods;
        TResult ret = PrefabOverrideUtil::collectOverrides(prefab, this, mods);
        if (T3D_SUCCEEDED(ret))
        {
            mModifications = mods;
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    void PrefabInstance::rebuildTemplateMapFromHierarchy()
    {
        mTemplateToInstance.clear();
    }

#if defined(T3D_EDITOR)
    //--------------------------------------------------------------------------

    TResult PrefabInstance::applyToPrefabAsset()
    {
        PrefabPtr prefabHold;
        Prefab *prefab = resolveSourcePrefab(prefabHold);
        if (prefab == nullptr || prefab->getRootGameObject() == nullptr)
        {
            return T3D_ERR_NOT_FOUND;
        }

        for (const auto &mod : mModifications)
        {
            Object *templateObj = nullptr;
            GameObject *go = PrefabOverrideUtil::findGameObjectByTemplateUUID(
                prefab->getRootGameObject(), mod.getTargetObjectUUID());
            if (go != nullptr)
            {
                templateObj = go;
            }
            else
            {
                Component *comp = PrefabOverrideUtil::findComponentByTemplateUUID(
                    prefab->getRootGameObject(), mod.getTargetObjectUUID());
                templateObj = comp;
            }

            if (templateObj != nullptr)
            {
                PrefabOverrideUtil::writeModificationToObject(templateObj, mod);
            }
        }

        Archive *archive = T3D_ASSET_MGR.getArchive();
        if (archive == nullptr)
        {
            return T3D_ERR_FAIL;
        }

        const String &filename = prefab->getFilename().empty()
            ? prefab->getName() : prefab->getFilename();
        return T3D_PREFAB_MGR.savePrefab(archive, filename, prefab);
    }

    //--------------------------------------------------------------------------

    TResult PrefabInstance::revert(const UUID &targetObjectUUID, const String &propertyPath)
    {
        PrefabPtr prefabHold;
        Prefab *prefab = resolveSourcePrefab(prefabHold);
        if (prefab == nullptr)
        {
            return T3D_ERR_NOT_FOUND;
        }

        Object *templateObj = PrefabOverrideUtil::findGameObjectByTemplateUUID(
            prefab->getRootGameObject(), targetObjectUUID);
        if (templateObj == nullptr)
        {
            templateObj = PrefabOverrideUtil::findComponentByTemplateUUID(
                prefab->getRootGameObject(), targetObjectUUID);
        }

        Object *instanceObj = resolveInstanceObject(targetObjectUUID);
        if (templateObj == nullptr || instanceObj == nullptr)
        {
            clearOverride(targetObjectUUID, propertyPath);
            return T3D_ERR_NOT_FOUND;
        }

        rttr::instance srcInst(*templateObj);
        rttr::instance dstInst(*instanceObj);
        rttr::property prop = srcInst.get_derived_type().get_property(propertyPath);
        if (prop.is_valid())
        {
            RTTRVariant val = prop.get_value(srcInst);
            prop.set_value(dstInst, val);
        }

        clearOverride(targetObjectUUID, propertyPath);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PrefabInstance::revertAll()
    {
        PropertyModifications mods = mModifications;
        for (const auto &mod : mods)
        {
            revert(mod.getTargetObjectUUID(), mod.getPropertyPath());
        }
        mModifications.clear();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PrefabInstance::unpackPrefab(GameObject *instanceRoot, UnpackMode mode)
    {
        if (instanceRoot == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        if (instanceRoot->getComponent<PrefabInstance>() != nullptr)
        {
            instanceRoot->removeComponent<PrefabInstance>();
        }

        if (mode == UnpackMode::kCompletely)
        {
            GameObjects objs;
            GameObject::collectHierarchy(instanceRoot, objs);
            for (const auto &item : objs)
            {
                if (item.second.get() == instanceRoot)
                {
                    continue;
                }
                if (item.second->getComponent<PrefabInstance>() != nullptr)
                {
                    item.second->removeComponent<PrefabInstance>();
                }
            }
        }

        return T3D_OK;
    }
#endif

    //--------------------------------------------------------------------------
}