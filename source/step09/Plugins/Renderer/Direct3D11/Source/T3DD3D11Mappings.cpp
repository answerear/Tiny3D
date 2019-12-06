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
                        d3dAccessFlag = D3D11_CPU_ACCESS_WRITE;
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
}

