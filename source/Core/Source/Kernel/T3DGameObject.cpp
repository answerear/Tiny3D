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


#include "Kernel/T3DGameObject.h"

#include "Bound/T3DFrustumBound.h"
#include "Component/T3DCamera.h"
#include "Component/T3DRenderable.h"
#include "Component/T3DTransform3D.h"
#include "Component/T3DComponent.h"
#include "Light/T3DLight.h"
#include "Kernel/T3DAgent.h"
#include "Render/T3DRenderPipeline.h"
#include "Resource/T3DScene.h"
#include "Resource/T3DSceneManager.h"
#include "bound/T3DBound.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

// #if defined (T3D_DEBUG)
//     Object *GameObject::acquire()
//     {
//         if (getName() == "Camera")
//         {
//             int a = 0;
//         }
//         return Object::acquire();
//     }
//
//     void GameObject::release()
//     {
//         if (getName() == "Camera")
//         {
//             int a = 0;
//         }
//         Object::release();
//     }
// #endif
    
    //--------------------------------------------------------------------------

    GameObject::WaitingDestroyComponents GameObject::msWaitingDestroyComponents;
    GameObject::WaitingDestroyGameObjects GameObject::msWaitingDestroyGameObjects;
    // GameObject::GameObjects GameObject::msGameObjects;
    
    //--------------------------------------------------------------------------

    void GameObject::destroyComponent(Component *component)
    {
        auto itr = std::find(msWaitingDestroyComponents.begin(), msWaitingDestroyComponents.end(), component);
        if (itr == msWaitingDestroyComponents.end())
        {
            msWaitingDestroyComponents.emplace_back(component);
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::destroyComponents()
    {
        while (!msWaitingDestroyComponents.empty())
        {
            Component *component = msWaitingDestroyComponents.front();
            component->onDestroy();
            component->setGameObject(nullptr);
            msWaitingDestroyComponents.pop_front();
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::destroyGameObjects()
    {
        while (!msWaitingDestroyGameObjects.empty())
        {
            GameObject *go = msWaitingDestroyGameObjects.front();
            go->onDestroy();
            msWaitingDestroyGameObjects.pop_front();
        }
    }

    //--------------------------------------------------------------------------

    GameObjectPtr GameObject::create(const String &name, bool managed)
    {
        return new GameObject(name, managed);
    }

    //--------------------------------------------------------------------------

    GameObjectPtr GameObject::createWithTransform(const String &name, bool managed)
    {
        GameObjectPtr go = create(name, managed);
        if (go != nullptr)
        {
            go->addComponent<Transform3D>();
        }
        return go;
    }
    
    //--------------------------------------------------------------------------

    GameObject::GameObject(const String &name, bool managed)
        : mName(name)
    {
        mUUID = UUID::generate();
        const ComponentSettings &settings = T3D_AGENT.getSettings().componentSettins;
        int32_t i = 0;
        for (auto it = settings.updateOrders.begin(); it != settings.updateOrders.end(); ++it, ++i)
        {
            mUpdateComponents.emplace(i, ComponentList());
        }

        if (managed && T3D_SCENE_MGR.getCurrentScene() != nullptr)
        {
            // msGameObjects.emplace(mUUID, this);
            T3D_SCENE_MGR.getCurrentScene()->addGameObject(this);
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::update()
    {
        TransformNodePtr node = getComponent<TransformNode>();
        if (node != nullptr)
        {
            node->visitActive([](TransformNode *node)
            {
                GameObject *go = node->getGameObject();
                go->onUpdate();
                // node->onUpdate();
                //
                // GameObject *go = node->getGameObject();
                // for (auto component : go->getComponents<Component>())
                // {
                //     if (component != node)
                //     {
                //         component->onUpdate();
                //     }
                // }
            });
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::onUpdate()
    {
        for (auto item : mUpdateComponents)
        {
            for (auto component : item.second)
            {
                component->onUpdate();
            }
        }

        for (auto item : mUpdateComponents2)
        {
            item.second->onUpdate();
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::frustumCulling(Camera *camera, RenderPipeline *pipeline) const
    {
        TransformNodePtr node = getComponent<TransformNode>();
        if (node != nullptr && camera != nullptr)
        {
            FrustumBound *frustum = camera->getGameObject()->getComponent<FrustumBound>();
            node->visitVisible([](TransformNode *node, Camera *camera, FrustumBound *frustum, RenderPipeline *pipeline)
            {
                GameObject *go = node->getGameObject();
                Renderable *renderable = go->getComponent<Renderable>();
                Bound *bound = go->getComponent<Bound>();
                if (renderable != nullptr)
                {
                    if (bound == nullptr || frustum == nullptr)
                    {
                        // 没有包围盒，不剔除，直接渲染
                        pipeline->addRenderable(camera, renderable);
                    }
                    else
                    {
                        // 有包围盒，根据包围盒来判断
                        if (frustum->test(bound))
                        {
                            pipeline->addRenderable(camera, renderable);
                        }
                    }
                }
            },
            camera, frustum, pipeline);
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::setupLights(RenderPipeline *pipeline) const
    {
        TransformNodePtr node = getComponent<TransformNode>();
        if (node != nullptr)
        {
            node->visitVisible([](TransformNode *node, RenderPipeline *pipeline)
            {
                GameObject *go = node->getGameObject();
                Light *light = go->getComponent<Light>();
                if (light != nullptr)
                {
                    pipeline->addLight(light);
                }
            }, pipeline);
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::destroy(GameObject *gameObject)
    {
        TransformNodePtr node = gameObject->getComponent<TransformNode>();
        if (node != nullptr)
        {
            node->reverseVisitAll([](TransformNode *node)
            {
                node->getGameObject()->removeAllComponents();
                destroyGameObject(node->getGameObject());
            });
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::destroyGameObject(GameObject *gameObject)
    {
        auto itr = std::find(msWaitingDestroyGameObjects.begin(), msWaitingDestroyGameObjects.end(), gameObject);
        if (itr == msWaitingDestroyGameObjects.end())
        {
            msWaitingDestroyGameObjects.emplace_back(gameObject);
            // msGameObjects.erase(gameObject->getUUID());
            T3D_SCENE_MGR.getCurrentScene()->removeGameObject(gameObject);
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::onDestroy()
    {
        
    }

    //--------------------------------------------------------------------------

    void GameObject::setupHierarchy()
    {
        TransformNode *node = getComponent<TransformNode>();
        T3D_ASSERT(node != nullptr);
        node->setupHierarchy();
    }
    
    //--------------------------------------------------------------------------

    void GameObject::putUpdatingQueue(const RTTRType &type, Component *component)
    {
        do
        {
            const ComponentSettings &settings = T3D_AGENT.getSettings().componentSettins;
            int32_t i = 0;
            for (auto it = settings.updateOrders.begin(); it != settings.updateOrders.end(); ++it, ++i)
            {
                if (*it == type.get_name())
                {
                    auto itUpdate = mUpdateComponents.find(i);
                    T3D_ASSERT(itUpdate != mUpdateComponents.end());
                    itUpdate->second.emplace_back(component);
                    break;
                }
            }

            int32_t orderCount = (int32_t)settings.updateOrders.size();
            if (i == orderCount)
            {
                // 不在预设序列里面，直接放到乱序更新队列
                mUpdateComponents2.emplace(type.get_name(), component);
            }
        } while (false);
    }

    //--------------------------------------------------------------------------

    void GameObject::setupComponents()
    {
        for (const auto &item : mComponentObjects)
        {
            RTTRType type = RTTRType::get_by_name(item.first);
            mComponents.emplace(type, item.second);
            item.second->setGameObject(this);
            putUpdatingQueue(type, item.second);
            item.second->onStart();
        }
    }

    //--------------------------------------------------------------------------
    
    ComponentPtr GameObject::addComponent(const RTTRType &type)
    {
        ComponentPtr component;
        
        do
        {
            if (!type.is_derived_from<Component>())
            {
                // 不是 Component 子类，无法创建 component
                break;
            }

            if (mTransformNode != nullptr)
            {
                // transform node 子类，不能重复添加
                break;
            }

            TArray<rttr::argument> args;
            args.push_back(UUID::generate());
            rttr::variant var = type.create(args);
            bool ok = false;
            Component *comp = var.convert<Component*>(&ok);
            if (!ok)
            {
                break;
            }

            component = comp;
            component->setGameObject(this);

            // 放入组件对象表里
            mComponents.emplace(type, component);
            mComponentObjects.emplace(type.get_name(), component);

            if (type.is_derived_from<TransformNode>())
            {
                mTransformNode = static_cast<TransformNode*>(comp);
            }

            // 放入组件更新队列
            putUpdatingQueue(type, component);
            
            component->onStart();
        } while (false);

        return component;
    }

    //--------------------------------------------------------------------------

    void GameObject::removeAllComponents()
    {
        for (auto itr = mComponents.begin(); itr != mComponents.end(); ++itr)
        {
            destroyComponent(itr->second);
        }

        mComponents.clear();
        mComponentObjects.clear();
        mUpdateComponents.clear();
        mUpdateComponents2.clear();

        mTransformNode = nullptr;
    }

    //--------------------------------------------------------------------------

    TResult GameObject::removeComponent(const RTTRType &type)
    {
        TResult ret = T3D_OK;

        do
        {
            auto itr = mComponents.find(type);
            if (itr == mComponents.end())
            {
                // 没找到
                ret = T3D_ERR_NOT_FOUND;
                break;
            }

            auto it = mComponentObjects.find(type.get_name().data());
            T3D_ASSERT(it != mComponentObjects.end());

            if (itr->second == mTransformNode)
            {
                mTransformNode = nullptr;
            }
            
            destroyComponent(itr->second);
            mComponents.erase(itr);
            mComponentObjects.erase(it);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GameObject::removeComponents(const RTTRType &type)
    {
        TResult ret = T3D_OK;

        do
        {
            auto range = mComponents.equal_range(type);
            if (range.first == range.second)
            {
                ret = T3D_ERR_NOT_FOUND;
                break;
            }
            
            for (auto itr = range.first; itr != range.second; ++itr)
            {
                if (itr->second == mTransformNode)
                {
                    mTransformNode = nullptr;
                }
                
                destroyComponent(itr->second);
                mComponents.erase(itr);
            }

            mComponentObjects.erase(type.get_name().data());
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    ComponentPtr GameObject::getComponent(const RTTRType &type) const
    {
        ComponentPtr component;

        do
        {
            bool found = false;
            auto itr = mComponents.find(type);
            if (itr == mComponents.end())
            {
                // 没找到，找所有子类
                for (auto t : type.get_derived_classes())
                {
                    auto it = mComponents.find(t);
                    if (it != mComponents.end())
                    {
                        itr = it;
                        found = true;
                        break;
                    }
                }
            }
            else
            {
                found = true;
            }

            if (!found)
            {
                break;
            }
            
            component = itr->second;
        } while (false);

        return component;
    }

    //--------------------------------------------------------------------------

    // TArray<ComponentPtr> GameObject::getComponents(const RTTRType &type) const
    // {
    //     TArray<ComponentPtr> components;
    //     
    //     auto range = mComponents.equal_range(type);
    //     for (auto itr = range.first; itr != range.second; ++itr)
    //     {
    //         components.emplace_back(itr->second);
    //     }
    //
    //     return components;
    // }

    //--------------------------------------------------------------------------
}
