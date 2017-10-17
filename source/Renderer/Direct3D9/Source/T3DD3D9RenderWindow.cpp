/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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
 **************************************************************************************************/

#include "T3DD3D9RenderWindow.h"
#include "T3DD3D9Renderer.h"
#include "T3DD3D9WindowEventHandler.h"


namespace Tiny3D
{
    D3D9RenderWindow::D3D9RenderWindow()
        : mHWnd(nullptr)
        , mParentHWnd(nullptr)
        , mIsFullScreen(false)
        , mActive(false)
        , mReady(false)
        , mClosed(false)
        , mIsExternal(false)
        , mD3DDevice(nullptr)
    {

    }

    D3D9RenderWindow::~D3D9RenderWindow()
    {

    }

    bool D3D9RenderWindow::isFullScreen() const
    {
        return mIsFullScreen;
    }

    bool D3D9RenderWindow::create(
        const String &name,
        const RenderWindowCreateParam &rkParam,
        const RenderWindowCreateParamEx &rkParamEx)
    {
        if (mHWnd)
            destroy();

        HWND externalHWnd = nullptr;
        VariantMapConstItr itr = rkParamEx.find("externalHandle");
        if (itr != rkParamEx.end())
        {
            externalHWnd = (HWND)itr->second.longValue();
        }

        HWND parentHWnd = nullptr;
        itr = rkParamEx.find("parentHandle");
        if (itr != rkParamEx.end())
        {
            parentHWnd = (HWND)itr->second.longValue();
        }

        HINSTANCE instance = nullptr;
        itr = rkParamEx.find("instance");
        if (itr != rkParamEx.end())
        {
            instance = (HINSTANCE)itr->second.longValue();
        }

        if (externalHWnd == nullptr)
        {
            WNDCLASSEX wc;
            wc.cbSize = sizeof(WNDCLASSEX);
            wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
            wc.lpfnWndProc = D3D9WindowEventHandler::WndProc;
            wc.cbClsExtra = 0;
            wc.cbWndExtra = 0;
            wc.hInstance = (HINSTANCE)instance;
            wc.hIcon = ::LoadIcon(NULL, IDC_ICON);
            wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
            wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
            wc.lpszMenuName = NULL;
            wc.lpszClassName = TEXT(name.c_str());
            wc.hIconSm = ::LoadIcon(NULL, IDC_ICON);

            ::RegisterClassEx(&wc);

            DWORD dwStyleEx = WS_EX_APPWINDOW;
            if (rkParam._fullscreen)
                dwStyleEx |= WS_EX_TOPMOST;

            DWORD dwStyle = WS_POPUP|WS_CLIPCHILDREN|WS_VISIBLE;
            if (parentHWnd != nullptr)
            {
                dwStyle |= WS_CHILD;
            }
            else
            {
                dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |
                           WS_SYSMENU | WS_MINIMIZEBOX;
            }

            RECT rect = 
            {
                rkParam._windowLeft, rkParam._windowTop, 
                rkParam._windowLeft + rkParam._windowWidth, rkParam._windowTop + rkParam._windowHeight
            };

            ::AdjustWindowRectEx(&rect, dwStyle, FALSE, dwStyleEx);

            mHWnd = ::CreateWindowEx(dwStyleEx, TEXT(name.c_str()),
                                     rkParam._windowTitle.c_str(), dwStyle,
                                     rect.left, rect.top, 
                                     rect.right - rect.left, rect.bottom - rect.top, 
                                     parentHWnd, NULL, instance, this);

            ::ShowWindow(mHWnd, SW_SHOWNORMAL);
            ::UpdateWindow(mHWnd);            

            mWidth = rkParam._windowWidth;
            mHeight = rkParam._windowHeight;
            mName = name;
            mIsFullScreen = rkParam._fullscreen;

            Entrance::getInstance().getWindowEventHandler()->addRenderWindow(this);

            mIsExternal = false;
        }
        else
        {
            mHWnd = externalHWnd;
            ::ShowWindow(mHWnd, SW_SHOWNORMAL);
            ::UpdateWindow(mHWnd);

            RECT rc;
            ::GetClientRect(mHWnd, &rc);
            mWidth = rc.right - rc.left;
            mHeight = rc.bottom - rc.top;
            mName = name;
            mIsFullScreen = rkParam._fullscreen;

            mIsExternal = true;
        }

        if (mIsFullScreen)
        {
            mColorDepth = rkParam._colorDepth;
        }
        else
        {
            // Get color depth from display
            HDC hdc = GetDC(mHWnd);
            mColorDepth = GetDeviceCaps(hdc, BITSPIXEL);
            ReleaseDC(mHWnd, hdc);
        }

        D3D9Renderer *renderer = (D3D9Renderer *)rkParamEx.at("renderer").longValue();
        LPDIRECT3D9 pD3D = renderer->getD3D();

        UINT uAdapter = 0;
        D3DADAPTER_IDENTIFIER9 d3dai;
        D3DDISPLAYMODE d3ddm;
        pD3D->GetAdapterIdentifier(uAdapter, 0, &d3dai);
        pD3D->GetAdapterDisplayMode(uAdapter, &d3ddm);


        D3DPRESENT_PARAMETERS d3dpp;
        memset(&d3dpp, 0, sizeof(d3dpp));

        D3DDEVTYPE devType = D3DDEVTYPE_HAL;

        d3dpp.BackBufferCount = 1;
        d3dpp.EnableAutoDepthStencil = TRUE;

        bool vsync = rkParamEx.at("vsync").boolValue();
        if (vsync)
            d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
        else
            d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

        d3dpp.Windowed = !mIsFullScreen;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

        if (mIsFullScreen)
        {
            d3dpp.BackBufferWidth = mWidth;
            d3dpp.BackBufferHeight = mHeight;
            d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

            if (mColorDepth > 16)
                d3dpp.BackBufferFormat = D3DFMT_X8B8G8R8;
            else
                d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
        }
        else
        {
            d3dpp.BackBufferWidth = mWidth;
            d3dpp.BackBufferHeight = mHeight;
            d3dpp.BackBufferFormat = d3ddm.Format;
        }

        if (mColorDepth > 16 )
        {
            // Try to create a 32-bit depth, 8-bit stencil
            if (FAILED(pD3D->CheckDeviceFormat(uAdapter,
                devType,  d3dpp.BackBufferFormat,  D3DUSAGE_DEPTHSTENCIL, 
                D3DRTYPE_SURFACE, D3DFMT_D24S8)))
            {
                // Bugger, no 8-bit hardware stencil, just try 32-bit zbuffer 
                if (FAILED( pD3D->CheckDeviceFormat(uAdapter,
                    devType,  d3dpp.BackBufferFormat,  D3DUSAGE_DEPTHSTENCIL, 
                    D3DRTYPE_SURFACE, D3DFMT_D32 )))
                {
                    // Jeez, what a naff card. Fall back on 16-bit depth buffering
                    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
                }
                else
                    d3dpp.AutoDepthStencilFormat = D3DFMT_D32;
            }
            else
            {
                // Woohoo!
                if (SUCCEEDED( pD3D->CheckDepthStencilMatch(uAdapter, devType,
                    d3dpp.BackBufferFormat, d3dpp.BackBufferFormat, D3DFMT_D24X8)))
                {
                    d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8; 
                } 
                else 
                    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8; 
            }
        }
        else
        {
            // 16-bit depth, software stencil
            d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        }

        mMultiSampleQuality = (DWORD)rkParamEx.at("MultiSampleQuality").longValue();

        if (mMultiSampleQuality < 2) // NONE
            mMultiSampleQuality = 0;
        if (mMultiSampleQuality > 16) // MAX
            mMultiSampleQuality = 16;

        if (mMultiSampleQuality && 
            checkMultiSampleQuality(
            pD3D,
            D3DMULTISAMPLE_NONMASKABLE,
            &mMultiSampleQuality, 
            d3dpp.BackBufferFormat, 
            d3dpp.AutoDepthStencilFormat, 
            uAdapter, 
            devType, 
            mIsFullScreen))
        {
            d3dpp.MultiSampleType = D3DMULTISAMPLE_NONMASKABLE;
            d3dpp.MultiSampleQuality = mMultiSampleQuality ? mMultiSampleQuality - 1 : NULL;
        }
        else
        {
            d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
            d3dpp.MultiSampleQuality = NULL;
        }

        HRESULT hr = pD3D->CreateDevice(uAdapter, devType, mHWnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &mD3DDevice);
        if (SUCCEEDED(hr))
        {
//             mD3DDevice->GetRenderTarget(0, &mpRenderSurface);
//             mD3DDevice->GetDepthStencilSurface( &mpRenderZBuffer );
        }
        else
        {
            hr = pD3D->CreateDevice(uAdapter, devType, mHWnd,
                D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &mD3DDevice);
            if (SUCCEEDED(hr))
            {
//                 mpD3DDevice->GetRenderTarget( 0, &mpRenderSurface );
//                 mpD3DDevice->GetDepthStencilSurface( &mpRenderZBuffer );
            }
            else
            {
                hr = pD3D->CreateDevice(uAdapter, devType, mHWnd,
                    D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &mD3DDevice);
                if (SUCCEEDED(hr))
                {
//                     mpD3DDevice->GetRenderTarget( 0, &mpRenderSurface );
//                     mpD3DDevice->GetDepthStencilSurface( &mpRenderZBuffer );
                }
            }
        }

        // TODO: make this a bit better e.g. go from pure vertex processing to software
        D3D9_RENDERER.setD3DDevice(mD3DDevice);

        mReady = true;

        return true;
    }

    void D3D9RenderWindow::destroy()
    {
        if (mD3DDevice != nullptr)
        {
            mD3DDevice->Release();
            mD3DDevice = nullptr;
        }

        if (!mIsExternal)
        {
            Entrance::getInstance().getWindowEventHandler()->removeRenderWindow(this);

            Entrance::getInstance().shutdown();

            ::DestroyWindow(mHWnd);
            mHWnd = nullptr;
        }
    }

    void D3D9RenderWindow::swapBuffers()
    {
        if (mD3DDevice != nullptr)
        {
            HRESULT hr = mD3DDevice->Present(NULL, NULL, 0, NULL);
            if (D3DERR_DEVICELOST == hr)
            {

            }
            else if (FAILED(hr))
            {

            }
        }
    }

    bool D3D9RenderWindow::checkMultiSampleQuality(LPDIRECT3D9 pD3D,
        D3DMULTISAMPLE_TYPE type, DWORD *outQuality, D3DFORMAT fBack, 
        D3DFORMAT fDepth, UINT adapterNum, D3DDEVTYPE deviceType, 
        BOOL fullScreen)
    {
        if (SUCCEEDED(pD3D->CheckDeviceMultiSampleType( 
            adapterNum, 
            deviceType, fBack, 
            fullScreen, type, outQuality)))
            return true;
        else
            return false;
    }

//     LRESULT D3D9RenderWindow::WndProc(
//         HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//     {
//         LPCREATESTRUCT lpcs;
//         D3D9RenderWindow* win = nullptr;
// 
//         // look up window instance
//         if( WM_CREATE != uMsg )
//         {
//             // Get window pointer
//             win = (D3D9RenderWindow*)GetWindowLongPtr(hWnd, GWL_USERDATA);
// 
//             if (win == nullptr)
//                 return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
//         }
// 
//         switch (uMsg)
//         {
//         case WM_ACTIVATE:
//             if (WA_INACTIVE == LOWORD(wParam))
//                 win->mActive = false;
//             else
//                 win->mActive = true;
//             break;
// 
//         case WM_CREATE:
//             // Log the new window
//             // Get CREATESTRUCT
//             lpcs = (LPCREATESTRUCT)lParam;
//             win = (D3D9RenderWindow*)(lpcs->lpCreateParams);
//             // Store pointer in window user data area
//             SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)win);
//             win->mActive = true;
// 
//             return 0;
//             break;
// 
//         case WM_KEYDOWN:
//             // TEMPORARY CODE
//             // TODO - queue up keydown / keyup events with
//             //  window name and timestamp to be processed
//             //  by main loop
// 
//             // ESCAPE closes window
//             if (wParam == VK_ESCAPE)
//             {
//                 win->mClosed = true;
//                 return 0L;
//             }
//             break;
// 
//         case WM_PAINT:
//             // If we get WM_PAINT messges, it usually means our window was
//             // comvered up, so we need to refresh it by re-showing the contents
//             // of the current frame.
//             if (win->mActive && win->mReady)
//                 win->update();
//             break;
// 
//         case WM_MOVE:
//             // Move messages need to be tracked to update the screen rects
//             // used for blitting the backbuffer to the primary
//             // *** This doesn't need to be used to Direct3D9 ***
//             break;
// 
//         case WM_ENTERSIZEMOVE:
//             // Previent rendering while moving / sizing
//             win->mReady = false;
//             break;
// 
//         case WM_EXITSIZEMOVE:
// //          win->WindowMovedOrResized();
//             win->mReady = true;
//             break;
// 
//         case WM_SIZE:
//             // Check to see if we are losing or gaining our window.  Set the
//             // active flag to match
//             if( SIZE_MAXHIDE == wParam || SIZE_MINIMIZED == wParam )
//                 win->mActive = false;
//             else
//             {
//                 win->mActive = true;
// //              if( win->mReady )
// //                  win->WindowMovedOrResized();
//             }
//             break;
// 
//         case WM_GETMINMAXINFO:
//             // Prevent the window from going smaller than some minimum size
//             ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
//             ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
//             break;
// 
//         case WM_CLOSE:
//             Entrance::getInstance().getActiveRenderer()->detachRenderTarget(win->getName());
//             DestroyWindow(win->mHWnd);
//             win->mClosed = true;
// //             return 0;
//             break;
// 
//         case WM_DESTROY:
//             PostQuitMessage(0);
//             return 0;
//         }
// 
//         return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
//     }
}
