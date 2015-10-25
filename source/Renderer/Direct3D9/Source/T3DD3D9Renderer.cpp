

#include "T3DD3D9Renderer.h"


namespace Tiny3D
{
    D3D9Renderer::D3D9Renderer(HINSTANCE hInstance)
    {

    }

    D3D9Renderer::~D3D9Renderer()
    {

    }

    String D3D9Renderer::getName() const
    {
        return "D3D9Renderer";
    }

    RenderWindow *D3D9Renderer::createRenderWindow(
        const RenderWindowCreateParam &rkCreateParam, 
        const RenderWindowCreateParamEx &rkCreateParamEx)
    {
        RenderWindow *window = nullptr;

        return window;
    }

    void D3D9Renderer::startRendering()
    {
        MSG  msg;

        // Render this window
        PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

//         while (mRenderTargets.size())
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
