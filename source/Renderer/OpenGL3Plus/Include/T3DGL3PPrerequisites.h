/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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
 **************************************************************************************************/

#ifndef __T3D_GL3P_PREREQUISITES_H__
#define __T3D_GL3P_PREREQUISITES_H__


#include <T3DPlatform.h>
#include <T3DLog.h>
#include <Tiny3D.h>
#include <glfw3.h>


#if defined T3DGL3PRENDERER_EXPORT
#define T3D_GL3PRENDERER_API        T3D_EXPORT_API
#else
#define T3D_GL3PRENDERER_API        T3D_IMPORT_API
#endif


namespace Tiny3D
{
    class GL3PRenderer;
    class GL3PRenderWindow;
    class GL3PHardwareVertexBuffer;
    class GL3PHardwareIndexBuffer;
    class GL3PHardwarePixelBuffer;
    class GL3PHardwareBufferManager;
}


#endif  /*__T3D_GL3P_PREREQUISITES_H__*/