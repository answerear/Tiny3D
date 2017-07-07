

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
