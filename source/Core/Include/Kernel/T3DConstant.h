/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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

#ifndef __T3D_CONSTANT_H__
#define __T3D_CONSTANT_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /**
     * @enum    PixelFormat
     * @brief   Values that represent pixel formats
     */
    TENUM()
    enum class PixelFormat : uint32_t
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

    TENUM()
    enum class TEXTURE_TYPE : uint32_t
    {
        TT_1D = 0,
        TT_2D,
        TT_2D_ARRAY,
        TT_3D,
        TT_CUBE,
        TT_CUBE_ARRAY,
        TT_RENDER_TEXTURE,
        TT_MAX
    };

    #define SHADER_TAG_QUEUE        "Queue"
}


#endif    /*__T3D_CONFIG_H__*/
