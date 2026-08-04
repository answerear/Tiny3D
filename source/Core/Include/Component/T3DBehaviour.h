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

#ifndef __T3D_BEHAVIOUR_H__
#define __T3D_BEHAVIOUR_H__


#include "Component/T3DComponent.h"


namespace Tiny3D
{
    /**
     * \brief 面向 gameplay 的脚本组件基类，承载 enabled 生命周期与完整脚本回调
     * \remarks 在 Component 之上提供 Awake / OnEnable / Start / Update / LateUpdate /
     *          FixedUpdate / OnDisable / OnDestroy 调度，以及基于 RTTR 的默认 clone()。
     *          生命周期由 GameObject / Scene 通过 invoke* 入口集中调度。
     */
    TCLASS()
    class T3D_ENGINE_API Behaviour : public Component
    {
        friend class GameObject;
        friend class Scene;

        TRTTI_ENABLE(Component)
        TRTTI_FRIEND

    public:
        /// 析构
        ~Behaviour() override;

        /**
         * \brief 运行时快速判定是否 Behaviour
         * \return 恒返回 this
         */
        Behaviour *asBehaviour() override { return this; }

        /**
         * \brief 设置 enabled 并在已 Awake 后按复合运行态补发 onEnable / onDisable
         * \param [in] enabled : 新的开关值；与当前值相同时无操作
         */
        void setEnabled(bool enabled) override;

        /**
         * \brief enabled 开关对本组件有效
         * \return 恒返回 true
         */
        bool supportsEnabled() const override { return true; }

        /**
         * \brief 判断组件是否处于可执行态
         * \return 所属 GameObject 存在、active 且本组件 enabled 时返回 true
         */
        bool isActiveAndEnabled() const;

        /**
         * \brief 是否在编辑态也执行脚本回调
         * \return 默认 false，仅 Play 模式调度
         */
        virtual bool executeInEditMode() const { return false; }

        /**
         * \brief 克隆当前 Behaviour，生成带新 UUID 的最派生类型实例
         * \return 克隆成功返回新组件；RTTR 创建或属性拷贝失败返回 nullptr
         */
        ComponentPtr clone() const override;

    protected:
        /// 默认构造
        Behaviour() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 组件唯一标识
         */
        explicit Behaviour(const UUID &uuid);

        /**
         * \brief 实例化后、全部组件就位时调用（默认空实现）
         */
        virtual void onAwake();

        /**
         * \brief 进入可运行态时调用，紧随 Awake 或 enabled 置真（默认空实现）
         */
        virtual void onEnable();

        // onStart() 继承自 Component：延迟到首次 onUpdate 之前
        // onUpdate() 继承自 Component：每帧（无参，dt 取 Time::deltaTime()）

        /**
         * \brief 每帧在所有 onUpdate 之后调用（默认空实现）
         */
        virtual void onLateUpdate();

        /**
         * \brief 固定步长物理帧调用（默认空实现）
         */
        virtual void onFixedUpdate();

        /**
         * \brief enabled 置假或销毁前调用（默认空实现）
         */
        virtual void onDisable();

        // onDestroy() 继承自 Component：帧末延迟销毁

        /**
         * \brief 基于 RTTR 逐属性拷贝脚本字段，跳过 UUID 与只读属性
         * \param [in] src : 源组件，不可为 nullptr
         * \return 成功返回 T3D_OK；src 为空返回 T3D_ERR_INVALID_POINTER
         */
        TResult cloneProperties(const Component * const src) override;

        /**
         * \brief 首次调用 onAwake 并置位 mAwaked
         */
        void invokeAwake();

        /**
         * \brief 首次进入运行态时调用 onEnable 并置位 mActiveState
         */
        void invokeEnable();

        /**
         * \brief 首次调用 onStart 并置位 mStarted
         */
        void invokeStart();

        /**
         * \brief 退出运行态时调用 onDisable 并清除 mActiveState
         */
        void invokeDisable();

        /**
         * \brief 依 isActiveAndEnabled() 补发 onEnable / onDisable，保证配对
         * \remarks 未 Awake 前不触发（反序列化 setEnabled 可能早于 Awake）
         */
        void refreshActiveState();

        /// onAwake 是否已调用
        bool wasAwaked() const { return mAwaked; }

        /// onStart 是否已调用
        bool wasStarted() const { return mStarted; }

    protected:
        /// onAwake 是否已调用（不序列化）
        bool mAwaked {false};
        /// onStart 是否已调用（不序列化）
        bool mStarted {false};
        /// 当前是否处于 enabled+active 的运行态（不序列化）
        bool mActiveState {false};
    };
}


#endif  /*__T3D_BEHAVIOUR_H__*/
