/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef __T3D_COMMON_H__
#define __T3D_COMMON_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    enum PixelFormat
    {
        E_PF_UNKNOWN = 0,
        E_PF_PALETTE8,
        E_PF_R5G6B5,
        E_PF_A1R5G5B5,
        E_PF_A4R4G4B4,
        E_PF_R8G8B8,
        E_PF_B8G8R8,
        E_PF_A8R8G8B8,
        E_PF_B8G8R8A8,
        E_PF_X8R8G8B8,
        E_PF_B8G8R8X8,
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
