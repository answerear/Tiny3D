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
#include "Component/T3DRenderable.h"
#include "Component/T3DTransformNode.h"
#include "Component/T3DComponent.h"
#include "Render/T3DRenderPipeline.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GameObjectPtr GameObject::create(const String &name)
    {
        return new GameObject(name);
    }

    //--------------------------------------------------------------------------

    GameObject::GameObject(const String &name)
        : mName(name)
    {
        
    }

    //--------------------------------------------------------------------------

    void GameObject::update()
    {
        TransformNodePtr node = getComponent<TransformNode>();
        if (node != nullptr)
        {
            node->visitActive([](TransformNode *node)
            {
                node->onUpdate();

                GameObject *go = node->getGameObject();
                for (auto component : go->getComponents<Component>())
                {
                    if (component != node)
                    {
                        component->onUpdate();
                    }
                }
            });
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::frustumCulling(Camera *camera, RenderPipeline *pipeline) const
    {
        TransformNodePtr node = getComponent<TransformNode>();
        if (node != nullptr)
        {
            node->visitVisible([](TransformNode *node, Camera *camera, RenderPipeline *pipeline)
            {
                GameObject *go = node->getGameObject();
                TArray<RenderablePtr> renderables = go->getComponents<Renderable>();
                for (const auto &renderable : renderables)
                {
                    if (renderable != nullptr)
                    {
                        // TODO : 暂时不剔除
                        pipeline->addRenderable(camera, renderable);
                    }
                }
            },
            camera, pipeline);
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::destroy()
    {
        TransformNodePtr node = getComponent<TransformNode>();
        if (node != nullptr)
        {
            node->reverseVisitAll([](TransformNode *node)
            {
                node->getGameObject()->onDestroy();
            });
        }
    }

    //--------------------------------------------------------------------------

    void GameObject::onDestroy()
    {
        removeAllComponents();
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
            
            mComponents.emplace(type, component);
            
            component->onStart();
        } while (false);

        return component;
    }

    //--------------------------------------------------------------------------

    void GameObject::removeAllComponents()
    {
        for (auto itr = mComponents.begin(); itr != mComponents.end(); ++itr)
        {
            itr->second->destroy();
        }

        mComponents.clear();
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

            itr->second->destroy();
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
                itr->second->destroy();
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
