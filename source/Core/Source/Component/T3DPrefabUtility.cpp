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

#include "Component/T3DPrefabUtility.h"

#if defined(T3D_EDITOR)

#include "Component/T3DPrefabInstance.h"
#include "Component/T3DTransform3D.h"
#include "Kernel/T3DGameObject.h"
#include "Resource/T3DPrefab.h"
#include "Resource/T3DAssetManager.h"
#include "Resource/T3DScene.h"
#include "Resource/T3DSceneManager.h"


namespace Tiny3D
{
    namespace
    {
        PrefabPtr sPrefabModePrefab;
        GameObjectPtr sPrefabModeRoot;
        ScenePtr sPrefabModeScene;
    }

    //--------------------------------------------------------------------------

    PrefabPtr PrefabUtility::createPrefab(GameObject *root, const String &prefabPath)
    {
        if (root == nullptr || prefabPath.empty())
        {
            return nullptr;
        }

        PrefabPtr prefab = T3D_ASSET_MGR.createPrefab(prefabPath, root);
        if (prefab == nullptr)
        {
            return nullptr;
        }

        TResult ret = T3D_ASSET_MGR.savePrefab(prefabPath, prefab.get());
        if (T3D_FAILED(ret))
        {
            return nullptr;
        }

        // 将原物体转为链接实例：挂 PrefabInstance 并建立映射（自身 UUID 既是模板也是实例）
        PrefabInstancePtr link = root->getComponent<PrefabInstance>();
        if (link == nullptr)
        {
            link = root->addComponent<PrefabInstance>();
        }
        if (link != nullptr)
        {
            link->setSourcePrefabUUID(prefab->getUUID());
            GameObject::TemplateInstanceMap map;
            GameObjects objs;
            GameObject::collectHierarchy(root, objs);
            for (const auto &item : objs)
            {
                map[item.first] = item.second.get();
                for (const auto &citem : item.second->getAllComponents())
                {
                    if (citem.second != nullptr)
                    {
                        map[citem.second->getUUID()] = citem.second.get();
                    }
                }
            }
            PrefabInstance::TemplateToInstanceMap instMap;
            for (const auto &item : map)
            {
                instMap[item.first] = item.second;
            }
            link->setTemplateToInstanceMap(instMap);
            link->clearAllOverrides();
        }

        return prefab;
    }

    //--------------------------------------------------------------------------

    PrefabPtr PrefabUtility::createPrefabVariant(Prefab *basePrefab, const String &variantPath)
    {
        if (basePrefab == nullptr || variantPath.empty())
        {
            return nullptr;
        }

        GameObjectPtr rootClone = basePrefab->instantiateUnlinked();
        if (rootClone == nullptr)
        {
            return nullptr;
        }

        PrefabPtr variant = T3D_ASSET_MGR.createPrefab(variantPath, rootClone);
        if (variant == nullptr)
        {
            return nullptr;
        }

        variant->setBasePrefabUUID(basePrefab->getUUID());
        TResult ret = T3D_ASSET_MGR.savePrefab(variantPath, variant.get());
        if (T3D_FAILED(ret))
        {
            return nullptr;
        }
        return variant;
    }

    //--------------------------------------------------------------------------

    GameObject *PrefabUtility::getPrefabRoot(GameObject *go)
    {
        while (go != nullptr)
        {
            if (go->getComponent<PrefabInstance>() != nullptr)
            {
                return go;
            }
            TransformNode *node = go->getTransformNode();
            if (node == nullptr || node->getParent() == nullptr)
            {
                break;
            }
            go = node->getParent()->getGameObject();
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult PrefabUtility::enterPrefabMode(Prefab *prefab)
    {
        if (prefab == nullptr || prefab->getRootGameObject() == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        if (isInPrefabMode())
        {
            exitPrefabMode(false);
        }

        sPrefabModeScene = T3D_SCENE_MGR.createScene("__PrefabMode__");
        if (sPrefabModeScene == nullptr)
        {
            return T3D_ERR_FAIL;
        }
        sPrefabModeScene->init();

        sPrefabModeRoot = prefab->instantiateUnlinked();
        if (sPrefabModeRoot == nullptr)
        {
            return T3D_ERR_FAIL;
        }

        GameObjects objs;
        GameObject::collectHierarchy(sPrefabModeRoot.get(), objs);
        for (const auto &item : objs)
        {
            sPrefabModeScene->addGameObject(item.second.get());
        }

        if (sPrefabModeRoot->getTransformNode() != nullptr
            && sPrefabModeScene->getRootTransform() != nullptr)
        {
            sPrefabModeScene->getRootTransform()->addChild(
                sPrefabModeRoot->getTransformNode());
        }

        sPrefabModePrefab = PrefabPtr(prefab);
        T3D_SCENE_MGR.setCurrentScene(sPrefabModeScene);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PrefabUtility::exitPrefabMode(bool save)
    {
        if (!isInPrefabMode())
        {
            return T3D_OK;
        }

        TResult ret = T3D_OK;
        if (save && sPrefabModePrefab != nullptr && sPrefabModeRoot != nullptr)
        {
            sPrefabModePrefab->setRootGameObject(sPrefabModeRoot);
            const String &filename = sPrefabModePrefab->getFilename().empty()
                ? sPrefabModePrefab->getName() : sPrefabModePrefab->getFilename();
            ret = T3D_ASSET_MGR.savePrefab(filename, sPrefabModePrefab.get());
        }

        sPrefabModeRoot = nullptr;
        sPrefabModePrefab = nullptr;
        if (sPrefabModeScene != nullptr)
        {
            T3D_SCENE_MGR.unloadScene(sPrefabModeScene);
            sPrefabModeScene = nullptr;
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    bool PrefabUtility::isInPrefabMode()
    {
        return sPrefabModePrefab != nullptr;
    }

    //--------------------------------------------------------------------------

    Prefab *PrefabUtility::getPrefabModePrefab()
    {
        return sPrefabModePrefab.get();
    }

    //--------------------------------------------------------------------------
}

#endif  /* T3D_EDITOR */