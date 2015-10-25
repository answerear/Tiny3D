

#ifndef __T3D_COMMON_H__
#define __T3D_COMMON_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
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

    typedef std::map<String, String>    NameValuePairList;
    typedef NameValuePairList           RenderWindowCreateParamEx;

    typedef std::list<Renderer*>            RendererList;
    typedef RendererList::iterator          RendererListItr;
    typedef RendererList::const_iterator    RendererListConstItr;
}


#endif  /*__T3D_COMMON_H__*/
