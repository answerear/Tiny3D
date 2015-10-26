

#include "T3DD3D9Renderer.h"
#include "T3DD3D9RenderWindow.h"


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
        return "T3DD3D9Renderer";
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

        return window;
    }

    void D3D9Renderer::startRendering()
    {
        MSG  msg;

        // Render this window
        PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

//         while (mRenderTargets.size())
        while (1)
        {
            if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
            else
            {
//                 if(!fireFrameStarted())
//                     return;

//                 // Render a frame during idle time (no messages are waiting)
//                 RenderTargetPriorityMap::iterator itarg, itargend;
//                 itargend = mPrioritisedRenderTargets.end();
//                 for( itarg = mPrioritisedRenderTargets.begin(); itarg != itargend; ++itarg )
//                 {
//                     if( itarg->second->isActive() )
//                         itarg->second->update();
//                 }
// 
//                 if(!fireFrameEnded())
//                     return;
            }
        }

    }
}
