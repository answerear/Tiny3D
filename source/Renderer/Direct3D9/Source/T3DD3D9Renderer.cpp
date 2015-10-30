

#include "T3DD3D9Renderer.h"
#include "T3DD3D9RenderWindow.h"
#include "Render/T3DRenderTarget.h"
#include "Misc/T3DEntrance.h"
#include "Listener/T3DApplicationListener.h"


namespace Tiny3D
{
    D3D9Renderer::D3D9Renderer(HINSTANCE hInstance)
        : mInstance(hInstance)
    {

    }

    D3D9Renderer::~D3D9Renderer()
    {

    }

    String D3D9Renderer::getName() const
    {
        return DIRECT3D9;
    }

    RenderWindow *D3D9Renderer::createRenderWindow(
        const RenderWindowCreateParam &rkCreateParam, 
        const RenderWindowCreateParamEx &rkCreateParamEx)
    {
        RenderWindow *window = new D3D9RenderWindow();

        RenderWindowCreateParamEx paramEx = rkCreateParamEx;
        char buf[64] = {0};
        ltoa((long_t)mInstance, buf, 10);
        String s(buf);
        paramEx["instance"] = s;

        window->create("D3D9RenderWindow", rkCreateParam, paramEx);

        attachRenderTarget(window);

        return window;
    }

    void D3D9Renderer::startRendering()
    {
        if (Entrance::getInstance().getApplicationListener())
            Entrance::getInstance().getApplicationListener()->applicationDidFinishLaunching();

        MSG msg;

        // Render this window
        PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

        while (mRenderTargets.size())
        {
            if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                if (WM_QUIT == msg.message)
                    break;

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                if (!fireFrameStarted())
                    break;

                RenderTargetListItr itr = mRenderTargets.begin();
                while (itr != mRenderTargets.end())
                {
                    itr->second->update();
                    ++itr;
                }

                if (!fireFrameEnded())
                    break;
            }
        }

        
    }
}
