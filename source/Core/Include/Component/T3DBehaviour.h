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
     * \brief 面向 gameplay 的脚本组件基类（对齐 Unity 的 Behaviour / MonoBehaviour）
     * \remarks
     *   在 Component 与具体 gameplay 之间插入一层，承载：
     *     - 组件级 enabled 开关（独立于 GameObject 的 Active）；
     *     - 完整生命周期：Awake / OnEnable / Start / Update / LateUpdate /
     *       FixedUpdate / OnDisable / OnDestroy；
     *     - 基于 RTTR 属性拷贝的默认 clone()，子类一般无需再覆写。
     *   开发者继承本类、用 TCLASS / TPROPERTY 标注字段，即可被 addComponent
     *   挂载、被序列化、在 Inspector 暴露字段。
     *   生命周期调度详见《Behaviour 脚本组件系统设计》。
     */
    TCLASS()
    class T3D_ENGINE_API Behaviour : public Component
    {
        /// 由 GameObject / Scene 调度生命周期、enabled 过滤
        friend class GameObject;
        friend class Scene;

        TRTTI_ENABLE(Component)
        TRTTI_FRIEND

    public:
        ~Behaviour() override;

        /// 运行时快速判定是否 Behaviour（避免热路径 rttr_cast）
        Behaviour *asBehaviour() override { return this; }

        // ===== 组件级开关（对标 Unity Behaviour.enabled）=====
        TPROPERTY(RTTRFuncName="Enabled", RTTRFuncType="getter")
        bool isEnabled() const { return mEnabled; }

        TPROPERTY(RTTRFuncName="Enabled", RTTRFuncType="setter")
        void setEnabled(bool enabled);

        /// 有效执行 = 所属 GameObject.active && 本组件 enabled
        bool isActiveAndEnabled() const;

        /// 是否在编辑态也执行（对标 Unity [ExecuteInEditMode]）；默认仅 Play 模式跑
        virtual bool executeInEditMode() const { return false; }

        // ===== 默认 clone：基于 RTTR 属性拷贝，子类一般无需再覆写 =====
        ComponentPtr clone() const override;

    protected:
        Behaviour() = default;

        explicit Behaviour(const UUID &uuid);

        // ===== 生命周期回调（子类按需覆写；默认空实现）=====
        /// 同步：实例化栈内、全部组件就位后
        virtual void onAwake();
        /// 同步：紧随 Awake 或 enabled 置真
        virtual void onEnable();
        // onStart() 继承自 Component：延迟到首次 onUpdate 之前
        // onUpdate() 继承自 Component：每帧（无参，dt 取 Time::deltaTime()）
        /// 每帧，在所有 onUpdate 之后
        virtual void onLateUpdate();
        /// 固定步长（接物理时）
        virtual void onFixedUpdate();
        /// enabled 置假 / 销毁前
        virtual void onDisable();
        // onDestroy() 继承自 Component：帧末延迟销毁

        /// 默认 clone 的属性拷贝钩子（基于 RTTR 逐属性拷贝，跳过 UUID）
        TResult cloneProperties(const Component * const src) override;

        // ===== 生命周期调用入口（供 GameObject / Scene 调度，集中状态机）=====
        void invokeAwake();
        void invokeEnable();
        void invokeStart();
        void invokeDisable();

        /// 依 isActiveAndEnabled() 结果补发 onEnable / onDisable，实现可靠配对
        void refreshActiveState();

        bool wasAwaked() const { return mAwaked; }
        bool wasStarted() const { return mStarted; }

    protected:
        /// 组件级开关，通过 Enabled getter/setter 暴露给 RTTR / 序列化
        bool mEnabled {true};

        // —— 生命周期状态机，避免重复 / 错配回调（不序列化）——
        bool mAwaked {false};       ///< onAwake 是否已调用
        bool mStarted {false};      ///< onStart 是否已调用
        bool mActiveState {false};  ///< 当前是否处于 enabled+active 的运行态
    };
}


#endif  /*__T3D_BEHAVIOUR_H__*/

