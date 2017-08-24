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

#ifndef __T3D_D3D9_RENDER_WINDOW_H__
#define __T3D_D3D9_RENDER_WINDOW_H__


#include "T3DD3D9Prerequisites.h"
#include "Render/T3DRenderWindow.h"
#include "T3DD3D9Renderer.h"


namespace Tiny3D
{
    class D3D9RenderWindow : public RenderWindow
    {
    public:
        D3D9RenderWindow();
        virtual ~D3D9RenderWindow();

        virtual bool create(
            const String &name,
            const RenderWindowCreateParam &rkParam,
            const RenderWindowCreateParamEx &rkParamEx);

        virtual void destroy();

        virtual void swapBuffers();

        virtual bool isFullScreen() const;

        LPDIRECT3DDEVICE9 getD3DDevice()    { return mD3DDevice; }

    protected:
        bool checkMultiSampleQuality(LPDIRECT3D9 pD3D, D3DMULTISAMPLE_TYPE type, 
            DWORD *outQuality, D3DFORMAT fBack, D3DFORMAT fDepth, 
            UINT adapterNum, D3DDEVTYPE deviceType, BOOL fullScreen);

    protected:
        HWND    mHWnd;
        HWND    mParentHWnd;

        DWORD   mMultiSampleQuality;

        bool    mIsExternal;
        bool    mIsFullScreen;
        bool    mActive;
        bool    mReady;
        bool    mClosed;

        LPDIRECT3DDEVICE9   mD3DDevice;
    };
}


#endif  /*__T3D_D3D9_RENDER_WINDOW_H__*/
