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

    bool GameObject::frustumCulling(Camera *camera) const
    {
        // TODO: 暂时都不剔除
        return false;
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
            component = var.convert<Component*>(&ok);
            if (!ok)
            {
                component = nullptr;
                break;
            }

            component->setGameObject(this);
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
