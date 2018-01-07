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
        HWND    mHWnd;          // ���ھ��
        int32_t mLeft;          // �������Ͻ�X����
        int32_t mTop;           // �������Ͻ�Y����
        int32_t mWidth;         // ���ڿ��
        int32_t mHeight;        // ���ڸ߶�
        bool    mIsExternal;    // �Ƿ��ⲿ�����Ĵ���
    };
}


#endif  /*__T3D_WIN32_WINDOW_H__*/