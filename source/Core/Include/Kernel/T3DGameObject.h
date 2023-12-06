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
#include "Render/T3DRenderPipeline.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API GameObject : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        static GameObjectPtr create(const String &name);
        
        ~GameObject() override = default;

        virtual void update();

        virtual void frustumCulling(Camera *camera, RenderPipeline *pipeline) const;

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

        TPROPERTY(RTTRFuncName="CameraMask", RTTRFuncType="setter")
        void setCameraMask(uint32_t mask) { mCameraMask = mask; }

        TPROPERTY(RTTRFuncName="CameraMask", RTTRFuncType="getter")
        uint32_t getCameraMask() const { return mCameraMask; }

        void destroy();

        /**
         * \brief 添加指定类名的组件
         * \param [in] name : 组件类名
         * \return 返回新增的组件对象
         */
        ComponentPtr addComponent(const String &name)
        {
            RTTRType type = RTTRType::get_by_name(name);
            return addComponent(type);
        }

        /**
         * \brief 添加指定类型的组件
         * \tparam [in] T : 组件类型，必须是 Component 子类
         * \return 返回新增的组件对象
         */
        template <typename T>
        SmartPtr<T> addComponent()
        {
            RTTRType type = RTTRType::get<T>();
            return smart_pointer_cast<T>(addComponent(type));
        }

        /**
         * \brief 移除指定类名的组件对象，仅移除第一个符合类名的组件
         * \param [in] name : 组件类名
         * \return 调用成功返回 T3D_OK
         */
        TResult removeComponent(const String &name)
        {
            RTTRType type = RTTRType::get_by_name(name);
            return removeComponent(type);
        }

        /**
         * \brief 移除指定类型组件对象，仅移除第一个符合类名的组件
         * \tparam [in] T : 组件类型
         * \return 调用成功返回 T3D_OK
         */
        template <typename T>
        TResult removeComponent()
        {
            RTTRType type = RTTRType::get<T>();
            return removeComponent(type);
        }

        /**
         * \brief 移除所有对应类型的组件对象
         * \param [in] name : 组件类名 
         * \return 调用成功返回 T3D_OK
         */
        TResult removeComponents(const String &name)
        {
            RTTRType type = RTTRType::get_by_name(name);
            return removeComponents(type);
        }

        /**
         * \brief 移除所有对应类型的组件对象
         * \tparam [in] T : 组件类型
         * \return 调用成功返回 T3D_OK
         */
        template <typename T>
        TResult removeComponents()
        {
            RTTRType type = RTTRType::get<T>();
            return removeComponents(type);
        }

        void removeAllComponents();

        /**
         * \brief 获取指定类型名的组件对象，仅获取到第一个匹配类名的
         * \param [in] name : 组件类名
         * \return 调用成功返回组件对象，否则返回 nullptr
         */
        ComponentPtr getComponent(const String &name) const
        {
            RTTRType type = RTTRType::get_by_name(name);
            return getComponent(type);
        }

        /**
         * \brief 获取指定类型名的组件对象，仅获取到第一个匹配类名的
         * \tparam [in] T : 组件类型
         * \return 调用成功返回组件对象，否则返回 nullptr
         */
        template <typename T>
        SmartPtr<T> getComponent() const
        {
            RTTRType type = RTTRType::get<T>();
            return smart_pointer_cast<T>(getComponent(type));
        }

        /**
         * \brief 获取指定类型名的所有组件对象
         * \param [in] name : 组件类名
         * \return 返回对应类型所有组件对象
         */
        TArray<ComponentPtr> getComponents(const String &name) const
        {
            RTTRType type = RTTRType::get_by_name(name);
            return getComponents(type);
        }

        /**
         * \brief 获取指定类型名的所有组件对象
         * \tparam [in] T : 组件类型
         * \return 返回对应类型所有组件对象
         */
        template <typename T>
        TArray<SmartPtr<T>> getComponents() const
        {
            RTTRType type = RTTRType::get<T>();
            return getComponents(type);
        }

    protected:
        GameObject() : GameObject("") {}
        
        GameObject(const String &name);

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        ComponentPtr addComponent(const RTTRType &type);

        TResult removeComponent(const RTTRType &type);

        TResult removeComponents(const RTTRType &type);

        ComponentPtr getComponent(const RTTRType &type) const;

        TArray<ComponentPtr> getComponents(const RTTRType &type) const;

        virtual void onDestroy();
        
    private:
        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="setter")
        void setUUID(const UUID &uuid) { mUUID = uuid; }

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
