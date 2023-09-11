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

#include "RHI/T3DRHIRenderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char * const RHIRenderer::NULLRENDERER = "NullRenderer";
    const char * const RHIRenderer::REFERENCE3D = "Reference3D";
    const char * const RHIRenderer::DIRECT3D9 = "Direct3D9";
    const char * const RHIRenderer::DIRECT3D11 = "Direct3D11";
    const char * const RHIRenderer::OPENGL3 = "OpenGL 3.x";
    const char * const RHIRenderer::OPENGLES2 = "OpenGL ES 2";
    const char * const RHIRenderer::OPENGLES3 = "OpenGL ES 3";
    const char * const RHIRenderer::VULKAN = "Vulkan";
    const char * const RHIRenderer::METAL = "Metal";

    //--------------------------------------------------------------------------
}
