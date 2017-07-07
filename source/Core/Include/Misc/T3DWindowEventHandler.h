

#ifndef __T3D_WINDOW_EVENT_HANDLER__
#define __T3D_WINDOW_EVENT_HANDLER__


#include "Misc/T3DObject.h"
#include "T3DTypedef.h"


#if defined (T3D_OS_WINDOWS)
#include <windows.h>
#endif


namespace Tiny3D
{
    class T3D_ENGINE_API WindowEventHandler 
        : public Singleton<WindowEventHandler>
        , public Object
    {
        friend class Entrance;

    public:
        virtual ~WindowEventHandler();

        void handleWindowMessage();

        void addWindowEventListener(RenderWindowPtr window, WindowEventListener *listener);
        void removeWindowEventListener(RenderWindowPtr window, WindowEventListener *listener);

        void addRenderWindow(RenderWindowPtr window);
        void removeRenderWindow(RenderWindowPtr window);

#if defined (T3D_OS_WINDOWS)
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    protected:
        LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#elif defined (T3D_OS_ANDROID)
#elif defined (T3D_OS_IOS)
#elif defined (T3D_OS_OSX)
#elif defined (T3D_OS_LINUX)
#endif

    protected:
        WindowEventHandler();

        typedef std::multimap<RenderWindowPtr, WindowEventListener*>    WindowEventListeners;
        typedef WindowEventListeners::iterator                          WindowEventListenersItr;
        typedef WindowEventListeners::const_iterator                    WindowEventListenersConstItr;
        typedef WindowEventListeners::value_type                        WindowEventListenersValue;

        WindowEventListeners    mListeners;
        RenderWindowList        mWindows;
    };
}


#endif  /*__T3D_WINDOW_EVENT_HANDLER__*/
