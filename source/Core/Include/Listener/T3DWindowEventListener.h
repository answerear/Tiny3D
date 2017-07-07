

#ifndef __T3D_WINDOW_EVENT_LISTENER_H__
#define __T3D_WINDOW_EVENT_LISTENER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API WindowEventListener
    {
    public:
        virtual ~WindowEventListener();

        virtual void windowMoved(RenderWindowPtr window);
        virtual void windowResized(RenderWindowPtr window);
        virtual bool windowClosing(RenderWindowPtr window);
        virtual void windowClosed(RenderWindowPtr window);
        virtual void windowFocusChange(RenderWindowPtr window);
    };
}


#endif  /*__T3D_WINDOW_EVENT_LISTENER_H__*/
