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


#include "T3DRmlUiSystem.h"

#include <RmlUi/Core.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    RmlUiSystemPtr RmlUiSystem::create()
    {
        return T3D_NEW RmlUiSystem();
    }

    //--------------------------------------------------------------------------

    const String &RmlUiSystem::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult RmlUiSystem::startup()
    {
        if (mInitialised)
        {
            return T3D_OK;
        }

        if (!Rml::Initialise())
        {
            T3D_LOG_ERROR(LOG_TAG_RMLUI, "Rml::Initialise() failed.");
            return T3D_ERR_FAIL;
        }

        mInitialised = true;
        const Rml::String version = Rml::GetVersion();
        T3D_LOG_INFO(LOG_TAG_RMLUI, "RmlUi %s initialised.", version.c_str());
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void RmlUiSystem::shutdown()
    {
        if (!mInitialised)
        {
            return;
        }

        // ~Agent 在销毁 RHI 之前调用本函数。插件 unload 时若已经关过，这里直接返回。
        T3D_LOG_INFO(LOG_TAG_RMLUI, "Rml::Shutdown() before RHI destroy.");
        Rml::Shutdown();
        mInitialised = false;
    }

    //--------------------------------------------------------------------------

    void RmlUiSystem::update()
    {
    }

    //--------------------------------------------------------------------------

    void RmlUiSystem::render(RHIContext *ctx, Camera *camera, RenderTarget *target, UIRenderPhase phase)
    {
        (void)ctx;
        (void)camera;
        (void)target;
        (void)phase;
    }

    //--------------------------------------------------------------------------

    bool RmlUiSystem::isPointerOverUI() const
    {
        return false;
    }

    //--------------------------------------------------------------------------

    bool RmlUiSystem::wantsKeyboard() const
    {
        return false;
    }

    //--------------------------------------------------------------------------

    void RmlUiSystem::releaseAll()
    {
    }

    //--------------------------------------------------------------------------
}
