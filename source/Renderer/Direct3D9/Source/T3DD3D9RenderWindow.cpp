

#include "T3DD3D9RenderWindow.h"
#include "Misc/T3DEntrance.h"
#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    D3D9RenderWindow::D3D9RenderWindow()
        : mHWnd(nullptr)
        , mParentHWnd(nullptr)
        , mIsFullScreen(false)
        , mActive(false)
        , mReady(false)
        , mClosed(false)
    {

    }

    D3D9RenderWindow::~D3D9RenderWindow()
    {

    }

    bool D3D9RenderWindow::isFullScreen() const
    {
        return mIsFullScreen;
    }

    bool D3D9RenderWindow::create(
        const String &name,
        const RenderWindowCreateParam &rkParam,
        const RenderWindowCreateParamEx &rkParamEx)
    {
        if (mHWnd)
            destroy();

        HWND externalHWnd = nullptr;
        RenderWindowCreateParamEx::const_iterator itr = rkParamEx.find("externalHandle");
        if (itr != rkParamEx.end())
        {
            externalHWnd = (HWND)atol(itr->second.c_str());
        }

        HWND parentHWnd = nullptr;
        itr = rkParamEx.find("parentHandle");
        if (itr != rkParamEx.end())
        {
            parentHWnd = (HWND)atol(itr->second.c_str());
        }

        HINSTANCE instance = nullptr;
        itr = rkParamEx.find("instance");
        {
            instance = (HINSTANCE)atol(itr->second.c_str());
        }

        if (externalHWnd == nullptr)
        {
            WNDCLASSEX wc;
            wc.cbSize = sizeof(WNDCLASSEX);
            wc.style = CS_HREDRAW | CS_VREDRAW;
            wc.lpfnWndProc = WndProc;
            wc.cbClsExtra = 0;
            wc.cbWndExtra = 0;
            wc.hInstance = (HINSTANCE)instance;
            wc.hIcon = ::LoadIcon(NULL, IDC_ICON);
            wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
            wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
            wc.lpszMenuName = NULL;
            wc.lpszClassName = TEXT(name.c_str());
            wc.hIconSm = ::LoadIcon(NULL, IDC_ICON);

            ::RegisterClassEx(&wc);

            DWORD dwStyleEx = 0;
            if (rkParam._fullscreen)
                dwStyleEx |= WS_EX_TOPMOST;

            DWORD dwStyle = WS_POPUP|WS_CLIPCHILDREN|WS_VISIBLE;
            if (parentHWnd != nullptr)
            {
                dwStyle |= WS_CHILD;
            }
            else
            {
                dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |
                           WS_SYSMENU | WS_MINIMIZEBOX;
            }

            mHWnd = ::CreateWindowEx(dwStyleEx, TEXT(name.c_str()),
                                     rkParam._windowTitle.c_str(), dwStyle,
                                     rkParam._windowLeft, rkParam._windowTop, rkParam._windowWidth,
                                     rkParam._windowHeight, parentHWnd, NULL, instance, this);

            ::ShowWindow(mHWnd, SW_SHOWNORMAL);
            ::UpdateWindow(mHWnd);

            mWidth = rkParam._windowWidth;
            mHeight = rkParam._windowHeight;
            mName = name;
            mIsFullScreen = rkParam._fullscreen;
        }
        else
        {
            mHWnd = externalHWnd;
            ::ShowWindow(mHWnd, SW_SHOWNORMAL);
            ::UpdateWindow(mHWnd);

            RECT rc;
            ::GetClientRect(mHWnd, &rc);
            mWidth = rc.right - rc.left;
            mHeight = rc.bottom - rc.top;
            mName = name;
            mIsFullScreen = rkParam._fullscreen;
        }

        if (mIsFullScreen)
        {
            mColorDepth = rkParam._colorDepth;
        }
        else
        {
            // Get color depth from display
            HDC hdc = GetDC(mHWnd);
            mColorDepth = GetDeviceCaps(hdc, BITSPIXEL);
            ReleaseDC(mHWnd, hdc);
        }



        mReady = true;

        return true;
    }

    void D3D9RenderWindow::destroy()
    {
        ::DestroyWindow(mHWnd);
        mHWnd = nullptr;
    }

    void D3D9RenderWindow::swapBuffers()
    {

    }

    LRESULT D3D9RenderWindow::WndProc(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LPCREATESTRUCT lpcs;
        D3D9RenderWindow* win = nullptr;

        // look up window instance
        if( WM_CREATE != uMsg )
        {
            // Get window pointer
            win = (D3D9RenderWindow*)GetWindowLongPtr(hWnd, GWL_USERDATA);

            if (win == nullptr)
                return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }

        switch (uMsg)
        {
        case WM_ACTIVATE:
            if (WA_INACTIVE == LOWORD(wParam))
                win->mActive = false;
            else
                win->mActive = true;
            break;

        case WM_CREATE:
            // Log the new window
            // Get CREATESTRUCT
            lpcs = (LPCREATESTRUCT)lParam;
            win = (D3D9RenderWindow*)(lpcs->lpCreateParams);
            // Store pointer in window user data area
            SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)win);
            win->mActive = true;

            return 0;
            break;

        case WM_KEYDOWN:
            // TEMPORARY CODE
            // TODO - queue up keydown / keyup events with
            //  window name and timestamp to be processed
            //  by main loop

            // ESCAPE closes window
            if (wParam == VK_ESCAPE)
            {
                win->mClosed = true;
                return 0L;
            }
            break;

        case WM_PAINT:
            // If we get WM_PAINT messges, it usually means our window was
            // comvered up, so we need to refresh it by re-showing the contents
            // of the current frame.
            if (win->mActive && win->mReady)
                win->update();
            break;

        case WM_MOVE:
            // Move messages need to be tracked to update the screen rects
            // used for blitting the backbuffer to the primary
            // *** This doesn't need to be used to Direct3D9 ***
            break;

        case WM_ENTERSIZEMOVE:
            // Previent rendering while moving / sizing
            win->mReady = false;
            break;

        case WM_EXITSIZEMOVE:
//          win->WindowMovedOrResized();
            win->mReady = true;
            break;

        case WM_SIZE:
            // Check to see if we are losing or gaining our window.  Set the
            // active flag to match
            if( SIZE_MAXHIDE == wParam || SIZE_MINIMIZED == wParam )
                win->mActive = false;
            else
            {
                win->mActive = true;
//              if( win->mReady )
//                  win->WindowMovedOrResized();
            }
            break;

        case WM_GETMINMAXINFO:
            // Prevent the window from going smaller than some minimum size
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
            break;

        case WM_CLOSE:
            Entrance::getInstance().getActiveRenderer()->detachRenderTarget(win->getName());
            DestroyWindow(win->mHWnd);
            win->mClosed = true;
//             return 0;
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}
