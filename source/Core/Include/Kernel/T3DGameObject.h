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

#ifndef __T3D_GAME_OBJECT_H__
#define __T3D_GAME_OBJECT_H__


#include "Component/T3DComponent.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API GameObject : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        static GameObjectPtr create();
        
        ~GameObject() override = default;

        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="getter")
        const UUID &getUUID() const { return mUUID; }

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        TPROPERTY(RTTRFuncName="Visible", RTTRFuncType="getter")
        bool isVisible() const { return mIsVisible; }

        TPROPERTY(RTTRFuncName="Visible", RTTRFuncType="setter")
        void setVisible(bool visible) { mIsVisible = visible; }

        TPROPERTY(RTTRFuncName="Active", RTTRFuncType="getter")
        bool isActive() const { return mIsActive; }

        TPROPERTY(RTTRFuncName="Active", RTTRFuncType="setter")
        void setActive(bool active) { mIsActive = active; }

        TPROPERTY(RTTRFuncName="CameraMask", RTTRFuncType="getter")
        void setCameraMask(uint32_t mask) { mCameraMask = mask; }

        TPROPERTY(RTTRFuncName="CameraMask", RTTRFuncType="setter")
        uint32_t getCameraMask() const { return mCameraMask; }

        ComponentPtr addComponent(const String &name)
        {
            RTTRType type = RTTRType::get_by_name(name);
            return addComponent(type);
        }

        template <typename T>
        SmartPtr<T> addComponent()
        {
            RTTRType type = RTTRType::get<T>();
            return addComponent(type);
        }

        ComponentPtr getComponent(const String &name) const
        {
            RTTRType type = RTTRType::get_by_name(name);
            return getComponent(type);
        }

        template <typename T>
        SmartPtr<T> getComponent() const
        {
            RTTRType type = RTTRType::get<T>();
            return getComponent(type);
        }

        TArray<ComponentPtr> getComponents(const String &name) const
        {
            RTTRType type = RTTRType::get_by_name(name);
            return getComponents(type);
        }

        template <typename T>
        TArray<SmartPtr<T>> getComponents() const
        {
            RTTRType type = RTTRType::get<T>();
            return getComponents(type);
        }

    protected:
        GameObject() = default;

        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="setter")
        void setUUID(const UUID &uuid) { mUUID = uuid; }

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        ComponentPtr addComponent(const RTTRType &type);

        ComponentPtr getComponent(const RTTRType &type) const;

        TArray<ComponentPtr> getComponents(const RTTRType &type) const;

    protected:
        /// 游戏对象 UUID
        UUID mUUID {};
        /// 游戏对象名称
        String mName {};
        /// 可见性
        bool mIsVisible {true};
        /// 有效性
        bool mIsActive {true};
        /// 相机可见掩码
        uint32_t mCameraMask {0};

        using Components = TMultimap<RTTRType, ComponentPtr>;

        Components  mComponents {};
    };
}


#endif    /*__T3D_GAME_OBJECT_H__*/
