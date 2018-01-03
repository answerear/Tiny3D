/*******************************************************************************
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
 ******************************************************************************/

#include "T3DWin32Window.h"


namespace Tiny3D
{
    Win32Window::Win32Window()
        : mHWnd(nullptr)
        , mLeft(0)
        , mTop(0)
        , mWidth(0)
        , mHeight(0)
        , mIsExternal(false)
    {

    }

    Win32Window::~Win32Window()
    {
        destroy();
    }

    bool Win32Window::create(const char *title, int32_t x, int32_t y,
        int32_t w, int32_t h, bool isFullscreen,
        HWND hParentWnd, HWND hExternalWnd, HINSTANCE hInstance)
    {
        if (hExternalWnd == nullptr)
        {
            // 不是外部窗口
            WNDCLASSEX wc;
            wc.cbSize = sizeof(WNDCLASSEX);
            wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
            wc.lpfnWndProc = Win32Window::WndProc;
            wc.cbClsExtra = 0;
            wc.cbWndExtra = 0;
            wc.hInstance = (HINSTANCE)hInstance;
            wc.hIcon = ::LoadIcon(NULL, IDC_ICON);
            wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
            wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
            wc.lpszMenuName = NULL;
            wc.lpszClassName = TEXT(title);
            wc.hIconSm = ::LoadIcon(NULL, IDC_ICON);

            ::RegisterClassEx(&wc);

            DWORD dwStyleEx = WS_EX_APPWINDOW;
            if (isFullscreen)
                dwStyleEx |= WS_EX_TOPMOST;

            DWORD dwStyle = WS_POPUP | WS_CLIPCHILDREN | WS_VISIBLE;
            if (hParentWnd != nullptr)
            {
                dwStyle |= WS_CHILD;
            }
            else
            {
                dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |
                    WS_SYSMENU | WS_MINIMIZEBOX;
            }

            RECT rect = { x, y, x + w, y + h };

            ::AdjustWindowRectEx(&rect, dwStyle, FALSE, dwStyleEx);

            mHWnd = ::CreateWindowEx(dwStyleEx, TEXT(title),
                title, dwStyle,
                rect.left, rect.top,
                rect.right - rect.left, rect.bottom - rect.top,
                hParentWnd, NULL, hInstance, this);

            ::ShowWindow(mHWnd, SW_SHOWNORMAL);
            ::UpdateWindow(mHWnd);

            mLeft = x;
            mTop = y;
            mWidth = w;
            mHeight = h;
            mIsExternal = false;
        }
        else
        {
            // 外部窗口
            mHWnd = hExternalWnd;
            ::ShowWindow(mHWnd, SW_SHOWNORMAL);
            ::UpdateWindow(mHWnd);

            RECT rc;
            ::GetClientRect(mHWnd, &rc);
            mLeft = rc.left;
            mTop = rc.top;
            mWidth = rc.right - rc.left;
            mHeight = rc.bottom - rc.top;
            mIsExternal = true;
        }

        return (mHWnd != nullptr);
    }

    bool Win32Window::create(const char *title, int32_t x, int32_t y, 
        int32_t w, int32_t h, bool isFullscreen, int32_t argc, va_list args)
    {
        if (mHWnd)
            destroy();

        // Windows平台上附加参数是3个
        T3D_ASSERT(argc == 3);

        // Param #1 程序实例
        HINSTANCE hInstance = va_arg(args, HINSTANCE);

        // Param #2 父窗口句柄
        HWND hParentWnd = va_arg(args, HWND);

        // Param #3 外部窗口句柄，看是否外部已经创建了窗口传入的句柄
        HWND hExternalWnd = va_arg(args, HWND);

        return create(title, x, y, w, h, isFullscreen, hParentWnd, hExternalWnd, hInstance);
    }

    void Win32Window::destroy()
    {
        if (!mIsExternal && mHWnd != nullptr)
        {
            ::DestroyWindow(mHWnd);
            mHWnd = nullptr;
        }
    }

    void Win32Window::pollEvents()
    {
        while (1)
        {
            MSG msg;
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            render();
        }
    }

    void *Win32Window::getNativeWinObject()
    {
        return mHWnd;
    }

    LRESULT Win32Window::WndProc(HWND hWnd, UINT uMsg, 
        WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_CREATE)
        {
            LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
            Win32Window *win = (Win32Window *)(lpcs->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)win);
            return 0;
        }

        Win32Window *win = (Win32Window *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        if (win == nullptr)
            return DefWindowProc(hWnd, uMsg, wParam, lParam);

        return win->WindowProc(hWnd, uMsg, wParam, lParam);
    }

    LRESULT Win32Window::WindowProc(HWND hWnd, UINT uMsg, 
        WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_CLOSE:
            {
                destroy();
                return 0;
            }
            break;
        }

        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}