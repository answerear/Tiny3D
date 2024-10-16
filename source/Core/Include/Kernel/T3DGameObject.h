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

#ifndef __T3D_GAME_OBJECT_H__
#define __T3D_GAME_OBJECT_H__


#include "T3DTypedef.h"
#include "Component/T3DComponent.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API GameObject : public Object
    {
        friend class Scene;
        
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        static GameObjectPtr create(const String &name, bool managed = true);

        static void destroyComponents();

        static void destroyGameObjects();

// #if defined (T3D_DEBUG)
//         Object *acquire() override;
//
//         void release() override;
// #endif

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

        static void destroy(GameObject *gameObject);

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
            return getComponents<Component>(type);
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
            return getComponents<T>(type);
        }

    protected:
        GameObject() : GameObject("") {}
        
        GameObject(const String &name, bool managed = true);

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        ComponentPtr addComponent(const RTTRType &type);

        TResult removeComponent(const RTTRType &type);

        TResult removeComponents(const RTTRType &type);

        ComponentPtr getComponent(const RTTRType &type) const;

        template<typename T>
        TArray<SmartPtr<T>> getComponents(const RTTRType &type) const
        {
            TArray<SmartPtr<T>> components;

            for (const auto &item : mComponents)
            {
                if (item.first == type || item.first.is_derived_from(type))
                {
                    components.emplace_back(item.second);
                }
            }

            return components;
        }

        void putUpdatingQueue(const RTTRType &type, Component *component);

        virtual void onDestroy();

        void onUpdate();

        static void destroyComponent(Component *component);

        static void destroyGameObject(GameObject *gameObject);
        
    private:
        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="setter")
        void setUUID(const UUID &uuid) { mUUID = uuid; }

        using ComponentsSet = TUnorderedMultimap<String, ComponentPtr>;

        TPROPERTY(RTTRFuncName="Components", RTTRFuncType="getter")
        const ComponentsSet &getAllComponents() const { return mComponentObjects; }

        TPROPERTY(RTTRFuncName="Components", RTTRFuncType="setter")
        void setAllComponents(const ComponentsSet &components) { mComponentObjects = components; setupComponents(); }

        /// 设置所有相关的组件，模拟 addComponent 行为，用于反序列化后 
        void setupComponents();

        /// 生成场景树，用于反序列化后
        void setupHierarchy();
        
        ComponentsSet mComponentObjects {};
        
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
        uint32_t mCameraMask {0x1};

        using Components = TUnorderedMultimap<RTTRType, ComponentPtr, RTTRTypeHash, RTTRTypeEqual>;

        /// 挂在 gameobject 上的组件集合
        Components  mComponents {};

        using ComponentList = TList<Component*>;
        using ComponentQueue = TMap<int32_t, ComponentList>;

        /// 按照设置里面设定更新顺序的组件
        ComponentQueue mUpdateComponents {};

        using ComponentQueue2 = TMultimap<String, Component*>;

        /// 没在设置里面设定更新顺序的组件
        ComponentQueue2 mUpdateComponents2 {};

        using WaitingDestroyComponents = TList<ComponentPtr>;
        
        /// 等待销毁的 component 列表
        static WaitingDestroyComponents msWaitingDestroyComponents;

        using WaitingDestroyGameObjects = TList<GameObjectPtr>;
        
        /// 等待销毁的 game object 列表
        static WaitingDestroyGameObjects msWaitingDestroyGameObjects;
    };
}


#endif    /*__T3D_GAME_OBJECT_H__*/
