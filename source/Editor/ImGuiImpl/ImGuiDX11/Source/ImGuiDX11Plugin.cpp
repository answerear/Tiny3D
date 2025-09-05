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


#include "ImGuiDX11Plugin.h"
#include "ImGuiImplDX11.h"
#include "ImGuiApp.h"


namespace Tiny3D
{
    namespace Editor
    {
        //----------------------------------------------------------------------

        const String &ImGuiDX11Plugin::getName() const
        {
            return mName;
        }

        //----------------------------------------------------------------------

        TResult ImGuiDX11Plugin::install()
        {
            ImGuiApp *app = static_cast<ImGuiApp *>(ImGuiApp::getInstancePtr());
            mImGuiImpl = T3D_NEW ImGuiImplDX11();
            app->setEditorRenderer(mImGuiImpl);
            return T3D_OK;
        }

        //----------------------------------------------------------------------

        TResult ImGuiDX11Plugin::startup()
        {
            return T3D_OK;
        }

        //----------------------------------------------------------------------

        TResult ImGuiDX11Plugin::shutdown()
        {
            return T3D_OK;
        }

        //----------------------------------------------------------------------

        TResult ImGuiDX11Plugin::uninstall()
        {
            TResult ret = T3D_OK;
            ImGuiApp *app = static_cast<ImGuiApp*>(ImGuiApp::getInstancePtr());
            app->setEditorRenderer(nullptr);
            T3D_SAFE_DELETE(mImGuiImpl);
            return ret;
        }

        //----------------------------------------------------------------------
    }
    
}

