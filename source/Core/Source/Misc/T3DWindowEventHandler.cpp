

#include "Misc/T3DWindowEventHandler.h"
#include "Render/T3DRenderWindow.h"
#include "Listener/T3DWindowEventListener.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(WindowEventHandler);

    WindowEventHandler::WindowEventHandler()
    {

    }

    WindowEventHandler::~WindowEventHandler()
    {

    }

    void WindowEventHandler::handleWindowMessage()
    {
#if defined (T3D_OS_WINDOWS)
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
#endif
    }

    void WindowEventHandler::addWindowEventListener(RenderWindowPtr window, WindowEventListener *listener)
    {
        mListeners.insert(WindowEventListenersValue(window, listener));
    }

    void WindowEventHandler::removeWindowEventListener(RenderWindowPtr window, WindowEventListener *listener)
    {
        WindowEventListenersItr itr = mListeners.begin();

        while (itr != mListeners.end())
        {
            if (itr->first == window && itr->second == listener)
            {
                mListeners.erase(itr);
                break;
            }
        }
    }

    void WindowEventHandler::addRenderWindow(RenderWindowPtr window)
    {
        mWindows.push_back(window);
    }

    void WindowEventHandler::removeRenderWindow(RenderWindowPtr window)
    {
        RenderWindowListItr itr = std::find(mWindows.begin(), mWindows.end(), window);

        if (itr != mWindows.end())
        {
            mWindows.erase(itr);
        }
    }

#if defined (T3D_OS_WINDOWS)
    LRESULT WindowEventHandler::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return WindowEventHandler::getInstance().WindowProc(hWnd, uMsg, wParam, lParam);
    }

    LRESULT WindowEventHandler::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_CREATE)
        {
            LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
            RenderWindow *win = (RenderWindow*)(lpcs->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)win);
            return 0;
        }

        RenderWindowPtr window = (RenderWindow *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        if (!window)
            return DefWindowProc(hWnd, uMsg, wParam, lParam);

        WindowEventListenersItr index,
            start = mListeners.lower_bound(window),
            end = mListeners.upper_bound(window);

        switch (uMsg)
        {
        case WM_ACTIVATE:
            {
                bool active = (LOWORD(wParam) != WA_INACTIVE);
                if (active)
                {
                    window->setActive(true);
                }
                else
                {
                    window->setActive(false);
                }

                for (; start != end; ++start)
                    (start->second)->windowFocusChange(window);
                break;
            }
        case WM_SYSKEYDOWN:
            switch (wParam)
            {
            case VK_CONTROL:
            case VK_SHIFT:
            case VK_MENU: //ALT
                //return zero to bypass defProc and signal we processed the message
                return 0;
            }
            break;
        case WM_SYSKEYUP:
            switch (wParam)
            {
            case VK_CONTROL:
            case VK_SHIFT:
            case VK_MENU: //ALT
            case VK_F10:
                //return zero to bypass defProc and signal we processed the message
                return 0;
            }
            break;
        case WM_SYSCHAR:
            // return zero to bypass defProc and signal we processed the message, unless it's an ALT-space
            if (wParam != VK_SPACE)
                return 0;
            break;
        case WM_ENTERSIZEMOVE:
            //log->logMessage("WM_ENTERSIZEMOVE");
            break;
        case WM_EXITSIZEMOVE:
            //log->logMessage("WM_EXITSIZEMOVE");
            break;
        case WM_MOVE:
            //log->logMessage("WM_MOVE");
            window->windowMovedOrResized();
            for (index = start; index != end; ++index)
                (index->second)->windowMoved(window);
            break;
        case WM_DISPLAYCHANGE:
            window->windowMovedOrResized();
            for (index = start; index != end; ++index)
                (index->second)->windowResized(window);
            break;
        case WM_SIZE:
            //log->logMessage("WM_SIZE");
            window->windowMovedOrResized();
            for (index = start; index != end; ++index)
                (index->second)->windowResized(window);
            break;
        case WM_GETMINMAXINFO:
            // Prevent the window from going smaller than some minimu size
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
            break;
        case WM_CLOSE:
            {
                //log->logMessage("WM_CLOSE");
                bool close = true;
                for (index = start; index != end; ++index)
                {
                    if (!(index->second)->windowClosing(window))
                        close = false;
                }
                if (!close) 
                    return 0;

                for (index = mListeners.lower_bound(window); index != end; ++index)
                    (index->second)->windowClosed(window);
                window->destroy();
                return 0;
            }
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
#endif

}
