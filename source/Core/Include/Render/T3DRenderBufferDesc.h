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


#ifndef __T3D_RENDER_BUFFER_DESC_H__
#define __T3D_RENDER_BUFFER_DESC_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DConstant.h"
#include "Render/T3DRenderConstant.h"
#include "T3DMultisamplerDesc.h"


namespace Tiny3D
{
    TSTRUCT()
    struct T3D_ENGINE_API VertexBufferDesc
    {
        TPROPERTY()
        uint32_t  verteSize {0};
        TPROPERTY()
        uint32_t  vertexCount {0};
        TPROPERTY()
        Buffer  buffer {};
    };

    TSTRUCT()
    struct T3D_ENGINE_API IndexBufferDesc
    {
        TPROPERTY()
        uint32_t  indexSize {0};
        TPROPERTY()
        uint32_t  indexCount {0};
        TPROPERTY()
        Buffer  buffer {};
    };


    TSTRUCT()
    struct T3D_ENGINE_API PixelBuffer1DDesc
    {
        TPROPERTY()
        uint32_t    width {0};
        TPROPERTY()
        uint32_t    mipmaps {0};
        TPROPERTY()
        uint32_t    arraySize {0};
        TPROPERTY()
        PixelFormat format {PixelFormat::E_PF_UNKNOWN};
        TPROPERTY()
        Buffer  buffer {};
    };

    TSTRUCT()
    struct T3D_ENGINE_API PixelBuffer2DDesc
    {
        TPROPERTY()
        uint32_t    width {0};
        TPROPERTY()
        uint32_t    height {0};
        TPROPERTY()
        uint32_t    mipmaps {0};
        TPROPERTY()
        uint32_t    arraySize {0};
        TPROPERTY()
        PixelFormat format {PixelFormat::E_PF_UNKNOWN};
        TPROPERTY()
        MultisamplerDesc    sampleDesc {};
        TPROPERTY()
        Buffer  buffer {};
    };

    TSTRUCT()
    struct T3D_ENGINE_API PixelBuffer3DDesc
    {
        uint32_t    width;
        TPROPERTY()
        uint32_t    height;
        TPROPERTY()
        uint32_t    depth;
        TPROPERTY()
        uint32_t    mipmaps;
        TPROPERTY()
        PixelFormat format {PixelFormat::E_PF_UNKNOWN};
        TPROPERTY()
        Buffer  buffer {};
    };
}


#endif  /*__T3D_RENDER_BUFFER_DESC_H__*/
