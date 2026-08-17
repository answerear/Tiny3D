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


#ifndef __T3D_COMPONENT_H__
#define __T3D_COMPONENT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DGameObject.h"


namespace Tiny3D
{
    /**
     * \brief GameObject 上挂载的组件基类，提供 UUID、enabled 开关、生命周期钩子与克隆接口
     * \remarks 构造时与 onPostLoad 会将自身注册到全局 msComponents 表；onDestroy 时注销。
     *          mGameObject 为强引用，与 GameObject 组件表构成循环引用，靠显式销毁流程断链。
     */
    TCLASS()
    class T3D_ENGINE_API Component : public Object, public Noncopyable
    {
        friend class GameObject;
        
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
     
    public:
        // static void addComponent(Component *component);
        //
        // static void removeComponent(Component *component);
        //
        // static Component *getComponentByUUID(const UUID &uuid);
        
        /**
         * \brief 克隆当前组件，生成带独立 UUID 的新实例
         * \return 克隆成功返回新组件；失败返回 nullptr
         */
        virtual ComponentPtr clone() const = 0;

        /**
         * \brief 运行时快速判定组件是否为 Behaviour
         * \return 若是 Behaviour 返回自身指针，否则返回 nullptr
         * \note 用于更新热路径避免 rttr_cast 开销；Behaviour 覆写为返回 this
         */
        virtual Behaviour *asBehaviour() { return nullptr; }

        /// 返回组件 UUID
        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="getter", "Description"="UUID value")
        const UUID &getUUID() const { return mUUID; }

        /// 返回组件 enabled 开关当前值
        TPROPERTY(RTTRFuncName="Enabled", RTTRFuncType="getter", "HIDE_IN_INSPECTOR")
        bool isEnabled() const { return mEnabled; }

        /**
         * \brief 设置组件 enabled 开关
         * \param [in] enabled : 新的开关值
         */
        TPROPERTY(RTTRFuncName="Enabled", RTTRFuncType="setter")
        virtual void setEnabled(bool enabled) { mEnabled = enabled; }

        /**
         * \brief enabled 开关对本组件是否有实际作用
         * \return 基类返回 false；Behaviour、Renderable 等子类覆写为 true
         * \remarks 返回 false 时 enabled 恒为真，Inspector 也不绘制对应勾选框
         */
        virtual bool supportsEnabled() const { return false; }

        /// 返回所属 GameObject；未接线时为 nullptr
        GameObject *getGameObject() const { return mGameObject; }

        /**
         * \brief 绑定所属 GameObject
         * \param [in] gameObject : 宿主 GameObject，可为 nullptr
         */
        void setGameObject(GameObject *gameObject) { mGameObject = gameObject; }

        /**
         * \brief 首次进入运行态前的回调（默认空实现）
         */
        virtual void onStart();
        
        /**
         * \brief 每帧更新回调（默认空实现）
         */
        virtual void onUpdate();

        /**
         * \brief 从 Archive 加载关联资源时的回调（默认空实现）
         * \param [in] archive : 资源包，可为 nullptr
         */
        virtual void onLoadResource(Archive *archive);

    protected:
        /// 默认构造，UUID 保持 INVALID
        Component() = default;
        
        /**
         * \brief 以指定 UUID 构造并注册到 msComponents
         * \param [in] uuid : 组件唯一标识
         */
        Component(const UUID &uuid);

        /// 析构（默认不额外清理，子类可覆写）
        ~Component() override;

        /**
         * \brief 设置组件 UUID
         * \param [in] uuid : 新的唯一标识
         */
        TPROPERTY(RTTRFuncName="UUID", RTTRFuncType="setter", "Description"="UUID value")
        void setUUID(const UUID &uuid) { mUUID = uuid; }

        /**
         * \brief 从源组件拷贝可序列化属性
         * \param [in] src : 源组件，可为 nullptr（此时仅返回 T3D_OK）
         * \return 成功返回 T3D_OK
         */
        virtual TResult cloneProperties(const Component * const src);

        /**
         * \brief 销毁前回调：默认执行 unregister()
         */
        virtual void onDestroy();

        /**
         * \brief 引擎收尾：清空 mGameObject 并从 msComponents 注销
         * \remarks 未 Awake 的 Behaviour 跳过脚本 onDestroy 时仍须走这里
         */
        void unregister();

        /**
         * \brief 反序列化完成后回调：将自身注册到 msComponents
         */
        void onPostLoad() override;

    protected:
        /// 唯一 ID
        UUID    mUUID {UUID::INVALID};
        /// 组件级开关，通过 Enabled getter/setter 暴露给 RTTR / 序列化
        bool    mEnabled {true};
        /// 绑定的 GameObject 强引用（见类注释中的循环引用说明）
        GameObjectPtr   mGameObject {nullptr};

        using Components = TUnorderedMap<UUID, ComponentPtr, UUIDHash, UUIDEqual>;
        
        /// 全局组件 UUID 注册表
        static Components msComponents;
    };
}


#endif  /*__T3D_COMPONENT_H__*/
