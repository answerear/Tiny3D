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


#include "ImGuiImplDX11.h"
#include "T3DEditorInfoDX11.h"



namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImGuiImplDX11::ImGuiImplDX11()
    {
        
    }
    
    //--------------------------------------------------------------------------

    ImGuiImplDX11::~ImGuiImplDX11()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplSDL2_Shutdown();
    }

    //--------------------------------------------------------------------------

    void ImGuiImplDX11::init(void *initData)
    {
        EditorInfoDX11 *info = static_cast<EditorInfoDX11*>(initData);
        ImGui_ImplSDL2_InitForD3D(info->sdlWindow);
        ImGui_ImplDX11_Init(info->d3dDevice, info->d3dContext);
        mSDLWindow = info->sdlWindow;
        mD3DDevice = info->d3dDevice;
        mD3DDeviceContext = info->d3dContext;
        mD3DRTView = info->d3dRTView;
    }

    //--------------------------------------------------------------------------

    bool ImGuiImplDX11::pollEvents()
    {
        bool done = false;
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(mSDLWindow))
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(mSDLWindow))
            {
                // Release all outstanding references to the swap chain's buffers before resizing.
                // CleanupRenderTarget();
                // g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
                // CreateRenderTarget();
            }
        }
        return !done;
    }

    //--------------------------------------------------------------------------

    void ImGuiImplDX11::update()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplSDL2_NewFrame();
    }

    //--------------------------------------------------------------------------

    void ImGuiImplDX11::preRender()
    {
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        mD3DDeviceContext->OMSetRenderTargets(1, &mD3DRTView, nullptr);
        mD3DDeviceContext->ClearRenderTargetView(mD3DRTView, clear_color_with_alpha);
    }

    //--------------------------------------------------------------------------

    void ImGuiImplDX11::postRender()
    {
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
}

