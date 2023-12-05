/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "Kernel/T3DGameObject.h"
#include "Component/T3DRenderable.h"
#include "Component/T3DTransformNode.h"


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
                node->update();

                GameObject *go = node->getGameObject();
                for (auto component : go->getComponents<Component>())
                {
                    if (component != node)
                    {
                        component->update();
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
                RenderablePtr renderable = go->getComponent<Renderable>();
                if (renderable != nullptr)
                {
                    // TODO : 暂时不剔除
                    pipeline->addRenderable(camera, renderable);
                }
            },
            camera, pipeline);
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
        } while (false);

        return component;
    }

    //--------------------------------------------------------------------------

    ComponentPtr GameObject::getComponent(const RTTRType &type) const
    {
        ComponentPtr component;

        do
        {
            auto itr = mComponents.find(type);
            if (itr == mComponents.end())
            {
                // 没找到
                break;
            }

            component = itr->second;
        } while (false);

        return component;
    }

    //--------------------------------------------------------------------------

    TArray<ComponentPtr> GameObject::getComponents(const RTTRType &type) const
    {
        TArray<ComponentPtr> components;
        
        auto range = mComponents.equal_range(type);
        for (auto itr = range.first; itr != range.second; ++itr)
        {
            components.emplace_back(itr->second);
        }

        return components;
    }

    //--------------------------------------------------------------------------
}
