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


#include "T3DRmlUiPlugin.h"
#include "T3DRmlUiSystem.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const String &RmlUiPlugin::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult RmlUiPlugin::install()
    {
        mSystem = RmlUiSystem::create();
        if (mSystem == nullptr)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        const TResult ret = T3D_AGENT.setUISystem(mSystem);
        if (T3D_FAILED(ret))
        {
            mSystem = nullptr;
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult RmlUiPlugin::startup()
    {
        // Rml::Initialise 在 Agent 创建完渲染器与窗口之后，经 UISystem::startup 调用。
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult RmlUiPlugin::shutdown()
    {
        // ~Agent 已经在 RHI 销毁前调用过 UISystem::shutdown。这里再调一次是空操作。
        // 运行途中卸载插件时，RHI 仍在，由这一次真正执行 Rml::Shutdown。
        if (mSystem != nullptr)
        {
            mSystem->shutdown();
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult RmlUiPlugin::uninstall()
    {
        if (mSystem != nullptr && T3D_AGENT.getUISystem() == mSystem.get())
        {
            T3D_AGENT.setUISystem(nullptr);
        }
        mSystem = nullptr;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
