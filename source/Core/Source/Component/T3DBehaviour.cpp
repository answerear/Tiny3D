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


#include "Component/T3DBehaviour.h"
#include "Kernel/T3DGameObject.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Behaviour::Behaviour(const UUID &uuid)
        : Component(uuid)
    {
    }

    //--------------------------------------------------------------------------

    Behaviour::~Behaviour()
    {
    }

    //--------------------------------------------------------------------------

    void Behaviour::setEnabled(bool enabled)
    {
        if (mEnabled == enabled)
        {
            return;
        }

        Component::setEnabled(enabled);

        // 已 Awake 后才根据复合运行态补发 onEnable / onDisable
        refreshActiveState();
    }

    //--------------------------------------------------------------------------

    bool Behaviour::isActiveAndEnabled() const
    {
        GameObject *go = getGameObject();
        return go != nullptr && go->isActiveInHierarchy() && mEnabled;
    }

    //--------------------------------------------------------------------------

    ComponentPtr Behaviour::clone() const
    {
        // 以运行时最派生类型创建新实例，赋予独立 UUID
        rttr::instance srcInst(*this);
        rttr::type t = srcInst.get_type();

        TArray<rttr::argument> args;
        UUID uuid = UUID::generate();
        args.push_back(uuid);
        rttr::variant var = t.create(args);

        bool ok = false;
        Behaviour *bahviour = var.convert<Behaviour *>(&ok);
        if (!ok || bahviour == nullptr)
        {
            return nullptr;
        }

        if (T3D_FAILED(bahviour->cloneProperties(this)))
        {
            return nullptr;
        }

        return bahviour;
    }

    //--------------------------------------------------------------------------

    TResult Behaviour::cloneProperties(const Component * const src)
    {
        TResult ret = Component::cloneProperties(src);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        if (src == nullptr)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        // 基于 RTTR 逐属性拷贝，覆盖子类脚本字段，免去子类手写 clone。
        // UUID 保持克隆时新生成的独立值，不从源拷贝。
        rttr::instance dstInst(*this);
        rttr::instance srcInst(*src);
        rttr::type t = srcInst.get_type();

        for (auto &prop : t.get_properties())
        {
            if (prop.get_name() == "UUID")
            {
                continue;
            }

            if (prop.is_readonly())
            {
                continue;
            }

            rttr::variant value = prop.get_value(srcInst);
            if (value.is_valid())
            {
                prop.set_value(dstInst, value);
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    // 生命周期回调默认空实现（子类按需覆写）
    //--------------------------------------------------------------------------

    void Behaviour::onAwake()
    {
    }

    //--------------------------------------------------------------------------

    void Behaviour::onEnable()
    {
    }

    //--------------------------------------------------------------------------

    void Behaviour::onLateUpdate()
    {
    }

    //--------------------------------------------------------------------------

    void Behaviour::onFixedUpdate()
    {
    }

    //--------------------------------------------------------------------------

    void Behaviour::onDisable()
    {
    }

    //--------------------------------------------------------------------------
    // 生命周期调用入口（集中状态机，供 GameObject / Scene 调度）
    //--------------------------------------------------------------------------

    void Behaviour::invokeAwake()
    {
        if (!mAwaked)
        {
            mAwaked = true;
            onAwake();
        }
    }

    //--------------------------------------------------------------------------

    void Behaviour::invokeEnable()
    {
        if (!mActiveState)
        {
            mActiveState = true;
            onEnable();
        }
    }

    //--------------------------------------------------------------------------

    void Behaviour::invokeStart()
    {
        if (!mStarted)
        {
            mStarted = true;
            onStart();
        }
    }

    //--------------------------------------------------------------------------

    void Behaviour::invokeDisable()
    {
        if (mActiveState)
        {
            mActiveState = false;
            onDisable();
        }
    }

    //--------------------------------------------------------------------------

    void Behaviour::refreshActiveState()
    {
        // 未 Awake 前不触发 OnEnable / OnDisable（反序列化 setEnabled 早于 Awake）
        if (!mAwaked)
        {
            return;
        }

        const bool shouldRun = isActiveAndEnabled();
        if (shouldRun && !mActiveState)
        {
            invokeEnable();
        }
        else if (!shouldRun && mActiveState)
        {
            invokeDisable();
        }
    }

    //--------------------------------------------------------------------------
}

