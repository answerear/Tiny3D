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
#include "Component/T3DTransformNode.h"
#include "Component/T3DComponent.h"
#include "Kernel/T3DAgent.h"
#include "Render/T3DRenderPipeline.h"
#include "bound/T3DBound.h"

namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GameObject::WaitingDestroyComponents GameObject::mWaitingDestroyComponents;
    GameObject::WaitingDestroyGameObjects GameObject::mWaitingDestroyGameObjects;
    
    //--------------------------------------------------------------------------

    void GameObject::destroyComponent(Component *component)
    {
        auto itr = std::find(mWaitingDestroyComponents.begin(), mWaitingDestroyComponents.end(), component);
        if (itr == mWaitingDestroyComponents.end())
        {
            mWaitingDestroyComponents.emplace_back(component);
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::destroyComponents()
    {
        while (!mWaitingDestroyComponents.empty())
        {
            Component *component = mWaitingDestroyComponents.front();
            component->onDestroy();
            component->setGameObject(nullptr);
            mWaitingDestroyComponents.pop_front();
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::destroyGameObjects()
    {
        while (!mWaitingDestroyGameObjects.empty())
        {
            GameObject *go = mWaitingDestroyGameObjects.front();
            go->onDestroy();
            mWaitingDestroyGameObjects.pop_front();
        }
    }

    //--------------------------------------------------------------------------

    GameObjectPtr GameObject::create(const String &name)
    {
        return new GameObject(name);
    }

    //--------------------------------------------------------------------------

    GameObject::GameObject(const String &name)
        : mName(name)
    {
        const ComponentSettings &settings = T3D_AGENT.getSettings().componentSettins;
        int32_t i = 0;
        for (auto it = settings.updateOrders.begin(); it != settings.updateOrders.end(); ++it, ++i)
        {
            mUpdateComponents.emplace(i, ComponentList());
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
        auto itr = std::find(mWaitingDestroyGameObjects.begin(), mWaitingDestroyGameObjects.end(), gameObject);
        if (itr == mWaitingDestroyGameObjects.end())
        {
            mWaitingDestroyGameObjects.emplace_back(gameObject);
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::onDestroy()
    {
        
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

            rttr::variant var = type.create();
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

            // 放入组件更新队列
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
        mUpdateComponents.clear();
        mUpdateComponents2.clear();
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

            destroyComponent(itr->second);
            mComponents.erase(itr);
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
                destroyComponent(itr->second);
                mComponents.erase(itr);
            }
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
