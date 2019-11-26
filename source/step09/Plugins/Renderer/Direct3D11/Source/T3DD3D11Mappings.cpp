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


#include "T3DD3D11Mappings.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char * const D3D11Mappings::POSITION = "POSITION";
    const char * const D3D11Mappings::BLENDWEIGHT = "BLENDWEIGHT";
    const char * const D3D11Mappings::BLENDINDICES = "BLENDINDICES";
    const char * const D3D11Mappings::NORMAL = "NORMAL";
    const char * const D3D11Mappings::COLOR = "COLOR";
    const char * const D3D11Mappings::TEXCOORD = "TEXCOORD";
    const char * const D3D11Mappings::TANGENT = "TANGENT";
    const char * const D3D11Mappings::BINORMAL = "BINORMAL";

    //--------------------------------------------------------------------------

    const char *D3D11Mappings::get(VertexAttribute::Semantic semantic)
    {
        switch (semantic)
        {
        case VertexAttribute::E_VAS_POSITION:
            return POSITION;
            break;
        case VertexAttribute::E_VAS_BLENDWEIGHT:
            return BLENDWEIGHT;
            break;
        case VertexAttribute::E_VAS_BLENDINDICES:
            return BLENDINDICES;
            break;
        case VertexAttribute::E_VAS_NORMAL:
            return NORMAL;
            break;
        case VertexAttribute::E_VAS_DIFFUSE:
        case VertexAttribute::E_VAS_SPECULAR:
            return COLOR;
            break;
        case VertexAttribute::E_VAS_TEXCOORD:
            return TEXCOORD;
            break;
        case VertexAttribute::E_VAS_TANGENT:
            return TANGENT;
            break;
        case VertexAttribute::E_VAS_BINORMAL:
            return BINORMAL;
            break;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    DXGI_FORMAT D3D11Mappings::get(VertexAttribute::Type type)
    {
        DXGI_FORMAT d3dformat;

        switch (type)
        {
        case VertexAttribute::E_VAT_COLOR:
            d3dformat = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
        case VertexAttribute::E_VAT_FLOAT1:
            d3dformat = DXGI_FORMAT_R32_FLOAT;
            break;
        case VertexAttribute::E_VAT_FLOAT2:
            d3dformat = DXGI_FORMAT_R32G32_FLOAT;
            break;
        case VertexAttribute::E_VAT_FLOAT3:
            d3dformat = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::E_VAT_FLOAT4:
            d3dformat = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
        case VertexAttribute::E_VAT_SHORT2:
            d3dformat = DXGI_FORMAT_R16G16_SINT;
            break;
        case VertexAttribute::E_VAT_SHORT4:
            d3dformat = DXGI_FORMAT_R16G16B16A16_SINT;
            break;
        case VertexAttribute::E_VAT_UBYTE4:
            d3dformat = DXGI_FORMAT_R8G8B8A8_UINT;
            break;
        }

        return d3dformat;
    }

    //--------------------------------------------------------------------------

    D3D11_USAGE D3D11Mappings::get(HardwareBuffer::Usage usage)
    {
        D3D11_USAGE d3dusage;

        switch (usage)
        {
        case HardwareBuffer::Usage::E_HBU_STATIC:
            d3dusage = D3D11_USAGE_IMMUTABLE;
            break;
        case HardwareBuffer::Usage::E_HBU_DYNAMIC:
            d3dusage = D3D11_USAGE_DYNAMIC;
            break;
        case HardwareBuffer::Usage::E_HBU_WRITE_ONLY:
            break;
        case HardwareBuffer::Usage::E_HBU_STATIC_WRITE_ONLY:
            break;
        case HardwareBuffer::Usage::E_HBU_DYNAMIC_WRITE_ONLY:
            break;
        }

        return d3dusage;
    }
}

