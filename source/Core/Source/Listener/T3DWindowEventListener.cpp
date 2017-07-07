
#include "Listener/T3DWindowEventListener.h"
#include "Render/T3DRenderWindow.h"


namespace Tiny3D
{
    WindowEventListener::~WindowEventListener()
    {

    }

    void WindowEventListener::windowMoved(RenderWindowPtr window)
    {
        
    }

    void WindowEventListener::windowResized(RenderWindowPtr window)
    {

    }

    bool WindowEventListener::windowClosing(RenderWindowPtr window)
    {
        return true;
    }

    void WindowEventListener::windowClosed(RenderWindowPtr window)
    {

    }

    void WindowEventListener::windowFocusChange(RenderWindowPtr window)
    {

    }
}
