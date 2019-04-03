/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __T3D_PIXEL_FORMAT_H__
#define __T3D_PIXEL_FORMAT_H__


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
}


#endif  /*__T3D_PIXEL_FORMAT_H__*/
