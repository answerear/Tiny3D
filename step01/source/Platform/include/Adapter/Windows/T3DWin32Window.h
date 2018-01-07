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

#ifndef __T3D_WIN32_WINDOW_H__
#define __T3D_WIN32_WINDOW_H__


#include "Adapter/T3DWindowInterface.h"
#include <windows.h>


namespace Tiny3D
{
    class Win32Window : public IWindow
    {
        T3D_DISABLE_COPY(Win32Window);
    public:
        Win32Window();

        virtual ~Win32Window();

        virtual bool create(const char *title, int32_t x, int32_t y, 
            int32_t w, int32_t h, bool isFullscreen, 
            int32_t argc, va_list args) override;

        virtual void destroy() override;

        virtual void *getNativeWinObject() override;

    protected:
        bool create(const char *title, int32_t x, int32_t y, int32_t w,
            int32_t h, bool isFullscreen, HWND hParentWnd, HWND hExternalWnd,
            HINSTANCE hInstance);

        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        virtual LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    protected:
        HWND    mHWnd;          // 窗口句柄
        int32_t mLeft;          // 窗口左上角X坐标
        int32_t mTop;           // 窗口左上角Y坐标
        int32_t mWidth;         // 窗口宽度
        int32_t mHeight;        // 窗口高度
        bool    mIsExternal;    // 是否外部创建的窗口
    };
}


#endif  /*__T3D_WIN32_WINDOW_H__*/