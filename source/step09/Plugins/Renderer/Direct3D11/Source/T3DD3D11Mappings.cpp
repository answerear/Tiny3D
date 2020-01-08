﻿/*******************************************************************************
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


#include "T3DD3D11Mappings.h"
#include "T3DD3D11Renderer.h"


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

    D3D11_FILL_MODE D3D11Mappings::get(PolygonMode mode)
    {
        D3D11_FILL_MODE d3dMode = D3D11_FILL_SOLID;

        switch (mode)
        {
        case PolygonMode::POINT:
        case PolygonMode::WIREFRAME:
            d3dMode = D3D11_FILL_WIREFRAME;
            break;
        default:
            d3dMode = D3D11_FILL_SOLID;
            break;
        }

        return d3dMode;
    }

    //--------------------------------------------------------------------------

    D3D11_CULL_MODE D3D11Mappings::get(CullingMode mode)
    {
        D3D11_CULL_MODE d3dMode = D3D11_CULL_NONE;

        switch (mode)
        {
        case CullingMode::CLOCKWISE:
            d3dMode = D3D11_CULL_BACK;
            break;
        case CullingMode::ANTICLOCKWISE:
            d3dMode = D3D11_CULL_FRONT;
            break;
        default:
            d3dMode = D3D11_CULL_NONE;
            break;
        }

        return d3dMode;
    }

    //--------------------------------------------------------------------------

    const char *D3D11Mappings::get(VertexAttribute::Semantic semantic)
    {
        switch (semantic)
        {
        case VertexAttribute::Semantic::E_VAS_POSITION:
            return POSITION;
            break;
        case VertexAttribute::Semantic::E_VAS_BLENDWEIGHT:
            return BLENDWEIGHT;
            break;
        case VertexAttribute::Semantic::E_VAS_BLENDINDICES:
            return BLENDINDICES;
            break;
        case VertexAttribute::Semantic::E_VAS_NORMAL:
            return NORMAL;
            break;
        case VertexAttribute::Semantic::E_VAS_DIFFUSE:
        case VertexAttribute::Semantic::E_VAS_SPECULAR:
            return COLOR;
            break;
        case VertexAttribute::Semantic::E_VAS_TEXCOORD:
            return TEXCOORD;
            break;
        case VertexAttribute::Semantic::E_VAS_TANGENT:
            return TANGENT;
            break;
        case VertexAttribute::Semantic::E_VAS_BINORMAL:
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
        case VertexAttribute::Type::E_VAT_COLOR:
            d3dformat = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
        case VertexAttribute::Type::E_VAT_FLOAT1:
            d3dformat = DXGI_FORMAT_R32_FLOAT;
            break;
        case VertexAttribute::Type::E_VAT_FLOAT2:
            d3dformat = DXGI_FORMAT_R32G32_FLOAT;
            break;
        case VertexAttribute::Type::E_VAT_FLOAT3:
            d3dformat = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case VertexAttribute::Type::E_VAT_FLOAT4:
            d3dformat = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
        case VertexAttribute::Type::E_VAT_SHORT2:
            d3dformat = DXGI_FORMAT_R16G16_SINT;
            break;
        case VertexAttribute::Type::E_VAT_SHORT4:
            d3dformat = DXGI_FORMAT_R16G16B16A16_SINT;
            break;
        case VertexAttribute::Type::E_VAT_UBYTE4:
            d3dformat = DXGI_FORMAT_R8G8B8A8_UINT;
            break;
        }

        return d3dformat;
    }

    //--------------------------------------------------------------------------

    D3D11_PRIMITIVE_TOPOLOGY D3D11Mappings::get(Renderer::PrimitiveType primitive)
    {
        D3D11_PRIMITIVE_TOPOLOGY d3dPrimitive;

        switch (primitive)
        {
        case Renderer::PrimitiveType::E_PT_POINT_LIST:
            d3dPrimitive = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
            break;
        case Renderer::PrimitiveType::E_PT_LINE_LIST:
            d3dPrimitive = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
            break;
        case Renderer::PrimitiveType::E_PT_LINE_STRIP:
            d3dPrimitive = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
            break;
        case Renderer::PrimitiveType::E_PT_TRIANGLE_LIST:
            d3dPrimitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            break;
        case Renderer::PrimitiveType::E_PT_TRIANGLE_STRIP:
            d3dPrimitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            break;
        default:
            d3dPrimitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            break;
        }

        return d3dPrimitive;
    }

    //--------------------------------------------------------------------------

    DXGI_FORMAT D3D11Mappings::get(HardwareIndexBuffer::Type idxType)
    {
        DXGI_FORMAT d3dFormat;

        switch (idxType)
        {
        case HardwareIndexBuffer::Type::E_IT_16BITS:
            d3dFormat = DXGI_FORMAT_R16_UINT;
            break;
        case HardwareIndexBuffer::Type::E_IT_32BITS:
            d3dFormat = DXGI_FORMAT_R32_UINT;
            break;
        default:
            d3dFormat = DXGI_FORMAT_R16_UINT;
            break;
        }

        return d3dFormat;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Mappings::get(HardwareBuffer::Usage usage, uint32_t mode, 
        D3D11_USAGE &d3dUsage, uint32_t &d3dAccessFlag)
    {
        TResult ret = T3D_OK;

        do 
        {
            switch (usage)
            {
            case HardwareBuffer::Usage::STATIC:
                {
                    if (mode == HardwareBuffer::AccessMode::CPU_NONE)
                    {
                        // 静态缓冲，CPU不可读写，只能初始化时候设置数据
                        d3dUsage = D3D11_USAGE_IMMUTABLE;
                        d3dAccessFlag = 0;
                    }
                    else if (mode == HardwareBuffer::AccessMode::CPU_WRITE)
                    {
                        d3dUsage = D3D11_USAGE_DEFAULT;
                        d3dAccessFlag = 0;
                    }
                    else
                    {
                        // 其他 CPU 访问标签在这里都是非法
                        ret = T3D_ERR_INVALID_PARAM;
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
                            "Usage is STATIC, so access mode must be CPU_NONE "
                            "or CPU_WRITE !");
                    }
                }
                break;
            case HardwareBuffer::Usage::DYNAMIC:
            case HardwareBuffer::Usage::STREAM:
                {
                    if (mode == HardwareBuffer::AccessMode::CPU_NONE)
                    {
                        // CPU不读也不写，这里建议使用STATIC性能更好
                        d3dUsage = D3D11_USAGE_DEFAULT;
                        d3dAccessFlag = 0;
                        T3D_LOG_WARNING(LOG_TAG_D3D11RENDERER,
                            "Usage is DYNAMIC, but CPU access mode is CPU_NONE."
                            " Here suggests STATIC instead of DYNAMIC !");
                    }
                    else if ((mode == (HardwareBuffer::AccessMode::CPU_READ
                        | HardwareBuffer::AccessMode::CPU_WRITE)))
                    {
                        // CPU读写，GPU读写
                        d3dUsage = D3D11_USAGE_STAGING;
                        d3dAccessFlag
                            = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
                    }
                    else if (mode == HardwareBuffer::AccessMode::CPU_READ)
                    {
                        // CPU读，GPU读写
                        d3dUsage = D3D11_USAGE_STAGING;
                        d3dAccessFlag = D3D11_CPU_ACCESS_READ;
                    }
                    else if (mode == HardwareBuffer::AccessMode::CPU_WRITE)
                    {
                        // CPU写，GPU读
                        d3dUsage = D3D11_USAGE_DYNAMIC;
                        d3dAccessFlag = D3D11_CPU_ACCESS_WRITE;
                    }
                    else if (mode == HardwareBuffer::AccessMode::GPU_COPY)
                    {
                        // CPU读写，GPU读写
                        d3dUsage = D3D11_USAGE_STAGING;
                        d3dAccessFlag 
                            = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
                    }
                    else
                    {
                        // 无效 CPU 访问方式参数
                        ret = T3D_ERR_INVALID_PARAM;
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
                            "Invalid CPU access mode parameter !");
                    }
                }
                break;
            default:
                {
                    ret = T3D_ERR_INVALID_PARAM;
                }
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    D3D11_MAP D3D11Mappings::get(HardwareBuffer::LockOptions options)
    {
        D3D11_MAP d3dOpt = D3D11_MAP_READ;

        switch (options)
        {
        case HardwareBuffer::LockOptions::READ:
            {
                d3dOpt = D3D11_MAP_READ;
            }
            break;
        case HardwareBuffer::LockOptions::WRITE:
            {
                d3dOpt = D3D11_MAP_WRITE;
            }
            break;
        case HardwareBuffer::LockOptions::READ_WRITE:
            {
                d3dOpt = D3D11_MAP_READ_WRITE;
            }
            break;
        case HardwareBuffer::LockOptions::WRITE_DISCARD:
            {
                d3dOpt = D3D11_MAP_WRITE_DISCARD;
            }
            break;
        case HardwareBuffer::LockOptions::WRITE_NO_OVERWRITE:
            {
                d3dOpt = D3D11_MAP_WRITE_NO_OVERWRITE;
            }
            break;
        default:
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
                    "Invalid LockOptions !");
            }
            break;
        }

        return d3dOpt;
    }

    //--------------------------------------------------------------------------

    DXGI_FORMAT D3D11Mappings::get(PixelFormat format)
    {
        DXGI_FORMAT fmt;

        switch (format)
        {
        case PixelFormat::E_PF_PALETTE8:
            
            break;
        case PixelFormat::E_PF_R5G6B5:

            break;
        case PixelFormat::E_PF_A1R5G5B5:
            break;
        case PixelFormat::E_PF_A4R4G4B4:
            break;
        case PixelFormat::E_PF_R8G8B8:
            
            break;
        case PixelFormat::E_PF_B8G8R8:
            break;
        case PixelFormat::E_PF_A8R8G8B8:
            fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case PixelFormat::E_PF_B8G8R8A8:
            break;
        case PixelFormat::E_PF_X8R8G8B8:
            break;
        case PixelFormat::E_PF_B8G8R8X8:
            break;
        }

        return fmt;
    }

    //--------------------------------------------------------------------------

    D3D11_FILTER D3D11Mappings::get(FilterOptions min, FilterOptions mag,
        FilterOptions mip, bool comparison /* = false */)
    {
        if (min == FilterOptions::ANISOTROPIC 
            || mag == FilterOptions::ANISOTROPIC 
            || mip == FilterOptions::ANISOTROPIC)
            return comparison ? D3D11_FILTER_COMPARISON_ANISOTROPIC : D3D11_FILTER_ANISOTROPIC;

        // FilterOptions::FilterOptions::NONE is not supported
#define MERGE_FOR_SWITCH(_comparison_, _min_ , _mag_, _mip_ ) \
    ((_comparison_ ? 8 : 0) | (_min_ == FilterOptions::LINEAR ? 4 : 0) | (_mag_ == FilterOptions::LINEAR ? 2 : 0) | (_mip_ == FilterOptions::LINEAR ? 1 : 0))

        switch ((MERGE_FOR_SWITCH(comparison, min, mag, mip)))
        {
        case MERGE_FOR_SWITCH(true, FilterOptions::POINT, FilterOptions::POINT, FilterOptions::POINT):
            return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
        case MERGE_FOR_SWITCH(true, FilterOptions::POINT, FilterOptions::POINT, FilterOptions::LINEAR):
            return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
        case MERGE_FOR_SWITCH(true, FilterOptions::POINT, FilterOptions::LINEAR, FilterOptions::POINT):
            return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
        case MERGE_FOR_SWITCH(true, FilterOptions::POINT, FilterOptions::LINEAR, FilterOptions::LINEAR):
            return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
        case MERGE_FOR_SWITCH(true, FilterOptions::LINEAR, FilterOptions::POINT, FilterOptions::POINT):
            return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
        case MERGE_FOR_SWITCH(true, FilterOptions::LINEAR, FilterOptions::POINT, FilterOptions::LINEAR):
            return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        case MERGE_FOR_SWITCH(true, FilterOptions::LINEAR, FilterOptions::LINEAR, FilterOptions::POINT):
            return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        case MERGE_FOR_SWITCH(true, FilterOptions::LINEAR, FilterOptions::LINEAR, FilterOptions::LINEAR):
            return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        case MERGE_FOR_SWITCH(false, FilterOptions::POINT, FilterOptions::POINT, FilterOptions::POINT):
            return D3D11_FILTER_MIN_MAG_MIP_POINT;
        case MERGE_FOR_SWITCH(false, FilterOptions::POINT, FilterOptions::POINT, FilterOptions::LINEAR):
            return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        case MERGE_FOR_SWITCH(false, FilterOptions::POINT, FilterOptions::LINEAR, FilterOptions::POINT):
            return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        case MERGE_FOR_SWITCH(false, FilterOptions::POINT, FilterOptions::LINEAR, FilterOptions::LINEAR):
            return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
        case MERGE_FOR_SWITCH(false, FilterOptions::LINEAR, FilterOptions::POINT, FilterOptions::POINT):
            return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        case MERGE_FOR_SWITCH(false, FilterOptions::LINEAR, FilterOptions::POINT, FilterOptions::LINEAR):
            return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        case MERGE_FOR_SWITCH(false, FilterOptions::LINEAR, FilterOptions::LINEAR, FilterOptions::POINT):
            return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        case MERGE_FOR_SWITCH(false, FilterOptions::LINEAR, FilterOptions::LINEAR, FilterOptions::LINEAR):
            return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        }

#undef MERGE_FOR_SWITCH

        return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    }

    //--------------------------------------------------------------------------

    D3D11_TEXTURE_ADDRESS_MODE D3D11Mappings::get(TextureAddressMode mode)
    {
        if (D3D11_RENDERER.getFeatureLevel() == D3D_FEATURE_LEVEL_9_1)
            return D3D11_TEXTURE_ADDRESS_WRAP;

        switch (mode)
        {
        case TextureAddressMode::WRAP:
            return D3D11_TEXTURE_ADDRESS_WRAP;
        case TextureAddressMode::MIRROR:
            return D3D11_TEXTURE_ADDRESS_MIRROR;
        case TextureAddressMode::CLAMP:
            return D3D11_TEXTURE_ADDRESS_CLAMP;
        case TextureAddressMode::BORDER:
            return D3D11_TEXTURE_ADDRESS_BORDER;
        }

        return D3D11_TEXTURE_ADDRESS_WRAP;
    }

    //--------------------------------------------------------------------------

    D3D11_COMPARISON_FUNC D3D11Mappings::get(CompareFunction func)
    {
        switch (func)
        {
        case CompareFunction::ALWAYS_FAIL:
            return D3D11_COMPARISON_NEVER;
        case CompareFunction::ALWAYS_PASS:
            return D3D11_COMPARISON_ALWAYS;
        case CompareFunction::LESS:
            return D3D11_COMPARISON_LESS;
        case CompareFunction::LESS_EQUAL:
            return D3D11_COMPARISON_LESS_EQUAL;
        case CompareFunction::EQUAL:
            return D3D11_COMPARISON_EQUAL;
        case CompareFunction::NOT_EQUAL:
            return D3D11_COMPARISON_NOT_EQUAL;
        case CompareFunction::GREATER_EQUAL:
            return D3D11_COMPARISON_GREATER_EQUAL;
        case CompareFunction::GREATER:
            return D3D11_COMPARISON_GREATER;
        };

        return D3D11_COMPARISON_ALWAYS;
    }
}

