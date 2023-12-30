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


#include "T3DD3D11Mapping.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    DXGI_FORMAT D3D11Mapping::get(PixelFormat src)
    {
        DXGI_FORMAT dst;
        switch (src)
        {
        case PixelFormat::E_PF_PALETTE8:
            
            break;
        case PixelFormat::E_PF_B5G6R5:
            dst = DXGI_FORMAT_B5G6R5_UNORM;
            break;
        case PixelFormat::E_PF_B5G5R5A1:
            dst = DXGI_FORMAT_B5G5R5A1_UNORM;
            break;
        case PixelFormat::E_PF_B4R4G4A4:
            dst = DXGI_FORMAT_B4G4R4A4_UNORM;
            break;
        case PixelFormat::E_PF_R8G8B8:
            dst = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case PixelFormat::E_PF_B8G8R8:
            dst = DXGI_FORMAT_B8G8R8X8_UNORM;
            break;
        case PixelFormat::E_PF_R8G8B8A8:
            dst = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case PixelFormat::E_PF_B8G8R8A8:
            dst = DXGI_FORMAT_B8G8R8A8_UNORM;
            break;
        case PixelFormat::E_PF_R8G8B8X8:
            dst = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case PixelFormat::E_PF_B8G8R8X8:
            dst = DXGI_FORMAT_B8G8R8X8_UNORM;
            break;
        }
        return dst;
    }

    //--------------------------------------------------------------------------

    D3D11_USAGE D3D11Mapping::get(Usage src)
    {
        D3D11_USAGE dst;

        switch (src)
        {
        case Usage::kStatic:
            dst = D3D11_USAGE_DEFAULT;
            break;
        case Usage::kImmutable:
            dst = D3D11_USAGE_IMMUTABLE;
            break;
        case Usage::kDynamic:
            dst = D3D11_USAGE_DYNAMIC;
            break;
        case Usage::kCopy:
            dst = D3D11_USAGE_STAGING;
            break;
        }
        
        return dst;
    }

    //--------------------------------------------------------------------------

    UINT D3D11Mapping::get(uint32_t accMode)
    {
        UINT dst = 0;

        switch (accMode)
        {
        case CPUAccessMode::kCPUNone:
            dst = 0;
            break;
        case CPUAccessMode::kCPUWrite:
            dst = D3D11_CPU_ACCESS_WRITE;
            break;
        case CPUAccessMode::kCPURead:
            dst = D3D11_CPU_ACCESS_READ;
            break;
        case CPUAccessMode::kCPUReadWrite:
            dst = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
            break;
        }
        
        return dst;
    }

    //--------------------------------------------------------------------------

    D3D11_TEXTURE1D_DESC D3D11Mapping::get(const PixelBuffer1DDesc &src)
    {
        D3D11_TEXTURE1D_DESC dst;
        memset(&dst, 0, sizeof(dst));
        
        dst.Width = src.width;          // 设置纹理宽度
        dst.MipLevels = src.mipmaps;    // 设置 Mipmap 层数
        dst.ArraySize = src.arraySize;  // 设置纹理数组大小
        dst.Format = get(src.format);   // 设置纹理格式
        
        return dst;
    }

    //--------------------------------------------------------------------------

    D3D11_TEXTURE2D_DESC D3D11Mapping::get(const PixelBuffer2DDesc &src)
    {
        D3D11_TEXTURE2D_DESC dst;
        memset(&dst, 0, sizeof(dst));
        
        dst.Width = src.width;          // 设置纹理宽度
        dst.Height = src.height;        // 设置纹理高度
        dst.MipLevels = src.mipmaps;    // 设置 Mipmap 层数
        dst.ArraySize = src.arraySize;  // 设置纹理数组大小
        dst.Format = get(src.format);   // 设置纹理格式
        dst.SampleDesc.Count = src.sampleDesc.Count;        // 设置多重采样数量
        dst.SampleDesc.Quality = src.sampleDesc.Quality;    // 设置多重采样质量
        
        return dst;
    }

    //--------------------------------------------------------------------------

    D3D11_TEXTURE3D_DESC D3D11Mapping::get(const PixelBuffer3DDesc &src)
    {
        D3D11_TEXTURE3D_DESC dst;
        memset(&dst, 0, sizeof(dst));
        return dst;
    }

    //--------------------------------------------------------------------------

    D3D_SRV_DIMENSION D3D11Mapping::get(TEXTURE_TYPE texType)
    {
        D3D_SRV_DIMENSION dim = D3D_SRV_DIMENSION_UNKNOWN;

        switch (texType)
        {
        case TEXTURE_TYPE::TT_1D:
            dim = D3D11_SRV_DIMENSION_TEXTURE1D;
            break;
        case TEXTURE_TYPE::TT_2D:
            dim = D3D11_SRV_DIMENSION_TEXTURE2D;
            break;
        case TEXTURE_TYPE::TT_2D_ARRAY:
            dim = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            break;
        case TEXTURE_TYPE::TT_3D:
            dim = D3D11_SRV_DIMENSION_TEXTURE3D;
            break;
        case TEXTURE_TYPE::TT_CUBE:
            dim = D3D11_SRV_DIMENSION_TEXTURECUBE;
            break;
        case TEXTURE_TYPE::TT_CUBE_ARRAY:
            dim = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
            break;
        }

        return dim;
    }

    //--------------------------------------------------------------------------

    TEXTURE_TYPE D3D11Mapping::get(D3D_SRV_DIMENSION srvDim)
    {
        TEXTURE_TYPE texType = TEXTURE_TYPE::TT_2D;

        switch (srvDim)
        {
        case D3D11_SRV_DIMENSION_TEXTURE1D:
            texType = TEXTURE_TYPE::TT_1D;
            break;
        case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2D:
            texType = TEXTURE_TYPE::TT_2D;
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
            texType = TEXTURE_TYPE::TT_2D_ARRAY;
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2DMS:
            break;
        case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
            break;
        case D3D11_SRV_DIMENSION_TEXTURE3D:
            texType = TEXTURE_TYPE::TT_3D;
            break;
        case D3D11_SRV_DIMENSION_TEXTURECUBE:
            texType = TEXTURE_TYPE::TT_CUBE;
            break;
        case D3D11_SRV_DIMENSION_TEXTURECUBEARRAY:
            texType = TEXTURE_TYPE::TT_CUBE_ARRAY;
            break;
        }

        return texType;
    }

    //--------------------------------------------------------------------------
}


