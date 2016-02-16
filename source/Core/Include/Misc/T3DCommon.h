

#ifndef __T3D_COMMON_H__
#define __T3D_COMMON_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    enum PixelFormat
    {
        E_PF_UNKNOWN = 0,
        E_PF_R5G6B5,
        E_PF_A1R5G5B5,
        E_PF_A4R4G4B4,
        E_PF_R8G8B8,
        E_PF_A8R8G8B8,
        E_PF_X8R8G8B8,
        E_PF_X8B8G8R8,
        E_PF_A8B8G8R8,
    };

    /** Parameter for creating render window. */
    struct RenderWindowCreateParam
    {
        /** The x-position of the window. Ignored if full_screen is true. */
        int32_t     _windowLeft;
        /** The y-position of the window. Ignored if full_screen is true. */
        int32_t     _windowTop;
        /** Width of the window in pixels. This value is equal to
         *  the width of the view port.*/
        int32_t     _windowWidth;
        /** Height of the window in pixels. This value is equal to
         *  the height of the view port.
         */
        int32_t     _windowHeight;
        /** Color depth of render window. Ignored if fullScreen is false
         *  since the desktop depth is used.
         */
        int32_t     _colorDepth;
        /** Title of the window. Ignored if full_screen is true. */
        String      _windowTitle;
        /** If true, the window fills the screen, with no title bar or border.
         *  Ignored if window_handle isn't 0.
         */
        bool        _fullscreen;
    };

//     typedef std::map<String, String>    NameValuePairList;
//     typedef NameValuePairList           RenderWindowCreateParamEx;

}


#endif  /*__T3D_COMMON_H__*/
